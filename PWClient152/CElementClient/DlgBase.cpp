// Filename	: DlgBase.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005


#include "DlgBase.h"
#include "AFI.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_LoginUIMan.h"
#include "EC_CrossServer.h"

// disable warning "extern on member function"
#pragma warning ( disable : 4630 )

#define HELP_PREFIX "HelpName_"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBase, CDlgTheme)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommand_Close)
AUI_ON_COMMAND("Btn_Help",		OnCommand_Help)
AUI_ON_COMMAND(HELP_PREFIX"*",	OnCommand_Help)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBase, CDlgTheme)
AUI_ON_EVENT(NULL,		WM_KILLFOCUS,		OnEventKillFocus)
AUI_END_EVENT_MAP()

CRenderBuffer::CRenderBuffer(int width, int height) : m_iWidth(0), m_iHeight(0), m_Buffer(0), m_iRowSpan(0)
{
	Reserve(width, height);
}

CRenderBuffer::~CRenderBuffer()
{
	if(m_Buffer)
		delete[] m_Buffer;
}

void CRenderBuffer::Reserve(int width, int height)
{
	if(m_iWidth != width || m_iHeight != height)
	{
		m_iWidth = width;
		m_iHeight = height;
		
		m_iRowSpan = width * 4;
		
		if(m_Buffer)
			delete[] m_Buffer;
		
		m_Buffer = new unsigned char[width * height * 4];
		memset(m_Buffer, 128, width * height * 4);
	}
}

void CRenderBuffer::CopyArea(RECT srcRect, unsigned char* srcBuffer, int srcRowSpan)
{
	if(srcRect.right <= m_iWidth && srcRect.bottom <= m_iHeight)
	{
		int srcWidth = srcRect.right - srcRect.left;
		int srcHeight = srcRect.bottom - srcRect.top;
		
		for(int row = 0; row < srcHeight; row++)
			memcpy(m_Buffer + (row + srcRect.top) * m_iRowSpan + (srcRect.left * 4), srcBuffer + row * srcRowSpan, srcWidth * 4);
	}
}

void CRenderBuffer::BlitBGR(unsigned char* destBuffer, int destRowSpan)
{
	for(int row = 0; row < m_iHeight; row++)
		memcpy(destBuffer + row * destRowSpan, m_Buffer + row * m_iRowSpan, m_iRowSpan);
}

void CRenderBuffer::ToGray()
{
	for (int row = 0;row < m_iHeight;row++)
	{
		for (int column = 0;column < m_iRowSpan;column += 4)
		{
			int r = m_Buffer[m_iRowSpan * row + column + 0];
			int g = m_Buffer[m_iRowSpan * row + column + 1];
			int b = m_Buffer[m_iRowSpan * row + column + 2];
			char avg = (r + g + b) / 3;
			m_Buffer[m_iRowSpan * row + column + 0] = avg;
			m_Buffer[m_iRowSpan * row + column + 1] = avg;
			m_Buffer[m_iRowSpan * row + column + 2] = avg;
		}
	}
}

void CRenderBuffer::FlipX()
{
	int flipwdith = m_iRowSpan / 2;
	for (int row = 0; row < m_iHeight; row++)
	{
		for (int column = 0; column < flipwdith; column += 4)
		{
			std::swap(m_Buffer[m_iRowSpan * row + column], m_Buffer[m_iRowSpan * row + m_iRowSpan - column - 4]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 1], m_Buffer[m_iRowSpan * row + m_iRowSpan - column - 3]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 2], m_Buffer[m_iRowSpan * row + m_iRowSpan - column - 2]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 3], m_Buffer[m_iRowSpan * row + m_iRowSpan - column - 1]);
		}
	}
}

void CRenderBuffer::FlipY()
{
	int flipwdith = m_iHeight / 2;
	for (int column = 0; column < m_iRowSpan; column += 4)
	{
		for (int row = 0; row < flipwdith; ++row)
		{
			std::swap(m_Buffer[m_iRowSpan * row + column], m_Buffer[m_iRowSpan * (m_iHeight - row - 1) + column]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 1], m_Buffer[m_iRowSpan * (m_iHeight - row - 1) + column + 1]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 2], m_Buffer[m_iRowSpan * (m_iHeight - row - 1) + column + 2]);
			std::swap(m_Buffer[m_iRowSpan * row + column + 3], m_Buffer[m_iRowSpan * (m_iHeight - row - 1) + column + 3]);
		}
	}
}

CDlgBase::CDlgBase()
{
}

CDlgBase::~CDlgBase()
{
}

bool CDlgBase::OnInitDialog()
{
	if (!AUIDialog::OnInitDialog())
		return false;

	return true;
}

void CDlgBase::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgBase::OnCommand_Close(const char * szCommand)
{
	OnCommand("IDCANCEL");
}

void CDlgBase::OnCommand_Help(const char * szCommand)
{
	// try to get the help window by self name or command string
	PAUIDIALOG pDlg = NULL;
	if(strstr(szCommand, HELP_PREFIX))
	{
		pDlg = m_pAUIManager->GetDialog(szCommand + strlen(HELP_PREFIX));
	}
	else
	{
		AString helpDlg;
		pDlg = m_pAUIManager->GetDialog(helpDlg.Format("%sHelp", GetName()));
	}
	
	if(pDlg)
	{
		pDlg->Show(!pDlg->IsShow());
	}
}

void CDlgBase::OnEventKillFocus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (GetFocus()){
		if (GetFocus()->GetType() == AUIOBJECT_TYPE_COMBOBOX && ((PAUICOMBOBOX)GetFocus())->GetState() == AUICOMBOBOX_STATE_POPUP ||
			GetFocus()->GetType() == AUIOBJECT_TYPE_LISTBOX && ((PAUILISTBOX)GetFocus())->GetComboBox() != NULL){
			//	焦点是 COMBOBOX 时，隐藏 COMBOBOX 列表
			ChangeFocus(NULL);
		}
	}
}

void CDlgBase::DisableForCrossServer(const char *szObjName)
{
	//	For CECInGameUIMan

	if (!szObjName ||!GetGameUIMan())
		return;
	
	if (CECCrossServer::Instance().IsOnSpecialServer())
	{
		PAUIOBJECT pObj = GetDlgItem(szObjName);
		if (pObj)
		{
			pObj->Enable(false);
			pObj->SetHint(GetStringFromTable(10130));
		}
	}
}

void CDlgBase::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// do nothing by default
}

int  CDlgBase::GetNameIndex(const char *szName, const char *szPrefix)
{
	if (!szName || !szPrefix || strstr(szName, szPrefix) != szName){
		return -1;
	}
	int nPrefixLen = strlen(szPrefix);
	const char *p = szName + nPrefixLen;
	if (!*p){
		return -1;	//	szName == szPrefix，不能解析出 index
	}
	while (*p && isdigit(*p)){	//	排除出现 szName 为 "Item_9ad" 、"Item_Bg1"、szPrefix 为 "Item_" 的情况
		++ p;
	}
	if (*p){
		return -1;
	}
	return atoi(szName+strlen(szPrefix));
}

int  CDlgBase::GetNameIndex(PAUIOBJECT pObj, const char *szPrefix)
{
	return GetNameIndex(pObj->GetName(), szPrefix);
}

bool CDlgBase::CheckNamePrefix(const char *szName, const char *szPrefix){
	return szName != NULL && szPrefix != NULL && szPrefix[0] && strstr(szName, szPrefix) == szName;
}

bool CDlgBase::CheckNamePrefix(PAUIOBJECT pObj, const char *szPrefix){
	return pObj != NULL && CheckNamePrefix(pObj->GetName(), szPrefix);
}

A2DSprite* CDlgBase::GetCoverDX(const char* str_image, int width, int height)
{
	bool bSuccess(false);
	A2DSprite* ret = NULL;
	A3DTexture* pTexture = NULL;
	while (true)
	{
		AString strPath;
		strPath.Format("Surfaces\\%s", str_image);
		if (!af_IsFileExist(strPath))
			break;
		pTexture = new A3DTexture;
		if (!pTexture->LoadFromFile(g_pGame->GetA3DDevice(), strPath, width, height, A3DFMT_A8R8G8B8))
			break;

		ret = new A2DSprite;
		A3DRECT rect_a;
		rect_a.left = 0;
		rect_a.top = 0;
		rect_a.right = width;
		rect_a.bottom = height;
		if(!ret->InitWithoutSurface(g_pGame->GetA3DDevice(), width, height, A3DFMT_A8R8G8B8, 1, &rect_a))
			break;
		BYTE* pBits = NULL;
		int iPitch;
		if (!pTexture->LockRect(NULL,(void**)&pBits,&iPitch,0)){
			break;
		}else{
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = width;
			rect.bottom = height;
			CRenderBuffer* pBuffer = new CRenderBuffer(width, height);
			pBuffer->CopyArea(rect, pBits, iPitch);
			pBuffer->FlipY();
			if (ret->UpdateTextures(pBuffer->m_Buffer, iPitch, A3DFMT_A8R8G8B8)){
				bSuccess = true;
			}
			delete pBuffer;
		}
		pTexture->UnlockRect();
		break;
	}
	if (pTexture)
		A3DRELEASE(pTexture);
	if (!bSuccess)
		A3DRELEASE(ret);
	return ret;
}

void CDlgBase::AlignToDlgItem(AUIObject *pObject, int nInsideX, int nAlignX,
	int nInsideY, int nAlignY, int nOffsetX/*= 0*/, int nOffsetY/*= 0*/)
{
	SIZE sBase ;
	POINT ptBase;

	if (pObject)
	{
		sBase = pObject->GetSize();
		ptBase = pObject->GetPos();
	}
	else
	{
		ptBase.x = ptBase.y = 0;
		A3DRECT rcWindow = m_pAUIManager->GetRect();		
		sBase.cx = rcWindow.Width();
		sBase.cy = rcWindow.Height();
	}
	RECT rect;
	rect.left = ptBase.x;
	rect.right = ptBase.x + sBase.cx;
	rect.top = ptBase.y;
	rect.bottom = ptBase.y + sBase.cy;

	AlignToRect(rect,nInsideX,nAlignX,nInsideY,nAlignY,nOffsetX,nOffsetY);
}

void CDlgBase::AlignToRect(RECT rect, int nInsideX, int nAlignX,
	int nInsideY, int nAlignY, int nOffsetX/*= 0*/, int nOffsetY/*= 0*/)
{
	SIZE sBase ;
	POINT ptBase;
	ptBase.x = rect.left;
	ptBase.y = rect.top;
	sBase.cx = rect.right - rect.left;
	sBase.cy = rect.bottom - rect.top;

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

CECGame * CDlgBase::GetGame() { return g_pGame; }
elementdataman * CDlgBase::GetElementDataMan(){ return GetGame()->GetElementDataMan(); }
CECGameRun * CDlgBase::GetGameRun() { return GetGame()->GetGameRun(); }
CECGameSession * CDlgBase::GetGameSession() { return GetGame()->GetGameSession(); }
CECWorld * CDlgBase::GetWorld() { return GetGameRun()->GetWorld(); }
CECHostPlayer * CDlgBase::GetHostPlayer() { return GetGameRun()->GetHostPlayer(); }
CECGameUIMan * CDlgBase::GetGameUIMan() { return dynamic_cast<CECGameUIMan *>(m_pAUIManager); }
CECLoginUIMan * CDlgBase::GetLoginUIMan() { return dynamic_cast<CECLoginUIMan *>(m_pAUIManager); }
CECBaseUIMan * CDlgBase::GetBaseUIMan() { return dynamic_cast<CECBaseUIMan *>(m_pAUIManager); }

CDlgDynamicControl::CDlgDynamicControl()
{

}

CDlgDynamicControl::~CDlgDynamicControl()
{
	ClearControls();
}

bool CDlgDynamicControl::OnCommandMap(const char *szCommand)
{
	size_t count = m_Controls.size();
	for (size_t i = 0; i < count; ++i) {
		const DynamicControl& entry = m_Controls[i];
		if (IsMatch(szCommand, entry.name, false)) {
			if (entry.command)
				return (*(entry.command))();
		}
	}
	return CDlgBase::OnCommandMap(szCommand);
}

PAUIOBJECT CDlgDynamicControl::CreateControlWithCommand(const ACString& hint, int type, ControlCommand* command)
{
	PAUIOBJECT ret = CreateControl(m_pA3DEngine, m_pA3DDevice, type, true);
	if (ret) {
		ret->SetHint(hint);
		DynamicControl entry;
		entry.control = ret;
		entry.name = ret->GetName();
		entry.command = command;
		m_Controls.push_back(entry);
	}
	return ret;
}

void CDlgDynamicControl::ClearControls()
{
	size_t count = m_Controls.size();
	for (size_t i = 0; i < count; ++i) {
		DeleteControl(m_Controls[i].control);
		delete m_Controls[i].command;
	}
	m_Controls.clear();
}