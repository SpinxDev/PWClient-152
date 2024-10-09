#include "global.h"
#include "MainFrm.h"
#include "UndoMaskAction.h"
#include "TerrainLayer.h"
#include "MaskModifier.h"


//#define new A_DEBUG_NEW


//向当前撤消动作中加入影响元素的原始数据,以便恢复
//向当前撤消动作中加入影响元素的原始数据,以便恢复
void CUndoMaskAction::AddGrid( int w,int h,BYTE by)
{
	int idx = m_iMapSize * h + w;
	if(m_mapGridData.find(idx) != m_mapGridData.end()) return;
	
	GrayGridData dat;
	dat.byteData = by;
	dat.nW = w;
	dat.nH = h;
	m_mapGridData[idx] = dat;
}

void CUndoMaskAction::AddData(ARectI&rc,  CELBitmap::LOCKINFO &lockInfor)
{
	BYTE* pLine = lockInfor.pData;
	for( int h = 0; h < lockInfor.iHeight; ++h)
	{
		// we should add iPitch after processing each line, NOT before.
		// pLine += lockInfor.iPitch;
		for( int w = 0; w < lockInfor.iWidth; ++w)
		{
			BYTE dat = *(pLine + w);
			AddGrid(w + rc.left,h + rc.top, dat);
		}
		pLine += lockInfor.iPitch;
	}
}

//	Release action
void CUndoMaskAction::Release()
{
	m_mapGridData.clear();
	m_rcUpdateRect.Clear();
}

//	Undo action
bool CUndoMaskAction::Undo()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain *pTerrain = pMap->GetTerrain();
		CTerrainLayer *pCurrentLay = pTerrain->GetLayer(m_nLay);
		if(pCurrentLay==NULL) return false;
		CELBitmap *pMaskBitmap = pCurrentLay->GetMaskBitmap();
		if(pMaskBitmap==NULL) return false;
		CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
		
		
		CELBitmap::LOCKINFO lockInfor;
		pMaskBitmap->LockRect(m_rcUpdateRect,&lockInfor);
		
		abase::hash_map<int,GrayGridData>::iterator it = m_mapGridData.begin();
		for(it; it != m_mapGridData.end(); ++it)
		{
			int  w = (it->second).nW;
			int  h = (it->second).nH;
			BYTE old = (it->second).byteData;
			int index = (h-m_rcUpdateRect.top)*lockInfor.iPitch+(w-m_rcUpdateRect.left);
			BYTE height =lockInfor.pData[index];
			lockInfor.pData[index] = (BYTE)old;
			(it->second).byteData = height;
		}
		pMap->GetTerrain()->GetMaskModifier()->UpdateMaskMapToRender(m_rcUpdateRect);
	}
	return true;
}

//	Redo action
bool CUndoMaskAction::Redo()
{
	Undo();
	return true;
}