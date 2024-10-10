
#ifndef _EC_SHAKECAMERA_H_
#define _EC_SHAKECAMERA_H_


#include <ABaseDef.h>
#include <A3DCamera.h>
class A3DGFXCtrlNoiseBase;
class ShakeAffector;


class CECShakeCameraBase
{
public:
	CECShakeCameraBase() { Reset(); }

	static CECShakeCameraBase* CreateShakeCam(DWORD dwPeriod, bool bShakeByDistance);

	explicit CECShakeCameraBase(DWORD dwPeriod, bool bShakebyDistance);
	virtual ~CECShakeCameraBase() = 0 {}

	virtual bool Tick(DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam);
	virtual void Stop() = 0;
	virtual A3DVECTOR3 GetShakeOffset() const = 0;

	inline void Reset()
	{
		m_bShaking		= false;
		m_vShakeOffset.Clear();
	}

	virtual void Start() = 0;
	
	void SetShakePeriod( DWORD dwPeriod ) { m_dwPeriod = dwPeriod; }
	DWORD GetShakePeriod() const { return m_dwPeriod;}
	 
	void SetShakeByDistance( bool bShakeByDis ) { m_bShakeByDistance = bShakeByDis; }
	bool GetShakeByDistance() const { return m_bShakeByDistance; }

	float GetTimeSpan() { return m_dwTimeSpan / 1000.0f;}

protected:
	
	inline bool GetShaking() const { return m_bShaking; }
	inline void SetShaking(bool bShaking) { m_bShaking = bShaking; }

protected:
	
	A3DVECTOR3	m_vShakeOffset;

protected:
	
	A3DGFXCtrlNoiseBase* m_pShakeNoise;
	ShakeAffector* m_pAffector;

	int m_shakeType;

	DWORD m_dwTimeSpan;

	//	Runtime properties
	bool		m_bShaking;	
	DWORD		m_dwPeriod;				//	shake camera (cycle) period by millisecond
	bool		m_bShakeByDistance;		//	shake camera attenuation by distance
};

class CECShakeCamera : public CECShakeCameraBase
{
public:

	explicit CECShakeCamera(DWORD dwPeriod, bool bShakebyDistance);
	virtual ~CECShakeCamera();

protected:
	
	virtual bool Tick(DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam);
	virtual void Start();
	virtual void Stop();
	virtual A3DVECTOR3 GetShakeOffset() const;

private:

	int				m_nShakeCount;

};

class CECShakeCamera2 : public CECShakeCameraBase
{
public:
	
	explicit CECShakeCamera2(DWORD dwPeriod, bool bShakebyDistance);
	virtual ~CECShakeCamera2();

public:
	
	virtual bool Tick(DWORD dwTickTime, A3DCamera* pCamera, float fDistToCam);
	virtual void Start();
	void Stop();
	A3DVECTOR3 GetShakeOffset() const;

private:
	
	A3DVECTOR3		m_vLastShakeOffset;
	A3DVECTOR3		m_vTotalShakeOffset;
	DWORD			m_dwShakeTime;
	A3DVECTOR3		m_vDir;
	int				m_iShakeCycleCnt;		//周期计数
	DWORD			m_dwHalfShakeTime;		//判断半周期
	bool			m_bCalcNewDir;
};

#endif