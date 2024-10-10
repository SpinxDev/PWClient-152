// FTManager.cpp: implementation of the FTManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTManager.h"

#include "MyFTCache.h"
#include "FTHelper.h"
#include "FTFont.h"
#include "WTFont.h"
#include "ftsnames.h"
#include "ttnameid.h"
#include <string.h>

CriticalSession FTManager::CSForAllManager;


IFTManager * _cdecl CreateFTManager(int nMaxFontFamilyCountInCache, int nMaxFontSizeInCache, int nMaxCacheSizeBytes)
{
	FTManager *pFTManager = new FTManager;
	if (!pFTManager->Create(nMaxFontFamilyCountInCache, nMaxFontSizeInCache, nMaxCacheSizeBytes))
	{
		SAFE_DELETE(pFTManager);
	}
	return pFTManager;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FTManager::FTManager()
{
	FTLibrary = NULL;
	FTCManager = NULL;
	FTCCMapCache = NULL;
}

FTManager::~FTManager()
{
	Destory();
}

bool FTManager::Create(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes)
{
	// set my own default value instead of freetype's
	if (nMaxFontFamilyCountInCache == 0) nMaxFontFamilyCountInCache = 4;
	if (nMaxFontStyleInCache == 0) nMaxFontStyleInCache = 4;
	if (nMaxCacheSizeBytes == 0) nMaxCacheSizeBytes = 1024 * 64;

	bool bSuccess = false;
	do {
		if (FT_Init_FreeType(&FTLibrary))
			break;

		if (FTC_Manager_New(FTLibrary, nMaxFontFamilyCountInCache, nMaxFontStyleInCache, nMaxCacheSizeBytes, MyFTCFaceRequest, NULL, &FTCManager))
			break;

		if (FTC_CMapCache_New(FTCManager, &FTCCMapCache))
			break;
		
		CacheSizeMax = nMaxCacheSizeBytes;

		bSuccess = true;
		
	} while(false);

	if (!bSuccess)
		Destory();

	return bSuccess;
}

void FTManager::Destory()
{
	if (FTCManager)
	{
		FTC_Manager_Done(FTCManager);
		FTCManager = NULL;
	}

	if (FTLibrary)
	{
		FT_Done_FreeType(FTLibrary);
		FTLibrary = NULL;
	}

	for( FaceID_Map::iterator it=FaceIDMap.begin(), it_end=FaceIDMap.end(); it!=it_end; ++it )
	{
		FaceID_Node node = it->second;
		if( node.faceId->file_length != 0 )
			delete[] node.faceId->file_data;
		delete node.faceId;
	}
	FaceIDMap.clear();
}


IFTFont * FTManager::CreateFont(const char *strFontPath, int nFontFileLength, UINT uPoint, 
								int nStyle, int nOutlineWidth,int nCreateMode,
								const wchar_t *strFaceName)
{
	CriticalSession::Scope cs(CSForAllManager);
	IFTFont* retFont = NULL;
	
	switch(nCreateMode)
	{
	case FONT_CREATE_FREETYPE:
	{
		FTFont *ftFont = new FTFont(this, CacheSizeMax);
		if (!ftFont->CreatePointFont(strFontPath, nFontFileLength, uPoint, nStyle, nOutlineWidth))
		{
			SAFE_DELETE(ftFont);
		}
		retFont = ftFont;
	}
	break;
	case FONT_CREATE_NOFREETYPE:
		WTFont *wtFont = new WTFont();
		if (!wtFont->CreatePointFont(strFontPath, nFontFileLength, uPoint, nStyle, nOutlineWidth,strFaceName))
		{
			wtFont->Release();
			delete wtFont;
			wtFont = NULL;
		}
		retFont = wtFont;
	break;
	}
	return retFont;
}

void FTManager::Release()
{
	CriticalSession::Scope cs(CSForAllManager);
	delete this;
}


IFTFont * FTManager::CreateFontIndirect(const FontParam &fontParam,int nCreateMode)
{
	CriticalSession::Scope cs(CSForAllManager);
	IFTFont* retFont = NULL;

	switch(nCreateMode)
	{
	case FONT_CREATE_FREETYPE:
	{
		FTFont *ftFont = new FTFont(this, CacheSizeMax);
		if (!ftFont->CreateFontIndirect(fontParam))
		{
			SAFE_DELETE(ftFont);
		}
		retFont = ftFont;
	}
	break;
	case FONT_CREATE_NOFREETYPE:
	break;
	}
	return retFont;
}

bool FTManager::FaceID_Key::operator < (FaceID_Key const &obj) const
{
	FaceID_Key const &x = *this;
	FaceID_Key const &y = obj;

	if( x.nFaceIndex != y.nFaceIndex )
		return x.nFaceIndex < y.nFaceIndex;

	if( x.nFontFileLength != y.nFontFileLength )
		return x.nFontFileLength < y.nFontFileLength;

	if (x.nFontStyle != y.nFontStyle)
		return x.nFontStyle < y.nFontStyle;

	// same nFaceIndex and same nFontFileLength and same nFontStyle
	unsigned fontFileLength = x.nFontFileLength;
	if( fontFileLength == 0 )
		return strcmp(x.strFontPath, y.strFontPath)<0;
	else			// memory file
		return x.pData < y.pData;
}

My_FaceID FTManager::LookupFaceID(const char *strFontPath, unsigned nFontFileLength, int nFaceIndex, int nFontStyle)
{
	FaceID_Key key;

	if (nFontFileLength == 0)
		strcpy(key.strFontPath, strFontPath);
	else
		key.pData = strFontPath;

	key.nFontFileLength = nFontFileLength;
	key.nFaceIndex = nFaceIndex;
	key.nFontStyle = nFontStyle;

	FaceID_Map::iterator it = FaceIDMap.find(key);
	if( it != FaceIDMap.end() )
	{
		FaceID_Node &node = it->second;
		++node.refCount;
		return node.faceId;
	}
	
	My_FaceID faceId = new My_FaceID_Rec;
	faceId->file_length = nFontFileLength;

	if( nFontFileLength == 0 )
		strcpy(faceId->file_path, strFontPath);
	else
	{
		faceId->file_data = new char[nFontFileLength];
		memcpy( faceId->file_data, strFontPath, nFontFileLength );
	}

	faceId->face_index = nFaceIndex;
	faceId->font_style = nFontStyle;
	FaceIDMap[key] = FaceID_Node(faceId, 1);
	return faceId;
}

void FTManager::RemoveFaceID(My_FaceID faceId)
{
	if( faceId == NULL )
		return;

	FaceID_Key key;
	key.nFontFileLength = faceId->file_length;
	if( key.nFontFileLength == 0 )
		strcpy(key.strFontPath, faceId->file_path);
	else
		key.pData = (const char *)faceId->file_data;

	key.nFaceIndex = faceId->face_index;
	key.nFontStyle = faceId->font_style;
	
	FaceID_Map::iterator it = FaceIDMap.find(key);
	if( it != FaceIDMap.end() )
	{
		FaceID_Node &node = it->second;
		--node.refCount;

		if( node.refCount <= 0 )
		{
			FTC_Manager_RemoveFaceID(FTCManager, (FTC_FaceID)node.faceId);
			if( node.faceId->file_length != 0 )
				delete[] node.faceId->file_data;

			delete node.faceId;
			FaceIDMap.erase(it);
		}
	}
}

