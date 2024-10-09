// AudioAddOperation.h: interface for the CAudioAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOADDOPERATION_H__1193A240_BF54_48A0_9366_0A5C9829BEE3__INCLUDED_)
#define AFX_AUDIOADDOPERATION_H__1193A240_BF54_48A0_9366_0A5C9829BEE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "SceneAudioObject.h"

class CAudioAddOperation :public COperation  
{
public:
	CAudioAddOperation();
	virtual ~CAudioAddOperation();

	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateAudio();
private:

	CSceneAudioObject m_tempAudio;
};

#endif // !defined(AFX_AUDIOADDOPERATION_H__1193A240_BF54_48A0_9366_0A5C9829BEE3__INCLUDED_)
