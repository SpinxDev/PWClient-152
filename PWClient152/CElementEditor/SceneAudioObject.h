// SceneAudioObject.h: interface for the CSceneAudioObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEAUDIOOBJECT_H__967A1B24_E43F_4C8B_971C_4A924819B80C__INCLUDED_)
#define AFX_SCENEAUDIOOBJECT_H__967A1B24_E43F_4C8B_971C_4A924819B80C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

class AM3DSoundBuffer;
class A3DViewport;

struct GFX_AUDIO_DATA
{
	float		m_fMinDist;	//最小距离
	float		m_fMaxDist;	//最大距离
	int			m_nGroup;	//播放组
	int         m_nAudioTypes; 
	AString		m_strPathName;//声音文件路径名
};

class CSceneAudioObject  :public CSceneObject
{
public:
	enum
	{
		AUDIO_TYPE_DAY = 0,
		AUDIO_TYPE_NIGHT,
		AUDIO_TYPE_BOTH,
	};

	CSceneAudioObject();
	
	virtual ~CSceneAudioObject();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void EntryScene();
	virtual void LeaveScene();
	//  Copy object
	virtual CSceneObject*		CopyObject();
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	GFX_AUDIO_DATA &GetProperty(){ return m_auData; };
	void SetProperty( const GFX_AUDIO_DATA& data){ m_auData = data; };

	void ReBuildAudio();

	void SetEnableAudio(bool bEnableAudio){ m_bEnableAudio = bEnableAudio; };
	void BuildProperty();
	
	// Used for copy object
	void ResetSoundBuffer(){ m_pAM3DSoundBuffer = NULL; };
private:
	void DrawCircle(DWORD clr);
	void DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	void FadeSound(AM3DSoundBuffer * pSound, bool bFadeIn);

	GFX_AUDIO_DATA m_auData;
	
	float m_fScale;
	AM3DSoundBuffer *m_pAM3DSoundBuffer;
	bool m_bEnableAudio;
	bool m_bIsPlaying;
	ASetTemplate<int> audio_type;
};

typedef CSceneAudioObject* PSCENEAUIDOOBJECT;

#endif // !defined(AFX_SCENEAUDIOOBJECT_H__967A1B24_E43F_4C8B_971C_4A924819B80C__INCLUDED_)
