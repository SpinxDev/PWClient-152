// Filename	: EC_LoginCG.h
// Creator	: Xu Wenbin
// Date		: 2010/11/22

#pragma once

#include <AMVideoClip.h>

class CECLoginCG
{
public:

	CECLoginCG();
	~CECLoginCG();
	void Release();

	bool Initialize(bool bFirst);
	bool DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam);
	bool Tick(DWORD dwDeltaTime);
	bool Render(bool bPresent);
	bool TestRender(bool bExclusive);
	bool TestSkipRender();
	void Resize();

private:
	bool LoadVideo(const char *szVideo, AMVideoClip *&pVideoOut);
	void FirstLogin();
	void UpdateScale(AMVideoClip *pVideo);
	void Stop(AMVideoClip *pVideo);
	void Pause(AMVideoClip *pVideo);
	void Start(AMVideoClip *pVideo, bool bLoop);

private:
	
	AMVideoClip *		m_pStartVideo;			  //	开场CG（只在第一次登录前播放，可跳过）
	AMVideoClip *		m_pBackVideo;			//	登录背景CG（每次到登录框都播放）
	bool	m_bBackVideo;								//	是否在播放第二段视频

	int		m_nPassRenderCount;						//	当两段视频都存在时，显示第二段视频前需要跳过的帧数，
																		//	以处理第二段视频开始显示为黑色、导致两段视频衔接时中间显示黑帧、从而导致跳跃的效果
	bool	m_bNeedResize;							//	WM_SIZE 消息等处理时，记录此标识，在 Render 时处理
};