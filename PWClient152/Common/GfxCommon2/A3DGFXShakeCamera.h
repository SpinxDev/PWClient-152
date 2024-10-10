#ifndef _A3DGFXSHAKECAMERA_H_
#define _A3DGFXSHAKECAMERA_H_

#include "GfxCommonTypes.h"

class A3DGFXEx;
class A3DGFXCtrlNoiseBase;
class A3DCamera;

enum SHAKECAM
{
	SHAKECAM_NONE		= 0,
	SHAKECAM_DIR_RAND	= 1,
	SHAKECAM_DIR_UPDOWN = 2,
	SHAKECAM_DIR_CAMERA	= 3,
	SHAKECAM_NUM,
};


A3DVECTOR3 GenerateDirection(SHAKECAM sc);

class ShakeAffector
{
public:
	enum 
	{
		SA_NONE = 0,
		SA_DAMPING_LINEAR,
		SA_DAMPING_QUADRATIC,
	}; 

	explicit ShakeAffector(const A3DGFXCtrlNoiseBase* pShakeNoise, int iType)
		: m_iType(iType)
		, m_pShakeNoise(pShakeNoise)
	{

	}

	int GetTypeId() const { return m_iType; }
	virtual void AffectShakeOffset(float fTimeSpan, A3DVECTOR3& vCurShake) = 0;

	static ShakeAffector* CreateAffector(int iType, const A3DGFXCtrlNoiseBase* pShakeNoise);

protected:
	int m_iType;
	const A3DGFXCtrlNoiseBase* m_pShakeNoise;
};

class IShakeCamera
{
public:
	IShakeCamera() : m_pGfx(NULL), m_dwPeriod(0), m_bShakeByDistance(false), m_bEnableBlur(false), m_fShakeAttenuDist(30.f) { Reset(); }

	static IShakeCamera* LoadFromFile(AFile* pFile, DWORD dwVersion, A3DGFXEx* pGfx);
	static void SaveToFile(IShakeCamera* pShakeCamera, AFile* pFile);
	static IShakeCamera* CreateShakeCam(A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakeByDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist);

	explicit IShakeCamera(A3DGFXEx* pGFXEx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist);
	virtual ~IShakeCamera() = 0 {}

	virtual void Tick(DWORD dwTickTime, DWORD dwTimeSpan, A3DCamera* pCamera, float fDistToCam) = 0;
	virtual void Stop() = 0;
	virtual A3DVECTOR3 GetShakeOffset() const = 0;
	
	IShakeCamera* Clone(A3DGFXEx* pGfx) const
	{
		return CreateShakeCam(pGfx, m_dwPeriod, m_bShakeByDistance, m_CameraBlurInfo, m_fShakeAttenuDist);
	}

	inline void Reset()
	{
		m_bShaking		= false;
		m_vShakeOffset.Clear();
	}

	inline void Start()
	{
		m_bShaking = false;
	}
	
	void SetShakePeriod( DWORD dwPeriod ) { m_dwPeriod = dwPeriod; }
	DWORD GetShakePeriod() const { return m_dwPeriod;}
	 
	void SetShakeByDistance( bool bShakeByDis ) { m_bShakeByDistance = bShakeByDis; }
	bool GetShakeByDistance() const { return m_bShakeByDistance; }

	bool IsEnableCamBlur() const { return m_bEnableBlur; }
	void SetEnableCamBlur(bool b) { m_bEnableBlur = b; }

	void SetCameraBlurInfo(const A3DGFX_CAMERA_BLUR_INFO& info) { m_CameraBlurInfo = info; }
	const A3DGFX_CAMERA_BLUR_INFO& GetCameraBlurInfo() const { return m_CameraBlurInfo; }

	void SetAttenuDist(float fDist) { m_fShakeAttenuDist = fDist; }
	float GetAttenuDist() const { return m_fShakeAttenuDist; }
	float CalcDistCo(float fDistToCam) { return 1.0f - fDistToCam / m_fShakeAttenuDist; }

protected:
	
	inline A3DGFXEx* GetGfx() const { return m_pGfx; }
	inline bool GetShaking() const { return m_bShaking; }
	inline void SetShaking(bool bShaking) { m_bShaking = bShaking; }

protected:
	
	A3DVECTOR3	m_vShakeOffset;

private:
	
	A3DGFXEx*	m_pGfx;
	bool		m_bEnableBlur;

	//	Runtime properties
	bool		m_bShaking;	
	DWORD		m_dwPeriod;				//	shake camera (cycle) period by millisecond
	bool		m_bShakeByDistance;		//	shake camera attenuation by distance

	A3DGFX_CAMERA_BLUR_INFO m_CameraBlurInfo;
	float		m_fShakeAttenuDist;
};

class ShakeCamera : public IShakeCamera
{
public:

	explicit ShakeCamera(A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist);
	virtual ~ShakeCamera();

protected:
	
	virtual void Tick(DWORD dwTickTime, DWORD dwTimeSpan, A3DCamera* pCamera, float fDistToCam);
	virtual void Stop();
	virtual A3DVECTOR3 GetShakeOffset() const;

private:

	int				m_nShakeCount;
};

class ShakeCamera2 : public IShakeCamera
{
public:
	
	explicit ShakeCamera2(A3DGFXEx* pGfx, DWORD dwPeriod, bool bShakebyDistance, const A3DGFX_CAMERA_BLUR_INFO& info, float fShakeAttenuDist);
	virtual ~ShakeCamera2();

public:
	
	void Tick(DWORD dwTickTime, DWORD dwTimeSpan, A3DCamera* pCamera, float fDistToCam);
	void Stop();
	A3DVECTOR3 GetShakeOffset() const;

private:
	
	A3DVECTOR3		m_vLastShakeOffset;
	A3DVECTOR3		m_vTotalShakeOffset;
	DWORD			m_dwShakeTime;
	A3DVECTOR3		m_vDir;
	int				m_iShakeCycleCnt;		//周期计数
	DWORD			m_dwHalfShakeTime;		//判断半周期
};

#endif