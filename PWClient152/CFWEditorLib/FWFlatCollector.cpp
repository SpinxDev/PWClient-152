#include "fwflatcollector.h"



FWFlatCollector::FWFlatCollector(void)
{
}

FWFlatCollector::~FWFlatCollector(void)
{
	Release();
}

bool FWFlatCollector::DrawRectangle2D(ARectI rect, DWORD dwLineColor, bool bFill, DWORD dwFillColor, bool bFlush)
{
	if (bFill)
	{
		AddRect_2D(
			rect.left,
			rect.top,
			rect.right,
			rect.bottom,
			dwLineColor);
		AddRect_2D(
			rect.left + 1,
			rect.top + 1,
			rect.right - 1,
			rect.bottom - 1,
			dwFillColor);
	}
	else
	{
		AddRect_2D(
			rect.left,
			rect.top,
			rect.left + 1,
			rect.bottom,
			dwLineColor);
		AddRect_2D(
			rect.left,
			rect.top,
			rect.right,
			rect.top + 1,
			dwLineColor);
		AddRect_2D(
			rect.right - 1,
			rect.top,
			rect.right,
			rect.bottom,
			dwLineColor);
		AddRect_2D(
			rect.left,
			rect.bottom - 1,
			rect.right,
			rect.bottom,
			dwLineColor);
	}

	if (bFlush) Flush_2D();
	
	return true;
}

bool FWFlatCollector::DrawLine(APointI pt1, APointI pt2, DWORD dwColor, bool bFlush)
{
	A3DVECTOR3 verts[4];
	if (abs(pt1.y - pt2.y) < abs(pt1.x - pt2.x))
	{
		verts[0].x = (float)pt1.x;
		verts[0].y = (float)pt1.y;
		verts[0].z = (float)0.f;
		verts[1].x = (float)pt1.x;
		verts[1].y = (float)pt1.y + 1;
		verts[1].z = (float)0.f;
		verts[2].x = (float)pt2.x;
		verts[2].y = (float)pt2.y;
		verts[2].z = (float)0.f;
		verts[3].x = (float)pt2.x;
		verts[3].y = (float)pt2.y - 1;
		verts[3].z = (float)0.f;
	}
	else
	{
		verts[0].x = (float)pt1.x;
		verts[0].y = (float)pt1.y;
		verts[0].z = (float)0.f;
		verts[1].x = (float)pt1.x + 1;
		verts[1].y = (float)pt1.y;
		verts[1].z = (float)0.f;
		verts[2].x = (float)pt2.x;
		verts[2].y = (float)pt2.y;
		verts[2].z = (float)0.f;
		verts[3].x = (float)pt2.x - 1;
		verts[3].y = (float)pt2.y;
		verts[3].z = (float)0.f;
	}

	WORD indices[6] = 
	{
		0, 1, 2,
		0, 2, 3,
	};

	A3DVECTOR3 v01 = verts[1] - verts[0];
	A3DVECTOR3 v12 = verts[2] - verts[1];
	A3DVECTOR3 v012; v012.CrossProduct(v01, v12);
	if (v012.z < 0)
	{
		indices[1] = 2;
		indices[2] = 1;
	}

	A3DVECTOR3 v02 = verts[2] - verts[0];
	A3DVECTOR3 v23 = verts[3] - verts[2];
	A3DVECTOR3 v023; v023.CrossProduct(v02, v23);
	if (v023.z < 0)
	{
		indices[4] = 3;
		indices[5] = 2;
	}


	AddRenderData_2D(verts, 4, indices, 6, dwColor);

	if (bFlush) Flush_2D();

	return true;
}
