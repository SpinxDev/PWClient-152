// TaskAreaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskAreaDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskAreaDlg dialog


CTaskAreaDlg::CTaskAreaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskAreaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskAreaDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTaskAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskAreaDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskAreaDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskAreaDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//#define _TASK_AREA
#ifdef _TASK_AREA

// cr mark, "card", 

#include <math.h>
#include <assert.h>

inline float _UnitRandom() { return rand() / (float)RAND_MAX; }
inline float _SymmetricRandom() { return _UnitRandom() * 2.0f - 1.0f; }

#define TASK_TEAM_NUM 2
#define TASK_TEAM_MEM_NUM 2

// Globals
static const float sGv;			= 10;
static const float sAcc			= 4.0f;
static const float sDeAcc		= 20.0f;
static const float sPssRadius	= 5.0f;
static const float sCtrlBRadius = 5.0f;
static const float sTcklRadius = 8.0f;
static const float sOppRadius = 12.0f;
static const float sDefRadius = 20.0f;
static const float sPerActTime = 1.0f;
static const float sBlSp = 20.0f;
static const float sBFrSpMin = 20.0f;
static const float sBFrSpMax = 30.0f;
static const float sBlSpAcc = 10.0f;

#define	_PI				3.1415926f
#define _ZONE_WIDTH		200
#define _ZONE_HEIGHT	256
#define _BORDER_WIDTH	4

inline float _clampf(float v, float f, float c)
{
	if (v < f)
		return f;
	else if (v > c)
		return c;
	return v;
}

struct TaskVec
{
	float x;
	float y;

	TaskVec() {}
	TaskVec(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
	TaskVec(const TaskVec& v)
	{
		x = v.x;
		y = v.y;
	}

	float Normalize()
	{
		float f = sqrtf(x * x + y * y);

		if (f < 1e-6)
		{
			x = y = 0;
			return 0;
		}

		float fR = 1 / f;
		x *= fR;
		y *= fR;

		return f;
	}

	float Magnitude() const { return sqrtf(x * x + y * y); }
	float SquareMagnitude() const { return x * x + y * y; }
	TaskVec operator * (float f) const { return TaskVec(x * f, y * f); }
	TaskVec operator + (const TaskVec& v) const { return TaskVec(x + v.x, y + v.y); }
	TaskVec operator - (const TaskVec& v) const { return TaskVec(x - v.x, y - v.y); }
	TaskVec& operator = (const TaskVec& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
};

inline TaskVec Normalize(const TaskVec& v)
{
	TaskVec r = v;
	r.Normalize();
	return r;
}

inline float DocProduct(const TaskVec& v1, const TaskVec& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline TaskVec RotVec(const TaskVec& v, float f)
{
	float fc = cosf(f);
	float fs = sinf(f);
	return TaskVec(fc * v.x + fs * v.y, fc * v.y - fs * v.x);
}

inline bool TurnClk(const TaskVec& v1, const TaskVec& v2)
{
	return (v1.x * v2.y - v1.y * v2.x) < 0.0f;
}

static const TaskVec sDirL(-1.0f, 0);
static const TaskVec sDirR(1.0f, 0);
static const TaskVec sUp(0, 1.0f);
static const TaskVec sDown(0, -1.0f);
static const TaskVec sDirLUp(Normalize(TaskVec(-1.0f, 1.0f)));
static const TaskVec sDirLDown(Normalize(TaskVec(-1.0f, -1.0f)));
static const TaskVec sDirRUp(Normalize(TaskVec(1.0f, 1.0f)));
static const TaskVec sDirRDown(Normalize(TaskVec(1.0f, -1.0f)));

struct TaskRect
{
	float l;
	float t;
	float r;
	float b;

	TaskRect() {}
	TaskRect(const TaskVec& lt, const TaskVec& rb)
	{
		l = lt.x;
		t = lt.y;
		r = rb.x;
		b = rb.y;
	}
	TaskRect(float _l, float _t, float _r, float _b)
	{
		l = _l;
		t = _t;
		r = _r;
		b = _b;
	}
	bool InRect(const TaskVec& v) const
	{
		return v.x >= l && v.x <= r && v.y >= b && v.y <= t;
	}
	TaskVec Clamp(const TaskVec& v) const
	{
		return TaskVec(_clampf(v.x, l, r), _clampf(v.y, b, t));
	}
};

struct TaskActZone
{
	enum ZoneType
	{
		enumFSZone,
		enumFMZone,
		enumMSZone,
		enumMMZone,
		enumBSZone,
		enumBMZone
	};

protected:
	ZoneType mZoneType;

public:
	bool InZone(const TaskVec& v) const;
	ZoneType GetType() const { return mZoneType; }
};

enum TaskPlayerType
{
	enumTL,
	enumTR,
	enumTML,
	enumTMR
};

enum TaskState
{
	enumStateNormal,
	enumStateFr
};

static const TaskRect _zone_border(
	0,
	static_cast<float>(_ZONE_HEIGHT),
	static_cast<float>(_ZONE_WIDTH),
	static_cast<float>(0));

const TaskActZone gZones[6];

inline const TaskActZone* DecideZone(const TaskVec& v)
{
	for (int i = 0; i < sizeof(gZones) / sizeof(TaskActZone); i++)
		if (gZones[i].InZone(v))
			return &gZones[i];

	assert(false);
	return NULL;
}

class TaskPlayerInfo;
class TaskTeamInfo;
class TaskPlayerStrategy;
class TaskGameInfo;

enum TaskStrategyType
{
	enumStraDisabled,
	enumStraAcc,
	enumStraDeAcc,
	enumStraCtrlB,
	enumStraPss,
	enumStraTckl,
	enumStraTraceB,
	enumGStraFrB,
};

class TaskStrategy
{
public:
	TaskStrategy(TaskStrategyType type) : mType(type) {}
	virtual ~TaskStrategy() {}

protected:
	TaskStrategyType mType;

public:
	virtual bool Tick(float fTickTime) = 0;
	TaskStrategyType GetType() const { return mType; }
};

class TaskGInfoStrategy : public TaskStrategy
{
public:
	TaskGInfoStrategy(TaskStrategyType type) : TaskStrategy(type) {}
	virtual ~TaskGInfoStrategy() {}

protected:
	TaskGameInfo* mGInfo;

public:
	void SetGInfo(TaskGameInfo* pInfo) { mGInfo = pInfo; }
};

class TaskGFreeBStrategy : public TaskGInfoStrategy
{
public:
	TaskGFreeBStrategy() : TaskGInfoStrategy(enumGStraFrB) {}
	~TaskGFreeBStrategy() {}

public:
	virtual bool Tick(float fTickTime);
	void Init(const TaskVec& vDir, float fSpUp);
};

class TaskPlayerStrategy : public TaskStrategy
{
public:
	TaskPlayerStrategy(TaskStrategyType type) : TaskStrategy(type) {}
	virtual ~TaskPlayerStrategy() {}

protected:
	TaskPlayerInfo* mPlayer;

public:
	void SetPlayer(TaskPlayerInfo* pInfo) { mPlayer = pInfo; }
};

class TaskAccStrategy : public TaskPlayerStrategy
{
public:
	TaskAccStrategy() : TaskPlayerStrategy(enumStraAcc) {}
	~TaskAccStrategy() {}

protected:
	const TaskVec* mpDir;

public:
	virtual bool Tick(float fTickTime);
	void SetDir(const TaskVec& dir) { mpDir = &dir; }
};

class TaskDeAccStrategy : public TaskPlayerStrategy
{
public:
	TaskDeAccStrategy() : TaskPlayerStrategy(enumStraDeAcc) {}
	~TaskDeAccStrategy() {}

public:
	virtual bool Tick(float fTickTime);
};

class TaskCtrlBStrategy : public TaskPlayerStrategy
{
public:
	TaskCtrlBStrategy() : TaskPlayerStrategy(enumStraCtrlB) {}
	~TaskCtrlBStrategy() {}

protected:
	TaskVec mDir;
	float mTime;

public:
	virtual bool Tick(float fTickTime);
	void SetTarget(const TaskVec& v);
};

class TaskPssStrategy : public TaskPlayerStrategy
{
public:
	TaskPssStrategy() : TaskPlayerStrategy(enumStraPss) {}
	~TaskPssStrategy() {}

protected:
	TaskPlayerInfo* mTarget;

public:
	virtual bool Tick(float fTickTime);
	void SetTarget(TaskPlayerInfo* p);
};

class TaskDisabledStrategy : public TaskPlayerStrategy
{
public:
	TaskDisabledStrategy() : TaskPlayerStrategy(enumStraDisabled) {}
	~TaskDisabledStrategy() {}

protected:
	float mTime;

public:
	virtual bool Tick(float fTickTime);
	void SetTime(float fTime) { mTime = fTime; }
};

class TaskTcklStrategy : public TaskPlayerStrategy
{
public:
	TaskTcklStrategy() : TaskPlayerStrategy(enumStraTckl) {}
	~TaskTcklStrategy() {}

protected:
	float mTime;

public:
	virtual bool Tick(float fTickTime);
	void SetTime(float fTime) { mTime = fTime; }
};

class TaskTraceBStrategy : public TaskPlayerStrategy
{
public:
	TaskTraceBStrategy() :
	TaskPlayerStrategy(enumStraTraceB),
	mInSpot(false) {}
	~TaskTraceBStrategy() {}

protected:
	bool mInSpot;

public:
	virtual bool Tick(float fTickTime);
};

class TaskPlayerInfo
{
public:
	TaskPlayerInfo()
	{
		mSp		= 10.0f;
		mCurSp	= 0;
		mCtrlB	= false;
	}

	~TaskPlayerInfo() { delete mCurStrategy; }

protected:
	TaskTeamInfo* mTeam;
	TaskPlayerStrategy* mCurStrategy;
	TaskPlayerInfo* mOpponent;
	float mOppDist;
	TaskPlayerType mType;
	TaskVec mDefPos;
	TaskVec mPos;
	const TaskVec* mpDir;
	long mBindId;
	long mInput;
	int mIndex;
	bool mCtrlB;

	bool mIsGK;
	float mSp;
	float mSta;
	float mPas;
	float mTac;
	float mSho;

	float mCurSp;

	friend class TaskPlayerStrategy;
	friend class TaskRunStrategy;

public:
	void SetIndex(int nIndex) { mIndex = nIndex; }
	int GetIndex() const { return mIndex; }
	void BindTeam(TaskTeamInfo* pTeam) { mTeam = pTeam; }
	TaskTeamInfo* GetTeam() { return mTeam; }
	void SetOpp(TaskPlayerInfo* p) { mOpponent = p; }
	TaskPlayerInfo* GetOpp() { return mOpponent; }
	void SetOppDist(float fDist) { mOppDist = fDist; }
	float GetOppDist() const { return mOppDist; }
	void SetPos(const TaskVec& pos) { mPos = pos; }
	const TaskVec& GetPos() const { return mPos; }
	void SetDir(const TaskVec& vDir) { mpDir = &vDir; }
	void SetDefPos(const TaskVec& pos) { mDefPos = pos; }
	const TaskVec& GetDefPos() const { return mDefPos; }
	void SetCtrlB(bool b) { mCtrlB = b; }
	bool GetCtrlB() const { return mCtrlB; }
	float GetSp() const { return mSp; }
	float GetCurSp() const { return mCurSp; }
	void SetCurSp(float f) { mCurSp = f; }

	void ActiveTick(float fTickTime);
	void TeamMemTick(float fTickTime);
	void DefTick(float fTickTime);
	void SetDestPos(const TaskVec& vDir, float fTickTime)
	{
		float f = mCurSp + sAcc * fTickTime;
		if (f > mSp) f = mSp;
		float fDist = (f + mSp) * .5f * fTickTime;
		mSp = f;
		mPos = _zone_border.Clamp(vDir * fDist + mPos);
		mpDir = &vDir;
	}
	bool DeAcc(float fTickTime)
	{
		bool bRet = false;
		float f = mCurSp - sDeAcc * fTickTime;

		if (f <= 0)
		{
			f = 0;
			bRet = true;
		}

		float fDist = (f + mSp) * .5f * fTickTime;
		mSp = f;
		mPos = _zone_border.Clamp(*mpDir * fDist + mPos);
		return bRet;
	}
	void SetCtrlBScheme(const TaskVec& vPos)
	{
		if (mCurStrategy) delete mCurStrategy;
		mCurStrategy = new TaskCtrlBStrategy;
		static_cast<TaskCtrlBStrategy*>(mCurStrategy)->SetPlayer(this);
		static_cast<TaskCtrlBStrategy*>(mCurStrategy)->SetTarget(vPos);
	}
	void SetPssScheme()
	{
		if (mCurStrategy) delete mCurStrategy;
		mCurStrategy = new TaskPssStrategy;
		static_cast<TaskPssStrategy*>(mCurStrategy)->SetPlayer(this);
		static_cast<TaskPssStrategy*>(mCurStrategy)->Init();
	}
	void SetTraceScheme()
	{
		if (mCurStrategy) delete mCurStrategy;
		mCurStrategy = new TaskTraceBStrategy;
		static_cast<TaskTraceBStrategy*>(mCurStrategy)->SetPlayer(this);
	}

	void TakeCtrlB();
	bool CanTckl();
	void SetDisabled();
	bool IsDisabled() const
	{
		if (!mCurStrategy) return false;
		return mCurStrategy->GetType() == enumStraDisabled;
	}
	void CalcOppDir(TaskVec& dir);
};

class TaskTeamInfo
{
public:
	TaskTeamInfo() { mActPos = -1; }
	~TaskTeamInfo() {}

protected:
	TaskGameInfo* mGame;
	TaskPlayerInfo* mPlayers[TASK_TEAM_MEM_NUM];
	TaskTeamInfo* mOpponent;
	int mIndex;
	TaskVec mHostPos;
	TaskVec mTargetPos;
	TaskVec mAttDir;
	TaskVec mRight;
	TaskRect mFZone;
	int mActPos;

protected:
	void CalcOpps();

public:
	// Properties

	void SetIndex(int n) { mIndex = n; }
	int GetIndex() const { return mIndex; }
	void InitPos(const TaskVec& host, const TaskVec& target)
	{
		mHostPos = host;
		mTargetPos = target;
		mAttDir = mTargetPos - mHostPos;
		mAttDir.Normalize();
		mRight = RotVec(mAttDir, _PI * .5f);
	}
	const TaskVec& GetHostPos() const { return mHostPos; }
	const TaskVec& GetTargetPos() const { return mTargetPos; }
	const TaskVec& GetAttDir() const { return mAttDir; }
	void SetGame(TaskGameInfo* p) { mGame = p; }
	TaskGameInfo* GetGame() { return mGame; }
	TaskTeamInfo* GetOpponent() { return mOpponent; }
	void SetOpponent(TaskTeamInfo* p) { mOpponent = p; }
	void SetPlayer(int nIndex, TaskPlayerInfo* pInfo)
	{
		pInfo->BindTeam(this);
		mPlayers[nIndex] = pInfo;
	}
	TaskPlayerInfo* GetPlayer(int nIndex) { return mPlayers[nIndex]; }
	void SetActPlayer(int nAct) { mActPos = nAct; }
	TaskPlayerInfo* GetActPlayer() { return mPlayers[mActPos]; }
	bool InHalf(const TaskVec& v) const { return fabs(v.y - mHostPos.y) < _ZONE_HEIGHT / 2.0f; }

	// Operations

	void ActiveTick(float fTickTime);
	void DefTick(float fTickTime);
	void SetTraceScheme();
};

class TaskGameInfo
{
public:
	TaskGameInfo() :
	 mState(enumStateNormal),
	 mCurStrategy(0) {}
	~TaskGameInfo() {}

protected:
	TaskTeamInfo* mTeams[TASK_TEAM_NUM];
	int mActTeam;
	TaskVec mBPos;
	float mBHei;
	TaskVec mBDir;
	float mBSp;
	float mBSpUp;
	TaskState mState;
	TaskGInfoStrategy* mCurStrategy;	

protected:
	void Swap()
	{
		TaskTeamInfo* p = mTeams[0];
		mTeams[0] = mTeams[1];
		mTeams[1] = p;
	}

public:
	// Properties
	const TaskVec& GetBPos() const { return mBPos; }
	void SetBPos(const TaskVec& pos) { mBPos = pos; }
	const TaskVec& GetBDir() const { return mBDir; }
	void SetBDir(const TaskVec& dir) { mBDir = dir; }
	float GetBHei() const { return mBHei; }
	void SetBHei(float fHei) { mBHei = fHei; }
	float GetBSp() const { return mBSp; }
	void SetBSp(float f) { mBSp = f; }
	float GetBSpUp() const { return mBSpUp; }
	void SetBSpUp(float f) { mBSpUp = f; }
	void SetTeams(TaskTeamInfo* t1, TaskTeamInfo* t2)
	{
		t1->SetGame(this);
		t2->SetGame(this);
		t1->SetOpponent(t2);
		t2->SetOpponent(t1);
		mTeams[0] = t1;
		t1->SetIndex(0);
		mTeams[1] = t2;
		t2->SetIndex(1);
	}
	TaskTeamInfo* GetTeam(int nIndex) { return mTeams[nIndex]; }
	TaskTeamInfo* GetActTeam() { return mTeams[mActTeam]; }
	TaskPlayerInfo* GetActPlayer() { return mTeams[mActTeam]->GetActPlayer(); }
	void SetActTeam(int nIndex) { mActTeam = nIndex; }
	void SetState(TaskState s) { mState = s; }
	TaskState GetState() const { return mState; }

	// Operations
	void SetUpBegin();
	void Tick(float fTickTime);
	void SetFrBScheme(const TaskVec& vDir, float fSpUp);
};

// Funcs

void TaskGFreeBStrategy::Init(const TaskVec& vDir, float fSpUp)
{
	mGInfo->SetBDir(vDir);
	mGInfo->SetBSp(sBlSp);
	mGInfo->SetBHei(0);
	mGInfo->SetBSpUp(fSpUp);
}

bool TaskGFreeBStrategy::Tick(float fTickTime)
{
	float fCurSpUp = mGInfo->GetBSpUp() - sGv * fTickTime;
	float fOff = (fCurSpUp + mGInfo->GetBSpUp()) * .5f * fTickTime;
	float fCurHei = mGInfo->GetBHei() + fOff;

	if (fCurHei <= 0)
	{
	}
}

bool TaskAccStrategy::Tick(float fTickTime)
{
	mPlayer->SetDestPos(*mpDir, fTickTime);
	return false;
}

bool TaskDeAccStrategy::Tick(float fTickTime)
{
	return mPlayer->DeAcc(fTickTime);
}

void TaskCtrlBStrategy::SetTarget(const TaskVec& v)
{
	TaskVec vTarget = _zone_border.Clamp(v);
	mDir = vTarget - mPlayer->GetPos();
	float fDist = mDir.Normalize();
	mTime = fDist / mPlayer->GetSp();
}

bool TaskCtrlBStrategy::Tick(float fTickTime)
{
	bool bRet;

	if (mTime <= fTickTime)
	{
		fTickTime = mTime;
		bRet = true;
	}
	else
	{
		mTime -= fTickTime;
		bRet = false;
	}

	mPlayer->SetDestPos(mDir, fTickTime);

	if (mPlayer->GetCtrlB())
	{
		TaskGameInfo* p = mPlayer->GetTeam()->GetGame();
		p->SetBPos(mPlayer->GetPos());
		p->SetBDir(mDir);
	}

	return bRet;
}

void TaskPssStrategy::SetTarget(TaskPlayerInfo* p)
{
	mTarget = p;
	float fOff = _UnitRandom() * sPssRadius;
	TaskVec vDir = RotVec(sUp, _UnitRandom() * (2.0f * _PI));
	TaskVec vTarget = vDir * fOff + p->GetPos();
	TaskVec vBDir = vTarget - mPlayer->GetPos();
	float fDist = vDir.Normalize();
	float fTime = fDist / sBlSp;
	float fSpUp = .5f * fTime * sGv;
}

bool TaskPssStrategy::Tick(float fTickTime)
{
	if (mTime <= fTickTime)
	{
		mPlayer->GetTeam()->SetActPlayer(mPlayer->GetIndex());
		mPlayer->GetTeam()->GetGame()->SetBPos(mPlayer->GetPos());
		mPlayer->SetCtrlB(true);
		return true;
	}
	else
	{
		mTime -= fTickTime;
		mPlayer->GetTeam()->GetGame()->SetBPos(mDir * (sBlSp * fTickTime) + mPlayer->GetTeam()->GetGame()->GetBPos());
		return false;
	}
}

bool TaskDisabledStrategy::Tick(float fTickTime)
{
	if (fTickTime >= mTime)
		return true;
	else
	{
		mTime -= fTickTime;
		return false;
	}
}

bool TaskTcklStrategy::Tick(float fTickTime)
{
	if (mTime > fTickTime)
	{
		mTime -= fTickTime;
		return false;
	}

	if (!mPlayer->GetOpp()->GetCtrlB())
		return true;

	if (_UnitRandom() < .7f)
	{
		mPlayer->GetOpp()->SetCtrlB(false);
		mPlayer->GetOpp()->SetDisabled();

		if (_UnitRandom() < .4f)
			mPlayer->TakeCtrlB();
		else
		{
			TaskVec vDir = mPlayer->GetPos() - mPlayer->GetOpp()->GetPos();
			vDir.Normalize();
			float fAngle = _SymmetricRandom() * (_PI / 2);
			vDir = RotVec(vDir, fAngle);
			mPlayer->GetTeam()->GetGame()->SetFrBScheme(vDir);
		}
	}

	return true;
}

bool TaskTraceBStrategy::Tick(float fTickTime)
{
	TaskGameInfo* pInfo = mPlayer->GetTeam()->GetGame();

	if (pInfo->GetState() != enumStateFr)
		return true;

	if (mInSpot)
	{
		float fDist = (pInfo->GetBPos() - mPlayer->GetPos()).Magnitude();

		if (fDist < sCtrlBRadius)
		{
			pInfo->SetState(enumStateNormal);
			pInfo->SetActTeam(mPlayer->GetTeam()->GetIndex());
			mPlayer->GetTeam()->SetActPlayer(mPlayer->GetIndex());
			mPlayer->SetCtrlB(true);
			pInfo->SetBPos(mPlayer->GetPos());
			return true;
		}
		else
			return false;
	}
	else
	{
		TaskVec vDir = pInfo->GetBTarget() - mPlayer->GetPos();
		float fDist = vDir.Normalize();
		float f = mPlayer->GetSp() * fTickTime;

		if (fDist <= f)
		{
			mPlayer->SetPos(pInfo->GetBTarget());
			mInSpot = true;
		}
		else
			mPlayer->SetPos(vDir * fDist + mPlayer->GetPos());

		return false;
	}
}

void TaskPlayerInfo::ActiveTick(float fTickTime)
{
	if (mCurStrategy && mCurStrategy->Tick(fTickTime))
	{
		delete mCurStrategy;
		mCurStrategy = NULL;
	}
}

void TaskPlayerInfo::TeamMemTick(float fTickTime)
{
	if (mCurStrategy && !mCurStrategy->Tick(fTickTime))	return;
	delete mCurStrategy;
	TaskAccStrategy* p = new TaskAccStrategy;
	p->SetPlayer(this);
	p->SetDirAndTime(mTeam->GetGame()->GetBDir(), sPerActTime);
	mCurStrategy = p;
}

void TaskPlayerInfo::CalcOppDir(TaskVec& dir)
{
	dir = mTeam->GetHostPos() - mOpponent->GetPos();
	dir.Normalize();
	TaskVec vTarget = mOpponent->GetPos() + (dir * sOppRadius);
	dir = vTarget - GetPos();
	dir.Normalize();
}

void TaskPlayerInfo::DefTick(float fTickTime)
{
	if (mCurStrategy && !mCurStrategy->Tick(fTickTime))	return;
	delete mCurStrategy;

	if (CanTckl())
	{
		TaskTcklStrategy* p = new TaskTcklStrategy;
		p->SetPlayer(this);
		p->SetTime(1.5f * sPerActTime);
	}
	else
	{
		TaskAccStrategy* p = new TaskAccStrategy;
		p->SetPlayer(this);
		mCurStrategy = p;
		TaskVec dir;

		if (mTeam->InHalf(mOpponent->GetPos()) || mOpponent->GetCtrlB())
			CalcOppDir(dir);
		else
		{
			dir = mDefPos - mPos;

			if (dir.y * mTeam->GetAttDir().y < 0)
				dir.Normalize();
			else
				CalcOppDir(dir);
		}

		p->SetDirAndTime(dir, sPerActTime);
	}
}

void TaskPlayerInfo::TakeCtrlB()
{
	mTeam->GetGame()->SetActTeam(mTeam->GetIndex());
	mTeam->SetActPlayer(mIndex);
	SetCtrlB(true);
}

bool TaskPlayerInfo::CanTckl()
{
	if (!mOpponent->GetCtrlB() || mOpponent->GetOppDist() > sTcklRadius)
		return false;

	return true;
}

void TaskPlayerInfo::SetDisabled()
{
	delete mCurStrategy;
	mCurStrategy = new TaskDisabledStrategy;
	static_cast<TaskDisabledStrategy*>(mCurStrategy)->SetTime(2.0f * sPerActTime);
}

int _opp_cmp(const void* p1, const void* p2);

struct _opp_dist
{
	int p1;
	int p2;
	float dist;
};

void TaskTeamInfo::CalcOpps()
{
	int i, j;

	_opp_dist opp_list[TASK_TEAM_MEM_NUM * TASK_TEAM_MEM_NUM];
	int nCount = 0;

	for (i = 0; i < TASK_TEAM_MEM_NUM; i++)
	{
		for (j = 0; j < TASK_TEAM_MEM_NUM; j++)
		{
			_opp_dist& o = opp_list[nCount];
			o.p1 = i;
			o.p2 = j;
			o.dist = (mPlayers[i]->GetPos() - mOpponent->mPlayers[j]->GetPos()).Magnitude();
			nCount++;
		}
	}

	qsort(
		opp_list,
		sizeof(opp_list) / sizeof(_opp_dist),
		sizeof(_opp_dist),
		_opp_cmp);

	nCount = 0;

	for (i = 0; i < sizeof(opp_list) / sizeof(_opp_dist); i++)
	{
		_opp_dist& o = opp_list[i];
		TaskPlayerInfo*& p1 = mPlayers[o.p1];
		TaskPlayerInfo*& p2 = mOpponent->mPlayers[o.p2];

		if (p1->GetOpp() || p2->GetOpp())
			continue;

		p1->SetOpp(p2);
		p1->SetOppDist(o.dist);
		p2->SetOpp(p1);
		p2->SetOppDist(o.dist);

		if (++nCount == TASK_TEAM_MEM_NUM)
			break;
	}
}

static int _opp_cmp(const void* p1, const void* p2)
{
	if (static_cast<const _opp_dist*>(p1)->dist < static_cast<const _opp_dist*>(p2)->dist)
		return -1;
	else
		return 1;
}

void TaskTeamInfo::ActiveTick(float fTickTime)
{
	if (mActPos >= 0) mPlayers[mActPos]->ActiveTick(fTickTime);

	for (int i = 0; i < TASK_TEAM_MEM_NUM; i++)
	{
		if (i == mActPos)
			continue;
		else
			mPlayers[i]->TeamMemTick(fTickTime);
	}
}

void TaskTeamInfo::DefTick(float fTickTime)
{
	for (int i = 0; i < TASK_TEAM_MEM_NUM; i++)
		mPlayers[i]->DefTick(fTickTime);
}

void TaskTeamInfo::SetTraceScheme()
{
	float fDist = 1e6;
	int nIndex = -1;

	for (int i = 0; i < TASK_TEAM_MEM_NUM; i++)
	{
		TaskPlayerInfo* p = mPlayers[i];
		if (p->IsDisabled()) continue;
		float f = (p->GetPos() - mGame->GetBPos()).Magnitude();

		if (f < fDist)
		{
			nIndex = i;
			fDist = f;
		}
	}

	if (nIndex >= 0) mPlayers[nIndex]->SetTraceScheme();
}

void TaskGameInfo::SetFrBScheme(const TaskVec& vDir, float fSpUp)
{
	mState = enumStateFr;
	delete mCurStrategy;
	mCurStrategy = new TaskGFreeBStrategy;
	static_cast<TaskGFreeBStrategy*>(mCurStrategy)->SetGInfo(this);
	static_cast<TaskGFreeBStrategy*>(mCurStrategy)->Init(vDir, fSp);
}

void TaskGameInfo::Tick(float fTickTime)
{
	if (mCurStrategy && mCurStrategy->Tick(fTickTime))
	{
		delete mCurStrategy;
		mCurStrategy = NULL;
	}

	mTeams[mActTeam]->ActiveTick(fTickTime);
	mTeams[TASK_TEAM_NUM-1-mActTeam]->DefTick(fTickTime);
}

void TaskGameInfo::SetUpBegin()
{
	static const TaskVec v1(_ZONE_WIDTH / 2, 0);
	static const TaskVec v2(_ZONE_WIDTH / 2, _ZONE_HEIGHT);

	mTeams[0]->InitPos(v1, v2);
	mTeams[0]->SetActPlayer(0);
	mTeams[1]->InitPos(v2, v1);

	GetActPlayer()->SetCtrlB(true);
	SetBPos(GetActPlayer()->GetPos());
	mBDir = TaskVec(0, 1.0f);
}

TaskPlayerInfo _players[TASK_TEAM_MEM_NUM * 2];
TaskTeamInfo _teams[2];
TaskGameInfo _info;

void _start_up()
{
	for (int i = 0; i < TASK_TEAM_MEM_NUM; i++)
	{
		_teams[0].SetPlayer(i, &_players[i]);
		_players[i].SetPos(TaskVec(10.0f + 40 * i, 50.0f));
		_players[i].SetDefPos(_players[i].GetPos());
		_players[i].SetIndex(i);
		_teams[1].SetPlayer(i, &_players[i+TASK_TEAM_MEM_NUM]);
		_players[i+TASK_TEAM_MEM_NUM].SetPos(TaskVec(10.0f + 40 * i, 150.0f));
		_players[i+TASK_TEAM_MEM_NUM].SetDefPos(_players[i+TASK_TEAM_MEM_NUM].GetPos());
		_players[i+TASK_TEAM_MEM_NUM].SetIndex(i);
	}

	_info.SetTeams(&_teams[0], &_teams[1]);
	_info.SetUpBegin();
}

TaskVec _trans_pos(const TaskVec& v, const RECT* pRect)
{
	TaskVec r(v.y / _ZONE_HEIGHT, v.x / _ZONE_WIDTH);
	r.x *= pRect->right - pRect->left;
	r.y *= pRect->bottom - pRect->top;
	return r;
}

TaskVec _invtrans_pos(float x, float y)
{
	return TaskVec(y * _ZONE_WIDTH, x * _ZONE_HEIGHT);
}

static DWORD _tick = 0;
static bool _init = false;

void _tick_up(float f)
{
	_info.Tick(f);
}

void _put_out(HDC hdc, RECT* pRect)
{
	CDC dc;
	dc.Attach(hdc);

	for (int i = 0; i < sizeof(_players) / sizeof(_players[0]); i++)
	{
		TaskVec v = _players[i].GetPos();
		v = _trans_pos(v, pRect);
		dc.FillSolidRect((int)v.x, (int)v.y, 1, 1, i >= TASK_TEAM_MEM_NUM ? RGB(0, 0, 0) : RGB(196, 196, 196));
	}

	COLORREF cl;

	if (_info.GetState() == enumStateFr)
		cl = RGB(0, 0, 255);
	else if (_info.GetActTeam()->GetIndex() == 0)
		cl = RGB(255, 0, 0);
	else
		cl = RGB(255, 255, 255);

	TaskVec b = _info.GetBPos();
	b = _trans_pos(b, pRect);
	dc.FillSolidRect((int)b.x, (int)b.y, 2, 2, cl);

	dc.Detach();
}

void _set_init() { _init = true; }
bool _is_f_init()
{
	return _init;
}

void on_m_active(float x, float y)
{
	TaskVec v = _invtrans_pos(x, y);
	_info.GetActPlayer()->SetCtrlBScheme(v);
}

void on_sel_change(int nSel)
{
	TaskPlayerInfo* p = _info.GetActTeam()->GetPlayer(nSel);
	if (p == _info.GetActPlayer()) return;
	_info.GetActPlayer()->SetCtrlB(false);
	p->SetPssScheme();
}

#else

void _set_init() {}
void _start_up() {}
bool _is_f_init() { return false; }
void _tick_up(float f) {}
void _put_out(HDC hdc, RECT* pRect) {}
void on_m_active(float x, float y) {}
void on_sel_change(int nSel) {}

#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskAreaDlg message handlers

BOOL CTaskAreaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
