// Filename	: AUI.h
// Creator	: Tom Zhou
// Date		: June 12, 2004

#ifndef _AUI_H_
#define _AUI_H_

// Macros.
#include "AUIDef.h"

// Global functions.
#include "AStringWithWildcard.h"
#include "AUICommon.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "UIRenderTarget.h"
#include "CSplit.h"
#include "AUICTranslate.h"
#include "AUIClockIcon.h"
#include "LuaDlgApi.h"

// Manager and dialog.
#include "AUILuaDialog.h"
#include "AUILuaManager.h"

// Control objects.
#include "AUIComboBox.h"
#include "AUIConsole.h"
#include "AUICheckBox.h"
#include "AUICustomize.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIModelPicture.h"
#include "AUIProgress.h"
#include "AUIRadioButton.h"
#include "AUIScroll.h"
#include "AUISlider.h"
#include "AUIStillImageButton.h"
#include "AUISubDialog.h"
#include "AUITextArea.h"
#include "AUITreeView.h"
#include "AUIVerticalText.h"
#include "AUIWindowPicture.h"


#ifdef _ANGELICA22 
#define SHADERS_ROOT_DIR		"shaders\\2.2\\"
#else
#define SHADERS_ROOT_DIR		"shaders\\"
#endif

#ifdef _ANGELICA22
#include "AGPAPerfIntegration.h"
#endif

#endif //_AUI_H_