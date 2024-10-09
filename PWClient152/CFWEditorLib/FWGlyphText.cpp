// FWGlyphText.cpp: implementation of the FWGlyphText class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphText.h"
#include "FWAlgorithm.h"
#include "CodeTemplate.h"
#include "FWTextMesh.h"
#include "CharOutline.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "GLUTess.h"
#include "GLUPolygon.h"
#include "Mesh.h"
#include "FWFireworkGraph.h"
#include "FWGlyphList.h"
#include "FWArtCreator.h"
#include "FWArt.h"
#include <ocidl.h>

#ifdef _FW_EDITOR
	#include "FWFlatCollector.h"
	#include "FWView.h"
	#include "FWDialogText.h"
#endif

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


#define HANDLE_HALF_WIDTH 2

FW_IMPLEMENT_SERIAL(FWGlyphText, FWGlyphGraphBase)



FWGlyphText::FWGlyphText() :
	m_strText(_AL("完美世界")),
	m_strFontName(_AL("宋体")),
	m_nPointSize(40),
	m_nFontStyle(STYLE_NORMAL)
{
	for (int i = 0; i < 4; i++)
		m_points[i].x = m_points[i].y = 0;
	m_pArt = FWArtCreator::GetInstance()->CreateArt(FWArtCreator::ID_ART_DEFAULT);
	m_nLastSelIndexOfArt = -1;
}

FWGlyphText::~FWGlyphText()
{
	SAFE_DELETE(m_pArt);
}

CCharOutline * FWGlyphText::CreateOutline() const
{
	CCharOutline *pOutline = CreateOriginalOutline();
	
	if (pOutline) 
		TransposeOutline(pOutline);

	return pOutline;
}

CCharOutline * FWGlyphText::CreateOriginalOutline() const
{
	HGDIOBJ hOldObj = NULL;
	HFONT hFont = NULL;
	HDC hDC = NULL;
	CCharOutline * pOutline = new CCharOutline;
	
	BEGIN_FAKE_WHILE;
	
	// build font
	LOGFONT logFont;
	ZeroMemory(&logFont, sizeof(LOGFONT));
	logFont.lfHeight = m_nPointSize;
	logFont.lfWeight = (m_nFontStyle & STYLE_BOLD) ? FW_BOLD : FW_NORMAL;
	logFont.lfItalic = (m_nFontStyle & STYLE_ITALIC) ? true : false;
	logFont.lfCharSet = DEFAULT_CHARSET;
	a_strcpy(logFont.lfFaceName, m_strFontName);
	hFont = CreateFontIndirect(&logFont);
	CHECK_BREAK(hFont);
	
	// select into hdc
	// just "borrow" the hdc, so any window would be suitable
	hDC = ::GetDC(NULL);
	CHECK_BREAK(hDC);
	hOldObj = SelectObject(hDC, hFont);

	// build outline
	int xoffset = 0;
	for (int i = 0; i < m_strText.GetLength(); i++)
	{
		CCharOutline outline;
		outline.CreateFromChar(m_strText[i], hDC);

		APtrList<CCharContour *>* pList = outline.GetCharContoursList();
		ALISTPOSITION pos = pList->GetHeadPosition();
		while (pos)
		{
			CCharContour *pContour = pList->GetNext(pos);
			CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
			for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
			{
				pBuffer->m_paVertices[i][0] += xoffset - outline.GetMetrics().gmptGlyphOrigin.x;
				pBuffer->m_paVertices[i][1] *= -1;
			}

			// pass pointer
			pOutline->GetCharContoursList()->AddTail(pContour);
		}
		// remove pointers
		pList->RemoveAll();

		xoffset += outline.GetXOffset();
	}

	MakeOutlineArtistic(pOutline);

	ARectI rect = GetOutlineRect(pOutline);
	pOutline->Simplify(.85f, rect.Height() * .015f);

	END_FAKE_WHILE;
	
	BEGIN_ON_FAIL_FAKE_WHILE;

	SAFE_DELETE(pOutline);

	END_ON_FAIL_FAKE_WHILE;
	
	SelectObject(hDC, hOldObj);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDC);

	return pOutline;
}

ARectI FWGlyphText::GetHandleRect(int nIndex) const
{
	return ARectI(-1, -1, -1, -1);
}


void FWGlyphText::MoveHandleTo(int nHandle, APointI newPoint)
{
}


ARectI FWGlyphText::GetSizeRect() const
{
	ARectI rect = GetTrueRect();
	rect.Inflate(HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH);
	return rect;
}


void FWGlyphText::SetSizeRect(ARectI rect)
{
	rect.Deflate(HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH);
	
	// note : size can not be changed, only the position can!
	if (GetSizeRect().Width() != rect.Width() ||
		GetSizeRect().Height() != rect.Height())
		return;

	SetCentralPoint(rect.CenterPoint());
}


double FWGlyphText::GetAngle() const
{
	return FWGlyph::GetAngle();
}


void FWGlyphText::SetAngle(double angle)
{
	FWGlyph::SetAngle(angle);

	if (angle)
	{
		POINTF centerF = 
		{
			(m_points[0].x + m_points[3].x) * .5f,
				(m_points[0].y + m_points[1].y) * .5f
		};
		
		RotatePoint(m_points, m_pointsRotated, 4, centerF, -angle);
	}
}


ARectI FWGlyphText::GetTrueRect() const
{
	if (GetAngle())
		return GetMinRect(m_pointsRotated, 4);
	else
		return GetMinRect(m_points, 4);
}

int FWGlyphText::GetHitHandle(APointI point) const
{
	return 0;	
}


int FWGlyphText::GetHandleCount() const
{
	return 0;
}


APointI FWGlyphText::GetHandle(int nPoint) const
{
	return APointI(0, 0);
}


bool FWGlyphText::IsHit(const APointI& point) const
{
	const POINT *pts = GetAngle() ? m_pointsRotated : m_points;

	HRGN hRgn = ::CreatePolygonRgn((LPPOINT)pts, 4, WINDING);

	return ::PtInRegion(hRgn, point.x, point.y) != 0;
}


void FWGlyphText::SetCentralPoint(APointI point)
{
	APointI centerOld = GetCentralPoint();

	// virtually move to new point
	ARectI rect = GetSizeRect();
	rect.Offset(point.x - centerOld.x, point.y - centerOld.y);
//	// clamp to clientrect
//	FWAlgorithm::ClampRectToRect(rect, FWView::GetInstance()->GetClientRectLP());
	// get clamped center point
	point = rect.CenterPoint();

	// move rect
	APointI offset = point - centerOld;
	for (int i = 0; i < 4; i++)
	{
		m_points[i].x += offset.x;
		m_points[i].y += offset.y;
	}
	SetAngle(GetAngle());
}


void FWGlyphText::GenerateMeshParam()
{
	CCharOutline *pOutline = NULL;
	
	BEGIN_FAKE_WHILE;
	
	pOutline = CreateOriginalOutline();
	CHECK_BREAK(pOutline);

	// note : call UpdateRect() prior to TransposeOutline()
	// because it need the original outline to be the parameter
	UpdateRect(pOutline);
	TransposeOutline(pOutline);

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

void FWGlyphText::UpdateRect(CCharOutline *pOutline)
{
	// note : the outline position is at the original font coordinate
	
	// get size of outline in font coordinate
	ARectI rectOutline = GetOutlineRect(pOutline);
	
	// compute offset
	APointI centerOld = GetCentralPoint();
	APointI centerNew = rectOutline.CenterPoint();
	APointI offset = centerOld - centerNew;
	rectOutline.Offset(offset);

	// set rect to outline rect
	m_points[0].x = m_points[1].x = rectOutline.left;
	m_points[2].x = m_points[3].x = rectOutline.right;
	m_points[0].y = m_points[3].y = rectOutline.bottom;
	m_points[1].y = m_points[2].y = rectOutline.top;

	// rotate rect if needed
	SetAngle(GetAngle());
}

void FWGlyphText::TransposeOutline(CCharOutline *pOutline) const
{
	// note : the outline position is at the original font coordinate
	
	// get size of outline in font coordinate
	ARectI rectOutline = GetOutlineRect(pOutline);
	
	// compute offset from original outline to current position
	APointI centerOld = GetCentralPoint();
	APointI centerNew = rectOutline.CenterPoint();
	APointI offset = centerOld - centerNew;

	// move outline to current outline position
	if (offset.x != 0 || offset.y != 0)
	{
		APtrList<CCharContour *> * pContourList = pOutline->GetCharContoursList();
		ALISTPOSITION pos = pContourList->GetHeadPosition();
		while (pos)
		{
			CCharContour *pContour = pContourList->GetNext(pos);
			CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
			for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
			{
				pBuffer->m_paVertices[i][0] += offset.x;
				pBuffer->m_paVertices[i][1] += offset.y;
			}
		}
	}

	// rotate outline if needed
	if (GetAngle())
	{
		APointI center = GetCentralPoint();
		
		A3DMATRIX4 mat;
		mat.RotateAxis(
			A3DVECTOR3(center.x, center.y, 0.f), 
			A3DVECTOR3(0.f, 0.f, 1.f),
			-GetAngle());

		A3DVECTOR3 vec(0.f, 0.f, 0.f);
		APtrList<CCharContour *> * pContourList = pOutline->GetCharContoursList();
		ALISTPOSITION pos = pContourList->GetHeadPosition();
		while (pos)
		{
			CCharContour *pContour = pContourList->GetNext(pos);
			CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
			for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
			{
				vec.x = pBuffer->m_paVertices[i][0];
				vec.y = pBuffer->m_paVertices[i][1];
				vec = vec * mat;
				pBuffer->m_paVertices[i][0] = vec.x;
				pBuffer->m_paVertices[i][1] = vec.y;
			}
		}
	}

}



void FWGlyphText::Serialize(FWArchive& ar)
{
	FWGlyphGraphBase::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_strText;
		ar << m_strFontName;
		ar << (BYTE)m_nFontStyle;
		ar << (BYTE)m_nPointSize;
		if (GetAngle())
		{
			for (int i = 0; i < 4; i++)
				ar << (short)m_pointsRotated[i].x << (short)m_pointsRotated[i].y;
		}
		else
		{
			for (int i = 0; i < 4; i++)
				ar << (short)m_points[i].x << (short)m_points[i].y;
		}
		ar << m_pArt;
	}
	else
	{
		ar >> m_strText;
		ar >> m_strFontName;
		BYTE tmp = 0;
		ar >> tmp; m_nFontStyle = tmp;
		ar >> tmp; m_nPointSize = tmp;
		if (GetAngle())
		{
			for (int i = 0; i < 4; i++)
			{
				short tmp = 0;
				ar >> tmp; m_pointsRotated[i].x = tmp;
				ar >> tmp; m_pointsRotated[i].y = tmp;
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				short tmp = 0;
				ar >> tmp; m_points[i].x = tmp;
				ar >> tmp; m_points[i].y = tmp;
			}
		}

		SAFE_DELETE(m_pArt);
		ar >> (const FWObject *&)m_pArt;
	}
}

void FWGlyphText::SetText(const ACHAR * szText)
{
	m_strText = szText;
}

void FWGlyphText::SetFontName(const ACHAR * szFontName)
{
	m_strFontName = szFontName;
}

void FWGlyphText::SetPointSize(int nPoint)
{
	m_nPointSize = nPoint;
}

void FWGlyphText::SetFontStyle(int nStyle)
{
	m_nFontStyle = nStyle;
}

#ifdef _FW_EDITOR

void FWGlyphText::DrawHandles(FWView *pView) const
{
	const POINT *pts = GetAngle() ? m_pointsRotated : m_points;
	
	FWFlatCollector *pCollector = pView->GetFlatCollector();
	for (int i = 0; i < 4; i++)
	{
		APointI pt1(pts[i].x, pts[i].y);
		pt1.x -= HANDLE_HALF_WIDTH;
		pt1.y -= HANDLE_HALF_WIDTH;
		APointI pt2(pts[i].x, pts[i].y);
		pt2.x += HANDLE_HALF_WIDTH;
		pt2.y += HANDLE_HALF_WIDTH;
		ARectI rect(pt1.x, pt1.y, pt2.x, pt2.y);
		pView->LPtoDP(&rect);
		pCollector->DrawRectangle2D(rect, COLOR_BLACK, true, COLOR_BLACK);
	}
}

const char * FWGlyphText::GetDlgToShow()
{
	return TO_STRING(FWDialogText);
}

void FWGlyphText::PrepareDlg(FWDialogPropBase * pDlg)
{
	FWDialogText *pTextDlg = (FWDialogText *) pDlg;
	
	pTextDlg->m_strText = m_strText;
	pTextDlg->m_strFont = m_strFontName;
	pTextDlg->m_nPointSize = m_nPointSize;
	pTextDlg->m_nFontStyle = m_nFontStyle;
	pTextDlg->SetArt(m_pArt);
	
	FWGlyph::PrepareDlg(pDlg);
}

void FWGlyphText::UpdateFromDlg(FWDialogPropBase * pDlg)
{
	FWDialogText *pTextDlg = (FWDialogText *) pDlg;
	
	m_strText = pTextDlg->m_strText;
	m_strFontName = pTextDlg->m_strFont;
	m_nPointSize = pTextDlg->m_nPointSize;
	m_nFontStyle = pTextDlg->m_nFontStyle;
	SAFE_DELETE(m_pArt);
	m_pArt = pTextDlg->GetArt()->Clone();
	
	GenerateMeshParam();
	
	FWGlyph::UpdateFromDlg(pDlg);
}

#endif


void FWGlyphText::MakeOutlineArtistic(CCharOutline *pOutline) const
{
	// put outline to center of coordinates
	ARectI rect = GetOutlineRect(pOutline);
	APointI center = rect.CenterPoint();

	APtrList<CCharContour *> *pContourList = pOutline->GetCharContoursList(); 
	ALISTPOSITION pos = pContourList->GetHeadPosition(); 
	while (pos) 
	{ 
		CCharContour *pContour = pContourList->GetNext(pos); 
		CCharVerticesBuffer *pBuffer = pContour->GetCharVerticesBuffer(); 
		for (int i = 0; i < pBuffer->m_nVerticesCount; i++) 
		{ 
			pBuffer->m_paVertices[i][0] -= center.x;
			pBuffer->m_paVertices[i][1] -= center.y;
		}
	}

	// make articstic
	m_pArt->MakeArtistic(pOutline);
}
