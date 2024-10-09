// SceneDummyObject.h: interface for the CSceneDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEDUMMYOBJECT_H__910241E2_2404_42DB_B3A2_767C77BF5ACD__INCLUDED_)
#define AFX_SCENEDUMMYOBJECT_H__910241E2_2404_42DB_B3A2_767C77BF5ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

class CSceneDummyObject : public CSceneObject 
{

public:
	CSceneDummyObject();
	virtual ~CSceneDummyObject();

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void UpdateProperty(bool bGet);

	//  Copy object
	virtual CSceneObject*		CopyObject();
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
private:
	void BuildProperty();
	float m_fScale;
	float m_fWidth;
};

#endif // !defined(AFX_SCENEDUMMYOBJECT_H__910241E2_2404_42DB_B3A2_767C77BF5ACD__INCLUDED_)
