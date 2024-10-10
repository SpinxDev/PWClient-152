// Filename	: DlgEngrave.h
// Creator	: Xu Wenbin
// Date		: 2011/4/28

#pragma once

#include "DlgBase.h"
#include <vector.h>
#include <AUIProgress.h>
#include <AUIImagePicture.h>

struct ENGRAVE_ESSENCE;

namespace S2C{
	struct cmd_engrave_start;
	struct cmd_engrave_result;
}

class CECIvtrItem;
class CDlgEngrave : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgEngrave();

	void OnCommand_Engrave(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown_Img_EngraveConfig(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Img_Equip(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	bool SetEquip(int iSlot);
	void StartEngrave(const S2C::cmd_engrave_start *pCmd);
	void EndEngrave();
	void EngraveResult(const S2C::cmd_engrave_result *pCmd);

	bool IsEngraving()const{ return m_state != STATE_NULL; }

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void SetEngraveService(int id_engrave_service);
	void ChooseEngrave(unsigned int id_engrave, bool bForceChoose = false);
	bool CanEngrave();

	void Lock(bool bLock = true);

private:
	typedef abase::vector<PAUIIMAGEPICTURE>	ImgList;
	typedef abase::vector<PAUIOBJECT>	ObjList;
	
	ImgList								m_ImgList_EngraveConfig;		//	��ǰNPC��Я�����Կ�����
	PAUIIMAGEPICTURE	m_pImg_EquipToEngrave;			//	���Կ�װ��
	ImgList							 m_ImgList_Material;						//	ԭ����ʾ
	ObjList								m_ObjList_MaterialNum;				//	ԭ����Ŀ��ʾ
	PAUIPROGRESS			m_pPrgs_Engrave;						//	�Կ̽���
	PAUIOBJECT					m_pObj_Engrave;							//	��ʼ�Կ̿ؼ�

	enum State{
		STATE_NULL,				//	δ�Կ�״̬
		STATE_WAIT,				//	�ѷ����Կ�����
		STATE_ENGRAVE,		//	���յ���ʼ�Կ�֪ͨ
	};

	State								m_state;						//	��ǰ�Կ�״̬
	const	ENGRAVE_ESSENCE	*m_pEngrave;	//	��ǰѡ�е��Կ�
	int										m_iSlot;						//	���Կ�װ���ڰ�����λ��
};
