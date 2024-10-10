#pragma once

class A3DCombinedAction;

class A3DCombActDynData
{
public:
	A3DCombActDynData(A3DCombinedAction* pAct, CECModel* pECModel);
	  virtual ~A3DCombActDynData();

  static int s_CombActFxArrayCount;

protected:


	A3DCombinedAction*	m_pAct;
	CECModel*			m_pECModel;
	int					m_nCurLoop;
	int					m_nCurActLoop;
	DWORD				m_dwTimeSpan;
	float				m_fWeight;
	int					m_nTransTime;
	DWORD				m_dwUserData;
	bool				m_bAbsTrack;
	bool				m_bNoFx;
	bool				m_bNoCamShake;
	ALISTPOSITION		m_CurEventPos;
	const ChildActInfo*	m_pParentInfo;
	CECModel*			m_pParentModel;
	DWORD				m_dwDeltaTime;
	bool				m_bStopPrevAct;
	FxBindBaseList		m_ActFxArray;
	int					m_nChannel;
	DWORD				m_dwEventMask;
	ActDynArray			m_arrActLoopNum;
	int					m_nCurActIndex;
	DWORD				m_dwDynComActSpan;
	bool				m_bIsInfiniteComAct;
	EventInfoMap		m_mapDynAddedInfo;

	clientid_t			m_idCaster;
	clientid_t			m_idCastTarget;
	A3DVECTOR3			m_ptFixed;
	unsigned char		m_SerialId;
	TargetDataVec		m_TargetsInfo;
	bool				m_IsAttackAct;

	bool				m_bResetPSWhenActionStop;
	bool				m_bSetSpeedWhenActStart;

	float				m_fModelScale;

	friend class GFX_INFO;
	friend class SFX_INFO;
	friend class ChildActInfo;
	friend class MaterialScaleChange;
	friend class SGCAttackPointMark;
	friend class GfxScriptEvent;
	friend class BoneScaleChange;
	friend class MaterialScaleTrans;
	friend class AUDIOEVENT_INFO;

protected:

	void ExpireActFx();
	void UpdateFxParam(DWORD dwDeltaTime);
	void LoopCurAct(PACTION_INFO pCur, DWORD dwDeltaTime);
	void LoopNext(DWORD dwDeltaTime);

	bool IsCurActFinished(const PACTION_INFO pInfo) const 
	{ return !pInfo->IsInfinite() && m_nCurActLoop >= m_arrActLoopNum[m_nCurActIndex].GetLoopNum(); }
	void CalcDynComActSpan();
	void ClearParentInfo();

public:

	// script writer specify the event name, and make sure the name is uniquely
	bool AddDynEventInfo(const char* szName, const EVENT_INFO* pInfo, bool bIsStart = true);
	void ClearDynEventInfo();

	void RemoveAllActiveFx(bool bForceStopFx = false);

	void Resume();
	void Release();

	int GetActChannel() const { return m_nChannel; }
	DWORD GetCurTimeSpan() const { return m_dwTimeSpan; }
	DWORD GetComActTimeSpan() const { return m_dwDynComActSpan; }
	bool IsComActInfinite() const { return m_bIsInfiniteComAct; }

	void SetParentActInfo(const ChildActInfo* pParent, CECModel* pModel, DWORD dwDeltaTime);

	virtual void Play(int nChannel, float fWeight, int nTransTime, DWORD dwEventMask, bool bRestart = true, bool bAbsTrack = false, bool bNoFx = false, bool bNoCamShake = false);
	virtual void UpdateAct(DWORD dwDeltaTime);
	virtual void UpdateEvent(DWORD dwActTime, DWORD dwEventTime);
	void Stop(bool bStopAct = true, bool bForceStopFx = false);
	void Render(A3DViewport* pView);
	int GetTransTime() const { return m_nTransTime; }
	void SetTransTime(int nTransTime) { m_nTransTime = nTransTime; }
	bool GetAbsTrack() const { return m_bAbsTrack; }
	void SetAbsTrack(bool b) { m_bAbsTrack = b; }
	bool GetNoFxFlag() const { return m_bNoFx; }
	void SetNoFxFlag(bool b) { m_bNoFx = b; }
	bool GetNoCamShakeFlag() const { return m_bNoCamShake; }
	void SetNoCamShakeFlag(bool b) { m_bNoCamShake = b; }
	bool IsAllEventFinished() const;
	virtual bool IsActionStopped() const { return m_pAct->m_ActLst.GetCount() == 0 || (m_pAct->m_nLoops != -1 && m_nCurLoop >= m_pAct->m_nLoops); }
	bool IsAllFinished() const { return IsActionStopped() && IsAllEventFinished(); }
	bool IsActionFinished() const { return m_dwTimeSpan >= m_dwDynComActSpan; }
	CECModel* GetHostModel() { return m_pECModel; }
	const ChildActInfo* GetParentActInfo() const { return m_pParentInfo; }
	CECModel* GetParentModel() { return m_pParentModel; }
	DWORD GetParentDeltaTime() { return m_dwDeltaTime; }
	A3DCombinedAction* GetComAct() { return m_pAct; }
	void SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	DWORD GetUserData() { return m_dwUserData; }
	void SetStopPrevAct(bool b) { m_bStopPrevAct = b; }
	bool GetStopPrevAct() const { return m_bStopPrevAct; }
	bool IsResetPSWhenActStop() const { return m_bResetPSWhenActionStop; }
	void SetResetPSWhenActStop(bool b) { m_bResetPSWhenActionStop = b; }
	bool IsSetSpeedWhenActStart() const { return m_bSetSpeedWhenActStart; }
	void SetSpeedWhenActStart(bool b) { m_bSetSpeedWhenActStart = b; }

	float	GetModelScale() const { return m_fModelScale; }

	// attack property
	void SetCasterId(clientid_t nCaster) { m_idCaster = nCaster; }
	clientid_t GetCasterId() const { return m_idCaster; }
	void SetCastTargetId(clientid_t idTarget) { m_idCastTarget = idTarget; }
	clientid_t GetCastTargetId() const { return m_idCastTarget; }
	void SetFixedPoint(const A3DVECTOR3& pt) { m_ptFixed = pt; }
	const A3DVECTOR3& GetFixedPoint() const { return m_ptFixed; }
	void SetSerialId(unsigned char id) { m_SerialId = id; }
	unsigned char GetSerialId() const { return m_SerialId; }
	void AddOneTarget(const TARGET_DATA& td) { m_TargetsInfo.push_back(td); }
	bool IsAttackAct() const { return m_IsAttackAct; }
	void SetAttackAct(bool b) { m_IsAttackAct = b; }
	void FlushDamageInfo();
};

class A3DAnimNodeBlendWalkRun;

class A3DWalkRunDynData : public A3DCombActDynData
{
	friend class CECModel;

public:

	explicit A3DWalkRunDynData( A3DCombinedAction* pAct, CECModel* pECModel );
	virtual ~A3DWalkRunDynData();

protected:

	virtual void Play( int iChannel, float fWeight, int iTransTime, DWORD dwEventMask, bool bRestart = true, bool bAbsTrack = false, bool bNoFx = false, bool bNoCamShake = false );
	virtual void UpdateAct( DWORD dwDeltaTime );
	virtual void UpdateEvent( DWORD dwActTime, DWORD dwEventTime );
	virtual bool IsActionStopped() const;

	void InnerUpdateAct( DWORD dwDeltaTime );

protected:

	A3DAnimNodeBlendWalkRun*	m_pWalkRunNode;

	float	m_fTimeSpan;
	float	m_fActTime;
	float	m_fEventTime;
};

class AnimBlendTree;

class A3DWalkRunUpDownDynData : public A3DWalkRunDynData
{
	friend class CECModel;

public:

	explicit A3DWalkRunUpDownDynData( A3DCombinedAction* pAct, CECModel* pECModel );
	virtual ~A3DWalkRunUpDownDynData();

	static bool IsUseSlope() { return s_bUseSlope; }
	static void SetUseSlope(bool bUseSlope ) { s_bUseSlope = bUseSlope; }
	static float GetSlopeAngle() { return s_fSlopeAngle; }
	static void SetSlopeAngel(float fSlopeAngle) { s_fSlopeAngle = fSlopeAngle; }
	static void SetWeightTransTime(float fTrans) { s_fWeightTrans = fTrans; }
	static float GetWeightTransTime() { return s_fWeightTrans; }

protected:

	virtual void Play( int iChannel, float fWeight, int iTransTime, DWORD dwEventMask, bool bRestart = true, bool bAbsTrack = false, bool bNoFx = false, bool bNoCamShake = false );
	virtual void UpdateAct( DWORD dwDeltaTime );

protected:

	AnimBlendTree*	m_pWalkRunUpDownBlendTree;

	float	m_fWRWeight;
	static float	s_fWeightTrans;//weight trans time

	static bool	s_bUseSlope; //是否启用上下坡动作融合
	static float   s_fSlopeAngle; //坡度阈值，单位°，坡度大于m_fSlopeAngle时上下坡动作权重为1，0-m_fSlopeAngle上下坡动作权重0-1
};

class A3DAnimNodeBlend3Children;

class A3DRunTurnDynData : public A3DCombActDynData
{
	friend class CECModel;

public:

	explicit A3DRunTurnDynData( A3DCombinedAction* pAct, CECModel* pECModel );
	virtual ~A3DRunTurnDynData();

protected:

	A3DAnimNodeBlend3Children* m_pRunTurnNode;

	float	m_fTimeSpan;
	float	m_fActTime;
	float	m_fEventTime;

protected:

	virtual void Play( int nChannel, float fWeight, int nTransTime, DWORD dwEventMask, bool bRestart = true, bool bAbsTrack = false, bool bNoFx = false, bool bNoCamShake = false );
	virtual void UpdateAct(DWORD dwDeltaTime);
	virtual void UpdateEvent( DWORD dwActTime, DWORD dwEventTime );
	virtual bool IsActionStopped() const;

};