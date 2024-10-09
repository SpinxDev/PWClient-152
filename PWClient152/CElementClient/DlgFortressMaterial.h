// Filename	: DlgFortressMaterial.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CECIvtrItem;
class CDlgFortressMaterial : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressMaterial();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Material(const char *szCommand);

	void OnEventLButtonDown_Img_Material(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void ResetMaterial(int nPackIndex = -1, int count = 0);
	
	static int GetMaterialLimit(int fortressLevel, int iMaterial);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	int	GetMaxMaterialCanAdd(int iMaterial);
	
private:
	
	enum {BUILDING_MATERIAL_COUNT = 5};	//	可以处理的材料总数
	
	PAUIOBJECT			m_pLab_M[BUILDING_MATERIAL_COUNT];	//	各材料数
	PAUIIMAGEPICTURE	m_pImg_Ma;		//	待缴纳材料
};