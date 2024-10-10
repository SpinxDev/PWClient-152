 /*
 * FILE: aabbcd.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: wang kuiwu, 2005/8/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */
#include "aabbcd.h"
//#include "a3dtrace.h"
#include "Face.h"

#include <A3DFlatCollector.h>

#include "afile.h"

namespace CHBasedCD
{


bool ClipBrush(BrushTraceInfo * pInfo, CCDSide * pSides, int nSides);
//@note : turn on this flag to indicate that use bevel during raytracing.
//        By Kuiwu[24/8/2005]
//#define _USE_BEVEL_RAYTRACE_

bool RayTraceBrush(BrushTraceInfo * pInfo, CCDSide * pSides, int nSides);

//////////////////////////////////////////////////////////////////////////
// member function of CCDBrush
//////////////////////////////////////////////////////////////////////////

bool CCDBrush::Load(AFile * pFileToLoad)
{
	assert(pFileToLoad);
	Release();
	DWORD readLen;
	//aabb
	pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &readLen);
	//reserved
	pFileToLoad->Read(&m_dwReserved, sizeof(DWORD), &readLen);
	//side
	pFileToLoad->Read(&m_nSides, sizeof(int), &readLen);
	assert(m_nSides > 0);
	m_pSides = new CCDSide[m_nSides];
	assert(m_pSides);
	for (int i = 0; i < m_nSides; i++) {
		A3DVECTOR3 vNormal;
		pFileToLoad->Read(&vNormal, sizeof(A3DVECTOR3), &readLen);
		float fDist;
		pFileToLoad->Read(&fDist, sizeof(float), &readLen);
		bool bBevel;
		pFileToLoad->Read(&bBevel, sizeof(bool), &readLen);
		
		m_pSides[i].Init(vNormal, fDist, bBevel);
	}
	
	return true;
}

bool CCDBrush::Save(AFile * pFileToSave)
{
	assert(pFileToSave);
	DWORD wrtLen;
	//aabb
	pFileToSave->Write(&m_aabb, sizeof(A3DAABB), &wrtLen);
	//reserved
	pFileToSave->Write(&m_dwReserved, sizeof(DWORD), &wrtLen);
	//side
	pFileToSave->Write(&m_nSides, sizeof(int), &wrtLen);
	for (int i = 0; i < m_nSides; i++) {
		A3DVECTOR3 vNormal(m_pSides[i].plane.GetNormal());
		pFileToSave->Write(&vNormal, sizeof(A3DVECTOR3), &wrtLen);
		float fDist = m_pSides[i].plane.GetDist();
		pFileToSave->Write(&fDist, sizeof(float), &wrtLen);
		bool bBevel = m_pSides[i].bevel;
		pFileToSave->Write(&bBevel, sizeof(bool), &wrtLen);
	}

	return true;
}

bool CCDBrush::Trace(BrushTraceInfo * pInfo)
{
	if (pInfo->bIsPoint) {
		return  RayTraceBrush(pInfo, m_pSides, m_nSides);
	}
	else{
		return ClipBrush(pInfo, m_pSides, m_nSides);
	}
}
//////////////////////////////////////////////////////////////////////////
// member function of CQBrush
//////////////////////////////////////////////////////////////////////////

CQBrush::~CQBrush()
{
	Release();
}
void CQBrush::Release()
{
	m_pCHData = NULL;
	for (int i= 0; i< m_nQPlane; i++) {
		 //if (m_pQPlane[i].bBevel && m_pQPlane[i].pHS) {
		if ( m_pQPlane[i].pHS) {
			 delete m_pQPlane[i].pHS;
		 }
	}
	m_nQPlane = 0;

}
void CQBrush::AddBrushBevels(CConvexHullData * pCHData)
{
	Release();
	assert(pCHData!= NULL);
	m_pCHData = pCHData;

	_FlushCH();

#if 1
	int		axis, dir;
	int		i, order;
	A3DVECTOR3  vNormal;
	float       fDist;

	A3DAABB aabb; 
	bool bResult = m_pCHData->GetAABB(aabb);
	assert(bResult);
	QPlane sidetemp;

#define FLOAT_EQUAL(a, b, epsilon)    (fabs((a) - (b)) < (epsilon))

	//
	// add the axial planes
	//
	order = 0;
	for (axis=0 ; axis <3 ; axis++){
		for (dir=-1 ; dir <= 1 ; dir+=2, order++){
			// see if the plane is allready present
			for (i=0; i< m_nQPlane ; i++){
				 vNormal = m_pQPlane[i].pHS->GetNormal();
				if (FLOAT_EQUAL(dir, vNormal.m[axis], 1E-5f)) {
					break;
				}
			}
			
			if (i == m_nQPlane){	
				// add a new side
				assert(m_nQPlane < MAX_FACE_IN_HULL);
				vNormal.Clear();
				vNormal.m[axis] = (float)dir;
				if (dir == 1) {
					fDist = aabb.Maxs.m[axis];
				}
				else{
					fDist = -aabb.Mins.m[axis];
				}
				m_pQPlane[m_nQPlane].pHS  = new CHalfSpace();
				m_pQPlane[m_nQPlane].pHS->SetNormal(vNormal);
				m_pQPlane[m_nQPlane].pHS->SetD(fDist);
				m_pQPlane[m_nQPlane].bBevel = true;
				m_nQPlane++;
			}

			// if the plane is not in it canonical order, swap it
			if (i != order)	{
				//@note: kuiwu trust the compiler op=
				sidetemp = m_pQPlane[order];
				m_pQPlane[order] = m_pQPlane[i];
				m_pQPlane[i] = sidetemp;
			}
		}
	}

	//
	// add the edge bevels
	//
	if (m_nQPlane == 6) {
		return; // pure axial
	}

	// test the non-axial plane edges
	for (i = 6; i < m_nQPlane; i++) {
		QPlane * pQPlane = &m_pQPlane[i];
		// assert(!pQPlane->bBevel);
		if(pQPlane->bBevel)
			continue;
		
		CFace * pFace = m_pCHData->GetFacePtr(pQPlane->iCHIndex);
		int j;
		int vNum = pFace->GetVNum();
		for (j = 0; j < vNum; j++) {
			A3DVECTOR3 v0, v1;
			int index = pFace->GetVID(j);
			v0 = m_pCHData->GetVertex(index);
			int k = (j +1) % vNum;
			index = pFace->GetVID(k);
			v1 = m_pCHData->GetVertex(index);
			A3DVECTOR3 vDir(v1 -v0);
			float mag = vDir.Normalize();
			if (mag < 0.5f) {
				continue;
			}
			vDir.Snap();
			for (k = 0; k < 3; k++) {
				if (vDir.m[k] == -1 || vDir.m[k] == 1) {
					break; //axial
				}
			}
			if (k != 3 ) {
				continue; // only test non-axial edges
			}
			// try the six possible slanted axials from this edge
			for (axis=0 ; axis <3 ; axis++){
				for (dir=-1 ; dir <= 1 ; dir+=2){
					v1.Clear();
					v1.m[axis] = (float) dir;
					//@note: kuiwu refine me
					//       maybe negative!!!
					vNormal = CrossProduct(vDir, v1);
					mag = vNormal.Normalize();
					if (mag < 0.5) {
						continue;
					}
					fDist = DotProduct(v0, vNormal);
					// if all the points on all the sides are
					// behind this plane, it is a proper edge bevel
					for (k = 0;  k < m_nQPlane; k++) {
							QPlane * pQPlane = &m_pQPlane[k];
							A3DVECTOR3 vHSNormal(pQPlane->pHS->GetNormal());
							float      fHSDist = pQPlane->pHS->GetDist();
							if (FLOAT_EQUAL(vHSNormal.x, vNormal.x, 0.01)
								&& FLOAT_EQUAL(vHSNormal.y, vNormal.y, 0.01)
								&& FLOAT_EQUAL(vHSNormal.z, vNormal.z, 0.01)
								&& FLOAT_EQUAL(fHSDist, fDist, 0.01)) {
								break;
							}
							if (pQPlane->bBevel) {
								continue;
							}
							CFace * pFace = m_pCHData->GetFacePtr(pQPlane->iCHIndex);
							int l;
							int vNum = pFace->GetVNum();
							for ( l = 0; l < vNum; l++) {
								int index = pFace->GetVID(l);
								A3DVECTOR3 vert(m_pCHData->GetVertex(index));
								float d = DotProduct(vert, vNormal) - fDist;
								if (d > 0.1f) {
									break; // point in front
								}
							}

							if (l != vNum) {
								break;
							}
					}
					if (k != m_nQPlane) {
						continue;
					}
					// add this plane
					assert(m_nQPlane < MAX_FACE_IN_HULL );
					m_pQPlane[m_nQPlane].pHS  = new CHalfSpace();
					m_pQPlane[m_nQPlane].pHS->SetNormal(vNormal);
					m_pQPlane[m_nQPlane].pHS->SetD(fDist);
					m_pQPlane[m_nQPlane].vert = v0;
					m_pQPlane[m_nQPlane].bBevel = true;
					m_nQPlane++;
				}
			}
	

		}
	}	

#undef  FLOAT_EQUAL
#endif
}


void CQBrush::_FlushCH()
{
	assert(m_pCHData != NULL && m_nQPlane == 0);
	int nFace = m_pCHData->GetFaceNum();
	assert(nFace > 0);
	int i;
	for (i = 0; i < nFace; i++, m_nQPlane++) {
		m_pQPlane[i].pHS = new CHalfSpace();
		A3DVECTOR3 vNormal(m_pCHData->GetFacePtr(i)->GetNormal());
		//vNormal.Snap();
		m_pQPlane[i].pHS->SetNormal(vNormal);
		m_pQPlane[i].pHS->SetD( m_pCHData->GetFacePtr(i)->GetDist());
		m_pQPlane[i].iCHIndex = i;
		m_pQPlane[i].bBevel = false;
	}

}




/*
 *	dynamic collision detect between aabb and brush 
 *  @note:  only consider the aabb, for raytrace case @ref RayTraceBrush
 *  @todo:  refine me and RayTraceBrush!  It is not a good way to keep duplicate code!!!  
 *  @author: kuiwu
 *  @ref:   quake2, quake3 source code and  RayTraceBrush
 */
bool ClipBrush(BrushTraceInfo * pInfo, CCDSide * pSides, int nSides)
{

	int			i, j;
	CHalfSpace	*plane, *clipplane;
	float		dist;
	float		enterfrac, leavefrac;
	A3DVECTOR3	ofs;
	float		d1, d2;
	bool	getout, startout;
	float		f;
	

	//@note: kuiwu, make sure the input
	assert(pSides != NULL && nSides > 0  && !pInfo->bIsPoint);

	A3DVECTOR3 vMaxs(pInfo->vExtents);
	A3DVECTOR3 vMins(-pInfo->vExtents);

	enterfrac = -1;
	leavefrac = 1;
	clipplane = NULL;
	

	getout = false;
	startout = false;
	A3DVECTOR3 p1(pInfo->vStart),p2(pInfo->vStart+pInfo->vDelta);

	for (i=0; i<nSides; i++)
	{
		plane = &(pSides[i].plane);
		A3DVECTOR3 vNormal = plane->GetNormal();

		// FIXME: special case for axial


		// push the plane out apropriately for mins/maxs
		// FIXME: use signbits into 8 way lookup for each mins/maxs
		for (j=0 ; j<3 ; j++)
		{
			if (vNormal.m[j] < 0)
				ofs.m[j] = vMaxs.m[j];
			else
				ofs.m[j] = vMins.m[j];
		}
		dist = DotProduct (ofs, vNormal);
		dist = plane->GetDist() - dist;

		d1 = DotProduct (p1, vNormal) - dist;
		d2 = DotProduct (p2, vNormal) - dist;

		if (d2 > 0)
			getout = true;	// endpoint is not in solid
		if (d1 > 0)
			startout = true;

		// if completely in front of face, no intersection
		if (d1 > 0 && d2 >= d1)
			return false;

		if (d1 <= 0 && d2 <= 0)
			continue;

		// crosses face
		if (d1 > d2)
		{	// enter
			//@note: kuiwu  2005/08/17  d1 - TraceInfo.fDistEpsilon maybe less than 0.0f 
			//@ref:  hl2 source code  & q3 code
			//assert(d1 - TraceInfo.fDistEpsilon > 0.0f);
			f = (d1 - pInfo->fDistEpsilon)/(d1-d2);
			if (f < 0.0f) {
				f = 0.0f;
			}
			if (f > enterfrac)
			{
				enterfrac = f;
				clipplane = plane;
				pInfo->iClipPlane = i;
				// leadside = side;
			}
		}
		else
		{	// leave
			f = (d1+pInfo->fDistEpsilon) / (d1-d2);
			//@note: kuiwu, 2005/8/20, 
			//@ref: q3 code
			if (f > 1.0f) {
				f = 1.0f;
			}

			if (f < leavefrac)
				leavefrac = f;
		}
	}

	if (!startout)
	{	// original point was inside brush
		pInfo->bStartSolid= true;
		if (!getout)
			pInfo->bAllSolid = true;
		//@note: added by kuiwu
		pInfo->fFraction  = 0.0f;
		
		return true;
	}

	if (enterfrac < leavefrac)
	{
		// if (enterfrac > -1 && enterfrac < trace->fraction)
		if (enterfrac > -1 && enterfrac < 1) 
		{
			if (enterfrac < 0)
				enterfrac = 0;
			pInfo->fFraction= enterfrac;
			pInfo->ClipPlane = *clipplane;
			// trace->surface = &(leadside->surface->c);
			// trace->contents = brush->contents;

			return true;
		}
	}
	
	return false;

	
}

/*
 *	@note: kuiwu, exist only for complability
 */
bool ClipAABBToBrush(AABBBrushTraceInfo& TraceInfo)
{

	CCDBrush cdBrush;
	TraceInfo.pBrush->Export(&cdBrush);
	return cdBrush.Trace(&TraceInfo);
}

//////////////////////////////////////////////////////////////////////////
// render routines
void CQBrush::RenderPlane(A3DFlatCollector* pFC, const A3DVECTOR3& vPos, const A3DVECTOR3& vNormal, DWORD dwColor, float fUpExtent, float fRightExtent)
{
	// project the vDir to the plane 
	A3DVECTOR3 vDir(1.0f,0.0f,0.0f), vUp;
	float d = DotProduct(vDir,vNormal);
	if( d > 1.0f -1e-5f)
	{
		vDir = A3DVECTOR3(0.0f,1.0f,0.0f);
		d = DotProduct(vDir, vNormal);
	}

	vDir -= d * vNormal;

	// now, vDir is a vector on the plane!
	vDir.Normalize();
	vUp = CrossProduct(vDir, vNormal);

	A3DVECTOR3 vExtents(0.1f, fUpExtent, fRightExtent);
	pFC->AddBox_3D(vPos, vDir, vUp, vNormal,vExtents,dwColor);
}

void CQBrush::Render(A3DFlatCollector* pFC,  bool bRenderAABBBevels, DWORD dwBevelCol,float fUpExtents,float fRightExtents, bool bRenderBevels, bool bRenderCH)
{

	if(bRenderAABBBevels)
	{
		A3DAABB aabb;
		m_pCHData->GetAABB(aabb);
		pFC->AddAABB_3D(aabb, 0x60ff0000);
	}

	if(bRenderCH)
	{
		m_pCHData->Render(pFC);
	}

	if(bRenderBevels)
	{
		// render the non-aabb bevels!
		for(int i=6; i< m_nQPlane; ++i)
		{
			QPlane * pQPlane = m_pQPlane+i;
			if(pQPlane->bBevel)
			{
				RenderPlane(pFC, pQPlane->vert, pQPlane->pHS->GetNormal(),dwBevelCol,fUpExtents,fRightExtents);
			}
		}
	}
}


void CQBrush::Transform(const A3DMATRIX4& mtxTrans)
{
	for(int i=0; i< m_nQPlane; ++i)
	{
		// the first-6 axial plane, we only transform the plane
		m_pQPlane[i].pHS->Transform(mtxTrans);
		if(i>5)
		{
			// additionally, we should transform the vertex
			if(m_pQPlane[i].bBevel)
			{
				m_pQPlane[i].vert = m_pQPlane[i].vert * mtxTrans;
			}
		}
	}
}

void CQBrush::Export(CCDBrush *pCDBrush)
{
	//@note: added by kuiwu
	assert(pCDBrush != NULL);
	pCDBrush->Release();
	pCDBrush->m_nSides = m_nQPlane;
	pCDBrush->m_pSides = new CCDSide[m_nQPlane];
	for( int i=0; i<m_nQPlane; ++i){
		pCDBrush->m_pSides[i].Init(*m_pQPlane[i].pHS, m_pQPlane[i].bBevel);
	}
	m_pCHData->GetAABB(pCDBrush->m_aabb);

	pCDBrush->m_dwReserved = m_pCHData->GetFlags();
}

/*
 *	special case for ray trace
 *  @note: raytrace brush with bevel does not work occasionally.
 *         That confused me!
 *  @author: kuiwu
 *  @ref:   quake2 , quake3 source code
 */
bool RayTraceBrush(BrushTraceInfo * pInfo, CCDSide * pSides, int nSides)
{

	int			i;
	CHalfSpace	*plane, *clipplane;
	float		dist;
	float		enterfrac, leavefrac;
	A3DVECTOR3	ofs;
	float		d1, d2;
	bool	getout, startout;
	float		f;
	

	//@note: kuiwu, make sure the input
	assert(pSides != NULL && nSides > 0 && pInfo->bIsPoint);
	enterfrac = -1;
	leavefrac = 1;
	clipplane = NULL;
	

	getout = false;
	startout = false;
	A3DVECTOR3 p1(pInfo->vStart),p2(pInfo->vStart+pInfo->vDelta);

	for (i=0; i<nSides; i++)
	{
#ifndef  _USE_BEVEL_RAYTRACE_
		//skip the bevel
		if (pSides[i].bevel) {
			continue;
		}
#endif
		plane = &(pSides[i].plane);
		A3DVECTOR3 vNormal = plane->GetNormal();

		// FIXME: special case for axial

		dist = plane->GetDist();
		d1 = DotProduct (p1, vNormal) - dist;
		d2 = DotProduct (p2, vNormal) - dist;

		if (d2 > 0)
			getout = true;	// endpoint is not in solid
		if (d1 > 0)
			startout = true;

		// if completely in front of face, no intersection
		if (d1 > 0 && d2 >= d1)
			return false;

		if (d1 <= 0 && d2 <= 0)
			continue;

		// crosses face
		if (d1 > d2)
		{	// enter
			//@note: kuiwu  2005/08/17  d1 - TraceInfo.fDistEpsilon maybe less than 0.0f 
			//@ref:  hl2 source code  & q3 code
			//assert(d1 - TraceInfo.fDistEpsilon > 0.0f);
			f = (d1 - pInfo->fDistEpsilon)/(d1-d2);
			if (f < 0.0f) {
				f = 0.0f;
			}
			if (f > enterfrac)
			{
				enterfrac = f;
				clipplane = plane;
				pInfo->iClipPlane = i;
				// leadside = side;
			}
		}
		else
		{	// leave
			f = (d1+pInfo->fDistEpsilon) / (d1-d2);
			//@note: kuiwu, 2005/8/20, 
			//@ref: q3 code
			if (f > 1.0f) {
				f = 1.0f;
			}

			if (f < leavefrac)
				leavefrac = f;
		}
	}

	if (!startout)
	{	// original point was inside brush
		pInfo->bStartSolid= true;
		if (!getout)
			pInfo->bAllSolid = true;
		//@note: added by kuiwu
		pInfo->fFraction  = 0.0f;
		
		return true;
	}

	//@note : In some singular situations, eg. the volume of the convex hull is zero, 
	//        two or more sides are opposite,  enterfrac will be very close to leavefrac.
	//@todo : try to avoid such situations when generating convex hull.  By Kuiwu[24/8/2005]
	if (enterfrac < leavefrac)
	{
		// if (enterfrac > -1 && enterfrac < trace->fraction)
		if (enterfrac > -1 && enterfrac < 1) 
		{
			if (enterfrac < 0)
				enterfrac = 0;
			pInfo->fFraction= enterfrac;
			pInfo->ClipPlane = *clipplane;
			// trace->surface = &(leadside->surface->c);
			// trace->contents = brush->contents;

			return true;
		}
	}
	
	return false;

	
}



}	// end of namespace