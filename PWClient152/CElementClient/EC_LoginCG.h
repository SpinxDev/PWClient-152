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
	
	AMVideoClip *		m_pStartVideo;			  //	����CG��ֻ�ڵ�һ�ε�¼ǰ���ţ���������
	AMVideoClip *		m_pBackVideo;			//	��¼����CG��ÿ�ε���¼�򶼲��ţ�
	bool	m_bBackVideo;								//	�Ƿ��ڲ��ŵڶ�����Ƶ

	int		m_nPassRenderCount;						//	��������Ƶ������ʱ����ʾ�ڶ�����Ƶǰ��Ҫ������֡����
																		//	�Դ���ڶ�����Ƶ��ʼ��ʾΪ��ɫ������������Ƶ�ν�ʱ�м���ʾ��֡���Ӷ�������Ծ��Ч��
	bool	m_bNeedResize;							//	WM_SIZE ��Ϣ�ȴ���ʱ����¼�˱�ʶ���� Render ʱ����
};