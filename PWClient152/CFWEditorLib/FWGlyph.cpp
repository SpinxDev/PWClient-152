#include "FWGlyph.h"
#include "FWAlgorithm.h"
#include "FWTextMesh.h"
#include "CharOutline.h"
#include "CodeTemplate.h"
#include "GLUPolygon.h"
#include "GLUTess.h"
#include "Mesh.h"
#include "FWFirework.h"
#include "FWGlyphList.h"
#include "FWAssemblySet.h"
#include "A3DGFXEditorInterface.h"
#include "Global.h"
#include <A3DDevice.h>

#ifdef _FW_EDITOR
	#include "FWDialogPropBase.h"
	#include "FWView.h"
	#include "FWStream.h"
	#include "FWFlatCollector.h"
#endif


#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNAMIC(FWGlyph, FWObject)

const int FWGlyph::DEFAULT_HANDLE_RADIUS = 3;

FWGlyph::FWGlyph():
	m_fAngle(0),
	m_pFirework(NULL)
{
	m_pMeshParam = new MeshParam;
	ZeroMemory(m_pMeshParam, sizeof(MeshParam));
}

FWGlyph::~FWGlyph()
{
	SAFE_DELETE_ARRAY(m_pMeshParam->pIndices);
	SAFE_DELETE_ARRAY(m_pMeshParam->pVertices);
	SAFE_DELETE(m_pMeshParam);
	SAFE_DELETE(m_pFirework);
}

//Get the central point of the glyph
APointI FWGlyph::GetCentralPoint() const
{
	return GetSizeRect().CenterPoint();
}



//This protected member is called by the 
//default implement of DrawHandles() and 
//GetHitHandle(). It provides the rect of 
//the handle of the index.
//The index is 1 based.
ARectI FWGlyph::GetHandleRect(int nIndex) const
{
	APointI point = GetHandle(nIndex);
	return ARectI(point.x - DEFAULT_HANDLE_RADIUS , point.y - DEFAULT_HANDLE_RADIUS , 
		point.x + DEFAULT_HANDLE_RADIUS , point.y + DEFAULT_HANDLE_RADIUS );
}

//Move the glyph to a new position where its 
//central point is specified by parameter 
//"point".
void FWGlyph::SetCentralPoint(APointI point)
{
	ARectI rect = GetSizeRect();
	int Height = rect.Height();
	int Width = rect.Width();
	rect.top = point.y - Height / 2;
	rect.bottom = rect.top + Height;
	rect.left = point.x - Width / 2;
	rect.right = rect.left + Width;
	SetSizeRect(rect);
}

//Test whether the point is on one of the 
//handles. Return 
//zero if no handle is hit, else return the 
//index of the handle.
//The index is 1 based.
int FWGlyph::GetHitHandle(APointI point) const
{
	int nHandleCount = GetHandleCount();
	for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
	{
		// GetHandleRect returns in logical coords
		ARectI rc = GetHandleRect(nHandle);
		if (rc.PtInRect(point))
			return nHandle;
	}
	return 0;
}





//Get the cursor to display to the user when 
//the mouse is on one of the handles.
//nIndex is the index of the handle.
//The index is 1 based.
HCURSOR FWGlyph::GetHandleCursor(int nIndex) const
{
	return ::LoadCursor(NULL, IDC_SIZEALL);
}

//Return the current angle of the glyph.
//The size rect should be upright when angle 
//= 0
//The Angle is measured by radian
double FWGlyph::GetAngle() const
{
	return m_fAngle;
}

//Rotate the glyph to a new angle. When 
//rotato, 
//the centre of the rotation should be the 
//point 
//returned by GetCentralPoint().
//NOTE:Parameter "angle" is an absolute 
//angle.
//The size rect should be upright when angle 
//= 0
//The Angle is measured by radian
void FWGlyph::SetAngle(double angle)
{
	m_fAngle = angle;
}



void FWGlyph::GenerateMeshParam()
{
	CCharOutline *pOutline = NULL;
	
	BEGIN_FAKE_WHILE;

	pOutline = CreateOutline();
	CHECK_BREAK(pOutline);

	CGLUPolygon gluPolygon;
	CHECK_BREAK(gluPolygon.CreateFromCharOutline(pOutline));

	CGLUTess gluTess;
	CHECK_BREAK(gluTess.CreateFromGLUPolygon(&gluPolygon));

	CMesh mesh;
	CHECK_BREAK(mesh.CreateFromGLUTess(&gluTess));

	mesh.MakeAntiClockwise();

	CHECK_BREAK(FillMeshParam(m_pMeshParam, &mesh) == 0);

    END_FAKE_WHILE;

	SAFE_DELETE(pOutline);
}

const FWAssemblyProfile & FWGlyph::GetAssemblyProfile() const
{
	return m_pFirework->GetAssemblyProfile();
}

void FWGlyph::SetAssemblyProfile(const FWAssemblyProfile &src)
{
	m_pFirework->SetAssemblyProfile(src);
}

bool FWGlyph::CopyTo(FWAssemblySet *pSet, float fScale, const A3DMATRIX4& matTM, float fStartTime)
{
	BEGIN_FAKE_WHILE;

	CHECK_BREAK(PrepareFireworkData(fScale));

	CHECK_BREAK(m_pFirework->CopyTo(pSet, matTM, fStartTime));

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

void FWGlyph::Init(FWFirework * pFirework, ACString strName)
{
	m_pFirework = pFirework;
	m_strName = strName;
}


void FWGlyph::Serialize(FWArchive &ar)
{
	FWObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fAngle;
		ar << m_strName;
		ar << m_pFirework;
	}
	else
	{
		ar >> m_fAngle;
		ar >> m_strName;
		SAFE_DELETE(m_pFirework);
		ar >> (const FWObject *&)m_pFirework;
	}
}

float FWGlyph::GetOffsetTime(int nIndex) const 
{ 
	return m_pFirework->GetOffsetTime(nIndex); 
}

const A3DVECTOR3 & FWGlyph::GetOffsetPos(int nIndex) const 
{
	return m_pFirework->GetOffsetPos(nIndex); 
}

void FWGlyph::SetOffsetTime(int nIndex, float fOffsetTime) 
{
	m_pFirework->SetOffsetTime(nIndex, fOffsetTime); 
}

void FWGlyph::SetOffsetPos(int nIndex, const A3DVECTOR3 & vOffsetPos) 
{
	m_pFirework->SetOffsetPos(nIndex, vOffsetPos);
}	

A3DVECTOR3 FWGlyph::GetSumOfOffsetPos()
{
	return m_pFirework->GetSumOfOffsetPos();
}

float FWGlyph::GetSumOfOffsetTime()
{
	return m_pFirework->GetSumOfOffsetTime();
}

int FWGlyph::GetParticalCount()
{
	return m_pFirework->GetParticalCount();
}

int FWGlyph::GetSoundCount()
{
	return m_pFirework->GetSoundCount();
}


#ifdef _FW_EDITOR

void FWGlyph::Draw(FWView *pView) const
{
	if (m_pMeshParam->nIndicesCount == 0 ||
		m_pMeshParam->nVerticesCount == 0)
		return;
	
	int i;
	
	FWStream *pStream = pView->GetFWStream();
	
	BEGIN_FAKE_WHILE;
	
	CHECK_BREAK(pStream->QueryIndexBuf(m_pMeshParam->nIndicesCount));
	
	CHECK_BREAK(pStream->QueryVertBuf(m_pMeshParam->nVerticesCount));
	
	// set indices
	WORD *pIndices = NULL;
	CHECK_BREAK(pStream->LockIndexBuffer(
		0, 
		m_pMeshParam->nIndicesCount * sizeof(WORD),
		(BYTE **)&pIndices, 
		0));
	for (i = 0; i < m_pMeshParam->nIndicesCount; i++)
		pIndices[i] = m_pMeshParam->pIndices[i];
	CHECK_BREAK(pStream->UnlockIndexBuffer());
	
	// set vertices
	A3DTLVERTEX *pVert = NULL;
	CHECK_BREAK(pStream->LockVertexBuffer(
		0, 
		m_pMeshParam->nVerticesCount * sizeof(A3DTLVERTEX),
		(BYTE **)&pVert,
		0));
	A3DVECTOR3 *pSrcVert = m_pMeshParam->pVertices;
	for (i = 0; i < m_pMeshParam->nVerticesCount; i++)
	{
		pVert->x = pSrcVert->x;
		pVert->y = pSrcVert->y;
		pView->LPtoDP(pVert->x, pVert->y);
		pVert->z = 0.f;
		pVert->rhw = 1.f;
		pVert->diffuse = COLOR_DARK_GRAY;
		pVert->specular = 0;
		pVert->tu = 0;
		pVert->tv = 0;
		pVert++;
		pSrcVert++;
	}
	CHECK_BREAK(pStream->UnlockVertexBuffer());
	
	pStream->Appear();
	pView->GetA3DDevice()->DrawIndexedPrimitive(
		A3DPT_TRIANGLELIST,
		0,
		m_pMeshParam->nVerticesCount,
		0,
		m_pMeshParam->nIndicesCount / 3);
	
	END_FAKE_WHILE;
}

void FWGlyph::DrawHandleRect(ARectI rect, FWView *pView) const
{
	pView->LPtoDP(&rect);
	
	pView->GetFlatCollector()->DrawRectangle2D(
		rect,
		COLOR_BLACK,
		true);
}


//Draw all the handles onto the screen.
void FWGlyph::DrawHandles(FWView *pView) const
{
	
	
	int nHandleCount = GetHandleCount();
	for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
	{
		APointI handle = GetHandle(nHandle);
		DrawHandleRect(GetHandleRect(nHandle), pView);
	}
}


const char * FWGlyph::GetDlgToShow()
{
	return m_pFirework->GetDlgToShow();
}

void FWGlyph::PrepareDlg(FWDialogPropBase *pDlg)
{
	pDlg->m_strName = m_strName;
	pDlg->m_fTime = GetFirework()->GetOffsetTime(FWFirework::OFFSET_INDEX_BY_GLYPH);
	
	m_pFirework->PrepareDlg(pDlg);
}

void FWGlyph::UpdateFromDlg(FWDialogPropBase * pDlg)
{
	m_strName = pDlg->m_strName;
	GetFirework()->SetOffsetTime(FWFirework::OFFSET_INDEX_BY_GLYPH, pDlg->m_fTime);
	
	m_pFirework->UpdateFromDlg(pDlg);
}

const char * FWGlyph::GetContextMenu()
{
	return NULL;
}

#endif
