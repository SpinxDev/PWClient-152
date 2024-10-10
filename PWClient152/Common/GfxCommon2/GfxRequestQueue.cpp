#include "StdAfx.h"

#ifdef _ANGELICA21

#include "GfxRequestQueue.h"

CGfxRequestQueue g_GfxRequestQueue;
CGfxRequestQueue*	glb_GetGfxRequestQueue()
{
	return &g_GfxRequestQueue;
}

CGfxRequestQueue::CGfxRequestQueue()
	: m_pA3DResRequest(0)
	, m_bInit(false)
	, m_pGfxLostFunc(0)
	, m_pGLParam(0)
{
	::InitializeCriticalSection(&m_csGfx);
}

CGfxRequestQueue::~CGfxRequestQueue()
{
	Release();
	::DeleteCriticalSection(&m_csGfx);
}

void	CGfxRequestQueue::Release()
{
	releaseGfxRIM(m_mapGfx2RequestItem);
	releaseGfxDownloaded(m_listGfxDownloaded);
}

void	CGfxRequestQueue::releaseGfxRIM(Gfx_Request_Item_Map& mapRIM)
{
	for (Gfx_Request_Item_Map::iterator it=mapRIM.begin();
		it!=mapRIM.end();++it)
	{
		Gfx_Request_Item* pRIM = it->second;

		for (GfxLost_GIMap::iterator itMI=pRIM->mapGfxLosts.begin();
			itMI!=pRIM->mapGfxLosts.end();++itMI)
		{
			delete itMI->second;
		}
		pRIM->mapGfxLosts.clear();
		delete pRIM;
	}
	mapRIM.clear();
}

void	CGfxRequestQueue::releaseGfxDownloaded(GfxDownloadedList& listD)
{
	for (GfxDownloadedList::iterator it=listD.begin();
		it!=listD.end();++it)
	{
		delete (*it);
	}
	listD.clear();
}

bool	CGfxRequestQueue::Init(GfxLostFunc pGLFunc,void* pGLParam)
{
	if(!pGLFunc || !pGLParam)
		return false;
	m_pGfxLostFunc = pGLFunc;
	m_pGLParam	   = pGLParam;

	m_pA3DResRequest = glb_GetResRequest();
	if(!m_pA3DResRequest)
		return false;

	m_bInit = true;
	return true;
}

void	CGfxRequestQueue::PushGfxResRequest(AString strGfxPath)
{
	if(!m_bInit)
		return;

	strGfxPath.MakeLower();
	if(af_IsFileExist(strGfxPath))
		return;
		
	AString strLog;
	//1,first to search Gfx's pack name
	StringArray aPcks;
	if(!m_pA3DResRequest->GetResPackName(A3DResRequest::RES_GFX,strGfxPath,aPcks))
	{
		strLog.Format("WebRes: [Gfx: %s] 's pcks in the server is not exists!",strGfxPath);
		m_pA3DResRequest->ResLog(strLog);
		return;
	}
	assert(aPcks.size() == 1 && "CGfxRequestQueue::PushGfxResRequest");
	AString& strPckName = aPcks[0];

	::EnterCriticalSection(&m_csGfx);

	//2,next find if strPckName is in the request map
	Gfx_Request_Item_Map::iterator itRIM = m_mapGfx2RequestItem.find(strPckName);
	if(itRIM!=m_mapGfx2RequestItem.end())
	{
		Gfx_Request_Item* pRIM = itRIM->second;

		GfxLost_GIMap::iterator itL = pRIM->mapGfxLosts.find(strGfxPath);
		if(itL==pRIM->mapGfxLosts.end())
		{
			GfxLost_GI* pGI 	= new GfxLost_GI;
			pGI->strGfxPath		= strGfxPath;
			pRIM->mapGfxLosts[strGfxPath] = pGI;
		}

		::LeaveCriticalSection(&m_csGfx);
		return;
	}

	strLog.Format("WebRes: [Gfx: %s] is not exists!",strGfxPath);
	m_pA3DResRequest->ResLog(strLog);

	//3,if not in the request map,create it
	Gfx_Request_Item* pRIM = new Gfx_Request_Item;	
	pRIM->strPckName   = strPckName;
	m_mapGfx2RequestItem[strPckName] = pRIM;

	GfxLost_GI* pGI 	= new GfxLost_GI;
	pGI->strGfxPath		= strGfxPath;
	pRIM->mapGfxLosts[strGfxPath] = pGI;

	::LeaveCriticalSection(&m_csGfx);

	//4,commit to download
	if(m_pGfxLostFunc)
	{
		m_pGfxLostFunc(strPckName,m_pGLParam);
	}
}

void	CGfxRequestQueue::PopGfxResRequest(const AString& strPckName)
{
	if(!m_bInit)
		return;

	ACSWrapper cs(&m_csGfx);

	Gfx_Request_Item_Map::iterator itRIM = m_mapGfx2RequestItem.find(strPckName);
	if(itRIM==m_mapGfx2RequestItem.end())
		return;

	Gfx_Request_Item* pRIM = itRIM->second;
	assert(pRIM->strPckName == strPckName && "CGfxRequestQueue::PopGfxResRequest");

	//pck downloaded and unpacked,so put the result to list to let the main render thread to replace the skin 
	for (GfxLost_GIMap::iterator it=pRIM->mapGfxLosts.begin();
		it!=pRIM->mapGfxLosts.end();++it)
	{
		GfxLost_GI* pMI = it->second;
		m_listGfxDownloaded.push_back(pMI);
	}
	pRIM->mapGfxLosts.clear();
	m_mapGfx2RequestItem.erase(itRIM);
	delete pRIM;
}

bool	CGfxRequestQueue::HasGfxResRequest_Done()
{
	if(!m_bInit)
		return false;

	ACSWrapper cs(&m_csGfx);

	if(m_listGfxDownloaded.empty())
		return false;

	GfxLost_GI* pPI = m_listGfxDownloaded.back();
	m_listGfxDownloaded.pop_back();

	delete pPI;
	return true;
}

#endif