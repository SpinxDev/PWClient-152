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
	//	加载CG，成功加载时返回true，加载后不立即播放
	//

	//	登录逻辑修改，不再需要视频
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
	//	更新缩放，以处理宽屏情况
	//	视图更新后调用

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

	//	设置视频位置
	A3DPOINT2 p;
	p.x = 0;
	p.y = pMainView->Y;
	pVideo->SetVidPos(p);

	//	设置视频缩放
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
		//	COM初始化，仅在第一次进行
		CoInitialize(NULL);
	}

	//	加载登录背景CG
 	LoadVideo("maps\\mtv\\loginloop.mpx", m_pBackVideo);

	if (bFirst)
	{
		//	刚打开客户端，首次尝试加载视频
		if (!CECLoginSwitch::Instance().IsUseToken() && !CECLoginSwitch::Instance().IsUseSSO())
		{
			//	使用 token 登录游戏时，会尝试直接登录游戏，因此不需要加载开场CG
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

	//	如果有视频被成功加载，则调用缩放处理
	if (m_pBackVideo || m_pStartVideo)
		Resize();

	return true;
}

void CECLoginCG::FirstLogin()
{
	//	跳过登录后的CG动画，显示登录界面及CG
	//

	//	第一段视频播放时，游戏为尚未开启登录状态
	Stop(m_pStartVideo);
	
	//	播放登录界面的背景CG
	Start(m_pBackVideo, true);
	m_bBackVideo = true;
}

bool CECLoginCG::DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bRet(false);
	
	while (m_pStartVideo)
	{
		//	跳过开始视频处理
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
		//	第一段视频还没有播放完
		m_pStartVideo->Tick();
		return false;
	}
	
	if (!m_pStartVideo || m_pStartVideo->IsStopped())
	{
		//	没有第一段视频，或者第一段视频已停止
		 if (!m_bBackVideo)
		 {
			 //	首次登录未启动，启动首次登录
			 FirstLogin();
			 return false;
		 }
	}
	
	if (m_pBackVideo)
	{
		//	更新第二段视频
		CECUIManager *pUIManger = g_pGame->GetGameRun()->GetUIManager();
		if (pUIManger->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN)
		{
			CECLoginUIMan* pLoginUI = pUIManger->GetLoginUIMan();
			if (pLoginUI)
			{
				if (pLoginUI->GetCurScene() == CECLoginUIMan::LOGIN_SCENE_LOGIN)
				{
					//	登录状态时，播放CG动画
					if (!m_pBackVideo->IsPlaying())
						Start(m_pBackVideo, true);
				}
				else
				{
					//	非登录状态时，暂停CG动画
					if (m_pBackVideo->IsPlaying())
						Pause(m_pBackVideo);
				}
			}
		}
		
		//	更新CG视频本身
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
	//	判断是否需要渲染
	//	bExclusive 为 true 时表示将只测试是否将只显示视频
	//
	CECUIManager *pUIManger = g_pGame->GetGameRun()->GetUIManager();
	if (pUIManger->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN)
	{
		CECLoginUIMan* pLoginUI = pUIManger->GetLoginUIMan();
		if (!pLoginUI ||
			pLoginUI->GetCurScene() != CECLoginUIMan::LOGIN_SCENE_LOGIN)
		{
			//	只在人物登录界面才显示（虽然在Tick中会更改视频的播放状态，
			//	以控制视频的播放，但存在一定误差，
			//	因此此处添加完整检查、以达到精确控制的目的）
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
	//	测试是否跳过当前帧
	//
	bool bSkip(false);

	if (TestRender(false))
	{
		//	正常情况下可以显示
		//
		if (!m_bBackVideo)
		{
			if (m_pStartVideo && !m_pStartVideo->IsPlaying())
			{
				//	第一段视频停止播放时、暂停显示
				//bSkip = true;
			}
		}
		else
		{
			if (m_pBackVideo && m_pBackVideo->IsPlaying())
			{
				if (m_pStartVideo)
				{
					//	有第一段视频时才需要做衔接处理，不然就直接播放了
					if (m_nPassRenderCount > 0 && --m_nPassRenderCount >= 0)
					{
						//	跳过起始一定帧数
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
