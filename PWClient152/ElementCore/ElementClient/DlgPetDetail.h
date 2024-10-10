// File		: DlgPetDetail.h
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "hashmap.h"

const short hintAddLoyalty[] = { 100, 100, 90, 80, 60, 30 };
const short hintDecLoyalty[] = { 0, 1, 5, 15, 50, 100 };
const short hintExp[] = { 10, 50, 100, 150 };
const short hintDrop[] = { 100, 60, 30, 10 };
const short hintAttack[] = { 60, 80, 100, 120 };
typedef abase::hash_map<int, int> PetRadarMap;

class CDlgPetDetail : public CDlgBase  
{
public:
	CDlgPetDetail();
	virtual ~CDlgPetDetail();

	void UpdatePet(int nSlot = -1);
	
protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual bool Render();

	static PetRadarMap	m_mapPetRadar;
	PAUILABEL			m_pTxt_Name;
	PAUILABEL			m_pTxt_Level;
	PAUILABEL			m_pTxt_Exp;
	PAUILABEL			m_pTxt_Attack;
	PAUILABEL			m_pTxt_PhyDefence;
	PAUILABEL			m_pTxt_MagDefence;
	PAUILABEL			m_pTxt_MoveSpeed;
	PAUILABEL			m_pTxt_Loyalty;
	PAUILABEL			m_pTxt_Food;
	PAUILABEL			m_pTxt_Hunger;
	PAUILABEL			m_pTxt_RequireLevel;
	PAUILABEL			m_pTxt_HP;
	PAUILABEL			m_pTxt_Definition;
	PAUILABEL			m_pTxt_Evade;
	PAUILABEL			m_pTxt_AtkSpeed;
	PAUILABEL			m_pTxt_Type;
	PAUILABEL			m_pTxt_Color;
	PAUILABEL			m_pTxt_Bind;
	PAUIIMAGEPICTURE	m_pImg_Icon;
	PAUIIMAGEPICTURE	m_pImg_Skill[4];
	PAUIIMAGEPICTURE	m_pImg_PetRadar;
	char				m_nPetRadar[8];
	static A2DSprite *	m_pA2DSpritePetRadarMask;

	int		m_nSlot;

};
