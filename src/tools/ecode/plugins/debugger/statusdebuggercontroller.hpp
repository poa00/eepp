#ifndef ECODE_STATUSDEBUGGERCONTROLLER_HPP
#define ECODE_STATUSDEBUGGERCONTROLLER_HPP

#include "../../uistatusbar.hpp"
#include "models/breakpointsmodel.hpp"
#include <eepp/system/luapattern.hpp>
#include <eepp/ui/tools/uicodeeditorsplitter.hpp>
#include <eepp/ui/uicodeeditor.hpp>
#include <eepp/ui/uirelativelayout.hpp>
#include <eepp/ui/uiscenenode.hpp>
#include <eepp/ui/uisplitter.hpp>
#include <eepp/ui/uitableview.hpp>
#include <eepp/ui/uitreeview.hpp>

using namespace EE;
using namespace EE::UI;
using namespace EE::UI::Tools;

namespace ecode {

class UIBreakpointsTableView : public UITableView {
  public:
	static UIWidget* New() { return eeNew( UIBreakpointsTableView, () ); }

	UIBreakpointsTableView() : UITableView() {}

	std::function<void( const std::string& file, int line, bool enabled )>
		onBreakpointEnabledChange;

	std::function<void( const std::string& file, int line )> onBreakpointRemove;

	std::function<UITextView*( UIPushButton* )> getCheckBoxFn( const ModelIndex& index,
															   const BreakpointsModel* model );

	UIWidget* createCell( UIWidget* rowWidget, const ModelIndex& index );
};

class StatusDebuggerController : public StatusBarElement {
  public:
	enum class State { NotStarted, Running, Paused };

	StatusDebuggerController( UISplitter* mainSplitter, UISceneNode* uiSceneNode,
							  PluginContextProvider* pluginContext );

	virtual ~StatusDebuggerController() {};

	UIWidget* getWidget();

	UIWidget* createWidget();

	UITableView* getUIThreads();

	UITableView* getUIStack();

	UIBreakpointsTableView* getUIBreakpoints();

	UITreeView* getUIVariables() const;

	UITabWidget* getUITabWidget() const;

	void setDebuggingState( State state );

  protected:
	UILinearLayout* mContainer{ nullptr };
	UITableView* mUIThreads{ nullptr };
	UITableView* mUIStack{ nullptr };
	UIBreakpointsTableView* mUIBreakpoints{ nullptr };
	UITreeView* mUIVariables{ nullptr };
	UITreeView* mUIExpressions{ nullptr };
	UISplitter* mUIThreadsSplitter{ nullptr };
	UIPushButton* mUIButStart{ nullptr };
	UIPushButton* mUIButStop{ nullptr };
	UIPushButton* mUIButContinue{ nullptr };
	UIPushButton* mUIButPause{ nullptr };
	UIPushButton* mUIButStepInto{ nullptr };
	UIPushButton* mUIButStepOver{ nullptr };
	UIPushButton* mUIButStepOut{ nullptr };
	UITabWidget* mUITabWidget{ nullptr };

	void createContainer();
};

} // namespace ecode

#endif // ECODE_STATUSDEBUGGERCONTROLLER_HPP
