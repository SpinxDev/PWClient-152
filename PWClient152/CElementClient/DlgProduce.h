// Filename	: DlgProduce.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "EC_TimeSafeChecker.h"

#define CDLGPRODUCE_MAX_MATERIALS	8

enum inherit_type
{
	INHERIT_REFINE	= 0x01,
	INHERIT_HOLE	= 0x02,
	INHERIT_STONE	= 0x04,
	INHERIT_ENGRAVE	= 0x08,
	INHERIT_ADDONS	= 0x10,
};

class CDlgProduce : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgProduce();
	virtual ~CDlgProduce();

	void OnCommand_set(const char * szCommand);
	void OnCommand_add(const char * szCommand);
	void OnCommand_minus(const char * szCommand);
	void OnCommand_max(const char * szCommand);
	void OnCommand_begin(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown_U(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetNumber(int nNewNum);
	int GetCurShopSet();
	int GetCurThing();
	bool UpdateProduce(int nSet, int nThing);	
	void ShowNPCPack(int nPack);	
	void BuildButton();

	void ClearMaterial();
	void SetMaterial(int iSrc, int iDst);
	void UpgradeEquipment(char inherit_type);
	bool IsProducing(){return m_pPrgs_1 ? (m_pPrgs_1->GetData() > 0) : false;};
	
	bool CheckProfRestrict();

	// 响应生产成功
	void OnProduceSucc();
	// 响应生产结束
	void OnProduceEnd();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	void OpenAdvancedEquipUp(int ivtrindex, int equipid, int addonsNum, int* addonsIdList);

protected:
	int m_nCurShopSet;
	int m_nCurThing;

	PAUIPROGRESS m_pPrgs_1;
	PAUIOBJECT m_pTxt_Number;
	PAUIOBJECT m_pTxt_Gold;
	PAUIOBJECT m_pTxt_out;
	PAUIIMAGEPICTURE m_pItem_out;
	PAUISTILLIMAGEBUTTON m_pBtn_Produce;
	PAUILABEL m_pTxt_CurrentSkillName;
	PAUILABEL m_pTxt_CurrentSkillLevel;
	PAUIPROGRESS m_pPrgs_Level;
	PAUILABEL m_pTxt_SkillLevel;
	PAUILABEL m_pTxt_SkillName;
	PAUILABEL m_pTxt_Bind;
	PAUISTILLIMAGEBUTTON m_pBtnSet[NUM_NPCIVTR];
	PAUIIMAGEPICTURE m_pImgU[IVTRSIZE_NPCPACK];
	PAUIIMAGEPICTURE m_pImgItem[CDLGPRODUCE_MAX_MATERIALS];
	PAUIOBJECT m_pTxt_no[CDLGPRODUCE_MAX_MATERIALS];
	int m_nSoltMaterail[CDLGPRODUCE_MAX_MATERIALS];
	int m_nRecipeIndexMaterail[CDLGPRODUCE_MAX_MATERIALS];
	ACString m_Confirm;

	int m_iAutoProduceCount;	// 剩余的合成次数
	bool m_bProduceNext;		// 是否合成下一个，合成失败的话会停止剩余的操作

	CECTimeSafeChecker m_produceTimer;

	virtual bool OnInitDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual bool Tick();
};
