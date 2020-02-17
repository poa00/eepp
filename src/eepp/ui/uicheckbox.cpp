#include <eepp/graphics/text.hpp>
#include <eepp/graphics/textureregion.hpp>
#include <eepp/ui/css/propertydefinition.hpp>
#include <eepp/ui/uicheckbox.hpp>

namespace EE { namespace UI {

UICheckBox* UICheckBox::New() {
	return eeNew( UICheckBox, () );
}

UICheckBox::UICheckBox() : UITextView( "checkbox" ), mChecked( false ), mTextSeparation( 4 ) {
	mActiveButton = UINode::New();
	mActiveButton->setVisible( false );
	mActiveButton->setEnabled( true );
	mActiveButton->setParent( this );
	mActiveButton->setPosition( 0, 0 );
	mActiveButton->setSize( 16, 16 );

	mInactiveButton = UINode::New();
	mInactiveButton->setVisible( true );
	mInactiveButton->setEnabled( true );
	mInactiveButton->setParent( this );
	mInactiveButton->setPosition( 0, 0 );
	mInactiveButton->setSize( 16, 16 );

	onPaddingChange();

	applyDefaultTheme();
}

UICheckBox::~UICheckBox() {}

Uint32 UICheckBox::getType() const {
	return UI_TYPE_CHECKBOX;
}

bool UICheckBox::isType( const Uint32& type ) const {
	return UICheckBox::getType() == type ? true : UITextView::isType( type );
}

void UICheckBox::setTheme( UITheme* Theme ) {
	UIWidget::setTheme( Theme );

	setThemeSkin( Theme, "checkbox" );

	mActiveButton->setThemeSkin( Theme, "checkbox_active" );
	mInactiveButton->setThemeSkin( Theme, "checkbox_inactive" );

	onThemeLoaded();
}

void UICheckBox::onThemeLoaded() {
	UISkin* tSkin = mActiveButton->getSkin();

	if ( tSkin ) {
		mActiveButton->setSize( tSkin->getSize() );
		mActiveButton->centerVertical();
	}

	tSkin = mInactiveButton->getSkin();

	if ( NULL != tSkin ) {
		mInactiveButton->setSize( tSkin->getSize() );
		mInactiveButton->centerVertical();
	}

	mMinControlSize = mActiveButton->getSkinSize();

	onPaddingChange();

	UIWidget::onThemeLoaded();
}

void UICheckBox::onAutoSize() {
	if ( mFlags & UI_AUTO_SIZE ) {
		if ( getSize().getWidth() == 0 ) {
			setInternalPixelsWidth( (int)mTextCache->getTextWidth() +
									mActiveButton->getPixelsSize().getWidth() + mTextSeparation +
									mRealPadding.Left + mRealPadding.Right );
		}

		if ( getSize().getHeight() == 0 ) {
			setInternalHeight( mActiveButton->getSize().getHeight() + mRealPadding.Top +
							   mRealPadding.Bottom );
		}

		mActiveButton->centerVertical();
		mInactiveButton->centerVertical();
	}

	if ( mLayoutWidthRule == LayoutSizeRule::WrapContent ) {
		setInternalPixelsWidth( (int)mTextCache->getTextWidth() + mRealPadding.Left +
								mRealPadding.Right + mActiveButton->getPixelsSize().getWidth() +
								mTextSeparation );
	}

	if ( mLayoutHeightRule == LayoutSizeRule::WrapContent ) {
		setInternalPixelsHeight( (int)mTextCache->getTextHeight() + mRealPadding.Top +
								 mRealPadding.Bottom );

		mActiveButton->centerVertical();
		mInactiveButton->centerVertical();
	}
}

void UICheckBox::onSizeChange() {
	UITextView::onSizeChange();

	mActiveButton->centerVertical();
	mInactiveButton->centerVertical();
}

Uint32 UICheckBox::onMessage( const NodeMessage* Msg ) {
	switch ( Msg->getMsg() ) {
		case NodeMessage::Click: {
			if ( Msg->getFlags() & EE_BUTTON_LMASK ) {
				switchState();
			}

			if ( NULL != getEventDispatcher() &&
				 ( Msg->getSender() == mActiveButton || Msg->getSender() == mInactiveButton ) ) {
				sendMouseEvent( Event::MouseClick, getEventDispatcher()->getMousePos(),
								getEventDispatcher()->getPressTrigger() );
			}

			return 1;
		}
	}

	return 0;
}

void UICheckBox::switchState() {
	setChecked( !mChecked );
}

void UICheckBox::setChecked( const bool& checked ) {
	if ( !checked ) {
		mActiveButton->setVisible( false );
		mInactiveButton->setVisible( true );

		mChecked = false;
		unsetFlags( UI_CHECKED );
		popState( UIState::StateChecked );
	} else {
		mActiveButton->setVisible( true );
		mInactiveButton->setVisible( false );

		mChecked = true;
		setFlags( UI_CHECKED );
		pushState( UIState::StateChecked );
	}

	onValueChange();
}

const bool& UICheckBox::isChecked() const {
	return mChecked;
}

void UICheckBox::onPaddingChange() {
	mActiveButton->setPosition( mPadding.Left, mActiveButton->getPosition().y );
	mInactiveButton->setPosition( mPadding.Left, mInactiveButton->getPosition().y );

	UITextView::onPaddingChange();
}

void UICheckBox::alignFix() {
	UITextView::alignFix();

	switch ( Font::getHorizontalAlign( getFlags() ) ) {
		case UI_HALIGN_CENTER:
			mRealAlignOffset.x =
				( Float )( ( Int32 )( ( mSize.x - mRealPadding.Left - mRealPadding.Right -
										mTextCache->getTextWidth() -
										mActiveButton->getPixelsSize().getWidth() +
										PixelDensity::dpToPx( mTextSeparation ) ) /
									  2.f ) ) +
				mActiveButton->getPixelsSize().getWidth() + PixelDensity::dpToPx( mTextSeparation );
			break;
		case UI_HALIGN_RIGHT:
			mRealAlignOffset.x = ( (Float)mSize.x - mRealPadding.Left - mRealPadding.Right -
								   (Float)mTextCache->getTextWidth() );
			break;
		case UI_HALIGN_LEFT:
			mRealAlignOffset.x =
				mActiveButton->getPixelsSize().getWidth() + PixelDensity::dpToPx( mTextSeparation );
			break;
	}

	mAlignOffset = PixelDensity::pxToDp( mRealAlignOffset );
}

UINode* UICheckBox::getCheckedButton() const {
	return mActiveButton;
}

UINode* UICheckBox::getInactiveButton() const {
	return mInactiveButton;
}

Int32 UICheckBox::getTextSeparation() const {
	return mTextSeparation;
}

void UICheckBox::setTextSeparation( const Int32& textSeparation ) {
	mTextSeparation = textSeparation;

	setPadding( getPadding() );
}

std::string UICheckBox::getPropertyString( const PropertyDefinition* propertyDef,
										   const Uint32& propertyIndex ) {
	if ( NULL == propertyDef )
		return "";

	switch ( propertyDef->getPropertyId() ) {
		case PropertyId::Selected:
			return isChecked() ? "true" : "false";
			break;
		default:
			return UITextView::getPropertyString( propertyDef, propertyIndex );
	}
}

bool UICheckBox::applyProperty( const StyleSheetProperty& attribute ) {
	if ( !checkPropertyDefinition( attribute ) )
		return false;

	switch ( attribute.getPropertyDefinition()->getPropertyId() ) {
		case PropertyId::Selected:
			setChecked( attribute.asBool() );
			break;
		default:
			return UITextView::applyProperty( attribute );
	}

	return true;
}

Uint32 UICheckBox::onKeyDown( const KeyEvent& Event ) {
	UITextView::onKeyDown( Event );

	if ( Event.getKeyCode() == KEY_SPACE ) {
		if ( Sys::getTicks() - mLastTick > 250 ) {
			mLastTick = Sys::getTicks();

			setChecked( !mChecked );
		}
	}

	return 1;
}

void UICheckBox::onAlphaChange() {
	UITextView::onAlphaChange();

	mActiveButton->setAlpha( mAlpha );
	mInactiveButton->setAlpha( mAlpha );
}

}} // namespace EE::UI
