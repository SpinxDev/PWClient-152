// Filename	: DlgShopCartSubList.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#include "DlgShopCartSubList.h"
#include "DlgShopCartSub.h"
#include "EC_GameUIMan.h"

#include "EC_ShoppingCart.h"

#include <AUIDef.h>

AUI_BEGIN_COMMAND_MAP(CDlgShopCartSubList, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgShopCartSubList, CDlgBase)
AUI_ON_EVENT("*",		WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_END_EVENT_MAP()

CDlgShopCartSubList::CDlgShopCartSubList()
: m_pShoppingCart(NULL)
, m_pSub(NULL)
, m_nNextSubDialogID(0)
, m_inRelease(false)
{
}

bool CDlgShopCartSubList::OnInitDialog()
{
	if (CDlgBase::OnInitDialog()){
		//	��ȡģ�� SUBDIALOG ��Ӧ�ļ�������ӶԻ�������
		DDX_Control("Sub_ShopCart", m_pSub);
		m_pSub->Show(false);
		m_strDialogFile = m_pSub->GetSubDialog()->GetFilename();
		m_pSub->SetDialog(NULL);
		return true;
	}
	return false;
}

bool CDlgShopCartSubList::Release(){
	m_inRelease = true;
	if (m_pShoppingCart){
		SetShopCart(NULL);
	}
	return CDlgBase::Release();
}

void CDlgShopCartSubList::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgShopCartSubList::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}

void CDlgShopCartSubList::SetShopCart(CECShoppingCart *pShoppingCart)
{
	m_pShoppingCart = pShoppingCart;
	UpdateItems();
}

CDlgShopCartSub * CDlgShopCartSubList::FindSubDialogFor(ItemShoppingCartID itemCartID, SubDialogArray::iterator &it)
{
	for (it = m_subs.begin(); it != m_subs.end(); ++ it){
		PAUISUBDIALOG pSub = *it;
		if (CDlgShopCartSub *pDlgCartSub = dynamic_cast<CDlgShopCartSub *>(pSub->GetSubDialog())){
			if (pDlgCartSub->ItemCartID() == itemCartID){
				return pDlgCartSub;
			}
		}
	}
	return NULL;
}

void CDlgShopCartSubList::OnModelChange(const CECShoppingCart *p, const CECObservableChange *q){
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	const CECShoppingCartChange *pChange = dynamic_cast<const CECShoppingCartChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	switch (pChange->GetChangeMask()){
	case CECShoppingCartChange::CART_CLEARED:
		ClearItems();
		break;
	case CECShoppingCartChange::CART_LOCKED:
	case CECShoppingCartChange::CART_UNLOCKED:
		{
			for (SubDialogArray::iterator it = m_subs.begin(); it != m_subs.end(); ++ it){
				PAUISUBDIALOG pSub = *it;
				if (CDlgShopCartSub *pDlgCartSub = dynamic_cast<CDlgShopCartSub *>(pSub->GetSubDialog())){
					pDlgCartSub->UpdateLock();
				}
			}
		}
		break;
	case CECShoppingCartChange::ITEM_COUNT_CHANGE:
		if (pChange->Item()){
			ItemShoppingCartID itemCartID = pChange->Item()->ItemCartID();
			SubDialogArray::iterator it;
			if (CDlgShopCartSub *pDlgCartSub = FindSubDialogFor(itemCartID, it)){
				pDlgCartSub->OnCountChange();
			}
		}
		break;
	case CECShoppingCartChange::ITEM_REMOVED:
		if (pChange->Item()){
			ItemShoppingCartID itemCartID = pChange->Item()->ItemCartID();			
			SubDialogArray::iterator it;
			if (CDlgShopCartSub *pDlgCartSub = FindSubDialogFor(itemCartID, it)){
				RemoveSubDialogAndAdjust(it);
			}
		}
		break;
	case CECShoppingCartChange::ITEM_ADDED:
		if (pChange->Item()){
			SubDialogResetScroll();
			Append(pChange->Item()->ItemCartID());
			GetParentDlgControl()->SetVBarPos(GetParentDlgControl()->GetVBarMax());
		}
		break;
	case CECShoppingCartChange::MALL_ITEMS_CHANGED:
		UpdateItems();
		break;
	}
}

void CDlgShopCartSubList::SubDialogResetScroll(){
	PAUISUBDIALOG pParent = GetParentDlgControl();
	pParent->SetHBarPos(0);
	pParent->SetVBarPos(0);
}

void CDlgShopCartSubList::UpdateItems(){
	if (m_inRelease){
		return;
	}
	ClearItems();
	if (m_pShoppingCart){
		for (int i(0); i < m_pShoppingCart->Count(); ++ i){
			const CECShoppingCartItem *pCartItem = m_pShoppingCart->ItemAt(i);
			Append(pCartItem->ItemCartID());
		}
	}
}

void CDlgShopCartSubList::ClearItems()
{	
	//	���¸��ؼ��Ĺ���λ�õ���ʼλ��
	//	���ڻָ��� m_pSub Ϊ����ģ�嵽��ʼλ��
	//	����� m_pSub ���Ƶ����� subdialog ��ʼλ������Ӱ��
	//	���� FitSize ����õ�ǰ�Ի���ĸ� subdialog �� SetSubDialogOrgSize ������ù�����λ�õ�0��bReset����Ĭ��Ϊtrue��
	//
	PAUISUBDIALOG pParent = GetParentDlgControl();
	pParent->SetHBarPos(0);
	pParent->SetVBarPos(0);

	//	������� SUBDIALOG
	for (SubDialogArray::iterator it = m_subs.begin(); it != m_subs.end(); ++ it){
		RemoveSubDialog(*it);
	}
	if (!m_subs.empty()){
		m_subs.clear();
		FitSize();
	}
}

void CDlgShopCartSubList::RemoveSubDialog(PAUISUBDIALOG pSub){
	//	�� AppendSubDialog ���Գƣ����������飬�������Ű棬������Ͳ�ɾ��
	if (pSub){
		PAUIDIALOG pDlg = pSub->GetSubDialog();
		DeleteControl(pSub);
		if (CDlgShopCartSub *pDlgCartSub = dynamic_cast<CDlgShopCartSub *>(pDlg)){
			pDlgCartSub->SetItem(NULL, INVALID_ITEM_SHOPPINGCART_ID);
			pDlg->SetDestroyFlag(true);		//	�����ǳ��򵥶����ضԻ����ļ��������Ҫ�ֶ�����ɾ��ѡ��
		}
	}
}

void CDlgShopCartSubList::RemoveSubDialogAndAdjust(SubDialogArray::iterator it){
	if (it == m_subs.end()){
		ASSERT(false);
		return;
	}
	int nVBarPosition = GetParentDlgControl()->GetVBarPos();
	UpdateItems();
	GetParentDlgControl()->SetVBarPos(nVBarPosition);
}

AString CDlgShopCartSubList::MakeSubDialogName(int index)
{
	AString strName;
	strName.Format("%s_%d", m_pSub->GetName(), index);
	return strName;
}

PAUISUBDIALOG CDlgShopCartSubList::GetSubDialog(int index)
{
	AString strName = MakeSubDialogName(index);
	PAUIOBJECT pObj = GetDlgItem(strName);
	return (PAUISUBDIALOG)pObj;
}

int CDlgShopCartSubList::SubDialogCount()const{
	return (int)m_subs.size();
}

SIZE CDlgShopCartSubList::CurrentDialogDefaultSize(){
	SIZE result = GetDefaultSize();
	result.cy = max(1, SubDialogCount()) * m_pSub->GetDefaultSize().cy;
	return result;
}

SIZE CDlgShopCartSubList::SubDialogOrgSize(){
	SIZE result;
	SIZE s = CurrentDialogDefaultSize();
	result.cx = (int)(s.cx * m_pAUIManager->GetWindowScale() + 0.5f);
	result.cy = (int)(s.cy * m_pAUIManager->GetWindowScale() + 0.5f);
	return result;
}

void CDlgShopCartSubList::FitSize()
{
	SIZE s = CurrentDialogDefaultSize();
	SetDefaultSize(s.cx, s.cy);

	SIZE s2 = SubDialogOrgSize();
	GetParentDlgControl()->SetSubDialogOrgSize(s2.cx, s2.cy);
}

int CDlgShopCartSubList::AllocSubDialogID()
{
	return m_nNextSubDialogID ++;
}

PAUISUBDIALOG CDlgShopCartSubList::AppendSubDialog()
{
	//	�����·�����µ� SUBDIALOG
	//

	//	��ģ���¡�� SUBDIALOG
	PAUISUBDIALOG pSub = (PAUISUBDIALOG)CloneControl(m_pSub);
	pSub->Show(true);

	AString strName;
	
	//	���� SUBDIALOG ����
	int subDialogID = AllocSubDialogID();
	strName = MakeSubDialogName(subDialogID);
	pSub->SetName((char *)(const char *)strName);

	//	����λ��
	SIZE s = m_pSub->GetDefaultSize();
	pSub->SetDefaultPos(0, SubDialogCount()*s.cy);
	
	//	���� SUBDIALOG ��ضԻ���
	PAUIDIALOG pDlg = NULL;
	if (m_strDialogName.IsEmpty()){
		//	�״δ�������¼ģ������
		pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
		m_strDialogName = pDlg->GetName();
		strName.Format("%s_%d", m_strDialogName, subDialogID);
		pDlg->SetName(strName);
	}else{
		strName.Format("%s_%d", m_strDialogName, subDialogID);
		pDlg = GetGameUIMan()->GetDialog(strName);
		if (!pDlg){
			//	�Ի��򲻴��ڣ�����
			pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
			pDlg->SetName(strName);
		}else{
			ASSERT(pDlg->GetDestroyFlag() == true);			
			//	Ϊ������ɾ���Ի��򡢻ָ�֮
			if (pDlg->GetDestroyFlag()){
				pDlg->SetDestroyFlag(false);
				pDlg->Show(true);
			}
			//	��ʼ��
			CDlgShopCartSub *pDlgCartSub = dynamic_cast<CDlgShopCartSub *>(pDlg);
			pDlgCartSub->SetItem(NULL, INVALID_ITEM_SHOPPINGCART_ID);
		}
	}

	//	�´����ĶԻ��򣬶���Ҫ���ݵ�ǰ�������Ž��е���
	pDlg->Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());

	//	������ȷ�ĶԻ���
	pSub->SetDialog(pDlg);

	//	�����ӶԻ���������Ի����С
	m_subs.push_back(pSub);
	FitSize();

	return pSub;
}

void CDlgShopCartSubList::Append(ItemShoppingCartID itemCartID)
{
	PAUISUBDIALOG pSubDialog = AppendSubDialog();
	CDlgShopCartSub *pShopCartSub = dynamic_cast<CDlgShopCartSub *>(pSubDialog->GetSubDialog());
	pShopCartSub->SetItem(m_pShoppingCart, itemCartID);
}
