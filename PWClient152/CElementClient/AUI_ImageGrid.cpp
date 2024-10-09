// Filename	: AUI_ImageGrid.cpp
// Creator	: Xu Wenbin
// Date		: 2010/5/19

#include "AUI_ImageGrid.h"
#include "EC_Game.h"

#include <AUIManager.h>
#include <A3DEngine.h>

#include <windowsx.h>

extern CECGame * g_pGame;

AUI_ImageGrid::AUI_ImageGrid()
{
	m_pDlgOwner = NULL;
	m_nFirstIndex = 0;
	m_nMaxRowVisible = 0;
	m_nMaxColVisible = 0;
	m_pScroll = NULL;
	m_pBkground = NULL;
	m_nFirstOffset = 0;
	m_bShow = true;
	m_nSelectedVirtualRow = -1;
	m_nSelectedVirtualCol = -1;
	m_pSpriteSelected = NULL;
	m_pSpriteMouseOn = NULL;
}

AUI_ImageGrid::~AUI_ImageGrid()
{
	ResetContent();
}

bool AUI_ImageGrid::IsReady()const
{
	return GetDialog() != NULL;
}

void AUI_ImageGrid::ResetContent()
{
	// �����������

	if (m_pDlgOwner)
	{
		m_pDlgOwner = NULL;
		m_nFirstIndex = 0;
		m_nMaxRowVisible = 0;
		m_nMaxColVisible = 0;
		m_pScroll = NULL;
		m_pBkground = NULL;
		m_nFirstOffset = 0;
		m_bShow = true;
		m_nSelectedVirtualRow = -1;
		m_nSelectedVirtualCol = -1;
		m_pSpriteSelected = NULL;
		m_pSpriteMouseOn = NULL;
		
		m_objs.clear();
		m_data.clear();
	}
}

bool AUI_ImageGrid::Init(PAUIDIALOG pDlgParent, const char *pszFirstName, int nMaxRowVisible, int nMaxColVisible, const char *pszScrollName, const char *pszBackground, A2DSprite *pSpriteSelected, A2DSprite *pSpriteMouseOn)
{
	// ��ʼ���ؼ�

	bool bRet = false;

	ResetContent();

	while (true)
	{
		// �������
		if (pDlgParent == NULL ||
			pszFirstName == NULL ||
			nMaxRowVisible <= 0 ||
			nMaxColVisible <= 0 ||
			pszScrollName == NULL)
		{
			break;
		}

		m_pDlgOwner = pDlgParent;
		m_nMaxRowVisible = nMaxRowVisible;
		m_nMaxColVisible = nMaxColVisible;

		// �����ؼ������п��ޣ�
		if (pszBackground && pszBackground[0])
		{
			m_pBkground = pDlgParent->GetDlgItem(pszBackground);
			if (!m_pBkground)
			{
				break;
			}
		}
		
		// ������
		PAUIOBJECT pScroll = pDlgParent->GetDlgItem(pszScrollName);
		m_pScroll = dynamic_cast<PAUISCROLL>(pScroll);
		if (m_pScroll == NULL)
		{
			break;
		}

		// �ؼ���ȡ
		if (!GetPrefixIndex(pszFirstName, m_strPrefix, m_nFirstIndex))
		{
			break;
		}
		m_nFirstOffset = 0;
		
		AString strName;
		PAUIIMAGEPICTURE pImage(NULL);
		bool bError(false);
		for (int r = 0; r < nMaxRowVisible; ++ r)
		{
			for (int c = 0; c < nMaxColVisible; ++ c)
			{
				strName = MakeName(r, c);
				pImage = dynamic_cast<PAUIIMAGEPICTURE>(pDlgParent->GetDlgItem(strName));
				if (!pImage)
				{
					bError = true;
					break;
				}
				pImage->SetAcceptMouseMessage(true);
				pImage->SetRenderCallback(GridImageRenderCallback, (DWORD)this, (DWORD)pImage);
				m_objs.push_back(pImage);
			}
			if (bError)
			{
				break;
			}
		}
		if (bError)
		{
			break;
		}
		m_pSpriteSelected = pSpriteSelected;
		m_pSpriteMouseOn = pSpriteMouseOn;

		// ���³�ʼ��ʾ
		UpdateView(GetFirstOffset());

		bRet = true;
		break;
	}

	if (!bRet)
	{
		ResetContent();
	}

	return bRet;
}

void AUI_ImageGrid::Append(DWORD dwData, void *pvData, const ACHAR *pszHint, A2DSprite *pA2DSprite, bool bUpdateView /* = false*/)
{
	// ��ӿ�������
	// ����Ч��ԭ���� Append ���һ��Ԫ��ʱ���Ƽ��� bUpdateView ����Ϊ true
	//

	if (!IsReady())
	{
		// ����ӿ��ӿؼ��󣬲ſ������ʾ����
		return;
	}

	GridItemData item;

	item.dwData = dwData;
	item.pvData = pvData;
	item.strHint = pszHint;
	item.pA2DSprite = pA2DSprite;

	m_data.push_back(item);
		
	// ���µ�ǰ��ͼ
	//
	if (bUpdateView)
	{
		UpdateView(GetFirstOffset());
	}
}

void AUI_ImageGrid::UpdateView(int nFirstOffset)
{
	// ������ʾ��ԭ�����Ϊ��ʼ�������ݱ仯��
	//
	
	if (!IsReady())
	{
		// δ��ʼ��
		return;
	}

	if (!IsShown())
	{
		// ��ǰ��������״̬
		return;
	}

	if (nFirstOffset < 0 ||
		nFirstOffset % GetMaxCol())
	{
		// nFirstShow �Ƿ�������ĳ�еĵ�һ���ؼ�
		return;
	}

	// ���±����ؼ�
	if (m_pBkground){
		m_pBkground->Show(true);
	}
	
	// ���¹�����λ��
	if (m_pScroll)
	{

		int nMaxVisible = GetMaxRow() * GetMaxCol();
		int nCurCount = GetCount();

		if (nCurCount <= nMaxVisible)
		{
			m_pScroll->SetBarLevel(0);
			m_pScroll->Show(false);
		}
		else
		{
			// ����������
			int nRows = nCurCount/GetMaxCol();
			if (nCurCount % GetMaxCol()){
				// ����һ��
				nRows ++;
			}

			// ������Ҫ������ҳ����
			m_pScroll->SetScrollRange(0, nRows-GetMaxRow());
			m_pScroll->SetScrollStep(1);
			m_pScroll->SetBarLength(-1);
			m_pScroll->Show(true);

			// ���㵱ǰ��
			m_pScroll->SetBarLevel(GetBarLevelFromOffset(nFirstOffset));
		}
	}

	// ���¸��ؼ����ݺ���ʾ
	m_nFirstOffset = nFirstOffset;

	int nBarLevel = nFirstOffset / GetMaxCol();
	for (int r = 0; r < GetMaxRow(); ++ r)
	{
		for (int c = 0; c < GetMaxCol(); ++ c)
		{
			PAUIIMAGEPICTURE pObj = GetObject(r, c);
			const GridItemData *pData = GetItemData(nBarLevel+r, c);
			if (pData)
			{
				pObj->Show(true);
				pObj->SetData(pData->dwData);
				pObj->SetDataPtr(pData->pvData);
				pObj->SetHint(pData->strHint);
				if (pData->pA2DSprite){
					pObj->SetCover(pData->pA2DSprite, 0);
				}
				else{
					pObj->SetCover(NULL, -1);
				}
			}
			else
			{
				pObj->Show(false);
				pObj->SetData(0);
				pObj->SetDataPtr(NULL);
				pObj->SetHint(_AL(""));
				pObj->SetCover(NULL, -1);
			}
		}
	}
}

AString AUI_ImageGrid::MakeName(int nRealRow, int nRealCol)const
{
	// ���㵱ǰӦ��ʾ������Ϊ [nRealRow, nRealCol] �Ŀؼ�������
	// Ҫ��ǰ�ؼ�����ȷ��ʼ��
	//
	AString strName;
	if (IsReady())
	{
		int nIndex = GetFirstIndex() + nRealRow*GetMaxCol() + nRealCol;
		strName.Format("%s%02d", m_strPrefix, nIndex);
	}
	return strName;
}

PAUIIMAGEPICTURE AUI_ImageGrid::GetObject(int nRealRow, int nRealCol)
{
	// ���� [nRealRow, nRealCol] ���Ŀؼ�
	//
	PAUIIMAGEPICTURE pObj = NULL;
	if (IsReady())
	{
		int nIndex = nRealRow*GetMaxCol() + nRealCol;
		if (nIndex < (int)m_objs.size())
		{
			pObj = m_objs[nIndex];
		}
	}
	return pObj;
}

PAUIIMAGEPICTURE AUI_ImageGrid::GetVirtualObject(int nVirtualRow, int nVirtualCol)
{
	//	���� [nVirtualRow, nVirtualCol] ���Ŀؼ�
	if (IsReady() && nVirtualRow >= 0 && nVirtualCol >= 0){
		int nRealRow = nVirtualRow - GetBarLevelFromOffset(GetFirstOffset());
		if (nRealRow >= 0){
			return GetObject(nRealRow, nVirtualCol);
		}
	}
	return NULL;
}

bool AUI_ImageGrid::GetPrefixIndex(const char *pszName, AString &strPrefix, int &nIndex)const
{
	// �� Image_1 �����Ʒ��� Image_ ������ǰ׺���� 1 �±�
	// ���ڳ�ʼ����̿ؼ�
	//

	bool bRet(false);

	AString strName = pszName;
	if (!strName.IsEmpty())
	{
		int nLen = strName.GetLength();
		int p = nLen-1;
		while (p>=0 && isdigit(strName[p]))
			p--;
		if (p < nLen-1)
		{
			// ������һ������
			strPrefix = strName.Left(p+1);
			strName.CutLeft(p+1);
			nIndex = strName.ToInt();
			bRet = true;
		}
	}
	return bRet;
}

const AUI_ImageGrid::GridItemData * AUI_ImageGrid::GetItemData(int nVirtualRow, int nVirtualCol)const
{
	// ��ѯλ��Ϊ [nVirtualRown, nVirtualCol] ��������
	//
	const GridItemData *pData = NULL;
	int nIndex = nVirtualRow * GetMaxCol() + nVirtualCol;
	if (nIndex >= 0 && nIndex < GetCount()){
		pData = &m_data[nIndex];
	}
	return pData;
}

PAUIOBJECT AUI_ImageGrid::OnMouse_Clicked(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	// ����������ؼ����ں���Ч���ݣ����������������������ͼ����
	//

	PAUIOBJECT pObjClicked = NULL;
	if (IsReady())
	{
		if (IsShown())
		{
			PAUISCROLL pScroll = GetScroll();
			if (pObj == pScroll)
			{
				// ����������
				UpdateView(GetOffsetFromBarLevel(pScroll->GetBarLevel()));
				
				// ���з����ݿؼ������ؿ�
			}
			else
			{
				// ���ܵ��������ؼ�
				for (int i(0); i < (int)m_objs.size(); ++ i)
				{
					PAUIIMAGEPICTURE pCur = m_objs[i];
					if (pCur == pObj)
					{
						// �ҵ���ѡ�еĿؼ�
						pObjClicked = pCur;
						m_nSelectedVirtualRow = i/GetMaxCol() + GetBarLevelFromOffset(GetFirstOffset());
						m_nSelectedVirtualCol = i%GetMaxCol();
						break;
					}
				}
			}
		}
	}
	return pObjClicked;
}

bool AUI_ImageGrid::CanAcceptMouseWheel(PAUIOBJECT pObj)
{
	// �жϸ��������ܷ��¹���������
	//
	bool bAccept(false);

	if (pObj)
	{
		while (true)
		{
			// ����Ƿ����Թ�����
			if (GetScroll() == pObj)
			{
				bAccept = true;
				break;
			}

			// ����Ƿ����Ա����ؼ�
			if (GetBkground() == pObj)
			{
				bAccept = true;
				break;
			}

			// ����Ƿ��������ݿؼ���Img_��			
			for (int r = 0; r < GetMaxRow(); ++ r)
			{
				for (int c = 0; c < GetMaxCol(); ++ c)
				{
					PAUIIMAGEPICTURE p = GetObject(r, c);
					if (p && p->IsShow() && p == pObj)
					{
						bAccept = true;
						break;
					}
				}
				if (bAccept)
				{
					break;
				}
			}
			break;
		}
	}
	return bAccept;
}

bool AUI_ImageGrid::IsMouseOn(PAUIOBJECT pObj)
{
	if (pObj){
		PAUIMANAGER pUIMan = m_pDlgOwner->GetAUIManager();
		PAUIDIALOG pDlgMouseOn(NULL);
		PAUIOBJECT pObjMouseOn(NULL);
		pUIMan->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
		return pDlgMouseOn == m_pDlgOwner && pObjMouseOn == pObj;
	}
	return false;
}

bool AUI_ImageGrid::IsSelected(PAUIOBJECT pObj)
{
	PAUIIMAGEPICTURE pObjSelected = GetVirtualObject(m_nSelectedVirtualRow, m_nSelectedVirtualCol);
	return pObjSelected && pObjSelected->IsShow() && pObjSelected == pObj;
}

bool AUI_ImageGrid::OnMouse_Wheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	// ������������������ͼ����
	//
	bool bRet(false);
	
	if (IsShown())
	{
		PAUISCROLL pScroll = GetScroll();
		if (pScroll)
		{
			if (CanAcceptMouseWheel(pObj))
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				int zDelta = (short)HIWORD(wParam);
				if( zDelta > 0 )
					zDelta = 1;
				else
					zDelta = -1;
				if( pScroll->IsShow() )
				{
					int nBarLevel = pScroll->GetBarLevel() - zDelta;
					pScroll->SetBarLevel(nBarLevel);
					UpdateView(GetOffsetFromBarLevel(pScroll->GetBarLevel()));
				}
				bRet = true;
			}
		}
	}
	
	return bRet;
}

void AUI_ImageGrid::OnTick()
{
	// ���������������ͼ����
	//
	if (IsReady())
	{
		if (IsShown())
		{
			PAUISCROLL pScroll = GetScroll();
			if (pScroll)
			{
				int nFirstOffset = GetOffsetFromBarLevel(pScroll->GetBarLevel());
				if (nFirstOffset != GetFirstOffset())
				{
					// �������������ק�����
					UpdateView(nFirstOffset);
				}
			}
		}
	}
}

int AUI_ImageGrid::GetOffsetFromBarLevel(int nBarLevel)const
{
	// �ӹ�������λ�ò�ѯ��Ӧ�е�һ���ؼ���offset
	//
	return nBarLevel*GetMaxCol();
}

int AUI_ImageGrid::GetBarLevelFromOffset(int nOffset)const
{
	// ����Ӧ��ĳ���ؼ���ѯ����ж�Ӧ�Ĺ�����λ��
	//
	int nMaxCol = GetMaxCol();
	return nMaxCol ? (nOffset/nMaxCol) : 0;
}

void AUI_ImageGrid::Show(bool bShow)
{
	if (IsReady())
	{
		if (IsShown() != bShow)
		{
			m_bShow = bShow;
			if (bShow)
			{
				// ������ͼ��ʾ
				UpdateView(GetFirstOffset());
			}
			else
			{
				// �������пؼ�
				int nMaxRow = GetMaxRow();
				int nMaxCol = GetMaxCol();
				for (int r = 0; r < nMaxRow; ++ r)
				{
					for (int c = 0; c < nMaxCol; ++ c)
					{
						PAUIOBJECT pObj = GetObject(r, c);
						if (pObj){
							pObj->Show(false);
						}
					}
				}
				PAUISCROLL pScroll = GetScroll();
				if (pScroll){
					pScroll->Show(false);
				}
				PAUIOBJECT pBkground = GetBkground();
				if (pBkground){
					pBkground->Show(false);
				}
			}
		}
	}
}

void AUI_ImageGrid::ResetImages(bool bRelease)
{
	for(int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		GridItemData& item = m_data[nIndex];
		A2DSprite* pImage = item.pA2DSprite;
		item.pA2DSprite = NULL;

		if(bRelease && pImage) A3DRELEASE(pImage);
	}
}

void AUI_ImageGrid::GridImageRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3)
{
	AUI_ImageGrid *pImageGrid = (AUI_ImageGrid *)param1;
	PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)param2;
	pObj->SetRenderCallback(NULL, 0);
	pObj->Render();
	if (pImageGrid->IsReady()){
		if (pImageGrid->m_pSpriteSelected || pImageGrid->m_pSpriteMouseOn){
			g_pGame->GetA3DEngine()->FlushInternalSpriteBuffer();

			A3DPOINT2 ptCenter = rc.CenterPoint();
			int x = ptCenter.x;
			int y = ptCenter.y;
			PAUIDIALOG pDlg = pImageGrid->GetDialog();
			float fScale = pDlg->GetScale() * pDlg->GetAUIManager()->GetWindowScale();

			A2DSprite *pSprite = pImageGrid->m_pSpriteSelected;
			if (pSprite && pImageGrid->IsSelected(pObj)){
				pSprite->SetScaleX(fScale);
				pSprite->SetScaleY(fScale);
				pSprite->DrawToBack(x, y);
			}
			pSprite = pImageGrid->m_pSpriteMouseOn;
			if (pSprite && pImageGrid->IsMouseOn(pObj)){
				pSprite->SetScaleX(fScale);
				pSprite->SetScaleY(fScale);
				pSprite->DrawToBack(x, y);
			}
		}
	}
	pObj->SetRenderCallback(GridImageRenderCallback, param1, param2, param3);
}