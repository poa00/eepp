#include "gitplugin.hpp"
#include <eepp/graphics/primitives.hpp>
#include <eepp/system/filesystem.hpp>
#include <eepp/system/scopedop.hpp>
#include <eepp/ui/doc/syntaxdefinitionmanager.hpp>
#include <eepp/ui/uidropdownlist.hpp>
#include <eepp/ui/uiiconthememanager.hpp>
#include <eepp/ui/uiloader.hpp>
#include <eepp/ui/uipopupmenu.hpp>
#include <eepp/ui/uistackwidget.hpp>
#include <eepp/ui/uistyle.hpp>
#include <eepp/ui/uitextedit.hpp>
#include <eepp/ui/uitooltip.hpp>
#include <eepp/ui/uitreeview.hpp>
#include <nlohmann/json.hpp>

using namespace EE::UI;
using namespace EE::UI::Doc;

using json = nlohmann::json;
#if EE_PLATFORM != EE_PLATFORM_EMSCRIPTEN || defined( __EMSCRIPTEN_PTHREADS__ )
#define GIT_THREADED 1
#else
#define GIT_THREADED 0
#endif

namespace ecode {

static const char* GIT_EMPTY = "";
static const char* GIT_SUCCESS = "success";
static const char* GIT_ERROR = "error";
static const char* GIT_BOLD = "bold";
static const char* GIT_NOT_BOLD = "notbold";
static const char* GIT_TAG = "tag";
static const char* GIT_REPO = "repo";

std::string GitPlugin::statusTypeToString( Git::GitStatusType type ) {
	switch ( type ) {
		case Git::GitStatusType::Untracked:
			return i18n( "git_untracked", "Untracked" );
		case Git::GitStatusType::Unmerged:
			return i18n( "git_unmerged", "Unmergedd" );
		case Git::GitStatusType::Changed:
			return i18n( "git_changed", "Changed" );
		case Git::GitStatusType::Staged:
			return i18n( "git_staged", "Staged" );
		case Git::GitStatusType::Ignored:
			return i18n( "git_ignored", "Ignored" );
	}
	return "";
}

static size_t hashBranches( const std::vector<Git::Branch>& branches ) {
	size_t hash = 0;
	for ( const auto& branch : branches )
		hash = hashCombine( hash, String::hash( branch.name ), String::hash( branch.remote ),
							String::hash( branch.lastCommit ), branch.type, branch.ahead,
							branch.behind );
	return hash;
}

class GitBranchModel : public Model {
  public:
	static std::shared_ptr<GitBranchModel> asModel( std::vector<Git::Branch>&& branches,
													size_t hash, GitPlugin* gitPlugin ) {
		return std::make_shared<GitBranchModel>( std::move( branches ), hash, gitPlugin );
	}

	enum Column { Name, Remote, Type, LastCommit };

	struct BranchData {
		std::string branch;
		std::vector<Git::Branch> data;
	};

	std::string refTypeToString( Git::RefType type ) {
		switch ( type ) {
			case Git::RefType::Head:
				return mPlugin->i18n( "git_local_branches", "Local Branches" ).toUtf8();
			case Git::RefType::Remote:
				return mPlugin->i18n( "git_remote_branches", "Remote Branches" ).toUtf8();
			case Git::RefType::Tag:
				return mPlugin->i18n( "git_tags", "Tags" ).toUtf8();
			default:
				break;
		}
		return "";
	}

	GitBranchModel( std::vector<Git::Branch>&& branches, size_t hash, GitPlugin* gitPlugin ) :
		mPlugin( gitPlugin ), mHash( hash ) {
		std::map<std::string, std::vector<Git::Branch>> branchTypes;
		for ( auto& branch : branches ) {
			auto& type = branchTypes[refTypeToString( branch.type )];
			type.emplace_back( std::move( branch ) );
		}
		for ( auto& branch : branchTypes ) {
			mBranches.emplace_back(
				BranchData{ std::move( branch.first ), std::move( branch.second ) } );
		}
	}

	size_t treeColumn() const { return Column::Name; }

	size_t rowCount( const ModelIndex& index ) const {
		if ( !index.isValid() )
			return mBranches.size();
		if ( index.internalId() == -1 )
			return mBranches[index.row()].data.size();
		return 0;
	}

	size_t columnCount( const ModelIndex& ) const { return 4; }

	ModelIndex parentIndex( const ModelIndex& index ) const {
		if ( !index.isValid() || index.internalId() == -1 )
			return {};
		return createIndex( index.internalId(), index.column(), &mBranches[index.internalId()],
							-1 );
	}

	ModelIndex index( int row, int column, const ModelIndex& parent ) const {
		if ( row < 0 || column < 0 )
			return {};
		if ( !parent.isValid() )
			return createIndex( row, column, &mBranches[row], -1 );
		if ( parent.internalData() )
			return createIndex( row, column, &mBranches[parent.row()].data[row], parent.row() );
		return {};
	}

	UIIcon* iconFor( const ModelIndex& index ) const {
		if ( index.column() == (Int64)treeColumn() ) {
			if ( index.hasParent() ) {
				Git::Branch* branch = static_cast<Git::Branch*>( index.internalData() );
				return mPlugin->findIcon( branch->type == Git::RefType::Tag ? GIT_TAG : GIT_REPO );
			}
		}
		return nullptr;
	}

	Variant data( const ModelIndex& index, ModelRole role ) const {
		switch ( role ) {
			case ModelRole::Display: {
				if ( index.internalId() == -1 ) {
					if ( index.column() == Column::Name )
						return Variant( String::format( "%s (%zu)", mBranches[index.row()].branch,
														mBranches[index.row()].data.size() ) );
					return Variant( GIT_EMPTY );
				}
				const Git::Branch& branch = mBranches[index.internalId()].data[index.row()];
				switch ( index.column() ) {
					case Column::Name: {
						if ( branch.type == Git::Remote &&
							 String::startsWith( branch.name, "origin/" ) ) {
							return Variant( std::string_view{ branch.name }.substr( 7 ).data() );
						} else if ( branch.type == Git::Head &&
									( branch.ahead || branch.behind ) ) {
							if ( branch.ahead && branch.behind ) {
								return Variant( String::format( "%s (+%ld/-%ld)", branch.name,
																branch.ahead, branch.behind ) );
							} else if ( branch.ahead ) {
								return Variant(
									String::format( "%s (+%ld)", branch.name, branch.ahead ) );
							} else {
								return Variant(
									String::format( "%s (-%ld)", branch.name, branch.behind ) );
							}
						}
						return Variant( branch.name.c_str() );
					}
					case Column::Remote:
						return Variant( branch.remote.c_str() );
					case Column::Type:
						return Variant( branch.typeStr() );
					case Column::LastCommit:
						return Variant( branch.lastCommit.c_str() );
				}
				return Variant( GIT_EMPTY );
			}
			case ModelRole::Class: {
				if ( index.internalId() == -1 )
					return Variant( GIT_BOLD );
				const Git::Branch& branch = mBranches[index.internalId()].data[index.row()];
				if ( branch.name == mPlugin->gitBranch() )
					return Variant( GIT_BOLD );
				return Variant( GIT_NOT_BOLD );
			}
			case ModelRole::Icon: {
				return iconFor( index );
			}
			default:
				break;
		}
		return {};
	}

	virtual bool classModelRoleEnabled() { return true; }

	size_t getHash() const { return mHash; }

  protected:
	std::vector<BranchData> mBranches;
	GitPlugin* mPlugin{ nullptr };
	size_t mHash{ 0 };
};

class GitStatusModel : public Model {
  public:
	static std::shared_ptr<GitStatusModel> asModel( Git::FilesStatus status,
													GitPlugin* gitPlugin ) {
		return std::make_shared<GitStatusModel>( std::move( status ), gitPlugin );
	}

	struct RepoStatusType;
	struct RepoStatus;

	struct DiffFile : Git::DiffFile {
		DiffFile( Git::DiffFile&& df, RepoStatusType* parent ) :
			Git::DiffFile( df ), parent( parent ){};
		RepoStatusType* parent;
	};

	struct RepoStatusType {
		std::string typeStr;
		Git::GitStatusType type;
		std::vector<DiffFile> files;
		RepoStatus* parent{ nullptr };
	};

	struct RepoStatus {
		std::string repo;
		std::vector<RepoStatusType> type;
	};

	enum Column { File, State, Inserted, Removed, RelativeDirectory };

	GitStatusModel( Git::FilesStatus&& status, GitPlugin* gitPlugin ) : mPlugin( gitPlugin ) {
		std::map<std::string, std::set<Git::GitStatusType>> typesFound;
		std::unordered_map<std::string, size_t> repoPos;
		std::unordered_map<size_t, std::unordered_map<Git::GitStatusType, size_t>> repoTypePos;

		for ( auto& s : status )
			for ( auto& f : s.second )
				typesFound[s.first].insert( f.statusType );

		for ( const auto& tf : typesFound ) {
			RepoStatus rs;
			rs.repo = tf.first;
			size_t pos = mStatus.size();
			repoPos[rs.repo] = pos;
			for ( const auto& s : tf.second ) {
				RepoStatusType rt;
				rt.typeStr = mPlugin->statusTypeToString( s );
				rt.type = s;
				repoTypePos[pos][s] = rs.type.size();
				rs.type.emplace_back( std::move( rt ) );
			}
			mStatus.emplace_back( std::move( rs ) );
		}

		for ( auto& s : status ) {
			for ( auto& fv : s.second ) {
				auto pos = repoPos[s.first];
				auto typePos = repoTypePos[pos][fv.statusType];
				DiffFile df( std::move( fv ), &mStatus[pos].type[typePos] );
				mStatus[pos].type[typePos].files.emplace_back( std::move( df ) );
			}
		}

		// Set the parents after the addreses are stable
		for ( auto& status : mStatus ) {
			for ( auto& type : status.type ) {
				for ( auto& f : type.files )
					f.parent = &type;
				type.parent = &status;
			}
		}
	}

	size_t treeColumn() const { return Column::File; }

	size_t rowCount( const ModelIndex& index ) const {
		if ( !index.isValid() )
			return mStatus.size();

		if ( index.internalId() == Repo )
			return mStatus[index.row()].type.size();

		if ( index.internalId() == Status )
			return mStatus[index.parent().row()].type[index.row()].files.size();

		return 0;
	}

	size_t columnCount( const ModelIndex& ) const { return 5; }

	ModelIndex parentIndex( const ModelIndex& index ) const {
		if ( !index.isValid() || index.internalId() == Repo )
			return {};

		if ( index.internalId() == Status ) {
			RepoStatusType* status = reinterpret_cast<RepoStatusType*>( index.internalData() );
			size_t f = 0;
			size_t statusSize = mStatus.size();
			for ( size_t i = 0; i < statusSize; i++ ) {
				if ( &mStatus[i] == status->parent ) {
					f = i;
					break;
				}
			}
			return createIndex( f, index.column(), status->parent, Repo );
		}

		if ( index.internalId() == GitFile ) {
			DiffFile* file = reinterpret_cast<DiffFile*>( index.internalData() );
			RepoStatusType* status = file->parent;
			RepoStatus* repoStatus = status->parent;
			size_t typeSize = repoStatus->type.size();
			size_t f = 0;
			for ( size_t i = 0; i < typeSize; i++ ) {
				if ( &status->parent->type[i] == status ) {
					f = i;
					break;
				}
			}
			return createIndex( f, index.column(), status, Status );
		}

		return {};
	}

	enum ModelCategory { Repo, Status, GitFile };

	ModelIndex index( int row, int column, const ModelIndex& parent ) const {
		if ( row < 0 || column < 0 )
			return {};

		if ( !parent.isValid() )
			return createIndex( row, column, &mStatus[row], Repo );

		if ( parent.internalId() == Repo )
			return createIndex( row, column, &mStatus[parent.row()].type[row], Status );

		if ( parent.internalId() == Status ) {
			size_t pprow = parent.parent().row();
			size_t prow = parent.row();
			return createIndex( row, column, &mStatus[pprow].type[prow].files[row], GitFile );
		}

		return {};
	}

	Variant data( const ModelIndex& index, ModelRole role ) const {
		switch ( role ) {
			case ModelRole::Display: {
				if ( index.internalId() == Repo ) {
					if ( index.column() == Column::File )
						return Variant( mStatus[index.row()].repo.c_str() );
					return Variant( GIT_EMPTY );
				} else if ( index.internalId() == Status ) {
					if ( index.column() == Column::File ) {
						return Variant(
							mStatus[index.parent().row()].type[index.row()].typeStr.c_str() );
					}
					return Variant( GIT_EMPTY );
				}
				const Git::DiffFile& s = mStatus[index.parent().parent().row()]
											 .type[index.parent().row()]
											 .files[index.row()];
				switch ( index.column() ) {
					case Column::File:
						return Variant( FileSystem::fileNameFromPath( s.file ) );
					case Column::Inserted:
						return Variant( String::format( "+%d ", s.inserts ) );
					case Column::Removed:
						return Variant( String::format( "-%d ", s.deletes ) );
					case Column::State:
						return Variant( String::format( "%c", s.statusChar ) );
					case Column::RelativeDirectory:
						return Variant( FileSystem::fileRemoveFileName( s.file ) );
				}
				break;
			}
			case ModelRole::Class: {
				if ( index.internalId() == GitFile ) {
					switch ( index.column() ) {
						case Column::Inserted:
							return Variant( GIT_SUCCESS );
						case Column::Removed:
							return Variant( GIT_ERROR );
						default:
							break;
					}
				}
				break;
			}
			case ModelRole::Icon: {
				if ( (Int64)treeColumn() == index.column() ) {
					if ( index.internalId() == Repo ) {
						return Variant( mPlugin->findIcon( "repo" ) );
					} else if ( index.internalId() == GitFile ) {
						const Git::DiffFile& s = mStatus[index.parent().parent().row()]
													 .type[index.parent().row()]
													 .files[index.row()];
						std::string iconName =
							UIIconThemeManager::getIconNameFromFileName( s.file );
						auto* scene = mPlugin->getUISceneNode();
						auto* d = scene->findIcon( iconName );
						if ( !d )
							return scene->findIcon( "file" );
						return d;
					}
				}
				break;
			}
			default:
				break;
		}
		return {};
	}

	virtual bool classModelRoleEnabled() { return true; }

	const RepoStatus* repo( const ModelIndex& index ) const {
		if ( index.internalId() != Repo )
			return nullptr;
		return &mStatus[index.row()];
	}

	const RepoStatusType* statusType( const ModelIndex& index ) const {
		if ( index.internalId() != Status )
			return nullptr;
		return &mStatus[index.parent().row()].type[index.row()];
	}

	const DiffFile* file( const ModelIndex& index ) const {
		if ( index.internalId() != GitFile )
			return nullptr;
		return &mStatus[index.parent().parent().row()]
					.type[index.parent().row()]
					.files[index.row()];
	}

	std::vector<std::string> getFiles( const std::string& repo, uint32_t statusType ) const {
		std::vector<std::string> files;
		for ( const auto& status : mStatus ) {
			if ( status.repo == repo ) {
				for ( const auto& type : status.type ) {
					if ( static_cast<uint32_t>( type.type ) & statusType ) {
						for ( const auto& file : type.files )
							files.push_back( file.file );
						break;
					}
				}
				break;
			}
		}
		return files;
	}

  protected:
	std::vector<RepoStatus> mStatus;
	GitPlugin* mPlugin{ nullptr };
};

Plugin* GitPlugin::New( PluginManager* pluginManager ) {
	return eeNew( GitPlugin, ( pluginManager, false ) );
}

Plugin* GitPlugin::NewSync( PluginManager* pluginManager ) {
	return eeNew( GitPlugin, ( pluginManager, true ) );
}

GitPlugin::GitPlugin( PluginManager* pluginManager, bool sync ) : PluginBase( pluginManager ) {
	if ( sync ) {
		load( pluginManager );
	} else {
#if defined( GIT_THREADED ) && GIT_THREADED == 1
		mThreadPool->run( [&, pluginManager] { load( pluginManager ); } );
#else
		load( pluginManager );
#endif
	}
}

GitPlugin::~GitPlugin() {
	mShuttingDown = true;
	if ( mStatusButton )
		mStatusButton->close();
	if ( mSidePanel && mTab )
		mSidePanel->removeTab( mTab );
}

void GitPlugin::load( PluginManager* pluginManager ) {
	AtomicBoolScopedOp loading( mLoading, true );
	pluginManager->subscribeMessages( this,
									  [this]( const auto& notification ) -> PluginRequestHandle {
										  return processMessage( notification );
									  } );

	std::string path = pluginManager->getPluginsPath() + "git.json";
	if ( FileSystem::fileExists( path ) ||
		 FileSystem::fileWrite( path, "{\n  \"config\":{},\n  \"keybindings\":{}\n}\n" ) ) {
		mConfigPath = path;
	}
	std::string data;
	if ( !FileSystem::fileGet( path, data ) )
		return;
	mConfigHash = String::hash( data );

	json j;
	try {
		j = json::parse( data, nullptr, true, true );
	} catch ( const json::exception& e ) {
		Log::error( "GitPlugin::load - Error parsing config from path %s, error: %s, config "
					"file content:\n%s",
					path.c_str(), e.what(), data.c_str() );
		// Recreate it
		j = json::parse( "{\n  \"config\":{},\n  \"keybindings\":{},\n}\n", nullptr, true, true );
	}

	bool updateConfigFile = false;

	if ( j.contains( "config" ) ) {
		auto& config = j["config"];

		if ( config.contains( "ui_refresh_frequency" ) )
			mRefreshFreq = Time::fromString( config.value( "ui_refresh_frequency", "5s" ) );
		else {
			config["ui_refresh_frequency"] = mRefreshFreq.toString();
			updateConfigFile = true;
		}

		if ( config.contains( "statusbar_display_branch" ) )
			mStatusBarDisplayBranch = config.value( "statusbar_display_branch", true );
		else {
			config["statusbar_display_branch"] = mStatusBarDisplayBranch;
			updateConfigFile = true;
		}

		if ( config.contains( "statusbar_display_modifications" ) )
			mStatusBarDisplayModifications =
				config.value( "statusbar_display_modifications", true );
		else {
			config["statusbar_display_modifications"] = mStatusBarDisplayModifications;
			updateConfigFile = true;
		}

		if ( config.contains( "status_recurse_submodules" ) )
			mStatusRecurseSubmodules = config.value( "status_recurse_submodules", true );
		else {
			config["status_recurse_submodules"] = mStatusRecurseSubmodules;
			updateConfigFile = true;
		}
	}

	if ( mKeyBindings.empty() ) {
		mKeyBindings["git-blame"] = "alt+shift+b";
	}

	if ( j.contains( "keybindings" ) ) {
		auto& kb = j["keybindings"];
		auto list = { "git-blame" };
		for ( const auto& key : list ) {
			if ( kb.contains( key ) ) {
				if ( !kb[key].empty() )
					mKeyBindings[key] = kb[key];
			} else {
				kb[key] = mKeyBindings[key];
				updateConfigFile = true;
			}
		}
	}

	if ( updateConfigFile ) {
		std::string newData = j.dump( 2 );
		if ( newData != data ) {
			FileSystem::fileWrite( path, newData );
			mConfigHash = String::hash( newData );
		}
	}

	mGit = std::make_unique<Git>( pluginManager->getWorkspaceFolder() );
	mGitFound = !mGit->getGitPath().empty();
	mProjectPath = mRepoSelected = mGit->getProjectPath();

	if ( getUISceneNode() ) {
		updateStatus();
		updateBranches();
	}

	subscribeFileSystemListener();
	mReady = true;
	fireReadyCbs();
	setReady();
}

void GitPlugin::updateUINow( bool force ) {
	if ( !mGit || !getUISceneNode() )
		return;

	updateStatus( force );
	updateBranches();
}

void GitPlugin::updateUI() {
	if ( !mGit || !getUISceneNode() )
		return;

	getUISceneNode()->debounce( [this] { updateUINow(); }, mRefreshFreq,
								String::hash( "git::status-update" ) );
}

void GitPlugin::updateStatusBarSync() {
	buildSidePanelTab();

	mGitContentView->setVisible( !mGit->getGitFolder().empty() )
		->setEnabled( !mGit->getGitFolder().empty() );
	mGitNoContentView->setVisible( !mGitContentView->isVisible() )
		->setEnabled( !mGitContentView->isEnabled() );

	if ( !mGit->getGitFolder().empty() ) {
		Lock l( mGitStatusMutex );
		mStatusTree->setModel( GitStatusModel::asModel( mGitStatus.files, this ) );
		mStatusTree->expandAll();
	}

	if ( !mStatusBar )
		getUISceneNode()->bind( "status_bar", mStatusBar );
	if ( !mStatusBar )
		return;

	if ( !mStatusButton ) {
		mStatusButton = UIPushButton::New();
		mStatusButton->setLayoutSizePolicy( SizePolicy::WrapContent, SizePolicy::MatchParent );
		mStatusButton->setParent( mStatusBar );
		mStatusButton->setId( "status_git" );
		mStatusButton->setClass( "status_but" );
		mStatusButton->setIcon( iconDrawable( "source-control", 10 ) );
		mStatusButton->reloadStyle( true, true );
		mStatusButton->getTextBox()->setUsingCustomStyling( true );
		auto childCount = mStatusBar->getChildCount();
		if ( childCount > 2 )
			mStatusButton->toPosition( mStatusBar->getChildCount() - 2 );

		mStatusButton->on( Event::MouseClick, [this]( const Event* ) {
			if ( mTab ) {
				mTab->setTabSelected();
				if ( mGitStatus.totalInserts || mGitStatus.totalDeletions )
					mPanelSwicher->getListBox()->setSelected( 1 );
			}
		} );
	}

	mStatusButton->setVisible( !mGit->getGitFolder().empty() );

	if ( mGit->getGitFolder().empty() )
		return;

	std::string text;
	{
		Lock l( mGitStatusMutex );
		text = mStatusBarDisplayModifications &&
					   ( mGitStatus.totalInserts || mGitStatus.totalDeletions )
				   ? String::format( "%s (+%d / -%d)", gitBranch().c_str(), mGitStatus.totalInserts,
									 mGitStatus.totalDeletions )
				   : gitBranch();
	}
	mStatusButton->setText( text );

	if ( !mStatusBarDisplayModifications )
		return;

	if ( !mStatusCustomTokenizer.has_value() ) {
		std::vector<SyntaxPattern> patterns;
		auto fontColor( getVarColor( "--font" ) );
		auto insertedColor( getVarColor( "--theme-success" ) );
		auto deletedColor( getVarColor( "--theme-error" ) );
		patterns.emplace_back(
			SyntaxPattern( { ".*%((%+%d+)%s/%s(%-%d+)%)" }, { "normal", "keyword", "keyword2" } ) );
		SyntaxDefinition syntaxDef( "custom_build", {}, std::move( patterns ) );
		SyntaxColorScheme scheme( "status_bar_git",
								  { { "normal"_sst, { fontColor } },
									{ "keyword"_sst, { insertedColor } },
									{ "keyword2"_sst, { deletedColor } } },
								  {} );
		mStatusCustomTokenizer = { std::move( syntaxDef ), std::move( scheme ) };
	}

	SyntaxTokenizer::tokenizeText( mStatusCustomTokenizer->def, mStatusCustomTokenizer->scheme,
								   *mStatusButton->getTextBox()->getTextCache() );

	mStatusButton->invalidateDraw();
}

void GitPlugin::updateStatus( bool force ) {
	if ( !mGit || !mGitFound || !mStatusBarDisplayBranch || mRunningUpdateStatus )
		return;
	mRunningUpdateStatus++;
	mThreadPool->run(
		[this, force] {
			if ( !mGit )
				return;

			if ( !mGit->getGitFolder().empty() ) {
				auto prevBranch = mGitBranch;
				{
					Lock l( mGitBranchMutex );
					mGitBranch = mGit->branch();
				}
				Git::Status prevGitStatus;
				{
					Lock l( mGitStatusMutex );
					prevGitStatus = mGitStatus;
				}
				Git::Status newGitStatus = mGit->status( mStatusRecurseSubmodules );
				{
					Lock l( mGitStatusMutex );
					mGitStatus = std::move( newGitStatus );
					if ( !force && mGitBranch == prevBranch && mGitStatus == prevGitStatus )
						return;
				}
			} else if ( !mStatusButton ) {
				return;
			}

			getUISceneNode()->runOnMainThread( [this] { updateStatusBarSync(); } );
		},
		[this]( auto ) { mRunningUpdateStatus--; } );
}

PluginRequestHandle GitPlugin::processMessage( const PluginMessage& msg ) {
	switch ( msg.type ) {
		case PluginMessageType::WorkspaceFolderChanged: {
			if ( mGit ) {
				mGit->setProjectPath( msg.asJSON()["folder"] );

				{
					Lock l( mRepoMutex );
					mProjectPath = mRepoSelected = mGit->getProjectPath();
				}

				updateUINow( true );
				mInitialized = true;
			}
			break;
		}
		case ecode::PluginMessageType::UIReady: {
			if ( !mInitialized )
				updateUINow();
			break;
		}
		case ecode::PluginMessageType::UIThemeReloaded: {
			mStatusCustomTokenizer.reset();
			updateUINow( true );
			break;
		}
		default:
			break;
	}
	return PluginRequestHandle::empty();
}

void GitPlugin::onFileSystemEvent( const FileEvent& ev, const FileInfo& file ) {
	PluginBase::onFileSystemEvent( ev, file );

	if ( mShuttingDown || isLoading() )
		return;

	if ( String::startsWith( file.getFilepath(), mGit->getGitFolder() ) &&
		 ( file.getExtension() == "lock" || file.isDirectory() ) )
		return;

	updateUI();
}

void GitPlugin::displayTooltip( UICodeEditor* editor, const Git::Blame& blame,
								const Vector2f& position ) {
	// HACK: Gets the old font style to restore it when the tooltip is hidden
	UITooltip* tooltip = editor->createTooltip();
	if ( tooltip == nullptr )
		return;

	String str( blame.error.empty()
					? String::format( "%s: %s (%s)\n%s: %s (%s)\n%s: %s\n\n%s",
									  i18n( "commit", "commit" ).capitalize().toUtf8().c_str(),
									  blame.commitHash.c_str(), blame.commitShortHash.c_str(),
									  i18n( "author", "author" ).capitalize().toUtf8().c_str(),
									  blame.author.c_str(), blame.authorEmail.c_str(),
									  i18n( "date", "date" ).capitalize().toUtf8().c_str(),
									  blame.date.c_str(), blame.commitMessage.c_str() )
					: blame.error );

	Text::wrapText( str, PixelDensity::dpToPx( 400 ), tooltip->getFontStyleConfig(),
					editor->getTabWidth() );

	editor->setTooltipText( str );

	mTooltipInfoShowing = true;
	mOldBackgroundColor = tooltip->getBackgroundColor();
	if ( Color::Transparent == mOldBackgroundColor ) {
		tooltip->reloadStyle( true, true, true, true );
		mOldBackgroundColor = tooltip->getBackgroundColor();
	}
	mOldTextStyle = tooltip->getFontStyle();
	mOldTextAlign = tooltip->getHorizontalAlign();
	mOldDontAutoHideOnMouseMove = tooltip->dontAutoHideOnMouseMove();
	mOldUsingCustomStyling = tooltip->getUsingCustomStyling();
	tooltip->setHorizontalAlign( UI_HALIGN_LEFT );
	tooltip->setPixelsPosition( tooltip->getTooltipPosition( position ) );
	tooltip->setDontAutoHideOnMouseMove( true );
	tooltip->setUsingCustomStyling( true );
	tooltip->setData( String::hash( "git" ) );
	tooltip->setBackgroundColor( editor->getColorScheme().getEditorColor( "background"_sst ) );
	tooltip->getUIStyle()->setStyleSheetProperty( StyleSheetProperty(
		"background-color",
		editor->getColorScheme().getEditorColor( "background"_sst ).toHexString(), true,
		StyleSheetSelectorRule::SpecificityImportant ) );

	if ( !mTooltipCustomSyntaxDef.has_value() ) {
		static std::vector<SyntaxPattern> patterns;

		patterns.emplace_back( SyntaxPattern( { "([%w:]+)%s(%x+)%s%((%x+)%)" },
											  { "normal", "keyword", "number", "number" } ) );
		patterns.emplace_back( SyntaxPattern( { "([%w:]+)%s(.*)%(([%w%.-]+@[%w-]+%.%w+)%)" },
											  { "normal", "keyword", "function", "link" } ) );
		patterns.emplace_back( SyntaxPattern( { "([%w:]+)%s(%d%d%d%d%-%d%d%-%d%d[%s%d%-+:]+)" },
											  { "normal", "keyword", "warning" } ) );
		SyntaxDefinition syntaxDef( "custom_build", {}, std::move( patterns ) );
		mTooltipCustomSyntaxDef = std::move( syntaxDef );
	}

	SyntaxTokenizer::tokenizeText( *mTooltipCustomSyntaxDef, editor->getColorScheme(),
								   *tooltip->getTextCache() );

	tooltip->notifyTextChangedFromTextCache();

	if ( editor->hasFocus() && !tooltip->isVisible() )
		tooltip->show();
}

void GitPlugin::hideTooltip( UICodeEditor* editor ) {
	mTooltipInfoShowing = false;
	UITooltip* tooltip = nullptr;
	if ( editor && ( tooltip = editor->getTooltip() ) && tooltip->isVisible() &&
		 tooltip->getData() == String::hash( "git" ) ) {
		editor->setTooltipText( "" );
		tooltip->hide();
		// Restore old tooltip state
		tooltip->setData( 0 );
		tooltip->setFontStyle( mOldTextStyle );
		tooltip->setHorizontalAlign( mOldTextAlign );
		tooltip->setUsingCustomStyling( mOldUsingCustomStyling );
		tooltip->setDontAutoHideOnMouseMove( mOldDontAutoHideOnMouseMove );
		tooltip->setBackgroundColor( mOldBackgroundColor );
	}
}

bool GitPlugin::onMouseLeave( UICodeEditor* editor, const Vector2i&, const Uint32& ) {
	hideTooltip( editor );
	return false;
}

std::string GitPlugin::gitBranch() {
	Lock l( mGitBranchMutex );
	return mGitBranch;
}

void GitPlugin::onRegisterListeners( UICodeEditor* editor, std::vector<Uint32>& listeners ) {
	listeners.push_back(
		editor->addEventListener( Event::OnCursorPosChange, [this, editor]( const Event* ) {
			if ( mTooltipInfoShowing )
				hideTooltip( editor );
		} ) );
}

void GitPlugin::onBeforeUnregister( UICodeEditor* editor ) {
	for ( auto& kb : mKeyBindings )
		editor->getKeyBindings().removeCommandKeybind( kb.first );
}

void GitPlugin::onUnregisterDocument( TextDocument* doc ) {
	for ( auto& kb : mKeyBindings )
		doc->removeCommand( kb.first );
}

Color GitPlugin::getVarColor( const std::string& var ) {
	return Color::fromString(
		getUISceneNode()->getRoot()->getUIStyle()->getVariable( var ).getValue() );
}

void GitPlugin::blame( UICodeEditor* editor ) {
	if ( !mGitFound ) {
		editor->setTooltipText(
			i18n( "git_not_found",
				  "Git binary not found.\nPlease check that git is accesible via PATH" ) );
		return;
	}
	mThreadPool->run( [this, editor]() {
		auto blame = mGit->blame( editor->getDocument().getFilePath(),
								  editor->getDocument().getSelection().start().line() + 1 );
		editor->runOnMainThread( [this, editor, blame] {
			displayTooltip(
				editor, blame,
				editor->getScreenPosition( editor->getDocument().getSelection().start() )
					.getPosition() );
		} );
	} );
}

void GitPlugin::checkout( Git::Branch branch ) {
	if ( !mGit )
		return;

	const auto checkOutFn = [this, branch]( bool createLocal ) {
		mLoader->setVisible( true );
		mThreadPool->run( [this, branch, createLocal] {
			auto result = createLocal
							  ? mGit->checkoutAndCreateLocalBranch( branch.name, mRepoSelected )
							  : mGit->checkout( branch.name, mRepoSelected );
			if ( result.success() ) {
				{
					Lock l( mGitBranchMutex );
					mGitBranch = branch.name;
				}
				if ( mBranchesTree->getModel() ) {
					if ( createLocal )
						updateBranches();
					else
						mBranchesTree->getModel()->invalidate( Model::DontInvalidateIndexes );
				}
			} else {
				showMessage( LSPMessageType::Warning, result.result );
			}
			getUISceneNode()->runOnMainThread( [this] { mLoader->setVisible( false ); } );
		} );
	};

	if ( branch.type == Git::RefType::Remote ) {
		UIMessageBox* msgBox = UIMessageBox::New(
			UIMessageBox::YES_NO, i18n( "git_create_local_branch", "Create local branch?" ) );
		msgBox->on( Event::OnConfirm, [checkOutFn]( const Event* ) { checkOutFn( true ); } );
		msgBox->on( Event::OnCancel, [checkOutFn]( const Event* ) { checkOutFn( false ); } );
		msgBox->setTitle( i18n( "git_checkout", "Check Out" ) );
		msgBox->center();
		msgBox->showWhenReady();
		return;
	}

	checkOutFn( false );
}

// Branch operations
void GitPlugin::branchRename( Git::Branch branch ) {
	UIMessageBox* msgBox = UIMessageBox::New(
		UIMessageBox::INPUT,
		String::format(
			i18n( "git_rename_branch_ask", "Enter the new name for the branch: '%s'" ).toUtf8(),
			branch.name ) );
	msgBox->on( Event::OnConfirm, [this, branch, msgBox]( const Event* ) {
		std::string newName( msgBox->getTextInput()->getText().toUtf8() );
		if ( newName.empty() || branch.name == newName )
			return;
		msgBox->closeWindow();
		runAsync( [this, branch,
				   newName]() { return mGit->renameBranch( branch.name, newName, mRepoSelected ); },
				  false, true );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_rename_branch", "Rename Branch" ) );
	msgBox->center();
	msgBox->getTextInput()->setText( branch.name );
	msgBox->showWhenReady();
}

void GitPlugin::branchDelete( Git::Branch branch ) {
	UIMessageBox* msgBox = UIMessageBox::New(
		UIMessageBox::OK_CANCEL,
		String::format( i18n( "git_confirm_branch_delete",
							  "Are you sure you want to delete the branch \"%s\"?" )
							.toUtf8(),
						branch.name ) );

	msgBox->on( Event::OnConfirm, [this, branch]( auto ) {
		runAsync( [this, branch]() { return mGit->deleteBranch( branch.name, mRepoSelected ); },
				  false, true );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_confirm", "Confirm" ) );
	msgBox->center();
	msgBox->showWhenReady();
}

void GitPlugin::pull() {
	runAsync( [this]() { return mGit->pull( mRepoSelected ); }, true, true );
}

void GitPlugin::push() {
	UIMessageBox* msgBox = UIMessageBox::New(
		UIMessageBox::OK_CANCEL,
		i18n( "git_confirm_push_changes",
			  "Are you sure you want to push the local changes to the remote server?" ) );

	msgBox->on( Event::OnConfirm, [this]( auto ) {
		runAsync( [this]() { return mGit->push( mRepoSelected ); }, true, true );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_confirm", "Confirm" ) );
	msgBox->center();
	msgBox->showWhenReady();
}

void GitPlugin::fetch() {
	runAsync( [this]() { return mGit->fetch( mRepoSelected ); }, true, true );
}

void GitPlugin::branchCreate() {
	UIMessageBox* msgBox = UIMessageBox::New(
		UIMessageBox::INPUT,
		i18n( "git_create_branch_ask",
			  "Create new branch at current branch (HEAD).\nEnter the name for the branch:" ) );
	msgBox->on( Event::OnConfirm, [this, msgBox]( const Event* ) {
		std::string newName( msgBox->getTextInput()->getText().toUtf8() );
		if ( newName.empty() )
			return;
		msgBox->closeWindow();
		runAsync( [this, newName]() { return mGit->createBranch( newName, true, mRepoSelected ); },
				  false, true );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_add_branch", "Add Branch" ) );
	msgBox->center();
	msgBox->showWhenReady();
}

void GitPlugin::commit() {
	UIMessageBox* msgBox = UIMessageBox::New( UIMessageBox::TEXT_EDIT,
											  i18n( "git_commit_message", "Commit Message:" ) );
	msgBox->on( Event::OnConfirm, [this, msgBox]( const Event* ) {
		std::string msg( msgBox->getTextEdit()->getText().toUtf8() );
		if ( msg.empty() )
			return;
		msgBox->closeWindow();
		runAsync( [this, msg]() { return mGit->commit( msg ); }, true, true );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_commit", "Commit" ) );
	msgBox->center();
	msgBox->showWhenReady();
}

void GitPlugin::fastForwardMerge( Git::Branch branch ) {
	runAsync(
		[this, branch]() {
			if ( branch.name == mGitBranch )
				return mGit->fastForwardMerge( mRepoSelected );

			auto remoteBranch = mGit->getAllBranchesAndTags(
				Git::RefType::Remote, "refs/remotes/" + branch.remote, mRepoSelected );
			if ( remoteBranch.empty() )
				return Git::Result{ "", -1 };

			return mGit->updateRef( branch.name, remoteBranch[0].lastCommit, mRepoSelected );
		},
		false, true );
}
// Branch operations

// File operations
void GitPlugin::stage( const std::vector<std::string>& files ) {
	if ( files.empty() )
		return;
	runAsync( [this, files]() { return mGit->add( files, mGit->repoPath( files[0] ) ); }, true,
			  false );
}

void GitPlugin::unstage( const std::vector<std::string>& files ) {
	if ( files.empty() )
		return;
	runAsync( [this, files]() { return mGit->reset( files, mGit->repoPath( files[0] ) ); }, true,
			  false );
}
// File operations

void GitPlugin::discard( const std::string& file ) {
	UIMessageBox* msgBox = UIMessageBox::New(
		UIMessageBox::OK_CANCEL,
		String::format( i18n( "git_confirm_discard_changes",
							  "Are you sure you want to discard the changes in file: \"%s\"?" )
							.toUtf8(),
						file ) );

	msgBox->on( Event::OnConfirm, [this, file]( auto ) {
		runAsync( [this, file]() { return mGit->restore( file, mGit->repoPath( file ) ); }, true,
				  false );
	} );
	msgBox->setCloseShortcut( { KEY_ESCAPE, KEYMOD_NONE } );
	msgBox->setTitle( i18n( "git_confirm", "Confirm" ) );
	msgBox->center();
	msgBox->showWhenReady();
}

void GitPlugin::openFile( const std::string& file ) {
	getUISceneNode()->runOnMainThread( [this, file] {
		mManager->getLoadFileFn()( mGit->getProjectPath() + file, []( auto, auto ) {} );
	} );
}

void GitPlugin::onRegister( UICodeEditor* editor ) {
	PluginBase::onRegister( editor );

	for ( auto& kb : mKeyBindings ) {
		if ( !kb.second.empty() )
			editor->getKeyBindings().addKeybindString( kb.second, kb.first );
	}

	if ( !editor->hasDocument() )
		return;

	auto& doc = editor->getDocument();
	doc.setCommand( "git-blame", [this]( TextDocument::Client* client ) {
		blame( static_cast<UICodeEditor*>( client ) );
	} );
	doc.setCommand( "show-source-control-tab", [this]() {
		if ( mTab )
			mTab->setTabSelected();
	} );
	doc.setCommand( "git-pull", [this] { pull(); } );
	doc.setCommand( "git-push", [this] { push(); } );
	doc.setCommand( "git-fetch", [this] { fetch(); } );
	doc.setCommand( "git-commit", [this] { commit(); } );
}

void GitPlugin::onUnregister( UICodeEditor* editor ) {
	PluginBase::onUnregister( editor );
}

bool GitPlugin::onCreateContextMenu( UICodeEditor*, UIPopUpMenu* menu, const Vector2i& /*position*/,
									 const Uint32& /*flags*/ ) {
	if ( !mGitFound )
		return false;

	menu->addSeparator();

	auto* subMenu = UIPopUpMenu::New();
	subMenu->addClass( "gitplugin_menu" );

	auto addFn = [this, subMenu]( const std::string& txtKey, const std::string& txtVal,
								  const std::string& icon = "" ) {
		subMenu
			->add( i18n( txtKey, txtVal ),
				   !icon.empty() ? findIcon( icon )->getSize( PixelDensity::dpToPxI( 12 ) )
								 : nullptr,
				   KeyBindings::keybindFormat( mKeyBindings[txtKey] ) )
			->setId( txtKey );
	};

	addFn( "git-blame", "Git Blame" );

	menu->addSubMenu( i18n( "git", "Git" ),
					  mManager->getUISceneNode()
						  ->findIcon( "source-control" )
						  ->getSize( PixelDensity::dpToPxI( 12 ) ),
					  subMenu );

	return false;
}

bool GitPlugin::onKeyDown( UICodeEditor* editor, const KeyEvent& event ) {
	if ( event.getSanitizedMod() == 0 && event.getKeyCode() == KEY_ESCAPE && editor->getTooltip() &&
		 editor->getTooltip()->isVisible() ) {
		hideTooltip( editor );
	}

	return false;
}

void GitPlugin::updateBranches( bool force ) {
	if ( !mGit || !mGitFound || ( mRunningUpdateBranches && !force ) )
		return;

	mRunningUpdateBranches++;
	mThreadPool->run(
		[this] {
			if ( !mGit || mGit->getGitFolder().empty() )
				return;

			auto prevBranch = mGitBranch;
			{
				Lock l( mGitBranchMutex );
				mGitBranch = mGit->branch();
			}

			mGit->getSubModules();

			auto branches = mGit->getAllBranchesAndTags( Git::RefType::All, {}, mRepoSelected );
			auto hash = hashBranches( branches );
			auto model = GitBranchModel::asModel( std::move( branches ), hash, this );

			if ( mBranchesTree &&
				 static_cast<GitBranchModel*>( mBranchesTree->getModel() )->getHash() == hash ) {
				if ( prevBranch != mGitBranch )
					mBranchesTree->getModel()->invalidate( Model::DontInvalidateIndexes );
				return;
			}

			getUISceneNode()->runOnMainThread( [this, model] { updateBranchesUI( model ); } );
		},
		[this]( auto ) { mRunningUpdateBranches--; } );
}

void GitPlugin::updateBranchesUI( std::shared_ptr<GitBranchModel> model ) {
	buildSidePanelTab();
	mBranchesTree->setModel( model );
	mBranchesTree->setColumnsVisible( { GitBranchModel::Name } );
	mBranchesTree->expandAll();

	if ( !mGit )
		return;

	auto subModules = mGit->getSubModules();
	std::sort( subModules.begin(), subModules.end() );

	size_t selectedIdx = 0;
	std::vector<std::pair<std::string, std::string>> repos;
	repos.clear();
	repos.reserve( subModules.size() + 1 );
	repos.emplace_back( FileSystem::fileNameFromPath( mProjectPath ), mProjectPath );
	size_t i = 1;
	for ( auto& subModule : subModules ) {
		std::string subModulePath = mProjectPath + subModule;
		if ( subModulePath == mRepoSelected )
			selectedIdx = i;
		repos.emplace_back( FileSystem::fileNameFromPath( subModule ), std::move( subModulePath ) );
		i++;
	}

	if ( repos == mRepos )
		return;

	mRepos = std::move( repos );
	std::vector<String> items;
	for ( const auto& repo : mRepos )
		items.push_back( repo.first );

	mRepoDropDown->getListBox()->clear();
	mRepoDropDown->getListBox()->addListBoxItems( items );
	mRepoDropDown->getListBox()->setSelected( selectedIdx );
}

void GitPlugin::buildSidePanelTab() {
	if ( mTab )
		return;
	if ( mSidePanel == nullptr )
		getUISceneNode()->bind( "panel", mSidePanel );
	UIIcon* icon = findIcon( "source-control" );
	UIWidget* node = getUISceneNode()->loadLayoutFromString(
		R"html(
		<RelativeLayout id="git_panel" lw="mp" lh="mp">
			<vbox id="git_content" lw="mp" lh="mp">
				<DropDownList id="git_panel_switcher" lw="mp" lh="22dp" border-type="inside" border-right-width="0" border-left-width="0" border-top-width="0" border-bottom-left-radius="0" border-bottom-right-radius="0" />
				<StackWidget id="git_panel_stack" lw="mp" lh="0" lw8="1">
					<vbox id="git_branches" lw="mp" lh="wc">
						<hbox lw="mp" lh="wc" padding="4dp">
							<DropDownList id="git_repo" lw="0" lh="wc" lw8="1" />
							<PushButton id="branch_pull" text="@string(pull_branch, Pull)" tooltip="@string(pull_branch, Pull Branch)" text-as-fallback="true" icon="icon(repo-pull, 12dp)" margin-left="2dp" />
							<PushButton id="branch_push" text="@string(push_branch, Push)" tooltip="@string(push_branch, Push Branch)" text-as-fallback="true" icon="icon(repo-push, 12dp)" margin-left="2dp" />
							<PushButton id="branch_add" text="@string(add_branch, Add Branch)" tooltip="@string(add_branch, Add Branch)" text-as-fallback="true" icon="icon(add, 12dp)" margin-left="2dp" />
						</hbox>
						<TreeView id="git_branches_tree" lw="mp" lh="0" lw8="1" />
					</vbox>
					<vbox id="git_status" lw="mp" lh="mp">
						<TreeView id="git_status_tree" lw="mp" lh="mp" />
					</vbox>
				</StackWidget>
			</vbox>
			<TextView id="git_no_content" lw="mp" lh="wc" word-wrap="true" visible="false"
				text='@string(git_no_git_repo, "Current folder is not a Git repository.")' padding="16dp" />
			<Loader margin-top="32dp" id="git_panel_loader" indeterminate="true" lw="24dp" lh="24dp" outline-thickness="2dp" visible="false" layout_gravity="bottom|right" margin-bottom="24dp" margin-right="24dp" />
		</RelativeLayout>
		)html" );
	mTab = mSidePanel->add( i18n( "source_control", "Source Control" ), node,
							icon ? icon->getSize( PixelDensity::dpToPx( 12 ) ) : nullptr );
	mTab->setId( "source_control" );
	mTab->setTextAsFallback( true );

	node->bind( "git_panel_switcher", mPanelSwicher );
	node->bind( "git_panel_stack", mStackWidget );
	node->bind( "git_branches_tree", mBranchesTree );
	node->bind( "git_status_tree", mStatusTree );
	node->bind( "git_content", mGitContentView );
	node->bind( "git_no_content", mGitNoContentView );
	node->bind( "git_panel_loader", mLoader );
	node->bind( "git_repo", mRepoDropDown );

	node->find( "branch_pull" )->on( Event::MouseClick, [this]( auto ) { pull(); } );
	node->find( "branch_push" )->on( Event::MouseClick, [this]( auto ) { push(); } );
	node->find( "branch_add" )->on( Event::MouseClick, [this]( auto ) { branchCreate(); } );

	mBranchesTree->setAutoExpandOnSingleColumn( true );
	mBranchesTree->setHeadersVisible( false );
	mBranchesTree->setExpandersAsIcons( true );
	mBranchesTree->setIndentWidth( PixelDensity::dpToPx( 16 ) );
	mBranchesTree->on( Event::OnModelEvent, [this]( const Event* event ) {
		const ModelEvent* modelEvent = static_cast<const ModelEvent*>( event );
		if ( !modelEvent->getModelIndex().hasParent() )
			return;

		const Git::Branch* branch =
			static_cast<Git::Branch*>( modelEvent->getModelIndex().internalData() );

		switch ( modelEvent->getModelEventType() ) {
			case Abstract::ModelEventType::Open: {
				checkout( *branch );
				break;
			}
			case Abstract::ModelEventType::OpenMenu: {
				bool focusOnSelection = mBranchesTree->getFocusOnSelection();
				mBranchesTree->setFocusOnSelection( false );
				mBranchesTree->getSelection().set( modelEvent->getModelIndex() );
				mBranchesTree->setFocusOnSelection( focusOnSelection );
				openBranchMenu( *branch );
				break;
			}
			default:
				break;
		}
	} );

	auto listBox = mPanelSwicher->getListBox();
	listBox->addListBoxItems( { i18n( "branches", "Branches" ), i18n( "status", "Status" ) } );
	mStackMap.resize( 2 );
	mStackMap[0] = node->find<UIWidget>( "git_branches" );
	mStackMap[1] = node->find<UIWidget>( "git_status" );
	listBox->setSelected( 0 );

	mPanelSwicher->on( Event::OnItemSelected, [this, listBox]( const Event* ) {
		mStackWidget->setActiveWidget( mStackMap[listBox->getItemSelectedIndex()] );
	} );

	mStatusTree->setAutoColumnsWidth( true );
	mStatusTree->setHeadersVisible( false );
	mStatusTree->setExpandersAsIcons( true );
	mStatusTree->on( Event::OnModelEvent, [this]( const Event* event ) {
		const ModelEvent* modelEvent = static_cast<const ModelEvent*>( event );
		if ( modelEvent->getModel() == nullptr )
			return;

		auto model = static_cast<const GitStatusModel*>( modelEvent->getModel() );
		if ( modelEvent->getModelIndex().internalId() == GitStatusModel::GitFile ) {
			const Git::DiffFile* file = model->file( modelEvent->getModelIndex() );
			if ( file == nullptr )
				return;

			switch ( modelEvent->getModelEventType() ) {
				case Abstract::ModelEventType::OpenMenu: {
					bool focusOnSelection = mStatusTree->getFocusOnSelection();
					mStatusTree->setFocusOnSelection( false );
					mStatusTree->getSelection().set( modelEvent->getModelIndex() );
					mStatusTree->setFocusOnSelection( focusOnSelection );
					openFileStatusMenu( *file );
					break;
				}
				default:
					break;
			}
		} else if ( modelEvent->getModelIndex().internalId() == GitStatusModel::Status ) {
			switch ( modelEvent->getModelEventType() ) {
				case Abstract::ModelEventType::OpenMenu: {
					bool focusOnSelection = mStatusTree->getFocusOnSelection();
					mStatusTree->setFocusOnSelection( false );
					mStatusTree->getSelection().set( modelEvent->getModelIndex() );
					mStatusTree->setFocusOnSelection( focusOnSelection );

					const auto* status = model->statusType( modelEvent->getModelIndex() );
					if ( status->type == Git::GitStatusType::Staged ||
						 status->type == Git::GitStatusType::Untracked ||
						 status->type == Git::GitStatusType::Changed ) {
						UIPopUpMenu* menu = UIPopUpMenu::New();
						menu->setId( "git_status_type_menu" );

						if ( status->type == Git::GitStatusType::Staged ) {
							addMenuItem( menu, "git-commit", "Commit" );

							addMenuItem( menu, "git-unstage-all", "Unstage All" );
						}

						if ( status->type == Git::GitStatusType::Untracked ||
							 status->type == Git::GitStatusType::Changed )
							addMenuItem( menu, "git-stage-all", "Stage All" );

						menu->on( Event::OnItemClicked, [this, model]( const Event* event ) {
							if ( !mGit )
								return;
							UIMenuItem* item = event->getNode()->asType<UIMenuItem>();
							std::string id( item->getId() );
							if ( id == "git-commit" ) {
								commit();
							} else if ( id == "git-stage-all" ) {
								stage( model->getFiles( mGit->repoName( "" ),
														(Uint32)Git::GitStatusType::Untracked |
															(Uint32)Git::GitStatusType::Changed ) );
							} else if ( id == "git-unstage-all" ) {
								unstage( model->getFiles( mGit->repoName( "" ),
														  (Uint32)Git::GitStatusType::Staged ) );
							}
						} );

						menu->showOverMouseCursor();
					}

					break;
				}
				default:
					break;
			}
		}
	} );

	mRepoDropDown->on( Event::OnItemSelected, [this]( const Event* ) {
		const auto& txt = mRepoDropDown->getListBox()->getItemSelectedText();

		for ( const auto& repo : mRepos ) {
			if ( txt == repo.first ) {
				{
					Lock l( mRepoMutex );
					mRepoSelected = repo.second;
				}
				updateBranches( true );
				break;
			}
		}
	} );
}

void GitPlugin::openBranchMenu( const Git::Branch& branch ) {
	UIPopUpMenu* menu = UIPopUpMenu::New();
	menu->setId( "git_branch_menu" );

	addMenuItem( menu, "git-fetch", "Fetch" );

	if ( mGitBranch != branch.name ) {
		addMenuItem( menu, "git-checkout", "Check Out..." );
		if ( branch.type == Git::RefType::Head ) {
			addMenuItem( menu, "git-branch-rename", "Rename" );
			addMenuItem( menu, "git-branch-delete", "Delete" );
		}
	} else {
		if ( branch.type == Git::RefType::Head ) {
			addMenuItem( menu, "git-pull", "Pull", "repo-pull" );
			if ( branch.ahead )
				addMenuItem( menu, "git-push", "Push", "repo-push" );
		}
	}

	if ( branch.type == Git::RefType::Head && branch.behind )
		addMenuItem( menu, "git-fast-forward-merge", "Fast Forward Merge" );

	addMenuItem( menu, "git-create-branch", "Create Branch", "repo-forked" );

	menu->on( Event::OnItemClicked, [this, branch]( const Event* event ) {
		if ( !mGit )
			return;
		UIMenuItem* item = event->getNode()->asType<UIMenuItem>();
		std::string id( item->getId() );
		if ( id == "git-checkout" ) {
			checkout( branch );
		} else if ( id == "git-pull" ) {
			pull();
		} else if ( id == "git-push" ) {
			push();
		} else if ( id == "git-branch-delete" ) {
			branchDelete( branch );
		} else if ( id == "git-branch-rename" ) {
			branchRename( branch );
		} else if ( id == "git-fetch" ) {
			fetch();
		} else if ( id == "git-fast-forward-merge" ) {
			fastForwardMerge( branch );
		} else if ( id == "git-create-branch" ) {
			branchCreate();
		}
	} );

	menu->showOverMouseCursor();
}

void GitPlugin::openFileStatusMenu( const Git::DiffFile& file ) {
	UIPopUpMenu* menu = UIPopUpMenu::New();
	menu->setId( "git_file_status_menu" );

	addMenuItem( menu, "git-open-file", "Open File" );

	if ( file.statusType != Git::GitStatusType::Staged ) {
		addMenuItem( menu, "git-stage", "Stage" );
	} else {
		addMenuItem( menu, "git-unstage", "Unstage" );
	}

	menu->addSeparator();

	addMenuItem( menu, "git-discard", "Discard" );

	menu->on( Event::OnItemClicked, [this, file]( const Event* event ) {
		if ( !mGit )
			return;
		UIMenuItem* item = event->getNode()->asType<UIMenuItem>();
		std::string id( item->getId() );
		if ( id == "git-stage" ) {
			stage( { file.file } );
		} else if ( id == "git-unstage" ) {
			unstage( { file.file } );
		} else if ( id == "git-discard" ) {
			discard( file.file );
		} else if ( id == "git-open-file" ) {
			openFile( file.file );
		}
	} );

	menu->showOverMouseCursor();
}

void GitPlugin::runAsync( std::function<Git::Result()> fn, bool _updateStatus,
						  bool _updateBranches ) {
	if ( !mGit )
		return;
	mLoader->setVisible( true );
	mThreadPool->run( [this, fn, _updateStatus, _updateBranches] {
		auto res = fn();
		getUISceneNode()->runOnMainThread( [this] { mLoader->setVisible( false ); } );
		if ( res.fail() ) {
			showMessage( LSPMessageType::Warning, res.result );
			return;
		}
		if ( _updateBranches )
			updateBranches();
		if ( _updateStatus )
			updateStatus( true );
	} );
}

void GitPlugin::addMenuItem( UIMenu* menu, const std::string& txtKey, const std::string& txtVal,
							 const std::string& icon ) {
	menu->add( i18n( txtKey, txtVal ), iconDrawable( icon, 12 ),
			   KeyBindings::keybindFormat( mKeyBindings[txtKey] ) )
		->setId( txtKey );
};

} // namespace ecode
