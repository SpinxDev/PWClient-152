/********************************************************************
  created:	   8/11/2005  
  filename:	   EC_TriangleMan.cpp
  author:      Wangkuiwu  
  description: Triangle manager, mainly used by shadow generator.
  Copyright (c) 2005 Archosaur Studio , All Rights Reserved.
*********************************************************************/

#include "A3DDevice.h"
#include "A3DCollision.h"
#include "A3dframe.h"
#include "aassist.h"
#include "A3dlitmodel.h"
#include "A3dcamera.h"
#include "EC_TriangleMan.h"
#include "EL_BrushBuilding.h"



#ifdef TMAN_VERBOSE_STAT
#include "acounter.h"
#endif

//#define  TMAN_DEBUG

#ifdef TMAN_DEBUG
char msg[200];
#endif
//low =x, high = z
#define MAKE_CELL_KEY(x, z)    ((DWORD)(((WORD)((x) & 0xffff)) | ((DWORD)((WORD)((z) & 0xffff))) << 16))
#define GET_KEY_X(key)		((WORD)((DWORD)(key) & 0xffff))	
#define GET_KEY_Z(key)		((WORD)((DWORD)(key) >> 16))


#define TMAN_TEST_RANGE       60.0f
#define TMAN_BUILD_LIMIT      4000




/*
 * The template function determines the lowest value of N in the range 
 * [0, last - first) for which the predicate *(first + N) == val is true. 
 * It then returns first + N. If no such value exists, the function returns 
 * last. It evaluates the predicate once, at most, for each N.
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [2/11/2005]
 * @ref: std find.
 */
template<class InputIterator, class T> inline
     InputIterator linear_find(InputIterator first, InputIterator last, const T& value)
{
	InputIterator it = first;
	while (it != last)
	{
		if (*it == value)
			break;	
		++it;
	}

	return it;
}
/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
template<class InputIterator> inline
     InputIterator reorder(vector<int>& leftIndex, InputIterator first, InputIterator last)
{
	if (leftIndex.size() == (unsigned int)(last -first))
	{
		return last;
	}
	unsigned int i; 
	InputIterator it = first;
	for(i = 0; i <leftIndex.size(); ++i, ++it)
	{
		if (i != (unsigned int)leftIndex[i])
		{
			*it = *(first+leftIndex[i]); 
			
		}
	}

	return it;
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
inline static bool PlaneAABBOverlap(const A3DVECTOR3& normal, const float d, const A3DVECTOR3& maxbox)
{
	A3DVECTOR3 vMin, vMax;
	for (int i = 0; i <=2; ++i)
	{
		if (normal.m[i] > 0.0f)
		{
			vMin.m[i] = -maxbox.m[i];
			vMax.m[i] = maxbox.m[i];
		}
		else
		{
			vMax.m[i] = -maxbox.m[i];
			vMin.m[i] = maxbox.m[i];
		}
	}

	if (DotProduct(normal, vMin) +d > 0.0f)
	{
		return false;
	}
	if (DotProduct(normal, vMax) +d >= 0.0f)
	{
		return true;
	}
	return false;
}

#define AXISTEST_X01(a, b, fa, fb, ext)							\
	fmin = (a)*v0.y - (b)*v0.z;									\
	fmax = (a)*v2.y - (b)*v2.z;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.y + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;


#define AXISTEST_X2(a, b, fa, fb, ext)							\
	fmin = (a)*v0.y - (b)*v0.z;									\
	fmax = (a)*v1.y - (b)*v1.z;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.y + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Y02(a, b, fa, fb, ext)							\
	fmin = (b)*v0.z - (a)*v0.x;									\
	fmax = (b)*v2.z - (a)*v2.x;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + fb * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Y1(a, b, fa, fb, ext)							\
	fmin = (b)*v0.z - (a)*v0.x;									\
	fmax = (b)*v1.z - (a)*v1.x;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.z;					\
	if(fmin>rad || fmax<-rad) return false;

#define AXISTEST_Z12(a, b, fa, fb, ext)							\
	fmin = (a)*v1.x - (b)*v1.y;									\
	fmax = (a)*v2.x - (b)*v2.y;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.y;					\
	if(fmin>rad || fmax<-rad) return false;


#define AXISTEST_Z0(a, b, fa, fb, ext)							\
	fmin = (a)*v0.x - (b)*v0.y;									\
	fmax = (a)*v1.x - (b)*v1.y;									\
	if(fmin>fmax) {const float tmp=fmax; fmax=fmin; fmin=tmp;	}	\
	rad = (fa) * ext.x + (fb) * ext.y;					\
	if(fmin>rad || fmax<-rad) return false;


/*
 * AABB triangle overlap test without translation. assume aabb center is 0.
 * @desc : This is the code from Tomas Moller, 
 *		   a bit optimized with some more lazy evaluation (faster path on PC).
 * @param :     
 * @return :
 * @note: The A3dsdk implementation is not tested thoroughly, so do not use it.
 * @todo: Handle the intersections for triangles completely in a box plane.  
 * @author: kuiwu [18/11/2005]
 * @ref: Tomas Moller's original code
 */
static bool AABBTriangleOverlapNT(const A3DVECTOR3& vExt, const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& normal)
{

	// use separating axis theorem to test overlap between triangle and aabb 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	float fmin, fmax;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	fmin = min3(v0.x, v1.x, v2.x);
	fmax = max3(v0.x, v1.x, v2.x);
	if (fmin > vExt.x || fmax < -vExt.x)
	{
		return false;
	}
	// Same for Y
	fmin = min3(v0.y, v1.y, v2.y);
	fmax = max3(v0.y, v1.y, v2.y);
	if (fmin > vExt.y || fmax < -vExt.y)
	{
		return false;
	}
	// Same for Z
	fmin = min3(v0.z, v1.z, v2.z);
	fmax = max3(v0.z, v1.z, v2.z);
	if (fmin > vExt.z || fmax < -vExt.z) 
	{
		return false;
	}

	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	float d = - DotProduct(normal, v0);
	if (!PlaneAABBOverlap(normal, d, vExt))
	{
		return false;
	}

	// 3) "Class III" tests
	float rad;

	const A3DVECTOR3 e0(v1 - v0);
	const  float fey0 = fabsf(e0.y);
	const  float fez0 = fabsf(e0.z);
	AXISTEST_X01(e0.z, e0.y, fez0, fey0, vExt);
	const  float fex0 = fabsf(e0.x);
	AXISTEST_Y02(e0.z, e0.x, fez0, fex0, vExt);
	AXISTEST_Z12(e0.y, e0.x, fey0, fex0, vExt);	

	const	A3DVECTOR3 e1(v2 - v1);
	const	float fey1  = fabsf(e1.y);
	const	float fez1  = fabsf(e1.z);
	AXISTEST_X01(e1.z, e1.y, fez1, fey1, vExt);
	const float fex1 = fabsf(e1.x);					
	AXISTEST_Y02(e1.z, e1.x, fez1, fex1, vExt);
	AXISTEST_Z0(e1.y, e1.x, fey1, fex1, vExt);
	const A3DVECTOR3	e2(v0 - v2);	
	const float fey2 = fabsf(e2.y);					
	const float fez2 = fabsf(e2.z);					
	AXISTEST_X2(e2.z, e2.y, fez2, fey2, vExt);	
	const float fex2 = fabsf(e2.x);					
	AXISTEST_Y1(e2.z, e2.x, fez2, fex2, vExt);			
	AXISTEST_Z12(e2.y, e2.x, fey2, fex2, vExt);

	
	return  true;


}


/*
 * Triangle-aabb overlap test using the separating axis theorem(SAT).
 * @desc : This is the code from Tomas Moller, 
 *		   a bit optimized with some more lazy evaluation (faster path on PC).
 * @param :     
 * @return :
 * @note: The A3dsdk implementation is not tested thoroughly, so do not use it.
 * @todo:  
 * @author: kuiwu [10/11/2005]
 * @ref: Tomas Moller's original code
 */
static  bool AABBTriangleOverlap(const A3DAABB& aabb, const A3DVECTOR3& v0, 
								 const A3DVECTOR3& v1, const A3DVECTOR3& v2,
								 const A3DVECTOR3& normal)
{

	// move everything so that the aabb center is in (0,0,0) 
	A3DVECTOR3 vert0(v0 - aabb.Center), 
			   vert1(v1 - aabb.Center), 
			   vert2(v2 - aabb.Center);

	return AABBTriangleOverlapNT(aabb.Extents, vert0, vert1, vert2, normal);

	
}

/*
 *
 * @desc : Currently transform the triangle to obb's local space,
 *         then do aabb-triangle overlap test.
 * @param :     
 * @return :
 * @note:  
 * @todo:   Implement a highly optimized explicit version.
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static bool OBBTriangleOverlap(const A3DOBB& obb, const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2,
							   const A3DVECTOR3& normal)
{
	
	//from  world space to obb local space
	A3DMATRIX4 mat;
	
	// TR ,  (TR)^(-1) = R^(-1) T(-t) = Transpose(R) T(-t)
	mat._11 = obb.XAxis.x;
	mat._21 = obb.XAxis.y;
	mat._31 = obb.XAxis.z;
	mat._12 = obb.YAxis.x;
	mat._22 = obb.YAxis.y;
	mat._32 = obb.YAxis.z;
	mat._13 = obb.ZAxis.x;
	mat._23 = obb.ZAxis.y;
	mat._33 = obb.ZAxis.z;

//	mat._41 = -DotProduct(obb.Center, obb.XAxis);
//	mat._42 = -DotProduct(obb.Center, obb.YAxis);
//	mat._43 = -DotProduct(obb.Center, obb.ZAxis);
	mat._41  = mat._42 = mat._43 = 0.0f;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;

	A3DAABB  aabb;
	//aabb.Center = A3DVECTOR3(0);
	aabb.Center   = obb.Center* mat;
	aabb.Extents = obb.Extents;
	aabb.CompleteMinsMaxs();

	//transform the vert
	A3DVECTOR3   vert0(v0*mat), vert1(v1*mat), vert2(v2*mat);
	//transform the normal,  mat is a rigid transform matrix, 
	//it is safe to use mat to transform normal directly.
	A3DVECTOR3  vN(normal*mat);

	return AABBTriangleOverlap(aabb, vert0, vert1, vert2, vN);
	
}
/*
 * Use aabb to approximate triangle
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static bool OBBTriangleOverlapApprox(const A3DOBB& obb, const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	A3DAABB  aabb;
	aabb.Clear();
	aabb.AddVertex(v0);
	aabb.AddVertex(v1);
	aabb.AddVertex(v2);
	aabb.CompleteCenterExts();
	return CLS_AABBOBBOverlap(aabb, obb);
}
/*
 *
 * @desc : currently use aabb to approximate triangle.
 * @param :     
 * @return :
 * @note:
 * @todo:   Implement an exact version.
 * @author: kuiwu [10/11/2005]
 * @ref:
 */
static  bool ViewTriangleOverlap( A3DCameraBase * pCamera,  const A3DVECTOR3& v0, 
							   const A3DVECTOR3& v1, const A3DVECTOR3& v2,
							   const A3DVECTOR3& normal)
{
#if 0
	
#else
	//treat triangle as aabb
	A3DAABB  aabb;
	aabb.Clear();
	aabb.AddVertex(v0);
	aabb.AddVertex(v1);
	aabb.AddVertex(v2);
	aabb.CompleteCenterExts();
	
	return pCamera->AABBInViewFrustum(aabb);
	
#endif
	
}

#define TMAN_IN     -1    
#define TMAN_OUT	 1	
#define TMAN_PART	 0

/*
 * Yet another aabb triangle overlap test method.
 * @desc : adjust  if the given triangle  can be trivially accepted or rejected, or need to
 *		   be clipped.
 * @param :     
 * @return : -1: completely in the AABB  
 *            1: completely out the AABB
 *            0: partly in&out, need clip * @note:
 * @todo:   
 * @author: kuiwu [18/11/2005]
 * @ref:
 */
static int AABBTriangleFastExclude(const A3DVECTOR3& vExt, const A3DVECTOR3 vert[3])
{

  // vertex bit codes for 6 least sign bit ,  l,r,b,t,n,f
  // set bit means out of corresponding frustum plane
  unsigned int Code[3]={0,0,0};  
  for (int i=0; i<3; i++)        
  {
	Code[i]	|= ((vert[i].x < -vExt.x)<<5);
	Code[i]	|= ((vert[i].x >  vExt.x)<<4);	
	Code[i]	|= ((vert[i].y < -vExt.y)<<3);
	Code[i]	|= ((vert[i].y >  vExt.y)<<2);
	Code[i]	|= ((vert[i].z < -vExt.z)<<1);
	Code[i]	|= ((vert[i].z >  vExt.z));
  }
  
  // each vertex is outside of at least one plane
  if (Code[0] & Code[1] & Code[2]) return(TMAN_OUT);     
  // all vertices are inside of all planes

  if (!(Code[0] | Code[1] | Code[2])) return(TMAN_IN); 
  //partly
  return(TMAN_PART);

}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
static int PlaneClipTriangle(const A3DVECTOR3& ext, const A3DVECTOR3 *inVert, int inNum, A3DVECTOR3 *outVert, int planeflag )
{
   const A3DVECTOR3 *A, *B;  //edge AB
   int Aout, Bout;     //edge endpoint outside condition
	
   int i;
   int iOut = 0;
   for (i = 0; i < inNum; i++) 
   {
	  A = inVert + i;
	  B = inVert + (i+1)%inNum;

     switch(planeflag)    //set endpoint outside condition 
	 {
		 case 0: Aout=(A->x < -ext.x); Bout=(B->x <-ext.x); break;  // LEFT
		 case 1: Aout=(A->x > ext.x);  Bout=(B->x > ext.x); break;  // RIGHT
		 case 2: Aout=(A->y <-ext.y);  Bout=(B->y <-ext.y); break;  // BOTTOM
 		 case 3: Aout=(A->y > ext.y);  Bout=(B->y > ext.y);  break;  // TOP
		 case 4: Aout=(A->z < -ext.z); Bout=(B->z < -ext.z); break;  // NEAR
		 case 5: Aout=(A->z > ext.z);  Bout=(B->z > ext.z ); break;  // FAR		 
		 default: assert(0);
	 };
	
	
	if (Aout != Bout)    //partly in(out), intersection occurs
    { 
	  float t;   //edge parametric value, used to intersect interpolation
      switch(planeflag)
      {
		case 0: t=(-ext.x - A->x)/(B->x - A->x ); break;  // LEFT
		case 1: t=(ext.x - A->x) /(B->x - A->x ); break;  // RIGHT
		case 2: t=(-ext.y - A->y)/(B->y - A->y ); break;  // BOTTOM
        case 3: t=(ext.y - A->y) /(B->y - A->y );  break;  // TOP
        case 4: t=(-ext.z -A->z)/(B->z -A->z ); break;  // NEAR
		case 5: t=(ext.z - A->z) /(B->z -A->z );  break;  // FAR
      };
      // interpolate geometry
	  outVert[iOut] = (*A) + ((*B) - (*A))*t;
      iOut++;
    }

	if ( (Aout && !Bout) || (!Aout && !Bout) )   // output 2nd edge vertex
    { 
	  outVert[iOut] = (*B);	
      iOut++;
    }
  }
	
	return iOut;

	
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note: without +z (far) plane.
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
static int AABBClipTriangle(const A3DVECTOR3& vExt, const A3DVECTOR3 invert[3], A3DVECTOR3 outvert[9])
{

  A3DVECTOR3 tmpInVert[9];	

  
  int i, NumOutVert;

  for (i = 0; i < 3; i++) 
  {
	  tmpInVert[i] = invert[i];
  }
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, 3,          outvert, 0);			//left
  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 1);		//right
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, NumOutVert, outvert, 2);			//bottom
  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 3);		//top
  NumOutVert = PlaneClipTriangle(vExt, tmpInVert, NumOutVert, outvert, 4);          //near	
//  NumOutVert = PlaneClipTriangle(vExt, outvert,   NumOutVert, tmpInVert, 5);        //far	
//  
//  for (i = 0; i < NumOutVert; ++i)
//  {
//	  outvert[i] = tmpInVert[i];
//  }
//  
  return(NumOutVert);	
}


/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [19/11/2005]
 * @ref:
 */
void CTriangleGrid::_BuildOBB(const TMan_Overlap_Info& tInfo,  A3DOBB& obb)
{
	obb.Center = tInfo.vStart + tInfo.vDelta *0.5f;

	float fMax = max2(tInfo.vExtent.x, tInfo.vExtent.y);
	obb.ZAxis  = tInfo.vDelta;
	obb.Extents.z = obb.ZAxis.Normalize();
	obb.Extents.z *= 0.5f;

	obb.XAxis  = CrossProduct(A3DVECTOR3(0,1,0), obb.ZAxis);
	obb.XAxis.Normalize();
	obb.Extents.x = fMax;
	obb.YAxis = CrossProduct(obb.XAxis, obb.ZAxis);
	obb.YAxis.Normalize();
	obb.Extents.y = fMax;
	
	obb.CompleteExtAxis();
	

}

CECTriangleMan::CECTriangleMan()
{
	m_pGrid = new CTriangleGrid(20, 17, 17);
}

CECTriangleMan::~CECTriangleMan()
{
	delete m_pGrid;
}

void CECTriangleMan::AddProvider(CELBrushBuilding * pProvider)
{
	m_pGrid->AddProvider(pProvider);
}

bool CECTriangleMan::RemoveProvider(CELBrushBuilding * pProvider)
{
	return m_pGrid->RemoveProvider(pProvider);
}

void CECTriangleMan::Build(const A3DVECTOR3& vCenter, DWORD dwFlag /* = TMAN_DEFAULT_BUILD */)
{
	//m_pGrid->Build(vCenter, dwFlag);
	m_pGrid->BuildGradually(vCenter, dwFlag);
}

void CECTriangleMan::TestOverlap(TMan_Overlap_Info& tInfo)
{
	//m_pGrid->TestOverlap(tInfo);
	m_pGrid->TestOverlapNF(tInfo);
}


CTriangleGrid::CTriangleGrid(int iCellSize /* = 10 */, int iW /* =11 */, int iH /* = 11 */)
:m_iCellSize(iCellSize),
 m_iW(iW),
 m_iH(iH),
 m_CellTbl(512),
 m_dwTestStamp(0),
 m_fOffset(0.02f)
{
	if (m_iW % 2 == 0)
	{
		--m_iW;
	}
	if (m_iH % 2 == 0)
	{
		--m_iH;
	}
	m_nCellCount = m_iW * m_iH * 2;
	m_UnOrganizedProvider.clear();
	m_iCenterX = -30000;
	m_iCenterZ = -30000;
	m_CellTbl.clear();

	m_pTrianglePool  = new CTManTrianglePool;

#ifdef TMAN_VERBOSE_STAT
	m_Stat.nCellSize = m_iCellSize;
	m_Stat.nWidth = m_iW;
	m_Stat.nHeight = m_iH;
	m_Stat.pCellTbl = &m_CellTbl;
#endif

}

CTriangleGrid::~CTriangleGrid()
{
	m_UnOrganizedProvider.clear();
	
	_ReleaseCellTbl();

   delete m_pTrianglePool;
}
/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note: the caller should assure NOT to add duplicate providers.
 * @todo:   
 * @author: kuiwu [8/11/2005]
 * @ref:
 */
void CTriangleGrid::AddProvider(CELBrushBuilding * pProvider)
{

	m_UnOrganizedProvider.push_back(pProvider);

#ifdef TMAN_VERBOSE_STAT
	m_Stat.nProvider++;
#endif
}

bool CTriangleGrid::RemoveProvider(CELBrushBuilding * pProvider)
{
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nProvider--;
#endif
	//find un-organize list  first
	CELBrushBuilding ** ppPrv = linear_find(m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.begin()+ m_UnOrganizedProvider.size(), pProvider);
	if (ppPrv != m_UnOrganizedProvider.begin() + m_UnOrganizedProvider.size() )
	{//in the un-organized list
		m_UnOrganizedProvider.erase(ppPrv);
#ifdef TMAN_VERBOSE_STAT
		m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
#endif
		return true;
	}
#ifdef TMAN_VERBOSE_STAT
		m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
		for (int i = 0; i < pProvider->GetA3DLitModel()->GetNumMeshes(); ++i)
		{
			m_Stat.nTriangle -= (pProvider->GetA3DLitModel()->GetMesh(i)->GetNumFaces());
		}
#endif
	//maybe in the organized cells
	int  j;
	CTriangleCell * pCell;
	A3DAABB prvAABB = pProvider->GetModelAABB();
	vector<int> leftIndex;
	TManCellTable::iterator it;
	for (it = m_CellTbl.begin(); it != m_CellTbl.end(); ++it)
	{
		pCell = *(it.value());
		if (pCell->m_Triangles.empty())
		{
			continue;
		}
		if (!CLS_AABBAABBOverlap(prvAABB.Center, prvAABB.Extents, pCell->m_AABB.Center, pCell->m_AABB.Extents))
		{
			continue;
		}
		leftIndex.clear();
		for (j = 0; j < (int)pCell->m_Triangles.size(); ++j)
		{
			TManTriangle * pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[j]);
			if(pTri->pProvider == pProvider)
			{
				assert(pTri->dwRef > 0);
				--pTri->dwRef;
			}
			else
			{
				leftIndex.push_back(j);
			}
		}
		vector<int>::iterator it = reorder(leftIndex, pCell->m_Triangles.begin(), pCell->m_Triangles.end());
		pCell->m_Triangles.erase(it, pCell->m_Triangles.end());
	}



	_CompactCellTbl();

	return true;
}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [22/11/2005]
 * @ref:
 */
int CTriangleGrid::_AddProvider(CELBrushBuilding * pProvider)
{
	int nAdd = 0;
	A3DLitModel * pLitModel = pProvider->GetA3DLitModel();
	for (int j = 0; j < pLitModel->GetNumMeshes(); ++j)
	{
		A3DLitMesh * pMesh = pLitModel->GetMesh(j);
		A3DLMVERTEX * pVerts = pMesh->GetVerts();
		WORD       * pIndices = pMesh->GetIndices();
		A3DVECTOR3  * pNormals = pMesh->GetNormals();  //per-vertex normal
		for (int k = 0; k < pMesh->GetNumFaces(); ++k)
		{
			int index = m_pTrianglePool->CreateTriangle();
			TManTriangle  * pTri  = m_pTrianglePool->GetTriangle(index);
			assert(pTri != NULL && pTri->dwRef == 0 );
			pTri->pProvider = pProvider;
			//@note : push out the vert to avoid z fighting. By Kuiwu[18/11/2005]
			pTri->vVert[0] = pVerts[pIndices[k*3]].pos + pNormals[pIndices[k*3]] * m_fOffset;
			pTri->vVert[1] = pVerts[pIndices[k*3+1]].pos + pNormals[pIndices[k*3+1]] * m_fOffset;
			pTri->vVert[2] = pVerts[pIndices[k*3+2]].pos + pNormals[pIndices[k*3+2]] * m_fOffset;
			A3DVECTOR3 e01(pTri->vVert[1] -  pTri->vVert[0]);
			A3DVECTOR3 e02(pTri->vVert[2] -  pTri->vVert[0]);
			pTri->vNormal = CrossProduct(e01, e02);  //no need to normalize
										
			_AddTriangle(pTri, index);					
		}
		nAdd += pMesh->GetNumFaces();
	}

	return (nAdd);
}

void CTriangleGrid::Build(const A3DVECTOR3& vCenter, DWORD dwFlag)
{
#ifdef TMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
#endif	


	bool bCenterChange = _UpdateCenter(vCenter, dwFlag);
	if (bCenterChange)
	{
		vector<int> leftIndex;

		A3DAABB  gridAABB;
		gridAABB.Center = vCenter;
		gridAABB.Extents.x = m_iCellSize *  (float)m_iW * 0.5f;
		gridAABB.Extents.y = 9999.0f;
		gridAABB.Extents.z = m_iCellSize *  (float)m_iH * 0.5f;
		gridAABB.CompleteMinsMaxs();
		CELBrushBuilding  * pProvider;
		leftIndex.clear();
		A3DAABB prvAABB;
		int nAdd = 0;
		for (int i = 0; i < (int)m_UnOrganizedProvider.size(); ++i )
		{
			pProvider = m_UnOrganizedProvider[i];
			prvAABB = pProvider->GetModelAABB();
			if (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents))
			{
				leftIndex.push_back(i);
				continue;
			}
			//try to add the provider
			nAdd += (_AddProvider(pProvider));
		}

		CELBrushBuilding ** ppPrv = reorder(leftIndex, m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.end());
		m_UnOrganizedProvider.erase(ppPrv, m_UnOrganizedProvider.end());
	#ifdef TMAN_VERBOSE_STAT
		m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
		m_Stat.nTriangle += nAdd; 
	#endif

	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwBuildTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif

}

/*
 *
 * @desc :
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [22/11/2005]
 * @ref:
 */
void CTriangleGrid::BuildGradually(const A3DVECTOR3& vCenter, DWORD dwFlag /* = TMAN_BUILD_DEFAULT */)
{
#ifdef TMAN_VERBOSE_STAT
	DWORD tStart;
	tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	m_Stat.nAddTriangle = 0;
#endif	


	bool bCenterChange = _UpdateCenter(vCenter, dwFlag);
	//@note : try to build even if center doesn't change. By Kuiwu[22/11/2005]
	vector<int> leftIndex;
	A3DAABB  gridAABB;
	gridAABB.Center = vCenter;
	gridAABB.Extents.x = m_iCellSize *  (float)m_iW * 0.5f;
	gridAABB.Extents.y = 9999.0f;
	gridAABB.Extents.z = m_iCellSize *  (float)m_iH * 0.5f;
	gridAABB.CompleteMinsMaxs();
	CELBrushBuilding  * pProvider;
	leftIndex.clear();
	A3DAABB prvAABB;
	int nAdd = 0;
	for (int i = 0; i < (int)m_UnOrganizedProvider.size(); ++i )
	{
		pProvider = m_UnOrganizedProvider[i];
		prvAABB = pProvider->GetModelAABB();
		if(((nAdd > TMAN_BUILD_LIMIT) && !(dwFlag&TMAN_BUILD_FORCE))
			|| (!CLS_AABBAABBOverlap(gridAABB.Center, gridAABB.Extents, prvAABB.Center, prvAABB.Extents)))
		{
				leftIndex.push_back(i);
				continue;
		}
		//try to add the provider
		nAdd += (_AddProvider(pProvider));
	}

	CELBrushBuilding ** ppPrv = reorder(leftIndex, m_UnOrganizedProvider.begin(), m_UnOrganizedProvider.end());
	m_UnOrganizedProvider.erase(ppPrv, m_UnOrganizedProvider.end());
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nUnorgPrv = m_UnOrganizedProvider.size();
	m_Stat.nTriangle += nAdd; 
	m_Stat.nAddTriangle = nAdd;
	m_Stat.dwBuildTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif
	
}

bool CTriangleGrid::_UpdateCenter(const A3DVECTOR3& vCenter, DWORD dwFlag)
{
	short iNewX, iNewZ;
	_GetCellIndex(vCenter, iNewX, iNewZ);
	if( ((dwFlag& TMAN_BUILD_FORCE) != TMAN_BUILD_FORCE ) 
		&& (iNewZ == m_iCenterZ) 
		&& (iNewX == m_iCenterX ))
	{
		return false;
	}
	short ix, iz, ix0, iz0;
	int  halfW = (m_iW -1) / 2;
	ix0 = iNewX - halfW;
	int halfH = (m_iH -1) /2;
	iz0 = iNewZ - halfH;
	CTriangleCell * pCell;
	DWORD dwKey;
	for (iz = iz0; iz < iz0 + m_iH; ++iz)
	{
		for (ix = ix0; ix < ix0 + m_iW; ++ix)
		{
			pCell = _FindCell(ix, iz);
			if (pCell)
			{//common
				continue;
			}
			pCell = new CTriangleCell;
			dwKey = MAKE_CELL_KEY(ix, iz);
			m_CellTbl.put(dwKey, pCell);

			pCell->m_AABB.Center = A3DVECTOR3(((float)ix + 0.5f) * m_iCellSize, 0.0f, ((float)iz + 0.5f) * m_iCellSize);
			pCell->m_AABB.Extents = A3DVECTOR3(0.5f*m_iCellSize, 9000.0f, 0.5f*m_iCellSize);
			pCell->m_AABB.CompleteMinsMaxs();
			pCell->m_AABB.Mins.y = 9000.0f;
			pCell->m_AABB.Maxs.y = -9000.0f;

		}
	}
	m_iCenterX = iNewX;
	m_iCenterZ = iNewZ;
	
	return true;
}

void CTriangleGrid::_GetCellIndex(const A3DVECTOR3& vPos, short& x, short& z)
{
	//x = (short)(vPos.x / (float)m_iCellSize);
	//z=  (short)(vPos.z / (float)m_iCellSize);
	x = (short) floor(vPos.x /(float)m_iCellSize);
	z = (short) floor(vPos.z /(float)m_iCellSize);

}

CTriangleCell * CTriangleGrid::_FindCell(short x, short z)
{
	DWORD   dwKey = MAKE_CELL_KEY(x, z);
	TManCellTable::pair_type  pair;	
	pair = m_CellTbl.get(dwKey);
	if (pair.second)
	{
		return *pair.first;
	}
	
	return NULL;
}

bool CTriangleGrid::_AddTriangle(TManTriangle * pTri,  int index)
{
	short x, z, xMin, zMin, xMax, zMax;
	_GetCellIndex(pTri->vVert[0], xMin, zMin);
	xMax = xMin;
	zMax = zMin;
	_GetCellIndex(pTri->vVert[1], x, z);
	xMax = max2(xMax, x);
	zMax = max2(zMax, z);
	xMin = min2(xMin, x);
	zMin = min2(zMin, z);
	_GetCellIndex(pTri->vVert[2], x, z);
	xMax = max2(xMax, x);
	zMax = max2(zMax, z);
	xMin = min2(xMin, x);
	zMin = min2(zMin, z);

	CTriangleCell * pCell;
	A3DAABB aabb;
	aabb.Extents = A3DVECTOR3(0.5f*m_iCellSize, 9000.0f, 0.5f*m_iCellSize);

	volatile short xx, zz;
	for (zz = zMin; zz <= zMax; ++zz)
	{
		for (xx = xMin; xx <= xMax; ++xx)
		{
			aabb.Center = A3DVECTOR3((xx + 0.5f) * m_iCellSize, 0.0f, (zz + 0.5f) * m_iCellSize);
			if (!AABBTriangleOverlap(aabb, pTri->vVert[0], pTri->vVert[1], pTri->vVert[2], pTri->vNormal))
			{
				continue;
			}
			pCell = _FindCell(xx, zz);
			if (!pCell)
			{
				pCell = new CTriangleCell;
				DWORD dwKey = MAKE_CELL_KEY(xx, zz);
				m_CellTbl.put(dwKey, pCell);
				pCell->m_AABB = aabb;
				pCell->m_AABB.CompleteMinsMaxs();
				pCell->m_AABB.Mins.y = 9000.0f;
				pCell->m_AABB.Maxs.y = -9000.0f;
			}
			pCell->m_Triangles.push_back(index);
			pCell->m_AABB.Mins.y = min4(pCell->m_AABB.Mins.y,  pTri->vVert[0].y,
										pTri->vVert[1].y, pTri->vVert[2].y);
			pCell->m_AABB.Maxs.y = max4(pCell->m_AABB.Maxs.y,  pTri->vVert[0].y,
										pTri->vVert[1].y, pTri->vVert[2].y);
			pCell->m_AABB.Center.y = (pCell->m_AABB.Mins.y + pCell->m_AABB.Maxs.y) * 0.5f;
			pCell->m_AABB.Extents.y = pCell->m_AABB.Maxs.y - pCell->m_AABB.Center.y;
			
			++pTri->dwRef;
		}	
	}
	
	//assert(nAdd >0);
	//return true;
	return (pTri->dwRef > 0);
}
#if 0
void CTriangleGrid::TestOverlap(TMan_Overlap_Info& tInfo)
{

	short cx, cz;
	_GetCellIndex(tInfo.vStart, cx, cz);

	if((tInfo.dwFlag & TMAN_CHECK_AUTOBUILD) == TMAN_CHECK_AUTOBUILD )
	{
		const float thresh = 2.5f;
		if ((fabs(cx - m_iCenterX) > thresh)
			||(fabs(cz - m_iCenterZ) > thresh))
		{
			Build(tInfo.vStart);
		}
	}
	tInfo.pVert.clear();
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nTestTriangle  = 0;
	DWORD tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	
#endif
	//assert(tInfo.pCamera != NULL);
	A3DAABB  aabb;
	aabb.Center = tInfo.vStart;
	aabb.Extents = tInfo.vExtent;
	aabb.CompleteMinsMaxs();
	A3DVECTOR3  pt[16];
	aabb.GetVertices(pt, NULL, false);
	A3DVECTOR3 * pV0, *pV1;
	pV0 = pt;
	pV1 = pt+8;
	int i;
	for (i = 0; i < 8; ++i)
	{
		pV1[i] = pV0[i] + tInfo.vDelta;
	}
	//construct obb
	A3DOBB  obb;
	obb.Build(pt, 16);
#ifdef TMAN_VERBOSE_STAT
	m_Stat.obb = obb;
#endif
	//check  cell

	const float testRange = 100.0f;
	int nHalf = (int) ( testRange/(float)m_iCellSize + 0.5f);
	
	short ix, iz, ix0, iz0;
	ix0 = cx - nHalf;
	iz0 = cz - nHalf;
	
	CTriangleCell * pCell;

	for (iz = iz0; iz < iz0 + nHalf*2; ++iz)
	{
		for (ix = ix0; ix < ix0 + nHalf*2; ++ix)
		{
			pCell = _FindCell(ix, iz);
			if (!pCell || pCell->m_Triangles.empty()
				||(!CLS_AABBOBBOverlap(pCell->m_AABB, obb))
				||(tInfo.pCamera && !tInfo.pCamera->AABBInViewFrustum(pCell->m_AABB)))
			{
				continue;
			}
			for (int j = 0; j < (int)pCell->m_Triangles.size(); ++j)
			{
				_TestTriangle(tInfo, obb, pCell->m_Triangles[j]);
			}
#ifdef TMAN_VERBOSE_STAT
			m_Stat.nTestTriangle += (pCell->m_Triangles.size());
#endif
		}
	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwTestTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif

}

#endif



void CTriangleGrid::TestOverlapNF(TMan_Overlap_Info& tInfo)
{
	tInfo.pVert.clear();
	tInfo.pIndex.clear();

	//need camera to back face culling
	if (!tInfo.pCamera)
	{
		return;
	}

	short cx, cz;
	_GetCellIndex(tInfo.vStart, cx, cz);

	
#ifdef TMAN_VERBOSE_STAT
	m_Stat.nTestTriangle  = 0;
	DWORD tStart = (DWORD)(ACounter::GetMicroSecondNow()/1000);
	
#endif

	//construct obb
	A3DOBB  obb;
	_BuildOBB(tInfo, obb);
	//obb.Extents *= 3.0f;
	
	A3DMATRIX4   w2lMat;  //world to obb local
	A3DMATRIX4   l2wMat;  //obb local to world
	w2lMat.SetCol(0, obb.XAxis);   //Rotation
	w2lMat.SetCol(1, obb.YAxis);
	w2lMat.SetCol(2, obb.ZAxis);
	
	w2lMat._41 = -DotProduct(obb.Center, obb.XAxis);   //translation
	w2lMat._42 = -DotProduct(obb.Center, obb.YAxis);
	w2lMat._43 = -DotProduct(obb.Center, obb.ZAxis);

	w2lMat.SetCol(3, A3DVECTOR3(0));
	w2lMat._44 = 1.0f;

	l2wMat.SetRow(0, obb.XAxis);  //rotation
	l2wMat.SetRow(1, obb.YAxis);
	l2wMat.SetRow(2, obb.ZAxis);

	l2wMat.SetRow(3, obb.Center);  //translation

	l2wMat.SetCol(3, A3DVECTOR3(0));
	l2wMat._44 = 1.0f;

	//A3DMATRIX4 tmpMat(w2lMat*l2wMat);  //expect I
	//A3DVECTOR3 tmpVec(obb.Center*w2lMat); //expect zero
	//tmpVec = tmpVec * l2wMat;

	

#ifdef TMAN_VERBOSE_STAT
	m_Stat.obb = obb;
#endif
	//check  cell
	int nHalf = (int) ( TMAN_TEST_RANGE/(float)m_iCellSize);
	
	short ix, iz, ix0, iz0;
	ix0 = cx - nHalf;
	iz0 = cz - nHalf;
	
	CTriangleCell * pCell;

	++m_dwTestStamp;

	for (iz = iz0; iz < iz0 + nHalf*2; ++iz)
	{
		for (ix = ix0; ix < ix0 + nHalf*2; ++ix)
		{
			pCell = _FindCell(ix, iz);
			//@note : not need view culling. By Kuiwu[18/11/2005]
			if (!pCell || pCell->m_Triangles.empty()
				||(!CLS_AABBOBBOverlap(pCell->m_AABB, obb)))
				//||(tInfo.pCamera && !tInfo.pCamera->AABBInViewFrustum(pCell->m_AABB)))
			{
				continue;
			}
			for (int j = 0; j < (int)pCell->m_Triangles.size(); ++j)
			{
				TManTriangle * pTri = m_pTrianglePool->GetTriangle(pCell->m_Triangles[j]);
				if (pTri->dwCheckStamp != m_dwTestStamp)
				{
#ifdef TMAN_VERBOSE_STAT
					++m_Stat.nTestTriangle;
#endif				
					_TestTriangleNF(tInfo, obb, *pTri, w2lMat, l2wMat);
					pTri->dwCheckStamp = m_dwTestStamp;
				}
			}
		}
	}
#ifdef TMAN_VERBOSE_STAT
	m_Stat.dwTestTime = (DWORD)(ACounter::GetMicroSecondNow()/1000) - tStart;
#endif

}

#if 0
bool CTriangleGrid::_TestTriangle(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri)
{

	//already exist?
	size_t i;
	for (i = 0; i < tInfo.pVert.size()/3; ++i)
	{
		if ((tInfo.pVert[i*3] == tri.vVert[0])
			&&(tInfo.pVert[i*3+1] == tri.vVert[1])
			&&(tInfo.pVert[i*3+2] == tri.vVert[2]))
		{
			return false;
		}
	}

	//back face culling
	if (((tInfo.dwFlag & TMAN_CHECK_CULLBACK) == TMAN_CHECK_CULLBACK ) && tInfo.pCamera)
	{
		//A3DVECTOR3    vView( tInfo.pCamera->GetPos() - tri.vCenter);
		//if (DotProduct(vView, tri.vNormal) < 0.0f)
		if(DotProduct(tInfo.pCamera->GetDir(), tri.vNormal))
		{
			return false;
		}
	}
	//view culling
	if (((tInfo.dwFlag & TMAN_CHECK_FRUSTUM) == TMAN_CHECK_FRUSTUM)  && tInfo.pCamera
		&&(!ViewTriangleOverlap(tInfo.pCamera, tri.vVert[0], tri.vVert[1], tri.vVert[2], tri.vNormal)))
	{
		return false;
	}
	// relative strict but  relative expensive test
	if (((tInfo.dwFlag & TMAN_CHECK_STRICT) == TMAN_CHECK_STRICT)
		&& (!OBBTriangleOverlapApprox(obb, tri.vVert[0], tri.vVert[1], tri.vVert[2])))
	{
		return false;
	}

	//exact but expensive test  
	if (((tInfo.dwFlag & TMAN_CHECK_EXACT) == TMAN_CHECK_EXACT)
		&& (!OBBTriangleOverlap(obb, tri.vVert[0], tri.vVert[1], tri.vVert[2], tri.vNormal )))
	{
		return false;
	}


	
	tInfo.pVert.push_back(tri.vVert[0]);
	tInfo.pVert.push_back(tri.vVert[1]);
	tInfo.pVert.push_back(tri.vVert[2]);

	return true;
}
#endif

bool CTriangleGrid::_TestTriangleNF(TMan_Overlap_Info& tInfo, const A3DOBB& obb, const TManTriangle& tri, const A3DMATRIX4& w2lMat, const A3DMATRIX4& l2wMat)
{



	//back face culling  and  light dir culling
	//assume camera ready, dir is from camera(light) to vertex (world space)
	if ((DotProduct(tri.vVert[0] - tInfo.pCamera->GetPos(), tri.vNormal) > 0.0f)
		||(DotProduct(tInfo.vDelta, tri.vNormal) > 0.0f))
	{
		return false;
	}
	
	//exact test  
	//transform the vert to obb local
	A3DVECTOR3 normal;  //no need translation
	normal.x = DotProduct(tri.vNormal, w2lMat.GetCol(0) );
	normal.y = DotProduct(tri.vNormal, w2lMat.GetCol(1) );
	normal.z = DotProduct(tri.vNormal, w2lMat.GetCol(2) );

	A3DVECTOR3 vert[3];
	vert[0] = tri.vVert[0] * w2lMat;
	vert[1] = tri.vVert[1] * w2lMat;
	vert[2] = tri.vVert[2] * w2lMat;
	if (!AABBTriangleOverlapNT(obb.Extents, vert[0], vert[1], vert[2], normal))
	{
		return false;
	}

	A3DVECTOR3 clipvert[9];
	int nVert = AABBClipTriangle(obb.Extents, vert, clipvert);
	if (nVert <= 0)
	{
		assert(0);
		return false;
	}
	int i;
	WORD base = tInfo.pVert.size();
	//transform back to world space
	for (i = 0; i < nVert; ++i)
	{
		clipvert[i] = clipvert[i] * l2wMat;
		tInfo.pVert.push_back(clipvert[i]);
	}
	//simple triangulation
	for (i = 0; i < nVert -2; ++i)
	{//as a triangle fan
	   tInfo.pIndex.push_back(base);
	   tInfo.pIndex.push_back(base + i +1);
	   tInfo.pIndex.push_back(base + i+ 2);
	}
	
	return true;
}



void CTriangleGrid::_ReleaseCellTbl()
{
	TManCellTable::iterator  it;
	CTriangleCell * pCell;
	for (it = m_CellTbl.begin(); it != m_CellTbl.end(); ++it)
	{
		pCell = *(it.value());
		assert(pCell != NULL);
		delete pCell;
	}

	m_CellTbl.clear();
}

void CTriangleGrid::_CompactCellTbl()
{

	TManCellTable::iterator it;
	it = m_CellTbl.begin();
	CTriangleCell * pCell;
	DWORD dwKey;
	short x, z;
	int halfW = m_iW/2;
	int halfH  = m_iH/2;

	while (it != m_CellTbl.end())
	{
		if (m_CellTbl.size() < (unsigned int)m_nCellCount )
		{
			//return;
			break;
		}

		pCell = *(it.value());
		dwKey  = *(it.key());
		x = (short)GET_KEY_X(dwKey);
		z = (short)GET_KEY_Z(dwKey);
		
		if (pCell->m_Triangles.empty() 
			&& ((abs(x - m_iCenterX) > halfW) || (abs(z - m_iCenterZ) > halfH) ) )
		{
			delete pCell;
		    it = m_CellTbl.erase(it);
		}
		else
		{
			++it;
		}


	}

}



CTManTrianglePool::CTManTrianglePool(int nCount /* = 40000 */, int nGrow /* = 40000 */)
:m_nMaxCount(nCount), m_nLast(0), m_nGrow(nGrow)
{
	assert(m_nMaxCount > 0);
	m_pBuf  = new TManTriangle[m_nMaxCount];
    assert(m_pBuf != NULL);
}
CTManTrianglePool::~CTManTrianglePool()
{
	delete[] m_pBuf;
}

int CTManTrianglePool::CreateTriangle()
{
	int i = m_nLast;
	do 
	{
		++i;
		if (i >= m_nMaxCount)
		{
			i -= m_nMaxCount;
		}
		if (m_pBuf[i].dwRef == 0)
		{//free
			m_nLast = i;
			return m_nLast;
		}
	} while( i != m_nLast);


	//no free triangle, realloc
	m_nLast = m_nMaxCount;
	_Grow(m_nMaxCount + m_nGrow);
	return m_nLast;
}

/*
 * 
 * @desc :   assume the input param is correct for efficiency.
 * @param :     
 * @return :
 * @note:
 * @todo:   
 * @author: kuiwu [18/11/2005]
 * @ref:
 */
void CTManTrianglePool::_Grow(int newcount)
{
	TManTriangle * pNewBuf = new TManTriangle[newcount];
	memcpy(pNewBuf, m_pBuf, sizeof(TManTriangle) * m_nMaxCount);
	delete[] m_pBuf;

	m_pBuf = pNewBuf;
	m_nMaxCount = newcount;
}



