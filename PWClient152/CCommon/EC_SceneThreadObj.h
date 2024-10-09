#pragma once

#define SCENE_OBJ_STATE_LOADED		0x00000001
#define SCENE_OBJ_STATE_TO_RELEASE	0x00000002

class CECSceneThreadObj
{
public:
	CECSceneThreadObj() : m_dwFlag(0), m_fDistToHost(0) {}
	virtual ~CECSceneThreadObj() {}

private:
	volatile unsigned long m_dwFlag;
	float m_fDistToHost;

public:
	bool IsLoaded() const { return (m_dwFlag & SCENE_OBJ_STATE_LOADED) != 0; }
	void SetLoaded() { m_dwFlag |= SCENE_OBJ_STATE_LOADED; }
	bool IsToRelease() const { return (m_dwFlag & SCENE_OBJ_STATE_TO_RELEASE) != 0; }
	void SetToRelease() { m_dwFlag |= SCENE_OBJ_STATE_TO_RELEASE; }
	void SetDistToHost(float fDist) { m_fDistToHost = fDist; }
	float GetDistToHost() const { return m_fDistToHost; }

	// virtual func
	virtual void LoadInThread(bool bInLoaderThread) {}
	virtual void ReleaseInThread() {}
	virtual A3DVECTOR3 GetLoadPos() { return A3DVECTOR3(0); }
};
