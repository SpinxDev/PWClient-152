// Filename	: AUIManager.cpp
// Creator	: Tom Zhou
// Date		: May 12, 2004
// Desc		: AUIManager is a manager of all AUI controls. We need to derive it
//			  to fit our specified game requirement.

#include "AUI.h"

#include <Windows.h>
#include <WindowsX.h>

#include "AF.h"
#include "AM.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "AScriptFile.h"
#include "AWScriptFile.h"
#include "A3DFTFontMan.h"

#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "AUIDialog.h"
#include "AUIEditBox.h"
#include "AUIComboBox.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUITreeView.h"
#include "AUIStillImageButton.h"
#include "AUISubDialog.h"
#include "AUICTranslate.h"
#include "CSplit.h"
#include "AUIImeHook.h"
#include "WndThreadManager.h"

#include "FTInterface.h"
#include "A2DDotLineMan.h"
#include "ImeUi.h"
#include "IMEWrapper.h"

#ifdef _ANGELICA_AUDIO
#include "FAudioInstance.h"
#include "FEventSystem.h"
#include "FEvent.h"
#endif

#ifdef _ANGELICA22
#include "AGPAPerfIntegration.h"
#endif

extern bool g_bHideIme;
extern int _tab_char;
static LPCWSTR _pate_font_face = L"方正细黑一简体";
static const float _pate_font_size	= 12.0f;

AUIManager::AUIManager()
{
	int i;

	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
	m_hWnd = NULL;
	m_pLoadingCallBack = NULL;
	m_nDefaultWidth = AUIMANAGER_DEFAULTWIDTH;
	m_nDefaultHeight = AUIMANAGER_DEFAULTHEIGHT;
	m_pWndThreadMan = NULL;
	m_szName = "";
	m_pDlgActive = NULL;
	m_pDlgHint = NULL;
	m_pFont = NULL;
	m_pPateFont = NULL;
	m_pFontMan = NULL;
	m_bOwnFontMan = true;
	m_nWordWidth = 0;
	m_pDotlineMan = NULL;
	m_pDotline = NULL;

	UINT a_uKeyboardMsg[] =
	{
		WM_CHAR,
		WM_KEYDOWN,
		WM_KEYUP,
		WM_SYSCHAR,
		WM_SYSKEYDOWN,
		WM_SYSKEYUP,
		WM_IME_CHAR,
		WM_INPUTLANGCHANGE,
		WM_IME_STARTCOMPOSITION,
		WM_IME_COMPOSITION,
		WM_IME_ENDCOMPOSITION,
		WM_IME_NOTIFY,
		WM_IME_SETCONTEXT,
		WM_USER_IME_CHANGE_FOCUS,
	};
	for( i = 0; i < sizeof(a_uKeyboardMsg) / sizeof(UINT); i++ )
		m_KeyboardMsg[a_uKeyboardMsg[i]] = 1;

	UINT a_uMouseMsg[] =
	{
		WM_MOUSEWHEEL,
		WM_LBUTTONDBLCLK,
		WM_LBUTTONDOWN,
		WM_LBUTTONUP,
		WM_MBUTTONDBLCLK,
		WM_MBUTTONDOWN,
		WM_MBUTTONUP,
		WM_MOUSEMOVE,
		WM_RBUTTONDBLCLK,
		WM_RBUTTONDOWN,
		WM_RBUTTONUP
	};
	for( i = 0; i < sizeof(a_uMouseMsg) / sizeof(UINT); i++ )
		m_MouseMsg[a_uMouseMsg[i]] = 1;

	m_pDlgMouseOn = NULL;
	m_pObjMouseOn = NULL;
	m_pObjHint = NULL;
	m_pObjHintSel = NULL;
	m_szMsgBoxFrame = "";
	m_szMsgBoxButton = "";
	m_szMsgBoxButtonDown = "";
	m_pAUIFrameHint = NULL;
	m_pA2DSpriteCover = NULL;
	m_bUseCover = true;
	m_nHintDelay = GetTickCount();
	m_fWindowScale = 1.0f;
	m_strDefaultFontName = _AL("方正细黑一简体");
	m_strDefaultDotlineFileName = "Surfaces\\dotline.tga";
	m_nDefaultFontSize = (int)_pate_font_size;
	m_strHint = _AL("");

	for( i = 0; i < AUIMANAGER_MAX_EMOTIONGROUPS; i++ )
	{
		m_pA2DSpriteEmotion[i] = NULL;
		m_vecEmotion[i] = NULL;
	}
	m_pvecImageList = NULL;
	m_pvecHintOtherFonts = NULL;
	m_bUseFontAdapt = false;

#ifdef _ANGELICA_AUDIO
	m_pAudioEvtSystem = NULL;
	//m_pCurAudioInst = NULL;
#endif
}

AUIManager::~AUIManager()
{
}

bool AUIManager::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, 
					  const char *pszFilename, int nDefaultWidth, int nDefaultHeight)
{
	bool bval = true;

	ASSERT(pA3DEngine && pA3DDevice);

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_nDefaultWidth = nDefaultWidth;
	m_nDefaultHeight = nDefaultHeight;
	af_GetFileTitle(pszFilename, m_strFilename);

	m_pWndThreadMan = new CWndThreadManager;
	if( !m_pWndThreadMan ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), no enough memory");
	
	int W, H, i, j, nIndex;
	for(int l = 0; l < AUIMANAGER_MAX_EMOTIONGROUPS; l++ )
	{
		char szFile[MAX_PATH];
		sprintf(szFile, "InGame\\Emotions%d.dds", l);
		AString strPath;
		strPath.Format("%s%s", "Surfaces\\", szFile);
		if( !af_IsFileExist(strPath) )
			break;

		m_pA2DSpriteEmotion[l] = new A2DSprite;
		if( !m_pA2DSpriteEmotion[l] ) return AUI_ReportError(__LINE__, __FILE__);
		bval = m_pA2DSpriteEmotion[l]->Init(m_pA3DDevice, szFile, AUI_COLORKEY);
		if( !bval ) break;
		m_vecEmotion[l] = new abase::vector<AUITEXTAREA_EMOTION>;
		if( !m_vecEmotion[l] ) return AUI_ReportError(__LINE__, __FILE__);

		W = H = 32;

		A3DRECT *rc;
		int nNumX = m_pA2DSpriteEmotion[l]->GetWidth() / W;
		int nNumY = m_pA2DSpriteEmotion[l]->GetHeight() / H;

		rc = new A3DRECT[nNumX * nNumY];
		for( i = 0; i < nNumY; i++ )
		{
			for( j = 0; j < nNumX; j++ )
			{
				nIndex = i * nNumX + j;
				rc[nIndex].SetRect(j * W, i * H, j * W + W, i * H + H);
			}
		}
		m_pA2DSpriteEmotion[l]->ResetItems(nNumX * nNumY, rc);
		delete [] rc;

		AWScriptFile s;
		AUITEXTAREA_EMOTION em;
		sprintf(szFile, "Surfaces\\InGame\\Emotions%d.txt", l);
		bval = s.Open(szFile);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);

		while( s.GetNextToken(true) )
		{
			em.nStartPos = a_atoi(s.m_szToken);
			s.GetNextToken(true);
			em.nNumFrames = min(a_atoi(s.m_szToken), AUITEXTAREA_EMOTHION_MAXFRAME);

			s.GetNextToken(true);
			em.strHint = s.m_szToken;
			for(i = 0; i < em.nNumFrames; i++ )
			{
				s.GetNextToken(true);
				em.nFrameTick[i] = a_atoi(s.m_szToken);
			}
			m_vecEmotion[l]->push_back(em);
		}
		s.Close();
	}

	m_rcWindow.SetRect(0, 0, m_nDefaultWidth, m_nDefaultHeight);

	SetHideIme(!pA3DDevice->GetDevFormat().bWindowed);
	::CreateCaret(m_pA3DDevice->GetDeviceWnd(), NULL, 1, 10);

	if (!m_pFontMan)
	{
		m_pFontMan = new A3DFTFontMan;
		bval = m_pFontMan->Create(m_pA3DDevice);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), failed to call m_pFontMan->Create()");
	}

	if( af_IsFileExist("Surfaces\\Frame\\Frame.tga") )
	{
		m_pAUIFrameHint = new AUIFrame;
		if( !m_pAUIFrameHint ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), no enough memory");

		bval = m_pAUIFrameHint->Init(pA3DEngine, pA3DDevice, "Frame\\Frame.tga");
		if( !bval ) A3DRELEASE(m_pAUIFrameHint);
	}	

	m_szMsgBoxFrame = "MsgBoxFrame.tga";
	m_szMsgBoxButton = "Button\\Button.tga";
	m_szMsgBoxButtonDown = "Button\\ButtonDown.tga";
	m_szMsgBoxButtonHover = "Button\\ButtonHover.tga";
	
	if( af_IsFileExist("Surfaces\\Cover.tga") )
	{
		m_pA2DSpriteCover = new A2DSprite;
		if( !m_pA2DSpriteCover ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), no enough memory");
		
#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSpriteCover->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSpriteCover->Init(pA3DDevice, "Cover.tga", AUI_COLORKEY);
		if( !bval ) A3DRELEASE(m_pA2DSpriteCover);
	}

	// Init Dotline
	if (!m_pDotlineMan)
	{
		m_pDotlineMan = new A2DDotLineMan;
		if (!m_pDotlineMan->Create(m_pA3DDevice))
		{
			delete m_pDotlineMan;
			m_pDotlineMan = NULL;
			return false;
		}
	}
	if (!m_pDotline)
	{
		m_pDotline = new A2DDotLine(m_pDotlineMan);
		if (!m_pDotline->Create(m_pA3DDevice, m_strDefaultDotlineFileName))
		{
			a_LogOutput(0, "AUIManager::Init() dot line create failed. %s", (const char*)(m_strDefaultDotlineFileName));
		}
	}

	if( pszFilename )
	{
		AString strTabFile(pszFilename);
		m_StringTable.clear();
		
		strTabFile.MakeLower();
		char *pch = (char*)strstr(strTabFile, ".");
		ASSERT(pch);
		strcpy(pch, ".stf");	// String table file.
		bval = ImportStringTable(strTabFile);		// Ignore error.

		pch = (char*)strstr(strTabFile, ".");
		ASSERT(pch);
		strcpy(pch, ".xml");

		bval = ImportUIParam(strTabFile);
	}

	m_pFont = m_pFontMan->CreatePointFont(m_nDefaultFontSize, m_strDefaultFontName, STYLE_NORMAL);
	if( !m_pFont ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), failed to call m_pFontMan->CreatePointFont()");

	m_pPateFont = m_pFontMan->CreatePointFont(_pate_font_size, _pate_font_face, STYLE_NORMAL, 0, 0, 0, 0, false, 0, 0, true);
	if( !m_pPateFont ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), failed to call m_pFontMan->CreatePointFont()");

	InitOtherFonts();
	
	if( pszFilename )
	{
		bval = LoadDialogLayout(pszFilename);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Init(), failed to call LoadDialogLayout()");

		char szName[MAX_PATH];
		af_GetFileTitle(pszFilename, szName, sizeof(szName));
		char *pch = strrchr(szName, '.');
		if( pch )
			*pch = '\0';
		SetName(szName);
	}

	if( pszFilename )
	{
		A3DRECT rcNew;
		A3DDEVFMT fmt = m_pA3DEngine->GetA3DDevice()->GetDevFormat();
		rcNew.SetRect(0, 0, fmt.nWidth, fmt.nHeight);
		ResizeWindows(m_rcWindow, rcNew);
	}
	AUIEditBox::SetHWnd(pA3DDevice->GetDeviceWnd());

	return true;
}

bool AUIManager::Release(void)
{
	SetHideIme(false);

	m_DlgZOrder.RemoveAll();
	m_DlgZOrderBack.RemoveAll();
	m_DlgName.clear();
	int i;
	for( i = 0; i < (int)m_aDialog.size(); i++ )
		A3DRELEASE(m_aDialog[i]);
	m_aDialog.clear();

	m_MouseMsg.clear();
	m_KeyboardMsg.clear();
	m_StringTable.clear();

	A3DRELEASE(m_pAUIFrameHint);
	A3DRELEASE(m_pA2DSpriteCover);

	if (m_bOwnFontMan)
		A3DRELEASE(m_pFontMan);

	for( i = 0; i < AUIMANAGER_MAX_EMOTIONGROUPS; i++ )
	{
		if( m_vecEmotion[i] )
		{
			m_vecEmotion[i]->clear();
			delete m_vecEmotion[i];
		}
		A3DRELEASE(m_pA2DSpriteEmotion[i]);
	}

	SAFE_DELETE(m_pWndThreadMan)

	A3DRELEASE(m_pDotlineMan);

#ifdef _ANGELICA_AUDIO
	m_pAudioEvtSystem = NULL;
	//m_pCurAudioInst = NULL;
#endif

	return true;
}

bool AUIManager::Save(const char *pszFilename)
{
	FILE *file = fopen(pszFilename, "wt");
	if( !file ) return false;

	for( int i = 0; i < (int)m_aDialog.size(); i++ )
	{
		fprintf(file, "%s %d %d %d\n", m_aDialog[i]->GetFilename(),
			m_aDialog[i]->GetPos().x, m_aDialog[i]->GetPos().y, m_aDialog[i]->IsShow());
	}

	fclose(file);

	return true;
}

bool AUIManager::ImportStringTable(const char *pszFilename)
{
	bool bval = true;
	AWScriptFile s;
	char szFilename[MAX_PATH];

	sprintf(szFilename, "Interfaces\\%s", pszFilename);
	bval = s.Open(szFilename);
	if( !bval ) return false;

	while( !s.IsEnd() )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.
		int idString = a_atoi(s.m_szToken);

		bval = s.GetNextToken(true);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::ImportStringTable(), failed to read from file");

		AUICTranslate tran;
		ACString str(tran.Translate(s.m_szToken));
		m_StringTable[idString] = str;
	}

	s.Close();

	if( a_stricmp(GetStringFromTable(1), _AL("") ) == 0 )	//1 默认字体
		m_StringTable[1] = _AL("方正细黑一简体");
	m_strDefaultFontName = GetStringFromTable(1);
	if( a_stricmp(GetStringFromTable(2), _AL("") ) == 0 )	//2 默认字体大小
		m_StringTable[2] = _AL("10");
	m_nDefaultFontSize = a_atoi(GetStringFromTable(2));
	if( a_stricmp(GetStringFromTable(3), _AL("") ) == 0 )	//3 符号 '\t' 相当于多少个 'W'的宽度
		m_StringTable[3] = _AL("30");
	_tab_char = a_atoi(GetStringFromTable(3));
	if( a_stricmp(GetStringFromTable(4), _AL("") ) == 0 )	//4 m_FontImagePicture 字体大小
		m_StringTable[4] = m_StringTable[2];
	if( a_stricmp(GetStringFromTable(5), _AL("") ) == 0 )	//5 MessageBox 字体大小
		m_StringTable[5] = m_StringTable[2];
	if( a_stricmp(GetStringFromTable(6), _AL("") ) == 0 )	//6 MessageBox shadow
		m_StringTable[6] = _AL("0");
	if( a_stricmp(GetStringFromTable(7), _AL("") ) == 0 )	//7 MessageBox outline
		m_StringTable[7] = _AL("0");
	if( a_stricmp(GetStringFromTable(8), _AL("") ) == 0 )	//8 MessageBox outline color
		m_StringTable[8] = _AL("0");

	m_FontHint.szFontName = GetStringFromTable(1);
	m_FontHint.nFontSize = a_atoi(GetStringFromTable(2));
	m_FontImagePicture.szFontName = GetStringFromTable(1);
	m_FontImagePicture.nFontSize = a_atoi(GetStringFromTable(4));
	m_FontImagePicture.nOutline = 1;
	m_FontMessageBox.szFontName = GetStringFromTable(1);
	m_FontMessageBox.nFontSize	= a_atoi(GetStringFromTable(5));
	m_FontMessageBox.nShadow	= a_atoi(GetStringFromTable(6));
	m_FontMessageBox.nOutline	= a_atoi(GetStringFromTable(7));
	swscanf(GetStringFromTable(8),_AL("%X"),&(m_FontMessageBox.clOutline));

	return true;
}

bool AUIManager::ImportUIParam(const char *pszFilename)
{
	AXMLFile aXMLFile;
	char szFilename[MAX_PATH];
	sprintf(szFilename, "Interfaces\\%s", pszFilename);

	if( !aXMLFile.LoadFromFile(szFilename) )
		return true;

	AXMLItem *pItem = aXMLFile.GetRootItem()->GetFirstChildItem();

	while( pItem )
	{
		if( a_stricmp(pItem->GetName(), _AL("HintFont")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pItem, _AL("FontName"), m_FontHint.szFontName)
			XMLGET_INT_VALUE(pItem, _AL("FontSize"), m_FontHint.nFontSize)
			XMLGET_INT_VALUE(pItem, _AL("Shadow"), m_FontHint.nShadow)
			XMLGET_BOOL_VALUE(pItem, _AL("Bold"), m_FontHint.bBold)
			XMLGET_BOOL_VALUE(pItem, _AL("Italic"), m_FontHint.bItalic)
			XMLGET_BOOL_VALUE(pItem, _AL("FreeType"), m_FontHint.bFreeType)
			XMLGET_INT_VALUE(pItem, _AL("Outline"), m_FontHint.nOutline)
			XMLGET_INT_VALUE(pItem, _AL("OutlineColor"), m_FontHint.clOutline)
		}
		else if( a_stricmp(pItem->GetName(), _AL("ImagePictureFont")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pItem, _AL("FontName"), m_FontImagePicture.szFontName)
			XMLGET_INT_VALUE(pItem, _AL("FontSize"), m_FontImagePicture.nFontSize)
			XMLGET_INT_VALUE(pItem, _AL("Shadow"), m_FontImagePicture.nShadow)
			XMLGET_BOOL_VALUE(pItem, _AL("Bold"), m_FontImagePicture.bBold)
			XMLGET_BOOL_VALUE(pItem, _AL("Italic"), m_FontImagePicture.bItalic)
			XMLGET_BOOL_VALUE(pItem, _AL("FreeType"), m_FontImagePicture.bFreeType)
			XMLGET_INT_VALUE(pItem, _AL("Outline"), m_FontImagePicture.nOutline)
			XMLGET_INT_VALUE(pItem, _AL("OutlineColor"), m_FontImagePicture.clOutline)
		}
		else if( a_stricmp(pItem->GetName(), _AL("MessageBoxFont")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pItem, _AL("FontName"), m_FontMessageBox.szFontName)
			XMLGET_INT_VALUE(pItem, _AL("FontSize"), m_FontMessageBox.nFontSize)
			XMLGET_INT_VALUE(pItem, _AL("Shadow"), m_FontMessageBox.nShadow)
			XMLGET_BOOL_VALUE(pItem, _AL("Bold"), m_FontMessageBox.bBold)
			XMLGET_BOOL_VALUE(pItem, _AL("Italic"), m_FontMessageBox.bItalic)
			XMLGET_BOOL_VALUE(pItem, _AL("FreeType"), m_FontMessageBox.bFreeType)
			XMLGET_INT_VALUE(pItem, _AL("Outline"), m_FontMessageBox.nOutline)
			XMLGET_INT_VALUE(pItem, _AL("OutlineColor"), m_FontMessageBox.clOutline)
		}
		else if( a_stricmp(pItem->GetName(), _AL("TabWidth")) == 0 )
		{
			XMLGET_INT_VALUE(pItem, _AL("CharNumber"), _tab_char)
		}
		pItem = pItem->GetNextItem();
	}

	aXMLFile.Release();

	return true;
}

const ACHAR * AUIManager::GetStringFromTable(int idString)
{
	return m_StringTable[idString];
}

int AUIManager::CreateEmptyDlg(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName)
{
	PAUIDIALOG pAUIDialog = CreateDlgInstance(pszTempName);
	if( !pAUIDialog ) return AUIMANAGER_WRONGDIALOGID;

	m_DlgZOrder.AddTail(pAUIDialog);
	m_aDialog.push_back(pAUIDialog);

	pAUIDialog->SetA3DEngine(pA3DEngine);
	pAUIDialog->SetA3DDevice(pA3DDevice);
	pAUIDialog->SetAUIManager(this);

	return m_aDialog.size() - 1;
}

int AUIManager::CreateDlg(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName)
{
	int idDlg = CreateEmptyDlg(pA3DEngine, pA3DDevice, pszTempName);
	if( AUIMANAGER_WRONGDIALOGID == idDlg )
		return AUIMANAGER_WRONGDIALOGID;

	bool bval = m_aDialog[idDlg]->Init(pA3DEngine, pA3DDevice, pszTempName);
	if( !bval ) return AUIMANAGER_WRONGDIALOGID;

	return idDlg;
}

bool AUIManager::LoadDialogLayout(const char *pszFilename)
{
	ASSERT(m_pA3DEngine && m_pA3DDevice);

	bool bval = true;
	bval = DestroyAllDialog();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::LoadDialogLayout(), failed to call DestroyAllDialog()");

	if( pszFilename )
	{
		AScriptFile s;
		char szFilename[MAX_PATH];

		sprintf(szFilename, "Interfaces\\%s", pszFilename);
		bval = s.Open(szFilename);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::LoadDialogLayout(), failed to open script file %s", pszFilename);

		while( !s.IsEnd() )
		{
			bval = s.GetNextToken(true);
			if( !bval ) break;		// End of file.

			BEGIN_FAKE_WHILE2;

			if( 0 == strcmpi(s.m_szToken, "#") ||
				0 == strcmpi(s.m_szToken, "//") )
			{
				s.SkipLine();
			}
			else
			{
				int idDlg = CreateDlg(m_pA3DEngine, m_pA3DDevice, s.m_szToken);
				if( AUIMANAGER_WRONGDIALOGID == idDlg )
					return AUI_ReportError(__LINE__, 1, "AUIManager::LoadDialogLayout(), failed to create dialog from %s", s.m_szToken);

				CHECK_BREAK2(s.GetNextToken(true));
				int x = atoi(s.m_szToken);

				CHECK_BREAK2(s.GetNextToken(true));
				int y = atoi(s.m_szToken);

				// old : m_aDialog[idDlg]->SetPos(x, y);
				m_aDialog[idDlg]->TranslatePosForAlign(x, y);
				m_aDialog[idDlg]->SetPosEx(x, y);

				CHECK_BREAK2(s.GetNextToken(true));

				bool bShow = atoi(s.m_szToken) ? true : false;
				m_aDialog[idDlg]->SetIsDefaultShow(bShow);
				m_aDialog[idDlg]->Show(bShow);
				
				if( m_pLoadingCallBack )
					m_pLoadingCallBack();
			}
			END_FAKE_WHILE2;

			BEGIN_ON_FAIL_FAKE_WHILE2;

			AUI_ReportError(DEFAULT_2_PARAM, "AUIManager:;LoadDialogLayout(), failed to read from file");

			END_ON_FAIL_FAKE_WHILE2;
		}

		s.Close();
	}

	return true;
}

bool AUIManager::DestroyAllDialog(void)
{
	for( int i = 0; i < (int)m_aDialog.size(); i++ )
		A3DRELEASE(m_aDialog[i]);
	m_aDialog.clear();
	m_DlgName.clear();
	m_DlgZOrder.RemoveAll();
	m_DlgZOrderBack.RemoveAll();

	return true;
}

bool AUIManager::DestroyDialog(int idDialog)
{
	if( idDialog < 0 || idDialog >= (int)m_aDialog.size() )
		return false;

	m_aDialog[idDialog]->Show(false);

	ALISTPOSITION pos;
	if( m_aDialog[idDialog]->IsBackgroundDialog() )
	{
		pos = m_DlgZOrderBack.Find(m_aDialog[idDialog]);
		m_DlgZOrderBack.RemoveAt(pos);
	}
	else
	{
		pos = m_DlgZOrder.Find(m_aDialog[idDialog]);
		m_DlgZOrder.RemoveAt(pos);
	}

	if( m_pDlgActive == m_aDialog[idDialog] )
		m_pDlgActive = NULL;
	if( m_pDlgHint == m_aDialog[idDialog] )
		m_pDlgHint = NULL;
	if( m_pDlgMouseOn == m_aDialog[idDialog] )
	{
		m_pDlgMouseOn = NULL;
		m_pObjMouseOn = NULL;
	}

	m_DlgName.erase(m_aDialog[idDialog]->GetName());
	A3DRELEASE(m_aDialog[idDialog]);
	m_aDialog.erase(m_aDialog.begin() + idDialog);

	return true;
}

bool AUIManager::DestroyDialog(const char *pszName)
{
	ASSERT(pszName);
	for( int i = 0; i < (int)m_aDialog.size(); i++ )
	{
		if( 0 == strcmpi(pszName, m_aDialog[i]->GetName()) )
			return DestroyDialog(i);
	}
	return false;
}

bool AUIManager::ShowDialog(int idDialog, bool bShow, bool bModal)
{
	if( idDialog >= 0 && idDialog < (int)m_aDialog.size() )
	{
		m_aDialog[idDialog]->Show(bShow, bModal);
		return true;
	}
	return false;
}

void AUIManager::BringWindowToTop(PAUIDIALOG pDlg)
{
	if( pDlg && pDlg != m_pDlgActive )
	{
		if( pDlg->IsCanChangeZOrder() )
			InsertDialogBefore(pDlg);
		
		if ( m_pDlgActive )
		{
			m_pDlgActive->OnEventMap(WM_KILLFOCUS, NULL, NULL, NULL);
			m_pDlgActive->SetCaptureObject(NULL);
		}
		m_pDlgActive = pDlg;
		m_pDlgActive->OnEventMap(WM_SETFOCUS, NULL, (LPARAM)pDlg, NULL);
	}
}

void AUIManager::InsertDialogAfter(PAUIDIALOG pDlg, PAUIDIALOG pDlgInsertAfter)
{
	if( !pDlg )
		return;

	if( pDlg->IsBackgroundDialog() )
	{
		ALISTPOSITION pos1 = m_DlgZOrderBack.Find(pDlg);
		m_DlgZOrderBack.RemoveAt(pos1);

		if( !pDlgInsertAfter )
			m_DlgZOrderBack.AddTail(pDlg);
		else
		{
			ALISTPOSITION pos2 = m_DlgZOrderBack.Find(pDlgInsertAfter);
			m_DlgZOrderBack.InsertAfter(pos2, pDlg);
		}
	}
	else
	{
		ALISTPOSITION pos1 = m_DlgZOrder.Find(pDlg);
		m_DlgZOrder.RemoveAt(pos1);

		if( !pDlgInsertAfter )
			m_DlgZOrder.AddTail(pDlg);
		else
		{
			ALISTPOSITION pos2 = m_DlgZOrder.Find(pDlgInsertAfter);
			m_DlgZOrder.InsertAfter(pos2, pDlg);
		}
	}
}

void AUIManager::InsertDialogBefore(PAUIDIALOG pDlg, PAUIDIALOG pDlgInsertBefore, PAUIDIALOG pTopSubDialog)
{
	if( !pDlg )
		return;

	if( pDlg->GetParentDlgControl() )
	{
		PAUIDIALOG pDlgParent = pDlg->GetParentDlgControl()->GetParent();
		if( pDlgParent && pDlgParent != pDlgInsertBefore )
		{
			InsertDialogBefore(pDlgParent, pDlgInsertBefore, pTopSubDialog ? pTopSubDialog : pDlg);
			return;
		}
	}

	if( pDlg->IsBackgroundDialog() )
	{
		ALISTPOSITION pos1 = m_DlgZOrderBack.Find(pDlg);
		if( pos1 )
		{
			m_DlgZOrderBack.RemoveAt(pos1);
			if( !pDlgInsertBefore )
				m_DlgZOrderBack.AddHead(pDlg);
			else
			{
				ALISTPOSITION pos2 = m_DlgZOrderBack.Find(pDlgInsertBefore);
				m_DlgZOrderBack.InsertBefore(pos2, pDlg);
			}
		}
	}
	else
	{
		ALISTPOSITION pos1 = m_DlgZOrder.Find(pDlg);
		if( pos1 )
		{
			m_DlgZOrder.RemoveAt(pos1);
			if( !pDlgInsertBefore )
				m_DlgZOrder.AddHead(pDlg);
			else
			{
				ALISTPOSITION pos2 = m_DlgZOrder.Find(pDlgInsertBefore);
				m_DlgZOrder.InsertBefore(pos2, pDlg);
			}
		}
	}

	PAUIDIALOG pTempSubDlg = pTopSubDialog;
	while( pTempSubDlg )
	{
		PAUIDIALOG pDlgParent = NULL;
		if( pTempSubDlg->GetParentDlgControl() )
			pDlgParent = pTempSubDlg->GetParentDlgControl()->GetParent();

		if( pDlgParent == pDlg )
		{
			InsertDialogBefore(pTempSubDlg, pDlg, pTopSubDialog);
			break;
		}

		pTempSubDlg = pDlgParent;
	}

	PAUIOBJECTLISTELEMENT pElement = pDlg->GetFirstControl();
	while( pElement )
	{
		if( pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG )
		{
			PAUISUBDIALOG pSubDialog = (PAUISUBDIALOG)pElement->pThis;
			PAUIDIALOG pDlgSub = pSubDialog->GetSubDialog();
			if( pDlgSub && pDlgSub != pTempSubDlg )
				InsertDialogBefore(pDlgSub, pDlg);
		}
		pElement = pElement->pNext;
	}
}

void AUIManager::ShowAllDialogs(bool bShow)
{
	for( int i = 0; i < (int)m_aDialog.size(); i++ )
	{
		m_aDialog[i]->Show(bShow);
	}
}

bool AUIManager::Tick(DWORD dwDeltaTime)
{
	bool bval = true;
	POINT ptPos;
	PAUIDIALOG pDlg = NULL;
	PAUIOBJECT pObj = NULL;
	static int count = 0;
	m_dwDeltaTime = dwDeltaTime;

	GetCursorPos(&ptPos);
	ScreenToClient(m_hWnd ? m_hWnd : m_pA3DDevice->GetDeviceWnd(), &ptPos);

	bval = HitTest(ptPos.x - m_rcWindow.left, ptPos.y - m_rcWindow.top, &pDlg, &pObj);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Tick(), failed to call HitTest()");

	if( pDlg != m_pDlgMouseOn || pObj != m_pObjMouseOn )
	{
		m_pDlgMouseOn = pDlg;
		m_pObjMouseOn = pObj;
		if( m_pObjMouseOn )
		{
			AString strSound = m_pObjMouseOn->GetHoverSound();
			if( strSound.GetLength() > 0 )
				m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(strSound);

			if( m_pObjMouseOn->GetType() == AUIOBJECT_TYPE_STILLIMAGEBUTTON )
				((PAUISTILLIMAGEBUTTON)m_pObjMouseOn)->StartOnHoverGfx();
		}
		m_nHintDelay = GetTickCount();
	}
	count ++;
	if(count >1)
	{
		m_pWndThreadMan->ReFreshWindows();
		count = 0;
	}
	for( int i = 0; i < (int)m_aDialog.size(); i++ )
	{
		if( m_aDialog[i]->GetDestroyFlag() )
		{
			bval = DestroyDialog(i);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Tick(), failed to call DestoryDialog(%d)", i);

			i--;	// Because destroied one dialog.
		}
		else if( m_aDialog[i]->IsShow() && !m_aDialog[i]->IsBlocked())
		{
			bval = m_aDialog[i]->Tick();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Tick(), failed to call m_aDialog[%d]->Tick()", i);
		}
	}

	return true;
}

bool AUIManager::Render(void)
{
#ifdef _ANGELICA22
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif
	bool bAlphaTest = m_pA3DDevice->GetAlphaTestEnable();
	if( bAlphaTest )
		m_pA3DDevice->SetAlphaTestEnable(false);
	BOOL bZWrite = TRUE;
#ifdef _ANGELICA22
	bZWrite = m_pA3DDevice->GetZWriteEnable();
#else
	IDirect3DDevice8* pRawDevice = m_pA3DDevice->GetD3DDevice();
	if(pRawDevice != NULL)
	{
		pRawDevice->GetRenderState(D3DRS_ZWRITEENABLE, (DWORD*)(&bZWrite));
	}
#endif
	if(bZWrite)
		m_pA3DDevice->SetZWriteEnable(false);
	
	bool bval = true;
	PAUIDIALOG pDlg;
	ALISTPOSITION pos;

	pos = m_DlgZOrderBack.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrderBack.GetPrev(pos);
		if( pDlg && pDlg->IsShow() && !pDlg->IsBlocked())
		{
			bval = pDlg->Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Render(), failed to call pDlg->Render()");
		}
	}

	pos = m_DlgZOrder.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrder.GetPrev(pos);
		if( pDlg && pDlg->IsShow() && !pDlg->IsBlocked() && pDlg != m_pDlgHint && !pDlg->IsDescendant(m_pDlgHint))
		{
			if( m_bUseCover && m_pA2DSpriteCover && m_pDlgActive && m_pDlgActive->IsModal() && m_pDlgActive == pDlg )
			{
				m_pA2DSpriteCover->SetScaleX(float(m_rcWindow.Width()) / m_pA2DSpriteCover->GetWidth());
				m_pA2DSpriteCover->SetScaleY(float(m_rcWindow.Height()) / m_pA2DSpriteCover->GetHeight());
				bval = m_pA2DSpriteCover->DrawToInternalBuffer(0, 0);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Render(), failed to call pDlg->Render()");
				m_pA3DEngine->FlushInternalSpriteBuffer();
			}
			bval = pDlg->Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Render(), failed to call pDlg->Render()");
		}
	}

	bval = RenderHint();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Render(), failed to call RenderHint()");

	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

	m_pFontMan->Flush();
	m_pDotlineMan->Flush();

	if( bAlphaTest )
		m_pA3DDevice->SetAlphaTestEnable(true);
	if(bZWrite)
		m_pA3DDevice->SetZWriteEnable(true);

	return true;
}

int AUIManager::GetDialogCount()
{
	return m_aDialog.size();
}

PAUIDIALOG AUIManager::GetDialog(int idDialog)
{
	ASSERT(idDialog >= 0 && idDialog < (int)m_aDialog.size());
	return m_aDialog[idDialog];
}

PAUIDIALOG AUIManager::GetDialog(const char *pszName)
{
	abase::hash_map<AString, PAUIDIALOG>::iterator it = m_DlgName.find(pszName);
	return it == m_DlgName.end() ? NULL : it->second;
}

bool AUIManager::OnCommand(const char *pszCommand, PAUIDIALOG pDlg)
{
	return pDlg->OnCommandMap(pszCommand);
}

// nRetVal may be: IDOK, IDCANCEL, IDYES, IDNO.
bool AUIManager::OnMessageBox(int nRetVal, PAUIDIALOG pDlg)
{
	// Virtual function, should be rewrited.
	pDlg->SetDestroyFlag(true);
	return true;
}

bool AUIManager::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	// Entry point of EVENT MAP mechanism
	return pDlg->OnEventMap(uMsg, wParam, lParam, pObj);
}

bool AUIManager::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_bRespawnMessage = false;
	if((g_bHideIme))
	{
		if(ImeUi_IsEnabled())
		{
			bool NoFurtherProcessing = IMEWrapper::StaticMsgProc(m_pA3DDevice->GetDeviceWnd(),uMsg,wParam,lParam);
			if(NoFurtherProcessing)
				return true;
		}
	}

	// Keyboard messages.
	if( GetWindowMessageType(uMsg) == WINDOWMESSAGE_TYPE_KEYBOARD )
	{
		if( m_pDlgActive && m_pDlgActive->IsShow() && m_pDlgActive->IsEnabled() && !m_pDlgActive->IsBlocked())
		{
			bool bRet = m_pDlgActive->DealWindowsMessage(uMsg, wParam, lParam, true);
			if (!m_bRespawnMessage && bRet)
				return true;
			else
				return false;
		}
		
	}
	// Mouse message.
	else if( GetWindowMessageType(uMsg) == WINDOWMESSAGE_TYPE_MOUSE )
	{
		int x = GET_X_LPARAM(lParam) - m_rcWindow.left;
		int y = GET_Y_LPARAM(lParam) - m_rcWindow.top;

		// Mouse position of WM_MOUSEWHEEL is screen coordinate, not client.
		if( WM_MOUSEWHEEL == uMsg )
		{
			POINT ptClient = { 0, 0 };
			ClientToScreen(m_hWnd ? m_hWnd : m_pA3DDevice->GetDeviceWnd(), &ptClient);
			x -= ptClient.x;
			y -= ptClient.y;
		}

		bool bFocusComboBox = false;
		if( m_pDlgActive && m_pDlgActive->GetFocus() && 
			((m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_COMBOBOX &&
			((PAUICOMBOBOX)m_pDlgActive->GetFocus())->GetState() == AUICOMBOBOX_STATE_POPUP) ||
			(m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_LISTBOX &&
			((PAUILISTBOX)m_pDlgActive->GetFocus())->GetComboBox() != NULL)) )
			bFocusComboBox = true;
		PAUIDIALOG pDlgOnMsg = NULL;
		if( !m_pDlgActive || (!m_pDlgActive->GetCaptureObject() &&
			!m_pDlgActive->IsModal() && !m_pDlgActive->IsCapture() &&
			!bFocusComboBox) )
		{
			A3DRECT rc;
			PAUIDIALOG pDlg;
			ALISTPOSITION pos;
			APtrList<PAUIDIALOG> DlgZOrder;
			PAUIDIALOG pDlgOld = m_pDlgActive;

			pos = m_DlgZOrder.GetHeadPosition();
			while( pos ) DlgZOrder.AddTail(m_DlgZOrder.GetNext(pos));
			pos = m_DlgZOrderBack.GetHeadPosition();
			while( pos ) DlgZOrder.AddTail(m_DlgZOrderBack.GetNext(pos));

			pos = DlgZOrder.GetHeadPosition();
			while( pos )
			{
				pDlg = DlgZOrder.GetNext(pos);
				if( !pDlg ) continue;
				
				if( !pDlg->IsShow() || !pDlg->IsEnabled() || pDlg->IsBlocked()) continue;

				if( !pDlg->GetFrame() || !pDlg->IsInHitArea(x, y) )
				{
					bool bOnControl = false;
					PAUIOBJECTLISTELEMENT pElement = pDlg->GetLastControl();
					while( pElement )
					{
						PAUIOBJECT pObj = pElement->pThis;
						if( pObj->IsShow() && !pObj->IsTransparent() )
						{
							bool bClip = false;
							A3DRECT rcClip;
							rc = pObj->GetRect();
							if( pObj->GetClipViewport() )
							{
								A3DVIEWPORTPARAM* param = pObj->GetClipViewport()->GetParam();
								rcClip.left = param->X - m_rcWindow.left;
								rcClip.top = param->Y - m_rcWindow.top;
								rcClip.right = rcClip.left + param->Width;
								rcClip.bottom = rcClip.top + param->Height;
								bClip = !rcClip.PtInRect(x, y);
							}
							if( pObj->IsInHitArea(x, y, uMsg) && !bClip )
							{
								bOnControl = true;
								break;
							}
						}

						pElement = pElement->pLast;
					}
					if( !bOnControl )
						continue;
				}

				if( uMsg != WM_MOUSEMOVE && uMsg != WM_MOUSEWHEEL && uMsg != WM_LBUTTONUP &&
					uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONUP)
					BringWindowToTop(pDlg);
				pDlgOnMsg = pDlg;
				break;
			}
			if( pDlgOld && pDlgOld != m_pDlgActive )
			{
				if( pDlgOld->IsPopup() || pDlgOld->GetType() == AUIDIALOG_TYPE_POPUPMENU )
				{
					pDlgOld->Show(false);
					pDlgOld->OnCommand("IDCANCEL");
				}
				else
					pDlgOld->ChangeFocus(NULL);
			}
		}
		else
			pDlgOnMsg = m_pDlgActive;

		if( m_pDlgActive && m_pDlgActive != pDlgOnMsg && !m_pDlgActive->IsCapture()
			&& m_pDlgActive->GetFocus()
			&& m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_STILLIMAGEBUTTON )
			m_pDlgActive->ChangeFocus(NULL);

		if( pDlgOnMsg && pDlgOnMsg->IsEnabled() && !pDlgOnMsg->IsBlocked() &&
			(pDlgOnMsg->IsCapture()	|| pDlgOnMsg->GetRect().PtInRect(x, y)
			|| pDlgOnMsg->GetCaptureObject() || bFocusComboBox ) )
		{
			bool bRet = pDlgOnMsg->DealWindowsMessage(uMsg, wParam, lParam, false);
			pDlgOnMsg->SetCommand("");
			if (!m_bRespawnMessage && bRet)
				return true;
			else
				return false;
		}
		else if( pDlgOnMsg && pDlgOnMsg->IsModal() )
			return true;
	}

	return false;
}

// Available dwType value: MB_NULL, MB_OK, MB_OKCANCEL, MB_YESNO.
bool AUIManager::MessageBox(const char *pszName, const ACHAR *pszText,
							DWORD dwType, A3DCOLOR color, PAUIDIALOG *ppDlg, abase::vector<A2DSprite*> *pvecImageList)
{
	if( ppDlg )
		*ppDlg = NULL;
	if( !pszName || !pszText )
		return true;

	if( GetDialog(pszName) )
		DestroyDialog(pszName);
	int idDlg;
	bool bval = true;
	int nNumButtons = 0;
	SIZE sButton = { 0, 0 };
	AUIOBJECT_SETPROPERTY p;
	PAUILABEL pLabel = NULL;
	PAUISTILLIMAGEBUTTON pButton[2] = { NULL, NULL };

	// Dialog.
	idDlg = CreateEmptyDlg(m_pA3DEngine, m_pA3DDevice, "");
	if( AUIMANAGER_WRONGDIALOGID == idDlg )
		return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), id returned by CreateEmptyDlg() is invalid");
	PAUIDIALOG pDlg = m_aDialog[idDlg];

	bval = pDlg->ImportStringTable("Interfaces\\MsgBox.stf");
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to load string table [Interfaces\\MsgBox.stf]");

	strcpy(p.fn, m_szMsgBoxFrame);
	bval = pDlg->SetProperty("Frame Image", &p);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to set dialog property [Frame Image] to file [%s]", m_szMsgBoxFrame);

	// Text label.
	pLabel = (PAUILABEL)pDlg->CreateControl(m_pA3DEngine, m_pA3DDevice, AUIOBJECT_TYPE_LABEL);
	if( !pLabel ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to create text label");
	pLabel->SetText(pszText);
	pLabel->SetFontAndColor(m_FontMessageBox.szFontName, m_FontMessageBox.nFontSize, color | 0xFF000000, m_FontMessageBox.nShadow, m_FontMessageBox.nOutline, 
		m_FontMessageBox.bBold, m_FontMessageBox.bItalic, m_FontMessageBox.bFreeType, m_FontMessageBox.clOutline);
	pLabel->SetImageList(pvecImageList);

	A3DPOINT2 ptSize;
	int nTextW = 0, nTextH = 0;
	A3DFTFont *pFont = pLabel->GetFont();
	ptSize = pFont->GetTextExtent(pszText);
	nTextW = ptSize.x;
	nTextH = ptSize.y;

	int nWidth, nHeight, nLines;
	int nLineSpace = 3 * m_fWindowScale;
	bval = AUI_GetTextRect(pFont, pLabel->GetText(), nWidth, nHeight, nLines, nLineSpace, pLabel->GetItemSet(), 0, 0, pvecImageList);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to call AUI_GetTextRect()");

	pLabel->SetSize(nWidth + 2, nHeight + 2);

	// First button.
	if( MB_NULL != dwType )
	{
		pButton[0] = (PAUISTILLIMAGEBUTTON)pDlg->CreateControl(
			m_pA3DEngine, m_pA3DDevice, AUIOBJECT_TYPE_STILLIMAGEBUTTON);
		if( !pButton[0] ) AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to create first button");

		strcpy(p.fn, m_szMsgBoxButton);
		bval = pButton[0]->SetProperty("Up Frame File", &p);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to set first button property [Up Frame File] to file [%s]", m_szMsgBoxButton);

		strcpy(p.fn, m_szMsgBoxButtonDown);
		bval = pButton[0]->SetProperty("Down Frame File", &p);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to set first button property [Down Frame File] to file [%s]", m_szMsgBoxButtonDown);

		// ignore error
		strcpy(p.fn, m_szMsgBoxButtonHover);
		bool b = AUI_IsLogEnabled();
		AUI_EnableLog(false);
		pButton[0]->SetProperty("OnHover Frame File", &p); 
		AUI_EnableLog(b);

		pButton[0]->SetFontAndColor(m_FontMessageBox.szFontName, m_FontMessageBox.nFontSize, color | 0xFF000000, m_FontMessageBox.nShadow, m_FontMessageBox.nOutline, 
			m_FontMessageBox.bBold, m_FontMessageBox.bItalic, m_FontMessageBox.bFreeType, m_FontMessageBox.clOutline);
		pButton[0]->Resize();

		nNumButtons++;
	}

	// Second button.
	if( MB_OK != dwType && MB_NULL != dwType )
	{
		pButton[1] = (PAUISTILLIMAGEBUTTON)pDlg->CreateControl(
			m_pA3DEngine, m_pA3DDevice, AUIOBJECT_TYPE_STILLIMAGEBUTTON);
		if( !pButton[1] ) AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to create second button");

		strcpy(p.fn, m_szMsgBoxButton);
		bval = pButton[1]->SetProperty("Up Frame File", &p);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to set first button property [Up Frame File] to file [%s]", m_szMsgBoxButton);

		strcpy(p.fn, m_szMsgBoxButtonDown);
		bval = pButton[1]->SetProperty("Down Frame File", &p);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::MessageBox(), failed to set first button property [Down Frame File] to file [%s]", m_szMsgBoxButtonDown);

		// ignore error
		strcpy(p.fn, m_szMsgBoxButtonHover);
		bool b = AUI_IsLogEnabled();
		AUI_EnableLog(false);
		pButton[1]->SetProperty("OnHover Frame File", &p); 
		AUI_EnableLog(b);

		pButton[1]->SetFontAndColor(m_FontMessageBox.szFontName, m_FontMessageBox.nFontSize, color | 0xFF000000, m_FontMessageBox.nShadow, m_FontMessageBox.nOutline, 
			m_FontMessageBox.bBold, m_FontMessageBox.bItalic, m_FontMessageBox.bFreeType, m_FontMessageBox.clOutline);
		pButton[1]->Resize();

		nNumButtons++;
	}

	// Button command and text.
	if( MB_OK == dwType || MB_OKCANCEL == dwType )
	{
		// IDOK.
		pButton[0]->SetCommand("IDOK");
		pButton[0]->SetText(pDlg->GetStringFromTable(IDOK));

		if( MB_OKCANCEL == dwType )
		{
			// IDCANCEL.
			pButton[1]->SetCommand("IDCANCEL");
			pButton[1]->SetText(pDlg->GetStringFromTable(IDCANCEL));
		}
	}
	else if( MB_YESNO == dwType )
	{
		// IDYES.
		pButton[0]->SetCommand("IDYES");
		pButton[0]->SetText(pDlg->GetStringFromTable(IDYES));

		// IDNO.
		pButton[1]->SetCommand("IDNO");
		pButton[1]->SetText(pDlg->GetStringFromTable(IDNO));
	}

	// Button size.
	if( nNumButtons > 0 )
	{
		sButton = pButton[0]->GetFrame()->GetSizeLimit();
		sButton.cx += nTextH * 4;
		sButton.cy += nTextH;
		if( pButton[0] ) pButton[0]->SetSize(sButton.cx, sButton.cy);
		if( pButton[1] ) pButton[1]->SetSize(sButton.cx, sButton.cy);
	}

	// Dialog size.
	SIZE sDialog = pDlg->m_pAUIFrameDlg->GetSizeLimit();
	int nDlgH = sDialog.cy + nTextH * (nLines + 2);
	if( sButton.cy > 0 ) nDlgH += sButton.cy + nTextH * 2;
	int W1 = nWidth + nTextH * 2;
	int W2 = (nNumButtons + 1) * sButton.cx;
	int nDlgW = sDialog.cx + max(W1, W2);
	pDlg->SetSize(nDlgW, nDlgH);

	// Position.
	int x, y;

//  old: 
//	x = -1;
//	y = -1;
//	pDlg->SetPos(x, y);
	pDlg->SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);
	
	x = (nDlgW - nWidth) / 2;
	y = sDialog.cy / 2 + nTextH;
	pLabel->SetPos(x, y);

	if( nNumButtons > 0 )
	{
		y = nDlgH - sDialog.cy / 2 - nTextH - sButton.cy;
		if( 1 == nNumButtons )
		{
			x = (nDlgW - sButton.cx) / 2;
			pButton[0]->SetPos(x, y);
		}					   
		else
		{
			x = nDlgW / 2 - sButton.cx - nTextH / 2;
			pButton[0]->SetPos(x, y);

			x = nDlgW / 2 + nTextH / 2;
			pButton[1]->SetPos(x, y);
		}
	}

	pDlg->SetAlpha(A3DCOLOR_GETALPHA(color));
	pDlg->SetType(AUIDIALOG_TYPE_MESSAGEBOX);
	pDlg->Show(true, MB_NULL == dwType ? false : true);

	char szName[256];
	if( strlen(pszName) == 0 || 0 == strcmpi(pszName, "Static") )
		sprintf(szName, "MsgBox_%d", rand() * rand());
	else
		strncpy(szName, pszName, 255);
	ASSERT(!GetDialog(szName));
	pDlg->SetName(szName);

	if( ppDlg ) *ppDlg = pDlg;

	return true;
}

bool AUIManager::RenderHint()
{
#ifdef _ANGELICA22
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif
	if( !m_pAUIFrameHint )
		return true;

	if( !m_pObjMouseOn )
		m_pObjHint = NULL;

	if( m_pDlgActive && m_pDlgMouseOn != m_pDlgActive &&
		(m_pDlgActive->IsModal() || m_pDlgActive->GetCaptureObject()) )
	{
		m_pObjHint = NULL;
		return true;
	}
		
	if( !m_pObjMouseOn && m_strHint == _AL("") || 
		m_pObjMouseOn && GetTickCount() - m_nHintDelay < m_pDlgMouseOn->GetHintDelay() ||
		m_strHint != _AL("") && GetTickCount() - m_nHintDelay < 500 )
	{
		m_pObjHint = NULL;
		return true;
	}

	POINT ptPos;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	GetCursorPos(&ptPos);
	ScreenToClient(m_hWnd ? m_hWnd : m_pA3DDevice->GetDeviceWnd(), &ptPos);
	ptPos.x -= m_rcWindow.left;
	ptPos.y -= m_rcWindow.top;

	int nLineSpace = 3 * m_fWindowScale;
	ACString strHint;
	int nMaxWidth = 0;
	PAUIDIALOG pDlg = m_pDlgMouseOn;
	PAUIOBJECT pObj = m_pObjMouseOn;
		
	if( m_pObjMouseOn )
	{
		if( m_pObjMouseOn->ExecuteRenderHintCallback(ptPos) )
			return true;

		nMaxWidth = m_pObjMouseOn->GetHintMaxWidth() * m_fWindowScale;

		PAUIOBJECT pObj = m_pObjMouseOn;
		strHint = pObj->GetHint();
		if( strHint.GetLength() <= 0 )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				strHint = pList->GetItemHint(pList->GetHOverItem());
				if( pList->GetNumColumns() > 0 && strHint.GetLength() <= 0 )
				{
					strHint = pList->GetItemColHint(pList->GetHOverItem(), pList->GetHOverItemCol());
					if( (pList->GetHOverItem() << 16) + pList->GetHOverItemCol() != (int)m_pObjHintSel )
						m_pObjHint = NULL;
					m_pObjHintSel = (void*)((pList->GetHOverItem() << 16) + pList->GetHOverItemCol());
				}
				else
				{
					if( pList->GetHOverItem() != (int)m_pObjHintSel )
						m_pObjHint = NULL;
					m_pObjHintSel = (void*)pList->GetHOverItem();
				}
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				if( pCombo->GetActiveItem() != (int)m_pObjHintSel )
					m_pObjHint = NULL;
				m_pObjHintSel = (void*)pCombo->GetActiveItem();
				strHint = pCombo->GetItemHint(pCombo->GetActiveItem());
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				bool bOnIcon;
				POINT ptObj = pObj->GetPos();
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				P_AUITREEVIEW_ITEM pItem = pTree->HitTest(
					ptPos.x - ptObj.x, ptPos.y - ptObj.y, &bOnIcon);
				if( !pItem || bOnIcon ) return true;
				if( pItem != m_pObjHintSel )
					m_pObjHint = NULL;
				m_pObjHintSel = (void*)pItem;
				strHint = pTree->GetItemHint(pItem);
			}
			else
				return true;
		}
	}
	else
		strHint = m_strHint;

	if( strHint.GetLength() > 8 && strHint.Left(8).CompareNoCase(_AL("^DIALOG:")) == 0 )
	{
		strHint.CutLeft(8);
		PAUIDIALOG pDlg = GetDialog(AC2AS(strHint));
		if( pDlg )
		{
			if( pDlg != m_pDlgHint )
			{
				if( m_pDlgHint )
				{
					m_pDlgHint->Show(false);
					m_pDlgHint->SetEnable(true);
				}

				pDlg->Show(true, false, false);
				pDlg->SetEnable(false);
				m_pDlgHint = pDlg;
			}

			int W = pDlg->GetSize().cx;
			int H = pDlg->GetSize().cy;
			int x = max(0, ptPos.x);
			int y = ptPos.y + 32;

			if( x + W > m_rcWindow.Width() )
				x = m_rcWindow.Width() - W;
			if( y + H > m_rcWindow.Height() )
			{
				if( ptPos.y - H >= 0 )
					y = ptPos.y - H;
				else
					y = m_rcWindow.Height() - H;
			}
			pDlg->SetPosEx(x, y);

			return pDlg->RenderRecurse();
		}
	}

	if( m_pDlgHint )
	{
		m_pDlgHint->Show(false);
		m_pDlgHint->SetEnable(true);
		m_pDlgHint = NULL;
	}

	if( strHint.GetLength() <= 0 )
		return true;

	EditBoxItemsSet ItemsSet;
	strHint = UnmarshalEditBoxText(strHint, ItemsSet);

	bool bval = true;
	int nWidth[10], nHeight[10], W, H, nLines, x, y, i;
	int nFontSize = m_FontHint.nFontSize;
	int nStyle = STYLE_NORMAL;
	if( m_FontHint.bBold )
		nStyle |= STYLE_BOLD;
	if( m_FontHint.bItalic )
		nStyle |= STYLE_ITALIC;
	A3DFTFont *pFont = GetDefaultFont();

	SIZE s = m_pAUIFrameHint->GetSizeLimit();
	CSplit sp(strHint);
	CSPLIT_STRING_VECTOR vec = sp.Split(_AL("\f"));
	bool bVertical = true;

	if(vec.size() == 1 )
	{
		CSplit sp1(strHint);
		vec = sp.Split(_AL("\t"));
		bVertical = false;
	}

	for(i = 0; i < vec.size(); i++)
	{
		while(vec[i].GetLength() >= 1 && (vec[i].Right(1) == _AL("\r") || vec[i].Right(1) == _AL("\n")) )
			vec[i].CutRight(1);
	}
		
	do
	{
		pFont = m_pFontMan->CreatePointFont((nFontSize--) * m_fWindowScale, m_FontHint.szFontName, nStyle, m_FontHint.bFreeType ? 0 : 1);
		W = H = 0;
		for(i = 0; i < vec.size(); i++)
		{
			bval = AUI_GetTextRect(pFont, vec[i], nWidth[i], nHeight[i], nLines, nLineSpace, &ItemsSet, 0, 0, 
				m_pvecImageList, m_fWindowScale, m_pvecHintOtherFonts, false, m_nWordWidth);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::RenderHint(), failed to call AUI_GetTextRect()");

			if( nMaxWidth > 0 && nWidth[i] > nMaxWidth )
			{
				nWidth[i] = nMaxWidth;
				A3DRECT rcText(0, 0, nMaxWidth, 2048);
				bval = AUI_TextOutFormat(pFont, 0, 0, vec[i], &rcText, 0, 0, nLineSpace, true, &nHeight[i], NULL, NULL, NULL, 
					false, m_FontHint.nShadow, 255, false, &ItemsSet, m_pvecImageList, m_fWindowScale, true, m_pvecHintOtherFonts, m_nWordWidth);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::RenderHint(), failed to call AUI_TextOutFormat()");
			}

			if( bVertical )
			{
				H += nHeight[i] + s.cy;
				if( nWidth[i] + s.cx > W )
					W = nWidth[i] + s.cx;
			}
			else
			{
				W += nWidth[i] + s.cx;
				if( nHeight[i] + s.cy > H )
					H = nHeight[i] + s.cy;
			}
		}
	}while(H > p->Height && nFontSize >= 8);

	if( !m_pObjMouseOn && m_strHint != _AL("") )
	{
		x = max(0, ptPos.x);
		y = ptPos.y + 32;

		if( x + W > m_rcWindow.Width() )
			x = m_rcWindow.Width() - W;
		if( y + H > m_rcWindow.Height() )
		{
			if( ptPos.y - s.cy - H >= 0 )
				y = ptPos.y - s.cy - H;
			else
				y = m_rcWindow.Height() - H;
		}
	}
	else if( m_pObjMouseOn && (m_pObjMouseOn != m_pObjHint || m_pObjMouseOn->IsHintFollow()) )
	{
		PAUIOBJECT pObj = m_pObjMouseOn;
		
		x = max(0, ptPos.x);
		if(pObj->GetType() == AUIOBJECT_TYPE_SLIDER)
		{
			y = ptPos.y + pObj->GetSize().cy;
		}
		else
		{
			y = ptPos.y + 32;
		}

		if( x + W > m_rcWindow.Width() )
			x = m_rcWindow.Width() - W;
		if( y + H > m_rcWindow.Height() )
		{
			if( ptPos.y - s.cy - H >= 0 )
				y= ptPos.y - s.cy - H;
			else
				y = m_rcWindow.Height() - H;
		}
		m_pObjHint = m_pObjMouseOn;
		m_ptObjHint.x = x;
		m_ptObjHint.y = y;
	}
	else
	{
		x = m_ptObjHint.x;
		y = m_ptObjHint.y;
	}

	for(i = 0; i < vec.size(); i++)
	{
		if( bVertical )
		{
			bval = m_pAUIFrameHint->Render(x + m_rcWindow.left, y + m_rcWindow.top, W + 2, nHeight[i] + s.cy,
				pFont, 0, NULL, AUIFRAME_ALIGN_LEFT, 0, 0, 255, false, m_FontHint.nShadow);
		}
		else
		{
			bval = m_pAUIFrameHint->Render(x + m_rcWindow.left, y + m_rcWindow.top, nWidth[i] + s.cx, nHeight[i] + s.cy,
				pFont, 0, NULL, AUIFRAME_ALIGN_LEFT, 0, 0, 255, false, m_FontHint.nShadow);
		}
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::RenderHint(), failed to call m_pAUIFrameHint->Render()");

		x += s.cx / 2;
		y += s.cy / 2;
		if( nMaxWidth > 0 )
		{
			A3DRECT rcText(0, 0, nWidth[i], nHeight[i]);
			rcText.Offset(x + m_rcWindow.left, y + m_rcWindow.top);
			bval = AUI_TextOutFormat(pFont, x + m_rcWindow.left, y + m_rcWindow.top, vec[i], &rcText, 0, 0, nLineSpace, false, NULL, NULL, NULL, NULL, 
				true, m_FontHint.nShadow, 255, false, &ItemsSet, m_pvecImageList, m_fWindowScale, true, m_pvecHintOtherFonts, m_nWordWidth);
		}
		else
		{
			bval = AUI_TextOutFormat(pFont, x + m_rcWindow.left, y + m_rcWindow.top, vec[i], NULL, 0, 0, nLineSpace, false, NULL, NULL, NULL, NULL, 
				true, m_FontHint.nShadow, 255, false, &ItemsSet, m_pvecImageList, m_fWindowScale, true, m_pvecHintOtherFonts, m_nWordWidth);
		}
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIManager::RenderHint(), failed to call AUI_TextOutFormat()");

		if( bVertical )
		{
			x -= s.cx / 2;
			y += s.cy / 2;
			y += nHeight[i];
		}
		else
		{
			x += s.cx / 2;
			y -= s.cy / 2;
			x += nWidth[i];
		}

	}
	
	return true;
}

bool AUIManager::HitTest(int x, int y, PAUIDIALOG *ppDlg,
	PAUIOBJECT *ppObj, PAUIDIALOG pDlgExclude)
{
	if( ppDlg ) *ppDlg = NULL;
	if( ppObj ) *ppObj = NULL;

	if( m_pDlgActive && m_pDlgActive->GetFocus() && 
		m_pDlgActive->GetFocus()->GetType() == AUIOBJECT_TYPE_COMBOBOX &&
		((PAUICOMBOBOX)m_pDlgActive->GetFocus())->GetState() == AUICOMBOBOX_STATE_POPUP )
	{
		if( m_pDlgActive->GetFocus()->IsInHitArea(x ,y, WM_LBUTTONDOWN) )
		{
			*ppDlg = m_pDlgActive;
			*ppObj = m_pDlgActive->GetFocus();
			return true;
		}
	}
	
	ALISTPOSITION pos;
	APtrList<PAUIDIALOG> DlgZOrder;
	pos = m_DlgZOrder.GetHeadPosition();
	while( pos ) DlgZOrder.AddTail(m_DlgZOrder.GetNext(pos));
	pos = m_DlgZOrderBack.GetHeadPosition();
	while( pos ) DlgZOrder.AddTail(m_DlgZOrderBack.GetNext(pos));

	pos = DlgZOrder.GetHeadPosition();
	while( pos )
	{
		PAUIDIALOG pDlg = DlgZOrder.GetNext(pos);
		if( !pDlg ) continue;

		if( !pDlg->GetRect().PtInRect(x, y) || !pDlg->IsShow() || !pDlg->IsEnabled() || pDlg == pDlgExclude ) continue;

		PAUIOBJECT pObjFocus = pDlg->GetFocus();
		if( pObjFocus )
		{ 
			A3DRECT rc, rcClip;
			bool bClip = false;
			rc = pObjFocus->GetRect();
			if( pObjFocus->GetClipViewport() )
			{
				A3DVIEWPORTPARAM *param = pObjFocus->GetClipViewport()->GetParam();
				rcClip.left = param->X - m_rcWindow.left;
				rcClip.top = param->Y - m_rcWindow.top;
				rcClip.right = rcClip.left + param->Width;
				rcClip.bottom = rcClip.top + param->Height;
				bClip = !rcClip.PtInRect(x, y);
			}

			if( pObjFocus->IsShow() && !pObjFocus->IsTransparent() && pObjFocus->IsInHitArea(x, y, WM_LBUTTONDOWN) && !bClip)
			{
				if( ppObj )	*ppObj = pObjFocus;
				if( ppDlg ) *ppDlg = pDlg;
				return true;
			}
		}

		PAUIOBJECTLISTELEMENT pElement = pDlg->GetLastControl();
		while( pElement )
		{
			PAUIOBJECT pObj = pElement->pThis;
			A3DRECT rc, rcClip;
			bool bClip = false;
			rc = pObj->GetRect();
			if( pObj->GetClipViewport() )
			{
				A3DVIEWPORTPARAM *param = pObj->GetClipViewport()->GetParam();
				rcClip.left = param->X - m_rcWindow.left;
				rcClip.top = param->Y - m_rcWindow.top;
				rcClip.right = rcClip.left + param->Width;
				rcClip.bottom = rcClip.top + param->Height;
				bClip = !rcClip.PtInRect(x, y);
			}
			if( pObj != pObjFocus && pObj->IsShow() && !pObj->IsTransparent() && pObj->IsInHitArea(x, y, WM_LBUTTONDOWN) && !bClip)
			{
				if( ppObj )	*ppObj = pObj;
				if( ppDlg )	*ppDlg = pDlg;
				return true;
			}
			pElement = pElement->pLast;
		}

		if( pDlg->GetFrame() && pDlg->IsInHitArea(x, y) )
		{
			if( ppDlg ) *ppDlg = pDlg;
			return true;
		}
	}

	return true;
}

void AUIManager::CalcWindowScale()
{
	float fScaleX, fScaleY;
	fScaleX = float(m_rcWindow.Width()) / m_nDefaultWidth;
	fScaleY = float(m_rcWindow.Height()) / m_nDefaultHeight;
	if( fScaleX < fScaleY )
		m_fWindowScale = fScaleX;
	else
		m_fWindowScale = fScaleY;
}

void AUIManager::ResizeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	A3DRECT rc;
	int i;
	int W = rcNew.Width(), H = rcNew.Height();

	if( W == 0 && H == 0 || rcOld == rcNew )
		return;

	m_rcWindow = rcNew;
	CalcWindowScale();

	for( i = 0; i < (int)m_aDialog.size(); i++ )
		m_aDialog[i]->Resize(rcOld, rcNew);

	if (m_pFont && m_pFontMan)
	{
		m_pFont = m_pFontMan->CreatePointFont(m_nDefaultFontSize * m_fWindowScale, m_pFont);
	}
	
	if (m_pPateFont && m_pFontMan)
	{
		m_pPateFont = m_pFontMan->CreatePointFont(GetWindowScale() * _pate_font_size, m_pPateFont);
	}
}

void AUIManager::SetWindowScale(float fWindowScale)
{
	int i;
	m_fWindowScale = fWindowScale;
	for( i = 0; i < (int)m_aDialog.size(); i++ )
		m_aDialog[i]->Resize(m_rcWindow, m_rcWindow);

	if (m_pFont && m_pFontMan)
	{
		m_pFont = m_pFontMan->CreatePointFont(m_nDefaultFontSize * m_fWindowScale, m_pFont);
	}
	
	if (m_pPateFont && m_pFontMan)
	{
		m_pPateFont = m_pFontMan->CreatePointFont(GetWindowScale() * _pate_font_size, m_pPateFont);
	}
}

void AUIManager::RearrangeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	ResizeWindows(rcOld, rcNew);
	return;


	A3DRECT rc;
	int i, x, y;
	PAUIDIALOG pDlg;
	int W = rcNew.Width(), H = rcNew.Height();

	if( W == rcOld.Width() && H == rcOld.Height() )
		return;

	for( i = 0; i < (int)m_aDialog.size(); i++ )
	{
		pDlg = m_aDialog[i];

		rc = pDlg->GetRect();

		if( rc.left <= 5 )
			x = rc.left;
		else if( rcOld.Width() - rc.right <= 5 )		// Right align.
			x = W - rc.Width();
		else
			x = W * rc.left / rcOld.Width();

		if( rc.top <= 5 )
			y = rc.top;
		else if( rcOld.Height() - rc.bottom <= 5 )		// Bottom align.
			y = H - rc.Height();
		else
			y = H * rc.top / rcOld.Height();

		// old : pDlg->SetPos(x, y, false);
		pDlg->SetPosEx(x, y);

		rc = m_aDialog[i]->GetRect();
		if( rc.left < 0 )
			x = 0;
		else if( rc.left > W )
			x = W - rc.Width();

		if( rc.top < 0 )
			y = 0;
		else if( rc.top > H )
			y = H - rc.Height();

		// old : pDlg->SetPos(x, y, false);
		pDlg->SetPosEx(x, y);

		pDlg->ChangeFocus(NULL);
	}
}


A3DFTFont* AUIManager::GetPateFont()
{
	if (m_pPateFont && m_pFontMan)
	{
		m_pPateFont = m_pFontMan->CreatePointFont(GetWindowScale() * _pate_font_size, m_pPateFont);
	}
	return m_pPateFont;
}

void AUIManager::SetHideIme(bool bHide)
{
	if (g_bHideIme == bHide)
		return;

	if (bHide)
	{
		ImeUi_SetupSink();
		StartImeHook();
		g_bHideIme = true;
	}
	else
	{
		ImeUi_ReleaseSink();
		EndImeHook();
		g_bHideIme = false;
	}
}


void AUIManager::GetMouseOn(PAUIDIALOG *ppDlg, PAUIOBJECT *ppObj)
{
	if( ppDlg ) *ppDlg = m_pDlgMouseOn;
	if( ppObj ) *ppObj = m_pObjMouseOn;
}

void AUIManager::SetHint(const ACHAR* szHint)
{
	if( m_strHint != szHint )
		m_nHintDelay = GetTickCount();
	m_strHint = szHint;
}

const ACHAR* AUIManager::GetHint()
{
	return m_strHint;
}

void AUIManager::SetObjHint(PAUIOBJECT pObj)
{
	m_pObjHint = pObj;
}

void AUIManager::SetUseCover(bool bUse)
{
	m_bUseCover = bUse;
}

int AUIManager::GetWindowMessageType(UINT uMsg)
{
	if( m_KeyboardMsg[uMsg] )
		return WINDOWMESSAGE_TYPE_KEYBOARD;
	else if( m_MouseMsg[uMsg] )
		return WINDOWMESSAGE_TYPE_MOUSE;
	else
		return WINDOWMESSAGE_TYPE_OTHER;
}

CWndThreadManager* AUIManager::GetWndThreadManager()
{
	return m_pWndThreadMan;
}

bool AUIManager::ScoutWindow(HWND hWnd)
{
	if(hWnd != NULL)
		return m_pWndThreadMan->InitFromWindow(hWnd) ? true : false;
	else
		return m_pWndThreadMan->Reset() == TRUE ? true : false;
}

void AUIManager::SaveAllRenderTarget()
{
	PAUIDIALOG pDlg;
	ALISTPOSITION pos;

	pos = m_DlgZOrderBack.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrderBack.GetPrev(pos);
		if( pDlg  )
			pDlg->SaveRenderTarget();
	}

	pos = m_DlgZOrder.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrder.GetPrev(pos);
		if( pDlg  )
			pDlg->SaveRenderTarget();
	}
}

PAUIDIALOG AUIManager::CreateDlgInstance(const AString strTemplName)
{
	return new AUIDialog;
}

void AUIManager::SetName(const char* pszName)
{
	m_szName = pszName;
}

void AUIManager::ClearActiveDialog()
{
	if (m_pDlgActive)
	{
		m_pDlgActive->OnEventMap(WM_KILLFOCUS, NULL, NULL, NULL);
		m_pDlgActive->SetCaptureObject(NULL);
		m_pDlgActive = NULL;
	}
}

bool AUIManager::ApplyTemplates()
{
	for (size_t i=0; i<m_aDialog.size(); i++)
	{
		if (!m_aDialog[i]->ApplyTemplates())
			return false;
	}

	return true;
}

A2DDotLineMan* AUIManager::GetDotLineMan()
{
	return m_pDotlineMan;
}

A2DDotLine* AUIManager::GetDefaultDotLine()
{
	return m_pDotline;
}

void AUIManager::SetAudioEvtSystem(AudioEngine::EventSystem* pEvtSystem)
{
#ifdef _ANGELICA_AUDIO
	m_pAudioEvtSystem = pEvtSystem;
#endif
}

bool AUIManager::LoadAudioEvtInstance(AudioEngine::Event*& pEvent, const char* szEventID)
{
#ifdef _ANGELICA_AUDIO
	if (!m_pAudioEvtSystem)
		return false;

	//if (pEvtInst)
	//	DestroyAudioEvtInstance(pEvtInst);

	pEvent = m_pAudioEvtSystem->GetEvent(szEventID);
	if (!pEvent)
		return false;

	//pEvtInst = pEvent->CreateInstance(false); //manually destroy
	return true;
#else
	return false;
#endif
}
//
//void AUIManager::DestroyAudioEvtInstance(AudioEngine::EventInstance*& pEvtInst)
//{
//#ifdef _ANGELICA_AUDIO
//	if (!pEvtInst || !pEvtInst->GetEvent())
//		return;
//
//	pEvtInst->GetEvent()->DestroyInstance(pEvtInst);
//	pEvtInst = NULL;
//
//	if (m_pCurAudioInst = pEvtInst)
//		m_pCurAudioInst = NULL;
//#endif
//}

void AUIManager::PlayAudioInstance(AudioEngine::Event *pEvent)
{
#ifdef _ANGELICA_AUDIO
	//if (m_pCurAudioInst)
	//	m_pCurAudioInst->Stop();

	//m_pCurAudioInst = pEvtInst;
	//if (m_pCurAudioInst)
	//	m_pCurAudioInst->Start();
	
	if(pEvent == NULL)
		return;
	AudioEngine::EventInstance* pEvtInst = pEvent->CreateInstance(true);
	if(pEvtInst != NULL)
		pEvtInst->Start();
#endif
}
//
//void AUIManager::StopAudioInstance(AudioEngine::EventInstance *pEvtInst)
//{
//#ifdef _ANGELICA_AUDIO
//	if(pEvtInst)
//		pEvtInst->Stop();
//	if(m_pCurAudioInst == pEvtInst)
//		m_pCurAudioInst = NULL;
//#endif
//}

//	class AUIManagerDialogIteratorByZOrder
AUIManagerDialogIteratorByZOrder::AUIManagerDialogIteratorByZOrder(AUIManager* pManager)
	: m_pAUIManager(pManager)
	, m_bLoopingBackList(false)
	, m_currentListPosition(NULL)
{
	m_currentListPosition = GetList().GetHeadPosition();
	CheckSwitchToBackList();
}

APtrList<PAUIDIALOG>& AUIManagerDialogIteratorByZOrder::GetList() {
	return m_pAUIManager->m_DlgZOrder;
}

APtrList<PAUIDIALOG>& AUIManagerDialogIteratorByZOrder::GetBackList() {
	return m_pAUIManager->m_DlgZOrderBack;
}

APtrList<PAUIDIALOG>& AUIManagerDialogIteratorByZOrder::GetCurrentList() {
	return m_bLoopingBackList ? GetBackList() : GetList();
}

void AUIManagerDialogIteratorByZOrder::CheckSwitchToBackList() {
	if (!m_bLoopingBackList && !m_currentListPosition) {
		m_currentListPosition = GetBackList().GetHeadPosition();
		m_bLoopingBackList = true;
	}
}

bool AUIManagerDialogIteratorByZOrder::GetCurrentAndLoopToNext(AUIDialog*& pDialog) {
	bool result(false);
	if (m_currentListPosition) {
		pDialog = GetCurrentList().GetNext(m_currentListPosition);
		CheckSwitchToBackList();
		result = true;
	}
	return result;
}

