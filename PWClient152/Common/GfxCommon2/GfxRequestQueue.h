// Filename	: GfxRequestQueue.h
// Creator	: Doujianwei
// Date		: 2012,12,21 
// Desc		: gfx request queue, record the gfx which is not exist

#ifndef _GFX_REQUEST_QUEUE_H_
#define _GFX_REQUEST_QUEUE_H_

#ifdef _ANGELICA21

#include "hashmap.h"
#include "AString.h"
#include "A3DResRequest.h"
#include <list>

typedef void	(*GfxLostFunc)(const AString& strSrcPath,void* pArg);
class CGfxRequestQueue
{
public:
	CGfxRequestQueue();
	~CGfxRequestQueue();

public:
	struct GfxLost_GI
	{
		AString		strGfxPath;
	};
	typedef abase::hash_map<AString,GfxLost_GI*>	GfxLost_GIMap;
	typedef std::list<GfxLost_GI*>					GfxDownloadedList;

	struct Gfx_Request_Item
	{
		AString		strPckName;
		GfxLost_GIMap mapGfxLosts;
	};
	typedef abase::hash_map<AString,Gfx_Request_Item*>	Gfx_Request_Item_Map;

protected:
	Gfx_Request_Item_Map m_mapGfx2RequestItem;
	GfxDownloadedList	m_listGfxDownloaded;
	CRITICAL_SECTION	m_csGfx;
	A3DResRequest*		m_pA3DResRequest;
	bool				m_bInit;

private:
	GfxLostFunc			m_pGfxLostFunc;
	void*				m_pGLParam;

public:
	bool			Init(GfxLostFunc pGLFunc,void* pGLParam);
	void			Release();
	
	bool			HasGfxResRequest_Done();

	void			PushGfxResRequest(AString strGfxPath);
	void			PopGfxResRequest(const AString& strPckName);

private:
	void			releaseGfxRIM(Gfx_Request_Item_Map& mapRIM);
	void			releaseGfxDownloaded(GfxDownloadedList& listD);
};
CGfxRequestQueue*	glb_GetGfxRequestQueue();

#endif

#endif