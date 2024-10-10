// FWFireworkGraph.cpp: implementation of the FWFireworkGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFireworkGraph.h"
#include "Mesh.h"
#include "GLUPolygon.h"
#include "GLUTess.h"
#include "FWTextMesh.h"
#include "A3DGFXEx.h"
#include "A3DParticleSystemEx.h"
#include "A3DMultiPlaneEmitter.h"
#include "FWAssemblyCache.h"
#include "FWConfig.h"
#include "FWArchive.h"
#ifdef _FW_EDITOR
	#include "FWDialogGraphMenu.h"
	#include "FWDialogGraph.h"
#endif


#define new A_DEBUG_NEW



FW_IMPLEMENT_SERIAL(FWFireworkGraph, FWFireworkLeaf)



FWFireworkGraph::FWFireworkGraph()
{
	m_pMeshParam = new MeshParam;
	ZeroMemory(m_pMeshParam, sizeof(MeshParam));
	m_profile.Init(
		FWAssemblyCache::GetInstance()->GetDefaultProfileIDByType(
			FWAssemblyCache::TYPE_TEXT));
	m_fThickness = 1.f;
}

FWFireworkGraph::~FWFireworkGraph()
{
	SAFE_DELETE_ARRAY(m_pMeshParam->pVertices);
	SAFE_DELETE_ARRAY(m_pMeshParam->pIndices);
	SAFE_DELETE(m_pMeshParam);
}

bool FWFireworkGraph::CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime)
{
	FWAssembly *pAssembly = NULL;
	
	BEGIN_FAKE_WHILE;
	
	// load assembly
	pAssembly = LoadAndInitAssembly();
	CHECK_BREAK(pAssembly);
	
	// update assembly using textmesh
	A3DGFXElement *pEle = pAssembly->GetElementByName("Text");
	CHECK_BREAK(pEle);
	CHECK_BREAK(pEle->GetEleTypeId() == ID_ELE_TYPE_PARTICLE_MULTIPLANE);

	A3DParticleSystemEx *pPartSys = static_cast<A3DParticleSystemEx *>(pEle);
	A3DMultiPlaneEmitter *pEmmiter = static_cast<A3DMultiPlaneEmitter *>(pPartSys->GetEmitter());
	CHECK_BREAK(pEmmiter);

	pEmmiter->SetVertAndPlaneData(
		m_pMeshParam->pVertices,
		m_pMeshParam->nVerticesCount,
		m_pMeshParam->pIndices,
		m_pMeshParam->nIndicesCount);
	pPartSys->SetPoolSize(pEmmiter->CalcPoolSize());
	
	// do copy
	CHECK_BREAK(DoCopy(pSet, pAssembly, matTM, fStartTime));
	
	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	A3DRELEASE(pAssembly);

	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}


bool FWFireworkGraph::BuildDataFromOutline(const CCharOutline *pOutline)
{
	BEGIN_FAKE_WHILE;
	
	CGLUPolygon gluPolygon;
	CHECK_BREAK(gluPolygon.CreateFromCharOutline(pOutline));
	
	CGLUTess gluTess;
	CHECK_BREAK(gluTess.CreateFromGLUPolygon(&gluPolygon));
	
	CMesh mesh;
	CHECK_BREAK(mesh.Create3D(pOutline, &gluTess, m_fThickness, false));
	
	CHECK_BREAK(FillMeshParam(m_pMeshParam, &mesh) == 0);
	
    END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

#ifdef _FW_EDITOR

void FWFireworkGraph::PrepareDlg(FWDialogPropBase *pDlg)
{
	FWDialogGraph *pGraphDlg = (FWDialogGraph *) pDlg;
	
	pGraphDlg->m_profile = m_profile;
	pGraphDlg->m_fThickness = m_fThickness;
}

void FWFireworkGraph::UpdateFromDlg(FWDialogPropBase * pDlg)
{
	FWDialogGraph *pGraphDlg = (FWDialogGraph *) pDlg;

	m_profile = pGraphDlg->m_profile;
	m_fThickness = pGraphDlg->m_fThickness;
}

#endif

void FWFireworkGraph::Serialize(FWArchive &ar)
{
	FWFireworkLeaf::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fThickness;
	}
	else
	{
		ar >> m_fThickness;
	}
}
