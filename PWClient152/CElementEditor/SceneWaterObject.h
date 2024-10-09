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


//#define WATER_OBJECT_DEBUG //���Կ���

//��ʾ����ռ�2Dƽ��(X,Z)�ϵ�һ����
typedef struct WGRID
{
	float x;
	float z;
}* PWGRID;

//�洢һ������
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

	//��������ˮ�ĳߴ�,�����½���ˮ
	void SetWaterSize(int nWidth,int nHeight,float nGridSize);
	
	void SetWaterWidth(int nWidth){ m_nAreaX = nWidth;};
	
	void SetWaterHeight(int nHeight){ m_nAreaZ = nHeight; };
	
	void SetWaterGridSize(float fGridSize){ m_fGridSize = fGridSize; };
	
	//�ͷſ���Ⱦ����
	void ReleaseRenderWater();

	//���½�����Ⱦˮ�飬���Ĵ�С���ƶ�λ�ú󣬱������
	void BuildRenderWater();

	//����ˮ�����ɫ
	void SetWaterColor(DWORD clr) { m_dwColor = clr; };
	
	//��engine����һ������Ⱦˮ��
	bool CreateRenderWater();

	//���ò��˳ߴ�
	void SetWaveSize(float size) { m_fWaveSize = size; }

	//�ж������Ƿ��ˮ�ཻ
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

	//����ˮ��ɼ����ӣ��������
	void CalculateVisibleChunk();
	
	//���������¼���ɼ���ˮ��
	void CalculateVisibleChunkFromProfile();
	
	//����ˮ���Ե�������������
	void CalculateProfile();

	void ProcessProfile();

	void SmoothProfile();

	bool RepairProfile();

	bool TestGridIntersectionWithWater(WGRID grid);

	//����������ͷ��ڴ�,���ƶ�����
	void ClearList()
	{ 
		APtrList<PWPROFILE> m_listNew;
		m_listWaterProfile = m_listNew; 
		CBitChunk newChunk;
		m_btWaterData = newChunk;
	};

	//����������ͷ��ڴ�
	void ReleaseProfile();

	void DrawGrid(A3DVECTOR3 vMin,A3DVECTOR3 vMax);//����һ��XZƽ�淽�飬������

	//for debug
	void DrawProfileGrid();
	void DrawWaterUnderGrid();

	int  FindGridDifferentPt(WGRID *pGridArray,int count,WGRID &grid1,WGRID &grid2);
	
	bool IsOrder(APtrList<PWGRID> &listGrid,WGRID w1,WGRID w2);

	bool IsOrder(WGRID oldGrid,WGRID w1,WGRID w2);

	bool IsEdge(WGRID grid);

	void DrawProfile();//���������ߣ�������
	
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

	//����ˮ��
	A3DVECTOR3 Similar(const A3DVECTOR3& vDelta);

	//������������
	void ReduceProfile();

	//������������������̫����CUT����������
	void CutProfile();

	void CircleProfile();

	//�ж�һ�������Ƿ��ǹر�
	bool ProfileIsClose(WPROFILE *pProfile);

	bool CameraUnderWater();

	void OnCamEnter();
	void OnCamLeave();

	CSceneObjectManager*	m_pSceneMan;

	int m_nAreaX;		//x ����ĸ�����
	int m_nAreaZ;		//y ����ĸ�����
	float m_fGridSize;	//���ӵ�ʵ�ʳߴ�m
	float m_fWaveSize;  //���˳ߴ�
	DWORD m_dwColor;
	int m_nStartPointX;
	int m_nStartPointZ;
	int m_nProfileWidth;
	int m_nProfileHeight;

	float m_fPixelValue;//����Ļ������ռ�ı�������

	CBitChunk m_btWaterData;//������ˮ������ݣ�1��ʾ��ǰ�鲻���ڣ�0��ʾ����

	//�����ˮ���������б�
	APtrList<PWPROFILE> m_listWaterProfile;
	bool m_bCamUnderWater;

	//For test
	//BYTE* m_pTestData;
};

typedef CSceneWaterObject* PSCENEWATEROBJECT;

#endif // !defined(AFX_SCENEWATEROBJECT_H__2562448C_4220_46AC_A967_79AD548548C2__INCLUDED_)
