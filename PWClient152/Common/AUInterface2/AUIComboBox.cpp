 // Filename	: AUIComboBox.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIComboBox is the class of combo box control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A3DViewport.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "CSplit.h"

#include "AUIComboBox.h"
#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUIListBox.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

#define AUICOMBOBOX_LINESPACE	0

AUIComboBox::AUIComboBox() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_COMBOBOX;
	m_nCurSel = -1;
	m_nActiveItem = -1;
	m_nState = AUICOMBOBOX_STATE_NORMAL;
	m_nAlign = AUIFRAME_ALIGN_LEFT;
	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	m_pAUIFrameExpand = NULL;
	m_pAUIFramePushed = NULL;
	m_pA2DSpriteHilight = NULL;
	m_pListBox = NULL;
	m_bJustPopup = false;
	m_nDefaultDir = AUICOMBOBOX_DIR_DOWN;
	m_nLineSpace = AUICOMBOBOX_LINESPACE;
	m_nItemMask = 0;
	m_pvecImageList = NULL;
}

AUIComboBox::~AUIComboBox()
{
}

bool AUIComboBox::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	ComboBox property line format:
	 *
	 *		COMBO					Keyword
	 *		GameType				Name
	 *		1						String ID of command.
	 *		10 10 200 25			x y Width Height
	 *		1						String ID of font name
	 *		20						Font height
	 *		0 0 0 255				Text color, R, G, B, A
	 *		"ComboFrame.bmp"		ImageFileName
	 *		"ComboHilight.bmp"		ImageFileName
	 */
	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1,  "AUIComboBox::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	// Font
	CHECK_BREAK2(pASF->GetNextToken(true));
	int idString = atoi(pASF->m_szToken);

	ACString strFontName = m_pParent->GetStringFromTable(idString);
	if( strFontName.GetLength() == 0 ) strFontName = _AL("方正细黑一简体");
	CSplit s(strFontName);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("###"));
	m_szFontName = vec[0];
	if( vec.size() > 1 ) m_nShadow = a_atoi(vec[1]);
	if( vec.size() > 2 ) m_nOutline = a_atoi(vec[2]);
	if( vec.size() > 3 ) m_bBold = a_atoi(vec[3]) ? true : false;
	if( vec.size() > 4 ) m_bItalic = a_atoi(vec[4]) ? true : false;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nFontHeight = atoi(pASF->m_szToken);

	if (!SetFontAndColor(m_szFontName, m_nFontHeight,
		m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic))
		return AUI_ReportError(__LINE__, 1, "AUIComboBox:Init(), failed to call SetFontAndColor()");

	// Text color
	CHECK_BREAK2(pASF->GetNextToken(true));
	int r = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int g = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int b = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int a = atoi(pASF->m_szToken);

	m_color = A3DCOLORRGBA(r, g, b, a);

	CHECK_BREAK2(pASF->GetNextToken(true));

	if (!InitIndirect(pASF->m_szToken, AUICOMBOBOX_FRAME_NORMAL))
		return AUI_ReportError(__LINE__, 1, "AUIComboBox::Init(), failed to call InitIndirect()");

	CHECK_BREAK2(pASF->GetNextToken(true));

	m_pListBox = (PAUILISTBOX)m_pParent->GetDlgItem(pASF->m_szToken);
	if( m_pListBox )	// Token is an existed listbox.
	{
		m_pListBox->Show(false);
		m_pListBox->SetHOver(true);
		m_pListBox->SetComboBox(this);
	}
	else
	{
		if (!InitIndirect(pASF->m_szToken, AUICOMBOBOX_FRAME_HILIGHT))
			return AUI_ReportError(__LINE__, 1, "AUIComboBox::Init(), failed to call InitIndirect()");
	}

	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIComboBox::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIComboBox::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_szFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
	m_nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_INT_VALUE(pItem, _AL("Align"), m_nAlign)
	XMLGET_STRING_VALUE(pItem, _AL("SoundEffect"), m_strSound)
#ifdef _ANGELICA_AUDIO
	LoadAudioInstance(m_strSound, m_pAudio);
#endif
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace)

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AString strListBox = "";
	XMLGET_STRING_VALUE(pItem, _AL("ListBoxName"), strListBox)
	if( strListBox != "" && m_pParent->GetDlgItem(strListBox) )
	{
		m_pListBox = (PAUILISTBOX)m_pParent->GetDlgItem(strListBox);
		m_pListBox->Show(false);
		m_pListBox->SetHOver(true);
		m_pListBox->SetComboBox(this);
	}

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0 )
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUICOMBOBOX_FRAME_NORMAL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameExpandImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUICOMBOBOX_FRAME_EXPAND);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FramePushedImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUICOMBOBOX_FRAME_PUSHED);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("HilightImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUICOMBOBOX_FRAME_HILIGHT);
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pChildItem, _AL("FontName"), m_szFontName)
			XMLGET_INT_VALUE(pChildItem, _AL("FontSize"), m_nFontHeight)
			XMLGET_INT_VALUE(pChildItem, _AL("Shadow"), m_nShadow)
			m_nShadow = A3DCOLORRGBA(0, 0, 0, m_nShadow);
			XMLGET_COLOR_VALUE(pChildItem, _AL("ShadowColor"), m_nShadow)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Bold"), m_bBold)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Italic"), m_bItalic)
			XMLGET_BOOL_VALUE(pChildItem, _AL("FreeType"), m_bFreeType)
			XMLGET_COLOR_VALUE(pChildItem, _AL("Color"), m_color)
			XMLGET_INT_VALUE(pChildItem, _AL("Outline"), m_nOutline)
			XMLGET_COLOR_VALUE(pChildItem, _AL("OutlineColor"), m_clOutline)
			XMLGET_COLOR_VALUE(pChildItem, _AL("InnerTextColor"), m_clInnerText)
			XMLGET_BOOL_VALUE(pChildItem, _AL("FadeColor"), m_bInterpColor)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextUpperColor"), m_clUpper)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextLowerColor"), m_clLower)
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Hint")) == 0)
		{
			ACString strHint;
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), strHint);
			AUICTranslate trans;
			m_szHint = trans.Translate(strHint);
		}
		pChildItem = pChildItem->GetNextItem();
	}
	
	if (!SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, 
			m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower))
		return AUI_ReportError(__LINE__, 1, "AUIComboBox::InitXML(), failed to call SetFontAndColor()");
	
	return true;
}

bool AUIComboBox::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	UpdateRenderTarget();
	if( idType == AUICOMBOBOX_FRAME_HILIGHT )
	{
		A3DRELEASE(m_pA2DSpriteHilight);
	
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pA2DSpriteHilight = new A2DSprite;
		if( !m_pA2DSpriteHilight ) return false;
		
#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSpriteHilight->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSpriteHilight->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSpriteHilight)
			return AUI_ReportError(__LINE__, 1, "AUIComboBox::InitIndirect(), failed to call m_pA2DSpriteHilight->Init()");
		}
	}
	else
	{
		if( idType == AUICOMBOBOX_FRAME_NORMAL )
			A3DRELEASE(m_pAUIFrame)
		else if( idType == AUICOMBOBOX_FRAME_EXPAND )
			A3DRELEASE(m_pAUIFrameExpand)
		else if( idType == AUICOMBOBOX_FRAME_PUSHED )
			A3DRELEASE(m_pAUIFramePushed)
		
		if( strlen(pszFileName) <= 0 ) return true;

		PAUIFRAME pAUIFrame = new AUIFrame;
		if( !pAUIFrame ) return false;

		bval = pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(pAUIFrame)
			return AUI_ReportError(__LINE__, 1, "AUIComboBox::InitIndirect(), failed to call m_pAUIFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());

		if( idType == AUICOMBOBOX_FRAME_NORMAL )
			m_pAUIFrame = pAUIFrame;
		else if( idType == AUICOMBOBOX_FRAME_EXPAND )
			m_pAUIFrameExpand = pAUIFrame;
		else if( idType == AUICOMBOBOX_FRAME_PUSHED )
			m_pAUIFramePushed = pAUIFrame;
	}

	return true;
}

bool AUIComboBox::Release(void)
{
	ResetContent();
	if( m_pListBox )
		m_pListBox->SetComboBox(NULL);

	A3DRELEASE(m_pAUIFrame);
	A3DRELEASE(m_pAUIFrameExpand);
	A3DRELEASE(m_pAUIFramePushed);
	A3DRELEASE(m_pA2DSpriteHilight);

	return AUIObject::Release();
}

bool AUIComboBox::Save(FILE *file)
{
	fprintf(file, "COMBO");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " %d %d %d %d %d %d", idString, m_nFontHeight,
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	fprintf(file, " \"%s\"", m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "");
	
	if( m_pListBox )
	{
		fprintf(file, " \"%s\" %d\n", m_pListBox->GetName(), m_nFrameMode);
	}
	else
	{
		fprintf(file, " \"%s\" %d\n", m_pA2DSpriteHilight
			? m_pA2DSpriteHilight->GetName() : "", m_nFrameMode);
	}
	
	return true;
}

bool AUIComboBox::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemComboBox = new AXMLItem;
	pXMLItemComboBox->InsertItem(pXMLItemDialog);
	pXMLItemComboBox->SetName(_AL("COMBO"));
	XMLSET_STRING_VALUE(pXMLItemComboBox, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("Align"), m_nAlign, 0)
	XMLSET_STRING_VALUE(pXMLItemComboBox, _AL("SoundEffect"), AS2AC(m_strSound), _AL(""))
	XMLSET_INT_VALUE(pXMLItemComboBox, _AL("LineSpace"), m_nLineSpace, AUICOMBOBOX_LINESPACE)

	XMLSET_STRING_VALUE(pXMLItemComboBox, _AL("Command"), AS2AC(m_strCommand), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemComboBox, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemComboBox);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemComboBox);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("FontName"), m_szFontName, _AL(""))
	XMLSET_INT_VALUE(pXMLItemText, _AL("FontSize"), m_nFontHeight, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("ShadowColor"), m_nShadow, _AL("0,0,0,0"));
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Bold"), m_bBold, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Italic"), m_bItalic, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FreeType"), m_bFreeType, true)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("Color"), m_color, _AL("255,255,255,255"));
	XMLSET_INT_VALUE(pXMLItemText, _AL("Outline"), m_nOutline, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("OutlineColor"), m_clOutline, _AL("0,0,0,255"))
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("InnerTextColor"), m_clInnerText, _AL("255,255,255,255"))
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FadeColor"), m_bInterpColor, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextUpperColor"), m_clUpper, _AL("255,255,255,255"))
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextLowerColor"), m_clLower, _AL("255,255,255,255"))
	
	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemComboBox);
	pXMLItemResource->SetName(_AL("Resource"));
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pAUIFramePushed )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FramePushedImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFramePushed->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pListBox )
	{
		XMLSET_STRING_VALUE(pXMLItemComboBox, _AL("ListBoxName"), AS2AC(m_pListBox->GetName()), _AL(""))
	}
	else
	{
		if( m_pA2DSpriteHilight )
		{
			AXMLItem *pXMLItemHilightImage = new AXMLItem;
			pXMLItemHilightImage->InsertItem(pXMLItemResource);
			pXMLItemHilightImage->SetName(_AL("HilightImage"));
			XMLSET_STRING_VALUE(pXMLItemHilightImage, _AL("FileName"), AS2AC(m_pA2DSpriteHilight->GetName()), _AL(""))
		}
		if( m_pAUIFrameExpand )
		{
			AXMLItem *pXMLItemFrameImage = new AXMLItem;
			pXMLItemFrameImage->InsertItem(pXMLItemResource);
			pXMLItemFrameImage->SetName(_AL("FrameExpandImage"));
			XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrameExpand->GetA2DSprite()->GetName()), _AL(""))
			XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
		}
	}

	return true;
}

bool AUIComboBox::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pAUIFrame ) return false;

	if( WM_LBUTTONDOWN == msg || WM_LBUTTONDBLCLK == msg)
	{
		if( AUICOMBOBOX_STATE_NORMAL == m_nState )
		{
			m_nActiveItem = m_nCurSel;
			if( m_pListBox )
			{
				m_pListBox->ResetContent();
				
				for( int i = 0; i < (int)m_Item.size(); i++ )
				{
					m_pListBox->AddString(m_Item[i].strTextSrc);
					m_pListBox->SetItemData(i, m_Item[i].dwData);
					m_pListBox->SetItemDataPtr(i, m_Item[i].pvData);
					m_pListBox->SetItemTextColor(i, m_Item[i].color);
					m_pListBox->SetItemHint(i, m_Item[i].strHint);
				}
				
				if( m_nCurSel >= 0 )
				{
					m_pListBox->SetCurSel(m_nCurSel);
					m_pListBox->EnsureVisible(m_nCurSel);
				}
				else
				{
					m_pListBox->SetCurSel(0);
					m_pListBox->EnsureVisible(0);
				}
				m_pParent->ChangeFocus(m_pListBox);
				m_pListBox->Show(true);
			}
			else
				m_bJustPopup = true;
			m_nState = AUICOMBOBOX_STATE_POPUP;
		}
		else
		{
			if( m_pListBox )
			{
				m_pListBox->Show(false);
				m_nState = AUICOMBOBOX_STATE_NORMAL;
			}
		}

		return true;
	}
	else if( WM_LBUTTONUP == msg && !m_pListBox )
	{
		if( !m_bJustPopup )
		{
			PAUIFRAME pFrame = m_pAUIFrame;
			if( m_pAUIFrameExpand )
				pFrame = m_pAUIFrameExpand;
			float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
			A3DPOINT2 ptSize;
			int nTextH;
			A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
			ptSize = pFont->GetTextExtent(_AL("Wjg"));
			nTextH = ptSize.y + m_nLineSpace * fWindowScale;
			int y = GET_Y_LPARAM(lParam) - GetPos().y - m_nOffY;

			if( m_nDir == AUICOMBOBOX_DIR_DOWN )
			{
				y -= m_nHeight;
				if( pFrame )
					y -= pFrame->GetSizeLimit().cy / 2;
			}
			else
			{
				y += m_Item.size() * nTextH;
				if( pFrame )
					y += pFrame->GetSizeLimit().cy / 2;
			}

			if( y >= 0 )
			{
				int nOldSel = m_nCurSel;
				SetCurSel(y / nTextH);
				if( m_nCurSel < 0 || m_nCurSel >= int(m_Item.size()) )
					SetCurSel(nOldSel);
				else if( nOldSel != m_nCurSel )
					Command();
			}
			m_nState = AUICOMBOBOX_STATE_NORMAL;
		}
		m_bJustPopup = false;

		return true;
	}
	else if( WM_MOUSEMOVE == msg && !m_pListBox )
	{
		PAUIFRAME pFrame = m_pAUIFrame;
		if( m_pAUIFrameExpand )
			pFrame = m_pAUIFrameExpand;
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		A3DPOINT2 ptSize;
		int nTextH;
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		ptSize = pFont->GetTextExtent(_AL("Wjg"));
		nTextH = ptSize.y + m_nLineSpace * fWindowScale;
		int y = GET_Y_LPARAM(lParam) - GetPos().y - m_nOffY;

		if( m_nDir == AUICOMBOBOX_DIR_DOWN )
		{
			y -= m_nHeight;
			if( pFrame )
				y -= pFrame->GetSizeLimit().cy / 2;
		}
		else
		{
			y += m_Item.size() * nTextH;
			if( pFrame )
				y += pFrame->GetSizeLimit().cy / 2;
		}

		int nItem = y / nTextH;
		if( nItem >= 0 && nItem < m_Item.size() )
		{
			m_bJustPopup = false;
			m_nActiveItem = nItem;
		}

		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIComboBox::Tick(void)
{
	return AUIObject::Tick();
}

bool AUIComboBox::Render(void)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	bool bval = true;
	POINT ptPos = GetPos();
	PAUIFRAME pAUIFrame;

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	if( !m_pAUIFrame ) return AUIObject::Render();
	FitFont();
	
	A3DPOINT2 ptSize;
	int nTextH;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("Wjg"));
	nTextH = ptSize.y + m_nLineSpace * fWindowScale;

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL || 
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		pAUIFrame = m_pAUIFrame;
		if( m_pAUIFramePushed && AUICOMBOBOX_STATE_POPUP == m_nState )
			pAUIFrame = m_pAUIFramePushed;
		if( pAUIFrame )
		{
			if( m_bEnabled )
				pAUIFrame->GetA2DSprite()->SetColor(A3DCOLORRGB(255, 255, 255));
			else
				pAUIFrame->GetA2DSprite()->SetColor(A3DCOLORRGB(160, 160, 160));

			// Render body.
			bval = pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth, m_nHeight, NULL, m_color,
				_AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIComboBox::Render(), failed to call m_pAUIFrame->Render()");
		}
		
		int nBorderW = 0;
		if( m_pAUIFrameExpand )
			nBorderW = m_pAUIFrameExpand->GetSizeLimit().cx / 2;
		else if( pAUIFrame )
			nBorderW = pAUIFrame->GetSizeLimit().cx / 2;
		if( m_Item.size() > 0 )
		{
			int x = ptPos.x + nBorderW;
			int y = ptPos.y + m_nHeight / 2 - nTextH / 2;
			A3DRECT rc(x, y, x + m_nWidth - 2 * nBorderW, y + nTextH);
			ACString strText;
			A3DCOLOR color;
			EditBoxItemsSet *pItemSet;
			if( m_nCurSel >= 0 )
			{
				strText = m_Item[m_nCurSel].strText;
				color = m_Item[m_nCurSel].color;
				pItemSet = &m_Item[m_nCurSel].itemsSet;
			}
			else
			{
				strText = m_szText;
				color = m_color;
				pItemSet = NULL;
			}
			
			if( m_nAlign == AUIFRAME_ALIGN_RIGHT ||
				m_nAlign == AUIFRAME_ALIGN_CENTER )
			{
				int nWidth, nHeight, nLines;

				AUI_GetTextRect(pFont, strText, nWidth, nHeight, nLines, 3, pItemSet, 0, 0, m_pvecImageList);

				if( m_nAlign == AUIFRAME_ALIGN_CENTER )
					x = x + rc.Width() / 2 - nWidth / 2;
				else
					x = x + rc.Width() - nWidth;
			}
			ACHAR szColor[10];
			if( !m_bEnabled )
				 a_strcpy(szColor, _AL("^7F7F7F"));
			else
			{
				A3DCOLOR r = A3DCOLOR_GETRED(color);
				A3DCOLOR g = A3DCOLOR_GETGREEN(color);
				A3DCOLOR b = A3DCOLOR_GETBLUE(color);
				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);
			}
			bval = AUI_TextOutFormat(pFont, x, y, szColor + strText,
				&rc, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false, pItemSet, m_pvecImageList);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call AUI_TextOutFormat()");
		}
	}

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL || 
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC )
	{
		if( AUICOMBOBOX_STATE_POPUP == m_nState && IsFocus() && m_Item.size() > 0 && !m_pListBox )
		{
			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIComboBox::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();

			if( m_pAUIFrameExpand )
				pAUIFrame = m_pAUIFrameExpand;
			else
				pAUIFrame = m_pAUIFrame;
			int cy1 = 0, cy2 = 0;

			if( pAUIFrame )
			{
				cy1 = pAUIFrame->GetSizeLimit().cy / 2;
				cy2 = cy1;
			}
			int nHeight = cy1 + cy2 + nTextH * m_Item.size();
			A3DRECT rcWindow = m_pParent->GetAUIManager()->GetRect();

			int yPopup;
			if( m_nDefaultDir == AUICOMBOBOX_DIR_DOWN )
			{
				if( ptPos.y + m_nHeight + nHeight > (m_nVPH > 0 ? m_nVPH : rcWindow.Height()) )
				{
					m_nDir = AUICOMBOBOX_DIR_UP;
					yPopup = ptPos.y - nHeight;
				}
				else
				{
					m_nDir = AUICOMBOBOX_DIR_DOWN;
					yPopup = ptPos.y + m_nHeight;
				}
			}
			else
			{
				if( ptPos.y < (m_nOffX > 0 ? m_nOffX : 0) + nHeight )
				{
					m_nDir = AUICOMBOBOX_DIR_DOWN;
					yPopup = ptPos.y + m_nHeight;
				}
				else
				{
					m_nDir = AUICOMBOBOX_DIR_UP;
					yPopup = ptPos.y - nHeight;
				}
			}

			if( pAUIFrame )
			{
				bval = pAUIFrame->Render(ptPos.x, yPopup, m_nWidth, nHeight, NULL, m_color,
					_AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIComboBox::Render(), failed to call m_pAUIFrame->Render()");
			}

			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIComboBox::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			// Draw hilight.
			int nBorderH = 0;
			int nBorderW = 0;
			if( pAUIFrame )
			{
				nBorderW = pAUIFrame->GetSizeLimit().cx / 2;
				nBorderH = pAUIFrame->GetSizeLimit().cy / 2;
			}
			int x = ptPos.x + nBorderW;
			int yTextBegin = yPopup + nBorderH;
			if( m_pA2DSpriteHilight && m_nActiveItem >= 0 )
			{
				float fScaleX = float(m_nWidth - 2 * nBorderW) / float(m_pA2DSpriteHilight->GetWidth());
				float fScaleY = float(nTextH) / float(m_pA2DSpriteHilight->GetHeight());

				m_pA2DSpriteHilight->SetScaleX(fScaleX);
				m_pA2DSpriteHilight->SetScaleY(fScaleY);
				m_pA2DSpriteHilight->SetAlpha(nAlpha);

				int y = yTextBegin + m_nActiveItem * nTextH;

				if( m_pParent->IsNoFlush() )
					bval = m_pA2DSpriteHilight->DrawToBack(x, y);
				else
					bval = m_pA2DSpriteHilight->DrawToInternalBuffer(x, y);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIComboBox::Render(), failed to call m_pA2DSpriteHilight->DrawToBack() or m_pA2DSpriteHilight->DrawToInternalBuffer()");
			}

			// Show combo box text items.
			int px = x;
			for( int i = 0; i < int(m_Item.size()); i++ )
			{
				x = px;
				int y = yTextBegin + i * nTextH;
				A3DRECT rc(x, y, x + m_nWidth - 2 * nBorderW, y + nTextH);

				if( m_nAlign == AUIFRAME_ALIGN_RIGHT ||
					m_nAlign == AUIFRAME_ALIGN_CENTER )
				{
					int nWidth, nHeight, nLines;

					AUI_GetTextRect(pFont, m_Item[i].strText, nWidth, nHeight, nLines, 3, &m_Item[i].itemsSet, 0, 0, m_pvecImageList);
					if( m_nAlign == AUIFRAME_ALIGN_CENTER )
						x = px + rc.Width() / 2 - nWidth / 2;
					else
						x = px + rc.Width() - nWidth;
				}
				A3DCOLOR r = A3DCOLOR_GETRED(m_Item[i].color);
				A3DCOLOR g = A3DCOLOR_GETGREEN(m_Item[i].color);
				A3DCOLOR b = A3DCOLOR_GETBLUE(m_Item[i].color);
				ACHAR szColor[10];
				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);
				bval = AUI_TextOutFormat(pFont, x, y, szColor + m_Item[i].strText,
					&rc, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false, &m_Item[i].itemsSet, m_pvecImageList);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call AUI_TextOutFormat()");
			}
		}
	}

	return true;
}

int AUIComboBox::AddString(const ACHAR *pszString)
{
	AUICOMBOBOX_ITEM Item;
	
	Item.strTextSrc = pszString;
	Item.strText = UnmarshalEditBoxText(pszString, Item.itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	Item.pvData = NULL;
	Item.dwData = 0;
	Item.uiDataPtr64 = 0;
	Item.uiData64 = 0;
	Item.color = m_color;
	m_Item.push_back(Item);
	return m_Item.size();
}

int AUIComboBox::InsertString(int nIndex, const ACHAR *pszString)
{
	if( nIndex < 0 || nIndex > int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;

	AUICOMBOBOX_ITEM Item;
	Item.strTextSrc = pszString;
	Item.strText = UnmarshalEditBoxText(pszString, Item.itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	Item.pvData = NULL;
	Item.dwData = 0;
	Item.uiDataPtr64 = 0;
	Item.uiData64 = 0;
	Item.color = m_color;
	m_Item.insert(m_Item.begin() + nIndex, Item);
	return m_Item.size();
}

int AUIComboBox::DeleteString(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;

	m_Item.erase(m_Item.begin() + nIndex);
	return m_Item.size();
}

int AUIComboBox::GetCount(void)
{
	return m_Item.size();
}

int AUIComboBox::GetCurSel(void)
{
	return m_nCurSel;
}

int AUIComboBox::SetCurSel(int nIndex)
{
	if( nIndex < -1 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;

	UpdateRenderTarget();
	m_nCurSel = nIndex;
	return nIndex;
}

const ACHAR * AUIComboBox::GetText(int nIndex)
{
	if( nIndex < -1 || nIndex >= int(m_Item.size()) )
		return _AL("");
	else if( nIndex == -1 )
		return m_szText;
	else
		return m_Item[nIndex].strText;
}

int AUIComboBox::SetText(int nIndex, const ACHAR *pszText)
{
	if( nIndex < -1 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;

	if( nIndex == m_nCurSel )
		UpdateRenderTarget();

	if( nIndex == -1 )
		m_szText = pszText;
	else
	{
		m_Item[nIndex].strTextSrc = pszText;
		m_Item[nIndex].itemsSet.Release();
		m_Item[nIndex].strText = UnmarshalEditBoxText(pszText, m_Item[nIndex].itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	}
	return nIndex;
}

A3DCOLOR AUIComboBox::GetItemTextColor(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return A3DCOLORRGB(0, 0, 0);
	else
		return m_Item[nIndex].color;
}

int AUIComboBox::SetItemTextColor(int nIndex, A3DCOLOR color)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;

	if( nIndex == m_nCurSel )
		UpdateRenderTarget();

	m_Item[nIndex].color = color;
	return nIndex;
}

void AUIComboBox::ResetContent(void)
{
	UpdateRenderTarget();

	m_nCurSel = -1;
	m_Item.clear();
}

bool AUIComboBox::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Expand Image") )
	{
		if( m_pAUIFrameExpand )
			strncpy(Property->fn, m_pAUIFrameExpand->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Pushed Image") )
	{
		if( m_pAUIFramePushed )
			strncpy(Property->fn, m_pAUIFramePushed->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
	{
		if( m_pA2DSpriteHilight )
			strncpy(Property->fn, m_pA2DSpriteHilight->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "List Box Name") )
	{
		if( m_pListBox )
			strncpy(Property->fn, m_pListBox->GetName(), AFILE_LINEMAXLEN - 1);
	}
	if( 0 == strcmpi(pszPropertyName, "Align") )
		Property->i = m_nAlign;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIComboBox::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUICOMBOBOX_FRAME_NORMAL);
	else if( 0 == strcmpi(pszPropertyName, "Frame Expand Image") )
		return InitIndirect(Property->fn, AUICOMBOBOX_FRAME_EXPAND);
	else if( 0 == strcmpi(pszPropertyName, "Frame Pushed Image") )
		return InitIndirect(Property->fn, AUICOMBOBOX_FRAME_PUSHED);
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
		return InitIndirect(Property->fn, AUICOMBOBOX_FRAME_HILIGHT);
	else if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nWidth = max(m_nWidth, s.cx);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nHeight = max(m_nHeight, s.cy);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "List Box Name") )
	{
		PAUILISTBOX pList = (PAUILISTBOX)m_pParent->GetDlgItem(Property->c);
		if( pList )
		{
			m_pListBox = pList;
			pList->SetComboBox(this);
		}
		else
		{
			if (m_pListBox)
				m_pListBox->SetComboBox(NULL);
			m_pListBox = NULL;
		}
	}
	if( 0 == strcmpi(pszPropertyName, "Align") )
		m_nAlign = Property->i;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIComboBox::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame Expand Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame Pushed Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Hilight Image", AUIOBJECT_PROPERTY_VALUE_CHAR);
}

A3DRECT AUIComboBox::GetRect(void)
{
	A3DRECT rc = AUIObject::GetRect();

	if( m_pAUIFrame && AUICOMBOBOX_STATE_POPUP == m_nState && !m_pListBox )
	{
		PAUIFRAME pFrame = m_pAUIFrame;
		if( m_pAUIFrameExpand )
			pFrame = m_pAUIFrameExpand;
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		A3DPOINT2 ptSize;
		int nTextH;
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		ptSize = pFont->GetTextExtent(_AL("Wjg"));
		nTextH = ptSize.y + m_nLineSpace * fWindowScale;

		if( m_nDir == AUICOMBOBOX_DIR_DOWN )
		{
			rc.bottom += m_Item.size() * nTextH;
			if( pFrame )
				rc.bottom += pFrame->GetSizeLimit().cy;
		}
		else
		{
			rc.top -= m_Item.size() * nTextH;
			if( pFrame )
				rc.top -= pFrame->GetSizeLimit().cy;
		}
	}

	return rc;
}

void AUIComboBox::SetFocus(bool bFocus)
{
	if( !bFocus )
	{
		if( m_pListBox )
		{
			if( !m_pListBox->IsFocus() )
			{
				m_pListBox->Show(false);
				m_nState = AUICOMBOBOX_STATE_NORMAL;
			}
		}
		else
			m_nState = AUICOMBOBOX_STATE_NORMAL;
	}
	AUIObject::SetFocus(bFocus);
}

void * AUIComboBox::GetItemDataPtr(int nIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
	{
		return NULL;
	}
	else
	{
		if (0 != m_Item[nIndex].pvData && strName != m_Item[nIndex].strDataPtrName)
			AUI_ReportError(__LINE__, 1, "AUIComboBox::GetItemDataPtr(), data name not match");
		return m_Item[nIndex].pvData;
	}
}

int AUIComboBox::SetItemDataPtr(int nIndex, void *pData, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;
	
	m_Item[nIndex].strDataPtrName = strName;
	m_Item[nIndex].pvData = pData;
	return nIndex;
}

DWORD AUIComboBox::GetItemData(int nIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
	{
		return AUICOMBOBOX_ERROR;
	}
	else
	{
		if (0 != m_Item[nIndex].dwData && strName != m_Item[nIndex].strDataName)
			AUI_ReportError(__LINE__, 1, "AUIComboBox::GetItemData(), data name not match");
		return m_Item[nIndex].dwData;
	}
}

int AUIComboBox::SetItemData(int nIndex, DWORD dwItemData, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;
	
	m_Item[nIndex].strDataName = strName;
	m_Item[nIndex].dwData = dwItemData;
	return nIndex;
}

UINT64 AUIComboBox::GetItemDataPtr64(int nIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
	{
		return NULL;
	}
	else
	{
		if (0 != m_Item[nIndex].uiDataPtr64 && strName != m_Item[nIndex].strDataPtr64Name)
			AUI_ReportError(__LINE__, 1, "AUIComboBox::GetItemDataPtr64(), data name not match");
		return m_Item[nIndex].uiDataPtr64;
	}
}

int AUIComboBox::SetItemDataPtr64(int nIndex, UINT64 uiDataPtr64, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;
	
	m_Item[nIndex].strDataPtr64Name = strName;
	m_Item[nIndex].uiDataPtr64 = uiDataPtr64;
	return nIndex;
}

UINT64 AUIComboBox::GetItemData64(int nIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
	{
		return AUICOMBOBOX_ERROR;
	}
	else
	{
		if (0 != m_Item[nIndex].uiData64 && strName != m_Item[nIndex].strData64Name)
			AUI_ReportError(__LINE__, 1, "AUIComboBox::GetItemData64(), data name not match");
		return m_Item[nIndex].uiData64;
	}
}

int AUIComboBox::SetItemData64(int nIndex, UINT64 uiData64, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;
	
	m_Item[nIndex].strData64Name = strName;
	m_Item[nIndex].uiData64 = uiData64;
	return nIndex;
}

const ACHAR * AUIComboBox::GetItemHint(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return _AL("");
	else
		return m_Item[nIndex].strHint;
}

int AUIComboBox::SetItemHint(int nIndex, const ACHAR *pszHint)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUICOMBOBOX_ERROR;
	
	m_Item[nIndex].strHint = pszHint;

	return nIndex;
}

int AUIComboBox::GetActiveItem()
{
	if( m_nState == AUICOMBOBOX_STATE_NORMAL )
		return m_nCurSel;
	else
		return m_nActiveItem;
}

bool AUIComboBox::SortItems(int nMode)
{
	UpdateRenderTarget();

	bool bSwapped;
	AUICOMBOBOX_ITEM Item;

	do
	{
		bSwapped = false;
		for( int i = 0; i < int(m_Item.size()) - 1; i++ )
		{
			if( (AUICOMBOBOX_SORT_ASCENT == nMode &&
				a_stricmp(m_Item[i].strText, m_Item[i + 1].strText) > 0) ||
				(AUICOMBOBOX_SORT_DESCENT == nMode &&
				a_stricmp(m_Item[i].strText, m_Item[i + 1].strText) < 0) )
			{
				Item = m_Item[i];
				m_Item[i] = m_Item[i + 1];
				m_Item[i + 1] = Item;
				bSwapped = true;
			}
		}
	} while( bSwapped );

	return true;
}

void AUIComboBox::Command()
{
	m_pParent->SetLastCommandGenerator(this);
	if( m_strCommand != "" )
		m_pParent->SetCommand(m_strCommand);
	else
		m_pParent->SetCommand(m_szName);

#ifdef _ANGELICA_AUDIO
	if( m_pAudio )
		m_pParent->GetAUIManager()->PlayAudioInstance(m_pAudio);
	else if( strlen(m_strSound) > 0 )
		m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#else
	if( strlen(m_strSound) > 0 )
		m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#endif
}

bool AUIComboBox::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(fWindowScale);
	if( m_pAUIFrameExpand )
		m_pAUIFrameExpand->SetScale(fWindowScale);
	if( m_pAUIFramePushed )
		m_pAUIFramePushed->SetScale(fWindowScale);

	return true;
}

void AUIComboBox::SetDefaultDir(int nDir)
{
	m_nDefaultDir = nDir;
}

void AUIComboBox::SetListBox(AUIListBox* pList)
{
	if(pList == m_pListBox) return;
	m_pListBox = pList;
	if (pList)
		pList->SetComboBox(this);
}

const AUIListBox* AUIComboBox::GetListBox() const
{
	return m_pListBox;
}

void AUIComboBox::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIComboBox::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSpriteHilight)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteHilight, bLoad);

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);

	if (m_pAUIFrameExpand)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameExpand->GetA2DSprite(), bLoad);

	if (m_pAUIFramePushed)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFramePushed->GetA2DSprite(), bLoad);
}