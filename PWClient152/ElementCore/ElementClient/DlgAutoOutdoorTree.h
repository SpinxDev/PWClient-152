/*
 * FILE: DlgAutoOutdoorTree.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_OUTDOOR_TREE_H_
#define _DLG_AUTO_OUTDOOR_TREE_H_

#include "DlgHomeBase.h"
#include "AutoForest.h"

class AUIListBox;
class AUISlider;
class AUILabel;
class AUIStillImageButton;


class CDlgAutoOutdoorTree: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoOutdoorTree();
	virtual ~CDlgAutoOutdoorTree();

	virtual AUIStillImageButton * GetSwithButton();

	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();

	void SetPolyAndLineBtnPush(bool i_bPushedPoly, bool i_bPushedLine);

	//获取线性树计数
	inline void SetActionCount2(const int i_nActionCount2) { m_nActionCount2 = i_nActionCount2;}
	inline int GetActionCount2() { return m_nActionCount2;}

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void SetSliderBound();
	bool FillTreeSetListBox();
	void SetLabelsText();

	void ShowTreeInfos();

	void OnCommandAddTreeInfo(const char* szCommand);
	void OnCommandDelTreeInfo(const char* szCommand);
	void OnCommandClearAllTreeInfo(const char* szCommand);

	void OnCommandEditTreePoly(const char* szCommand);
	void OnCommandEditTreeLine(const char* szCommand);

	void OnCommandCreateTree(const char* szCommand);
	void OnCommandMoveSlider(const char * szCommand);


	void CreateTreePoly();
	void CreateTreeLine();

	void OnEventDblClkTreeSetList(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventDblClkTreePlantList(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	
protected:
	AUISlider* m_pSliderTreeSeed;
	AUISlider* m_pSliderTreeDensity;
	AUISlider* m_pSliderTreeSlope;
	AUISlider* m_pSliderTreeSpaceX;
	AUISlider* m_pSliderTreeSpaceZ;
	AUISlider* m_pSliderTreeSpaceLine;
	AUISlider* m_pSliderTreeSpaceNoise;
	AUISlider* m_pSliderTreeWeight;


	AUILabel* m_pLabelTreeSeed;
	AUILabel* m_pLabelTreeDensity;
	AUILabel* m_pLabelTreeSlope;
	AUILabel* m_pLabelTreeSpaceX;
	AUILabel* m_pLabelTreeSpaceZ;
	AUILabel* m_pLabelTreeSpaceLine;
	AUILabel* m_pLabelTreeSpaceNoise;
	AUILabel* m_pLabelTreeWeight;

	int m_nTreeSeed;
	int m_nTreeDensity;
	int m_nTreeSlope;
	int m_nTreeSpaceX;
	int m_nTreeSpaceZ;
	int m_nTreeSpaceLine;
	int m_nTreeSpaceNoise;
	int m_nTreeWeight;

	float m_fTreeWeight;

	bool m_bCheckTreeRegular;

	AUIListBox* m_pListBoxTreeSet;
	AUIListBox* m_pListBoxTreePlant;

	CAutoForest::TREEINFO* m_pTreeInfo;
	int m_nNumTreeInfos;

	typedef AArray<PLANTIDWEIGHTINFO, PLANTIDWEIGHTINFO&> TreeInfoArray_t;
	TreeInfoArray_t m_TreeInfoArray;



	AUIStillImageButton* m_pBtnTreePoly;
	AUIStillImageButton* m_pBtnTreeLine;

	AUIStillImageButton* m_pBtnCreate;
	
	enum BtnEnum_e
	{
		B_POLY = 0,
		B_LINE,
		B_NUM
	};

	AUIStillImageButton* m_pBtns[B_NUM];

	int m_nActionCount2; //线性树计数
	
protected:
	bool MakeSureThisInfoNoExiting(const TreeInfoArray_t &i_InfoArray, const PLANTIDWEIGHTINFO &i_Info);
};

#endif