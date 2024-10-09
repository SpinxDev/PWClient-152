
#include "arc_overlay.h"
#include "EC_Global.h"
#include <ALog.h>

namespace CC_SDK
{	
	ArcOverlay& ArcOverlay::Instance()
	{
		static ArcOverlay overlay;
		return overlay;
	}
	
	ArcOverlay::ArcOverlay()
		: m_hModule(NULL)
		, m_hCCSDK(NULL)
		, m_init(NULL)
		, m_uninit(NULL)
		, m_gotoURL(NULL)
		, m_getAccountName(NULL)
		, m_getToken(NULL)
		, m_bLoaded(false)
	{
	}
	
	ArcOverlay::~ArcOverlay()
	{
	}
	
	bool ArcOverlay::Init()
	{
		if (!m_bLoaded){
			char dllPath[MAX_PATH] = {0};
			_snprintf(dllPath, MAX_PATH, "%s\\arcsdk.dll", g_szWorkDir);
			m_hModule = LoadLibraryA(dllPath);
			if (m_hModule){
				m_init = (CC_Init)GetProcAddress(m_hModule, "CC_Init");
				if (!m_init) a_LogOutput(1, "ArcOverlay::Init,Failed to to get CC_Init.");

				m_uninit = (CC_UnInit)GetProcAddress(m_hModule, "CC_UnInit");
				if (!m_uninit) a_LogOutput(1, "ArcOverlay::Init,Failed to to get CC_UnInit.");

				m_gotoURL = (CC_GotoUrlInOverlay)GetProcAddress(m_hModule, "CC_GotoUrlInOverlay");
				if (!m_gotoURL) a_LogOutput(1, "ArcOverlay::Init,Failed to to get CC_GotoUrlInOverlay.");

				m_getAccountName = (CC_GetAccountName)GetProcAddress(m_hModule, "CC_GetAccountName");
				if (!m_getAccountName) a_LogOutput(1, "ArcOverlay::Init,Failed to to get CC_GetAccountName.");
				
				m_getToken = (CC_GetToken)GetProcAddress(m_hModule, "CC_GetToken");
				if (!m_getToken) a_LogOutput(1, "ArcOverlay::Init,Failed to to get CC_GetToken.");

				if (m_init && m_uninit && m_gotoURL && m_getAccountName && m_getToken){
					m_hCCSDK = m_init(0, 0, 0);
					if (m_hCCSDK){
						m_bLoaded = true;
					}else{
						a_LogOutput(1, "ArcOverlay::Init, Failed to call CC_Init(0,0,0).");
					}
				}
			}
			else{
				DWORD dwError = GetLastError();
				a_LogOutput(1, "ArcOverlay::Init,Failed to load %s(GetLastError=%u)", dllPath, dwError);
			}
		}
		
		return m_bLoaded;
	}
	
	void ArcOverlay::UnInit()
	{
		if (m_hCCSDK && m_uninit){
			HRESULT hRes = m_uninit(m_hCCSDK);
			if (hRes != S_OK){
				a_LogOutput(1, "ArcOverlay::UnInit,Failed to call CC_UnInit(ret=%x)", hRes);
			}
		}
		m_hCCSDK = NULL;
		m_init = NULL;
		m_uninit = NULL;
		m_gotoURL = NULL;
		m_getAccountName = NULL;
		m_getToken = NULL;
		if (m_hModule){
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
		m_bLoaded = false;
	}
	
	bool ArcOverlay::GotoURL(const wchar_t *pszUrl)
	{
		bool bRet(false);
		if (m_bLoaded){
			HRESULT hRes = m_gotoURL(m_hCCSDK, const_cast<wchar_t *>(pszUrl));
			if (hRes != S_OK){
				a_LogOutput(1, "ArcOverlay::GotoURL,Failed to call CC_GotoUrlInOverlay(ret=%x)", hRes);
			}
			else{
				bRet = true;
			}
		}
		return bRet;
	}

	bool ArcOverlay::GetAccountName(AWString &strAccount){
		bool result(false);
		if (m_bLoaded){
			int nTryTimes = 6;
			for (int i(0); i < nTryTimes; ++ i){
				wchar_t szAccount[128] = {0};
				HRESULT hRes = m_getAccountName(m_hCCSDK, szAccount, ARRAY_SIZE(szAccount));
				if (hRes == S_OK){
					strAccount = szAccount;
					result = true;
					a_LogOutput(1, "ArcOverlay::GetAccountName,Success at i=%d", i);
					break;
				}
				if (hRes != CC_ERR_CLIENT_INTERNAL){
					a_LogOutput(1, "ArcOverlay::GetAccountName,Failed to call CC_GetAccountName(ret=%x) at i=%d", hRes, i);
					break;
				}
				a_LogOutput(1, "ArcOverlay::GetAccountName,Failed to call CC_GetAccountName(ret=%x) at i=%d, try again...", hRes, i);
				Sleep(1000);
			}
		}
		return result;
	}

	bool ArcOverlay::GetToken(const wchar_t* pszAccount, AWString &strToken){
		bool result(false);
		if (m_bLoaded){
			wchar_t szToken[128] = {0};
			HRESULT hRes = m_getToken(m_hCCSDK, const_cast<wchar_t *>(pszAccount), szToken, ARRAY_SIZE(szToken));
			if (hRes != S_OK){
				a_LogOutput(1, "ArcOverlay::GetToken,Failed to call CC_GetToken(ret=%x)", hRes);
			}else{
				strToken = szToken;
				result = true;
			}
		}
		return result;
	}

}  // namespace CC_SDK
