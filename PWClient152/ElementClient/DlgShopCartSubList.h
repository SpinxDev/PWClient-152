// Filename	: DlgShopCartSubList.h
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#ifndef _ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_
#define _ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_

#include "DlgBase.h"

#include <AUISubDialog.h>

class CECShoppingCart;
class CECObservableChange;
typedef int	ItemShoppingCartID;
class CDlgShopCartSub;

class CDlgShopCartSubList : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgShopCartSubList();
	
	void OnCommand_CANCEL(const char *szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

	void SetShopCart(CECShoppingCart *pShoppingCart);

	//	�ɸ��Ի���ת�������� CECShoppingCartObserver
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);

protected:
	virtual bool OnInitDialog();
	virtual bool Release();
	
	void ClearItems();
	void UpdateItems();
	void Append(ItemShoppingCartID itemCartID);

	CECShoppingCart * m_pShoppingCart;

	PAUISUBDIALOG	m_pSub;					//	SUBDIALOG �ؼ�ģ��
	AString			m_strDialogName;		//	�ӶԻ�������
	AString			m_strDialogFile;		//	�ӶԻ����ļ�

	typedef abase::vector<PAUISUBDIALOG> SubDialogArray;
	SubDialogArray	m_subs;					//	���ΰ���������Ʒ�Ŀؼ��б�
	int				m_nNextSubDialogID;		//	�´�����ӿؼ�ʱʹ�õ��±�

	bool			m_inRelease;			//	��ǰ�����ͷ���

	int				AllocSubDialogID();				//	����һ���µ��ӶԻ���ID�������������Ʒ��
	int				SubDialogCount()const;			//	��ǰ�ӶԻ���ĸ�������Ӧ��Ʒ������
	AString			MakeSubDialogName(int index);	//	��ѯ�����±���ӶԻ�������
	PAUISUBDIALOG	GetSubDialog(int index);		//	��ѯ�����±���ӶԻ���
	PAUISUBDIALOG	AppendSubDialog();				//	����һ��SubDialog
	void			RemoveSubDialog(PAUISUBDIALOG pSub);
	SIZE			CurrentDialogDefaultSize();
	SIZE			SubDialogOrgSize();
	void			FitSize();						//	ʹ��ǰ�Ի����С��������SUBDIALOG
	void			SubDialogResetScroll();			//	�ָ���������Ĭ��λ�ã���Ҫ����λ�õ�����

	CDlgShopCartSub * FindSubDialogFor(ItemShoppingCartID itemCartID, SubDialogArray::iterator &it);
	void			RemoveSubDialogAndAdjust(SubDialogArray::iterator it);
};

#endif	//	_ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_