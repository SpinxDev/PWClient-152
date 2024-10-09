#ifndef	_CDWITHCH_H_
#define _CDWITHCH_H_

//////////////////////////////////////////////////
// CH is the abbreviation of "Convex Hull"
// CD is the abbreviation of "Collision Detection"
//////////////////////////////////////////////////

#include "ConvexHullData.h"

namespace CHBasedCD
{
//////////////////////////////////////////////////
// Static Collide with CH
//////////////////////////////////////////////////
bool IsSphereOutsideCH(const A3DVECTOR3& vecCentroid,float fRadius,const CConvexHullData& CHData);

inline bool IsVertexOutsideCH(const A3DVECTOR3& v,const CConvexHullData& CHData)
{
	return IsSphereOutsideCH(v,0.0f,CHData);
}

//////////////////////////////////////////////////
// Dynamic Collide with CH
// To note is that the vCollidePos is not the contact point 
// but the position of sphere center when collision occurs!
//////////////////////////////////////////////////
bool SphereCollideWithCH(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius, const CConvexHullData& CHData,const CFace** ppCDFace,A3DVECTOR3& vCollidePos,float& fraction);

inline bool RayIntersectWithCH(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta, const CConvexHullData& CHData,const CFace** ppCDFace,A3DVECTOR3& vCollidePos,float& fraction)
{
	return SphereCollideWithCH(vecStart, vecDelta, 0.0f, CHData, ppCDFace, vCollidePos, fraction);
}



//////////////////////////////////////////////////
// Collide with Triangle
//////////////////////////////////////////////////

//-----------------------------------------------------------------------
// An exact Sphere-Triangle collision test model
// To note is the return value is not a bool-type value but int-type!
// Returned value:
//		0: no collision occurs
//		1: hit the inside of the triangle, when this is the case, 
//			the vNormal means just the normal of triangle.
//		2: hit the edge or the vertex of the triangle,when this is the 
//			case, the vNormal means the normal of the slide plane
//-----------------------------------------------------------------------
int SphereExactlyCollideWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos, A3DVECTOR3& vNormal, float& fraction);

// Just a coarse collision test using the same method of SphereCollideWithCH!
bool SphereCollideWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction);

// Ray-Triangle intersect test in a similar way of RayIntersectWithCH, so it's slow.
bool RayIntersectWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction);

//-------------------------------------------------------------------
// A fast Ray-Triangle intersect test! Maybe the fastest version!?
// using barycentric coordinates u, v: A point in barycentric coordinate system
// T(u, v) = (1-u-v) V0 + u V1 + v V2
// Refer to: http://www.acm.org/jgt/papers/MollerTrumbore97/
//-------------------------------------------------------------------
// bCullBackSide: Culling the back face of triangle?
bool FastRayIntersectWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction, bool bCullBackFace = true);

//////////////////////////////////////////////////
// Auxiliary functions
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Test if two AABB overlap.
// Return value:
//		0: no overlap
//		1: overlap partly
//		2: aabb1 is fully inside aabb2
//		3: aabb2 is fully inside aabb1
//////////////////////////////////////////////////
int AABBOverlapTest(const A3DAABB& aabb1, const A3DAABB& aabb2);


} // end namespace

#endif