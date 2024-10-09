#pragma once

#include <ABaseDef.h>
#include <ATime.h>
#include "ECScriptCheckBase.h"
#include "ECScriptCheckState.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

#include <queue>

// 判断脚本是否执行过
// nScriptID	脚本ID
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

// 判断用户是否选择显示下一条
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

// 判断玩家等级
// nMinLevel 最低等级
// nMaxLevel 最高等级
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

// 判断玩家职业
// szProfression 职业名称
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

// 判断玩家的生命值
// nMinHPPercent 最低百分比
// nMaxHPPercent 最高百分比
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

// 判断玩家的魔法值
// nMinMPPercent 最低百分比
// nMaxMPPercent 最高百分比
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

// 检查特定窗口是否处于显示状态
// szDialogName 检查的窗口名称
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

// 检查窗口中某个控件的当前显示文字是否为特定文字，窗口必须是显示状态
// szDialogName 窗口名称
// szItemName	控件名称
// szItemText	控件显示文字
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

// 检查窗口中某个单选框或者复选框是否被选中
// szDialogName 窗口名称
// szItemName	控件名称
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

// 检查玩家是否杀死了一只怪物
class CECScriptCheckKillMonster: public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetKillMonster();
	}
};

// 检查玩家是否装备了新武器
class CECScriptCheckEquipNewWeapon: public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetEquipNewWeapon();
	}
};

// 检查当前进行服务的NPC
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

// 检查当前选中的目标NPC
// szTargetName		NPC名字，""表示无名字要求
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

// 检查当前是否选中玩家
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

// 检查当前选中的目标怪物
// szTargetName		目标怪物名字，""表示无名字要求
// szTargetAttrib	目标特殊属性，如“舍命突击”等，""表示无要求,"*"表示必须有特殊属性
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

// 判断玩家所在的地图区域名称
// szAddressName	地图区域名称
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


// 判断玩家所在的地图id
// nMapID	地图instance id
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


// 检查任务状态
// nTaskID		任务ID
// nTaskStatus	检查任务状态，0--已完成,1--已接受,2--达到杀怪或任务物品要求
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

// 检查任务包裹中的物品
// nItemID		物品ID
// nMinNumber	物品最低数量
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

// 检查包裹中的物品
// nItemID		物品ID
// nMinNumber	物品最低数量
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

// 检查玩家技能等级
// nSkillID			技能ID
// nSkillLevelMin	技能最低等级
// nSkillLevelMax	技能最高等级
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

// 检查玩家技能熟练度
// nSkillID			技能ID
// nSkillAbilityMin	技能最低熟练度
// nSkillAbilityMax	技能最高熟练度
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

// 检查玩家是否处于死亡状态
class CECScriptCheckPlayerIsDead : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDead();
	}
};

// 检查玩家是否处于飞行状态
class CECScriptCheckPlayerIsFlying : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsFlying();
	}
};

// 检查玩家是否处于打坐状态
class CECScriptCheckPlayerIsSitting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsSitting();
	}
};

// 检查玩家是否处于粉名状态
class CECScriptCheckPlayerIsInVader : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsInVader();
	}
};

// 检查玩家是否处于红名状态
class CECScriptCheckPlayerIsPariah : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPariah();
	}
};

// 检查玩家是否处于潜水状态
class CECScriptCheckPlayerIsDiving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDiving();
	}
};

// 检查玩家是否已打开PVP开关
class CECScriptCheckPlayerIsPVPOpen : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPVPOpen();
	}
};

// 检查玩家是否处于时装模式
class CECScriptCheckPlayerIsInFashionMode : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsInFashionMode();
	}
};

// 检查玩家是否处于战斗状态
class CECScriptCheckPlayerIsFighting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsFighting();
	}
};

// 检查玩家是否处于交易状态
class CECScriptCheckPlayerIsTrading : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsTrading();
	}
};

// 检查玩家是否正在使用仓库
class CECScriptCheckPlayerIsUsingTrashBox : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUsingTrashBox();
	}
};

// 检查玩家是否被定身、睡眠或者击晕
class CECScriptCheckPlayerIsRooting : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsRooting();
	}
};

// 检查玩家是否处于无法攻击状态，如睡眠，击晕等
class CECScriptCheckPlayerCannotAttack : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusCannotAttack();
	}
};

// 检查玩家是否正在攻击
class CECScriptCheckPlayerIsMeleeing : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsMeleeing();
	}
};

// 检查玩家是否正在与NPC对话
class CECScriptCheckPlayerIsTalkingWithNPC : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsTalkingWithNPC();
	}
};

// 检查玩家是否正在释放持续性魔法
class CECScriptCheckPlayerIsSpellingMagic : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsSpellingMagic();
	}
};

// 检查玩家是否正在跳跃
class CECScriptCheckPlayerIsJumping : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsJumping();
	}
};

// 检查玩家是否正在拾取物品
class CECScriptCheckPlayerIsPicking : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPicking();
	}
};

// 检查玩家是否正在采集材料
class CECScriptCheckPlayerIsGathering : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsGathering();
	}
};

// 检查玩家是否正在复活
class CECScriptCheckPlayerIsReviving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsReviving();
	}
};

// 检查玩家是否正在使用回程卷，或者正在对其他玩家使用复活卷
class CECScriptCheckPlayerIsUsingItem : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUsingItem();
	}
};

// 检查玩家是否正在个性化调整
class CECScriptCheckPlayerIsChangingFace : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsChangingFace();
	}
};

// 检查玩家是否处于特殊姿势（如坐下，倒地等）
class CECScriptCheckPlayerIsDoingSessionPose : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsDoingSessionPose();
	}
};

//检查玩家是否正在调整视角
class CECScriptCheckPlayerIsAdjustOrient : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsAdjustOrient();
	}
};

// 检查玩家仓库是否有密码
class CECScriptCheckPlayerTrashBoxHasPsw : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusTrashBoxHasPsw();
	}
};

// 检查玩家是否正在移动
class CECScriptCheckPlayerIsMoving : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsMoving();
	}
};

// 检查玩家是否正在游泳
class CECScriptCheckPlayerIsUnderWater : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsUnderWater();
	}
};

// 检查玩家是否正在调整视点到人物的距离
class CECScriptCheckPlayerIsAdjustDistance : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsAdjustDistance();
	}
};

// 检查玩家是否正在翻跟头
class CECScriptCheckPlayerIsPlayTrick : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetPlayerStatusIsPlayTrick();
	}
};

// 检查玩家是否将包裹中的物品移动到包裹的其他格子
class CECScriptCheckInventoryItemMoved : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const
	{
		return pState->GetInventoryItemMoved();
	}
};

// 检查定时器是否已触发
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

// 检查是否已接到PQ任务
class CECScriptCheckPQIsTriggered : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState)const
	{
		return pState->GetPQIsTriggered();
	}
};

// 检查是否已经打开账号仓库
class CECScriptCheckAccountBoxIsTriggered : public CECScriptCheckBase
{
public:
	virtual bool Evaluate(IECScriptCheckState * pState)const
	{
		return pState->GetAccountBoxIsTriggered();
	}
};

// 检查是否进行自动寻路或自动移动
class CECScriptCheckAutoMove : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetAutoMoving();
	}
};

// 检查帧率
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
			// 样本不充足
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

// 检查是否拥有宠物
class CECScriptCheckHavePet : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetHavePet();
	}
};

// 检查玩家声望
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

// 检查转生次数
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

// 检查是否具有称号
class CECScriptCheckTitle : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return pState->GetHaveTitle();
	}
};

// 检查当前是不是没有脚本被激活
class CECScriptCheckNoActiveScript : public CECScriptCheckBase {
public:
	virtual bool Evaluate(IECScriptCheckState * pState) const {
		return !pState->GetHaveActiveScript();
	}
};