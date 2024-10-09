// ElementResMan.h: interface for the ElementResMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTRESMAN_H__D9143607_0C45_48B2_8BF2_E5B98603697E__INCLUDED_)
#define AFX_ELEMENTRESMAN_H__D9143607_0C45_48B2_8BF2_E5B98603697E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AList2.h"

#define LENGTH_MD5_CODE 64
#define LENGTH_ID 64
#define LENGTH_ORG_NAME 128
#define LENGTH_BLOCK_FILE 1024
#define RES_FILE_VERSION 0x1


class CSceneObjectManager;

struct  Res_Item
{
	char szOrgName[LENGTH_ORG_NAME];// Original name
	char szMd5Name[LENGTH_MD5_CODE]; // Md5 name
	char szID[LENGTH_ID];              
	APtrList<char *> listReference;
};

class ElementResMan  
{
public:
	enum
	{
		RES_TYPE_BUILDING = 0,
		RES_TYPE_GRASS,
		RES_TYPE_TREE,
		RES_TYPE_GFX,
		RES_TYPE_BUILDING_TEXTURE,
		RES_TYPE_COUNT,
	};

	ElementResMan();
	virtual ~ElementResMan();

public:
	
	bool		AddRes(const int iType, const char *szName, char *szNewName);
	bool        ReplaceRes( const int iType, const char *szName, const char *szNewRes);
	bool        RefIsExist( const int iType, const char *szName, const char *szSceneObjectName);
	int			GetResCount( const int iType) const;
	Res_Item*   GetRes(const int iType, int iIndex);

	APtrList<char *>* GetReferenceList( const int iType, const char* szName );
	void		DeleteReference( const int iType, const char *szID, const char *szReferenceName );
	void		AddReference( const int iType, const char * szID, const char *szReferenceName );
	void		DeleteRes( const int iType, const char *szID);

	void		DeleteRedundanceReference(const int iType, const char *szMap,CSceneObjectManager *pSMan);
	Res_Item*	ResIsExistByMd5(const int iType, const char *szMd5Code, Res_Item* pTest = NULL);
	Res_Item*   ResIsExistByName( const int iType, const char *szName);
	bool        ResIsExistByOrgName( const int iType, const char *szOrgName);
	Res_Item*	ResIsExistByPath( const int iType, const char *szPath);

	bool Save();
	bool Load();
	bool Load(const char *szPath);

	void		FastAddBuildingRes(const char* szOrg, const char* szName);

public:
	static void	GetPathAndName(const CString strPathName, CString& strPath, CString& strName);
	static bool CopyResFile( const char *szDst, const char *szSrc);
	static bool CopyResFileEx( const char *szDst, const char *szSrc);
	static BOOL CreateFolder(CString path);
	static 	CString GetParentPath(CString  path);
	
protected:
	bool   ProcessBuildingRes( const char *szName, char *szNewName);
	bool   ProcessGfxRes( const char *szName);
	char*  ProcessBuildingTextureRes( const char *szName, const char *szReferenceName);
	bool   ProcessGrassRes( const char *szName);
	bool   ProcessTreeRes(const char *szName, const char *szCompositeMap);

	bool   ReplaceBuildingRes(const char *szName, const char *szNewRes);
	bool   ReplaceTextureRes(const int iType, const char *szName, const char *szNewRes);
	bool   ReplaceTreeRes(const char *szName, const char* szNewRes);

	//bool   CopyResFile( const char *szDst, const char *szSrc);

	char *		GenerateMd5Code(char*szBuffer, const char* szName);
	INT64		GetFileSize( const char* szName) const;
	void		ReleaseResTable();

	bool		WriteTable();
	bool		ReadTable(const char *szPath);

	//void		GetPathAndName(const CString strPathName, CString& strPath, CString& strName);
	CString		TransOrgName(const char *szOrg);
	//CString     GetParentPath(CString  path);
	//BOOL        CreateFolder(CString path);
	
private:
	
	APtrList<Res_Item*> ResTable[RES_TYPE_COUNT];
	DWORD mIdCount[RES_TYPE_COUNT];
	int   iFolderIndex[RES_TYPE_COUNT];
};

#endif // !defined(AFX_ELEMENTRESMAN_H__D9143607_0C45_48B2_8BF2_E5B98603697E__INCLUDED_)






