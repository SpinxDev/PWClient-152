// SceneWaterObject.h: interface for the CSceneWaterObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEWATEROBJECT_H__2562448C_4220_46AC_A967_79AD548548C2__INCLUDED_)
#define AFX_SCENEWATEROBJECT_H__2562448C_4220_46AC_A967_79AD548548C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AList2.h"
#include "SceneObject.h"
#include "BitChunk.h"


//#define WATER_OBJECT_DEBUG //调试开关

//表示世界空间2D平面(X,Z)上的一个点
typedef struct WGRID
{
	float x;
	float z;
}* PWGRID;

//存储一条轮廓
typedef struct WPROFILE
{
	APtrList<PWGRID> listGrid;
}* PWPROFILE;

class CSceneObjectManager;

class CSceneWaterObject : public CSceneObject
{
public:
	CSceneWaterObject(CSceneObjectManager* pSceneMan);
	virtual ~CSceneWaterObject();

	virtual void				Translate(const A3DVECTOR3& vDelta);
	virtual void				SetPos(const A3DVECTOR3& vPos);

	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	
	//  Copy object
	virtual CSceneObject*		CopyObject();

	//重新设置水的尺寸,并重新建立水
	void SetWaterSize(int nWidth,int nHeight,float nGridSize);
	
	void SetWaterWidth(int nWidth){ m_nAreaX = nWidth;};
	
	void SetWaterHeight(int nHeight){ m_nAreaZ = nHeight; };
	
	void SetWaterGridSize(float fGridSize){ m_fGridSize = fGridSize; };
	
	//释放可渲染对象
	void ReleaseRenderWater();

	//重新建立渲染水块，更改大小和移动位置后，必须调用
	void BuildRenderWater();

	//设置水面的颜色
	void SetWaterColor(DWORD clr) { m_dwColor = clr; };
	
	//用engine创建一个可渲染水块
	bool CreateRenderWater();

	//设置波浪尺寸
	void SetWaveSize(float size) { m_fWaveSize = size; }

	//判断区域是否跟水相交
	bool IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);

	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags) { return true; }
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion) { return true; }

	void GetProperty(int &nAreaX, int &nAreaZ,float &fGridSize, float &fWaveSize,DWORD &dwColor)
	{
		nAreaX = m_nAreaX;
		nAreaZ = m_nAreaZ;
		fGridSize = m_fGridSize;
		fWaveSize = m_fWaveSize;
		dwColor = m_dwColor;
	};

	void SetProperty(int nAreaX, int nAreaZ,float fGridSize, float fWaveSize,DWORD dwColor)
	{
		m_nAreaX = nAreaX;
		m_nAreaZ = nAreaZ;
		m_fGridSize = fGridSize;
		m_fWaveSize = fWaveSize;
		m_dwColor = dwColor;
	};
	
	void BuildProperty();

	void SetSceneManager(CSceneObjectManager* pSceneMan) { m_pSceneMan = pSceneMan; }

	A3DAABB GetAABB() const;
private:
	bool GridIsExist(APtrList<PWGRID> &listGrid,WGRID grid);

	//计算水面可见格子，结果保存
	void CalculateVisibleChunk();
	
	//从轮廓重新计算可见的水块
	void CalculateVisibleChunkFromProfile();
	
	//计算水面边缘轮廓，结果保存
	void CalculateProfile();

	void ProcessProfile();

	void SmoothProfile();

	bool RepairProfile();

	bool TestGridIntersectionWithWater(WGRID grid);

	//清除表，但不释放内存,复制对象用
	void ClearList()
	{ 
		APtrList<PWPROFILE> m_listNew;
		m_listWaterProfile = m_listNew; 
		CBitChunk newChunk;
		m_btWaterData = newChunk;
	};

	//清除轮廓，释放内存
	void ReleaseProfile();

	void DrawGrid(A3DVECTOR3 vMin,A3DVECTOR3 vMax);//绘制一个XZ平面方块，调试用

	//for debug
	void DrawProfileGrid();
	void DrawWaterUnderGrid();

	int  FindGridDifferentPt(WGRID *pGridArray,int count,WGRID &grid1,WGRID &grid2);
	
	bool IsOrder(APtrList<PWGRID> &listGrid,WGRID w1,WGRID w2);

	bool IsOrder(WGRID oldGrid,WGRID w1,WGRID w2);

	bool IsEdge(WGRID grid);

	void DrawProfile();//绘制轮廓线，调试用
	
	void DrawLine(WGRID old,WGRID cur,DWORD clr);

	//for debug
	void DrawLine2(A3DVECTOR3 v1,A3DVECTOR3 v2, DWORD clr);
	
	void DrawWaterSurface();
	
	bool FindStartPoint(int& sx, int& sy,BYTE *pData,BYTE *pFlag);
	
	void CreateProfile(BYTE*pData);
	
	bool FindGrid(PWPROFILE pProfile,BYTE *pData,BYTE *pFlag);
	
	bool PlaneIntersectionWithLine(A3DVECTOR3 vStart,A3DVECTOR3 vEnd,A3DVECTOR3 point,A3DVECTOR3 normal,A3DVECTOR3 &out);
	
	bool GetNextPoint(int lastDir, int cx,int cy, int& nx, int& ny, int& newDir
									  ,BYTE *pData,BYTE *pFlag);

	bool IsIntersectionWithWater(A3DVECTOR3 pt1,A3DVECTOR3 pt2);
	
	void MoveWater();

	//对齐水块
	A3DVECTOR3 Similar(const A3DVECTOR3& vDelta);

	//对轮廓做精简
	void ReduceProfile();

	//对于轮廓上两点间距离太长的CUT成两条轮廓
	void CutProfile();

	void CircleProfile();

	//判断一条轮廓是否是关闭
	bool ProfileIsClose(WPROFILE *pProfile);

	bool CameraUnderWater();

	void OnCamEnter();
	void OnCamLeave();

	CSceneObjectManager*	m_pSceneMan;

	int m_nAreaX;		//x 方向的格子数
	int m_nAreaZ;		//y 方向的格子数
	float m_fGridSize;	//格子的实际尺寸m
	float m_fWaveSize;  //波浪尺寸
	DWORD m_dwColor;
	int m_nStartPointX;
	int m_nStartPointZ;
	int m_nProfileWidth;
	int m_nProfileHeight;

	float m_fPixelValue;//从屏幕到世界空间的比例缩放

	CBitChunk m_btWaterData;//这儿存放水块的数据，1表示当前块不存在，0表示存在

	//这儿是水的轮廓的列表
	APtrList<PWPROFILE> m_listWaterProfile;
	bool m_bCamUnderWater;

	//For test
	//BYTE* m_pTestData;
};

typedef CSceneWaterObject* PSCENEWATEROBJECT;

#endif // !defined(AFX_SCENEWATEROBJECT_H__2562448C_4220_46AC_A967_79AD548548C2__INCLUDED_)
