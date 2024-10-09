// Filename	: EC_CameraCtrl.h
// Creator	: Tom Zhou
// Date		: September 21, 2004

#ifndef _CECCAMERA_H_
#define _CECCAMERA_H_

#include "ABaseDef.h"
#include "A3DVector.h"

#define CECCAMERA_MAX_USERDEFINE	3
#define CECCAMERA_MIN_DISTANCE		0.0f
#define CECCAMERA_MAX_DISTANCE		20.0f

class CECCamera
{
public:
	enum
	{
		CAMERA_POS_GROUND = 0,
		CAMERA_POS_AIR,
		CAMERA_POS_WATER
	};

	enum
	{
		CAMERA_MODE_NEAR = 0,
		CAMERA_MODE_MIDDLE,
		CAMERA_MODE_FAR,
		CAMERA_MODE_MAX
	};

	typedef struct _INIT_DATA
	{
		float fDistance;
		A3DVECTOR3 vecDirCam;
		A3DVECTOR3 vecUpCam;
	} INIT_DATA, * P_INIT_DATA;

	enum
	{
		ACTION_NONE = 0,
		ACTION_DEFAULT,
		ACTION_PRESET,
		ACTION_MAX
	};

	typedef struct _ACTION_DATA
	{
		int nAction;
		int nDeltaX;	// Mouse move x.
		int nDeltaY;	// Mouse move y.
		int nDeltaZ;	// Mouse wheel roll.
		A3DVECTOR3 vecDirChar;
	} ACTION_DATA, * P_ACTION_DATA;

	typedef struct _TICK_DATA
	{
		bool bWater;
		bool bTuning;
		bool bRunning;
		DWORD dwTime;		// Millisecond
		A3DVECTOR3 vecPosChar;
		A3DVECTOR3 vecDirChar;
		A3DVECTOR3 vecUpChar;
	} TICK_DATA, * P_TICK_DATA;

	typedef struct _ADJUST_DATA
	{
		A3DVECTOR3 vecPosCam;
		A3DVECTOR3 vecDirCam;
		A3DVECTOR3 vecUpCam;
	} ADJUST_DATA, * P_ADJUST_DATA;

public:
	static A3DVECTOR3 m_vecAxisX, m_vecAxisY, m_vecAxisZ;

public:
	CECCamera();
	virtual ~CECCamera();

	bool Init(P_INIT_DATA pInitData);
	bool Release();

	bool Action(P_ACTION_DATA pActionData);
	bool Tick(P_TICK_DATA pTickData, P_ADJUST_DATA pAdjustData);

	bool Fix_Get();
	void Fix_Set(bool bFix);
	bool SceneryMode_Get();
	void SceneryMode_Set(bool bSet);
	void UserView_Define(int nIndex);
	void UserView_Retrieve(int nIndex);
	int CameraPos_Get();
	void CameraPos_Set(int nPos);
	void SetVirtualKeyState(bool *bState);
	
	void SetDgree(float d) { m_fDegree = d;}
	void SetPitch(float p) { m_fPitch = p;}
	float GetDegree() const { return m_fDegree;}
	float GetPitch() const { return m_fPitch;} 

protected:
	bool m_bFixed;
	bool m_bSliding;
	int m_nCameraPos;
	int m_nCameraMode;
	bool m_bSceneryMode;
	A3DVECTOR3 m_vecDirCam, m_vecUpCam;
	float m_fPitch, m_fPitchDest;
	float m_fDegree, m_fDegreeDest;
	float m_fDistance, m_fDistanceDest;
	float m_fPitchUser[CECCAMERA_MAX_USERDEFINE];
	float m_fDistanceUser[CECCAMERA_MAX_USERDEFINE];
	bool m_bVirtualKeyState[7];

	bool UpdateDirAndUp();
};

typedef CECCamera * PCECCAMERA;

#endif //_CECCAMERA_H_
