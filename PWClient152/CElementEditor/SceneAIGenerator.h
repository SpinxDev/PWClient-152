// SceneAIGenerator.h: interface for the CSceneAIGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEAIGENERATOR_H__D446CAD6_1B77_4360_8FEF_560CEEDF2FF1__INCLUDED_)
#define AFX_SCENEAIGENERATOR_H__D446CAD6_1B77_4360_8FEF_560CEEDF2FF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"
#include "AObject.h"
#include <A3DVertex.h>

#define EXP_AI_VERSION 0x0


class CPtrList;
class CECModel;

#define STEP_LEN 4.0f

struct AI_DATA
{
	DWORD dwAreaType;	//���������(���أ�ƽ��)
	float fEdgeLen;		//��
	float fEdgeWth;		//��
	float fEdgeHei;		//���ӵĸ�
	int   nMonsterType; //0,��ʾ��ͨȺ��; 1,��ʾȺ������1; 2,��ʾȺ������2
	bool  bPoint;       //06-13�Ƿ��ǹ��������(��û��д���̺����)
	
	int   nControllerId;
	int   nMounsterLife; // ���ɶ���������������ڹ��int�� ��λ��,0��ʾ����
	int   nMounsterNum;  // �ܹ����ɵ������Ŀ
	bool  bMounsterRenascenceAuto;//��������Ƿ��Զ�����
	
	//����������Ѿ��ϳ�������Ϊ����ʾ�û��޸ģ���ʱ����
	bool  bGenMounsterInit;//�����Ƿ���ʼʱ����
	bool  bValidAtOneTime;//�����Ƿ�ͬʱ��Чһ��
	DWORD dwGenID;//�����ƶ����
	//del 06.7.4
};

struct NPC_PROPERTY
{
	AString strNpcName;		//��������
	DWORD dwID;				//����ID
	DWORD dwNum;			//������
	int	  iRefurbish;		//ˢ����
	int	  iRefurbishMin;	//ˢ����
	DWORD dwDiedTimes;		//���������������Ĵ���
	DWORD dwInitiative;		//������������ѡ��
	float fOffsetWater;		//�����ˮ�ĸ߶�
	float fOffsetTerrain;	//�����صĸ߶�
	
	//new 
	DWORD dwMounsterFaction;//�����Լ������
	DWORD dwMounsterFactionHelper;//����Ѱ����������
	DWORD dwMounsterFactionAccept;//������Ӧ���������
	DWORD dwDisapearTime;//�������������ʧʱ�䣨0-100S��
	bool  bNeedHelp;//�Ƿ�Ҫ���
	
	bool  bUseDefautFaction;//�ù���Ĭ�����
	bool  bUseDefautFactionHelper;//�ù���Ĭ�ϵİ�����
	bool  bUseDefautFactionAccept;//������ӦĬ�ϵ�������

	int   nPathID;
	int   nLoopType;       //ѭ����ʽ
	AString strPathName;   //�ﶨ·����
	int     nSpeedFlags;     //0,���٣�1,����
};

struct FIXED_NPC_DATA
{
	AString strNpcName;		//�̶�λ��NPC������
	DWORD dwID;				//NPC��ID
	int   iRefurbish;		//NPC��ˢ��Ƶ��
	int   iRefurbishMin;	//NPC��ˢ��Ƶ��
	
	float fRadius;			//����뾶
	DWORD dwNpcType;	    //����(���أ�ƽ��)
	bool  bIsMounster;      //�ǹ���

	//��·��
	int		nPathID;         //·��ID
	int     nLoopType;       //ѭ����ʽ
	AString strPathName;     //·������
	int     nSpeedFlags;     //0,���٣�1,����
	DWORD   dwDisapearTime;  //�������������ʧʱ�䣨0-100S��

	int   nControllerId;
	int   nMounsterLife; // ���ɶ���������������ڹ��int�� ��λ��,0��ʾ����
	int   nMounsterNum;  // �ܹ����ɵ������Ŀ
	
	bool  bMounsterRenascenceAuto;//��������Ƿ��Զ�����
	bool  bPatrol;//�����Լ�Ѳ��

	bool  bGenMounsterInit;//�����Ƿ���ʼʱ����
	bool  bValidAtOneTime;//�����Ƿ�ͬʱ��Чһ��
	DWORD dwGenID;//�����ƶ����
};

void		init_ai_data( AI_DATA &data);
void        Init_npc_property( NPC_PROPERTY * data);
void		init_fixed_npc_data(FIXED_NPC_DATA &data);
AString     GetModelPath(DWORD dwId);


struct AI_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct FIXED_NPC_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct NPC_BEZIER_SEL_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct CONTROLLER_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct CONTROLLER_NPC_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

enum LOOP_TYPE
{
	LOOP_NO = 0,//��ѭ��
	LOOP_RETURN,//ԭ·����
	LOOP_CIRCLE,//ʼ��ѭ��
};

class CSceneAIGenerator :public CSceneObject 
{
public:
	enum AREA_TYPE
	{
		TYPE_TERRAIN_FOLLOW = 0,//����������
		TYPE_PLANE,//��ĳһƽ����
		TYPE_BOX,//�����ڲ�
	};

	enum 
	{
		INDIVIDUALITY_NORMAL = 0,
		INDIVIDUALITY_INITIATIVE,	
		INDIVIDUALITY_PASSIVITY,
	};


	CSceneAIGenerator();
	virtual ~CSceneAIGenerator();

	virtual void Translate(const A3DVECTOR3& vDelta);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void EntryScene();
	virtual void LeaveScene();
	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
	
	//Exp data
	bool ExpData(AFile *pFile,DWORD dwVersion);
	bool ImpData(AFile *pFile,DWORD dwVersion);
	//  Copy object
	virtual CSceneObject*		CopyObject();
	AI_DATA						GetProperty(){ return m_Data; };
	void						SetProperty(const AI_DATA& data){ m_Data = data; };

	DWORD						GetNPCNum();
	AString                     GetNPCModelPath(int index);
	CECModel*                   GetNpcModel(int index);
	NPC_PROPERTY*				GetNPCProperty(int index); 
	NPC_PROPERTY*				GetNPCProperty(AString name); 
	void                        AddNPC(NPC_PROPERTY* npc);
	void                        DeleteNPC(int index);
	void                        ReleaseModels();
	void                        ReloadModels();
	int                         GetCopyFlags(){ return m_nCopy; }
	void                        SetCopyFlags( int nCopy){ m_nCopy = nCopy; }

private:
	AString                     GetModelPath(DWORD dwId);
	void                        DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	void						DrawRect();
	void                        DrawMounsterName();
	void						BuildProperty();

	void                        RenderModels(A3DViewport *pView);
	
	ASetTemplate<int>          area_type;
	ASetTemplate<int>          group_type;
	ASetTemplate<int>          loop_type;
	AI_DATA			m_Data;				//���������
	CPtrList        m_listNPC;			//�����е�NPC�б�
	CPtrList        m_listModels;        //�����ģ�͵��б�����Ԥ��ģ��
	AI_ID_FUNCS*    m_pIDSelFuncs;	
	CONTROLLER_ID_FUNCS *m_pControllerSelFuncs;
	AString         m_strNPCList;
	float           m_fArea;//temp
	int             m_nCopy;//���������������ǵڼ�������(0,1)
};




class CSceneFixedNpcGenerator :public CSceneObject 
{
public:
	enum NPC_TYPE
	{
		TYPE_TERRAIN_FOLLOW = 0,//����������
		TYPE_PLANE,//��ĳһƽ����
	};
	
	CSceneFixedNpcGenerator();
	virtual ~CSceneFixedNpcGenerator();

	virtual void	Render(A3DViewport* pA3DViewport);
	virtual void	Tick(DWORD timeDelta);
	virtual void	UpdateProperty(bool bGet);
	virtual void    Release();
	virtual void	Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void    Translate(const A3DVECTOR3& vDelta);
	virtual void	SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up);
		
	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	//Exp data
	bool ExpData(AFile *pFile,DWORD dwVersion);
	bool ImpData(AFile *pFile,DWORD dwVersion);

	//  Copy object
	virtual CSceneObject*		CopyObject();

	void			BuildProperty();

	inline FIXED_NPC_DATA  GetProperty(){ return m_Data; };
	inline void            SetProperty( const FIXED_NPC_DATA & data){ m_Data = data; };
	inline CECModel*       GetECModel(){ return m_pModel; };
	inline AString         GetECModelPath(){ return GetModelPath(m_Data.dwID); };
	void                   ReloadModel();
	AString                GetNPCName(DWORD dwId);
	int                    GetCopyFlags(){ return m_nCopy; }
	void                   SetCopyFlags( int nCopy){ m_nCopy = nCopy; }
	int					   GetSightRange(DWORD dwId);
private:
	
	AString         GetModelPath(DWORD dwId);
	
	FIXED_NPC_DATA m_Data;
	CECModel*      m_pModel; //Preview model
	FIXED_NPC_ID_FUNCS* m_pIDSelFuncs;
	CONTROLLER_NPC_ID_FUNCS *m_pControllerSelFuncs;
	NPC_BEZIER_SEL_ID_FUNCS *m_pBezierSelFuncs;
	ASetTemplate<int>   npc_type;
	ASetTemplate<int>   loop_type;
	ASetTemplate<int>   speed_type;
	AString m_strText;
	int m_nSight;
	int m_nCopy;//���������������ǵڼ�������(0,1)
};

#endif // !defined(AFX_SCENEAIGENERATOR_H__D446CAD6_1B77_4360_8FEF_560CEEDF2FF1__INCLUDED_)
