// SceneInstanceBox.h: interface for the SceneInstanceBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEINSTANCEBOX_H__E2652C22_A8ED_450D_B360_F62871AC1236__INCLUDED_)
#define AFX_SCENEINSTANCEBOX_H__E2652C22_A8ED_450D_B360_F62871AC1236__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

struct INSTANCE_DATA
{
	int nSelfWorldId;//�Լ�����������ID
	int nWorldId;    //Ŀ������ID
	int nLimitLevel; //�ȼ�����
	A3DVECTOR3 vTargetPos; //Ŀ������λ��
	float fEdgeHei;
	float fEdgeLen;
	float fEdgeWth;
};

void init_instance_data( INSTANCE_DATA &dat);

class SceneInstanceBox : public CSceneObject
{
public:
	SceneInstanceBox();
	virtual ~SceneInstanceBox();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta){};
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void RotateX(float fRad){};
	virtual void RotateY(float fRad){};
	virtual void RotateZ(float fRad){};
	virtual void Rotate(const A3DVECTOR3& axis,float fRad){};
	//  Copy object
	virtual CSceneObject*		CopyObject();
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);

	INSTANCE_DATA &GetProperty(){ return m_Data; };
	void SetProperty( const INSTANCE_DATA& data){ m_Data = data; };
	void GetAABB(A3DVECTOR3 &vMin, A3DVECTOR3 &vMax);
	//�ж������Ƿ���������ཻ
	bool IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);

protected:
	void BuildProperty();

private:
	INSTANCE_DATA m_Data;
};

#endif // !defined(AFX_SCENEINSTANCEBOX_H__E2652C22_A8ED_450D_B360_F62871AC1236__INCLUDED_)
