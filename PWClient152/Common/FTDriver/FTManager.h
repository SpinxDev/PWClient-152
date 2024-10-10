// FTManager.h: interface for the FTManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTMANAGER_H__2E87E4E7_739B_4D27_8071_7C7638F2348B__INCLUDED_)
#define AFX_FTMANAGER_H__2E87E4E7_739B_4D27_8071_7C7638F2348B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include FT_FREETYPE_H 
#include <freetype/ftcache.h>
#include "FTInterface.h"
#include "CriticalSession.h"
#include "MyFTCache.h"
#include <map>

class FTFont;

class FTManager : public IFTManager
{
private:
	FT_Library FTLibrary;
	FTC_Manager FTCManager;
	FTC_CMapCache FTCCMapCache;
	int CacheSizeMax;
	static CriticalSession CSForAllManager;
public:
	FT_Library GetLibrary() { return FTLibrary; }
	FTC_Manager GetCacheManager() { return FTCManager; }
	FTC_CMapCache GetCMapCache() { return FTCCMapCache; }
	int GetCacheSizeMax() { return CacheSizeMax; }

	void Destory();
	bool Create(int nMaxFontFamilyCountInCache = 0, int nMaxFontStyleInCache = 0, int nMaxCacheSizeBytes = 0);
	FTManager();
	virtual ~FTManager();
public:
	// IFTManager interface implement
	IFTFont * CreateFont(const char *strFontPath, int nFontFileLength, 
		UINT uPoint, int nStyle, int nOutlineWidth,int nCreateMode = FONT_CREATE_FREETYPE,
		const wchar_t *strFaceName = NULL);
	IFTFont * CreateFontIndirect(const FontParam &fontParam,int nCreateMode = FONT_CREATE_FREETYPE);
	void Release();

private:
	// Look up for a existing faceId. Create one when not found
	My_FaceID LookupFaceID(const char *strFontPath, unsigned nFontFileLength, int nFaceIndex, int nFontStyle);
	void RemoveFaceID(My_FaceID faceId);
	friend class FTFont;

	// if nFontFileLength == 0, strFontPath is a string, else it is a buffer
	struct FaceID_Key
	{
		union
		{
			char strFontPath[MAX_PATH];
			const char* pData;
		};

		int nFontFileLength;
		int nFaceIndex;
		int nFontStyle;

		bool operator < (FaceID_Key const &obj) const;
	};

	struct FaceID_Node
	{
		My_FaceID faceId;
		int refCount;
		
		FaceID_Node(): faceId(NULL), refCount(0)
			{};
		FaceID_Node(My_FaceID faceId, int refCount): faceId(faceId), refCount(refCount)
			{};
	};

	typedef std::map<FaceID_Key, FaceID_Node> FaceID_Map;
	FaceID_Map FaceIDMap;
};

#endif // !defined(AFX_FTMANAGER_H__2E87E4E7_739B_4D27_8071_7C7638F2348B__INCLUDED_)
