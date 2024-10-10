// Filename	: AUIDialog.cpp
// Creator	: Tom Zhou
// Date		: May 12, 2004
// Desc		: AUIDialog is a class that simulate a Windows dialog.

#pragma warning (disable: 4284)

#include "AUI.h"

#include <Windows.h>
#include <WindowsX.h>

#include "AM.h"
#include "AFI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DFlatCollector.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A2DSpriteBuffer.h"
#include "A3DRenderTarget.h"
#include "A3DSurface.h"

#include "A3DGDI.h"
#include "AScriptFile.h"
#include "AWScriptFile.h"
#include "AUICTranslate.h"
#include "CSplit.h"
#include "FTInterface.h"
#include "UIRenderTarget.h"

#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUICheckBox.h"
#include "AUIComboBox.h"
#include "AUIConsole.h"
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
#include "AStringWithWildcard.h"

#ifdef _ANGELICA_AUDIO
#include "FEventInstance.h"
#endif

#define AUIDIALOG_FILE_VERSION	6
#define AUIDIALOG_HINT_DELAY	500

AUI_BEGIN_EVENT_MAP_BASE(AUIDialog)

AUI_END_EVENT_MAP()


AUI_BEGIN_COMMAND_MAP_BASE(AUIDialog)

AUI_END_COMMAND_MAP()

AUIDialog::AUIDialog() : m_StringTable(64), m_AUIObjectTable(16)
{
	m_nFileVersion = 0;
	m_pObjLastCommandGenerator = NULL;
	m_bEnabled = true;
	m_bPopup = false;
	m_bOptimizeResourceLoad = false;
	m_nOptimizeResourceState = 1;

	m_nType = AUIDIALOG_TYPE_DIALOG;
	m_nRenderState = AUIDIALOG_RENDERSTATE_NORMAL;

	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
	m_pUIRenderTarget = NULL;
	m_bImageCoverOrderly = false;
	m_pAUIManager = NULL;
	m_pParentDlgControl = NULL;
	m_bUpdateRenderTarget = false;
	m_bFlushRenderTarget = true;

	m_bAnomaly = false;
	m_nAlpha = 255;
	m_nWholeAlpha = 255;
	m_nFrameMode = 0;
	m_pAUIFrameDlg = NULL;
	m_pAUIFrameTitle = NULL;
	for( int i = 0; i < AUIDIALOG_FRAME_NUM; i++ )
	{
		m_pA2DSprite[i] = NULL;
		m_pt[i].x = m_pt[i].y = 0;
		m_Size[i].cx = m_Size[i].cy = -1;
	}

	m_nListSize = 0;
	m_pListHead = m_pListTail = NULL;

	m_bModal = false;
	m_bShow = false;
	m_bDefaultShow = false;
	m_fScale = 1.0f;
	m_nWidth = 0;
	m_nHeight = 0;
	m_x = 0;
	m_y = 0;
	m_bBackgroundDialog = false;
	m_bCanMove = true;
	m_bCanChangeZOrder = true;
	m_bCapture = false;
	m_bNoFlush = false;
	m_bCanOutOfWindow = false;
	m_bCanEscClose =true;
	m_ptLButtonDown.x = -65535;
	m_ptLButtonDown.y = -65535;
	m_nState = AUIDIALOG_CLOSE_NORMAL;
	m_bDestroyFlag = false;
	m_nBlinkInterval = 0;
	m_dwLife = 0xFFFFFFFF;
	m_dwLifeStart = GetTickCount();
	m_nHintDelay = AUIDIALOG_HINT_DELAY;

	m_nShadow = 0;
	m_nOutline = 0;
	m_bBold = false;
	m_bItalic = false;
	m_pFont = NULL;
	m_nFontHeight = 10;
	m_fDisplayHeight = 10;
	m_color = A3DCOLORRGB(96, 96, 96);

	char szName[80];
	sprintf(szName, "Dialog_%d", rand() * rand());
	m_szName = szName;
	m_szFilename = "";
	m_strSound = "";
	m_strSoundClose = "";

	m_nCurSel = 0;
	m_nMaxMenuTextWidth = 0;
	m_nMaxMenuTextHeight = 0;
	m_pA2DSpriteHilight = NULL;

	m_dwData = 0;
	m_pvData = NULL;
	m_strDataName = "";
	m_strDataPtrName = "";
	m_uiData64 = 0;
	m_uiDataPtr64 = 0;
	m_strData64Name = "";
	m_strDataPtr64Name = "";

	m_pObjFocus = NULL;
	m_pObjEvent = NULL;
	m_pObjCapture = NULL;
	
	m_fPercentX = -2;
	m_fPercentY = -2;
	m_nDefaultWidth = 0;
	m_nDefaultHeight = 0;

	m_nMinDistDlgLToScrR = 1;
	m_nMinDistDlgRToScrL = 1;
	m_nMinDistDlgBToScrT = 1;
	m_nMinDistDlgTToScrB = 1;

	m_nCurImageCover = 0;

#ifdef _ANGELICA_AUDIO
	m_pAudio = NULL;
	m_pAudioClose = NULL;
#endif
}

AUIDialog::~AUIDialog()
{
}

bool AUIDialog::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName)
{
	bool bval = true;
	AScriptFile ASF;

	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;

	if( stricmp(pszTempName + strlen(pszTempName) - 4, ".xml") == 0 )
		return InitFromXML(pszTempName);

	// Import string table.
	AString strTabFile("Interfaces\\");
	strTabFile += pszTempName;

	m_szFilename = pszTempName;

	strTabFile.MakeLower();
	char *pch = (char*)strstr(strTabFile, ".");
	ASSERT(pch);
	strcpy(pch, ".stf");	// String table file.

	bval = ImportStringTable(strTabFile);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call ImportStringTable()");
	
	AUIDIALOG_STRINGTABLE::pair_type p = m_StringTable.get(-1);
	if( p.second )
		m_nFileVersion = a_atoi(*p.first);

	// Construct dialog.
	AString strTempName("Interfaces\\");
	strTempName += pszTempName;
	bval = ASF.Open(strTempName);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to open file %s", pszTempName);

	char szToken[AFILE_LINEMAXLEN];
	while( !ASF.IsEnd() )
	{
		bval = ASF.GetNextToken(true);
		if( !bval ) break;		// End of file.

		AScriptFile *pASF = &ASF;
		strcpy(szToken, ASF.m_szToken);
		if( 0 == strcmpi(szToken, "DIALOG") ||
			0 == strcmpi(szToken, "POPUP") )
		{
			bval = InitFrame(pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call InitFrame()");

			if( 0 == strcmpi(szToken, "POPUP") )
			{
				bval = ImportMenuItems(pASF);
				if( !bval ) return AUI_ReportError("AUIDialog::Init(), Failed to call ImportMenuItems()");
				break;		// Single menu items file.
			}
		}
		else if( 0 == strcmpi(szToken, "CHECK") )
		{
			PAUICHECKBOX pObject = new AUICheckBox;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUICheckBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "COMBO") )
		{
			PAUICOMBOBOX pObject = new AUIComboBox;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIComboBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "CONSOLE") )
		{
			PAUICONSOLE pObject = new AUIConsole;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIConsole");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "EDIT") )
		{
			PAUIEDITBOX pObject = new AUIEditBox;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIEditBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "IMAGEPICTURE") )
		{
			PAUIIMAGEPICTURE pObject = new AUIImagePicture;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failedto init control AUIImagePicture");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "LABEL") )
		{
			PAUILABEL pObject = new AUILabel;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUILabel");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "LIST") )
		{
			PAUILISTBOX pObject = new AUIListBox;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIListbox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "MODELPICTURE") )
		{
			PAUIMODELPICTURE pObject = new AUIModelPicture;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIModelPicture");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "PROGRESS") )
		{
			PAUIPROGRESS pObject = new AUIProgress;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIProgress");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "RADIO") )
		{
			PAUIRADIOBUTTON pObject = new AUIRadioButton;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failedto init control AUIRadioButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "SCROLL") )
		{
			PAUISCROLL pObject = new AUIScroll;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failedto init control AUIRadioButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "SLIDER") )
		{
			PAUISLIDER pObject = new AUISlider;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUISlider");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "STILLIMAGEBUTTON") )
		{
			PAUISTILLIMAGEBUTTON pObject = new AUIStillImageButton;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIStillImageButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "SUBDIALOG") )
		{
			PAUISUBDIALOG pObject = new AUISubDialog;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIStillImageButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "TEXT") )
		{
			PAUITEXTAREA pObject = new AUITextArea;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUITextArea");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "TREE") )
		{
			PAUITREEVIEW pObject = new AUITreeView;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(pA3DEngine, pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUITreeView");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( 0 == strcmpi(szToken, "WINDOWPICTURE") )
		{
			PAUIWINDOWPICTURE pObject = new AUIWindowPicture;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->Init(m_pA3DEngine, m_pA3DDevice, pASF);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIWindowPicture");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else
			InitCustomizeControl(szToken, pASF);
	}
	
	ASF.Close();
	
	// Import object hint table.
	strTabFile = "Interfaces\\";
	strTabFile += pszTempName;

	strTabFile.MakeLower();
	pch = (char*)strstr(strTabFile, ".");
	ASSERT(pch);
	strcpy(pch, ".htf");	// Object hint file.

	if( af_IsFileExist(strTabFile) )
	{
		bval = ImportHintTable(strTabFile);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call ImportHintTable()");
	}

	UpdateData(false);
	return OnInitDialog();
}

bool AUIDialog::InitCustomizeControl(const char * szObjectType, AScriptFile* pASF)
{
	bool bval = true;
	PAUICUSTOMIZE pObject = new AUICustomize;
	if( !pObject ) return false;
	pObject->SetParent(this);
	
	AUIOBJECT_SETPROPERTY p;
	strcpy(p.c, szObjectType);
	pObject->SetProperty("Object Type", &p);

	bval = pObject->Init(m_pA3DEngine, m_pA3DDevice, pASF);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUICustomize");

	bval = AddObjectToList(pObject);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");

	return true;
}

bool AUIDialog::InitFrame(AScriptFile *pASF)
{
	/*	Dialog header format:
	 *
	 *		DIALOG					Keyword
	 *		Login					Name
	 *		25 25 300 200			x, y, width, height
	 *		9						String ID of title
	 *		10						String ID of font name
	 *		15						Font height
	 *		"DlgFrame.bmp"			Frame image
	 *		"DlgBack.bmp"			Background image
	 *		"DlgTitle.bmp"			Title bar image
	 *		0 0	122 21				Title bar position and size
	 *		"DlgClose.bmp"			Background image
	 *		0 0						Background position
	 *		255 255 255				Title text color, R, G, B
	 *		128						Dialog alpha
	 *		1						Show or not
	 */
	int idString;

	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));
 	if( !SetName(pASF->m_szToken) )
 		return AUI_ReportError(__LINE__, 1, "AUIDialog::InitFrame(), Dialog name conflict!");
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_x = atoi(pASF->m_szToken);
	m_fPercentX = -2;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_y = atoi(pASF->m_szToken);
	m_fPercentY = -2;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nWidth = atoi(pASF->m_szToken);
	m_nDefaultWidth = m_nWidth;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nHeight = atoi(pASF->m_szToken);
	m_nDefaultHeight = m_nHeight;

	CHECK_BREAK2(pASF->GetNextToken(true));
	idString = atoi(pASF->m_szToken);
	if( idString >= 0 )
		m_strTitle = GetStringFromTable(idString);

	CHECK_BREAK2(pASF->GetNextToken(true));
	idString = atoi(pASF->m_szToken);

	ACString strFontName = GetStringFromTable(idString);
	if( strFontName.GetLength() == 0 ) strFontName = _AL("方正细黑一简体");
	CSplit s(strFontName);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("###"));
	strFontName = vec[0];
	
	if( vec.size() > 1 ) m_nShadow = a_atoi(vec[1]);
	if( vec.size() > 2 ) m_nOutline = a_atoi(vec[2]);
	if( vec.size() > 3 ) m_bBold = a_atoi(vec[3]) ? true : false;
	if( vec.size() > 4 ) m_bItalic = a_atoi(vec[4]) ? true : false;

	CHECK_BREAK2(pASF->GetNextToken(true));
	int m_nFontHeight = atoi(pASF->m_szToken);
	if( m_nFontHeight <= 0 || m_nFontHeight > 100 )
		m_nFontHeight = 10;

	CHECK_BREAK2(SetFontAndColor(strFontName, m_nFontHeight,
		m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic));

	bool bAllReadTokenSucceed = true;
	for( int i = AUIDIALOG_FRAME_MIN; i < AUIDIALOG_FRAME_MAX; i++ )
	{
		BEGIN_FAKE_WHILE;

		CHECK_BREAK2(pASF->GetNextToken(true));

		if( strlen(pASF->m_szToken) > 0 )
		{
			if (!InitIndirect(pASF->m_szToken, i))
				return AUI_ReportError(__LINE__, 1, "AUIDialog::InitFrame(), failed to call InitIndirect");
		}

		if( AUIDIALOG_FRAME_TITLE == i || AUIDIALOG_FRAME_CLOSE == i )
		{
			CHECK_BREAK2(pASF->GetNextToken(true));
			m_pt[i].x = atoi(pASF->m_szToken);

			CHECK_BREAK2(pASF->GetNextToken(true));
			m_pt[i].y = atoi(pASF->m_szToken);

			if( AUIDIALOG_FRAME_TITLE == i )
			{
				CHECK_BREAK2(pASF->GetNextToken(true));
				m_Size[i].cx = atoi(pASF->m_szToken);

				CHECK_BREAK2(pASF->GetNextToken(true));
				m_Size[i].cy = atoi(pASF->m_szToken);
			}
		}

		END_FAKE_WHILE;

		BEGIN_ON_FAIL_FAKE_WHILE;

		bAllReadTokenSucceed = false;
		break;

		END_ON_FAIL_FAKE_WHILE;
	}
	CHECK_BREAK(bAllReadTokenSucceed);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int r = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int g = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int b = atoi(pASF->m_szToken);

	m_color = A3DCOLORRGB(r, g, b);

	CHECK_BREAK2(pASF->GetNextToken(true));
	SetAlpha(atoi(pASF->m_szToken));

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_bShow = atoi(pASF->m_szToken) ? true : false;

	if( GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}
	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIDialog::InitFrame(), Failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIDialog::InitFrameXML(AXMLItem *pItem)
{
	bool bBackgroundDialog = m_bBackgroundDialog;
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("Anomaly"), m_bAnomaly)
	XMLGET_INT_VALUE(pItem, _AL("Alpha"), m_nAlpha)
	XMLGET_BOOL_VALUE(pItem, _AL("BackgroundDialog"), bBackgroundDialog)
	XMLGET_BOOL_VALUE(pItem, _AL("CanOutOfWindow"), m_bCanOutOfWindow)
	XMLGET_INT_VALUE(pItem, _AL("MinDistDlgRToScrL"), m_nMinDistDlgRToScrL)
	XMLGET_INT_VALUE(pItem, _AL("MinDistDlgLToScrR"), m_nMinDistDlgLToScrR)
	XMLGET_INT_VALUE(pItem, _AL("MinDistDlgBToScrT"), m_nMinDistDlgBToScrT)
	XMLGET_INT_VALUE(pItem, _AL("MinDistDlgTToScrB"), m_nMinDistDlgTToScrB)
	XMLGET_BOOL_VALUE(pItem, _AL("CanMove"), m_bCanMove)
	XMLGET_BOOL_VALUE(pItem, _AL("CanChangeZOrder"), m_bCanChangeZOrder)
	XMLGET_BOOL_VALUE(pItem, _AL("CanEscClose"), m_bCanEscClose)
	XMLGET_BOOL_VALUE(pItem, _AL("PopupDialog"), m_bPopup)
	XMLGET_BOOL_VALUE(pItem, _AL("ImageCoverOrderly"), m_bImageCoverOrderly)
	XMLGET_INT_VALUE(pItem, _AL("HintDelay"), m_nHintDelay)
	XMLGET_STRING_VALUE(pItem, _AL("SoundEffect"), m_strSound)
	XMLGET_STRING_VALUE(pItem, _AL("SoundEffectClose"), m_strSoundClose)
#ifdef _ANGELICA_AUDIO
	m_pAUIManager->LoadAudioEvtInstance(m_pAudio, m_strSound);
	m_pAUIManager->LoadAudioEvtInstance(m_pAudioClose, m_strSoundClose);
#endif

	bool bRenderTarget = false;
	XMLGET_BOOL_VALUE(pItem, _AL("StaticRender"), bRenderTarget)
	SetRenderTarget(bRenderTarget);

 	if( !SetName(m_szName) )
 		return AUI_ReportError(__LINE__, 1, "AUIDialog::InitFrame(), Dialog name conflict!");
	
	SetBackgroundDialog(bBackgroundDialog);

	m_fPercentX = -2;
	m_fPercentY = -2;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0)
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIDIALOG_FRAME_FRAME);
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		pChildItem = pChildItem->GetNextItem();
	}

	return true;
}

bool AUIDialog::ImportMenuItems(AScriptFile *pASF)
{
	int idString;
	MENU_ITEM mi;

	while( !pASF->IsEnd() )
	{
		if (!pASF->GetNextToken(true))	// Keyword, ignore.
			break;						// End of file.


		BEGIN_FAKE_WHILE2;

		if( 0 == strcmpi(pASF->m_szToken, "MENUITEM") )
		{
			CHECK_BREAK2(pASF->GetNextToken(true));
			idString = atoi(pASF->m_szToken);
			mi.strMenuText = GetStringFromTable(idString);

			A3DPOINT2 ptSize;
			int nTextW = 0, nTextH = 0;
			A3DFTFont *pFont = m_pFont ? m_pFont : m_pAUIManager->GetDefaultFont();
			ptSize = pFont->GetTextExtent(GetStringFromTable(2) + mi.strMenuText);
			nTextW = ptSize.x;
			nTextH = ptSize.y;

			m_nMaxMenuTextWidth = max(m_nMaxMenuTextWidth, nTextW);
			m_nMaxMenuTextHeight = max(m_nMaxMenuTextHeight, nTextH);

			CHECK_BREAK2(pASF->GetNextToken(true));
			mi.strCommand = pASF->m_szToken;

			mi.bCheck = false;
			mi.bEnable = true;
			m_MenuItem.push_back(mi);
		}
		else if( 0 == strcmpi(pASF->m_szToken, "HILIGHT") )
		{
			CHECK_BREAK2(pASF->GetNextToken(true));

			m_pA2DSpriteHilight = new A2DSprite;
			if( !m_pA2DSpriteHilight ) 
				return AUI_ReportError(__LINE__, 1, "AUIDialog::ImportMenuItems(), no enough memory");
#ifdef _ANGELICA22
			//d3d9ex
			m_pA2DSpriteHilight->SetDynamicTex(true);
#endif //_ANGELICA22

			CHECK_BREAK2(m_pA2DSpriteHilight->Init(m_pA3DDevice, pASF->m_szToken, AUI_COLORKEY));
		}
		else
			pASF->SkipLine();

		END_FAKE_WHILE2;

		BEGIN_ON_FAIL_FAKE_WHILE2;

		return AUI_ReportError(DEFAULT_2_PARAM, "AUIDialog::ImportMenuItems(), Failed to read from file");

		END_ON_FAIL_FAKE_WHILE2;
	}

	m_nType = AUIDIALOG_TYPE_POPUPMENU;
	m_nMaxMenuTextWidth += m_nFontHeight;

	ASSERT(m_pA2DSpriteHilight);
	float fScaleX = float(m_nMaxMenuTextWidth) / float(m_pA2DSpriteHilight->GetWidth());
	float fScaleY = float(m_nMaxMenuTextHeight) / float(m_pA2DSpriteHilight->GetHeight());
	m_pA2DSpriteHilight->SetScaleX(fScaleX);
	m_pA2DSpriteHilight->SetScaleY(fScaleY);

	SIZE s = { 0, 0 };
	if( m_pAUIFrameDlg )
		s = m_pAUIFrameDlg->GetSizeLimit();
	m_nDefaultWidth = m_nWidth = s.cx + m_nMaxMenuTextWidth;
	m_nDefaultHeight = m_nHeight = s.cy + m_nMaxMenuTextHeight * m_MenuItem.size();

	return true;
}

bool AUIDialog::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUIDIALOG_FRAME_MIN && idType < AUIDIALOG_FRAME_MAX);

	if( idType == AUIDIALOG_FRAME_FRAME ||
		idType == AUIDIALOG_FRAME_TITLE )
	{
		UpdateRenderTarget();
		PAUIFRAME pAUIFrame;
		if( AUIDIALOG_FRAME_FRAME == idType )
		{
			A3DRELEASE(m_pAUIFrameDlg);
		}
		else
		{
			A3DRELEASE(m_pAUIFrameTitle);
		}
		if( strlen(pszFileName) <= 0 ) return true;

		pAUIFrame = new AUIFrame;
		if( !pAUIFrame ) return false;

		bval = pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(pAUIFrame)
			return AUI_ReportError(__LINE__, 1, "AUIDialog::InitIndirect(), Failed to call pAUIFrame->Init()");
		}

		if( AUIDIALOG_FRAME_FRAME == idType )
			m_pAUIFrameDlg = pAUIFrame;
		else
			m_pAUIFrameTitle = pAUIFrame;
		if( m_pAUIManager )
			pAUIFrame->SetScale(m_pAUIManager->GetWindowScale() * m_fScale);
	}
	else
	{
		A3DRELEASE(m_pA2DSprite[idType]);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pA2DSprite[idType] = new A2DSprite;
		if( !m_pA2DSprite[idType] ) return false;
		
#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSprite[idType]->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSprite[idType]->Init(m_pA3DDevice,	pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSprite[idType])
			return AUI_ReportError(__LINE__, 1, "AUIDialog::InitIndirect(), Failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUIDIALOG_FRAME_CLOSE == idType )
		{
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight() / 3;
			A3DRECT a_rc[3];	// Normal, onhover, click.
			for( int i = 0; i < 3; i++ )
				a_rc[i].SetRect(0, H * i, W, H * i + H);
			bval = m_pA2DSprite[idType]->ResetItems(3, a_rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::InitIndirect(), Failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
	}

	return true;
}

bool AUIDialog::Release(void)
{
	int i;
	PAUIOBJECTLISTELEMENT pElement, pDeleted;

	if( m_pParentDlgControl && m_pParentDlgControl->GetParent() )
		m_pParentDlgControl->RemoveDialog();

	m_AUIObjectTable.clear();
	pElement = m_pListHead;
	m_pListHead = m_pListTail = NULL;
	while( pElement )
	{
		if (pElement->bHeapAlloc)
			A3DRELEASE(pElement->pThis);

		pDeleted = pElement;
		pElement = pElement->pNext;
		delete pDeleted;
	}

	A3DRELEASE(m_pAUIFrameDlg);
	A3DRELEASE(m_pAUIFrameTitle);
	for( i = 0; i < AUIDIALOG_FRAME_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	A3DRELEASE(m_pA2DSpriteHilight);
	
	for( i = 0; i < (int)m_vecFrame.size(); i++ )
		A3DRELEASE(m_vecFrame[i].pA3DViewport);
	m_vecFrame.clear();

	A3DRELEASE(m_pUIRenderTarget)

#ifdef _ANGELICA_AUDIO
	//m_pAUIManager->DestroyAudioEvtInstance(m_pAudio);
	//m_pAUIManager->DestroyAudioEvtInstance(m_pAudioClose);
#endif

	return true;
}

bool AUIDialog::Save(const char *pszFilename)
{
	m_szFilename = pszFilename;

	int len = strlen(pszFilename);
	if( len >= 4 && stricmp(pszFilename + len - 4, ".xml") == 0 )
	{
		AXMLFile aXMLFile;
		
		AXMLItem *pXMLItemDialog = new AXMLItem;
		pXMLItemDialog->InsertItem(aXMLFile.GetRootItem());
		pXMLItemDialog->SetName(_AL("DIALOG"));
		XMLSET_STRING_VALUE(pXMLItemDialog, _AL("Name"), AS2AC(m_szName), _AL(""))
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("x"), m_x, 0)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("y"), m_y, 0)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("Width"), m_nWidth, 0)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("Height"), m_nHeight, 0)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("Anomaly"), m_bAnomaly, false)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("Alpha"), m_nAlpha, 255)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("BackgroundDialog"), m_bBackgroundDialog, false)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("CanOutOfWindow"), m_bCanOutOfWindow, false)
		
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("MinDistDlgRToScrL"), m_nMinDistDlgRToScrL, 1)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("MinDistDlgLToScrR"), m_nMinDistDlgLToScrR, 1)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("MinDistDlgBToScrT"), m_nMinDistDlgBToScrT, 1)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("MinDistDlgTToScrB"), m_nMinDistDlgTToScrB, 1)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("CanMove"), m_bCanMove, true)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("CanChangeZOrder"), m_bCanChangeZOrder, true)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("CanEscClose"), m_bCanEscClose, true)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("PopupDialog"), m_bPopup, false)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("ImageCoverOrderly"), m_bImageCoverOrderly, false)
		XMLSET_BOOL_VALUE(pXMLItemDialog, _AL("StaticRender"), m_pUIRenderTarget != NULL, false)
		XMLSET_INT_VALUE(pXMLItemDialog, _AL("HintDelay"), m_nHintDelay, AUIDIALOG_HINT_DELAY)
		XMLSET_STRING_VALUE(pXMLItemDialog, _AL("SoundEffect"), AS2AC(m_strSound), _AL(""))
		XMLSET_STRING_VALUE(pXMLItemDialog, _AL("SoundEffectClose"), AS2AC(m_strSoundClose), _AL(""))
		
		AXMLItem *pXMLItemResource = new AXMLItem;
		pXMLItemResource->InsertItem(pXMLItemDialog);
		pXMLItemResource->SetName(_AL("Resource"));
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		if( m_pAUIFrameDlg )
		{
			XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrameDlg->GetA2DSprite()->GetName()), _AL(""))
			XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
		}
		
		PAUIOBJECTLISTELEMENT pElement = m_pListHead;
		AUIDIALOG_AUIOBJECTTABLE pTable(16);
		while( pElement )
		{
			if( !pTable.put(pElement->pThis->GetName(), pElement->pThis) )
			{
				AString strText;
				strText.Format("Repeated object name '%s'!", pElement->pThis->GetName());
				::MessageBoxA(NULL, strText, "", MB_OK);
				aXMLFile.Release();
				return false;
			}
			
			bool bval = pElement->pThis->SaveXML(pXMLItemDialog);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Save(), Failed to call pElement->pThis->Save()");
			pElement = pElement->pNext;
		}

		if(!aXMLFile.SaveToFile(pszFilename))
		{
			aXMLFile.Release();
			return false;
		}
		aXMLFile.Release();
		return true;
	}

	m_StringTable.clear();

	FILE *file = fopen(pszFilename, "wt");
	if( !file ) return false;

	int idStrTitle = AddStringToTable(m_strTitle);
	if( idStrTitle < 0 ) 
	{
		fclose(file);
		return false;
	}

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_strFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idStrFont = AddStringToTable(szFontName);
	if( idStrFont < 0 ) 
	{
		fclose(file);
		return false;
	}

	fprintf(file, "DIALOG \"%s\" %d %d %d %d %d %d %d", m_szName, m_x,
		m_y, m_nWidth, m_nHeight, idStrTitle, idStrFont, m_nFontHeight);

	for( int i = 0; i < AUIDIALOG_FRAME_NUM; i++ )
	{
		if( AUIDIALOG_FRAME_FRAME == i )
		{
			if( m_pAUIFrameDlg )
				m_pAUIFrameDlg->Save(file);
			else
				fprintf(file, " \"\"");
		}
		else if( AUIDIALOG_FRAME_TITLE == i )
		{
			if( m_pAUIFrameTitle )
				m_pAUIFrameTitle->Save(file);
			else
				fprintf(file, " \"\"");
			fprintf(file, " %d %d %d %d", m_pt[i].x,
				m_pt[i].y, m_Size[i].cx, m_Size[i].cy);
		}
		else
		{
			if( m_pA2DSprite[i] )
				fprintf(file, " \"%s\"", m_pA2DSprite[i]->GetName());
			else
				fprintf(file, " \"\"");
			
			if( AUIDIALOG_FRAME_CLOSE == i )
				fprintf(file, " %d %d", m_pt[i].x, m_pt[i].y);
		}
	}

	fprintf(file, " %d %d %d %d %d %d\n", A3DCOLOR_GETRED(m_color),
		A3DCOLOR_GETGREEN(m_color), A3DCOLOR_GETBLUE(m_color), m_nAlpha, m_bShow, m_nFrameMode);

	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->GetType() != AUIOBJECT_TYPE_COMBOBOX )
		{
			bool bval = pElement->pThis->Save(file);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Save(), Failed to call pElement->pThis->Save()");
		}
		pElement = pElement->pNext;
	}

	pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->GetType() == AUIOBJECT_TYPE_COMBOBOX )
		{
			bool bval = pElement->pThis->Save(file);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Save(), Failed to call pElement->pThis->Save()");
		}
		pElement = pElement->pNext;
	}

	fclose(file);

	char szSTFFile[MAX_PATH];
	strcpy(szSTFFile, pszFilename);
	char *pch = strstr(szSTFFile, ".");
	ASSERT(pch);
	strcpy(pch, ".stf");

	bool bval = ExportStringTable(szSTFFile);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Save(), Failed to call ExportStringTable()");

	return true;
}

void AUIDialog::CheckRadioButton(int idGroup, int idCheck)
{
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->GetType() == AUIOBJECT_TYPE_RADIOBUTTON )
		{
			PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)pElement->pThis;
			int idGroupThis = pRadio->GetGroupID();
			if( idGroupThis == idGroup )
			{
				int idCheckThis = pRadio->GetButtonID();
				if( idCheckThis == idCheck )
					pRadio->Check(true);
				else
					pRadio->Check(false);
			}
		}
		pElement = pElement->pNext;
	}
}

int AUIDialog::GetCheckedRadioButton(int idGroup)
{
	AUIDIALOG_AUIOBJECTTABLE::iterator iter = m_AUIObjectTable.begin();
	for (; iter != m_AUIObjectTable.end(); ++iter)
	{
		AUIRadioButton *pControl = dynamic_cast<AUIRadioButton *>(*iter.value());
		if (pControl &&
			pControl->GetGroupID() == idGroup &&
			pControl->IsChecked())
			return pControl->GetButtonID();
	}
	return -1;
}

bool AUIDialog::OnCommand(const char *pszCommand)
{
	ASSERT(m_pAUIManager);

	// Do not deal with empty command.
	if( strlen(pszCommand) <= 0 )
		return true;
	else
	{
		if( AUIDIALOG_TYPE_MESSAGEBOX == m_nType )
		{
			int nRetVal = IDOK;
			if( 0 == strcmpi(pszCommand, "IDCANCEL") )
				nRetVal = IDCANCEL;
			else if( 0 == strcmpi(pszCommand, "IDYES") )
				nRetVal = IDYES;
			else if( 0 == strcmpi(pszCommand, "IDNO") )
				nRetVal = IDNO;
			return m_pAUIManager->OnMessageBox(nRetVal, this);
		}
		else
			return m_pAUIManager->OnCommand(pszCommand, this);
	}
}

bool AUIDialog::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool bKeyboard)
{
	bool bval, bRet = false;

	if( AUIDIALOG_TYPE_POPUPMENU == m_nType )
		return DealWindowsPopupMenuMessage(uMsg, wParam, lParam);
	else if( bKeyboard )
	{
		m_strCommand = "";
		if( m_pObjFocus && m_pObjFocus->IsEnabled() )
			bRet = m_pObjFocus->OnDlgItemMessage(uMsg, wParam, lParam);

		if( m_strCommand.GetLength() > 0 )
		{
			bRet = OnCommand(m_strCommand);
			bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam, this, m_pObjFocus);
		}
		else 
		{
			bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam, this, m_pObjFocus);
			if( !bRet && WM_KEYDOWN == uMsg )
			{
				int nVKey = int(wParam);
				if( VK_TAB == nVKey )
				{
					if( m_pObjFocus && m_pObjFocus->GetType() == AUIOBJECT_TYPE_EDITBOX )
					{
						if( AUI_PRESS(VK_SHIFT) )
							TabStop(m_pObjFocus, -1);
						else
							TabStop(m_pObjFocus, 1);
						bRet = true;
					}
				}
				else if( VK_ESCAPE == nVKey )
				{
					if( m_pParentDlgControl && m_pParentDlgControl->GetParent() )
						bRet = m_pParentDlgControl->GetParent()->DealWindowsMessage(uMsg, wParam, lParam, bKeyboard);
					else if( m_bCanEscClose )
						bRet = OnCommand("IDCANCEL");
				}
				else if( VK_RETURN == nVKey )
				{
					if( m_pObjFocus && m_pObjFocus->GetType() == AUIOBJECT_TYPE_EDITBOX )
						bRet = true;
					else
						bRet = OnCommand("IDOK");
				}
			}
		}

		return bRet;
	}
	else
	{
		if( m_bCapture )
		{
			bRet = DealWindowsMouseMessage(uMsg, wParam, lParam);
			bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam,
				this, m_pObjEvent ? m_pObjEvent : m_pObjCapture);
			return bRet;
		}

		A3DRECT rcWindow = m_pAUIManager->GetRect();
		int x = GET_X_LPARAM(lParam) - rcWindow.left;
		int y = GET_Y_LPARAM(lParam) - rcWindow.top;
		
		// Mouse position of WM_MOUSEWHEEL is screen coordinate, not client.
		if( WM_MOUSEWHEEL == uMsg )
		{
			POINT ptClient = { 0, 0 };
			ClientToScreen(m_pAUIManager->GetHWnd() ? m_pAUIManager->GetHWnd() : m_pA3DDevice->GetDeviceWnd(), &ptClient);
			x -= ptClient.x;
			y -= ptClient.y;
		}

		m_pObjEvent = NULL;
		if( m_pObjFocus && m_pObjFocus->IsShow() ||
			m_pObjCapture && m_pObjCapture->IsShow() )
		{
			if( m_pObjCapture || m_pObjFocus->IsInHitArea(x, y, uMsg) )
			{
				PAUIOBJECT pObj = m_pObjCapture ? m_pObjCapture : m_pObjFocus;

				if( pObj->IsTransparent() )
					return false;
				if( pObj->AcceptMouseMessage() )
				{
					if( !pObj->IsEnabled() )
						return true;
					m_strCommand = "";
					bRet = pObj->OnDlgItemMessage(uMsg, wParam, lParam);
					if( m_strCommand.GetLength() > 0 )
						OnCommand(m_strCommand);
					bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam, this, pObj);
					return true;
				}
			}
		}

		bool bClip;
		PAUIOBJECT pObj;
		A3DRECT rc, rcClip;
		A3DVIEWPORTPARAM *param;
		PAUIOBJECTLISTELEMENT pElement = m_pListTail;
		while( pElement )
		{
			pObj = pElement->pThis;
			if( pObj->IsShow() && pObj != m_pObjFocus )
			{
				bClip = false;
				rc = pObj->GetRect();
				if( pObj->GetClipViewport() )
				{
					param = pObj->GetClipViewport()->GetParam();
					rcClip.left = param->X - rcWindow.left;
					rcClip.top = param->Y - rcWindow.top;
					rcClip.right = rcClip.left + param->Width;
					rcClip.bottom = rcClip.top + param->Height;
					bClip = !rcClip.PtInRect(x, y);
				}
				if( pObj->IsInHitArea(x, y, uMsg) && !bClip && !pObj->IsTransparent() )
				{
					if( pObj->AcceptMouseMessage() && pObj->IsEnabled() )
					{
						if( m_pObjFocus != pObj && uMsg != WM_MOUSEMOVE && uMsg != WM_MOUSEWHEEL && 
							uMsg != WM_LBUTTONUP && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONUP)
							ChangeFocus(pObj);
						
						m_ptLButtonDown.x = m_ptLButtonDown.y = -65535;
						m_strCommand = "";
						bRet = pObj->OnDlgItemMessage(uMsg, wParam, lParam);
						if( m_strCommand.GetLength() > 0 )
							OnCommand(m_strCommand);
						bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam, this, pObj);
						return true;
					}
					if( !m_pObjEvent ) m_pObjEvent = pObj;
				}
			}

			pElement = pElement->pLast;
		}

		if( WM_LBUTTONDOWN == uMsg || WM_LBUTTONUP == uMsg ||
			(WM_MOUSEMOVE == uMsg && (wParam & MK_LBUTTON)) )
		{
			ChangeFocus(NULL);
		}

		bRet = DealWindowsMouseMessage(uMsg, wParam, lParam);
		bval = m_pAUIManager->OnEvent(uMsg, wParam, lParam, this, m_pObjEvent);
		return bRet;
	}

	return false;
}

bool AUIDialog::TabStop(PAUIOBJECT pAUIObject, int nDir)
{
	if( !m_pListHead || !m_pListTail ) return true;

	if( !pAUIObject ) pAUIObject = m_pListHead->pThis;

	// Find element in list.
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pAUIObject == pElement->pThis )
			break;
		pElement = pElement->pNext;
	}

	if( !pElement )
	{
		pElement = m_pListHead;
		pAUIObject = m_pListHead->pThis;
	}

	if( nDir > 0 )
	{
		pElement = pElement->pNext;
		if( !pElement ) pElement = m_pListHead;
		while( pElement && pElement->pThis != pAUIObject )
		{
			if( pElement->pThis->AcceptMouseMessage() &&
				pElement->pThis->GetType() == AUIOBJECT_TYPE_EDITBOX )
			{
				ChangeFocus(pElement->pThis);
				break;
			}
			pElement = pElement->pNext;
			if( !pElement ) pElement = m_pListHead;
		}
	}
	else if( nDir < 0 )
	{
		pElement = pElement->pLast;
		if( !pElement ) pElement = m_pListTail;
		while( pElement && pElement->pThis != pAUIObject )
		{
			if( pElement->pThis->AcceptMouseMessage() &&
				pElement->pThis->GetType() == AUIOBJECT_TYPE_EDITBOX )
			{
				ChangeFocus(pElement->pThis);
				break;
			}
			pElement = pElement->pLast;
			if( !pElement ) pElement = m_pListTail;
		}
	}

	return true;
}

bool AUIDialog::DealWindowsMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;

	A3DRECT rcWindow = m_pAUIManager->GetRect();
	int x = GET_X_LPARAM(lParam) - rcWindow.left;
	int y = GET_Y_LPARAM(lParam) - rcWindow.top;
	A3DRECT rcClose(0, 0, 0, 0), rcItem(0, 0, 0, 0);
	if( m_pA2DSprite[AUIDIALOG_FRAME_CLOSE] )
		rcItem = m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->GetItem(0)->GetRect();
	rcItem.SetRect(rcItem.left * fWindowScale, rcItem.top * fWindowScale,
		rcItem.right * fWindowScale, rcItem.bottom * fWindowScale);
	rcClose.left = m_x + m_nWidth - m_pt[AUIDIALOG_FRAME_CLOSE].x * fWindowScale;
	rcClose.top = m_y + m_pt[AUIDIALOG_FRAME_CLOSE].y * fWindowScale;
	rcClose.right = rcClose.left + rcItem.Width();
	rcClose.bottom = rcClose.top + rcItem.Height();

	if( WM_LBUTTONDOWN == uMsg )
	{
		if( rcClose.PtInRect(x, y) )
			m_nState = AUIDIALOG_CLOSE_CLICK;
		else if( m_bCanMove )
		{
			PAUIDIALOG pDlg = this;
			while( pDlg->GetParentDlgControl() )
				pDlg = pDlg->GetParentDlgControl()->GetParent();

			m_pAUIManager->BringWindowToTop(pDlg);
			pDlg->SetCapture(true, x, y);
		}
	}
	else if( WM_MOUSEMOVE == uMsg )
	{
		if( m_ptLButtonDown.x > -65535 && m_ptLButtonDown.y > -65535 )
		{
			A3DRECT rcWindow = m_pAUIManager->GetRect();

			if (m_bCanMove)
			{	
				int iFitX = x;
				a_Clamp(iFitX, 0, rcWindow.Width());
				int iLastX = m_ptLButtonDown.x;
				a_Clamp(iLastX, 0, rcWindow.Width());
				m_x += iFitX - iLastX;

				int iFitY = y;
				a_Clamp(iFitY, 0, rcWindow.Height());
				int iLastY = m_ptLButtonDown.y;
				a_Clamp(iLastY, 0, rcWindow.Height());
				m_y += iFitY - iLastY;
					
				AdjustFromXY(rcWindow.Width(), rcWindow.Height());		
			}

			m_ptLButtonDown.x = x;
			m_ptLButtonDown.y = y;
		}
		else if( rcClose.PtInRect(x, y) )
		{
			if( AUIDIALOG_CLOSE_CLICK != m_nState )
				m_nState = AUIDIALOG_CLOSE_ONHOVER;
		}
		else
			m_nState = AUIDIALOG_CLOSE_NORMAL;
	}
	else if( WM_LBUTTONUP == uMsg )
	{
		PAUIDIALOG pDlg = this;
		while( pDlg->GetParentDlgControl() )
			pDlg = pDlg->GetParentDlgControl()->GetParent();
		pDlg->SetCapture(false);

		if( rcClose.PtInRect(x, y) && AUIDIALOG_CLOSE_CLICK == m_nState )
			OnCommand("IDCANCEL");
		m_nState = AUIDIALOG_CLOSE_NORMAL;
	}

	return true;
}

bool AUIDialog::DealWindowsPopupMenuMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( WM_LBUTTONDOWN == uMsg )
		;
	else if( WM_MOUSEMOVE == uMsg || WM_LBUTTONUP == uMsg )
	{
		A3DRECT rc(0, 0, 0, 0);
		if( m_pAUIFrameDlg && m_pAUIFrameDlg->GetA2DSprite()->GetItem(AUIFRAME_RECT_UPPERLEFT) )
			rc = m_pAUIFrameDlg->GetA2DSprite()->GetItem(AUIFRAME_RECT_UPPERLEFT)->GetRect();
		A3DRECT rcWindow = m_pAUIManager->GetRect();
		int y = GET_Y_LPARAM(lParam) - m_y - rc.Height() - rcWindow.top;
		int nSel = y / (m_nMaxMenuTextHeight * m_pAUIManager->GetWindowScale() * m_fScale);

		if( nSel < 0 )
			nSel = 0;
		else if( nSel > (int)m_MenuItem.size() - 1 )
			nSel = m_MenuItem.size() - 1;

		if( WM_MOUSEMOVE == uMsg )
		{
			if( m_MenuItem[nSel].strCommand.GetLength() > 0 )
				m_nCurSel = nSel;
		}
		else if( WM_LBUTTONUP == uMsg && m_MenuItem[nSel].bEnable )
		{
			OnCommand(m_MenuItem[nSel].strCommand);
			Show(false);
		}

		return true;
	}
	else if( WM_KEYDOWN == uMsg && VK_ESCAPE == int(wParam) )
	{
		Show(false);
	}
	else
		return false;

	return true;
}		

POINT AUIDialog::GetPos(void)
{
	POINT ptPos;
	ptPos.x = m_x;
	ptPos.y = m_y;
	return ptPos;
}

APointF AUIDialog::GetPercentPos(void)
{
	APointF ptPos;
	ptPos.x = m_fPercentX;
	ptPos.y = m_fPercentY;
	return ptPos;
}

void AUIDialog::SetPosEx(int x, int y, int alignModeX, int alignModeY, AUIDialog *pParentDlg)
{
	// get parent central point
	POINT parentPos;
	SIZE parentSize;

	A3DRECT rcWindow = m_pAUIManager->GetRect();
	if (pParentDlg)
	{
		parentPos = pParentDlg->GetPos();
		parentSize = pParentDlg->GetSize();
	}
	else
	{
		parentPos.x = parentPos.y = 0;
		
		parentSize.cx = rcWindow.Width();
		parentSize.cy = rcWindow.Height();
	}

	// compute new pos of this dialog
	SIZE thisSize = GetSize();
	switch(alignModeX) 
	{
	case alignNone:
		m_x = x;
		break;
	case alignMin:
		m_x = parentPos.x;
		break;
	case alignMax:
		m_x = parentPos.x + parentSize.cx - thisSize.cx;
		break;
	case alignCenter:
		m_x = parentPos.x + parentSize.cx / 2 - thisSize.cx / 2;
		break;
	default:
		ASSERT(!"invalid value for alignModeX");
		break;
	}

	switch(alignModeY) 
	{
	case alignNone:
		m_y = y;
		break;
	case alignMin:
		m_y = parentPos.y;
		break;
	case alignMax:
		m_y = parentPos.y + parentSize.cy - thisSize.cy;
		break;
	case alignCenter:
		m_y = parentPos.y + parentSize.cy / 2 - thisSize.cy / 2;
		break;
	default:
		ASSERT(!"invalid value for alignModeX");
		break;
	}

	AdjustFromXY(rcWindow.Width(), rcWindow.Height());
}

void AUIDialog::SetPercentPosEx(float x, float y)
{
	A3DDEVFMT fmt = m_pA3DEngine->GetA3DDevice()->GetDevFormat();

	m_fPercentX = x;
	m_fPercentY = y;
	AdjustFromPercent(fmt.nWidth, fmt.nHeight);
	AdjustFromXY(fmt.nWidth, fmt.nHeight);
}

void AUIDialog::TranslatePosForAlign(int &x, int &y)
{
	SIZE s = GetSize();
	A3DRECT rcWindow = m_pAUIManager->GetRect();

	if( -1 == x )			// -1 means center.
		x = (int)(rcWindow.Width() - s.cx) / 2;
	else if( x < -1 )		// Right align.
		x += (int)rcWindow.Width() - (int)s.cx + 2;

	if( -1 == y )			// -1 means center.
		y = (int)(rcWindow.Height() - s.cy) / 2;
	else if( y < -1 )		// Bottom align.
		y += (int)rcWindow.Height() - (int)s.cy + 2;
}

bool AUIDialog::AddObjectToList(PAUIOBJECT pAUIObject)
{
	return AddObjectToList(pAUIObject, true);
}

bool AUIDialog::SetName(const char *pszName)
{
 	if( m_pAUIManager->m_DlgName[pszName] )
 		return false;

	m_pAUIManager->m_DlgName.erase(m_szName);
	m_szName = pszName;
	m_pAUIManager->m_DlgName[m_szName] = this;

	return true;
}


void AUIDialog::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSpriteHilight)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteHilight, bLoad);

	for (int i = 0; i < AUIDIALOG_FRAME_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrameDlg)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameDlg->GetA2DSprite(), bLoad);

	if (m_pAUIFrameTitle)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameTitle->GetA2DSprite(), bLoad);

	PAUIOBJECTLISTELEMENT pElement;
	pElement = m_pListHead;

	while (pElement)
	{
		pElement->pThis->OptimizeSpriteResource(bLoad);
		pElement = pElement->pNext;
	}
}

void AUIDialog::Show(bool bShow, bool bModal, bool bActive)
{
	if (IsOptimizeResourceLoad())
	{
		if (bShow)
			OptimizeSpriteResource(bShow);
	}

	m_nCurSel = 0;
	m_bModal = bModal;
	if( !bShow && !m_bShow ) return;

	bool bChangeState = (bShow != m_bShow);

	if( bShow )
	{
		if( bModal || bActive )
		{
			if( !IsCapture() && m_pAUIManager->m_pDlgActive )
				m_pAUIManager->m_pDlgActive->SetCapture(false);
			m_pAUIManager->BringWindowToTop(this);
			char szName[AFILE_LINEMAXLEN];
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				strncpy(szName, pElement->pThis->GetName(), AFILE_LINEMAXLEN - 1);
				szName[AFILE_LINEMAXLEN - 1] = 0;
				if( strstr(szName, "DEFAULT_") == szName )
				{
					ChangeFocus(pElement->pThis);
					break;
				}
				pElement = pElement->pNext;
			}
			if( bModal )
				m_pAUIManager->SetUseCover(bActive);
		}
		else if( m_pParentDlgControl && m_pParentDlgControl->GetParent() && m_pParentDlgControl->GetParent()->IsShow() )
			m_pAUIManager->InsertDialogBefore(this, m_pParentDlgControl->GetParent());
		else if( m_bCanChangeZOrder ) 
		{
			if( m_pAUIManager->GetActiveDialog() )
				m_pAUIManager->InsertDialogAfter(this, m_pAUIManager->GetActiveDialog());
			else
				m_pAUIManager->InsertDialogBefore(this);
		}

		if( bChangeState )
		{
#ifdef _ANGELICA_AUDIO
			if( m_pAudio )
				m_pAUIManager->PlayAudioInstance(m_pAudio);
			else if( strlen(m_strSound) > 0 )
				m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#else
			if( strlen(m_strSound) > 0 )
				m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#endif
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				if( pElement->pThis->IsShow() && pElement->pThis->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE )
				{
					PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pElement->pThis;
					pImage->StartGfx();
				}

				pElement = pElement->pNext;
			}
		}

		m_bShow = true;
		OnShowDialog();
	}
	else
	{
		if( bChangeState )
		{
#ifdef _ANGELICA_AUDIO
			if (m_pAudioClose)
				m_pAUIManager->PlayAudioInstance(m_pAudioClose);
			else if( strlen(m_strSoundClose) > 0 )
				m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSoundClose);
#else
			if( strlen(m_strSoundClose) > 0 )
				m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSoundClose);
#endif
		}

		m_bShow = false;
		ALISTPOSITION pos = m_pAUIManager->m_DlgZOrder.Find(this);
		if( pos )
		{
			if( m_bCanChangeZOrder )
				m_pAUIManager->InsertDialogAfter(this);

			if( m_pAUIManager->m_DlgZOrder.GetCount() > 0 && m_pAUIManager->GetActiveDialog() == this )
			{
				ALISTPOSITION pos = m_pAUIManager->m_DlgZOrder.GetHeadPosition();
				while( pos )
				{
					PAUIDIALOG pDlg = m_pAUIManager->m_DlgZOrder.GetNext(pos);
					if( pDlg && pDlg->IsShow() )
					{
						m_pAUIManager->BringWindowToTop(pDlg);
						break;
					}
				}
			}

			ChangeFocus(NULL);
		}
		SetCapture(false);
		SetCaptureObject(NULL);

		OnHideDialog();
	}

	if( bChangeState )
	{
		PAUIOBJECTLISTELEMENT pElement = m_pListHead;
		while( pElement )
		{
			if( pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG )
			{
				PAUISUBDIALOG pDlg = (PAUISUBDIALOG)pElement->pThis;
				if( pDlg->GetSubDialog() )
				{
					if( m_bShow && pDlg->IsShow() )
						pDlg->GetSubDialog()->Show(true, false, bActive);
					else
						pDlg->GetSubDialog()->Show(false);
				}
			}
			pElement = pElement->pNext;
		}
	}
}

bool AUIDialog::IsShow(void)
{
	return m_bShow;
}

void AUIDialog::SetCapture(bool bCapture, int x, int y)
{
	if( bCapture )
	{
		POINT ptPos = { x, y };
		if( -1 == x || -1 == y )
		{
			GetCursorPos(&ptPos);
			ScreenToClient(m_pAUIManager->GetHWnd() ? m_pAUIManager->GetHWnd() : m_pA3DDevice->GetDeviceWnd(), &ptPos);
		}

		m_bCapture = true;
		m_ptLButtonDown.x = ptPos.x;
		m_ptLButtonDown.y = ptPos.y;
		::SetCapture(m_pAUIManager->GetHWnd() ? m_pAUIManager->GetHWnd() : m_pA3DDevice->GetDeviceWnd());
	}
	else
	{
		m_bCapture = false;
		m_ptLButtonDown.x = -65535;
		m_ptLButtonDown.y = -65535;
		ReleaseCapture();
	}
}

bool AUIDialog::Tick(void)
{
	bool bval = true;
	PAUIOBJECTLISTELEMENT pElement;

	pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->IsShow() )
		{
			bval = pElement->pThis->Tick();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Tick(), Failed to call pElement->pThis->Tick()");
		}

		pElement = pElement->pNext;
	}

	if( (GetTickCount() - m_dwLifeStart) / 1000 >= m_dwLife )
		SetDestroyFlag(true);

	OnTick();

	return true;
}

bool AUIDialog::Render(void)
{
#ifdef _ANGELICA22
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif
	if( m_vecFrame.size() == 0 && m_pUIRenderTarget )
	{
		if( m_bUpdateRenderTarget || m_pUIRenderTarget->NeedRender() )
		{
			bool bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pAUIManager->GetA3DFTFontMan()->Flush();

			int x = m_x;
			int y = m_y;
			m_x = 0;
			m_y = 0;
			m_nRenderState = AUIDIALOG_RENDERSTATE_STATIC;
			if(!m_pUIRenderTarget->BeginRenderRTAlpha())
			{
				m_x = x;
				m_y = y;
				m_nRenderState = AUIDIALOG_RENDERSTATE_NORMAL;
				RenderOnce();
				return true;
			}

			RenderOnce();
			m_pUIRenderTarget->BeginRenderRT();
			RenderOnce();
			m_pUIRenderTarget->EndRenderRT();
			m_bUpdateRenderTarget = false;
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				if( pElement->pThis->GetType() == AUIOBJECT_TYPE_TEXTAREA )
				{
					DWORD i;
					abase::vector<AUITEXTAREA_EDITBOX_ITEM> &vecItemLink = ((PAUITEXTAREA)pElement->pThis)->GetEditBoxItems();
					for(i = 0; i < vecItemLink.size(); i++)
						vecItemLink[i].rc.Offset(x, y);
					abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = ((PAUITEXTAREA)pElement->pThis)->GetNameLink();
					for(i = 0; i < vecNameLink.size(); i++)
						vecNameLink[i].rc.Offset(x, y);
				}
				pElement = pElement->pNext;
			}
			m_x = x;
			m_y = y;
			m_pUIRenderTarget->SetNeedRender(false);
		}

		if (m_bFlushRenderTarget)
			m_pUIRenderTarget->Render(m_x, m_y);

		m_nRenderState = AUIDIALOG_RENDERSTATE_DYNAMIC;
		RenderOnce();
		m_nRenderState = AUIDIALOG_RENDERSTATE_NORMAL;
	}
	else
	{
		m_nRenderState = AUIDIALOG_RENDERSTATE_NORMAL;
		RenderOnce();
	}

	return true;
}

bool AUIDialog::RenderOnce(void)
{
	if( m_nBlinkInterval > 0 && (GetTickCount() / m_nBlinkInterval) % 2 )
		return true;

	bool bval = true;

	if( m_nRenderState != AUIDIALOG_RENDERSTATE_DYNAMIC )
	{
		A3DRECT rcWindow = m_pAUIManager->GetRect();
		if( m_pFont )
		{
			float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
			FitFont();
		}

		A3DFTFont *pFont = m_pFont ? m_pFont : m_pAUIManager->GetDefaultFont();

		float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
		int nAlpha = m_nAlpha * m_nWholeAlpha / 255;
		POINT dPos = GetPos();
		dPos.x += rcWindow.left;
		dPos.y += rcWindow.top;

		// Only render when frames is OK.
		if ( m_pParentDlgControl )
		{
			bval = RenderFrameInSubdialog();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call RenderFrameInSubdialog");
		}
		else if( m_pAUIFrameDlg )
		{
			bval = m_pAUIFrameDlg->Render(dPos.x, dPos.y, m_nWidth, m_nHeight, pFont,
					m_color, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, true, m_nShadow);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call m_pAUIFrameDlg->Render()");
		}

		if( AUIDIALOG_TYPE_POPUPMENU == m_nType && m_pAUIFrameDlg && m_pAUIFrameDlg->GetA2DSprite()->GetTextures() && m_pFont )
		{
			m_pAUIFrameDlg->GetA2DSprite()->GetTextures()->GetSpriteBuffer()->FlushBuffer();
			bval = RenderMenuItems();
			return bval;
		}

		// Render title bar and text.
		if( m_pAUIFrameDlg && m_pAUIFrameTitle && m_pAUIFrameDlg->GetA2DSprite()->GetItem(AUIFRAME_RECT_UPPERLEFT) )
		{
			A3DRECT rc = m_pAUIFrameDlg->GetA2DSprite()->GetItem(AUIFRAME_RECT_UPPERLEFT)->GetRect();
			
			rc.left *= fWindowScale;
			rc.right *= fWindowScale;
			rc.top *= fWindowScale;
			rc.bottom *= fWindowScale;

			// -1 means auto fit.
			int cx = m_Size[AUIDIALOG_FRAME_TITLE].cx * fWindowScale;
			if( -1 == m_Size[AUIDIALOG_FRAME_TITLE].cx )
				cx = m_nWidth - m_pt[AUIDIALOG_FRAME_TITLE].x * fWindowScale
					- m_pt[AUIDIALOG_FRAME_CLOSE].x * fWindowScale;
			int cy = m_Size[AUIDIALOG_FRAME_TITLE].cy * fWindowScale;
			if( -1 == m_Size[AUIDIALOG_FRAME_TITLE].cy )
				cy = m_pAUIFrameTitle->GetA2DSprite()->GetHeight() * fWindowScale;
			
			// -1 means auto position.
			if( -1 == m_pt[AUIDIALOG_FRAME_TITLE].x )
				m_pt[AUIDIALOG_FRAME_TITLE].x = rc.Width() * fWindowScale;
			if( -1 == m_pt[AUIDIALOG_FRAME_TITLE].y )
				m_pt[AUIDIALOG_FRAME_TITLE].y = rc.Height() * fWindowScale - cy;

			int x = dPos.x + m_pt[AUIDIALOG_FRAME_TITLE].x;
			int y = dPos.y + m_pt[AUIDIALOG_FRAME_TITLE].y;

			bval = m_pAUIFrameTitle->Render(x, y , cx, cy, pFont,
				m_color, m_strTitle, AUIFRAME_ALIGN_CENTER, 0, 0, nAlpha, false, m_nShadow);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to m_pAUIFrameTitle->Render()");
		}

		// Render close button if have.
		if( m_pA2DSprite[AUIDIALOG_FRAME_CLOSE] )
		{
			m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->SetScaleX(float(m_nWidth) / m_nDefaultWidth);
			m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->SetScaleY(float(m_nHeight) / m_nDefaultHeight);
			m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->SetAlpha(nAlpha);
			m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->SetCurrentItem(m_nState);

			int x = dPos.x + m_nWidth - m_pt[AUIDIALOG_FRAME_CLOSE].x * fWindowScale;
			int y = dPos.y + m_pt[AUIDIALOG_FRAME_CLOSE].y * fWindowScale;
			
			if( IsNoFlush() )
				bval = m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->DrawToBack() or m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->DrawToInternalBuffer()");
		}

		if( m_pA2DSprite[AUIDIALOG_FRAME_BACK] )
		{
			int x = dPos.x, y = dPos.y, W = m_nWidth, H = m_nHeight;

			if( m_pAUIFrameDlg )
			{
				A3DRECT rc = m_pAUIFrameDlg->GetA2DSprite()->
					GetItem(AUIFRAME_RECT_UPPERLEFT)->GetRect();
				x += rc.right;
				y += rc.bottom;
				W -= 2 * rc.Width();
				H -= 2 * rc.Height();
			}

			bool bval = true;
			float fScaleX = float(W) / float(m_pA2DSprite[AUIDIALOG_FRAME_BACK]->GetWidth());
			float fScaleY = float(H) / float(m_pA2DSprite[AUIDIALOG_FRAME_BACK]->GetHeight());

			m_pA2DSprite[AUIDIALOG_FRAME_BACK]->SetScaleX(fScaleX);
			m_pA2DSprite[AUIDIALOG_FRAME_BACK]->SetScaleY(fScaleY);

			if( IsNoFlush() )
				bval = m_pA2DSprite[AUIDIALOG_FRAME_BACK]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUIDIALOG_FRAME_BACK]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call m_pA2DSprite[AUIDIALOG_FRAME_BACK]->DrawToBack() or m_pA2DSprite[AUIDIALOG_FRAME_BACK]->DrawToInternalBuffer()");
		}

		// No any frame image, so we draw dialog by pure color.
	//	if( !m_pAUIFrameDlg && !m_pAUIFrameTitle && !m_pA2DSprite[AUIDIALOG_FRAME_CLOSE] )
	//	{
	//		extern A3DGDI *g_pA3DGDI;
	//		A3DCOLOR color = (nAlpha << 24) | (m_color & 0xFFFFFF);
	//
	//		bval = g_pA3DGDI->Draw2DRectangle(GetRect(), color);
	//		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call g_pA3DGDI->Draw2DRectangle()");
	//	}

		if( m_vecFrame.size() > 0 )
		{
			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pAUIManager->GetA3DFTFontMan()->Flush();
		}
	}

	bval = RenderControls();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Render(), Failed to call RenderControls()");

	m_pA3DEngine->GetA3DFlatCollector()->Flush_2D();

	return true;
}

bool AUIDialog::RenderControls(void)
{
#ifdef _ANGELICA22
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif
	int i;
	bool bval = true;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	int nOffX = rcWindow.left, nOffY = rcWindow.top, nVPW = rcWindow.Width(), nVPH = rcWindow.Height();

	PAUIOBJECT pObjNoViewport1 = NULL, pObjNoViewport2 = NULL;
	if( m_pParentDlgControl && m_pObjFocus && m_pObjFocus->GetClipViewport() == m_pParentDlgControl->GetSubDialogViewport() )
	{
		if( m_pObjFocus->GetType() == AUIOBJECT_TYPE_COMBOBOX && ((PAUICOMBOBOX)m_pObjFocus)->GetState() == AUICOMBOBOX_STATE_POPUP )
			pObjNoViewport1 = m_pObjFocus;
		if( m_pObjFocus->GetType() == AUIOBJECT_TYPE_LISTBOX && ((PAUILISTBOX)m_pObjFocus)->GetComboBox() )
		{
			pObjNoViewport1 = m_pObjFocus;
			pObjNoViewport2 = ((PAUILISTBOX)m_pObjFocus)->GetComboBox();
		}
	}
	
	if( pObjNoViewport1 )
	{
		pObjNoViewport1->SetClipViewport(NULL);
		if( pObjNoViewport2 )
			pObjNoViewport2->SetClipViewport(NULL);
	}

	if( m_vecFrame.size() > 0 )
	{
		for( i = 0; i < (int)m_vecFrame.size(); i++ )
		{
			if (m_vecFrame[i].nRenderPriority != AUIDIALOG_VIEWPORT_RENDER_BOTTOM)
				continue;
			bval = RenderControlsInViewport(i);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call RenderControlsInViewport()");
		}

	}

	m_nCurImageCover = 0;
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->IsShow() &&
			pElement->pThis != m_pObjFocus &&
			!pElement->pThis->GetClipViewport() &&
			(pElement->pThis->GetType() != AUIOBJECT_TYPE_IMAGEPICTURE || !((PAUIIMAGEPICTURE)pElement->pThis)->GetRenderTop()) )
		{
			pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

			bval = pElement->pThis->Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call pElement->pThis->Render()");
		}
		pElement = pElement->pNext;
	}

	if( m_pObjFocus && m_pObjFocus->IsShow() && m_pObjFocus->GetType() != AUIOBJECT_TYPE_COMBOBOX && !m_pObjFocus->GetClipViewport() )
	{
		m_pObjFocus->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

		bval = m_pObjFocus->Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call m_pObjFocus->Render()");
	}

	if( m_bImageCoverOrderly )
	{
		for(int i = 1; i < AUIIMAGEPICTURE_MAXCOVERS; i++)
		{
			m_nCurImageCover = i;
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				if( pElement->pThis->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE &&
					pElement->pThis->IsShow() &&
					!pElement->pThis->GetClipViewport() &&
					!((PAUIIMAGEPICTURE)pElement->pThis)->GetRenderTop() )
				{
					pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

					bval = pElement->pThis->Render();
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call pElement->pThis->Render()");
				}

				pElement = pElement->pNext;
			}
		}
	}

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

	m_nCurImageCover = 0;
	pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->IsShow() &&
			pElement->pThis != m_pObjFocus &&
			!pElement->pThis->GetClipViewport() &&
			pElement->pThis->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE && ((PAUIIMAGEPICTURE)pElement->pThis)->GetRenderTop() )
		{
			pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

			bval = pElement->pThis->Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call pElement->pThis->Render()");
		}
		pElement = pElement->pNext;
	}

	if( m_pObjFocus && m_pObjFocus->IsShow() && m_pObjFocus->GetType() == AUIOBJECT_TYPE_COMBOBOX && !m_pObjFocus->GetClipViewport() )
	{
		m_pObjFocus->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

		bval = m_pObjFocus->Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call m_pObjFocus->Render()");
	}

	if( m_bImageCoverOrderly )
	{
		for(int i = 1; i < AUIIMAGEPICTURE_MAXCOVERS; i++)
		{
			m_nCurImageCover = i;
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				if( pElement->pThis->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE &&
					pElement->pThis->IsShow() &&
					!pElement->pThis->GetClipViewport() &&
					((PAUIIMAGEPICTURE)pElement->pThis)->GetRenderTop() )
				{
					pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

					bval = pElement->pThis->Render();
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call pElement->pThis->Render()");
				}

				pElement = pElement->pNext;
			}
		}
	}

	if( m_vecFrame.size() > 0 )
	{
		for( i = 0; i < (int)m_vecFrame.size(); i++ )
		{
			if (m_vecFrame[i].nRenderPriority != AUIDIALOG_VIEWPORT_RENDER_TOP)
				continue;
			bval = RenderControlsInViewport(i);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call RenderControlsInViewport()");
		}

	}

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControls(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

	m_pAUIManager->GetA3DFTFontMan()->Flush();

	if( pObjNoViewport1 )
	{
		pObjNoViewport1->SetClipViewport(m_pParentDlgControl->GetSubDialogViewport());
		if( pObjNoViewport2 )
			pObjNoViewport2->SetClipViewport(m_pParentDlgControl->GetSubDialogViewport());
	}

	return true;
}

bool AUIDialog::RenderControlsInViewport(int nIndex)
{
	A3DViewport *pViewport = m_pA3DEngine->GetActiveViewport();
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	POINT dPos = GetPos();
	dPos.x += rcWindow.left;
	dPos.y += rcWindow.top;


	bool bval = true;
	A3DRECT rcDlg, rcObj;
	int nOffX = rcWindow.left, nOffY = rcWindow.top, nVPW = rcWindow.Width(), nVPH = rcWindow.Height();
	A3DVIEWPORTPARAM *pClip = m_vecFrame[nIndex].pA3DViewport->GetParam();
	A3DRECT rcView(nOffX, nOffY, nOffX + nVPW, nOffY + nVPH), rcInter;

	int nX = dPos.x + m_vecFrame[nIndex].rc.left * fWindowScale;
	int nY = dPos.y + m_vecFrame[nIndex].rc.top * fWindowScale;
	int nW = m_vecFrame[nIndex].rc.Width() * fWindowScale;
	int nH = m_vecFrame[nIndex].rc.Height() * fWindowScale;
	rcDlg.SetRect(nX, nY, nX + nW, nY + nH);

	rcInter = rcDlg & rcView;
	
	if( rcInter.Width() <= 0 || rcInter.Height() <= 0 )
		return true;

	pClip->X = rcInter.left;
	pClip->Y = rcInter.top;
	pClip->Width = rcInter.Width();
	pClip->Height = rcInter.Height();

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");
	m_pAUIManager->GetA3DFTFontMan()->Flush();

	m_vecFrame[nIndex].pA3DViewport->SetParam(pClip);
	m_vecFrame[nIndex].pA3DViewport->Active();

	m_nCurImageCover = 0;
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis->IsShow() && pElement->pThis != m_pObjFocus &&
			pElement->pThis->GetClipViewport() == m_vecFrame[nIndex].pA3DViewport )
		{
			rcObj = pElement->pThis->GetRect();
			rcObj.Offset(nOffX, nOffY);
	
			if( pElement->pThis->GetType() == AUIOBJECT_TYPE_LABEL && 
				pElement->pThis->GetSize().cx == 0 && pElement->pThis->GetSize().cy == 0 && rcObj.top <= rcDlg.bottom || 
				rcObj.left <= rcDlg.right && rcObj.right >= rcDlg.left &&
				rcObj.top <= rcDlg.bottom && rcObj.bottom >= rcDlg.top )
			{
				pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

				bval = pElement->pThis->Render();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call pElement->pThis->Render()");
			}
		}

		pElement = pElement->pNext;
	}

	if( m_pObjFocus && m_pObjFocus->IsShow() &&
		m_pObjFocus->GetClipViewport() == m_vecFrame[nIndex].pA3DViewport)
	{
		rcObj = m_pObjFocus->GetRect();
		rcObj.Offset(nOffX, nOffY);

		if( rcObj.left <= rcDlg.right && rcObj.right >= rcDlg.left &&
			rcObj.top <= rcDlg.bottom && rcObj.bottom >= rcDlg.top )
		{
			m_pObjFocus->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

			bval = m_pObjFocus->Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call m_pObjFocus->Render()");
		}
	}

	if( m_bImageCoverOrderly )
	{
		for(int i = 1; i < AUIIMAGEPICTURE_MAXCOVERS; i++)
		{
			m_nCurImageCover = i;
			PAUIOBJECTLISTELEMENT pElement = m_pListHead;
			while( pElement )
			{
				if( pElement->pThis->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE &&
					pElement->pThis->IsShow() && 
					pElement->pThis->GetClipViewport() == m_vecFrame[nIndex].pA3DViewport )
				{
					rcObj = pElement->pThis->GetRect();
					rcObj.Offset(nOffX, nOffY);
			
					if( rcObj.left <= rcDlg.right && rcObj.right >= rcDlg.left &&
						rcObj.top <= rcDlg.bottom && rcObj.bottom >= rcDlg.top )
					{
						pElement->pThis->SetViewportParam(nOffX, nOffY, nVPW, nVPH);

						bval = pElement->pThis->Render();
						if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call pElement->pThis->Render()");
					}
				}

				pElement = pElement->pNext;
			}
		}
	}

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

	m_pAUIManager->GetA3DFTFontMan()->Flush();

	pViewport->Active();

	return true;
}

bool AUIDialog::RenderMenuItems(void)
{
	int x, y;
	bool bval = true;
	SIZE sz = {0, 0};
	if( m_pAUIFrameDlg )
		sz = m_pAUIFrameDlg->GetSizeLimit();
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	int xBegin = rcWindow.left + m_x + sz.cx / 2;
	int yBegin = rcWindow.top + m_y + sz.cy / 2;

	if( m_nCurSel >= 0 )
	{
		x = xBegin;
		y = yBegin + m_nCurSel * m_nMaxMenuTextHeight * fWindowScale;
		if( 1 || IsNoFlush() )
			bval = m_pA2DSpriteHilight->DrawToBack(x, y);
		else
			bval = m_pA2DSpriteHilight->DrawToInternalBuffer(x, y);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderMenuItems(), Failed to call m_pA2DSpriteHilight->DrawToBack() or m_pA2DSpriteHilight->DrawToInternalBuffer()");
	}

	A3DPOINT2 ptSize;
	ACString strCheck;
	A3DCOLOR color, r, g, b;
	
	FitFont();

	A3DFTFont *pFont = m_pFont ? m_pFont : m_pAUIManager->GetDefaultFont();

	ptSize = pFont->GetTextExtent(GetStringFromTable(2));
	for( int i = 0; i < (int)m_MenuItem.size(); i++ )
	{
		x = xBegin;
		y = yBegin + i * m_nMaxMenuTextHeight * fWindowScale;

		color = m_color;
		if( !m_MenuItem[i].bEnable )
		{
			r = A3DCOLOR_GETRED(m_color) / 2;
			g = A3DCOLOR_GETRED(m_color) / 2;
			b = A3DCOLOR_GETRED(m_color) / 2;
			color = A3DCOLORRGB(r, g, b);
		}

		if( m_MenuItem[i].bCheck )
			strCheck = GetStringFromTable(2);
		else
		{
			x += ptSize.x;
			strCheck = _AL("");
		}

		pFont->TextOut(x, y, strCheck + m_MenuItem[i].strMenuText, color);
	}

	m_pAUIManager->GetA3DFTFontMan()->Flush();

	return true;
}

PAUIOBJECT AUIDialog::CreateControl(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, int idControl, bool bAddToDialog)
{
	ASSERT(pA3DEngine && pA3DDevice);
	ASSERT(idControl >= AUIOBJECT_TYPE_MIN && idControl < AUIOBJECT_TYPE_MAX);

	bool bval = true;
	PAUIOBJECT pControl = NULL;
	switch( idControl )
	{
		case AUIOBJECT_TYPE_CHECKBOX:
			pControl = new AUICheckBox;
			if( !pControl ) return NULL;
			break;

		case AUIOBJECT_TYPE_COMBOBOX:
			pControl = new AUIComboBox;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_CONSOLE:
			pControl = new AUIConsole;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_CUSTOMIZE:
			pControl = new AUICustomize;
			if( !pControl ) return NULL;
			break;

		case AUIOBJECT_TYPE_EDITBOX:
			pControl = new AUIEditBox;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_IMAGEPICTURE:
			pControl = new AUIImagePicture;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_LABEL:
			pControl = new AUILabel;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_LISTBOX:
			pControl = new AUIListBox;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_MODELPICTURE:
			pControl = new AUIModelPicture;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_PROGRESS:
			pControl = new AUIProgress;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_RADIOBUTTON:
			pControl = new AUIRadioButton;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_SCROLL:
			pControl = new AUIScroll;
			if( !pControl ) return NULL;
			break;
			
		case AUIOBJECT_TYPE_SLIDER:
			pControl = new AUISlider;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_STILLIMAGEBUTTON:
			pControl = new AUIStillImageButton;
			if( !pControl ) return NULL;
			break;

		case AUIOBJECT_TYPE_SUBDIALOG:
			pControl = new AUISubDialog;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_TEXTAREA:
			pControl = new AUITextArea;
			if( !pControl ) return NULL;
			break;
		
		case AUIOBJECT_TYPE_TREEVIEW:
			pControl = new AUITreeView;
			if( !pControl ) return NULL;
			break;

		case AUIOBJECT_TYPE_VERTICALTEXT:
			pControl = new AUIVerticalText;
			if( !pControl ) return NULL;
			break;

		case AUIOBJECT_TYPE_WINDOWPICTURE:
			pControl = new AUIWindowPicture;
			if( !pControl ) return NULL;
			break;
		
	}

	if( pControl )
	{
		pControl->SetParent(this);
		bval = pControl->Init(pA3DEngine, pA3DDevice, NULL);
		if( !bval )
		{
			delete pControl;
			return NULL;
		}
		
		if( bAddToDialog )
		{
			bval = AddObjectToList(pControl);
			if( !bval )
			{
				delete pControl;
				return NULL;
			}
		}
	}
	return pControl;
}

bool AUIDialog::DeleteControl(PAUIOBJECT pAUIObject)
{
	PAUIOBJECTLISTELEMENT pos;
	DeleteControl(pAUIObject, pos);
	return true;
}

PAUIOBJECT AUIDialog::CloneControl(PAUIOBJECT pAUIObject, bool bAddToDialog)
{
	PAUIOBJECT pControl = CreateControl(m_pA3DEngine,
		m_pA3DDevice, pAUIObject->GetType(), bAddToDialog);
	if( !pControl ) return NULL;

	AUIOBJECT_PROPERTY a_Property[AUIOBJECT_MAXPROPERTYNUM];
	int nNumProperties = AUI_RetrieveControlPropertyList(
		pAUIObject->GetType(), a_Property);

	bool bval = true;
	AUIOBJECT_SETPROPERTY Property;
	for( int i = 0; i < nNumProperties; i++ )
	{
		if( 0 == stricmp(a_Property[i].szName, "Name") )
			continue;	// We do not clone control name.

		bval = pAUIObject->GetProperty(a_Property[i].szName, &Property);
		//if( !bval )
		//{
		//	A3DRELEASE(pControl);
		//	return NULL;
		//}

		if(bval)
		{
			bval = pControl->SetProperty(a_Property[i].szName, &Property);
			//if( !bval )
			//{
			//	A3DRELEASE(pControl);
			//	return NULL;
			//}
		}
	}

	return pControl;
}


SIZE AUIDialog::GetSize(void)
{
	SIZE sz;
	sz.cx = m_nWidth;
	sz.cy = m_nHeight;
	return sz;
}

SIZE AUIDialog::GetDefaultSize(void)
{
	SIZE sz;
	sz.cx = m_nDefaultWidth;
	sz.cy = m_nDefaultHeight;
	return sz;
}

void AUIDialog::SetScale(float fScale)
{
	m_fScale = fScale;
	if( m_pParentDlgControl )
		SetSize(m_nWidth, m_nHeight);

	Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());
}

void AUIDialog::SetSize(int W, int H)
{
	if( m_pUIRenderTarget && (W != m_nWidth || H != m_nHeight) )
	{
		m_pUIRenderTarget->Release();
		m_pUIRenderTarget->Init(m_pA3DDevice, W, H);
		UpdateRenderTarget();
	}

	AUIOBJECT_SETPROPERTY p;
	p.i = W;
	SetProperty("Width", &p);
	p.i = H;
	SetProperty("Height", &p);
}

void AUIDialog::SetDefaultSize(int W, int H)
{
	int nWidth = W * m_pAUIManager->GetWindowScale() * m_fScale;
	int nHeight = H * m_pAUIManager->GetWindowScale() * m_fScale;

	if( m_pUIRenderTarget && (m_nWidth != nWidth || m_nHeight != nHeight) )
	{		
		m_pUIRenderTarget->Release();
		m_pUIRenderTarget->Init(m_pA3DDevice, nWidth, nHeight);
		UpdateRenderTarget();
	}
	
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDefaultWidth = W;
	m_nDefaultHeight = H;
}

A3DRECT AUIDialog::GetRect(void)
{
	A3DRECT rc(m_x, m_y, m_x + m_nWidth, m_y + m_nHeight);
	return rc;
}

void AUIDialog::ChangeFocus(PAUIOBJECT pNewFocus)
{
	if( m_pObjFocus != pNewFocus )
	{
		if( m_pObjFocus )
		{
			m_pObjFocus->OnDlgItemMessage(WM_KILLFOCUS, (WPARAM)pNewFocus, 0);
			m_pObjFocus->SetFocus(false);			
			m_pAUIManager->OnEvent(WM_KILLFOCUS, (WPARAM)pNewFocus, (LPARAM)this, this, m_pObjFocus);
		}
		if( pNewFocus )
		{
			pNewFocus->SetFocus(true);
			m_pAUIManager->OnEvent(WM_SETFOCUS, (WPARAM)m_pObjFocus, (LPARAM)this, this, pNewFocus);
		}

		m_pObjFocus = pNewFocus;
		UpdateRenderTarget();
	}
}


PAUIOBJECT AUIDialog::GetDlgItem(const char *pszItemName)
{
	AUIDIALOG_AUIOBJECTTABLE::pair_type p;
	p = m_AUIObjectTable.get(pszItemName);
	if( p.second )
		return *p.first;
	else
		return NULL;
}


bool AUIDialog::ImportStringTable(const char *pszFilename)
{
	bool bval = true;
	int idString;
	ACString str;
	AWScriptFile s;
	AUIDIALOG_STRINGTABLE::pair_type p;

	bval = s.Open(pszFilename);
	if( !bval ) return true;	// Ignore error.

	while( !s.IsEnd() )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.
		idString = a_atoi(s.m_szToken);

		bval = s.GetNextToken(true);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::ImportStringTable(), failed to read from file");

		str = s.m_szToken;
		p = m_StringTable.get(idString);
		if( p.second )
			*p.first = str;
		else
			m_StringTable.put(idString, str);
	}

	s.Close();

	return true;
}

bool AUIDialog::ExportStringTable(const char *pszFilename)
{
	FILE *file = fopen(pszFilename, "wb");
	if( !file ) return false;

	// Unicode header.
	BYTE byteHeader[2];
	byteHeader[0] = 0xFF;
	byteHeader[1] = 0xFE;
	fwrite(byteHeader, sizeof(BYTE), 2, file);

	// Write file version
	fwprintf(file, _AL("-1\t\"%d\"\n"), AUIDIALOG_FILE_VERSION);
	
	AUIDIALOG_STRINGTABLE::iterator it = m_StringTable.begin();
	for( ; it != m_StringTable.end(); ++it )
	{
		int idString = *(it.key());
		ACString str = *(it.value());
		fwprintf(file, _AL("%d\t\"%s\"\n"), idString, str);
	}

	fclose(file);

	return true;
}

int AUIDialog::AddStringToTable(const ACHAR *pszString)
{
	int idString = m_StringTable.size();
	if( m_StringTable.put(idString, pszString) )
		return idString;
	else
		return -1;
}

const ACHAR * AUIDialog::GetStringFromTable(int idString)
{
	AUIDIALOG_STRINGTABLE::pair_type p = m_StringTable.get(idString);
	if( p.second )
		return *p.first;
	else
		return m_pAUIManager->GetStringFromTable(idString);
}

#define AUI_pszFilterBmp "Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"
#define AUI_pszFilterWav "Sound Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||"

int AUIDialog::RetrievePropertyList(PAUIOBJECT_PROPERTY a_Property)
{
	AUIOBJECT_PROPERTY p[] =
	{
		{ "Name", AUIOBJECT_PROPERTY_CHAR, "" },
		{ "Width", AUIOBJECT_PROPERTY_INT, "" },
		{ "Height", AUIOBJECT_PROPERTY_INT, "" },
		{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
		{ "Anomaly", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Alpha", AUIOBJECT_PROPERTY_INT, "" },
		{ "Pop Up Dialog", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Background Dialog", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Can Esc Close", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Can Change Z-Order", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Can Move", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Can Out Of Window", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Min Dist DlgR To ScrL", AUIOBJECT_PROPERTY_INT, "" },
		{ "Min Dist DlgL To ScrR", AUIOBJECT_PROPERTY_INT, "" },
		{ "Min Dist DlgB To ScrT", AUIOBJECT_PROPERTY_INT, "" },
		{ "Min Dist DlgT To ScrB", AUIOBJECT_PROPERTY_INT, "" },
		{ "Static Render", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Image Cover Orderly", AUIOBJECT_PROPERTY_BOOL, "" },
		{ "Hint Delay", AUIOBJECT_PROPERTY_INT, "" },
#ifdef _ANGELICA_AUDIO
		{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
		{ "Sound Effect Close", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
		{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
		{ "Sound Effect Close", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
	};
	memcpy(a_Property, p, sizeof(p));
	int nNumProperties = sizeof(p) / sizeof(AUIOBJECT_PROPERTY);
	return nNumProperties;
}

bool AUIDialog::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Name") )
		strncpy(Property->c, m_szName, AUIDIALOG_MAXNAMELEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		Property->dw = m_color;
	else if( 0 == strcmpi(pszPropertyName, "X") )
		Property->i = m_x;
	else if( 0 == strcmpi(pszPropertyName, "Y") )
		Property->i = m_y;
	else if( 0 == strcmpi(pszPropertyName, "Width") )
		Property->i = m_nWidth;
	else if( 0 == strcmpi(pszPropertyName, "Height") )
		Property->i = m_nHeight;
	else if( 0 == strcmpi(pszPropertyName, "Font") )
	{
		a_strncpy(Property->font.szName, m_strFontName, AFILE_LINEMAXLEN - 1);
		Property->font.nHeight = m_nFontHeight;
	}
	else if( 0 == strcmpi(pszPropertyName, "Alpha") )
		Property->i = m_nAlpha;
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrameDlg && m_pAUIFrameDlg->GetA2DSprite() )
			strncpy(Property->fn, m_pAUIFrameDlg->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
		else
			strcpy(Property->fn, "");
	}
	else if( 0 == strcmpi(pszPropertyName, "Background Image") )
	{
		if( m_pA2DSprite[AUIDIALOG_FRAME_BACK] )
			strncpy(Property->fn, m_pA2DSprite[AUIDIALOG_FRAME_BACK]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Title") )
		a_strncpy(Property->s, m_strTitle, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Image") )
	{
		if( m_pAUIFrameTitle && m_pAUIFrameTitle->GetA2DSprite() )
			strncpy(Property->fn, m_pAUIFrameTitle->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
		else
			strcpy(Property->fn, "");
	}
	else if( 0 == strcmpi(pszPropertyName, "Title Bar X") )
		Property->i = m_pt[AUIDIALOG_FRAME_TITLE].x;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Y") )
		Property->i = m_pt[AUIDIALOG_FRAME_TITLE].y;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Width") )
		Property->i = m_Size[AUIDIALOG_FRAME_TITLE].cx;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Height") )
		Property->i = m_Size[AUIDIALOG_FRAME_TITLE].cy;
	else if( 0 == strcmpi(pszPropertyName, "Close Button Image") )
	{
		if( m_pA2DSprite[AUIDIALOG_FRAME_CLOSE] )
			strncpy(Property->fn, m_pA2DSprite[AUIDIALOG_FRAME_CLOSE]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Close Button X") )
		Property->i = m_pt[AUIDIALOG_FRAME_CLOSE].x;
	else if( 0 == strcmpi(pszPropertyName, "Close Button Y") )
		Property->i = m_pt[AUIDIALOG_FRAME_CLOSE].y;
	else if( 0 == strcmpi(pszPropertyName, "Background Dialog") )
		Property->b = m_bBackgroundDialog;
	else if( 0 == strcmpi(pszPropertyName, "Can Out Of Window") )
		Property->b = m_bCanOutOfWindow;
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgR To ScrL") )
		Property->i = m_nMinDistDlgRToScrL;
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgL To ScrR") )
		Property->i = m_nMinDistDlgLToScrR;
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgB To ScrT") )
		Property->i = m_nMinDistDlgBToScrT;
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgT To ScrB") )
		Property->i = m_nMinDistDlgTToScrB;

	else if( 0 == strcmpi(pszPropertyName, "Can Move") )
		Property->b = m_bCanMove;
	else if( 0 == strcmpi(pszPropertyName, "Can Change Z-Order") )
		Property->b = m_bCanChangeZOrder;
	else if( 0 == strcmpi(pszPropertyName, "Can Esc Close") )
		Property->b = m_bCanEscClose;
	else if( 0 == strcmpi(pszPropertyName, "Pop Up Dialog") )
		Property->b = m_bPopup;
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect") )
		strncpy(Property->fn, m_strSound, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect Close") )
		strncpy(Property->fn, m_strSoundClose, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Anomaly") )
		Property->b = m_bAnomaly;
	else if( 0 == strcmpi(pszPropertyName, "Static Render") )
		Property->b = (m_pUIRenderTarget != NULL);
	else if( 0 == strcmpi(pszPropertyName, "Image Cover Orderly") )
		Property->b = m_bImageCoverOrderly;
	else if( 0 == strcmpi(pszPropertyName, "Hint Delay") )
		Property->i = m_nHintDelay;

	return true;
}

bool AUIDialog::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Name") )
		SetName(Property->c);
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		SetFontAndColor(NULL, 0, Property->dw);
	else if( 0 == strcmpi(pszPropertyName, "Font") )
	{
		SetFontAndColor(Property->font.szName, Property->font.nHeight, m_color,
			m_nShadow, m_nOutline, Property->font.bBold, Property->font.bItalic);
	}
	else if( 0 == strcmpi(pszPropertyName, "X") )
		m_x = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Y") )
		m_y = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		m_nWidth = Property->i;
		if( m_pAUIFrameDlg )
		{
			SIZE s = m_pAUIFrameDlg->GetSizeLimit();
			m_nWidth = max(m_nWidth, s.cx);
		}
		m_nDefaultWidth = m_nWidth / (m_pAUIManager->GetWindowScale() * m_fScale);
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		m_nHeight = Property->i;
		if( m_pAUIFrameDlg )
		{
			SIZE s = m_pAUIFrameDlg->GetSizeLimit();
			m_nHeight = max(m_nHeight, s.cy);
		}
		m_nDefaultHeight = m_nHeight / (m_pAUIManager->GetWindowScale() * m_fScale);
	}
	else if( 0 == strcmpi(pszPropertyName, "Shadow") )
		m_nShadow = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Outline") )
		SetFontAndColor(m_strFontName, m_nFontHeight, m_color, m_nShadow, Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Alpha") )
		SetAlpha(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrameDlg )
			m_pAUIFrameDlg->SetMode(m_nFrameMode);
		if( m_pAUIFrameTitle )
			m_pAUIFrameTitle->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUIDIALOG_FRAME_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Background Image") )
		return InitIndirect(Property->fn, AUIDIALOG_FRAME_BACK);
	else if( 0 == strcmpi(pszPropertyName, "Title") )
		m_strTitle = Property->s;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Image") )
		return InitIndirect(Property->fn, AUIDIALOG_FRAME_TITLE);
	else if( 0 == strcmpi(pszPropertyName, "Title Bar X") )
		m_pt[AUIDIALOG_FRAME_TITLE].x = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Y") )
		m_pt[AUIDIALOG_FRAME_TITLE].y = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Width") )
		m_Size[AUIDIALOG_FRAME_TITLE].cx = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Title Bar Height") )
		m_Size[AUIDIALOG_FRAME_TITLE].cy = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Close Button Image") )
		return InitIndirect(Property->fn, AUIDIALOG_FRAME_CLOSE);
	else if( 0 == strcmpi(pszPropertyName, "Close Button X") )
		m_pt[AUIDIALOG_FRAME_CLOSE].x = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Close Button Y") )
		m_pt[AUIDIALOG_FRAME_CLOSE].y = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Background Dialog") )
		SetBackgroundDialog(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Can Out Of Window") )
		SetCanOutOfWindow(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgR To ScrL") )
		SetMinDistDlgRToScrL(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgL To ScrR") )
		SetMinDistDlgLToScrR(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgT To ScrB") )
		SetMinDistDlgTToScrB(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Min Dist DlgB To ScrT") )
		SetMinDistDlgBToScrT(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Can Move") )
		SetCanMove(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Can Change Z-Order") )
		SetCanChangeZOrder(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Can Esc Close") )
		SetCanEscClose(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Pop Up Dialog") )
		SetPopup(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect") )
	{
		m_strSound = Property->fn;
#ifdef _ANGELICA_AUDIO
		m_pAUIManager->LoadAudioEvtInstance(m_pAudio, m_strSound);
#endif
	}
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect Close") )
	{
		m_strSoundClose = Property->fn;
#ifdef _ANGELICA_AUDIO
		m_pAUIManager->LoadAudioEvtInstance(m_pAudioClose, m_strSoundClose);
#endif
	}
	else if( 0 == strcmpi(pszPropertyName, "Anomaly") )
		m_bAnomaly = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Static Render") )
		SetRenderTarget(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Image Cover Orderly") )
		SetImageCoverOrderly(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Hint Delay") )
		m_nHintDelay = Property->i;

	return true;
}

bool AUIDialog::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Name", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Text Color", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("X", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Y", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Width", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Height", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Shadow", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Outline", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Alpha", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Background Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Title", AUIOBJECT_PROPERTY_VALUE_ACHAR) &&
			SetAUIObjectPropertyType("Title Bar Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Title Bar X", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Title Bar Y", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Title Bar Width", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Title Bar Height", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Close Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Close Button X", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Close Button Y", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("MinDist DlgLeft to ScrRight", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("MinDist DlgRight to ScrLeft", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("MinDist DlgTop to ScrBottom", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("MinDist ScrBottom to DlgTop", AUIOBJECT_PROPERTY_VALUE_INT);
}

bool AUIDialog::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight, 
		A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic)
{
	if( pszFontName )
	{
		int nStyle = STYLE_NORMAL;
		if( bBold ) nStyle |= STYLE_BOLD;
		if( bItalic ) nStyle |= STYLE_ITALIC;
		m_pFont = m_pAUIManager->GetA3DFTFontMan()->CreatePointFont(
			nFontHeight, pszFontName, nStyle, 0, nOutline);
		if( !m_pFont ) return AUI_ReportError(__LINE__, 1, "AUIDialog::SetFontAndColor(), failed to create font");

		m_strFontName = pszFontName;
		m_nFontHeight = nFontHeight;
		m_nShadow = nShadow;
		m_fDisplayHeight = m_nFontHeight * m_pAUIManager->GetWindowScale() * m_fScale;
	}

	m_color = dwColor;

	return true;
}


void AUIDialog::SetRenderTarget(bool bRenderTarget)
{
	A3DRELEASE(m_pUIRenderTarget)
	if( bRenderTarget && m_nWidth > 0 && m_nHeight > 0 )
	{
		m_pUIRenderTarget = new UIRenderTarget;
		if( !m_pUIRenderTarget ) return;

		if( m_pUIRenderTarget->Init(m_pA3DDevice, m_nWidth, m_nHeight) )
			UpdateRenderTarget();
		else
			A3DRELEASE(m_pUIRenderTarget)
	}
}

bool AUIDialog::IsActive()
{
	return m_pAUIManager->GetActiveDialog() == this ? true : false;
}

DWORD AUIDialog::GetData(AString strName)
{
	if (0 != m_dwData && strName != m_strDataName)
		AUI_ReportError(__LINE__, 1, "AUIDialog::GetData(), data name not match");
	return m_dwData;
}

void AUIDialog::SetData(DWORD dwData, AString strName)
{
	m_strDataName = strName;
	m_dwData = dwData;
}

void AUIDialog::ForceGetData(DWORD &dwData, AString &strName)
{
	dwData = m_dwData;
	strName = m_strDataName;
}

void * AUIDialog::GetDataPtr(AString strName)
{
	if (0 != m_pvData && strName != m_strDataPtrName)
		AUI_ReportError(__LINE__, 1, "AUIDialog::GetDataPtr(), data name not match");
	return m_pvData;
}

void AUIDialog::ForceGetDataPtr(void *&pvData, AString &strName)
{
	pvData = m_pvData;
	strName = m_strDataPtrName;
}

void AUIDialog::SetDataPtr(void *pvData, AString strName)
{
	m_strDataPtrName = strName;
	m_pvData = pvData;
}

UINT64 AUIDialog::GetData64(AString strName)
{
	if (0 != m_uiData64 && strName != m_strData64Name)
		AUI_ReportError(__LINE__, 1, "AUIDialog::GetData64(), data name not match");
	return m_uiData64;
}

void AUIDialog::SetData64(UINT64 uiData64, AString strName)
{
	m_strData64Name = strName;
	m_uiData64 = uiData64;
}

void AUIDialog::ForceGetData64(UINT64 &uiData64, AString &strName)
{
	uiData64 = m_uiData64;
	strName = m_strData64Name;
}

UINT64 AUIDialog::GetDataPtr64(AString strName)
{
	if (0 != m_uiDataPtr64 && strName != m_strDataPtr64Name)
		AUI_ReportError(__LINE__, 1, "AUIDialog::GetDataPtr64(), data name not match");
	return m_uiDataPtr64;
}

void AUIDialog::ForceGetDataPtr64(UINT64 &uiDataPtr64, AString &strName)
{
	uiDataPtr64 = m_uiDataPtr64;
	strName = m_strDataPtr64Name;
}

void AUIDialog::SetDataPtr64(UINT64 uiDataPtr64, AString strName)
{
	m_strDataPtr64Name = strName;
	m_uiDataPtr64 = uiDataPtr64;
}

void AUIDialog::AlignTo(AUIDialog *pBase, int nInsideX, int nAlignX,
	int nInsideY, int nAlignY, int nOffsetX, int nOffsetY)
{

	SIZE sBase ;
	POINT ptBase;
	if (pBase)
	{
		sBase = pBase->GetSize();
		ptBase = pBase->GetPos();
	}
	else
	{
		ptBase.x = ptBase.y = 0;
		A3DRECT rcWindow = m_pAUIManager->GetRect();		
		sBase.cx = rcWindow.Width();
		sBase.cy = rcWindow.Height();
	}
	
	if( nInsideX == AUIDIALOG_ALIGN_INSIDE )
	{
		if( nAlignX == AUIDIALOG_ALIGN_LEFT )
			m_x = ptBase.x;
		else if( nAlignX == AUIDIALOG_ALIGN_RIGHT )
			m_x = ptBase.x + sBase.cx - m_nWidth;
		else
			m_x = ptBase.x + (sBase.cx - m_nWidth) / 2;
	}
	else if( nInsideX == AUIDIALOG_ALIGN_OUTSIDE )
	{
		if( nAlignX == AUIDIALOG_ALIGN_LEFT )
			m_x = ptBase.x - m_nWidth;
		else if( nAlignX == AUIDIALOG_ALIGN_RIGHT )
			m_x = ptBase.x + sBase.cx;
		else
			m_x = ptBase.x + (sBase.cx - m_nWidth) / 2;
	}

	if( nInsideY == AUIDIALOG_ALIGN_INSIDE )
	{
		if( nAlignY == AUIDIALOG_ALIGN_TOP )
			m_y = ptBase.y;
		else if( nAlignY == AUIDIALOG_ALIGN_BOTTOM )
			m_y = ptBase.y + sBase.cy - m_nHeight;
		else
			m_y = ptBase.y + (sBase.cy - m_nHeight) / 2;
	}
	else if( nInsideY == AUIDIALOG_ALIGN_OUTSIDE )
	{
		if( nAlignY == AUIDIALOG_ALIGN_TOP )
			m_y = ptBase.y - m_nHeight;
		else if( nAlignY == AUIDIALOG_ALIGN_BOTTOM )
			m_y = ptBase.y + sBase.cy;
		else
			m_y = ptBase.y + (sBase.cy - m_nHeight) / 2;
	}

	m_x += nOffsetX;
	m_y += nOffsetY;

	A3DRECT rcWindow = m_pAUIManager->GetRect();
	AdjustFromXY(rcWindow.Width(), rcWindow.Height());
}

void AUIDialog::SetLife(DWORD dwLife)
{
	m_dwLife = dwLife;
	m_dwLifeStart = GetTickCount();
}

void AUIDialog::SetBackgroundDialog(bool bBackgroundDialog)
{
	if( bBackgroundDialog == m_bBackgroundDialog )
		return;
	m_bBackgroundDialog = bBackgroundDialog;
	
	ALISTPOSITION pos;
	if( m_bBackgroundDialog )
	{
		pos = m_pAUIManager->m_DlgZOrder.Find(this);
		if( pos ) m_pAUIManager->m_DlgZOrder.RemoveAt(pos);

		pos = m_pAUIManager->m_DlgZOrderBack.Find(this);
		if( !pos ) m_pAUIManager->m_DlgZOrderBack.AddTail(this);
	}
	else
	{
		pos = m_pAUIManager->m_DlgZOrderBack.Find(this);
		if( pos ) m_pAUIManager->m_DlgZOrderBack.RemoveAt(pos);

		pos = m_pAUIManager->m_DlgZOrder.Find(this);
		if( !pos ) m_pAUIManager->m_DlgZOrder.AddTail(this);
	}

	PAUIOBJECTLISTELEMENT pEle = m_pListHead;
	while (pEle)
	{
		AUISubDialog* pSubDlg = dynamic_cast<AUISubDialog*> (pEle->pThis);
		if (pSubDlg && pSubDlg->GetSubDialog())
			pSubDlg->GetSubDialog()->SetBackgroundDialog(bBackgroundDialog);
		
		pEle = pEle->pNext;
	}
}


bool AUIDialog::IsInHitArea(int x, int y)
{
	if( !GetRect().PtInRect(x, y) )
		return false;

	if (m_pParentDlgControl)
	{
		AUIDialog* pDlgParent = m_pParentDlgControl->GetParent();
		while( pDlgParent)
		{
			if (!pDlgParent->GetRect().PtInRect(x, y))
				return false;
			if (!pDlgParent->GetParentDlgControl())
				break;
			pDlgParent = pDlgParent->GetParentDlgControl()->GetParent(); //子窗口有可能大小超过父窗口
		}
		return true;
	}

	if( !m_bAnomaly )
		return true;

	if( !m_pAUIFrameDlg )
		return false;

	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	DWORD color = m_pAUIFrameDlg->GetPixel(int((x - GetPos().x) / fWindowScale), int((y - GetPos().y) / fWindowScale), m_nDefaultWidth, m_nDefaultHeight);
	return A3DCOLOR_GETALPHA(color) > 0;
}


bool AUIDialog::ImportHintTable(const char *pszFilename)
{
	bool bval = true;
	AWScriptFile s;
	AString strItem;
	AWString strHint;
	AUICTranslate trans;

	bval = s.Open(pszFilename);
	if( !bval ) return true;	// Ignore error.

	while( !s.IsEnd() )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.
		strItem = AC2AS(s.m_szToken);

		bval = s.GetNextToken(true);
		if( !bval )
			return AUI_ReportError(__LINE__, 1, "AUIDialog::ImportHintTable(), failed to read from file");

		if( GetDlgItem(strItem) )
		{
			strHint = trans.Translate(s.m_szToken);
			GetDlgItem(strItem)->SetHint(strHint);
		}
	}

	s.Close();

	return true;
}

void AUIDialog::AddFrame(A3DViewport *pViewport, A3DRECT &rc, int nRenderPriority /* = AUIDIALOG_VIEWPORT_RENDER_BOTTOM */)
{
	FRAME frame;

	frame.pA3DViewport = pViewport;
	frame.rc = rc;
	frame.nRenderPriority = nRenderPriority;
	m_vecFrame.push_back(frame);
}

void AUIDialog::DelFrame(A3DViewport *pViewport)
{
	int i;

	for( i = 0; i < (int)m_vecFrame.size(); i++ )
	{
		if( pViewport == m_vecFrame[i].pA3DViewport )
		{
			m_vecFrame.erase(m_vecFrame.begin() + i);
			break;
		}
	}
}

void AUIDialog::ChangeFrame(A3DViewport *pViewport, A3DRECT &rc, int nRenderPriority /* = AUIDIALOG_VIEWPORT_RENDER_BOTTOM */)
{
	int i;

	for( i = 0; i < (int)m_vecFrame.size(); i++ )
	{
		if( pViewport == m_vecFrame[i].pA3DViewport )
		{
			m_vecFrame[i].rc = rc;
			m_vecFrame[i].nRenderPriority = nRenderPriority;
			return;
		}
	}

	AddFrame(pViewport, rc, nRenderPriority);
}

A3DRECT* AUIDialog::GetFrameRect(A3DViewport *pViewport)
{
	int i;

	for( i = 0; i < (int)m_vecFrame.size(); i++ )
	{
		if( pViewport == m_vecFrame[i].pA3DViewport )
			return &m_vecFrame[i].rc;
	}

	return NULL;
}

void AUIDialog::EnableMenuItem(const char *pszCommand, bool bEnable)
{
	int i;

	for( i = 0; i < (int)m_MenuItem.size(); i++ )
	{
		if( 0 == stricmp(m_MenuItem[i].strCommand, pszCommand) )
		{
			m_MenuItem[i].bEnable = bEnable;
			break;
		}
	}
}

void AUIDialog::CheckMenuItem(const char *pszCommand, bool bCheck)
{
	int i;

	for( i = 0; i < (int)m_MenuItem.size(); i++ )
	{
		if( 0 == stricmp(m_MenuItem[i].strCommand, pszCommand) )
		{
			m_MenuItem[i].bCheck = bCheck;
			break;
		}
	}
}

void AUIDialog::SaveRenderTarget()
{
	if( m_pUIRenderTarget )
	{
		AString filename = m_szName + ".dds";
		m_pUIRenderTarget->SaveRenderTarget(filename);
	}
}

bool AUIDialog::OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj) 
{
	const AUI_EVENT_MAP_TYPE *pMap = GetEventMap();
	
	do
	{
		if (ProcessEventMap(pMap->pMapEntries, uMsg, wParam, lParam, pObj))
			return true;
		pMap = pMap->pBaseMap;
	} while (pMap);
	
	return false;
}

bool AUIDialog::ProcessEventMap(const AUI_EVENT_MAP_ENTRY *pMapEntry, UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	while (pMapEntry->pFunc) 
	{ 
		if (uMsg == pMapEntry->uMessage &&
			(
			(!pObj && pMapEntry->szObjectName == "") ||
			(pObj && IsMatch(pObj->GetName(), pMapEntry->szObjectName, false)))) 
		{ 
			(this->*(pMapEntry->pFunc))(wParam, lParam, pObj); 
			return true; 
		} 
		pMapEntry++; 
	} 
	return false;
}

bool AUIDialog::OnCommandMap(const char *szCommand)
{
	const AUI_COMMAND_MAP_TYPE *pMap = GetCommandMap();
	
	do
	{
		// find in my command map
		if (ProcessCommandMap(pMap->pMapEntries, szCommand))
			return true;
		// not found in my command map
		// find in the base class'
		pMap = pMap->pBaseMap;
	} while (pMap);
	
	return false;
}

bool AUIDialog::ProcessCommandMap(const AUI_COMMAND_MAP_ENTRY *pMapEntry, const char *szCommand)
{
	while (pMapEntry->pFunc) 
	{
		if (IsMatch(szCommand, pMapEntry->szCommand, false))
		{ 
			// call the handler
			(this->*(pMapEntry->pFunc))(szCommand); 
			return true; 
		}
		pMapEntry++; 
	} 
	return false;
}



//////////////////////////////////////////////////////////////////////////
// DDX implement
//////////////////////////////////////////////////////////////////////////

void AUIDialog::UpdateData(bool bSave)
{
	DoDataExchange(bSave);
}


#define BEGIN_DDX_ON_SAVE(className) \
className * pControl = dynamic_cast<className *>(GetDlgItem(szControlName)); \
if(!pControl) \
{ \
	ASSERT(pControl); \
}else if (bSave) \
{ \

#define OR_DDX_ON_LOAD \
} \
else \
{

#define END_DDX \
}

#define SIMPLE_DDX(className, val, propFuncPostfix) \
BEGIN_DDX_ON_SAVE(className) \
 \
val = pControl->Get##propFuncPostfix(); \
 \
OR_DDX_ON_LOAD \
 \
pControl->Set##propFuncPostfix(val); \
 \
END_DDX


void AUIDialog::DDX_EditBox(bool bSave, const char *szControlName, float & fVal)
{
	BEGIN_DDX_ON_SAVE(AUIEditBox);

	ACString strVal = pControl->GetText();
	fVal = strVal.ToFloat();

	OR_DDX_ON_LOAD;

	ACString strVal;
	strVal.Format(_AL("%f"), fVal);
	pControl->SetText(strVal);

	END_DDX;
}

void AUIDialog::DDX_EditBox(bool bSave, const char *szControlName, int & nVal)
{
	BEGIN_DDX_ON_SAVE(AUIEditBox);
	
	ACString strVal = pControl->GetText();
	nVal = strVal.ToFloat();
	
	OR_DDX_ON_LOAD;
	
	ACString strVal;
	strVal.Format(_AL("%d"), nVal);
	pControl->SetText(strVal);
	
	END_DDX;
}

void AUIDialog::DDX_EditBox(bool bSave, const char *szControlName, ACString &strVal)
{
	SIMPLE_DDX(AUIEditBox, strVal, Text);
}

void AUIDialog::DDX_EditBox(bool bSave, const char *szControlName, ACHAR *szVal)
{
	BEGIN_DDX_ON_SAVE(AUIEditBox);
	
	a_strcpy(szVal, pControl->GetText());
	
	OR_DDX_ON_LOAD;
	
	pControl->SetText(szVal);
	
	END_DDX;
}

void AUIDialog::DDX_CheckBox(bool bSave, const char *szControlName, bool &bChecked)
{
	BEGIN_DDX_ON_SAVE(AUICheckBox);

	bChecked = pControl->IsChecked();

	OR_DDX_ON_LOAD;

	pControl->Check(bChecked);

	END_DDX;
}

void AUIDialog::DDX_ComboBox(bool bSave, const char *szControlName, int &nIndex)
{
	SIMPLE_DDX(AUIComboBox, nIndex, CurSel);

}

void AUIDialog::DDX_ListBox(bool bSave, const char *szControlName, int &nIndex)
{
	SIMPLE_DDX(AUIListBox, nIndex, CurSel);
}

void AUIDialog::DDX_Progress(bool bSave, const char *szControlName, int &nProgress)
{
	SIMPLE_DDX(AUIProgress, nProgress, Progress);
}

void AUIDialog::DDX_RadioButton(bool bSave, int nGroupID, int &nCheckedButtonID)
{
	if (bSave)
	{
		nCheckedButtonID = -1;
		AUIDIALOG_AUIOBJECTTABLE::iterator iter = m_AUIObjectTable.begin();
		for (; iter != m_AUIObjectTable.end(); ++iter)
		{
			AUIRadioButton *pControl = dynamic_cast<AUIRadioButton *>(*iter.value());
			if (pControl &&
				pControl->GetGroupID() == nGroupID &&
				pControl->IsChecked())
			{
				nCheckedButtonID = pControl->GetButtonID();
				return;
			}
		}
	}
	else
	{
		AUIDIALOG_AUIOBJECTTABLE::iterator iter = m_AUIObjectTable.begin();
		for (; iter != m_AUIObjectTable.end(); ++iter)
		{
			AUIRadioButton *pControl = dynamic_cast<AUIRadioButton *>(*iter.value());
			if (pControl &&
				pControl->GetGroupID() == nGroupID &&
				pControl->GetButtonID() == nCheckedButtonID)
			{
				CheckRadioButton(nGroupID, nCheckedButtonID);
				return;
			}
		}
	}
}


void AUIDialog::DDX_Scroll(bool bSave, const char *szControlName, int &nLevel)
{
	SIMPLE_DDX(AUIScroll, nLevel, BarLevel)
}


void AUIDialog::DDX_StillImageButton(bool bSave, const char *szControlName, bool &bPushed)
{
	BEGIN_DDX_ON_SAVE(AUIStillImageButton);

	bPushed = pControl->IsPushed();

	OR_DDX_ON_LOAD;

	pControl->SetPushed(bPushed);

	END_DDX;
}

void AUIDialog::DDX_StillImageButtonGroup(bool bSave, int &nIndexOfPushedButton, int nButtonCount, ...)
{
	if (bSave)
	{
		bool bAtLeastOneButtonIsPushed = false;
		va_list buttonNames;
		va_start(buttonNames, nButtonCount);
		for (int nIndex = 0; nIndex < nButtonCount; nIndex++)
		{
			const char * name = va_arg(buttonNames, const char *);
			AUIStillImageButton *pButton = dynamic_cast<AUIStillImageButton *>(GetDlgItem(name));
			ASSERT(pButton);
			if (pButton->IsPushed())
			{
				bAtLeastOneButtonIsPushed = true;
				nIndexOfPushedButton = nIndex;
				break;
			}
		}
		va_end(buttonNames);
		if (!bAtLeastOneButtonIsPushed)
			nIndexOfPushedButton = -1;
	}
	else
	{
		va_list buttonNames;
		va_start(buttonNames, nButtonCount);
		for (int nIndex = 0; nIndex < nButtonCount; nIndex++)
		{
			const char * name = va_arg(buttonNames, const char *);
			AUIStillImageButton *pButton = dynamic_cast<AUIStillImageButton *>(GetDlgItem(name));
			ASSERT(pButton);
			pButton->SetPushed(nIndex == nIndexOfPushedButton);
		}
		va_end(buttonNames);
	}
}


void AUIDialog::CheckStillImageButton(int nIndexToCheck, int nButtonCount, ...)
{
	va_list buttonNames;
	va_start(buttonNames, nButtonCount);
	for (int nIndex = 0; nIndex < nButtonCount; nIndex++)
	{
		const char * name = va_arg(buttonNames, const char *);
		AUIStillImageButton *pButton = dynamic_cast<AUIStillImageButton *>(GetDlgItem(name));
		ASSERT(pButton);
		pButton->SetPushed(nIndex == nIndexToCheck);
	}
	va_end(buttonNames);
}

void AUIDialog::MoveAllControls(int dx, int dy)
{
	if( dx == 0 && dy == 0 )
		return;

	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		POINT ptPos = pElement->pThis->GetPos(true);
		pElement->pThis->SetPos(ptPos.x + dx, ptPos.y + dy);
		pElement = pElement->pNext;
	}
}


void AUIDialog::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	m_nWidth = m_nDefaultWidth * fWindowScale;
	m_nHeight = m_nDefaultHeight * fWindowScale;

	if( m_pUIRenderTarget )
	{
		m_pUIRenderTarget->Release();
		m_pUIRenderTarget->Init(m_pA3DDevice, m_nWidth, m_nHeight);
		UpdateRenderTarget();
	}
	m_fDisplayHeight =  m_nFontHeight * fWindowScale;
	FitFont();

	int W, H;
	
	W = rcNew.Width();
	H = rcNew.Height();
	AdjustFromPercent(W, H);
	AdjustFromXY(W, H);

	PAUIOBJECTLISTELEMENT pElement;
	PAUIOBJECT pObj;
	pElement = m_pListHead;
	while( pElement )
	{
		pObj = pElement->pThis;
		pObj->Resize();
		pElement = pElement->pNext;
	}
	if( m_pAUIFrameTitle )
		m_pAUIFrameTitle->SetScale(fWindowScale);
	if( m_pAUIFrameDlg )
		m_pAUIFrameDlg->SetScale(fWindowScale);
	if( m_pA2DSpriteHilight )
	{
		float fScaleX = float(m_nMaxMenuTextWidth * fWindowScale) / float(m_pA2DSpriteHilight->GetWidth());
		float fScaleY = float(m_nMaxMenuTextHeight * fWindowScale) / float(m_pA2DSpriteHilight->GetHeight());
		m_pA2DSpriteHilight->SetScaleX(fScaleX);
		m_pA2DSpriteHilight->SetScaleY(fScaleY);
	}
}

bool AUIDialog::ReadNameFromXML(const char *pszTempName, AString& strName, bool& bHasSubDlg)
{
	AXMLFile aXMLFile;
	AString strFile("Interfaces\\");
	strFile += pszTempName;
	bHasSubDlg = false;

	if (!aXMLFile.LoadFromFile(strFile))
		return AUI_ReportError(__LINE__, 1, "AUIDialog::ReadNameFromXML(), Failed to load %s", pszTempName);

	AXMLItem* pItem = aXMLFile.GetRootItem()->GetFirstChildItem();

	if (a_stricmp(pItem->GetName(), _AL("DIALOG")) != 0) 
		return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to load %s", pszTempName);

	XMLGET_STRING_VALUE(pItem, _AL("Name"), strName)

	pItem = pItem->GetFirstChildItem();
	while( pItem )
	{
		if( a_stricmp(pItem->GetName(), _AL("SUBDIALOG")) == 0 )
		{
			bHasSubDlg = true;
			break;
		}
	
		pItem = pItem->GetNextItem();
	}

	aXMLFile.Release();
	return true;
}

bool AUIDialog::InitFromXML(const char *pszTempName)
{
	AXMLFile aXMLFile;
	AString strFile("Interfaces\\");
	m_szFilename = pszTempName;
	strFile += pszTempName;
	if( !aXMLFile.LoadFromFile(strFile) )
		return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to load %s", pszTempName);
	AXMLItem *pItem = aXMLFile.GetRootItem()->GetFirstChildItem();
	if( a_stricmp(pItem->GetName(), _AL("DIALOG")) != 0 )
		return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to load %s", pszTempName);
	
	if (!InitFrameXML(pItem))
	{
		aXMLFile.Release();
		return false;
	}

	pItem = pItem->GetFirstChildItem();
	bool bval = true;
	while( pItem )
	{
		if( a_stricmp(pItem->GetName(), _AL("CHECK")) == 0 )
		{
			PAUICHECKBOX pObject = new AUICheckBox;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUICheckBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("COMBO")) == 0 )
		{
			PAUICOMBOBOX pObject = new AUIComboBox;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIComboBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("CONSOLE")) == 0 )
		{
			PAUICONSOLE pObject = new AUIConsole;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIConsole");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("EDIT")) == 0 )
		{
			PAUIEDITBOX pObject = new AUIEditBox;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIEditBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("IMAGEPICTURE")) == 0 )
		{
			PAUIIMAGEPICTURE pObject = new AUIImagePicture;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIImagePicture");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("LABEL")) == 0 )
		{
			PAUILABEL pObject = new AUILabel;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUILabel");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("LIST")) == 0 )
		{
			PAUILISTBOX pObject = new AUIListBox;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIListBox");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("PROGRESS")) == 0 )
		{
			PAUIPROGRESS pObject = new AUIProgress;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIProgress");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("RADIO")) == 0 )
		{
			PAUIRADIOBUTTON pObject = new AUIRadioButton;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIRadioButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("SCROLL")) == 0 )
		{
			PAUISCROLL pObject = new AUIScroll;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIScroll");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("SLIDER")) == 0 )
		{
			PAUISLIDER pObject = new AUISlider;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUISlider");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("STILLIMAGEBUTTON")) == 0 )
		{
			PAUISTILLIMAGEBUTTON pObject = new AUIStillImageButton;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIStillImageButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("SUBDIALOG")) == 0 )
		{
			PAUISUBDIALOG pObject = new AUISubDialog;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIStillImageButton");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("TEXT")) == 0 )
		{
			PAUITEXTAREA pObject = new AUITextArea;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUITextArea");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("TREE")) == 0 )
		{
			PAUITREEVIEW pObject = new AUITreeView;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUITreeView");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("VERTICALTEXT")) == 0 )
		{
			PAUIVERTICALTEXT pObject = new AUIVerticalText;
			if( !pObject ) return false;
			pObject->SetParent(this);

			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIVerticalText");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		else if( a_stricmp(pItem->GetName(), _AL("WINDOWPICTURE")) == 0 )
		{
			PAUIWINDOWPICTURE pObject = new AUIWindowPicture;
			if( !pObject ) return false;
			pObject->SetParent(this);
			bval = pObject->InitXML(m_pA3DEngine, m_pA3DDevice, pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to init control AUIWindowPicture");

			bval = AddObjectToList(pObject);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::Init(), Failed to call AddObjectToList()");
		}
		
		pItem = pItem->GetNextItem();
	}

	aXMLFile.Release();

	UpdateData(false);
	return OnInitDialog();
}

bool AUIDialog::IsOutOfWindowParamValid(int nWindowWidth, int nWindowHeight)
{
	if (m_nMinDistDlgBToScrT <= 0 || m_nMinDistDlgLToScrR <= 0 || m_nMinDistDlgRToScrL <= 0 || m_nMinDistDlgTToScrB <= 0)
		return false;

	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	if ( (m_nMinDistDlgBToScrT * fWindowScale + m_nMinDistDlgTToScrB * fWindowScale - m_nHeight) > nWindowHeight ||
		 (m_nMinDistDlgLToScrR * fWindowScale + m_nMinDistDlgRToScrL * fWindowScale - m_nWidth) > nWindowWidth )
		 return false;

	return true;
}

void AUIDialog::AdjustFromXY(int nWindowWidth, int nWindowHeight)
{
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
				
	if( !m_bCanOutOfWindow && m_x < 2 && (int)nWindowWidth - m_x - m_nWidth >= 2 )
	{
		m_x = 0;
		m_fPercentX = -2;
	}
	else if( !m_bCanOutOfWindow && m_x >= 2 && (int)nWindowWidth - m_x - m_nWidth < 2 )
	{
		m_x = nWindowWidth - m_nWidth;
		m_fPercentX = 2;
	}
	else
	{
		if (IsOutOfWindowParamValid(nWindowWidth, nWindowHeight))
			a_Clamp(m_x, int(m_nMinDistDlgRToScrL * fWindowScale)-m_nWidth, nWindowWidth-int(m_nMinDistDlgLToScrR * fWindowScale));
					
		m_fPercentX = (float(m_nWidth) / 2 + m_x) / nWindowWidth;
	}
				
	if( !m_bCanOutOfWindow && m_y < 2 && (int)nWindowHeight - m_y - m_nHeight >= 2 )
	{
		m_y = 0;
		m_fPercentY = -2;
	}
	else if( !m_bCanOutOfWindow && m_y >=2 && (int)nWindowHeight - m_y - m_nHeight < 2 )
	{
		m_y = nWindowHeight - m_nHeight;
		m_fPercentY = 2;
	}
	else
	{
		if (IsOutOfWindowParamValid(nWindowWidth, nWindowHeight))
			a_Clamp(m_y, int(m_nMinDistDlgBToScrT * fWindowScale) - m_nHeight, nWindowHeight -int(m_nMinDistDlgTToScrB * fWindowScale));
					
		m_fPercentY = (float(m_nHeight) / 2 + m_y) / nWindowHeight;
	}
}

void AUIDialog::AdjustFromPercent(int nWindowWidth, int nWindowHeight)
{
	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	int W = nWindowWidth;
	int H = nWindowHeight;

	if( m_fPercentX == -2 )
		m_x = 0;
	else if( m_fPercentX == 2 )
		m_x = W - m_nWidth;
	else
	{
		m_x = W * m_fPercentX - float(m_nWidth) / 2;
		if (IsOutOfWindowParamValid(W, H))
			a_Clamp(m_x, int(m_nMinDistDlgRToScrL * fWindowScale)-m_nWidth, W-int(m_nMinDistDlgLToScrR * fWindowScale));
	}

	if( m_fPercentY == -2 )
		m_y = 0;
	else if( m_fPercentY == 2 )
		m_y = H - m_nHeight;
	else
	{
		m_y = H * m_fPercentY - float(m_nHeight) / 2;
		if (IsOutOfWindowParamValid(W, H))
			a_Clamp(m_y, int(m_nMinDistDlgBToScrT * fWindowScale)-m_nWidth, H-int(m_nMinDistDlgTToScrB * fWindowScale));
	}
}

void AUIDialog::FitFont()
{
	if (!m_pFont)
	{
		return;
	}
	AUIManager* pAUIMan = GetAUIManager();
	if (!pAUIMan)
	{
		ASSERT(0);
		return;
	}
	
	A3DFTFontMan* pFontMan = pAUIMan->GetA3DFTFontMan();
	if (!pFontMan)
	{
		ASSERT(0);
		return;
	}
	m_pFont = pFontMan->CreatePointFont(m_fDisplayHeight, m_pFont);
}

bool AUIDialog::ApplyTemplates()
{
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if (!pElement->pThis->ApplyTemplate())
			return false;

		pElement = pElement->pNext;
	}

	return true;
}

// return deleted element's prev-postion
bool AUIDialog::DeleteControl(PAUIOBJECT pAUIObject, PAUIOBJECTLISTELEMENT& pos)
{
	bool bFind = false;

	pos = NULL; 
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis == pAUIObject )
		{
			bFind = true;
			pos = pElement->pLast;

			if( pAUIObject == m_pAUIManager->m_pObjMouseOn)
				m_pAUIManager->m_pObjMouseOn = NULL;
			if( pAUIObject == m_pObjFocus )
				ChangeFocus(NULL);
			if( pAUIObject == m_pObjCapture)
				m_pObjCapture = NULL;
			if( pAUIObject == m_pObjEvent )
				m_pObjEvent = NULL;
			
			PAUIOBJECTLISTELEMENT pDelete = pElement;			

			if( pDelete->pLast ) pDelete->pLast->pNext = pDelete->pNext;
			if( pDelete->pNext ) pDelete->pNext->pLast = pDelete->pLast;
			if( pDelete == m_pListHead ) m_pListHead = pDelete->pNext;
			if( pDelete == m_pListTail ) m_pListTail = pDelete->pLast;
			
			size_t s = m_AUIObjectTable.erase(pAUIObject->GetName());
			ASSERT(s);

			A3DRELEASE(pDelete->pThis);
			delete pDelete;
			
			m_nListSize--;
			if( m_nListSize <= 0 )
			{
				m_pListHead = NULL;
				m_pListTail = NULL;
			}
			
			break;
		}
		pElement = pElement->pNext;
	}

	return bFind;
}

PAUIOBJECTLISTELEMENT AUIDialog::InsertControl(PAUIOBJECTLISTELEMENT pos, AUIObject& obj)
{
	if (pos == NULL || pos == m_pListTail || (( !m_pListHead && !m_pListTail )))
	{
		AddObjectToList(&obj, false);
		return m_pListTail;
	}

	bool bval = m_AUIObjectTable.put(obj.GetName(), &obj);
	if (!bval)
		return NULL;

	PAUIOBJECTLISTELEMENT pElement = new AUIOBJECTLISTELEMENT;
	if (!pElement)
		return NULL;
	pElement->pThis = &obj;
	pElement->bHeapAlloc = false;

	if (pos->pNext)
		pos->pNext->pLast = pElement;
	pElement->pNext = pos->pNext;

	pos->pNext = pElement;
	pElement->pLast = pos;
	
	return pElement;
}

bool AUIDialog::ReplaceControl(PAUIOBJECT pObjOld, AUIObject& objNew)
{
	if (pObjOld == &objNew)
		return true;

	PAUIOBJECTLISTELEMENT pos;
	if (!DeleteControl(pObjOld, pos))
		return false;

	if (NULL == InsertControl(pos, objNew))
		return false;

	return true;
}

bool AUIDialog::AddObjectToList(PAUIOBJECT pAUIObject, bool bHeapAlloc)
{
	if( !m_pListHead && !m_pListTail )
	{
		m_pListHead = new AUIOBJECTLISTELEMENT;
		if( !m_pListHead ) return false;

		m_pListHead->bHeapAlloc = bHeapAlloc;
		m_pListHead->id = 0;
		m_pListHead->pLast = NULL;
		m_pListHead->pThis = pAUIObject;
		m_pListHead->pNext = NULL;

		m_pListTail = m_pListHead;
	}
	else
	{
		PAUIOBJECTLISTELEMENT pElement = new AUIOBJECTLISTELEMENT;
		if( !pElement ) return false;

		m_pListTail->pNext = pElement;

		pElement->bHeapAlloc = bHeapAlloc;
		pElement->id = m_pListTail->id + 1;
		pElement->pLast = m_pListTail;
		pElement->pThis = pAUIObject;
		pElement->pNext = NULL;

		m_pListTail = pElement;
	}
	m_nListSize++;

	bool bval = m_AUIObjectTable.put(pAUIObject->GetName(), pAUIObject);
	return bval;
}

bool AUIDialog::RenderRecurse()
{
	int nRenderState = m_nRenderState;
	RenderOnce();
	m_nRenderState = nRenderState;
	
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while (pElement)
	{
		if (pElement->pThis && pElement->pThis->IsShow() && pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
		{
			AUISubDialog* pSubDlg = dynamic_cast<AUISubDialog*> (pElement->pThis);
			if (pSubDlg && pSubDlg->GetSubDialog())
			{
				if (!pSubDlg->GetSubDialog()->RenderRecurse())
				{
					return AUI_ReportError("AUIDialog::InnerRenderToTexture Fail");
				}
			}
		}

		pElement = pElement->pNext;
	}

	return true;
}

bool AUIDialog::RenderToRT(UIRenderTarget* pTarget)
{
	if (!pTarget)
		return false;

	bool bRet;

	bRet = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( ! bRet ) 
	{
		a_LogOutput(__LINE__, "AUIDialog::RenderToRT(), A3DEngine.FlushInternalSpriteBuffer Failed");
		return false;
	}

	m_pAUIManager->GetA3DFTFontMan()->Flush();

	int x = m_x;
	int y = m_y;
	m_x = 0;
	m_y = 0;

	if (!pTarget->BeginRenderRTAlpha())
	{
		a_LogOutput(__LINE__, "AUIDialog::RenderToRT(), UIRenderTarget.BeginRenderRTAlpha Failed");
		return false;
	}

	RenderOnce();

	bRet = pTarget->BeginRenderRT();
	if( ! bRet )
	{
		a_LogOutput(__LINE__, "AUIDialog::RenderToRT(), UIRenderTarget.BeginRenderRT Failed");
		return false;
	}

	RenderOnce();

	bRet = pTarget->EndRenderRT();
	if( ! bRet )
	{
		a_LogOutput(__LINE__, "AUIDialog::RenderToRT(), UIRenderTarget.EndRenderRT Failed");
		return false;
	}

	m_x = x;
	m_y = y;

	return true;
}

bool AUIDialog::RenderRTToTexture(A3DTexture* pTexture)
{
	if (!pTexture)
		return false;

	UIRenderTarget* pTarget = new UIRenderTarget;

	int x = m_x;
	int y = m_y;
	m_x = 0;
	m_y = 0;
	if (!pTarget->Init(m_pA3DDevice, m_nWidth, m_nHeight))
	{
		A3DRELEASE(pTarget);
		a_LogOutput(__LINE__, "AUIDialog::RenderRTToTexture(), UIRenderTarget Init Failed");
		return false;
	}

	RenderToRT(pTarget);

	A3DSurface suf;
#ifdef _ANGELICA2
	bool bval = suf.Init(m_pA3DDevice, pTarget->GetUIRenderTarget()->GetTargetSurface(), "");	
#else
	bool bval = suf.Init(m_pA3DDevice, pTarget->GetUIRenderTarget()->GetTargetSurface()->m_pD3DSurface, "");
#endif
	if (bval)
	{
		pTexture->Release();
		pTexture->Create(m_pA3DDevice, m_nWidth, m_nHeight, A3DFMT_A8R8G8B8);

		// copy
		if (!pTexture->UpdateFromSurface(&suf, A3DRECT(0,0,m_nWidth,m_nHeight)))
		{
			A3DRELEASE(pTarget);
			return false;
		}
	}

	A3DRELEASE(pTarget);
	return true;
}

bool AUIDialog::RenderFrameInSubdialog()
{
	if (!m_pParentDlgControl || !m_pParentDlgControl->GetSubDialogViewport())
		return true;

	// prepare frame
	PAUIFRAME pFrame = NULL;
	if (m_pParentDlgControl->IsRenderDlgFrame())
		pFrame = m_pAUIFrameDlg;
	else 
		pFrame = m_pParentDlgControl->GetFrame();
	if (!pFrame)
		return true;

	// prepare viewport
	A3DViewport *pOldViewport = m_pA3DEngine->GetActiveViewport();
	A3DViewport *pViewport = m_pParentDlgControl->GetSubDialogViewport();
	A3DRECT rcViewport(0,0,0,0);
	for (size_t i=0; i<m_vecFrame.size(); i++)
	{
		if (m_vecFrame[i].pA3DViewport == pViewport)
		{
			rcViewport = m_vecFrame[i].rc;
			break;
		}
	}
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT dPos = GetPos();
	dPos.x += rcWindow.left;
	dPos.y += rcWindow.top;

	float fWindowScale = m_pAUIManager->GetWindowScale() * m_fScale;
	A3DRECT rcDlg(dPos.x + rcViewport.left * fWindowScale, 
				  dPos.y + rcViewport.top * fWindowScale, 
				  dPos.x + rcViewport.left * fWindowScale + rcViewport.Width() * fWindowScale, 
				  dPos.y + rcViewport.top * fWindowScale + rcViewport.Height() * fWindowScale);
	A3DRECT rcInter = rcDlg & rcWindow;
	if( rcInter.Width() <= 0 || rcInter.Height() <= 0 )
		return true;

	A3DVIEWPORTPARAM *pClip = pViewport->GetParam();
	pClip->X = rcInter.left;
	pClip->Y = rcInter.top;
	pClip->Width = rcInter.Width();
	pClip->Height = rcInter.Height();

	bool bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderControlsInViewport(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");
	m_pAUIManager->GetA3DFTFontMan()->Flush();
	pViewport->SetParam(pClip);
	pViewport->Active();
	
	// render in viewport
	int x = dPos.x - m_pParentDlgControl->GetHBarPos();
	int y = dPos.y - m_pParentDlgControl->GetVBarPos();
	int w = pClip->Width + m_pParentDlgControl->GetHBarMax();
	int h = pClip->Height + m_pParentDlgControl->GetVBarMax();
	bval = pFrame->Render(x, y, w, h, (m_pFont ? m_pFont : m_pAUIManager->GetDefaultFont()), 
		m_color, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, m_nAlpha * m_nWholeAlpha / 255, true, m_nShadow);

	pOldViewport->Active();

	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIDialog::RenderFrameInSubdialog(), Failed to call m_pAUIFrameDlg->Render()");

	return true;
}

bool AUIDialog::IsDescendant(const AUIDialog* pDlg) const
{
	if (!pDlg)
		return false;

	if (!m_pParentDlgControl)
		return false;

	PAUIDIALOG pDlgParent = m_pParentDlgControl->GetParent();
	if (!pDlgParent)
		return false;

	if (pDlg == pDlgParent)
		return true;

	return pDlgParent->IsDescendant(pDlg);
}