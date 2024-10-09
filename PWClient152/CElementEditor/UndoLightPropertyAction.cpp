// UndoLightPropertyAction.cpp: implementation of the CUndoLightPropertyAction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "UndoLightPropertyAction.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoLightPropertyAction::CUndoLightPropertyAction()
{
	m_pLight = NULL;
}

CUndoLightPropertyAction::~CUndoLightPropertyAction()
{

}

//	Release action
void CUndoLightPropertyAction::Release()
{
}

//	Undo action
bool CUndoLightPropertyAction::Undo()
{
	if(m_pLight)
	{
		LIGHTDATA data;
		m_pLight->GetLightData(data);
		m_pLight->SetLightData(m_lightData);
		m_lightData = data;

		m_pLight->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
	}
	return true;
}

//	Redo action
bool CUndoLightPropertyAction::Redo()
{
	return Undo();
}

void CUndoLightPropertyAction::GetLightData(CSceneLightObject *pLight)
{
	ASSERT(pLight);
	m_pLight = pLight;
	pLight->GetLightData(m_lightData);
}
