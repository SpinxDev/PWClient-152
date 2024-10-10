#ifndef _A3DSKILLGFXEVENT_H_
#define _A3DSKILLGFXEVENT_H_

#include "A3DMatrix.h"
#include "A3DGFXExMan.h"
#include "GfxCommonTypes.h"

extern A3DGFXExMan* AfxGetGFXExMan();

namespace _SGC 
{

class A3DSkillGfxComposer;
class A3DSkillGfxEvent;

enum GfxMoveMode
{
	enumLinearMove = 0,		// 直线
	enumParabolicMove,		// 抛物线
	enumMissileMove,		// 导弹
	enumMeteoricMove,		// 流星雨
	enumHelixMove,			// 螺旋线
	enumCurvedMove,			// 香蕉线
	enumAccMove,			// 加速
	enumOnTarget,			// 定点
	enumLink,				// 链接
	enumRandMove,			// 随机移动
	enumCustomParabolicMove,				// 自定义抛物线
	enumCustomMissileMove,			//自定义导弹
	enumCustomMeteoricMove,			//自定义流星雨
	enumOnTargetFix,		//定点，不跟随目标移动
	enumMoveModeNum
};

enum EmitShape
{
	enumBox = 0,
	enumSphere,
	enumCylinder,
	enumShapeNum
};

enum GfxHitPos
{
	enumHitCenter = 0,
	enumHitBottom
};

struct GFX_SKILL_PARAM
{
	union
	{
		bool	bVal;
		int		nVal;
		float	fVal;
	};

	bool		m_bArea;
	EmitShape	m_Shape;
	A3DVECTOR3	m_vSize;
};

enum GfxSkillValType
{
	enumGfxSkillBool,
	enumGfxSkillInt,
	enumGfxSkillFloat,
	enumGfxSkillValNum
};

typedef DWORD GfxFlyHitMode;

enum
{
	enumAttFlyOnly		= 1, // (1 << 0)
	enumAttHitOnly		= 2, // (1 << 1)
	enumAttBothFlyHit	= 3, // (enumAttFlyOnly | enumAttHitOnly)
	enumAttHitGround	= 4, // (1 << 2)
};

struct SKILL_GFX_EVENT_PARAMS
{
	clientid_t				nCasterID;
	clientid_t				nCastTargetID;
	unsigned char			SerialID;
	const A3DVECTOR3*		pFixedPt;
	clientid_t				nHostID;
	clientid_t				nTargetID;
	GfxFlyHitMode			mode;
	int						nFlySfxPriority;
	int						nHitSfxPriority;
	bool					bReverse;
	float					fFlySpeed;
	DWORD					dwDelayTime;
	bool					bHasTarget;
	int						nAttIndex;
	int						nAttOrientation;
	float					fGfxScale;
	bool					bAllExtraHit;
};

class GFX_CUSTOM_MOVE_PARAM
{
public:
	GFX_CUSTOM_MOVE_PARAM(GfxMoveMode nMode) 
		: m_nMode(nMode) {}

	virtual ~GFX_CUSTOM_MOVE_PARAM() {}

	static GFX_CUSTOM_MOVE_PARAM* CreateCustomMoveParam(GfxMoveMode nMode);

	virtual void Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual void Save(AFile* pFile) = 0;
	virtual void Clone(GFX_CUSTOM_MOVE_PARAM* pParam) = 0;

	GfxMoveMode GetMoveMode() const { return m_nMode; }
protected:
	GfxMoveMode	m_nMode;
};

class GFX_CUSTOM_LINK_MOVE_PARAM : public GFX_CUSTOM_MOVE_PARAM
{
public:
	GFX_CUSTOM_LINK_MOVE_PARAM(GfxMoveMode nMode) : GFX_CUSTOM_MOVE_PARAM(nMode),
		m_fAcc(0),
		m_bMoving(false)
	{

	}
public:
	float m_fAcc;
	bool m_bMoving;
protected:
	virtual void Load(AFile* pFile, DWORD dwVersion);
	virtual void Save(AFile* pFile);
	virtual void Clone(GFX_CUSTOM_MOVE_PARAM* pParam);
};

class GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM : public GFX_CUSTOM_MOVE_PARAM
{
public:
	GFX_CUSTOM_PARAMBOLIC_MOVE_PARAM(GfxMoveMode nMode) : GFX_CUSTOM_MOVE_PARAM(nMode),
		m_fInitYaw(0),
		m_fAngleSpeed(0),
		m_fFixAmplitude(1.0f),
		m_fProportionalAmplitude(0),
		m_bAverage(false),
		m_bHalfAverage(false),
		m_bGroup(false),
		m_fGroupDelay(200),
		m_nSeg(1),
		m_bLasting(false)
	{

	}

public:
	float	m_fInitYaw;
	float	m_fAngleSpeed;
	float	m_fFixAmplitude;
	float	m_fProportionalAmplitude;
	bool	m_bAverage;
	bool	m_bHalfAverage;
	bool	m_bGroup;
	float	m_fGroupDelay;
	int		m_nSeg;
	bool	m_bLasting;			//是否持续技能

protected:
	virtual void Load(AFile* pFile, DWORD dwVersion);
	virtual void Save(AFile* pFile);
	virtual void Clone(GFX_CUSTOM_MOVE_PARAM* pParam);
};

class GFX_CUSTOM_MISSILE_MOVE_PARAM : public GFX_CUSTOM_MOVE_PARAM
{
public:
	GFX_CUSTOM_MISSILE_MOVE_PARAM(GfxMoveMode nMode) : GFX_CUSTOM_MOVE_PARAM(nMode),
		m_fInitYaw(0),
		m_nCurveTime(400),
		m_fCurveAmplitude(1),
		m_bAverage(false),
		m_bHalfAverage(false),
		m_nCurveStayTime(0),
		m_fCurveStayAmplitude(1),
		m_fAngleSpeedStay(1),
		m_bSelfRotate(false)
	{
		
	}

public:
	float	m_fInitYaw;
	int		m_nCurveTime;
	float	m_fCurveAmplitude;
	bool	m_bAverage;
	bool	m_bHalfAverage;
	int		m_nCurveStayTime;
	float	m_fCurveStayAmplitude;
	float	m_fAngleSpeedStay;
	bool	m_bSelfRotate;

protected:
	virtual void Load(AFile* pFile, DWORD dwVersion);
	virtual void Save(AFile* pFile);
	virtual void Clone(GFX_CUSTOM_MOVE_PARAM* pParam);
};

class GFX_CUSTOM_METEORIC_MOVE_PARAM : public GFX_CUSTOM_MOVE_PARAM
{
public:
	GFX_CUSTOM_METEORIC_MOVE_PARAM(GfxMoveMode nMode) : GFX_CUSTOM_MOVE_PARAM(nMode),
		m_fFallHeight(10.0f),
		m_fGroundHeight(0.0f),
		m_fRadius(5.0f),
		m_bFixRadius(false),
		m_fXOffset(0.0f),
		m_fZOffset(0.0f),
		m_bRandomOffset(false),
		m_bInnerCentric(false)
	{

	}

public:
	float	m_fFallHeight;
	float	m_fGroundHeight;
	float	m_fRadius;
	bool	m_bFixRadius;
	float	m_fXOffset;
	float	m_fZOffset;
	bool	m_bRandomOffset;
	bool	m_bInnerCentric;

protected:
	virtual void Load(AFile* pFile, DWORD dwVersion);
	virtual void Save(AFile* pFile);
	virtual void Clone(GFX_CUSTOM_MOVE_PARAM* pParam);
};


class CGfxMoveBase
{
protected:

	CGfxMoveBase(GfxMoveMode mode) : m_Mode(mode), m_pMoveParam(NULL), m_pEvent(NULL), m_bIsMoving(false) {}

public:

	virtual ~CGfxMoveBase() {}

protected:

	GfxMoveMode	m_Mode;
	GFX_CUSTOM_MOVE_PARAM* m_pMoveParam;
	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vMoveDir;
	bool		m_bOneOfCluser;
	float		m_fFlySpeed;
	DWORD		m_dwMaxFlyTime;
	bool		m_bReverse;

	// rand area
	bool		m_bArea;
	EmitShape	m_Shape;
	A3DVECTOR3	m_vSize;
	A3DVECTOR3	m_vXRange;
	A3DVECTOR3	m_vYRange;
	A3DVECTOR3	m_vZRange;
	float		m_fSquare;
	float		m_fSquareH;

	//
	int			m_nIndex;
	int			m_nCount;
	A3DSkillGfxEvent*	m_pEvent;

	bool		m_bIsMoving;

protected:
	void CalcRange(const A3DVECTOR3& vDir)
	{
		m_vYRange = _unit_y;
		m_vZRange.Set(vDir.x, 0, vDir.z);
		if (m_vZRange.Normalize() < .01f) m_vZRange = _unit_z;
		m_vXRange = CrossProduct(m_vYRange, m_vZRange);
		m_vXRange *= m_vSize.x;
		m_vYRange *= m_vSize.y;
		m_vZRange *= m_vSize.z;
		m_fSquare = SquareMagnitude(m_vSize);
		m_fSquareH = m_vSize.x * m_vSize.x + m_vSize.z * m_vSize.z;
	}

	A3DVECTOR3 GetRandOff() const
	{
		if (m_Shape == enumBox)
		{
			A3DVECTOR3 xOff, yOff, zOff;
			xOff = _SymmetricRandom() * m_vXRange;
			yOff = _SymmetricRandom() * m_vYRange;
			zOff = _SymmetricRandom() * m_vZRange;
			return xOff + yOff + zOff;
		}
		else if (m_Shape == enumSphere)
		{
			float x, y, z;

			while (true)
			{
				x = _SymmetricRandom();
				y = _SymmetricRandom();
				z = _SymmetricRandom();
				if (x * x + y * y + z * z <= 1.0f) break;
			}
			
			return x * m_vXRange + y * m_vYRange + z * m_vZRange;
		}
		else if (m_Shape == enumCylinder)
		{
			float x, y, z;
			y = _SymmetricRandom();

			while (true)
			{
				x = _SymmetricRandom();
				z = _SymmetricRandom();
				if (x * x + z * z <= 1.0f) break;
			}

			return x * m_vXRange + y * m_vYRange + z * m_vZRange;
		}
		else
			return _unit_zero;
	}

	bool IsPosInRange(const A3DVECTOR3& vPos) const
	{
		if (m_Shape == enumBox)
		{
			if (fabsf(vPos.x) > m_vSize.x) return false;
			if (fabsf(vPos.y) > m_vSize.y) return false;
			if (fabsf(vPos.z) > m_vSize.z) return false;
			return true;
		}
		else if (m_Shape == enumSphere)
			return SquareMagnitude(vPos) <= m_fSquare;
		else if (m_Shape == enumCylinder)
		{
			if (fabsf(vPos.y) > m_vSize.y) return false;
			if (vPos.x * vPos.x + vPos.z * vPos.z > m_fSquareH) return false;
			return true;
		}

		return true;
	}

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget) = 0;
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos) = 0;
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		m_bArea		= param->m_bArea;
		m_Shape		= param->m_Shape;
		m_vSize		= param->m_vSize;
	}
	virtual void SetIsCluster(bool bCluster, int nIndex, int nCount) { m_bOneOfCluser = bCluster; m_nIndex=nIndex; m_nCount=nCount; }
	virtual void UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget) {}
	virtual void SetReverse(bool bReverse) { m_bReverse = bReverse; }

	void SetIsArea(bool bArea) { m_bArea = bArea; }
	void SetShape(EmitShape shape) { m_Shape = shape; }
	void SetRange(const A3DVECTOR3& vSize) { m_vSize = vSize; }
	GfxMoveMode GetMode() const { return m_Mode; }
	const A3DVECTOR3& GetPos() const { return m_vPos; }
	const A3DVECTOR3& GetMoveDir() const { return m_vMoveDir; }
	bool IsReverse() const { return m_bReverse; }
	void SetFlySpeed(float fSpeed) { m_fFlySpeed = fSpeed; }
	void SetMaxFlyTime(DWORD dwTime) { m_dwMaxFlyTime = dwTime; }
	void SetSkillEvent(A3DSkillGfxEvent* pEvent) { m_pEvent=pEvent; }
	void SetCustomMoveParam(GFX_CUSTOM_MOVE_PARAM* pParam) { m_pMoveParam = pParam; }
	static CGfxMoveBase* CreateMoveMethod(GfxMoveMode mode);
};

class CGfxLinearMove : public CGfxMoveBase
{
public:
	CGfxLinearMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxLinearMove() {}

protected:
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxParabolicMove : public CGfxMoveBase
{
public:
	CGfxParabolicMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxParabolicMove() {}

protected:
	A3DVECTOR3 m_vOrgPos;
	A3DVECTOR3 m_vHorzVel;
	float m_fVertVel;
	float m_fTotalTime;
	float m_fCurTime;
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxMissileMove : public CGfxMoveBase
{
public:
	CGfxMissileMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxMissileMove() {}

protected:
	A3DVECTOR3 m_vOrgPos;
	float m_fCurVel;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxMeteoricMove : public CGfxMoveBase
{
public:
	CGfxMeteoricMove(GfxMoveMode mode) : CGfxMoveBase(mode) { m_fRadius = 2.0f; }
	~CGfxMeteoricMove() {}

protected:
	A3DVECTOR3 m_vFallVel;
	float m_fRadius;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxHelixMove : public CGfxMoveBase
{
public:
	CGfxHelixMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_fRadius(0) {}
	~CGfxHelixMove() {}

protected:
	float m_fFlyTime;
	float m_fRadius;
	float m_fShrinkRate;
	float m_fAngle;
	float m_fSpeed;

	A3DVECTOR3 m_vCenter;
	A3DVECTOR3 m_vAxisX;
	A3DVECTOR3 m_vAxisY;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxCurvedMove : public CGfxMoveBase
{
public:
	CGfxCurvedMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_bFromRight(true) {}
	~CGfxCurvedMove() {}

protected:
	bool m_bFromRight;
	float m_fLatSpd;
	float m_fTime;
	float m_fSpeed;
	A3DVECTOR3 m_vAxisX;
	A3DVECTOR3 m_vAxisZ;
	A3DVECTOR3 m_vZSpd;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_bFromRight = param->bVal;
	}
};

class CGfxAccMove : public CGfxMoveBase
{
public:
	CGfxAccMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_fAcc(1.0f) {}
	~CGfxAccMove() {}

protected:
	float m_fAcc;
	float m_fSpeed;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fAcc = param->fVal;
	}
};

class CGfxOnTargetMove : public CGfxMoveBase
{
public:
	CGfxOnTargetMove(GfxMoveMode mode) : CGfxMoveBase(mode) { m_fRadius = 0; }
	virtual ~CGfxOnTargetMove() {}

protected:
	float		m_fRadius;
	A3DVECTOR3	m_vOffset;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fRadius = param->fVal;
	}
};

class CGfxOnTargetFixMove : public CGfxOnTargetMove
{
public:
	CGfxOnTargetFixMove(GfxMoveMode mode) : CGfxOnTargetMove(mode) {}
	virtual ~CGfxOnTargetFixMove() {}
public:
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};

class CGfxLinkMove : public CGfxMoveBase
{
public:
	CGfxLinkMove(GfxMoveMode mode) : CGfxMoveBase(mode), m_fAcc(0) {}
	~CGfxLinkMove()
	{
		for (size_t i = 0; i < m_Params.size(); i++)
			delete m_Params[i];
	}

protected:
	float m_fSpeed;
	float m_fAcc;
	A3DVECTOR3	m_vDestPos;
	GfxParamList m_Params;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void UpdateGfxParam(A3DGFXEx* pGfx, const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
};

class CGfxRandMove : public CGfxMoveBase
{
public:
	CGfxRandMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxRandMove() {}

protected:
	float		m_fStep;
	float		m_fSpeed;
	float		m_fTimeSpan;
	float		m_fCurSpan;
	A3DVECTOR3	m_CtrlPoints[6];

	void CalcStep();
	A3DVECTOR3 GetNextStep(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vCenter) const;

public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
	virtual void SetParam(const GFX_SKILL_PARAM* param)
	{
		CGfxMoveBase::SetParam(param);
		m_fSpeed = param->fVal;
	}
};

class CGfxCustomParabolicMove : public CGfxMoveBase
{
public:
	CGfxCustomParabolicMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxCustomParabolicMove() {}
	
protected:
	A3DVECTOR3 m_vOrgPos;
	
	A3DVECTOR3	m_vHorzPos;
	A3DVECTOR3	m_vHorzDir;
	float m_fSpeed;
	A3DVECTOR3	m_vParabolicDir;		//抛物线扩张方向
	float	m_fAngleSpeed;			//旋转角速度
	float	m_fAmplitude;		//扩张系数, 幅度/总距离
	float	m_fFixAmplitude;	//固定幅度,不受距离影响,如果为0则使用扩张系数
	bool	m_bAverage;			
	bool	m_bGroup;
	float	m_fGroupDelay;
	int		m_nSeg;
	bool	m_bLasting;
	
public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};


class CGfxCustomMissileMove : public CGfxMoveBase
{
public:
	CGfxCustomMissileMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxCustomMissileMove() {}
	
protected:
	A3DVECTOR3 m_vOrgPos;
	float m_fCurVel;
	A3DVECTOR3	m_vHorzDir;
	float	m_fInitDist;
	DWORD	m_dwTotalTime;
	
	A3DVECTOR3	m_vMissileDir;			//yaw导弹弧度扩张方向
	bool m_bAverage;
	DWORD	m_dwCurveTime;
	float	m_fCurveAmplitude;
	DWORD	m_dwCurveStayTime;
	float	m_fCurveStayAmplitude;
	float	m_fAngleSpeedStay;
	bool	m_bSelfRotate;
	
	int m_nAdjustCount;
public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};


class CGfxCustomMeteoricMove : public CGfxMoveBase
{
public:
	CGfxCustomMeteoricMove(GfxMoveMode mode) : CGfxMoveBase(mode) {}
	~CGfxCustomMeteoricMove() {}
	
protected:
	float	m_fFallHeight;
	float	m_fGroundHeight;
	float	m_fRadius;
	bool	m_bFixRadius;
	float	m_fXOffset;
	float	m_fZOffset;
	bool	m_bRandomOffset;
	bool	m_bInnerCentric;	//内聚
	
	A3DVECTOR3	m_vFallVel;
	float	m_fGoundY;
	float	m_fOldY;
	
public:
	virtual void StartMove(const A3DVECTOR3& vHost, const A3DVECTOR3& vTarget);
	virtual bool TickMove(DWORD dwDeltaTime, const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos);
};


inline CGfxMoveBase* CGfxMoveBase::CreateMoveMethod(GfxMoveMode mode)
{
	switch(mode)
	{
	case enumLinearMove:
		return new CGfxLinearMove(mode);
	case enumParabolicMove:
		return new CGfxParabolicMove(mode);
	case enumMissileMove:
		return new CGfxMissileMove(mode);
	case enumMeteoricMove:
		return new CGfxMeteoricMove(mode);
	case enumHelixMove:
		return new CGfxHelixMove(mode);
	case enumCurvedMove:
		return new CGfxCurvedMove(mode);
	case enumAccMove:
		return new CGfxAccMove(mode);
	case enumOnTarget:
		return new CGfxOnTargetMove(mode);
	case enumLink:
		return new CGfxLinkMove(mode);
	case enumRandMove:
		return new CGfxRandMove(mode);
	case enumCustomParabolicMove:
		return new CGfxCustomParabolicMove(mode);
	case enumCustomMissileMove:
		return new CGfxCustomMissileMove(mode);
	case enumCustomMeteoricMove:
 		return new CGfxCustomMeteoricMove(mode);
	case enumOnTargetFix:
		return new CGfxOnTargetFixMove(mode);
	}

	assert(false);
	return NULL;
}

class A3DSkillGfxMan;
class A3DSkillGfxEvent
{
public:

	A3DSkillGfxEvent(A3DSkillGfxMan* pMan, GfxMoveMode mode);
	virtual ~A3DSkillGfxEvent();

protected:

	friend class A3DSkillGfxMan;

	struct HitGfxInfo
	{
		clientid_t id_target;
		A3DGFXEx* hit_gfx;
		char is_infinite; // -1: Resource NotReady, 0: Not Infinite, 1: Infinite
		bool hit_ground;
	};

	typedef abase::vector<HitGfxInfo> HitGfxArr;

	A3DSkillGfxMan* m_pMan;
	CGfxMoveBase*	m_pMoveMethod;
	A3DSkillGfxComposer* m_pComposer;

	A3DGFXEx*		m_pFlyGfx;
	float			m_fFlySpeed;
	DWORD			m_dwCurSpan;
	DWORD			m_dwHitSpan;
	DWORD			m_dwDelayTime;
	int				m_nDivisions;

	clientid_t		m_nHostID;			// SGC起始者ID
	clientid_t		m_nTargetID;		// SGC终止者ID
	clientid_t		m_nCasterID;
	clientid_t		m_nCastTargetID;
	DWORD			m_dwUserData;
	unsigned char	m_SerialId;
	bool			m_bHasTarget;
	TargetDataVec	m_TargetsInfo;
	HitGfxArr		m_HitGfxArr;
	A3DVECTOR3		m_vFixedPoint;
	bool			m_bTargetAdded;
	GfxFlyHitMode	m_FlyHitMode;
	int				m_nFlySfxPriority;
	int				m_nHitSfxPriority;
	int				m_nAttIndex;
	int				m_nAttOrientation;
	float			m_fGfxScale;

	bool			m_bRandomHook;
	char			m_strHook[80];

	bool			m_bGfxUseLod;
	bool			m_bGfxDisableCamShake;
	bool			m_bHostECMCreatedByGfx;

	// 具有相同Caster的Event构成一个链表
	A3DSkillGfxEvent* m_pPrvEvent;
	A3DSkillGfxEvent* m_pSblEvent;

	enum
	{
		enumWait,
		enumFlying,
		enumHit,
		enumFinished
	} m_enumState;

	bool		m_bAllExtraHit;

protected:

	void ReleaseFlyGfx(bool bCache = true);
	void ReleaseGfx(bool bCache = true);
	void PlayExtraHitGfx(DWORD dwModifier, bool bOnTarget=true);

	//	play a hit gfx on target by id
	void PlayHitGfxOnTarget(clientid_t idTarget, const char* szHook = NULL);
	//	loop all targets and play hit gfxs on them but skip the cast target
	void PlayHitGfxOnAllTargets();

	//	Play hit gfx will only play hit gfx on cast target by default
	virtual void PlayHitGfx(const char* szHook = NULL);
	//	virtual funcs
	virtual void HitTarget(const char* szHook = NULL);
	//	call back at the start point of hit target
	virtual void OnPreHitTarget();
	//	flush targets info
	virtual void FlushTargetsInfo();

public:
	// inline funcs
	void SetComposer(A3DSkillGfxComposer* pComposer) { m_pComposer = pComposer; }
	CGfxMoveBase* GetMoveMethod() { return m_pMoveMethod; }
	GfxMoveMode GetMode() const { return m_pMoveMethod->GetMode(); }
	A3DGFXEx* GetFlyGfx() { return m_pFlyGfx; }
	void SetFlyGfx(A3DGFXEx* pGfx) { m_pFlyGfx = pGfx; }
	void SetFlySpeed(float fSpeed) { m_fFlySpeed = fSpeed; }
	void SetDelay(DWORD dwDelay) { m_dwDelayTime = dwDelay; }
	DWORD GetDelay() const { return m_dwDelayTime; }
	void SetParam(const GFX_SKILL_PARAM* param) { m_pMoveMethod->SetParam(param); }
	void SetCustomMoveParam(GFX_CUSTOM_MOVE_PARAM* pMoveParam) { m_pMoveMethod->SetCustomMoveParam(pMoveParam); }
	void SetHasTarget(bool b) { m_bHasTarget = b; }
	void SetIsCluster(bool bCluster, int nIndex, int nCount) { m_pMoveMethod->SetIsCluster(bCluster,nIndex,nCount); }
	void SetReverse(bool bReverse) { m_pMoveMethod->SetReverse(bReverse); }
	void SetDivisions(int nDivisions) { m_nDivisions = nDivisions; }
	bool IsFinished() const { return m_enumState == enumFinished; }
	clientid_t GetHostID() const { return m_nHostID; }
	void SetHostID(clientid_t nID) { m_nHostID = nID; }
	clientid_t GetTargetID() const { return m_nTargetID; }
	void SetTargetID(clientid_t nID) { m_nTargetID = nID; }
	clientid_t GetCasterID() const { return m_nCasterID; }
	void SetCasterID(clientid_t nID) { m_nCasterID = nID; }
	clientid_t GetCastTargetID() const { return m_nCastTargetID; }
	void SetCastTargetID(clientid_t nID) { m_nCastTargetID = nID; }
	unsigned char GetSerialId() const { return m_SerialId; }
	void SetSerialId(unsigned char id) { m_SerialId = id; }
	void SetFixedPoint(const A3DVECTOR3& v) { m_vFixedPoint = v; }
	void SetFlyHitMode(GfxFlyHitMode mode) { m_FlyHitMode = mode; }
	void SetFlySfxPriority(int n) { m_nFlySfxPriority = n; }
	void SetHitSfxPriority(int n) { m_nHitSfxPriority = n; }

	int GetAttIndex() const { return m_nAttIndex; }
	void SetAttIndex(int n) { m_nAttIndex = n; }

	void SetAttOrientation(int n) { m_nAttOrientation = n;}

	void SetGfxScale(float fScale) { m_fGfxScale = fScale; }
	float GetGfxScale() const { return m_fGfxScale; }

	bool IsRandomHook() const { return m_bRandomHook;}
	void SetRandomHook(bool rh) {m_bRandomHook = rh;}
	void SetHitHook(const char* strHook) {strcpy_s(m_strHook, 80, strHook);}
	const char* GetHitHook() const { return m_strHook; }

	//	Userdata set by PlayAttackAction
	void SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	
	void SetGfxUseLod(bool b) { m_bGfxUseLod = b; }
	bool GetGfxUseLod() const { return m_bGfxUseLod; }
	void SetDisableCamShake(bool b) { m_bGfxDisableCamShake = b; }
	bool GetDisableCamShake() const { return m_bGfxDisableCamShake; }
	void SetHostModelCreatedByGfx(bool b) { m_bHostECMCreatedByGfx = b; }
	bool GetHostModelCreatedByGfx() const { return m_bHostECMCreatedByGfx; }

	void Resume();
	bool AddOneTarget(const TARGET_DATA& td);
	A3DSkillGfxEvent* GetLast()
	{
		A3DSkillGfxEvent* pLast = this;

		while (pLast->m_pSblEvent)
			pLast = pLast->m_pSblEvent;

		return pLast;
	}
	bool GetTargetAddedFlag() const { return m_bTargetAdded; }
	void SetTargetAddedFlag(bool b) { m_bTargetAdded = b; }
	bool IsAllExtraHit() const { return m_bAllExtraHit; }
	void SetAllExtraHit(bool b) { m_bAllExtraHit = b; }


	// virtual funcs
	virtual A3DGFXEx* LoadFlyGfx(A3DDevice* pDev, const char* szPath);
	virtual A3DGFXEx* LoadHitGfx(A3DDevice* pDev, const char* szPath, DWORD dwModifier);
	virtual void Tick(DWORD dwDeltaTime);
	virtual void Render();
};

enum
{
	enumAttStateStart = 0,
	enumAttStateFlying,
	enumAttStateHit,
};

struct ECMODEL_GFX_PROPERTY
{
	bool bGfxUseLod;
	bool bGfxDisableCamShake;
	bool bHostECMCreatedByGfx;
};

struct SKILL_GFX_HITTARGET_PARAM
{
	clientid_t idCaster;					//	PlayAttackAction parameter (caster model id)
	clientid_t idTarget;					//	PlayAttackAction parameter (target model id)
	DWORD userData;							//	PlayAttackAction parameter (UserData)
	const A3DSkillGfxComposer* pComposer;
	int	nAttIndex;
	int	nAttOrientation;
};

struct SGC_POS_INFO;
struct GETPOSITIONBYID_PARAMS
{
	GETPOSITIONBYID_PARAMS(clientid_t nId
		, const GfxHitPos& hitPos
		, const char* szHook/* = NULL*/
		, bool bRelHook/* = false*/
		, const A3DVECTOR3* pOffset/* = NULL*/
		, const char* szHanger/* = NULL*/
		, bool bChildHook/* = false*/
		, void* pUserData/* = NULL*/
		)
	{
		init(nId, hitPos, szHook, bRelHook, pOffset, szHanger, bChildHook, pUserData);
	}

	GETPOSITIONBYID_PARAMS(clientid_t nId
		, const SGC_POS_INFO& sgcPosInfo
		, void* pUserData);

	void init(
		clientid_t nId
		, const GfxHitPos& hitPos
		, const char* szHook
		, bool bRelHook
		, const A3DVECTOR3* pOffset
		, const char* szHanger
		, bool bChildHook
		, void* pUserData
		)
	{
		m_nId = nId;
		m_HitPos = hitPos;
		m_szHook = szHook;
		m_bRelHook = bRelHook;
		m_pOffset = pOffset;
		m_szHanger = szHanger;
		m_bChildHook = bChildHook;
		m_pUserData = pUserData;
	}

	clientid_t	m_nId;
	GfxHitPos	m_HitPos;
	const char* m_szHook;
	bool		m_bRelHook;
	const A3DVECTOR3* m_pOffset;
	const char* m_szHanger;
	bool		m_bChildHook;
	void*		m_pUserData;
};

typedef void (WINAPI *SHOW_DAMAGE_VALUE)(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions,DWORD dwModifier);
typedef void (WINAPI *SHOW_CASTER_INFO)(clientid_t idCaster, DWORD dwModifier);
typedef void (WINAPI *SKILL_GFX_TICK_EVENT)(clientid_t idCaster, int state, DWORD dwTickTime, const A3DVECTOR3* pDir, const A3DVECTOR3* pPos);
typedef void (WINAPI *SKILL_GFX_HITTARGET)(const SKILL_GFX_HITTARGET_PARAM* pParam);

class A3DSkillGfxMan
{
public:
	
	A3DSkillGfxMan() {}
	virtual ~A3DSkillGfxMan() {}

protected:

	friend class A3DSkillGfxEvent;
	typedef APtrList<A3DSkillGfxEvent*> SkillGfxEventLst;
	typedef abase::hash_map<clientid_t, A3DSkillGfxEvent*> SkillGfxEventMap;

	A3DDevice* m_pDevice;
	SkillGfxEventLst m_FreeLst[enumMoveModeNum];
	SkillGfxEventMap m_EventMap;

	static SHOW_DAMAGE_VALUE m_DamageShowFunc;
	static SHOW_CASTER_INFO m_CasterShowFunc;
	static SKILL_GFX_TICK_EVENT m_SkillGfxTickFunc;
	static SKILL_GFX_HITTARGET m_SkillGfxHitTargetFunc;

protected:

	virtual A3DSkillGfxEvent* CreateOneEvent(GfxMoveMode mode) = 0;
	//	Old interface, perfer to use the new one instead
	virtual bool GetPositionById(clientid_t nId, A3DVECTOR3& vPos, GfxHitPos HitPos, const char* szHook = NULL, bool bRelHook = false, const A3DVECTOR3* pOffset = NULL, const char* szHanger = NULL, bool bChildHook = false) = 0;
	//	New interface for client to override, use struct so we may add new parameters without forcing client to rebuild
	virtual bool GetPositionById(const GETPOSITIONBYID_PARAMS& param, A3DVECTOR3& vPos)
	{
		//	Call the old GetPositionById to keep the default manner works fine, but client may override THIS FUNCTION to benefit from 
		return GetPositionById(param.m_nId, vPos, param.m_HitPos, param.m_szHook, param.m_bRelHook, param.m_pOffset, param.m_szHanger, param.m_bChildHook);
	}
	virtual bool GetDirAndUpById(clientid_t nId, A3DVECTOR3& vDir, A3DVECTOR3& vUp) = 0;
	virtual float GetTargetScale(clientid_t nTargetId) = 0;
	virtual bool GetPropertyById(clientid_t nId, ECMODEL_GFX_PROPERTY* pProperty) { return false; }

	virtual A3DSkillGfxEvent* GetEmptyEvent(GfxMoveMode mode)
	{
		if (mode < 0 || mode >= enumMoveModeNum)
			return NULL;
		
		if (m_FreeLst[static_cast<int>(mode)].GetCount() == 0)
			return CreateOneEvent(mode);
		else
		{
			A3DSkillGfxEvent* pEvent = m_FreeLst[static_cast<int>(mode)].RemoveTail();
			pEvent->Resume();
			return pEvent;
		}
	}

	virtual void PushEvent(clientid_t nCasterId, A3DSkillGfxEvent* pEvent)
	{
		SkillGfxEventMap::iterator it = m_EventMap.find(nCasterId);

		if (it == m_EventMap.end())
			m_EventMap[nCasterId] = pEvent;
		else
		{
			A3DSkillGfxEvent* pNext = it->second;

			while (pNext->m_pSblEvent)
				pNext = pNext->m_pSblEvent;

			pNext->m_pSblEvent = pEvent;
			pEvent->m_pPrvEvent = pNext;
		}
	}

	bool TickOneEvent(SkillGfxEventMap::iterator it, DWORD dwDeltaTime)
	{
		clientid_t nCaster = it->first;
		A3DSkillGfxEvent* pEvent = it->second;

		while (pEvent)
		{
			A3DSkillGfxEvent* pPrev = pEvent->m_pPrvEvent;
			A3DSkillGfxEvent* pNext = pEvent->m_pSblEvent;

			if (pEvent->IsFinished())
			{
				pEvent->Resume();
				m_FreeLst[pEvent->GetMode()].AddTail(pEvent);

				if (pNext) pNext->m_pPrvEvent = pPrev;

				if (pPrev)
					pPrev->m_pSblEvent = pNext;
				else if (pNext)
					it->second = pNext;
				else
					return true;
			}
			else
				pEvent->Tick(dwDeltaTime);

			pEvent = pNext;
		}

		return false;
	}

public:

	size_t GetEventCount() const { return m_EventMap.size(); }

	A3DSkillGfxEvent* GetSkillGfxEvent(clientid_t nCasterId, unsigned char SerialId)
	{
		SkillGfxEventMap::iterator it = m_EventMap.find(nCasterId);

		if (it == m_EventMap.end())
			return NULL;

		A3DSkillGfxEvent* pEvent = it->second;

		while (pEvent)
		{
			if (pEvent->GetSerialId() == SerialId)
				break;

			pEvent = pEvent->m_pSblEvent;
		}

		return pEvent;
	}

	bool Tick(DWORD dwDeltaTime)
	{
		SkillGfxEventMap::iterator it = m_EventMap.begin();

		while (it != m_EventMap.end())
		{
			if (TickOneEvent(it, dwDeltaTime))
				it = m_EventMap.erase(it);
			else
				++it;
		}

		return true;
	}

	bool Render()
	{
		SkillGfxEventMap::iterator it = m_EventMap.begin();

		for (; it != m_EventMap.end(); ++it)
		{
			A3DSkillGfxEvent* pEvent = it->second;

			while (pEvent)
			{
				pEvent->Render();
				pEvent = pEvent->m_pSblEvent;
			}
		}

		return true;
	}

	void Release()
	{
		SkillGfxEventMap::iterator it = m_EventMap.begin();

		for (; it != m_EventMap.end(); ++it)
		{
			A3DSkillGfxEvent* pEvent = it->second;

			while (pEvent)
			{
				A3DSkillGfxEvent* pNext = pEvent->m_pSblEvent;
				delete pEvent;
				pEvent = pNext;
			}
		}

		m_EventMap.clear();

		for (int i = 0; i < enumMoveModeNum; i++)
		{
			ALISTPOSITION pos = m_FreeLst[i].GetHeadPosition();

			while (pos) delete m_FreeLst[i].GetNext(pos);
			m_FreeLst[i].RemoveAll();
		}
	}

	void Init(A3DDevice* pDev) { m_pDevice = pDev; }
	bool AddSkillGfxEvent(A3DSkillGfxComposer* pComposer, const SKILL_GFX_EVENT_PARAMS& param, int nIndex);

	static void SetDamageShowFunc(SHOW_DAMAGE_VALUE func);
	static SHOW_DAMAGE_VALUE GetDamageShowFunc();
	static void SetCasterShowFunc(SHOW_CASTER_INFO func);
	static SHOW_CASTER_INFO GetCasterShowFunc();
	static void SetSkillGfxTickFunc(SKILL_GFX_TICK_EVENT func);
	static SKILL_GFX_TICK_EVENT GetSkillGfxTickFunc();
	static void SetSkillGfxHitTargetFunc(SKILL_GFX_HITTARGET func);
	static SKILL_GFX_HITTARGET GetSkillGfxTickHitTargetFunc();
};

extern A3DSkillGfxMan* AfxGetSkillGfxEventMan();

}

#endif
