// Filename	: DlgEquipUp.h
// Creator	: Xu Wenbin
// Date		: 2009/10/16

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIRadioButton.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"

class CECIvtrItem;
class CECIvtrRecipe;
class CECIvtrEquip;
class CECEquipUpInheritProperty;

enum InheritType
{
	INHERIT_PROPERTY_NONE = 0,
		INHERIT_PROPERTY_REFINE,
		INHERIT_PROPERTY_HOLE,
		INHERIT_PROPERTY_STONE,
		INHERIT_PROPERTY_STONEHOLE,
		INHERIT_PROPERTY_ENGRAVE,
		INHERIT_PROPERTY_ADDONS,
};

class CDlgEquipUp : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()	
public:
	
	CDlgEquipUp();
	~CDlgEquipUp();
	
	virtual bool Release();	
	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	
	void SetEquipUp(CECIvtrItem *pOldEquip, CECIvtrRecipe *pRecipe, bool bAdvanced);
	
	void SetInheritableAddons(int num, int* addons);
	CECIvtrItem* GetEquip() { return m_pOldEquip; }
	CECIvtrRecipe* GetRecipe() { return m_pRecipe; }
	
	int m_addonsNum;
	int m_addonsId[5];
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void showDialog();
	virtual bool Tick();
	
	int GetWealth();
	virtual int GetWealthLeft();
	int GetFee();

	bool CanInheritAll(int wealthLeft);
	bool IsAllPropertiesCheckedYes();
	bool IsAnyPropertiesCheckedNo();
	void UpdateInheritAllCheckBox(); 
	bool CanConfirm(char* inheritState = NULL, char* activestatus = NULL);
	void PrepareShowData(CECIvtrEquip *pEquip);

	void UpdateInheritInput(int wealthLeft);
	void AddMovableItem(const AString* itemName, int num);
	void AddResizableItem(const AString* itemName, int num);

	
private:
	enum {FEE_ITEM_ID = 12980};
	enum {CONFIG_ID = 694};
	
protected:
	CECIvtrItem *m_pOldEquip;
	CECIvtrRecipe *m_pRecipe;
	abase::vector<CECEquipUpInheritProperty*> m_pProperties;
	
	PAUILABEL m_pTxt_Name;
	PAUICHECKBOX m_pChk_InheritAll;
	bool m_bInheritAll;
	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;
	PAUILABEL m_pTxt_Num;
	
	abase::vector<PAUIOBJECT> m_pMovableItem;
	abase::vector<PAUIOBJECT> m_pResizableItem;
};	

class CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
public:
	CDlgEquipUp* pDlg;
	
	PAUIRADIOBUTTON m_pRdo_Inherit;
	PAUIRADIOBUTTON m_pRdo_NoInherit;
	PAUILABEL m_pTxt_Num;
	int m_iInheritFee;
	bool m_bActive;
	bool m_bHided;
	int m_iItemTotalHeight;
	
	abase::vector<PAUIOBJECT> m_pRelativeControl;	//界面调整时需要隐藏的控件,包括仅供显示的所有控件
	CECEquipUpInheritProperty(CDlgEquipUp* pDlg);
	virtual ~CECEquipUpInheritProperty(){};
	
	void InitialDialogItem(const AString* itemName, int num);
	virtual void OnInitial();
	virtual void OnShow() = 0;
	virtual int GetInheritFee();
	virtual void UpdateInput(int wealthLeft) = 0;	//实时更新输入界面状态
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip = NULL) = 0;
	virtual void SetInheritData(CECIvtrEquip * pEquip) = 0;
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip) = 0;
	
	virtual bool CanCheckYes(int wealthLeft) = 0;  // 判断能否勾选相应的继承项的“继承（yes)"按钮
	virtual void CheckYes() = 0; // 勾选对应继承项的“继承（yes)”按钮
	virtual bool IsYesChecked(); // 判断继承项的“继承（yes)”按钮是否被选中
	virtual bool IsNoChecked();  // 判断继承项的“不继承（no）”按钮是否被选中
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL) = 0; // 判断相应的继承项是否被勾选（可以勾选“继承（yes)”或“不继承（no）”）
	virtual void CheckAdvancedshow(bool bIsAdvanced);

	void HideAllItem(bool bHide);
	void MoveAllItem(int y = 0);

protected:
	int GetRefineFee(int level);
	int GetWeaponFee(int level, int nHole);
	int GetArmorFee(int level, int nHole);
	int GetStoneFee(int level);
	int GetEngraveFee(int propNum);
	int GetAddonsFee(int propNum);
	int GetScaledFee(int fee);
	int GetEngraveScaledFee(int fee);
	int GetAddonsScaledFee(int fee);
	float GetFeeRate();
	float GetEngraveFeeRate();
	float GetAddonsFeeRate();

	void InvalidateButton(bool bEnable);
	bool IsWealthEnough(int wealthLeft);
private:
	enum {CONFIG_ID = 694};
};

class CECEquipUpInheritRefine : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
public:
	CECEquipUpInheritRefine(CDlgEquipUp* pDlg);
	
	virtual void OnInitial();		//初始化控件，置空
	virtual void OnShow();			//打开对话框时，更新界面数据
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);
	virtual void SetInheritData(CECIvtrEquip * pEquip);		//导入装备属性数据
	virtual void UpdateInput(int wealthLeft);
	
	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	
protected:
	PAUILABEL m_pTxt_EquipRefine;
	int m_iRefineLevel;
};

class CECEquipUpInheritHole : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
	friend class CECEquipUpInheritStoneHole;
public:
	CECEquipUpInheritHole(CDlgEquipUp* pDlg);
	
	virtual void OnInitial();
	virtual void OnShow();
	virtual void SetInheritData(CECIvtrEquip * pEquip);
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);
	
	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void UpdateInput(int wealthLeft);
	
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	
protected:
	PAUILABEL m_pTxt_EquipHole;
	int m_iHoleNum;
};
class CECEquipUpInheritStone : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
	friend class CECEquipUpInheritStoneHole;
public:
	CECEquipUpInheritStone(CDlgEquipUp* pDlg);
	
	virtual void OnInitial();
	virtual void OnShow();
	virtual void SetInheritData(CECIvtrEquip * pEquip);
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);
	
	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void UpdateInput(int wealthLeft);
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	
protected:
	enum {STONE_COUNT = 4};
	PAUITEXTAREA m_pTxt_Stone;
	int m_iHoleNum;
};

class CECEquipUpInheritStoneHole : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
	
public:
	CECEquipUpInheritStoneHole(CDlgEquipUp* pDlg);
	~CECEquipUpInheritStoneHole();
	
	virtual void OnInitial();
	virtual void OnShow();
	virtual void SetInheritData(CECIvtrEquip * pEquip);
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);

	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void UpdateInput(int wealthLeft);
	virtual int GetInheritFee();
	virtual bool IsYesChecked();
	virtual bool IsNoChecked();
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	
	CECEquipUpInheritHole* GetInheritHole() { return m_pInheritHole; }
	CECEquipUpInheritStone* GetInheritStone() { return m_pInheritStone; }
	
protected:
	CECEquipUpInheritHole* m_pInheritHole;
	CECEquipUpInheritStone* m_pInheritStone;
	bool m_bResetStoneButton;
	
};

class CECEquipUpInheritEngrave : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;
	
public:
	CECEquipUpInheritEngrave(CDlgEquipUp* pDlg);
	virtual void OnInitial();
	virtual void OnShow();
	virtual void SetInheritData(CECIvtrEquip * pEquip);
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);
	virtual void UpdateInput(int wealthLeft);

	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	
protected:
	int m_iEngraveNum;
	PAUITEXTAREA m_pTex_Engrave;
};

class CECEquipUpInheritAddons : public CECEquipUpInheritProperty
{
	friend class CDlgEquipUp;	
public:
	CECEquipUpInheritAddons(CDlgEquipUp* pDlg);
	virtual void OnInitial();
	virtual void OnShow();
	virtual void SetInheritData(CECIvtrEquip * pEquip);
	virtual bool CheckForcedInherit(CECIvtrEquip * pEquip);
	virtual void UpdateInput(int wealthLeft);

	virtual bool CanCheckYes(int wealthLeft);
	virtual void CheckYes();
	virtual bool CanInherit(char *inheritState = NULL, char *activestatus = NULL);
	virtual void CalculateInheritFee(CECIvtrEquip* pEquip);
	virtual void CheckAdvancedshow(bool bIsAdvanced);
	
	int GetAddonsNum() { return m_iAddonsNum; }
	
protected:
	int m_iAddonsNum;
	PAUITEXTAREA m_pTxt_Addons;	
	ACString m_sAddonsDesc;
};
