          
#include "CDWithCH.h"
#include <math.h>

#include <A3DCollision.h>

#define TO_UINT(x) ( ( unsigned int& ) x )		

//////////////////////////////////////////////////////////////////////////
// Note by wenfeng, 05-8-8
// note: it seems that this threshold is very important to let the critical
//		 case pass the SphereIntersectFacePreTest! In my test, it's better
//		 to set to 1e-4 -- 1e-3 to get a great result, while if we set it to
//		 1e-5, bugs come out!

// Introduce this distance to weaken the condition of SphereIntersectFacePreTest

#define CRITICAL_DIST_THRESH 1e-3f
//////////////////////////////////////////////////////////////////////////



namespace CHBasedCD
{

// solve a quadratic ax2+bx+c=0, and return the number of real root.
// if there are 2 roots, r1<r2
int SolveQuadratic(float a, float b, float c,float& r1, float& r2)
{
	if(a==0.0f)
	{
		if(b==0.0f) return -2;
		r1=-c/b;
		return -1;
	}

	float Delta=b*b-4*a*c;
	if(Delta<0.0f) return 0;
	if(a>0.0f)
	{
		r1=float(( -b - sqrt(Delta) ) / (2*a));
		r2=float(( -b + sqrt(Delta) ) / (2*a));
	}
	else
	{
		r2=float(( -b - sqrt(Delta) ) / (2*a));
		r1=float(( -b + sqrt(Delta) ) / (2*a));
	}

	if(Delta==0.0f) 
		return 1;
	else
		return 2;
	
}

// Check whether a 3D point is inside a triangle
bool IsPointInTriangle(const A3DVECTOR3& point, const A3DVECTOR3& pa, const A3DVECTOR3& pb, const A3DVECTOR3& pc)
{
	A3DVECTOR3 e10=pb-pa;
	A3DVECTOR3 e20=pc-pa;

	float a=DotProduct(e10,e10);
	float b=DotProduct(e10,e20);
	float c=DotProduct(e20,e20);
	float ac_bb=a*c-b*b;
	
	A3DVECTOR3 vp(point-pa);
	
	float d=DotProduct(vp,e10);
	float e=DotProduct(vp,e20);
	
	float x=d*c-e*b;
	float y=e*a-d*b;
	float z=x+y-ac_bb;

	unsigned int uiRes=TO_UINT(z) & ~(TO_UINT(x) | TO_UINT(y));

	//return (bool)(uiRes & 0x80000000);
	return ( (uiRes & 0x80000000) != 0);
}

bool SphereIntersectFacePreTest(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const CFace *pFace)
{
	A3DVECTOR3 vecEnd=vecStart+vecDelta;
	
	/*
	bool bResult = pFace->DistV2Plane(vecStart) >= fRadius &&  
						  pFace->DistV2Plane(vecEnd) <= fRadius;
	//*/

	//*
	bool bResult = // DotProduct(vecDelta, pFace->GetNormal()) < 0.0f && 
					pFace->DistV2Plane(vecStart) > (fRadius - CRITICAL_DIST_THRESH) && 
					pFace->DistV2Plane(vecEnd) < (fRadius + CRITICAL_DIST_THRESH);
	//*/

//////////////////////////////////////////////////////////////////////////
// test code by wenfeng, 05-8-8
	
#define CRITICAL_AREA_DIST 1e-4f
	float fStartDist = pFace->DistV2Plane(vecStart);
	float fEndDist = pFace->DistV2Plane(vecEnd);
	bool bStartPosInCriticalArea =  fStartDist> (fRadius - CRITICAL_AREA_DIST) && fStartDist < (fRadius + CRITICAL_AREA_DIST);
	if(bStartPosInCriticalArea)
	{
		int itest =0;
	}
//////////////////////////////////////////////////////////////////////////
	
	return bResult;
}

/*
 *  test if the sphere intersects with the polygon
 *	@author kuiwu
 */
bool SphereIntersectPolygonTest(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const CFace *pFace,A3DVECTOR3& vCollidePos,float& fraction)
{
	A3DVECTOR3   vecEnd(vecStart + vecDelta);

	assert(vecStart.x > -6000.0f && vecStart.x < 6000.0f);
	assert(vecStart.y > -6000.0f && vecStart.y < 6000.0f);
	assert(vecStart.z > -6000.0f && vecStart.z < 6000.0f);

	float d0 = pFace->DistV2Plane(vecStart);

#define EPSILION   1E-6
	
	if(DotProduct(vecDelta, pFace->GetNormal()) > 0.0f )
		return false;


	if (fabs(d0) < fRadius + EPSILION){

		int i(0);
		for(i=0;i<pFace->GetEdgeNum();i++)
		{
			CHalfSpace hs(pFace->GetEdgeHalfSpace(i));
			hs.Translate(fRadius);			//向外膨胀一个球半径的距离
			if(hs.Outside(vecStart))		//在某一个hs的外部
					return false;
		}	
		// 如果有额外面片，也必须考虑
		for(i=0;i<pFace->GetExtraHSNum();i++)
		{
			CHalfSpace hs(pFace->GetExtraHalfSpace(i));
			hs.Translate(fRadius);			//向外膨胀一个球半径的距离
			if(hs.Outside(vecStart))		//在某一个hs的外部
				return false;
		}

		fraction = 0.0f;
		vCollidePos  = vecStart;
		return true;
	}
	
	float d1 = pFace->DistV2Plane(vecEnd);

	//parallel
	if ( fabs(d0 - d1) < EPSILION) {
			return false;
	}
	bool  D0_POSITIVE_INTER =  (( d0 > fRadius + EPSILION) && (d1 < fRadius +EPSILION) );


if ( D0_POSITIVE_INTER ){
		fraction = (d0 - fRadius) / (d0 - d1);
		vCollidePos = vecStart * (1-fraction) + vecEnd * fraction;

		A3DVECTOR3 vPlaneHit(vCollidePos - pFace->GetNormal()* fRadius);

		
		//@todo: kuiwu refine me!
		//       it is not an effiecient way to test a point in polygon
		
		int i(0);
		for(i=0;i<pFace->GetEdgeNum();i++)
		{
			CHalfSpace hs(pFace->GetEdgeHalfSpace(i));
			//hs.Translate(fRadius);			//向外膨胀一个球半径的距离
			//if(hs.Outside(vCollidePos))		//在某一个hs的外部
			if(hs.Outside(vPlaneHit))		//在某一个hs的外部
					return false;
		}	
		// 如果有额外面片，也必须考虑
		for(i=0;i<pFace->GetExtraHSNum();i++)
		{
			CHalfSpace hs(pFace->GetExtraHalfSpace(i));
			//hs.Translate(fRadius);			//向外膨胀一个球半径的距离
			//if(hs.Outside(vCollidePos))		//在某一个hs的外部
			if(hs.Outside(vPlaneHit))		//在某一个hs的外部
				return false;
		}

		return true;
	}

#undef  EPSILION

	return false;

}

bool SphereIntersectFaceExactTest(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const CFace *pFace,A3DVECTOR3& vCollidePos,float& fraction)
{
//////////////////////////////////////////////////////////////////////////
// test code 
/*
	A3DVECTOR3 vecEnd=vecStart+vecDelta;
	
	float d0, d1;
	d0 = pFace->DistV2Plane(vecStart);
	d1 = pFace->DistV2Plane(vecEnd);
	
	bool bResult =   d0 > (fRadius-CRITICAL_DIST_THRESH)  &&  d1<(fRadius + CRITICAL_DIST_THRESH);
	
	if (!bResult) {
		return false;
	}
*/	

	//求解碰撞交点
	float fdp=DotProduct(pFace->GetNormal(),vecDelta);
	if(fdp<1e-6 && fdp>-1e-6)	//直线和平面平行的情况
		return false;
	fraction= pFace->GetDist()+fRadius-DotProduct(pFace->GetNormal(),vecStart);
	fraction/=fdp;

	if(fraction < 0.0f || fraction > 1.0f)
		return false;

	vCollidePos=vecStart+fraction*vecDelta;
	
	//测试交点是否在FACE内部
	int i(0);
	for(i=0;i<pFace->GetEdgeNum();i++)
	{
		CHalfSpace hs(pFace->GetEdgeHalfSpace(i));
		hs.Translate(fRadius);			//向外膨胀一个球半径的距离
		if(hs.Outside(vCollidePos))		//在某一个hs的外部
			return false;
	}
	
	// 如果有额外面片，也必须考虑
	for(i=0;i<pFace->GetExtraHSNum();i++)
	{
		CHalfSpace hs(pFace->GetExtraHalfSpace(i));
		hs.Translate(fRadius);			//向外膨胀一个球半径的距离
		if(hs.Outside(vCollidePos))		//在某一个hs的外部
			return false;
	}

	return true;
}


bool SphereCollideWithCH(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius, const CConvexHullData& CHData,const CFace** ppCDFace,A3DVECTOR3& vCollidePos,float& fraction)
{

	CFace* pFace;
	for(int i=0;i<CHData.GetFaceNum();i++)
	{
		pFace=CHData.GetFacePtr(i);

//////////////////////////////////////////////////////////////////////////
// test code:
//		bool test0, test1, test2;
//		bool test2;

		//test0 = SphereIntersectFacePreTest(vecStart,vecDelta,fRadius,pFace);
//		test1 = SphereIntersectFaceExactTest(vecStart,vecDelta,fRadius,pFace,vCollidePos,fraction);
//		test2 = SphereIntersectPolygonTest(vecStart,vecDelta,fRadius,pFace,vCollidePos,fraction);
		
//		assert( test1 == test2);
//
//		//test0 = SphereIntersectFacePreTest(vecStart,vecDelta,fRadius,pFace);
//		test1 = SphereIntersectFaceExactTest(vecStart,vecDelta,fRadius,pFace,vCollidePos,fraction);
//		test2 = SphereIntersectPolygonTest(vecStart,vecDelta,fRadius,pFace,vCollidePos,fraction);
//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		// Note by wenfeng, 05-8-9
		// 目前的代码仍可以优化！
		// 充分利用凸包的特性，如果起点和终点都在某一面的外部，则可直接返回
		// 这样就不必完成凸包的面的循环！
		//////////////////////////////////////////////////////////////////////////
		
		//依次进行可见性，相交，精确相交测试！
		if(	SphereIntersectFacePreTest(vecStart,vecDelta,fRadius,pFace) &&
			SphereIntersectFaceExactTest(vecStart,vecDelta,fRadius,pFace,vCollidePos,fraction))
		{
			*ppCDFace=pFace;
			return true;
		}
	}

	return false;

}

/*
bool RayIntersectWithCH(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,const CConvexHullData& CHData,const CFace** ppCDFace,A3DVECTOR3& vCollidePos,float& fraction)
{
	return SphereCollideWithCH(vecStart, vecDelta, 0.0f, CHData, ppCDFace, vCollidePos, fraction);
}
*/

// This method now is only a coarse solution to handle the problem
bool SphereCollideWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction)
{
	//采用与SphereIntersectFacePreTest和SphereIntersectFaceExactTest类似的算法
	
	//先构造三角形的平面方程
	CHalfSpace TriangleHS(Vertices[0],Vertices[1],Vertices[2]);
	
	//是否和平面相交
	A3DVECTOR3 vecEnd=vecStart+vecDelta;
	bool StartSide=TriangleHS.DistV2Plane(vecStart)<fRadius;
	bool EndSide=TriangleHS.DistV2Plane(vecEnd)<fRadius;	
	if(!(StartSide^EndSide)) return false;

	//交点是否在三角形内部
	
	//求解碰撞交点
	float fdp=DotProduct(TriangleHS.GetNormal(),vecDelta);
	if(fdp<1e-6 && fdp>-1e-6)	//直线和平面平行的情况
		return false;
	fraction= TriangleHS.GetDist()+fRadius-DotProduct(TriangleHS.GetNormal(),vecStart);
	fraction/=fdp;
	vCollidePos=vecStart+fraction*vecDelta;

	//依次构造三条边对应的halfspace
	CHalfSpace EdgeHS(Vertices[0],Vertices[1],Vertices[1]+TriangleHS.GetNormal());
	EdgeHS.Translate(fRadius);
	if(EdgeHS.Outside(vCollidePos)) return false;

	EdgeHS.Set(Vertices[1],Vertices[2],Vertices[2]+TriangleHS.GetNormal());
	EdgeHS.Translate(fRadius);
	if(EdgeHS.Outside(vCollidePos)) return false;

	EdgeHS.Set(Vertices[2],Vertices[0],Vertices[0]+TriangleHS.GetNormal());
	EdgeHS.Translate(fRadius);
	if(EdgeHS.Outside(vCollidePos)) return false;

	return true;
}

bool RayIntersectWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction)
{
	//先构造三角形的平面方程 vNormal.X=d
	//但与SphereCollideWithTriangle不同，这里vNormal不作归一化，从而实现加速的目的
	A3DVECTOR3 vNormal=CrossProduct(Vertices[1]-Vertices[0],Vertices[2]-Vertices[0]);
	float d=DotProduct(Vertices[0],vNormal);
	
	//是否和平面相交
	A3DVECTOR3 vecEnd=vecStart+vecDelta;
	bool StartSide=DotProduct(vecStart,vNormal)<d;
	bool EndSide=DotProduct(vecEnd,vNormal)<d;
	if(!(StartSide^EndSide)) return false;

	//交点是否在三角形内部

	//先求解交点
	float fdp=DotProduct(vNormal,vecDelta);
	if(fdp<1e-6 && fdp>-1e-6)	//直线和平面平行的情况
		return false;
	fraction= d-DotProduct(vNormal,vecStart);
	fraction/=fdp;
	vCollidePos=vecStart+fraction*vecDelta;	
	
	//依次构造三条边对应的平面方程
	A3DVECTOR3 vEN;
	float Ed;

	vEN=CrossProduct(Vertices[1]-Vertices[0],vNormal);
	Ed=DotProduct(vEN,Vertices[0]);
	if(DotProduct(vCollidePos,vEN)>Ed) return false;

	vEN=CrossProduct(Vertices[2]-Vertices[1],vNormal);
	Ed=DotProduct(vEN,Vertices[1]);
	if(DotProduct(vCollidePos,vEN)>Ed) return false;

	vEN=CrossProduct(Vertices[0]-Vertices[2],vNormal);
	Ed=DotProduct(vEN,Vertices[2]);
	if(DotProduct(vCollidePos,vEN)>Ed) return false;

	return true;
}

bool IsSphereOutsideCH(const A3DVECTOR3& vecCentroid,float fRadius,const CConvexHullData& CHData)
{
	for(int i=0;i<CHData.GetFaceNum();i++)
	{
		CFace& face=CHData.GetFace(i);
		CHalfSpace hs(static_cast<CHalfSpace>(face));
		hs.Translate(fRadius);
		if(hs.Outside(vecCentroid))
			return true;
	}
	return false;
}

#if 0
int SphereExactlyCollideWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,float fRadius,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos, A3DVECTOR3& vNormal, float& fraction)
{
	if(fRadius<1e-6)
		return RayIntersectWithTriangle( vecStart, vecDelta, Vertices, vCollidePos, fraction);

	bool bCheckEdge=false;

	// 构造三角形所在平面并求交
	CHalfSpace hs(Vertices[0],Vertices[1],Vertices[2]);
	float NDotD=DotProduct(hs.GetNormal(),vecDelta);
	float StartDist2Plane, EndDist2Plane;
	if(NDotD>0.0f)			
		return 0;				//不考虑back face的碰撞
	else
	{
		StartDist2Plane=hs.DistV2Plane(vecStart);
		if(NDotD==0.0f)
		{
			// 平行三角形平面移动的情况
			if(fabs(StartDist2Plane)<fRadius)
				bCheckEdge=true;
			else
				return 0;
		}
	}

	A3DVECTOR3 vecEnd=vecStart+vecDelta;
	EndDist2Plane=hs.DistV2Plane(vecEnd);
	
	if(StartDist2Plane > fRadius )
	{
		if(EndDist2Plane > fRadius)
			return 0;

		// 求解球心轨迹与偏移平面的交点
		fraction= fRadius-StartDist2Plane;
		fraction/=NDotD;
		vCollidePos=vecStart+fraction*vecDelta;

		// 交点是否在三角形内？
		A3DVECTOR3 vecHitPlanePos(vCollidePos-fRadius*hs.GetNormal());
		if(IsPointInTriangle(vecHitPlanePos, Vertices[0], Vertices[1], Vertices[2]))
		{
			vNormal=hs.GetNormal();
			return 1;
		}
		bCheckEdge=true;
	}
	else if(StartDist2Plane>0)	
		bCheckEdge=true;
	
	if(!bCheckEdge) return 0;

	// 开始判断是否和三角形的三边发生了碰撞
	float sqrVecDelta=vecDelta.SquaredMagnitude();
	float sqrvEdge;
	float sqrRadius=fRadius*fRadius;
	A3DVECTOR3 vEdge,vC2V;
	float a,b,c;	
	float EDotDelta, EDotC2V;
	float r1=0.0f,r2=0.0f;
	float t1=1.0f,t2;
	bool bFoundCollision=false;
	A3DVECTOR3 vHitPos;
	int res;

	// 是否需要进一步检测顶点
	// 暂时的这种判断并不准确，因此直接进行所有的操作！
	//bool bCheckV0=false,bCheckV1=false,bCheckV2=false;

	// v0->v1
	vEdge=Vertices[1]-Vertices[0];
	vC2V=Vertices[0]-vecStart;

	EDotDelta=DotProduct(vEdge,vecDelta);
	EDotC2V=DotProduct(vEdge,vC2V);
	sqrvEdge=vEdge.SquaredMagnitude();

	a=sqrVecDelta*sqrvEdge-EDotDelta*EDotDelta;
	b=2*EDotC2V*EDotDelta-2*sqrvEdge*DotProduct(vC2V,vecDelta);
	c=sqrvEdge*(vC2V.SquaredMagnitude()-sqrRadius)-EDotC2V*EDotC2V;
	
	res=SolveQuadratic(a,b,c,r1,r2);
	if(res>0 && r1 >= 0.0f && r1 < t1 )
	{
		t2=(EDotDelta*r1-EDotC2V)/sqrvEdge;
		if( t2>=0.0f && t2<=1.0f )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[0]+t2*vEdge;
		}
		// 需要进一步判断顶点是否碰撞的情况
		//else if(t2>1.0f) bCheckV1=true;
		//else bCheckV0=true;
	}

	// v1->v2
	vEdge=Vertices[2]-Vertices[1];
	vC2V=Vertices[1]-vecStart;

	EDotDelta=DotProduct(vEdge,vecDelta);
	EDotC2V=DotProduct(vEdge,vC2V);
	sqrvEdge=vEdge.SquaredMagnitude();

	a=sqrVecDelta*sqrvEdge-EDotDelta*EDotDelta;
	b=2*EDotC2V*EDotDelta-2*sqrvEdge*DotProduct(vC2V,vecDelta);
	c=sqrvEdge*(vC2V.SquaredMagnitude()-sqrRadius)-EDotC2V*EDotC2V;
	
	res=SolveQuadratic(a,b,c,r1,r2);
	if( res>0 && r1 >= 0.0f && r1 < t1 )
	{
		t2=(EDotDelta*r1-EDotC2V)/sqrvEdge;
		if( t2>=0.0f && t2<=1.0f )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[1]+t2*vEdge;
		}
		// 需要进一步判断顶点是否碰撞的情况
		//else if(t2>1.0f) bCheckV2=true;
		//else bCheckV1=true;
	}
	
	// v2->v0
	vEdge=Vertices[0]-Vertices[2];
	vC2V=Vertices[2]-vecStart;

	EDotDelta=DotProduct(vEdge,vecDelta);
	EDotC2V=DotProduct(vEdge,vC2V);
	sqrvEdge=vEdge.SquaredMagnitude();

	a=sqrVecDelta*sqrvEdge-EDotDelta*EDotDelta;
	b=2*EDotC2V*EDotDelta-2*sqrvEdge*DotProduct(vC2V,vecDelta);
	c=sqrvEdge*(vC2V.SquaredMagnitude()-sqrRadius)-EDotC2V*EDotC2V;
	
	res=SolveQuadratic(a,b,c,r1,r2);
	if(res>0 &&  r1 >= 0.0f && r1 < t1 )
	{
		t2=(EDotDelta*r1-EDotC2V)/sqrvEdge;
		if( t2>=0.0f && t2<=1.0f )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[2]+t2*vEdge;
		}
		// 需要进一步判断顶点是否碰撞的情况
		//else if(t2>1.0f) bCheckV0=true;
		//else bCheckV2=true;
	}
	
	// 开始检测顶点
	a=sqrVecDelta;

	//if(bCheckV0)
	{
		vC2V=vecStart-Vertices[0];		
		b=2* DotProduct(vecDelta,vC2V);
		c=vC2V.SquaredMagnitude()-sqrRadius;
		res=SolveQuadratic(a,b,c,r1,r2);
		if( res>0 && r1 >= 0.0f && r1 < t1 )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[0];
		}
	}

	//if(bCheckV1)
	{
		vC2V=vecStart-Vertices[1];		
		b=2* DotProduct(vecDelta,vC2V);
		c=vC2V.SquaredMagnitude()-sqrRadius;
		res=SolveQuadratic(a,b,c,r1,r2);
		if( res>0 && r1 >= 0.0f && r1 < t1 )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[1];
		}
	}

	//if(bCheckV2)
	{
		vC2V=vecStart-Vertices[2];		
		b=2* DotProduct(vecDelta,vC2V);
		c=vC2V.SquaredMagnitude()-sqrRadius;
		res=SolveQuadratic(a,b,c,r1,r2);
		if( res>0 && r1 >= 0.0f && r1 < t1 )
		{
			t1=r1;
			bFoundCollision=true;
			vHitPos=Vertices[2];
		}
	}

	if(bFoundCollision)
	{
		fraction=t1;
		vCollidePos=vecStart+t1*vecDelta;
		vNormal=vCollidePos-vHitPos;
		vNormal.Normalize();
		return 2;
	}
	else
		return 0;
}
#endif


int AABBOverlapTest(const A3DAABB& aabb1, const A3DAABB& aabb2)
{
	if(CLS_AABBAABBOverlap(aabb1.Center, aabb1.Extents, aabb2.Center, aabb2.Extents))
	{
		if( aabb1.IsAABBIn(aabb2) )
			return 3;
		
		if( aabb2.IsAABBIn(aabb1) )
			return 2;
		
		return 1;		
	}
	else
		return 0;
}

bool FastRayIntersectWithTriangle(const A3DVECTOR3& vecStart,const A3DVECTOR3& vecDelta,const A3DVECTOR3 Vertices[3],A3DVECTOR3& vCollidePos,float& fraction, bool bCullBackFace)
{
	float u,v,t;

	/* find vectors for two edges sharing vert0 */
	A3DVECTOR3 edge1(Vertices[1] - Vertices[0]),edge2(Vertices[2] - Vertices[0]);

	/* begin calculating determinant - also used to calculate U parameter */
	A3DVECTOR3 pvec = CrossProduct(vecDelta, edge2);
	A3DVECTOR3 tvec,qvec;
	float det,inv_det;

	/* if determinant is near zero, ray lies in plane of triangle */
	det = DotProduct(edge1,pvec);

#define FRIT_EPSILON 1e-6f

	if(bCullBackFace)			/* if culling is desired */
	{
		if(det < FRIT_EPSILON)
			return false;

		/* calculate distance from vert0 to ray origin */
		tvec =  vecStart - Vertices[0];

		/* calculate U parameter and test bounds */
		u = DotProduct(tvec,pvec);
		if(u < 0 || u > det)
			return false;
		
		/* prepare to test V parameter */
		qvec = CrossProduct(tvec, edge1);
		
		/* calculate V parameter and test bounds */
		v = DotProduct(vecDelta, qvec);
		if (v < 0 || u + v > det)
		  return false;

		/* calculate t, scale parameters, ray intersects triangle */
		t = DotProduct(edge2, qvec);
		inv_det = 1 / det;
		t *= inv_det;
		/*
		//*************************************************
		// Noted by wenfeng, 05-11-14
		// since we don't use u,v coordinates further, the following 
		// code can be skipped.
		//*************************************************
		u *= inv_det;
		v *= inv_det;
		*/
			
	}
	else		 /* the non-culling branch */
	{
		if (det > -FRIT_EPSILON && det < FRIT_EPSILON)
			return false;
		inv_det = 1 / det;

		/* calculate distance from vert0 to ray origin */
		tvec = vecStart - Vertices[0];

		/* calculate U parameter and test bounds */
		u = DotProduct(tvec,pvec) * inv_det;
		if ( u < 0 || u > 1)
			return false;

		/* prepare to test V parameter */
		qvec = CrossProduct(tvec, edge1);

		/* calculate V parameter and test bounds */
		v = DotProduct(vecDelta, qvec)* inv_det;
		if ( v < 0 || u + v > 1)
			return false;

		/* calculate t, ray intersects triangle */
		t = DotProduct(edge2, qvec) * inv_det;
	}
	
	//*****************************************
	// Added by wenfeng, 05-11-14
	// more test for t!
	//*****************************************
	if( t > 1 || t < 0 )
		return false;
	else
	{
		fraction = t;
		vCollidePos = vecStart + t * vecDelta;
		return true;
	}

// comment this to avoid warning C4067
// #undef FRIT_EPSILON 1e-6f 

}

}	// end namespace