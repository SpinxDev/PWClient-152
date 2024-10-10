// Filename	: AUISubDialog.cpp
// Creator	: Xiao Zhou
// Date		: 2009/4/2
// Desc		: AUISubDialog is the class of sub dialog control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "CSplit.h"

#include "AUIDef.h"
#include "AUIDialog.h"
#include "AUISubDialog.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

#define AUISUBDIALOG_SCROLL_DELAY		100
#define AUISUBDIALOG_SCROLL_STEP		20
#define AUISUBDIALOG_WHEEL_STEP			60

AUISubDialog::AUISubDialog() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_SUBDIALOG;
	m_nState = AUISUBDIALOG_STATE_NORMAL;
	m_bRenderHScroll = true;
	m_bRenderVScroll = true;
	m_nVBarPos = 0;
	m_nVBarMax = 0;
	m_nHBarPos = 0;
	m_nHBarMax = 0;
	m_bDraggingVScroll = false;
	m_bDraggingHScroll = false;
	m_nScrollDelay = GetTickCount();
	m_ptVBar.x = m_ptVBar.y = m_nDragDist = 0;
	m_ptHBar.x = m_ptHBar.y = 0;
	m_nSubDialogOrgWidth = m_nSubDialogOrgHeight = 0;
	m_nSubDialogOrgDefaultWidth = m_nSubDialogOrgDefaultHeight = 0;
	m_pViewport = NULL;

	m_bIsLoad = false;
	m_szDialogFileName = "";
	m_pSubDialog = NULL;
	m_pVBarFrame = NULL;
	m_pHBarFrame = NULL;
	m_bForceRenderScroll = true;
	for( int i = 0; i < AUISUBDIALOG_RECT_NUM; i++ )
	{
		m_pA2DSprite[i] = NULL;
		m_rcImage[i].SetRect(0, 0, 0, 0);
	}

	m_pFrame = NULL;
	m_nFrameMode = 0;
	m_bRenderDlgFrame = false;
}

AUISubDialog::~AUISubDialog()
{
	if (m_pViewport)
		delete m_pViewport;
}

bool AUISubDialog::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	AString strDlgName;

	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_STRING_VALUE(pItem, _AL("DialogFile"), strDlgName)
	XMLGET_BOOL_VALUE(pItem, _AL("RenderDlgFrame"), m_bRenderDlgFrame)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0 )
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("VScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_VSCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("VBarFrameImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_VBAR);
				}
				if( a_stricmp(pResouceChildItem->GetName(), _AL("LeftImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_LEFT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("RightImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_RIGHT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("HScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_HSCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("HBarFrameImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_HBAR);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISUBDIALOG_RECT_FRAME);					
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		pChildItem = pChildItem->GetNextItem();
	}

	if( strDlgName.GetLength() > 0 && !LoadDialog(strDlgName) )
		return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitXML(), failed to call LoadDialog()");

	return true;
}

bool AUISubDialog::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUISUBDIALOG_RECT_MIN && idType < AUISUBDIALOG_RECT_MAX);

	if( AUISUBDIALOG_RECT_VBAR == idType )
	{
		A3DRELEASE(m_pVBarFrame);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pVBarFrame = new AUIFrame;
		if( !m_pVBarFrame ) return false;

		bval = m_pVBarFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName);
		if( !bval )
		{
			A3DRELEASE(m_pVBarFrame)
			return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitIndirect(), failed to call m_pVBarFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pVBarFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( AUISUBDIALOG_RECT_HBAR == idType )
	{
		A3DRELEASE(m_pHBarFrame);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pHBarFrame = new AUIFrame;
		if( !m_pHBarFrame ) return false;

		bval = m_pHBarFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName);
		if( !bval )
		{
			A3DRELEASE(m_pHBarFrame)
			return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitIndirect(), failed to call m_pHBarFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pHBarFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( AUISUBDIALOG_RECT_FRAME == idType )
	{
		A3DRELEASE(m_pFrame);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pFrame = new AUIFrame;
		if( !m_pFrame ) return false;

		bval = m_pFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(m_pFrame)
			return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitIndirect(), failed to call m_pFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
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
		bval = m_pA2DSprite[idType]->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSprite[idType])
			return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUISUBDIALOG_RECT_UP == idType || AUISUBDIALOG_RECT_DOWN == idType || AUISUBDIALOG_RECT_LEFT == idType || AUISUBDIALOG_RECT_RIGHT == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
		m_rcImage[idType] = m_pA2DSprite[idType]->GetItem(0)->GetRect();
	}
		
	return true;
}

bool AUISubDialog::Release(void)
{
	A3DRELEASE(m_pVBarFrame);
	A3DRELEASE(m_pHBarFrame);
	A3DRELEASE(m_pFrame);
	for( int i = 0; i < AUISUBDIALOG_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	RemoveDialog();

	return AUIObject::Release();
}

bool AUISubDialog::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemSubDialog = new AXMLItem;
	pXMLItemSubDialog->InsertItem(pXMLItemDialog);
	pXMLItemSubDialog->SetName(_AL("SUBDIALOG"));
	XMLSET_STRING_VALUE(pXMLItemSubDialog, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemSubDialog, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemSubDialog, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemSubDialog, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemSubDialog, _AL("Height"), m_nHeight, 0)
	XMLSET_STRING_VALUE(pXMLItemSubDialog, _AL("DialogFile"), AS2AC(m_szDialogFileName), _AL(""))
	XMLSET_BOOL_VALUE(pXMLItemSubDialog, _AL("RenderDlgFrame"), m_bRenderDlgFrame, false)
	XMLSET_STRING_VALUE(pXMLItemSubDialog, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemSubDialog);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pA2DSprite[AUISUBDIALOG_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISUBDIALOG_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("VScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->GetName()), _AL(""))
	}
	if( m_pVBarFrame )
	{
		AXMLItem *pXMLItemBarFrameImage = new AXMLItem;
		pXMLItemBarFrameImage->InsertItem(pXMLItemResource);
		pXMLItemBarFrameImage->SetName(_AL("VBarFrameImage"));
		XMLSET_STRING_VALUE(pXMLItemBarFrameImage, _AL("FileName"), AS2AC(m_pVBarFrame->GetA2DSprite()->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISUBDIALOG_RECT_LEFT] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("LeftImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("RightImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("HScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->GetName()), _AL(""))
	}
	if( m_pHBarFrame )
	{
		AXMLItem *pXMLItemBarFrameImage = new AXMLItem;
		pXMLItemBarFrameImage->InsertItem(pXMLItemResource);
		pXMLItemBarFrameImage->SetName(_AL("HBarFrameImage"));
		XMLSET_STRING_VALUE(pXMLItemBarFrameImage, _AL("FileName"), AS2AC(m_pHBarFrame->GetA2DSprite()->GetName()), _AL(""))
	}
	if( m_pFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	return true;
}

bool AUISubDialog::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_LBUTTONDOWN ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_LBUTTONDBLCLK ||
		msg == WM_MOUSEMOVE ||
		msg == WM_LBUTTONUP )
	{
		POINT ptPos = GetPos();
		int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;

		if( WM_LBUTTONUP == msg )
		{
			m_bDraggingVScroll = false;
			m_bDraggingHScroll = false;
			m_pParent->SetCaptureObject(NULL);
			m_nState = AUISUBDIALOG_STATE_NORMAL;
		}
		// Click on VScroll bar.
		else if( m_bDraggingVScroll || m_nVBarMax > 0 && m_rcImage[AUISUBDIALOG_RECT_VBAR].PtInRect(x, y) )
		{
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUISUBDIALOG_STATE_PRESSVBAR == m_nState && m_nVBarMax > 0 )
					{
						int H = m_rcImage[AUISUBDIALOG_RECT_VBAR].Height() - m_nVBarHeight;
						SetVBarPos((y - m_rcImage[AUISUBDIALOG_RECT_VBAR].top - m_nDragDist) * m_nVBarMax / H);
					}
				}
				else
				{
					m_bDraggingVScroll = false;
					m_pParent->SetCaptureObject(NULL);
				}
			}
			else
			{
				// Page up.
				if( y < m_ptVBar.y )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUISUBDIALOG_STATE_PRESSUP;
						m_nScrollDelay = 0;
					}
				}
				// Page down.
				else if( y > m_ptVBar.y + m_nVBarHeight )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUISUBDIALOG_STATE_PRESSDOWN;
						m_nScrollDelay = 0;
					}
				}
				// VScroll bar.
				else if( m_nVBarMax > 0 )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingVScroll = true;
						m_nDragDist = y - m_ptVBar.y;
						m_pParent->SetCaptureObject(this);
						m_nState = AUISUBDIALOG_STATE_PRESSVBAR;
					}
				}
			}
		}
		// Click on HScroll bar.
		else if( m_bDraggingHScroll || m_nHBarMax > 0 && m_rcImage[AUISUBDIALOG_RECT_HBAR].PtInRect(x, y) )
		{
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUISUBDIALOG_STATE_PRESSHBAR == m_nState && m_nHBarMax > 0 )
					{
						int W = m_rcImage[AUISUBDIALOG_RECT_HBAR].Width() - m_nHBarWidth;
						SetHBarPos((x - m_rcImage[AUISUBDIALOG_RECT_HBAR].left - m_nDragDist) * m_nHBarMax / W);
					}
				}
				else
				{
					m_bDraggingHScroll = false;
					m_pParent->SetCaptureObject(NULL);
				}
			}
			else
			{
				// Page left.
				if( x < m_ptHBar.x )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUISUBDIALOG_STATE_PRESSLEFT;
						m_nScrollDelay = 0;
					}
				}
				// Page right.
				else if( x > m_ptHBar.x + m_nHBarWidth )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUISUBDIALOG_STATE_PRESSRIGHT;
						m_nScrollDelay = 0;
					}
				}
				// Scroll bar.
				else if( m_nHBarMax > 0 )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingHScroll = true;
						m_nDragDist = x - m_ptHBar.x;
						m_pParent->SetCaptureObject(this);
						m_nState = AUISUBDIALOG_STATE_PRESSHBAR;
					}
				}
			}
		}
		else if( m_nVBarMax > 0 && x >= m_rcImage[AUISUBDIALOG_RECT_VBAR].left && x <= m_rcImage[AUISUBDIALOG_RECT_VBAR].right )
		{
			// Click on up button.
			if( y < m_rcImage[AUISUBDIALOG_RECT_VBAR].top )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUISUBDIALOG_STATE_PRESSUP;
			}
			// Click on down button.
			else if( y > m_rcImage[AUISUBDIALOG_RECT_VBAR].bottom )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUISUBDIALOG_STATE_PRESSDOWN;
			}
		}
		else if( m_nHBarMax > 0 && y >= m_rcImage[AUISUBDIALOG_RECT_HBAR].top && y <= m_rcImage[AUISUBDIALOG_RECT_HBAR].bottom )
		{
			// Click on left button.
			if( x < m_rcImage[AUISUBDIALOG_RECT_HBAR].left )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUISUBDIALOG_STATE_PRESSLEFT;
			}
			// Click on right button.
			else if( x > m_rcImage[AUISUBDIALOG_RECT_HBAR].right )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUISUBDIALOG_STATE_PRESSRIGHT;
			}
		}

		return true;
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_bDraggingVScroll = false;
		m_bDraggingHScroll = false;
		m_nState = AUISUBDIALOG_STATE_NORMAL;
	}
	else if( WM_MOUSEWHEEL == msg )
	{
		int zDelta = (short)HIWORD(wParam);
		if( zDelta > 0 )
		{
			if( m_nVBarPos > AUISUBDIALOG_WHEEL_STEP )
				SetVBarPos(m_nVBarPos - AUISUBDIALOG_WHEEL_STEP);
			else
				SetVBarPos(0);
		}
		else if( zDelta < 0 )
		{
			if( m_nVBarPos + AUISUBDIALOG_WHEEL_STEP < m_nVBarMax )
				SetVBarPos(m_nVBarPos + AUISUBDIALOG_WHEEL_STEP);
			else
				SetVBarPos(m_nVBarMax);
		}
		
		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUISubDialog::Tick(void)
{
	DWORD dwTick = GetTickCount();
	if( dwTick - m_nScrollDelay >= AUISUBDIALOG_SCROLL_DELAY )
	{
		if( AUISUBDIALOG_STATE_PRESSUP == m_nState )
		{
			if( m_nVBarPos > AUISUBDIALOG_SCROLL_STEP )
				SetVBarPos(m_nVBarPos - AUISUBDIALOG_SCROLL_STEP);
			else
				SetVBarPos(0);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUISUBDIALOG_STATE_PRESSDOWN == m_nState )
		{
			if( m_nVBarPos + AUISUBDIALOG_SCROLL_STEP < m_nVBarMax )
				SetVBarPos(m_nVBarPos + AUISUBDIALOG_SCROLL_STEP);
			else
				SetVBarPos(m_nVBarMax);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUISUBDIALOG_STATE_PRESSLEFT == m_nState )
		{
			if( m_nHBarPos > AUISUBDIALOG_SCROLL_STEP )
				SetHBarPos(m_nHBarPos - AUISUBDIALOG_SCROLL_STEP);
			else
				SetHBarPos(0);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUISUBDIALOG_STATE_PRESSRIGHT == m_nState )
		{
			if( m_nHBarPos + AUISUBDIALOG_SCROLL_STEP < m_nHBarMax )
				SetHBarPos(m_nHBarPos + AUISUBDIALOG_SCROLL_STEP);
			else
				SetHBarPos(m_nHBarMax);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}

	return AUIObject::Tick();
}

bool AUISubDialog::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC )
	{
		if( m_pSubDialog )
		{
			POINT ptPos = GetPos();
			m_pSubDialog->SetPosEx(ptPos.x, ptPos.y);
		}
	}

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC )
	{
		bool bval = true;
		if( m_bRenderVScroll && m_nVBarMax > 0 )	
		{
			bval = RenderVScroll();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::Render(), failed to call RenderVScroll()");
		}

		if( m_bRenderHScroll && m_nHBarMax > 0 )	
		{
			bval = RenderHScroll();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::Render(), failed to call RenderHScroll()");
		}
	}

	return true;
}

void AUISubDialog::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	if( m_pSubDialog && m_pParent->IsShow() )
	{
		if( bShow )
		{
			m_pSubDialog->Show(true, false, false);
			m_pParent->GetAUIManager()->InsertDialogBefore(m_pSubDialog, m_pParent);
		}
		else
			m_pSubDialog->Show(false);
	}

	AUIObject::Show(bShow);
}

void AUISubDialog::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUISUBDIALOG_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pVBarFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pVBarFrame->GetA2DSprite(), bLoad);

	if (m_pHBarFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pHBarFrame->GetA2DSprite(), bLoad);

	if (m_pSubDialog)
		m_pSubDialog->OptimizeSpriteResource(bLoad);
}

bool AUISubDialog::RenderVScroll()
{
	if( !m_pVBarFrame || 
		!m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL] ||
		!m_pA2DSprite[AUISUBDIALOG_RECT_UP] ||
		!m_pA2DSprite[AUISUBDIALOG_RECT_DOWN] )
		return true;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	bool bval = true;
	POINT ptPos = GetPos();
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw up button.
	int nItem = (AUISUBDIALOG_STATE_PRESSUP == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUISUBDIALOG_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_UP]->SetCurrentItem()");

	int W = m_rcImage[AUISUBDIALOG_RECT_UP].Width();
	int x = ptPos.x + m_nWidth - W;
	int y = ptPos.y;
	m_pA2DSprite[AUISUBDIALOG_RECT_UP]->SetAlpha(nAlpha);
	m_pA2DSprite[AUISUBDIALOG_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_UP]->SetScaleY(fWindowScale);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_UP]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_UP]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int H = m_rcImage[AUISUBDIALOG_RECT_UP].Height();
	y += H;

	float fScale = float(m_rcImage[AUISUBDIALOG_RECT_VBAR].Height()) / m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUISUBDIALOG_STATE_PRESSDOWN == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->SetCurrentItem()");

	y = ptPos.y + m_nHeight - m_rcImage[AUISUBDIALOG_RECT_DOWN].Height();

	m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button
	H = m_nHeight - m_rcImage[AUISUBDIALOG_RECT_UP].Height() - m_rcImage[AUISUBDIALOG_RECT_DOWN].Height() - m_nVBarHeight;
	m_ptVBar.y = m_rcImage[AUISUBDIALOG_RECT_UP].Height() + m_nVBarPos * H / m_nVBarMax;
	m_pVBarFrame->SetScale(fWindowScale);
	bval = m_pVBarFrame->Render(x, ptPos.y + m_ptVBar.y, m_rcImage[AUISUBDIALOG_RECT_VSCROLL].Width(), m_nVBarHeight,
		NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderVScroll(), failed to call m_pVBarFrame->Render()");

	return true;
}

bool AUISubDialog::RenderHScroll()
{
	if( !m_pHBarFrame || 
		!m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL] ||
		!m_pA2DSprite[AUISUBDIALOG_RECT_LEFT] ||
		!m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT] )
		return true;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	POINT ptPos = GetPos();
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw left button.
	int nItem = (AUISUBDIALOG_STATE_PRESSLEFT == m_nState) ? 1 : 0;
	bool bval = m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->SetCurrentItem()");

	int H = m_rcImage[AUISUBDIALOG_RECT_LEFT].Height();
	int x = ptPos.x;
	int y = ptPos.y + m_nHeight - H;
	m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->SetAlpha(nAlpha);
	m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->SetScaleY(fWindowScale);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->DrawToInternalBuffer()");

	// Draw scroll area.
	int W = m_rcImage[AUISUBDIALOG_RECT_LEFT].Width();
	x += W;

	float fScale = float(m_rcImage[AUISUBDIALOG_RECT_HBAR].Width())	/ m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->GetItem(0)->GetRect().Width();
	m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->SetScaleX(fScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUISUBDIALOG_STATE_PRESSRIGHT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->SetCurrentItem()");

	x = ptPos.x + m_nWidth - m_rcImage[AUISUBDIALOG_RECT_RIGHT].Width() - (m_nVBarMax > 0 ? m_rcImage[AUISUBDIALOG_RECT_VSCROLL].Width() : 0);

	m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->DrawToBack() or m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->DrawToInternalBuffer()");

	// Draw scrollbar button
	W = m_nWidth - m_rcImage[AUISUBDIALOG_RECT_LEFT].Width() - m_rcImage[AUISUBDIALOG_RECT_RIGHT].Width() - m_nHBarWidth - (m_nVBarMax > 0 ? m_rcImage[AUISUBDIALOG_RECT_LEFT].Width() : 0);
	m_ptHBar.x = m_rcImage[AUISUBDIALOG_RECT_LEFT].Width() + m_nHBarPos * W / m_nHBarMax;
	m_pHBarFrame->SetScale(fWindowScale);
	bval = m_pHBarFrame->Render(ptPos.x + m_ptHBar.x, y, m_nHBarWidth, m_rcImage[AUISUBDIALOG_RECT_HBAR].Height(),
		NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISubDialog::RenderHScroll(), failed to call m_pHBarFrame->Render()");

	return true;
}

bool AUISubDialog::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "VScroll Area Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Left Button Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_LEFT] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_LEFT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Right Button Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_RIGHT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "HScroll Area Image") )
	{
		if( m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "VScroll Bar Image") )
	{
		if( m_pVBarFrame )
			strncpy(Property->fn, m_pVBarFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "HScroll Bar Image") )
	{
		if( m_pHBarFrame )
			strncpy(Property->fn, m_pHBarFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pFrame )
			strncpy(Property->fn, m_pFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Mode") )
	{
		Property->i = m_nFrameMode;
	}
	else if( 0 == strcmpi(pszPropertyName, "Dialog File") )
	{
		strncpy(Property->fn, m_szDialogFileName, AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Enable Dialog Frame") )
	{
		Property->b = m_bRenderDlgFrame;
	}
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUISubDialog::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "VScroll Area Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_VSCROLL);
	else if( 0 == strcmpi(pszPropertyName, "VScroll Bar Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_VBAR);
	else if( 0 == strcmpi(pszPropertyName, "Left Button Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_LEFT);
	else if( 0 == strcmpi(pszPropertyName, "Right Button Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_RIGHT);
	else if( 0 == strcmpi(pszPropertyName, "HScroll Area Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_HSCROLL);
	else if( 0 == strcmpi(pszPropertyName, "HScroll Bar Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_HBAR);
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUISUBDIALOG_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Frame Mode") )
	{
		m_nFrameMode = Property->i;
		if (m_pFrame) m_pFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Dialog File") )
		return LoadDialog(Property->fn);
	else if( 0 == strcmpi(pszPropertyName, "Enable Dialog Frame") )
		SetRenderDlgFrame(Property->b);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

void AUISubDialog::SetDialog(PAUIDIALOG pDlg)
{
	if( pDlg == m_pSubDialog )
		return;

	RemoveDialog();
	m_pSubDialog = pDlg;
	m_bIsLoad = false;
	PAUIMANAGER pUIManager = m_pParent->GetAUIManager();
	if( m_pSubDialog )
	{
		if( m_pParent && m_pParent->IsShow() && m_bShow )
		{
			m_pSubDialog->Show(true, false, false);
			pUIManager->InsertDialogBefore(m_pSubDialog, m_pParent);
		}
		SIZE sz = m_pSubDialog->GetSize();
		m_nSubDialogOrgWidth = sz.cx;
		m_nSubDialogOrgHeight = sz.cy;
		sz = m_pSubDialog->GetDefaultSize();
		m_nSubDialogOrgDefaultWidth = sz.cx;
		m_nSubDialogOrgDefaultHeight = sz.cy;
		m_pSubDialog->SetCanOutOfWindow(true);
		m_pSubDialog->SetParentDlgControl(this);
	}
	UpdateSubDialog();
}

bool AUISubDialog::LoadDialog(const char* pszTempName)
{
	if( stricmp(m_szDialogFileName, pszTempName) == 0 )
		return true;

	RemoveDialog();
	PAUIMANAGER pUIManager = m_pParent->GetAUIManager();
	if( !pszTempName || strlen(pszTempName) == 0 )
	{
		UpdateSubDialog();
		return true;
	}

	int idDlg = pUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, pszTempName);
	if( idDlg == AUIMANAGER_WRONGDIALOGID )
		return false;
	
	m_pSubDialog = pUIManager->GetDialog(idDlg);
	m_bIsLoad = true;
	if( m_pSubDialog )
	{
		if( m_pParent && m_pParent->IsShow() && m_bShow )
		{
			m_pSubDialog->Show(true, false, false);
			pUIManager->InsertDialogBefore(m_pSubDialog, m_pParent);
		}
		m_szDialogFileName = pszTempName;
		SIZE sz = m_pSubDialog->GetSize();
		m_nSubDialogOrgWidth = sz.cx;
		m_nSubDialogOrgHeight = sz.cy;
		sz = m_pSubDialog->GetDefaultSize();
		m_nSubDialogOrgDefaultWidth = sz.cx;
		m_nSubDialogOrgDefaultHeight = sz.cy;
		m_pSubDialog->SetCanOutOfWindow(true);
		m_pSubDialog->SetParentDlgControl(this);
		m_pSubDialog->SetBackgroundDialog(m_pParent->IsBackgroundDialog());
	}
	else
		 return false;

	UpdateSubDialog();

	return true;
}

void AUISubDialog::RemoveDialog()
{
	if( m_pSubDialog )
	{
		m_pSubDialog->SetParentDlgControl(NULL);
		m_pSubDialog->SetSize(m_nSubDialogOrgWidth, m_nSubDialogOrgHeight);
		PAUIOBJECTLISTELEMENT pElement = m_pSubDialog->GetFirstControl();
		while( pElement )
		{
			pElement->pThis->SetClipViewport(NULL);
			pElement = pElement->pNext;
		}
		
		m_pSubDialog->MoveAllControls(m_nHBarPos, m_nVBarPos);
		m_pSubDialog->DelFrame(m_pViewport);
		A3DRELEASE(m_pViewport);
		m_nSubDialogOrgWidth = m_nSubDialogOrgHeight = 0;
		m_nSubDialogOrgDefaultWidth = m_nSubDialogOrgDefaultHeight = 0;
		m_szDialogFileName = "";
		m_pSubDialog->Show(false);
		if( m_bIsLoad )
			m_pSubDialog->SetDestroyFlag(true);

		m_pSubDialog = NULL;
	}
}

void AUISubDialog::SetSubDialogOrgSize(int nWidth, int nHeight, bool bReset)
{
	if( bReset || m_nSubDialogOrgWidth != nWidth || m_nSubDialogOrgHeight != nHeight )
	{
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		m_nSubDialogOrgWidth = nWidth;
		m_nSubDialogOrgHeight = nHeight;
		m_nSubDialogOrgDefaultWidth = nWidth / fWindowScale;
		m_nSubDialogOrgDefaultHeight = nHeight / fWindowScale;
		UpdateSubDialog(bReset);
	}
}

void AUISubDialog::SetVBarPos(int nPos)
{
	a_Clamp(nPos, 0, m_nVBarMax);
	if( nPos != m_nVBarPos )
	{
		UpdateRenderTarget();

		if( m_pSubDialog )
			m_pSubDialog->MoveAllControls(0, m_nVBarPos - nPos);
	}
	m_nVBarPos = nPos;
}

void AUISubDialog::SetHBarPos(int nPos)
{
	a_Clamp(nPos, 0, m_nHBarMax);
	if( nPos != m_nHBarPos )
	{
		UpdateRenderTarget();

		if( m_pSubDialog )
			m_pSubDialog->MoveAllControls(m_nHBarPos - nPos, 0);
	}

	m_nHBarPos = nPos;
}

void AUISubDialog::UpdateSubDialog(bool bNewDialog)
{
	UpdateRenderTarget();

	if( !m_pSubDialog )
	{
		if (m_bForceRenderScroll)
		{
			m_nVBarPos = 0;
			m_nHBarPos = 0;
			m_nVBarMax = 100;	
			m_nHBarMax = 100;	
		}
		else
		{
			m_nVBarPos = 0;
			m_nHBarPos = 0;
			m_nVBarMax = 0;	
			m_nHBarMax = 0;	
			return;
		}		
	}
	
	int nVBarWidth;
	if( m_nSubDialogOrgHeight > m_nHeight && m_pA2DSprite[AUISUBDIALOG_RECT_VSCROLL] )
	{
		m_nVBarMax = m_nSubDialogOrgHeight - m_nHeight;
		nVBarWidth = m_rcImage[AUISUBDIALOG_RECT_VSCROLL].Width();
		m_rcImage[AUISUBDIALOG_RECT_VBAR].SetRect(m_nWidth - m_rcImage[AUISUBDIALOG_RECT_VSCROLL].Width(), m_rcImage[AUISUBDIALOG_RECT_UP].Height(), 
			m_nWidth, m_nHeight - m_rcImage[AUISUBDIALOG_RECT_DOWN].Height());
		if (m_pVBarFrame && m_pVBarFrame->GetA2DSprite())
			m_nVBarHeight = m_pVBarFrame->GetA2DSprite()->GetHeight();
		else
			m_nVBarHeight = 0;
	}
	else
	{
		m_nVBarMax = 0;
		nVBarWidth = 0;
		m_nVBarHeight = 0;
		m_rcImage[AUISUBDIALOG_RECT_VBAR].SetRect(0, 0, 0, 0);
	}

	if( m_nSubDialogOrgWidth > m_nWidth - nVBarWidth && m_pA2DSprite[AUISUBDIALOG_RECT_HSCROLL] )
	{
		m_nHBarMax = m_nSubDialogOrgWidth - m_nWidth + nVBarWidth;
		if( m_nVBarMax > 0 )
			m_nVBarMax += m_rcImage[AUISUBDIALOG_RECT_DOWN].Width();

		m_rcImage[AUISUBDIALOG_RECT_HBAR].SetRect(m_rcImage[AUISUBDIALOG_RECT_LEFT].Width(), m_nHeight - m_rcImage[AUISUBDIALOG_RECT_HSCROLL].Height(), 
			m_nWidth - m_rcImage[AUISUBDIALOG_RECT_RIGHT].Width() - nVBarWidth, m_nHeight);
		if (m_pHBarFrame && m_pHBarFrame->GetA2DSprite())
			m_nHBarWidth = m_pHBarFrame->GetA2DSprite()->GetWidth();
		else
			m_nHBarWidth = 0;
	}
	else
	{
		m_nHBarMax = 0;
		m_nHBarWidth = 0;
		m_rcImage[AUISUBDIALOG_RECT_HBAR].SetRect(0, 0, 0, 0);
	}

	if( bNewDialog )
	{
		m_pSubDialog->MoveAllControls(m_nHBarPos, m_nVBarPos);
		m_nVBarPos = 0;
		m_nHBarPos = 0;
	}
	else
	{
		if (m_nVBarPos > m_nVBarMax)
		{
			SetVBarPos(m_nVBarMax);
		}
		if (m_nHBarPos > m_nHBarMax)
		{
			SetHBarPos(m_nHBarMax);
		}
		// 		a_ClampRoof(m_nVBarPos, m_nVBarMax);
		// 		a_ClampRoof(m_nHBarPos, m_nHBarMax);
	}

	if( !m_pViewport )
	{
		m_pViewport = new A3DViewport;
		if( !m_pViewport )
		{
			AUI_ReportError(__LINE__, 1, "AUISubDialog::UpdateSubDialog(), not enough memory!");
			return;
		}

		A3DVIEWPORTPARAM parm = { 0,0,0,0,0.0f,1.0f };
		bool ret = m_pViewport->Init(m_pA3DDevice, &parm, false, false, 0);
		if(!ret)
		{
			AUI_ReportError(__LINE__, 1, "AUISubDialog::UpdateSubDialog(), failed to call m_pViewport->Init()");
			return;
		}
	}

	UpdateViewport();
}

void AUISubDialog::UpdateViewport()
{
	if( !m_pSubDialog )
		return;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	float p = 1.0f / fWindowScale;
	A3DRECT rc(0, 0, m_nWidth - (m_nVBarMax > 0 ? m_rcImage[AUISUBDIALOG_RECT_VBAR].Width() : 0), 
		m_nHeight - (m_nHBarMax > 0 ? m_rcImage[AUISUBDIALOG_RECT_HBAR].Height() : 0));
	m_pSubDialog->SetSize(rc.Width(), rc.Height());
	if( m_pA3DViewport )
	{
		A3DRECT *pParentViewportRect = m_pParent->GetFrameRect(m_pA3DViewport);
		if( pParentViewportRect )
		{
			A3DRECT rcParent = *pParentViewportRect;
			rcParent.SetRect(rcParent.left * fWindowScale, rcParent.top * fWindowScale, rcParent.right * fWindowScale, rcParent.bottom * fWindowScale);
			rcParent.Offset(-m_x, -m_y);
			rc &= rcParent;
		}
	}
	rc.SetRect(rc.left * p, rc.top * p, rc.right * p, rc.bottom * p);
	m_pSubDialog->ChangeFrame(m_pViewport, rc);
	PAUIOBJECTLISTELEMENT pElement = m_pSubDialog->GetFirstControl();
	while( pElement )
	{
		pElement->pThis->SetClipViewport(m_pViewport);
		if( pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG )
		{
			(PAUISUBDIALOG(pElement->pThis))->UpdateViewport();
		}
		pElement = pElement->pNext;
	}
}

void AUISubDialog::SetPos(int x, int y)
{
	if( x != m_x || y != m_y )
	{
		AUIObject::SetPos(x, y);
		if( m_pA3DViewport )
			UpdateViewport();
	}
	else
		AUIObject::SetPos(x, y);
}

void AUISubDialog::SetDefaultPos(int x, int y)
{
	if( x != m_nDefaultX || y != m_nDefaultY )
	{
		AUIObject::SetDefaultPos(x, y);
		if( m_pA3DViewport )
			UpdateViewport();
	}
	else
		AUIObject::SetDefaultPos(x, y);
}

void AUISubDialog::SetSize(int W, int H)
{
	if( W != m_nWidth || H != m_nHeight )
	{
		if( m_pSubDialog )
			m_pSubDialog->MoveAllControls(m_nHBarPos, m_nVBarPos);

		m_nVBarPos = 0;
		m_nHBarPos = 0;
		AUIObject::SetSize(W, H);
		UpdateSubDialog();
	}
	else
		AUIObject::SetSize(W, H);
}

void AUISubDialog::SetDefaultSize(int W, int H)
{
	if( W != m_nDefaultWidth || H != m_nDefaultHeight )
	{
		if( m_pSubDialog )
			m_pSubDialog->MoveAllControls(m_nHBarPos, m_nVBarPos);

		m_nVBarPos = 0;
		m_nHBarPos = 0;
		AUIObject::SetDefaultSize(W, H);
		UpdateSubDialog();
	}
	else
		AUIObject::SetDefaultSize(W, H);
}

bool AUISubDialog::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	POINT ptPos = GetPos();
	x -= m_nOffX + ptPos.x;
	y -= m_nOffY + ptPos.y;

	if( m_bRenderVScroll && m_nVBarMax > 0 && x >= m_rcImage[AUISUBDIALOG_RECT_VBAR].left ||
		m_bRenderHScroll && m_nHBarMax > 0 && y >= m_rcImage[AUISUBDIALOG_RECT_HBAR].top )
		return true;

	return false;
}

bool AUISubDialog::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	for( int i = 0; i < AUISUBDIALOG_RECT_NUM; i++ )
		if( m_pA2DSprite[i] )
		{
			m_rcImage[i] = m_pA2DSprite[i]->GetItem(0)->GetRect();
			m_rcImage[i].SetRect(m_rcImage[i].left * fWindowScale, m_rcImage[i].top * fWindowScale,
				m_rcImage[i].right * fWindowScale, m_rcImage[i].bottom * fWindowScale);
		}

	m_nSubDialogOrgWidth = m_nSubDialogOrgDefaultWidth * fWindowScale;
	m_nSubDialogOrgHeight = m_nSubDialogOrgDefaultHeight * fWindowScale;

	float fx = 0.0f;
	float fy = 0.0f;
	if( m_nHBarMax != 0 )
		fx = float(m_nHBarPos) / m_nHBarMax;
	if( m_nVBarMax != 0 )
		fy = float(m_nVBarPos) / m_nVBarMax;
	UpdateSubDialog(false);

	m_nHBarPos = m_nHBarMax * fx;
	m_nVBarPos = m_nVBarMax * fy;

	return true;
}

void AUISubDialog::ForceRenderHVScroll(bool bForce /* = true */)
{
	m_bForceRenderScroll = bForce;
}
