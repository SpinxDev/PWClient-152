// Filename	: EC_ModelAsynLoadHelper.h
// Creator	: Xu Wenbin
// Date		: 2014/11/28

#pragma once

#include <wtypes.h>

#include "EC_AsynLoadHelper.h"
#include "EC_IvtrTypes.h"

#include <AString.h>

//	ģ�ͻ���
class CECModelAsynLoadContent : public CECAsynLoadContent{
	int		m_id;			//	Player��NPC��Matter ��ΩһID�����ڲ���ʵ��
	DWORD	m_dwBornStamp;	//	����ʱ������Ҫ���� NPC����Ϊ��Ωһ
	float	m_fDistToHost;	//	�� Host Player �ľ��룬���ڼ���ǰ���򣬰�˳�����
public:
	CECModelAsynLoadContent(int id, DWORD dwBornStamp);
	void SetDistToHost(float fDistToHost);
	int	GetID()const{
		return m_id;
	}
	DWORD GetBornStamp()const{
		return m_dwBornStamp;
	}
	float GetDistToHost()const{
		return m_fDistToHost;
	}
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual float GetSortWeight()const{
		return m_fDistToHost;
	}
	virtual void UpdateSortWeight(){}
};

//	�������ģ�ͼ��ز���
class CECPlayerModelAsynLoadContent : public CECModelAsynLoadContent{
public:
	CECPlayerModelAsynLoadContent(int id, DWORD dwBornStamp);
	virtual void UpdateSortWeight();
};

//	�������ģ�ͼ��ز���
class CECPlayerBasicModelAsynLoadContent : public CECPlayerModelAsynLoadContent{
	int		m_iProfession;
	int		m_iGender;
	int		m_iBodyID;
	int		m_aEquips[IVTRSIZE_EQUIPPACK];
	AString	m_strPetPath;
	bool	m_bSimpleModel;
	bool	m_bSimpleFace;
	bool	m_bClientModel;
public:
	CECPlayerBasicModelAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, int iBodyID, const int *aEquips, const char *szPetPath, bool bSimpleModel, bool bSimpleFace, bool bClientModel);
	int	GetProfession()const{
		return m_iProfession;
	}
	int	GetGender()const{
		return m_iGender;
	}
	int	GetBodyID()const{
		return m_iBodyID;
	}
	const int *GetEquips()const{
		return m_aEquips;
	}
	const AString &GetPetPath()const{
		return m_strPetPath;
	}
	bool GetSimpleModel()const{
		return m_bSimpleModel;
	}
	bool GetSimpleFace()const{
		return m_bSimpleFace;
	}
	bool GetClientModel()const{
		return m_bClientModel;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	��������ģ�ͼ��ز���
class CECPlayerFaceAsynLoadContent : public CECPlayerModelAsynLoadContent{
	int		m_iProfession;
	int		m_iGender;
	int		m_iFaceID;
public:
	CECPlayerFaceAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, int iFaceID);
	int	GetProfession()const{
		return m_iProfession;
	}
	int	GetGender()const{
		return m_iGender;
	}
	int GetFaceID()const{
		return m_iFaceID;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &rhs)const;
	virtual bool ThreadLoad();
};

//	�������ģ�͵ȼ��ز���
class CECPlayerShapeAsynLoadContent : public CECPlayerModelAsynLoadContent{
	int		m_iShape;
	bool	m_bClientModel;
public:
	CECPlayerShapeAsynLoadContent(int id, DWORD dwBornStamp, int iShape, bool bClientModel);
	int	GetShape()const{
		return m_iShape;
	}
	bool GetClientModel()const{
		return m_bClientModel;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	����װ��ģ�ͼ��ز���
class CECPlayerEquipmentAsynLoadContent : public CECPlayerModelAsynLoadContent{
	int		m_iProfession;
	int		m_iGender;
	__int64	m_iEquipMask;	
	int		m_aEquips[IVTRSIZE_EQUIPPACK];
	bool	m_bSimpleModel;
public:
	CECPlayerEquipmentAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, __int64 iEquipMask, const int *aEquips, bool bSimpleModel);
	int	GetProfession()const{
		return m_iProfession;
	}
	int	GetGender()const{
		return m_iGender;
	}
	__int64	GetEquipMask()const{
		return m_iEquipMask;
	}
	const int * GetEquips()const{
		return m_aEquips;
	}
	bool GetSimpleModel()const{
		return m_bSimpleModel;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	�������ģ�͵ȼ��ز���
class CECPlayerPetAsynLoadContent : public CECPlayerModelAsynLoadContent{
	AString	m_strFilePath;
public:
	CECPlayerPetAsynLoadContent(int id, DWORD dwBornStamp, const char *szFile);
	const AString &GetFilePath()const{
		return m_strFilePath;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	NPC ����ģ�ͼ��ز���
class CECNPCModelAsynLoadContent : public CECModelAsynLoadContent{
	int		m_tid;
	AString	m_strFilePath;
public:
	CECNPCModelAsynLoadContent(int id, DWORD dwBornStamp, int tid, const char *szFilePath);
	int	GetTID()const{
		return m_tid;
	}
	const AString &GetFilePath()const{
		return m_strFilePath;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	Matter ģ�ͼ��ز���
class CECMatterModelAsynLoadContent : public CECModelAsynLoadContent{
	AString	m_strFilePath;
public:
	CECMatterModelAsynLoadContent(int id, const char *szFilePath);
	const AString &GetFilePath()const{
		return m_strFilePath;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};

//	Scene Model ģ�ͼ��ز���
class CECSceneModelAsynLoadContent : public CECModelAsynLoadContent{
	AString	m_strFilePath;
public:
	CECSceneModelAsynLoadContent(int id, const char *szFilePath);
	const AString &GetFilePath()const{
		return m_strFilePath;
	}
	virtual CECAsynLoadContent * Clone()const;
	virtual bool operator == (const CECAsynLoadContent &)const;
	virtual bool ThreadLoad();
};
