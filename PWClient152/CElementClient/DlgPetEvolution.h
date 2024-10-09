// File		: DlgPetEvolution.h
// Creator	: Wang Yongdong
// Date		: 2012/11/26

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIProgress.h"
#include <A3DGeometry.h >
#include <AUIComboBox.h >

class CECModel;


class CDlgPetEvolution : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
		
public:
	void DoDataExchange(bool bSave);

	CDlgPetEvolution();
	virtual ~CDlgPetEvolution();
		
	void SetPet(int tid, int index);	
	
	int GetPetIdx() const { return m_iPetIndex;}

	void OnCommand_RebuildInherit(const char * szCommand);
	void OnCommand_AddExp(const char * szCommand);
	void OnCommand_NatrueList(const char* szCommand);
	
	void OnCommand_Evolution(const char * szCommand);
	void OnCommand_RebuildNature(const char * szCommand);
	
	
	void OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void OnCommand_CANCEL(const char * szCommand);

	void EnableAddExp(bool bCan);//ι��

	int GetCurPetId() const { return m_idPet;}

	void OnSendEvolutonCmd(int iSel); // ��������

	void UpdateNature(bool bShow, bool bResetCombo = true); //�����Ը�

protected:
	PAUIIMAGEPICTURE m_pImg_Pet;
	
	PAUIOBJECT m_pButtonPress;
	
	int m_nAngle;
	int	m_idPet;
	int m_iPetIndex;
	CECModel * m_pModel;
	A3DAABB		m_aabb;
	
	DWORD m_dwStartTime;
	DWORD m_dwLastTime;
	int m_iIntervalTime;
	BOOL m_bAdd;

	int m_iAddExpItemNum;

	PAUIPROGRESS m_pExpProg;

	PAUIPROGRESS m_pAtkProg;
	PAUIPROGRESS m_pDefProg;
	PAUIPROGRESS m_pAtkLvlProg;
	PAUIPROGRESS m_pDefLvlProg;
	PAUIPROGRESS m_pHpProg;

	PAUIIMAGEPICTURE m_pImg_Skill[2];

	PAUICOMBOBOX m_pComboWash; // ϴ��ͽ���
	PAUICOMBOBOX m_pComboNature; //�Ը���ѵ

	PAUISTILLIMAGEBUTTON m_pBtnAddExp; // ι��
	PAUISTILLIMAGEBUTTON m_pBtnEvo; // ����
	PAUISTILLIMAGEBUTTON m_pBtnNature; // �Ը���ѵ

	PAUISTILLIMAGEBUTTON	m_pBtn_LeftTurn;
	PAUISTILLIMAGEBUTTON	m_pBtn_RightTurn;

protected:
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	
	bool IsPetLoaded() {return m_pModel != NULL; }	
	
	void UpdateUI();	
	void PrepareUI();

	CECModel *LoadPetModel(int idPet);
	void Reset();
	void UpdateRenderCallback();

	void PropertyAdd(AUIObject *pObj);
	void PropertyMinus(AUIObject *pObj);

	void UpdateEvolution(bool bShow);//
	void UpdateInherit(bool bShow); // ���¼̳���Ϣ

	void ComputeAABB(CECModel* pModel, A3DAABB& aabb);
};
