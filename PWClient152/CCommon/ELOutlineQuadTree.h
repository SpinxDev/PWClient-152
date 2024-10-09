/*
 * FILE: ELOutlineQuadTree.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Wangkuiwu, 2005/7/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef   _EL_OUTLINE_QUAD_TREE_H_
#define   _EL_OUTLINE_QUAD_TREE_H_

#include <A3DTerrain2Env.h>
#include <A3DTerrain2LOD.h>

class CELOutlineTexture;
class CELOutlineTextureCache;
class A3DVECTOR3;
class A3DDevice;
class A3DStream;
class A3DTerrain2;
class TerrainBlockInfo;


/**< tile: 1024*1024 meter^2,  block: 64*64 meter^2, grid: 2*2 meter^2  */


#define OUTLINE_QNODE_ACTIVE  0x01
#define OUTLINE_BLK_LEN           32



enum OUTLINE_QNODE_EDGE{ OLQ_EDGE_TOP = 0, OLQ_EDGE_RIGHT, OLQ_EDGE_BOTTOM, OLQ_EDGE_LEFT};
enum OUTLINE_QNODE_CHILD{OLQ_CHILD_LEFTTOP = 0,  OLQ_CHILD_RIGHTTOP, OLQ_CHILD_LEFTBOTTOM, OLQ_CHILD_RIGHTBOTTOM, OLQ_CENTER };

class CELOutlineQuadNode
{
public:
	  WORD ix, iz;          ///< top left in grid
	  WORD iLen;
	  WORD iActiveFlag;
  	  WORD   iCenterVert;   //the index 
	  int  iParent;
	  int  iChildren[4];
	  
	  bool  bRender;
	  int  nEdgeVert[4];
/*        the edge vert order
 *			|--------------->		
 *          |				|
 *			|				|
 *			|				|
 *			|				|
 *		   \|/			   \|/
 *			 --------------->
 */	  
	  WORD * pEdgeVert[4];  

	  CELOutlineQuadNode(){
		  pEdgeVert[0] = NULL;
		  pEdgeVert[1] = NULL;
		  pEdgeVert[2] = NULL;
		  pEdgeVert[3] = NULL;
	  }
	  ~CELOutlineQuadNode(){
		  for (int i = 0; i < 4; i++) {
			  if (pEdgeVert[i]) {
				  delete[] pEdgeVert[i];
			  }
		  }
	  }
	  bool IsLeaf(){
		  return (iChildren[0] == -1);
	  }
	  bool IsActive(){
		  return ((iActiveFlag& OUTLINE_QNODE_ACTIVE) == OUTLINE_QNODE_ACTIVE);
	  }
};

class CELOutlineQuadTree
{
	friend class CELOutlineTextureCache;
public:
	CELOutlineQuadTree();
	~CELOutlineQuadTree();
	bool  Init(A3DDevice * pDevice, int iSubIndex, float wx, float wz);
	void  SetOwnerTex(CELOutlineTexture * pELOTex);
	void  Release();
	bool  Load(AString& szDir);
	void  CreateView(DWORD dwFrame, const ARectI& rcSeam, const ARectI& rcView);

	void  StitchAndCompleteTerrain(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible,  A3DTerrain2LOD::GRID * pTerGrid);


	void Render(A3DCameraBase * pCamera);

	void   ClearSeamStream(){
		m_nSeamFace = m_nSeamVert = 0;
	}

private:

	struct VERT_INDEX_INFO
	{
		WORD ix, iz;
		int  index;
	};

	// the edge used to seam, may be from terrain  or olm 
	struct Seam_Edge_Info 
	{
		A3DVECTOR3  * pVerts;  ///< NULL if the edge comes from olm
		int           nVerts;
		int           nInterval; ///< the interval len in grid
	};
	// the block used to seam
	struct Seam_Block_Info
	{
		CELOutlineQuadNode * pBlkQNode;  ///< the block quad node 
		Seam_Edge_Info   edge[4];
	};

	
	void _SetQNodeRenderFlag(ARectI& rc, bool bRender);
	
	int _ExtractBlkEdge(A3DTerrain2Block * pBlock, A3DTerrain2LOD::GRID * pTerGrids,int edge, int& nVert, A3DVECTOR3 * pVerts);
	void _StitchTop(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchBottom(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchLeft(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchRight(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);


	void _StitchTop2(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchBottom2(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchLeft2(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _StitchRight2(int tblr, int iStart, int iEnd,  A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);


	void _StitchSingleBlkTop(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, 
					int nBlkEdge, int iBlkEdgeLen, int iLeft, int iTop,
					A3DLVERTEX * pVerts, WORD* pIndices);

	void _StitchSingleBlkBottom(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, 
					int nBlkEdge, int iBlkEdgeLen, int iLeft, int iTop,
					A3DLVERTEX * pVerts, WORD* pIndices);

	void _StitchSingleBlkRight(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, 
					int nBlkEdge, int iBlkEdgeLen, int iLeft, int iTop,
					A3DLVERTEX * pVerts, WORD* pIndices);
	void _StitchSingleBlkLeft(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, 
					int nBlkEdge, int iBlkEdgeLen, int iLeft, int iTop,
					A3DLVERTEX * pVerts, WORD* pIndices);

	void _AddSeam(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, 
					int nBlkEdge, int iBlkEdgeLen, int iEdge, int iLeft, int iTop, 
					A3DLVERTEX * pVerts, WORD* pIndices);
	void _AddSeam(CELOutlineQuadNode * pQNode, const Seam_Block_Info& seamInfo,
					A3DLVERTEX * pVerts, WORD* pIndices);


	int  _GetSeamVertIndex(int ix, int iz, int iLeft, int iTop, const A3DVECTOR3& pos, A3DLVERTEX * pVerts);

	void _ExtractNodeEdge(CELOutlineQuadNode * pNode);
	int   _ExtractNodeEdge(CELOutlineQuadNode * pQNode, int EdgeFlag, A3DVECTOR3 * ppEdgeVert[]);
	void  _BuildStream();
	void  _CompleteTerrain(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible,  A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void  _Sticth(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);	
	void  _Sticth2(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);	

	void _StitchBlk(const Seam_Block_Info& seamInfo,	A3DLVERTEX * pVerts, WORD* pIndices);
	void _StitchQNode(CELOutlineQuadNode * pQNode, const Seam_Block_Info& seamInfo, A3DLVERTEX * pVerts, WORD * pIndices);


	void _CompleteTerrain(int r, int c, A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);
	void _CompleteTerrain2(int r, int c, A3DTerrain2::ACTBLOCKS *pActBlocks,  A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices);

	float _GetHeight(float x, float z);
	CELOutlineQuadNode * _GetBlkQNode(float x, float z);
	CELOutlineQuadNode * _GetBlkQNode(int ix, int iz);

	


	CELOutlineTexture * m_pELOTexture;
	A3DDevice *		m_pA3DDevice;

	ARectF              m_RectWorld;
	ARectI              m_Rect;				///< tile rect area in grid  (2*2  meter^2)
	ARectI              m_RectBlk;          ///< tile rect area in block (64*64 meter^2)
	
	int                 m_iSubIndex;
	
	A3DVECTOR3        * m_pVertPos;
	int                 m_nVert;
	int                 m_nFace;
	int                 m_nMaxFace;
	A3DStream		*   m_pStream;           //the render stream

	A3DStream       *   m_pSeamStream;
	int                 m_nSeamVert;
	int                 m_nSeamFace; 


	CELOutlineQuadNode * m_pQNodes;
	int                  m_nQNodes;

	bool                 m_bStreamDirty;

	VERT_INDEX_INFO     m_SeamVertIndexInfo[OUTLINE_BLK_LEN+1][OUTLINE_BLK_LEN+1];

	A3DVECTOR3       m_vBBMins;  
	A3DVECTOR3       m_vBBMaxs;

	bool             m_bLoadOk;

};



#endif 
