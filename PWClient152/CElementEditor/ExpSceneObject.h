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
	DWORD	dwObjectType;			//���������
	DWORD	dwObjectSize;			//��������ݿ�ߴ�
	DWORD   dwObjectNums;			//���ݿ����Ŀ
};

struct SF_DATA_STATIC_MODEL
{
	DWORD			dwID;			//�����ID
	DWORD           dwPathID;		//ģ��·������ID
	A3DVECTOR3		vPosition;		//�����λ��
	A3DVECTOR3		vDirection;		//����ķ���    
	A3DVECTOR3		vUpDirection;   //������Ϸ���
	A3DVECTOR3		vScale;         //��������ű���
};

struct SF_STRING_HEAD
{
	DWORD   dwStringNums;			//�ַ�������Ŀ
};

class CExpSceneObject  
{
public:
	CExpSceneObject();
	virtual ~CExpSceneObject();

	bool Init(A3DVECTOR3 vOffset,const char *exppath,const char *expname);

	bool DoExport();			    //�������
private:
	const char* m_szPath;
	const char* m_szName;
	A3DVECTOR3 m_vOffset;

};

#endif // !defined(AFX_EXPSCENEOBJECT_H__0AA6939E_F2EC_4BB6_A650_5F4592EE8C71__INCLUDED_)
