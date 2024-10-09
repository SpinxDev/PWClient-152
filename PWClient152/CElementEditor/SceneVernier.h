// SceneVernier.h: interface for the CSceneVernier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEVERNIER_H__977CC462_0DB1_48D4_AA48_A7D1D6C105D4__INCLUDED_)
#define AFX_SCENEVERNIER_H__977CC462_0DB1_48D4_AA48_A7D1D6C105D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

class CSceneVernier :public CSceneObject
{
public:
	CSceneVernier();
	virtual ~CSceneVernier();
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void UpdateProperty(bool bGet);

	//	Load data
	virtual bool Load(CELArchive& ar){ return true;};
	//	Save data
	virtual bool Save(CELArchive& ar){ return true;};

	float m_fAreaSize;
	int   m_nShowPlane;//0:xz; 1:xy; 2:zy;
private:
	void BuildProperty();
	void DrawZxis();


	
};

#endif // !defined(AFX_SCENEVERNIER_H__977CC462_0DB1_48D4_AA48_A7D1D6C105D4__INCLUDED_)
