/*
 * FILE: DlgAutoAllRes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/4/6
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_ALL_RES_H_
#define _DLG_AUTO_ALL_RES_H_

#include "DlgHomeBase.h"
#include "AUITreeView.h"


class CDlgAutoAllRes : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	enum AllResTreeItem_e
	{
		ARTI_TERRAIN = 0,
		ARTI_MOUNTAIN_POLY,
		ARTI_MOUNTAIN_LINE,
		ARTI_ROAD,
		ARTI_WATER,
		ARTI_LOCALTEX,

		ARTI_TREE_POLY,
		ARTI_TREE_LINE,
		ARTI_GRASS,
		ARTI_BUILDING,
		ARTI_OTHEROBJECT,

		ARTI_FURNITURE,
		ARTI_PLANT,
		ARTI_TOY,
		ARTI_VIRTU,
		ARTI_NUM
	};

	enum Btn_e 
	{
		B_MODIFY = 0,
		B_CANCEL,
		B_DEL,
		B_NUM
	};
	
	CDlgAutoAllRes();
	virtual ~CDlgAutoAllRes();
	
	void InsertResTreeChildItem(AllResTreeItem_e i_Parent, ACHAR* i_szName, int i_nID = 0);

	void DelResTreeLastChildItem(AllResTreeItem_e i_Parent);

	int GetTreeViewSelItemIdx() const;
	
	void SetBtnsEnable(bool i_bModify, bool i_bCancel, bool i_bDel);
	void OnCommandCancelRes(const char* szCommand);

	void ReLoadSceneRes();

protected:
	virtual void OnShowDialog();
	
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void InitVars();
	

	void OnCommandModifyRes(const char* szCommand);
	void OnCommandDelRes(const char* szCommand);
	
	

	void OnEventResTree(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	bool DelResAction( const P_AUITREEVIEW_ITEM i_pItem) const;

	int GetTreeViewItemIdx(const P_AUITREEVIEW_ITEM i_pItem) const;

	void SetCurSelAreaViewPoints(P_AUITREEVIEW_ITEM i_pItem);

	void TurnToMountainPolyMode();
	void TurnToMountainLineMode();
	void TurnToRoadMode();
	void TurnToLocalTexMode();
	void TurnToTreePolyMode();
	void TurnToTreeLineMode();
	void TurnToGrassMode();

	void SetModelOperation(P_AUITREEVIEW_ITEM i_pItem);
	void TurnToBuildingMode();
	void TurnToOtherObjectMode();
	void TurnToFurnitureMode();
	void TurnToPlantMode();
	void TurnToToyMode();
	void TurnToVirtuMode();

	void EditAreaToViewArea();
	void ViewAreaToEditArea(bool i_bLine);

	
	int GetParentItemID(P_AUITREEVIEW_ITEM i_pItem);

	void MakeSureTexOpEnable(void); //确认纹理操作是否可用
	
	void Clear();

	//与
	struct ResCount_t
	{
		ResCount_t()
			:nCntMountainPoly(1),
			nCntMountainLine(1),
			nCntRoad(1),
			nCntLocalTex(1),
			nCntTreePoly(1),
			nCntTreeLine(1),
			nCntGrass(1),
			nCntBuilding(1),
			nCntOthObj(1),
			nCntFurniture(1),
			nCntPlant(1),
			nCntToy(1),
			nCntVirtu(1)
		{}

		int nCntMountainPoly;
		int nCntMountainLine;
		int nCntRoad;
		int nCntLocalTex;

		int nCntTreePoly;
		int nCntTreeLine;
		int nCntGrass;

		int nCntBuilding;
		int nCntOthObj;
		int nCntFurniture;
		int nCntPlant;
		int nCntToy;
		int nCntVirtu;
	};

	void InsertModelItem(unsigned int i_uModelID, unsigned int i_uType, ResCount_t &i_ResCount);
	void SetOperationCount(const ResCount_t &i_ResCount);

	void InsertTerrainItem(AllResTreeItem_e i_itemType, ResCount_t &i_ResCount);

protected:
	AUITreeView* m_pTreeAllRes;
	P_AUITREEVIEW_ITEM m_pTreeParentItem[ARTI_NUM];
	class AUIStillImageButton* m_pBtn[B_NUM];
	P_AUITREEVIEW_ITEM m_pCurModifyingItem;


};

#endif //_DLG_AUTO_ALL_RES_H_