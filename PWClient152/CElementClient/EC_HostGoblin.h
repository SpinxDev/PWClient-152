/*
 * FILE: EC_HostGoblin.h
 *
 * DESCRIPTION: Goblin follow the hostplayer
 *
 * CREATED BY: Sunxuewei, 2008/11/06
 *
 */
#ifndef _EC_HOSTGOBLIN_H_
#define _EC_HOSTGOBLIN_H_

#include "A3DTypes.h"
#include "EC_Counter.h"
#include "EC_MsgDef.h"
#include "EC_Goblin.h"
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECModel;
class CECPlayer;
class CECViewport;
class CECInventory;
class CECSkill;
class CECIvtrGoblinEquip;
class CECObject;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostGoblin
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 
//
///////////////////////////////////////////////////////////////////////////
class CECHostGoblin : public CECGoblin
{
public:
	enum
	{
		MAX_SKILLNUM = 8,
	};
private:
	int				m_iSecurityState;	// С���鰲ȫ״̬�� 0: ��ȫ״̬��>0��ת��״̬��Ϊת����ϵ�ʱ��㣬-1:�ɽ���״̬ 
	unsigned int	m_iExp;				// ����ֵ	
	int				m_iLevel;			// �ȼ�	

	int				m_iTotalStatusPt;	// �������������Ե�����
	int				m_iStrength;		// ����
	int				m_iAgility;			// ����
	int				m_iVitality;		// ����
	int				m_iEnergy;			// ����

	int				m_iStrEnhance;		// Buff�������ӵ�����
	int				m_iAgiEnhance;		// Buff�������ӵ�����
	int				m_iVitEnhance;		// Buff�������ӵ�����
	int				m_iEngEnhance;		// Buff�������ӵ�����
	

	int				m_iTotalGeniusPt;	// �����������츳������
	int				m_aGenius[5];		// �ѷ���Ľ�ľˮ����5���츳

	int				m_iVigor;			// ��ǰԪ��ֵ
	int				m_iMaxVigor;		// Ԫ������
	float			m_fVigorGen;		// Ԫ���ظ�

	int				m_iStamina;			// ����ֵ
	int				m_iGrowDegree;		// �ɳ���
	int				m_iMaxRandomPt;		// ��ǰ�ܻ�ȡ������������ֵ

	int				m_iCurrMaxSkillNum;	// ��ǰ�ܹ�ѧϰ�ļ��ܵ�����
	
	CECInventory*	m_pEquipPack;		// װ����

	CECSkill*		m_aSkills[MAX_SKILLNUM];	// �����б�
public:		//	Constructor and Destructor
	
	CECHostGoblin();
	virtual ~CECHostGoblin();

public:		//	Operations
	//	Initialize object
	virtual bool Init(int tid, CECIvtrGoblin* pIvtrGoblin, CECPlayer* pPlayer);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	unsigned int GetExp() const				{ return m_iExp; }
	void SetExp(unsigned int iExp)			{ m_iExp = iExp; }

	void SetLevel(int iLevel)				{ m_iLevel = iLevel; }
	int GetLevel() const					{ return m_iLevel; }		

	//  ���Ե����������
	int GetStatusPt()						{ return m_iTotalStatusPt; }
	int GetStrength()						{ return m_iStrength; }
	int GetVitality()						{ return m_iVitality; }
	int GetAgility()						{ return m_iAgility; }
	int GetEnergy()							{ return m_iEnergy; }

	int GetStrengthEnhance()				{ return m_iStrEnhance; }
	int GetVitalityEnhance()				{ return m_iVitEnhance; }
	int GetAgilityEnhance()					{ return m_iAgiEnhance; }
	int GetEnergyEnhance()					{ return m_iEngEnhance; }

	//  �����������츳������
	int GetGeniusPt()						{ return m_iTotalGeniusPt; }
	//  ��ľˮ�������Ե㣬bAddEquipΪtrue��ʾ����װ����
	int GetGenius(int index, bool bAddEquip = true);

	//  С�������˵������
	const wchar_t* GetBasicDescText();
	//  С���鰲ȫ״̬ 0: ��ȫ״̬��>0��ת��״̬������ת����ϵ�ʱ��㣬-1:�ɽ���״̬ 
	int GetSecurityState()					{ return m_iSecurityState;}

	//  Ԫ��
	int GetAP()								{ return m_iVigor; }
	//  Ԫ������
	int GetMaxAP()							{ return m_iMaxVigor; }
	//  Ԫ���ָ��ٶ�
	float GetAPGen()						{ return m_fVigorGen; }
	//  ����
	int GetStamina()						{ return m_iStamina; }
	//  �ɳ���
	int GetGrowDegree()						{ return m_iGrowDegree; }

	//  ����ʱ���������ٶ�
	float GetMPCost();

	//  ��ǰ��ȡ��������Ե�
	int GetRandomStatusPt()					{ return m_iTotalStatusPt - (m_iLevel - 1); }
	//  ��ǰ�ܻ�ȡ��������Ե�
	int GetMaxRandomStatusPt()				{ return m_iMaxRandomPt; }

	//  ��ȡװ����
	CECInventory* GetEquipment()			{ return m_pEquipPack; }

	//  ��ȡ����
	CECSkill* GetSkill(int index)			{ ASSERT(index >=0 && index < GetSkillNum()); return m_aSkills[index]; }
	//	��ȡ��ѧϰ�ļ�������
	int GetSkillNum();
	//  ��ȡ��ǰ�ܹ�ѧϰ�ļ��ܵ��������
	int GetCurrMaxSkillNum()				{ return m_iCurrMaxSkillNum; }
	
	//	��ȡ��idָ���ļ���
	CECSkill* GetSkillByID(int id);	
	
	//  �Ƿ���Խ��ף�
	bool CanTrade()							{ return (m_iSecurityState<0) ? true : false; }

	//	�ж�С�����Ƿ�����ͷ�ָ���ļ���
	//  0:�ɹ�            1:ְҵ��ƥ��        2:mp����
	//  3:AP����		  4:�츳�㲻��		  5:����ID
	//  6:δѡ��Ŀ��      7:��С���鼼��      8:С����ȼ�����
	int CheckSkillCastCondition(CECSkill* pSkill);

	//  ����װ��
	bool DestroyEquip(int iSlot);

	//  ����С����ʵ�ʻ�ȡ�ľ���ֵ�� iExpΪ����ľ���ֵ
	//  iLevelΪ������ĵȼ��� > 0 ��˵���Ӿ�����ע�룬Ϊ0��Ϊ�����Ǿ���ֵ����
	unsigned int InjectExp(unsigned int iExp, int iLevel = 0);
	
	//  ����ʹС���龭������ָ��ֵ����ĵȼ�ΪiLevel�ľ������еľ���ֵ
	unsigned int NeedPillExp(unsigned int goblin_exp, int iLevel);

	//  ����ʹС���龭������ָ��ֵ��������Ǿ���ֵ
	unsigned int NeedPlayerExp(unsigned int goblin_exp);
		
	//  ����С�������Ե������Ҫ�ĵ���
	int NeedStatusPt(int iBase, int iNum);

	//  С���鵱ǰδʹ�õ����Ե���
	int UnusedStatusPt();	
	
	//  С���鵱ǰδʹ�õ��츳����
	int UnusedGeniusPt();

	//	С�����ͷż���
	//  iIndexΪС������ѧ��ļ�������
	bool CastSkill(int iIndex, int idTarget, bool bForceAttack);

	//  ��Ϣ����
	void OnMsgGoblinVigor(const ECMSG& Msg);
	void OnMsgGoblinEnhance(const ECMSG& Msg);
	void OnMsgGoblinStamina(const ECMSG& Msg);
	void OnMsgGoblinExp(const ECMSG& Msg);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif//_EC_HOSTGOBLIN_H_