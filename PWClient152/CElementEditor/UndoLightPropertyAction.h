// UndoLightPropertyAction.h: interface for the CUndoLightPropertyAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOLIGHTPROPERTYACTION_H__401D4EED_5195_47C0_A941_2698212B0CEC__INCLUDED_)
#define AFX_UNDOLIGHTPROPERTYACTION_H__401D4EED_5195_47C0_A941_2698212B0CEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoAction.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "A3DTypes.h"
#include "SceneLightObject.h"

class CUndoLightPropertyAction :public CUndoAction   
{

public:
	CUndoLightPropertyAction();
	virtual ~CUndoLightPropertyAction();


	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

	void GetLightData(CSceneLightObject *pLight);
private:
	LIGHTDATA m_lightData;
	CSceneLightObject *m_pLight;
};

#endif // !defined(AFX_UNDOLIGHTPROPERTYACTION_H__401D4EED_5195_47C0_A941_2698212B0CEC__INCLUDED_)
