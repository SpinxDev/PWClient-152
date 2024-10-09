// File		: DlgGeneralCardRespawn.h  
// Creator	: Wang
// Date		: 2013/09/13

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgGenCardRespawn : public CDlgBase  
{
	friend class CECGameUIMan;
	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
	
public:
	CDlgGenCardRespawn();
	virtual ~CDlgGenCardRespawn();
	
	void OnCommandCANCEL(const char * szCommand);
	void OnCommandRespwan(const char * szCommand);
	void OnEventLButtonDownMajor(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDownMinor(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void InstallMajor(CECIvtrItem* pItem,int iSrc);
	void InstallMinor(CECIvtrItem* pItem,int iSrc);
	void SetTarget();
	void ClearCard(int mask);
	bool IsMajorInstalled();
	bool CanMajorInstall(CECIvtrItem* pItem);
	bool CanMinorInstall(CECIvtrItem* pItem);
	CECIvtrItem* GenerateTargetCart(CECIvtrItem* pMajor,CECIvtrItem* pMinor);
	CECIvtrItem* GetMajorCard();
	void DoRespawn();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	
protected:
	PAUIIMAGEPICTURE m_pImg_Major;
	PAUIIMAGEPICTURE m_pImg_Minor;
//	PAUIIMAGEPICTURE m_pImg_Target;

	PAUISTILLIMAGEBUTTON m_pBtnRespawn;

	int m_MajorIndex;
	int m_MinorIndex;
};
