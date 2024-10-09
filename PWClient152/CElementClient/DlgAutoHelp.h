// Filename	: DlgAutoHelp.h 
// Creator	: WYD


#pragma once

#include "DlgBase.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"
#include "AUIEditBox.h"

class A2DSprite;
class A3DDevice;

class CImgAnimation
{
public:
	struct keyframe
	{
		A2DSprite* pSprite;
		int tick;
	};
public:
	CImgAnimation(A3DDevice* device);
	~CImgAnimation();

	void Tick(DWORD dt);
	void AddFrame(const char* szPath, int tick);
	void Start();
	void Stop() {m_bStart = false;}
	void Resume() { m_bStart = true;}
	void ReleaseFrame();
	void SetTargetImgControl(PAUIIMAGEPICTURE p) { m_pImg = p;}
	void SetPlayRate(float r);
	void SetRandPlay(bool bRand) { m_bRandPlay = bRand;}
	void SetLoopFlag(bool bLoop) { m_bLoop = bLoop;}
protected:
	void Reset();
private:
	abase::vector<keyframe> m_frames;
	int m_tick;
	int m_curFrame;
	bool m_bLoop;
	float m_fPlayRate;
	PAUIIMAGEPICTURE m_pImg;
	bool m_bStart;
	bool m_bRandPlay;
	int m_RandTick;
	int m_RandDelta;
	A3DDevice* m_pA3DDevice;
};

//////////////////////////////////////////////////////////////////////////

class CDlgAutoHelp : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	
public:
	CDlgAutoHelp();
	virtual ~CDlgAutoHelp();

	void OnActiveAutoHelp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	
	void SetAutoHelpState(bool bAuto);
	void ResetAutoHelpTime() { m_autoHelpCloseTime = 0;}
	void ResetOpenTime(){ m_openTime = 0;}
	void SetForbidAutoHelpFlag(bool bForbid);
	bool IsForbidAutoHelp() ;
	void IncOpenTime(int delta) ;
	void HideCloseButton();
	void SetMsg(const ACString& str);

	static bool IsAutoHelp();
protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();

protected:
	bool m_bAutoHelp; // �Զ�ץȡ״̬

	int m_closeTime; // ����5���ӹر�
	int m_autoHelpCloseTime; // ����һ���� �˳�ץȡ״̬
	int m_openTime;//����5���ӿ���
	int m_iMsgTime; // ��ʾ5�����ʧ

	PAUITEXTAREA m_pTxt_Msg;
	PAUIEDITBOX m_pImg_Talk;
	PAUIIMAGEPICTURE m_pImg_Girl;
	bool m_bAutoDisappear;			// �Ƿ��Զ��رգ�ֻ���ڸս�����ϷС����һ����ʾʱ�Ż��Զ��ر�
//	CImgAnimation* m_pAnim;
};
