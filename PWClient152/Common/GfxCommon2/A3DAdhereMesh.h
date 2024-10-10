#ifndef _A3DADHEREMESH_H_
#define _A3DADHEREMESH_H_

// adhere a source mesh to dest mesh
// then source mesh driven by dest mesh
// map vertex in source mesh to the triangle in dest
// 

class A3DClothMeshImp;

class A3DAdhereMesh
{
public:

	struct AdhereParam
	{
		int iTriangleIdx;
		float    m_fDist;//distance from source vertex to dest triangle plane
		float    m_fWeight1;
		float    m_fWeight2;

		float    m_fAdhereWeight;//adhere weight (0-1)
	};

	A3DAdhereMesh();
	virtual ~A3DAdhereMesh();
	virtual void Release();

	// adhere pSrcMesh to pDstMesh
	bool Init(A3DSkin* pSkin, A3DSkinModel* pModel, A3DClothMeshImp* pSrcMesh, A3DClothMeshImp* pDstMesh, AdhereParam* pAdherePara = NULL);
	// sync source mesh from dest mesh
	void SyncSrcMesh();

	A3DClothMeshImp* GetSrcClothMesh() { return m_pSrcMesh; }
	AdhereParam*     GetAdherParam() { return m_pAdereParam; }

protected:
	int GetClothMeshVertNum(A3DClothMeshImp* pClothMesh);
	int GetClothMeshIdxNum(A3DClothMeshImp* pClothMesh);
	void GetClothMeshOriVertsAndIdxs(A3DClothMeshImp* pClothMesh, abase::vector<A3DVECTOR3>& vs, abase::vector<DWORD>& Idxs);

	void GetBlendedVertices(A3DVECTOR3* pVert);
	
protected:
	
	A3DSkin* m_pSkin;
	A3DSkinModel* m_pSkinModel;
	
	A3DClothMeshImp*	 m_pSrcMesh;
	A3DClothMeshImp*     m_pDstMesh;

	A3DVECTOR3*      m_pSyncVert;
	A3DVECTOR3*      m_pSyncNormal;
	int*             m_pSyncIndex;

	A3DVECTOR3*      m_pTriNormal;

	AdhereParam*     m_pAdereParam; // one for per vertex in source mesh

};

#endif