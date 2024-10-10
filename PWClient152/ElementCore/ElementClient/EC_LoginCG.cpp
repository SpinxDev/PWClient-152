#include "EC_LoginCG.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_Resource.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "EC_Viewport.h"
#include "EC_LoginSwitch.h"
#include <A3DMacros.h>
#include <AFI.h>
#include <A3DEngine.h>
#include <A3DViewport.h>

#define OPENING_ANIMATION_SCALE	(AMVID_FIT_X | AMVID_FIT_Y)

#define PASS_RENDER_COUNT 10

CECLoginCG::CECLoginCG()
: m_pStartVideo(NULL)
, m_pBackVideo(NULL)
, m_bBackVideo(false)
, m_nPassRenderCount(PASS_RENDER_COUNT)
, m_bNeedResize(false)
{
}

CECLoginCG::~CECLoginCG()
{
	Release();
}

void CECLoginCG::Release()
{
	A3DRELEASE(m_pStartVideo);
	A3DRELEASE(m_pBackVideo);
	m_bBackVideo = false;
	m_nPassRenderCount = PASS_RENDER_COUNT;
	m_bNeedResize = false;
}

bool CECLoginCG::LoadVideo(const char *szVideo, AMVideoClip *&pVideoOut)
{
	//	����CG���ɹ�����ʱ����true�����غ���������
	//

	//	��¼�߼��޸ģ�������Ҫ��Ƶ
	return false;

	//if (pVideoOut)
	//	return false;
	//
	//if (af_IsFileExist(szVideo))
	//{
	//	pVideoOut = new AMVideoClip;
	//	while (true)
	//	{
	//		A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	//		AMVideoEngine * pAMVideoEngine = g_pGame->GetA3DEngine()->GetAMVideoEngine();
	//		if (!pA3DDevice)
	//		{
	//			a_LogOutput(1, "CECLoginCG::LoadVideo, Failed to load %s because A3DDevice is empty.", szVideo);
	//			break;
	//		}
	//		if (!pAMVideoEngine)
	//		{
	//			a_LogOutput(1, "CECLoginCG::LoadVideo, Failed to load %s because AMVideoEngine is empty.", szVideo);
	//			break;
	//		}

	//		if (!pVideoOut->Init(pA3DDevice, pAMVideoEngine, szVideo, true))
	//		{
	//			a_LogOutput(1, "CECLoginCG::LoadVideo, Failed to init %s.", szVideo);
	//			A3DRELEASE(pVideoOut);
	//			break;
	//		}
	//		pVideoOut->SetAutoScaleFlags(OPENING_ANIMATION_SCALE);
	//		pVideoOut->SetVolume(g_pGame->GetConfigs()->GetSystemSettings().nSoundVol);
	//		UpdateScale(pVideoOut);
	//		break;
	//	}
	//}

	//return pVideoOut != NULL;
}

void CECLoginCG::UpdateScale(AMVideoClip *pVideo)
{
	//	�������ţ��Դ���������
	//	��ͼ���º����

	if (!pVideo)
		return;

	if (!g_pGame->GetConfigs()->GetSystemSettings().bWideScreen)
	{
		pVideo->SetAutoScaleFlags(OPENING_ANIMATION_SCALE);
		return;
	}

	A3DDEVFMT devFmt = g_pGame->GetA3DDevice()->GetDevFormat();
	A3DVIEWPORTPARAM * pMainView = g_pGame->GetViewport()->GetA3DViewport()->GetParam();	
	if( pMainView->Y < 0 )
		return;	

	//	������Ƶλ��
	A3DPOINT2 p;
	p.x = 0;
	p.y = pMainView->Y;
	pVideo->SetVidPos(p);

	//	������Ƶ����
	float fScaleX = 1.0f;
	if (pVideo->GetVideoWidth() > 0)
		fScaleX = pMainView->Width / (float)pVideo->GetVideoWidth();
	float fScaleY = 1.0f;
	if (pVideo->GetVideoHeight() > 0)
		fScaleY = pMainView->Height / (float)pVideo->GetVideoHeight();

	pVideo->SetScaleFactors(fScaleX, fScaleY);
}

bool CECLoginCG::Initialize(bool bFirst)
{	
	if (bFirst)
	{
		//	COM��ʼ�������ڵ�һ�ν���
		CoInitialize(NULL);
	}

	//	���ص�¼����CG
 	LoadVideo("maps\\mtv\\loginloop.mpx", m_pBackVideo);

	if (bFirst)
	{
		//	�մ򿪿ͻ��ˣ��״γ��Լ�����Ƶ
		if (!CECLoginSwitch::Instance().IsUseToken() && !CECLoginSwitch::Instance().IsUseSSO())
		{
			//	ʹ�� token ��¼��Ϸʱ���᳢��ֱ�ӵ�¼��Ϸ����˲���Ҫ���ؿ���CG
			LoadVideo("maps\\mtv\\login.mpx", m_pStartVideo);
			if (m_pStartVideo)
				Start(m_pStartVideo, false);
		}
	}

	if (!m_pStartVideo || !m_pStartVideo->IsPlaying())
	{
		m_bBackVideo = true;
		Start(m_pBackVideo, true);
	}

	//	�������Ƶ���ɹ����أ���������Ŵ���
	if (m_pBackVideo || m_pStartVideo)
		Resize();

	return true;
}

void CECLoginCG::FirstLogin()
{
	//	������¼���CG��������ʾ��¼���漰CG
	//

	//	��һ����Ƶ����ʱ����ϷΪ��δ������¼״̬
	Stop(m_pStartVideo);
	
	//	���ŵ�¼����ı���CG
	Start(m_pBackVideo, true);
	m_bBackVideo = true;
}

bool CECLoginCG::DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bRet(false);
	
	while (m_pStartVideo)
	{
		//	������ʼ��Ƶ����
		if (message != WM_KEYDOWN)
			break;
		if (!(wParam == VK_ESCAPE || wParam == VK_SPACE || wParam == VK_RETURN ||
			wParam >= 'A' && wParam <= 'Z' ||
			wParam >= '0' && wParam <= '9'))
			break;
		if (m_bBackVideo)
			break;
		Stop(m_pStartVideo);
		FirstLogin();
		bRet = true;
		break;
	}
	return bRet;
}

bool CECLoginCG::Tick(DWORD dwDeltaTime)
{
	if (m_pStartVideo && m_pStartVideo->IsPlaying())
	{
		//	��һ����Ƶ��û�в�����
		m_pStartVideo->Tick();
		return false;
	}
	
	if (!m_pStartVideo || m_pStartVideo->IsStopped())
	{
		//	û�е�һ����Ƶ�����ߵ�һ����Ƶ��ֹͣ
		 if (!m_bBackVideo)
		 {
			 //	�״ε�¼δ�����������״ε�¼
			 FirstLogin();
			 return false;
		 }
	}
	
	if (m_pBackVideo)
	{
		//	���µڶ�����Ƶ
		CECUIManager *pUIManger = g_pGame->GetGameRun()->GetUIManager();
		if (pUIManger->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN)
		{
			CECLoginUIMan* pLoginUI = pUIManger->GetLoginUIMan();
			if (pLoginUI)
			{
				if (pLoginUI->GetCurScene() == CECLoginUIMan::LOGIN_SCENE_LOGIN)
				{
					//	��¼״̬ʱ������CG����
					if (!m_pBackVideo->IsPlaying())
						Start(m_pBackVideo, true);
				}
				else
				{
					//	�ǵ�¼״̬ʱ����ͣCG����
					if (m_pBackVideo->IsPlaying())
						Pause(m_pBackVideo);
				}
			}
		}
		
		//	����CG��Ƶ����
		if (m_pBackVideo->IsPlaying())
			m_pBackVideo->Tick();
	}

	return false;
}

bool CECLoginCG::Render(bool bPresent)
{
	bool bRet(false);

	if (m_bNeedResize)
	{
		m_bNeedResize = false;
		UpdateScale(m_pStartVideo);
		UpdateScale(m_pBackVideo);
	}

	if (m_pStartVideo && !m_bBackVideo)
	{
		m_pStartVideo->DrawToBack();
		bRet = true;
	}
	else if (m_pBackVideo && m_pBackVideo->IsPlaying())
	{
		m_pBackVideo->DrawToBack();
		bRet  = true;
	}
	return bRet;
}

bool CECLoginCG::TestRender(bool bExclusive)
{
	//	�ж��Ƿ���Ҫ��Ⱦ
	//	bExclusive Ϊ true ʱ��ʾ��ֻ�����Ƿ�ֻ��ʾ��Ƶ
	//
	CECUIManager *pUIManger = g_pGame->GetGameRun()->GetUIManager();
	if (pUIManger->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN)
	{
		CECLoginUIMan* pLoginUI = pUIManger->GetLoginUIMan();
		if (!pLoginUI ||
			pLoginUI->GetCurScene() != CECLoginUIMan::LOGIN_SCENE_LOGIN)
		{
			//	ֻ�������¼�������ʾ����Ȼ��Tick�л������Ƶ�Ĳ���״̬��
			//	�Կ�����Ƶ�Ĳ��ţ�������һ����
			//	��˴˴����������顢�Դﵽ��ȷ���Ƶ�Ŀ�ģ�
			return false;
		}
	}

	bool bRender(false);	
	if (bExclusive)
	{
		bRender = (m_pStartVideo != NULL && !m_bBackVideo);
	}
	else
	{
		if (m_pStartVideo && m_pStartVideo->IsPlaying())
		{
			bRender = true;
		}
		else if (m_pBackVideo && m_pBackVideo->IsPlaying())
		{
			bRender = true;
		}
	}
	return bRender;
}

bool CECLoginCG::TestSkipRender()
{
	//	�����Ƿ�������ǰ֡
	//
	bool bSkip(false);

	if (TestRender(false))
	{
		//	��������¿�����ʾ
		//
		if (!m_bBackVideo)
		{
			if (m_pStartVideo && !m_pStartVideo->IsPlaying())
			{
				//	��һ����Ƶֹͣ����ʱ����ͣ��ʾ
				//bSkip = true;
			}
		}
		else
		{
			if (m_pBackVideo && m_pBackVideo->IsPlaying())
			{
				if (m_pStartVideo)
				{
					//	�е�һ����Ƶʱ����Ҫ���νӴ�����Ȼ��ֱ�Ӳ�����
					if (m_nPassRenderCount > 0 && --m_nPassRenderCount >= 0)
					{
						//	������ʼһ��֡��
						bSkip = true;
					}
				}
			}
		}
	}

	return bSkip;
}

void CECLoginCG::Resize()
{
	m_bNeedResize = true;
}

void CECLoginCG::Stop(AMVideoClip *pVideo)
{
	if (pVideo)
	{
		if (!pVideo->IsStopped())
		{
			pVideo->Stop();
			while (!pVideo->IsStopped())
			{
				Sleep(1);
				pVideo->Stop();
			}
		}
	}
}

void CECLoginCG::Start(AMVideoClip *pVideo, bool bLoop)
{
	if (pVideo)
	{
		if (!pVideo->IsPlaying())
		{
			pVideo->Start(bLoop);
			while (!pVideo->IsPlaying())
			{
				Sleep(1);
				pVideo->Start(bLoop);
			}
		}
	}
}

void CECLoginCG::Pause(AMVideoClip *pVideo)
{
	if (pVideo)
	{
		if (pVideo->IsPlaying())
		{
			pVideo->Pause();
			while (pVideo->IsPlaying())
			{
				Sleep(1);
				pVideo->Pause();
			}
		}
	}
}
