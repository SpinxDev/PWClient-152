// ExpSceneObject.h: interface for the ExpSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPSCENEOBJECT_H__0AA6939E_F2EC_4BB6_A650_5F4592EE8C71__INCLUDED_)
#define AFX_EXPSCENEOBJECT_H__0AA6939E_F2EC_4BB6_A650_5F4592EE8C71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "A3DVector.h"

enum
{
	CHUNK_FILE_FLAG			= 0,
	CHUNK_DATA_FLAG			= 1,
	CHUNK_STRING_FLAG		= 2,
};

struct SF_FILE_HEAD
{
	DWORD dwIdentify;
	DWORD dwVersion;
	DWORD dwFileDataSize;
	DWORD dwObjectTypesNum;
};

struct SF_DATA_HEAD
{
	DWORD	dwObjectType;			//对象的类型
	DWORD	dwObjectSize;			//对象的数据块尺寸
	DWORD   dwObjectNums;			//数据块的数目
};

struct SF_DATA_STATIC_MODEL
{
	DWORD			dwID;			//对象的ID
	DWORD           dwPathID;		//模型路径名字ID
	A3DVECTOR3		vPosition;		//对象的位置
	A3DVECTOR3		vDirection;		//对象的方向    
	A3DVECTOR3		vUpDirection;   //对象的上方向
	A3DVECTOR3		vScale;         //对象的缩放倍数
};

struct SF_STRING_HEAD
{
	DWORD   dwStringNums;			//字符串的数目
};

class CExpSceneObject  
{
public:
	CExpSceneObject();
	virtual ~CExpSceneObject();

	bool Init(A3DVECTOR3 vOffset,const char *exppath,const char *expname);

	bool DoExport();			    //输出数据
private:
	const char* m_szPath;
	const char* m_szName;
	A3DVECTOR3 m_vOffset;

};

#endif // !defined(AFX_EXPSCENEOBJECT_H__0AA6939E_F2EC_4BB6_A650_5F4592EE8C71__INCLUDED_)
