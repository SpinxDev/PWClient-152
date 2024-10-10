#pragma once

#include <ABaseDef.h>
#include <ATime.h>
#include "ECScriptCheckBase.h"
#include "ECScriptCheckState.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

#include <queue>

// �жϽű��Ƿ�ִ�й�
// nScriptID	�ű�ID
class CECScriptCheckScriptExecuted : public CECScriptCheckBase
{
private:
	int m_nScriptID;
public:
	CECScriptCheckScriptExecuted()
	{
		AddParam(m_nScriptID);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetScriptExecuted(m_nScriptID);
	}
};

// �ж��û��Ƿ�ѡ����ʾ��һ��
class CECScriptCheckDisplayNext : public CECScriptCheckBase
{
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		bool bNext = pState->GetDisplayNext();
		if( bNext )
			pState->SetDisplayNext(false);
		return bNext;
	}
};

// �ж���ҵȼ�
// nMinLevel ��͵ȼ�
// nMaxLevel ��ߵȼ�
class CECScriptCheckPlayerLevelBetween : public CECScriptCheckBase
{
private:
	int m_nMinLevel;
	int m_nMaxLevel;
public:
	CECScriptCheckPlayerLevelBetween()
	{
		AddParam(m_nMinLevel);
		AddParam(m_nMaxLevel);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int nLevel = pState->GetPlayerLevel();
		if (-1 == m_nMaxLevel)
			return nLevel >= m_nMinLevel;
		else
			return nLevel >= m_nMinLevel && nLevel <= m_nMaxLevel;
	}
};

// �ж����ְҵ
// szProfression ְҵ����
class CECScriptCheckPlayerProfession : public CECScriptCheckBase
{
private:
	AWString m_szProfression;
public:
	CECScriptCheckPlayerProfession()
	{
		AddParam(m_szProfression);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int nProfessionID = pState->GetPlayerProfessionID();
		AWString strProfessionName = g_pGame->GetGameRun()->GetProfName(nProfessionID);
		return m_szProfression == strProfessionName;
	}
};

// �ж���ҵ�����ֵ
// nMinHPPercent ��Ͱٷֱ�
// nMaxHPPercent ��߰ٷֱ�
class CECScriptCheckPlayerHPPercentBetween : public CECScriptCheckBase
{
private:
	float m_nMinHPPercent;
	float m_nMaxHPPercent;
public:
	CECScriptCheckPlayerHPPercentBetween()
	{
		AddParam(m_nMinHPPercent);
		AddParam(m_nMaxHPPercent);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int nCurHP = pState->GetCurrentHP();
		int nMaxHP = pState->GetMaxHP();
		float fHPPercent = (float)nCurHP / nMaxHP;

		return m_nMinHPPercent<=fHPPercent && fHPPercent<=m_nMaxHPPercent;
	}
};

// �ж���ҵ�ħ��ֵ
// nMinMPPercent ��Ͱٷֱ�
// nMaxMPPercent ��߰ٷֱ�
class CECScriptCheckPlayerMPPercentBetween : public CECScriptCheckBase
{
private:
	float m_nMinMPPercent;
	float m_nMaxMPPercent;
public:
	CECScriptCheckPlayerMPPercentBetween()
	{
		AddParam(m_nMinMPPercent);
		AddParam(m_nMaxMPPercent);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{

		int nCurMP = pState->GetCurrentMP();
		int nMaxMP = pState->GetMaxMP();
		float fMPPercent = (float)nCurMP / nMaxMP;

		return m_nMinMPPercent<=fMPPercent && fMPPercent<=m_nMaxMPPercent;
	}
};

// ����ض������Ƿ�����ʾ״̬
// szDialogName ���Ĵ�������
class CECScriptCheckDialogIsVisible : public CECScriptCheckBase
{
private:
	AString m_szDialogName;
public:
	CECScriptCheckDialogIsVisible()
	{
		AddParam(m_szDialogName);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetDialogVisibleState(m_szDialogName);
	}
};

// ��鴰����ĳ���ؼ��ĵ�ǰ��ʾ�����Ƿ�Ϊ�ض����֣����ڱ�������ʾ״̬
// szDialogName ��������
// szItemName	�ؼ�����
// szItemText	�ؼ���ʾ����
class CECScriptCheckDialogItemText : public CECScriptCheckBase
{
private:
	AString m_szDialogName;
	AString m_szItemName;
	AWString m_szItemText;
public:
	CECScriptCheckDialogItemText()
	{
		AddParam(m_szDialogName);
		AddParam(m_szItemName);
		AddParam(m_szItemText);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return m_szItemText == 
			pState->GetDialogItemText(m_szDialogName, m_szItemName);
	}
};

// ��鴰����ĳ����ѡ����߸�ѡ���Ƿ�ѡ��
// szDialogName ��������
// szItemName	�ؼ�����
class CECScriptCheckDialogItemIsChecked : public CECScriptCheckBase
{
private:
	AString m_szDialogName;
	AString m_szItemName;
public:
	CECScriptCheckDialogItemIsChecked()
	{
		AddParam(m_szDialogName);
		AddParam(m_szItemName);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetDialogItemIsChecked(m_szDialogName, m_szItemName);
	}
};

// �������Ƿ�ɱ����һֻ����
class CECScriptCheckKillMonster: public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetKillMonster();
	}
};

// �������Ƿ�װ����������
class CECScriptCheckEquipNewWeapon: public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetEquipNewWeapon();
	}
};

// ��鵱ǰ���з����NPC
// nNPCID		NPCID
class CECScriptCheckServiceNPC : public CECScriptCheckBase
{
private:
	int	m_nNPCID;
public:
	CECScriptCheckServiceNPC()
	{
		AddParam(m_nNPCID);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int idServiceNPC = pState->GetServiceNPCID();
		return idServiceNPC == m_nNPCID;
	}
};

// ��鵱ǰѡ�е�Ŀ��NPC
// szTargetName		NPC���֣�""��ʾ������Ҫ��
class CECScriptCheckTargetNPC : public CECScriptCheckBase
{
private:
	AWString	m_szTargetName;
public:
	CECScriptCheckTargetNPC()
	{
		AddParam(m_szTargetName);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int idSelObj = pState->GetSelectedTargetID();
		if (pState->GetIsNPCID(idSelObj))
		{
			if (!pState->GetIsNPCExists(idSelObj) ||
				pState->GetIsMonsterNPC(idSelObj))
				return false;
	
			return	m_szTargetName==L"" || 
					m_szTargetName == pState->GetNPCName(idSelObj);
		}
		return false;
	}
};

// ��鵱ǰ�Ƿ�ѡ�����
class CECScriptCheckTargetPlayer : public CECScriptCheckBase
{
public:
	CECScriptCheckTargetPlayer()
	{
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int idSelObj = pState->GetSelectedTargetID();
		return pState->GetIsPlayerID(idSelObj);
	}
};

// ��鵱ǰѡ�е�Ŀ�����
// szTargetName		Ŀ��������֣�""��ʾ������Ҫ��
// szTargetAttrib	Ŀ���������ԣ��硰����ͻ�����ȣ�""��ʾ��Ҫ��,"*"��ʾ��������������
class CECScriptCheckTargetMonster : public CECScriptCheckBase
{
private:
	AWString	m_szTargetName;
	AWString	m_szTargetAttrib;
public:
	CECScriptCheckTargetMonster()
	{
		AddParam(m_szTargetName);
		AddParam(m_szTargetAttrib);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int idSelObj = pState->GetSelectedTargetID();
		if (pState->GetIsNPCID(idSelObj))
		{
			if (!pState->GetIsNPCExists(idSelObj) ||
				!pState->GetIsMonsterNPC(idSelObj))
				return false;
			
			if( m_szTargetName == L"" && m_szTargetAttrib == L"")
				return true;
			
			AWString szName = pState->GetNPCName(idSelObj);
			if( m_szTargetName != L"" && m_szTargetName != szName )
				return false;
			
			if( m_szTargetAttrib == L"" )
				return true;
			
			int nPropID = pState->GetNPCRandomProp(idSelObj);
			AWString strPropName = pState->GetStringFromGameUIST(330 + nPropID);
			if( m_szTargetAttrib == L"*" )
				return strPropName != L"";
			else
				return m_szTargetAttrib == strPropName;
		}
		return false;
	}
};

// �ж�������ڵĵ�ͼ��������
// szAddressName	��ͼ��������
class CECScriptCheckAddress : public CECScriptCheckBase
{
private:
	AWString m_szAddressName;
public:
	CECScriptCheckAddress()
	{
		AddParam(m_szAddressName);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetIsCurrentPrecinctExists() &&
			m_szAddressName == pState->GetCurrentPrecinctName();
	}
};


// �ж�������ڵĵ�ͼid
// nMapID	��ͼinstance id
// 2012/8/10 zhougaomin01305
class CECScriptCheckMapID : public CECScriptCheckBase
{
private:
	int	m_nMapID;
public:
	CECScriptCheckMapID()
	{
		AddParam(m_nMapID);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return m_nMapID == pState->GetMapID();
	}
};


// �������״̬
// nTaskID		����ID
// nTaskStatus	�������״̬��0--�����,1--�ѽ���,2--�ﵽɱ�ֻ�������ƷҪ��
class CECScriptCheckTaskStatus : public CECScriptCheckBase
{
private:
	int	m_nTaskID;
	int	m_nTaskStatus;
	AString m_strExcludedID;

public:
	CECScriptCheckTaskStatus()
	{
		AddParam(m_nTaskID);
		AddParam(m_nTaskStatus);
		AddParam(m_strExcludedID);
	}

	void ParseExcludedID(abase::vector<int> & vecExcludedID) const
	{
		int nLength = m_strExcludedID.GetLength();
		if (nLength == 0)
			return;

		vecExcludedID.clear();
		AString strTemp = m_strExcludedID + ":";
		++nLength;
		int nBegin = 0;
		for (int i = 0; i < nLength; i++)
		{
			if (strTemp[i] == ':')
			{
				AString strID = strTemp.Mid(nBegin, i - nBegin);
				vecExcludedID.push_back(strID.ToInt());
				nBegin = i + 1;
			}
		}
	}

	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		abase::vector<int> vecExcludedID;
		ParseExcludedID(vecExcludedID);

		switch(m_nTaskStatus)
		{
		case 0:
			return pState->GetIsTaskFinished(m_nTaskID, vecExcludedID);
		case 1:
			return pState->GetIsTaskTaked(m_nTaskID, vecExcludedID);
		case 2:
			return pState->GetCanFinishTask(m_nTaskID, vecExcludedID);
		}

		ASSERT(!"invalid task status");
		return false;
	}
};

// �����������е���Ʒ
// nItemID		��ƷID
// nMinNumber	��Ʒ�������
class CECScriptCheckTaskInventoryItemCount : public CECScriptCheckBase
{
private:
	int	m_nItemID;
	int	m_nMinNumber;
public:
	CECScriptCheckTaskInventoryItemCount()
	{
		AddParam(m_nItemID);
		AddParam(m_nMinNumber);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetTaskInventoryItemCount(m_nItemID) >= m_nMinNumber;
	}
};

// �������е���Ʒ
// nItemID		��ƷID
// nMinNumber	��Ʒ�������
class CECScriptCheckInventoryItemCount : public CECScriptCheckBase
{
private:
	int	m_nItemID;
	int	m_nMinNumber;
public:
	CECScriptCheckInventoryItemCount()
	{
		AddParam(m_nItemID);
		AddParam(m_nMinNumber);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetInventoryItemCount(m_nItemID) >= m_nMinNumber;
	}
};

// �����Ҽ��ܵȼ�
// nSkillID			����ID
// nSkillLevelMin	������͵ȼ�
// nSkillLevelMax	������ߵȼ�
class CECScriptCheckSkillLevelBetween : public CECScriptCheckBase
{
private:
	int	m_nSkillID;
	int	m_nSkillLevelMin;
	int	m_nSkillLevelMax;
public:
	CECScriptCheckSkillLevelBetween()
	{
		AddParam(m_nSkillID);
		AddParam(m_nSkillLevelMin);
		AddParam(m_nSkillLevelMax);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int nSkillLevel = pState->GetSkillLevel(m_nSkillID);
		if (m_nSkillLevelMax == -1)
			return nSkillLevel >= m_nSkillLevelMin;
		else
			return nSkillLevel >= m_nSkillLevelMin && nSkillLevel <= m_nSkillLevelMax;
	}
};

// �����Ҽ���������
// nSkillID			����ID
// nSkillAbilityMin	�������������
// nSkillAbilityMax	�������������
class CECScriptCheckSkillAbilityBetween : public CECScriptCheckBase
{
private:
	int	m_nSkillID;
	int	m_nSkillAbilityMin;
	int	m_nSkillAbilityMax;
public:
	CECScriptCheckSkillAbilityBetween()
	{
		AddParam(m_nSkillID);
		AddParam(m_nSkillAbilityMin);
		AddParam(m_nSkillAbilityMax);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		int nSkillAbility = pState->GetSkillAbility(m_nSkillID);
		if (m_nSkillAbilityMax == -1)
			return nSkillAbility >= m_nSkillAbilityMin;
		else
			return nSkillAbility >= m_nSkillAbilityMin && nSkillAbility <= m_nSkillAbilityMax;
	}
};

// �������Ƿ�������״̬
class CECScriptCheckPlayerIsDead : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDead();
	}
};

// �������Ƿ��ڷ���״̬
class CECScriptCheckPlayerIsFlying : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsFlying();
	}
};

// �������Ƿ��ڴ���״̬
class CECScriptCheckPlayerIsSitting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsSitting();
	}
};

// �������Ƿ��ڷ���״̬
class CECScriptCheckPlayerIsInVader : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsInVader();
	}
};

// �������Ƿ��ں���״̬
class CECScriptCheckPlayerIsPariah : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPariah();
	}
};

// �������Ƿ���Ǳˮ״̬
class CECScriptCheckPlayerIsDiving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDiving();
	}
};

// �������Ƿ��Ѵ�PVP����
class CECScriptCheckPlayerIsPVPOpen : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPVPOpen();
	}
};

// �������Ƿ���ʱװģʽ
class CECScriptCheckPlayerIsInFashionMode : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsInFashionMode();
	}
};

// �������Ƿ���ս��״̬
class CECScriptCheckPlayerIsFighting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsFighting();
	}
};

// �������Ƿ��ڽ���״̬
class CECScriptCheckPlayerIsTrading : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsTrading();
	}
};

// �������Ƿ�����ʹ�òֿ�
class CECScriptCheckPlayerIsUsingTrashBox : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUsingTrashBox();
	}
};

// �������Ƿ񱻶���˯�߻��߻���
class CECScriptCheckPlayerIsRooting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsRooting();
	}
};

// �������Ƿ����޷�����״̬����˯�ߣ����ε�
class CECScriptCheckPlayerCannotAttack : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusCannotAttack();
	}
};

// �������Ƿ����ڹ���
class CECScriptCheckPlayerIsMeleeing : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsMeleeing();
	}
};

// �������Ƿ�������NPC�Ի�
class CECScriptCheckPlayerIsTalkingWithNPC : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsTalkingWithNPC();
	}
};

// �������Ƿ������ͷų�����ħ��
class CECScriptCheckPlayerIsSpellingMagic : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsSpellingMagic();
	}
};

// �������Ƿ�������Ծ
class CECScriptCheckPlayerIsJumping : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsJumping();
	}
};

// �������Ƿ�����ʰȡ��Ʒ
class CECScriptCheckPlayerIsPicking : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPicking();
	}
};

// �������Ƿ����ڲɼ�����
class CECScriptCheckPlayerIsGathering : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsGathering();
	}
};

// �������Ƿ����ڸ���
class CECScriptCheckPlayerIsReviving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsReviving();
	}
};

// �������Ƿ�����ʹ�ûس̾��������ڶ��������ʹ�ø����
class CECScriptCheckPlayerIsUsingItem : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUsingItem();
	}
};

// �������Ƿ����ڸ��Ի�����
class CECScriptCheckPlayerIsChangingFace : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsChangingFace();
	}
};

// �������Ƿ����������ƣ������£����صȣ�
class CECScriptCheckPlayerIsDoingSessionPose : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDoingSessionPose();
	}
};

//�������Ƿ����ڵ����ӽ�
class CECScriptCheckPlayerIsAdjustOrient : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsAdjustOrient();
	}
};

// �����Ҳֿ��Ƿ�������
class CECScriptCheckPlayerTrashBoxHasPsw : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusTrashBoxHasPsw();
	}
};

// �������Ƿ������ƶ�
class CECScriptCheckPlayerIsMoving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsMoving();
	}
};

// �������Ƿ�������Ӿ
class CECScriptCheckPlayerIsUnderWater : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUnderWater();
	}
};

// �������Ƿ����ڵ����ӵ㵽����ľ���
class CECScriptCheckPlayerIsAdjustDistance : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsAdjustDistance();
	}
};

// �������Ƿ����ڷ���ͷ
class CECScriptCheckPlayerIsPlayTrick : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPlayTrick();
	}
};

// �������Ƿ񽫰����е���Ʒ�ƶ�����������������
class CECScriptCheckInventoryItemMoved : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetInventoryItemMoved();
	}
};

// ��鶨ʱ���Ƿ��Ѵ���
class CECScriptCheckIsTimerTriggered : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		DWORD dwStart = 0, dwDuration = 0;
		pState->GetScriptTimerParam(dwStart, dwDuration);
		return (a_GetTime() - dwStart >= dwDuration);
	}
};

// ����Ƿ��ѽӵ�PQ����
class CECScriptCheckPQIsTriggered : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState)const
	{
		return pState->GetPQIsTriggered();
	}
};

// ����Ƿ��Ѿ����˺Ųֿ�
class CECScriptCheckAccountBoxIsTriggered : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState)const
	{
		return pState->GetAccountBoxIsTriggered();
	}
};

// ����Ƿ�����Զ�Ѱ·���Զ��ƶ�
class CECScriptCheckAutoMove : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetAutoMoving();
	}
};

// ���֡��
class CECScriptCheckFrameRateBetween : public CECScriptCheckBase {
private:
	int m_iMaxFrameRate;
	int m_iMinFrameRate;
public:
	CECScriptCheckFrameRateBetween() {
		AddParam(m_iMinFrameRate);
		AddParam(m_iMaxFrameRate);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		const int MAX_FRAMERATE_RECORD = 5;
		const std::deque<unsigned short>& frameRateList = pState->GetFrameRate();
		if (frameRateList.size() < MAX_FRAMERATE_RECORD) {
			// ����������
			return false;
		}
		for (std::deque<unsigned short>::const_iterator itr = frameRateList.begin();
		itr != frameRateList.end(); ++itr) {
			if (*itr < m_iMinFrameRate || *itr > m_iMaxFrameRate) {
				return false;
			}
		}
		return true;
	}
};

// ����Ƿ�ӵ�г���
class CECScriptCheckHavePet : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetHavePet();
	}
};

// ����������
class CECScriptCheckReputationBetween : public CECScriptCheckBase {
private:
	int m_iMinReputation;
	int m_iMaxReputation;
public:
	CECScriptCheckReputationBetween() {
		AddParam(m_iMinReputation);
		AddParam(m_iMaxReputation);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		int reputation = pState->GetReputation();
		return m_iMinReputation <= reputation && reputation <= m_iMaxReputation;
	};
};

// ���ת������
class CECScriptCheckReincarnation : public CECScriptCheckBase {
private:
	unsigned short m_ucReincarnationCount;
public:
	CECScriptCheckReincarnation() {
		AddParam(m_ucReincarnationCount);
	}
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		unsigned char reincarnationCount = pState->GetReincarnationCount();
		return reincarnationCount - 1 == m_ucReincarnationCount;
	}
};

// ����Ƿ���гƺ�
class CECScriptCheckTitle : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetHaveTitle();
	}
};

// ��鵱ǰ�ǲ���û�нű�������
class CECScriptCheckNoActiveScript : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return !pState->GetHaveActiveScript();
	}
};