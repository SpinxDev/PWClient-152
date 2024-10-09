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
	
	ImgList								m_ImgList_EngraveConfig;		//	当前NPC上携带的镌刻配置
	PAUIIMAGEPICTURE	m_pImg_EquipToEngrave;			//	待镌刻装备
	ImgList							 m_ImgList_Material;						//	原料显示
	ObjList								m_ObjList_MaterialNum;				//	原料数目显示
	PAUIPROGRESS			m_pPrgs_Engrave;						//	镌刻进度
	PAUIOBJECT					m_pObj_Engrave;							//	开始镌刻控件

	enum State{
		STATE_NULL,				//	未镌刻状态
		STATE_WAIT,				//	已发出镌刻请求
		STATE_ENGRAVE,		//	已收到开始镌刻通知
	};

	State								m_state;						//	当前镌刻状态
	const	ENGRAVE_ESSENCE	*m_pEngrave;	//	当前选中的镌刻
	int										m_iSlot;						//	待镌刻装备在包裹中位置
};
