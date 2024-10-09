// ExpSceneObject.cpp: implementation of the ExpSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "ExpSceneObject.h"


//#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpSceneObject::CExpSceneObject()
{

}

CExpSceneObject::~CExpSceneObject()
{

}

bool CExpSceneObject::DoExport()
{
	AUX_ProgressLog("Start to export scene objects ...");
	AUX_ProgressLog("All scene objects done .");
	return true;
}

bool CExpSceneObject::Init(A3DVECTOR3 vOffset,const char *exppath,const char *expname)
{
	m_szPath = exppath;
	m_szName = expname;
	m_vOffset = vOffset;

	return true;
}
