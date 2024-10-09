#include "global.h"
#include "MainFrm.h"
#include "UndoMapAction.h"


//#define new A_DEBUG_NEW

//向当前撤消动作中加入影响元素的原始数据,以便恢复
void CUndoMapAction::AddGrid(int nIndex,float fData)
{
	for(int i = 0; i<m_nNumGridData; i++)
	{
		//我这儿是为了保存进来的数据都是最早的数据
		if(m_pGridData[i].nIndex == nIndex) return;
	}
	
	GridData* pGridData = new GridData[m_nNumGridData + 1];
	if(pGridData)
	{
		memcpy(pGridData,m_pGridData,sizeof(GridData)*m_nNumGridData);
		pGridData[m_nNumGridData].fData = fData;
		pGridData[m_nNumGridData].nIndex = nIndex;
	}
	if(m_pGridData)	delete []m_pGridData;
	m_pGridData = pGridData;
	m_nNumGridData++;
}

//	Release action
void CUndoMapAction::Release()
{
	if(m_pGridData) delete []m_pGridData; 
	m_nNumGridData = 0;
	m_pGridData = 0;
	m_rcUpdateRect.Clear();
}

//	Undo action
bool CUndoMapAction::Undo()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		if(pHeightMap)
		{
			for(int n=0; n<m_nNumGridData; n++)
			{
				int   index = m_pGridData[n].nIndex;
				float old = m_pGridData[n].fData;
				float height = pHeightMap->pHeightData[index];
				pHeightMap->pHeightData[index] = old;
				m_pGridData[n].fData = height;
			}
		}
		pMap->UpdateTerrainHeight(m_rcUpdateRect);
	}
	return true;
}

//	Redo action
bool CUndoMapAction::Redo()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		if(pHeightMap)
		{
			for(int n=0; n<m_nNumGridData; n++)
			{
				int   index = m_pGridData[n].nIndex;
				float old = m_pGridData[n].fData;
				float height = pHeightMap->pHeightData[index];
				pHeightMap->pHeightData[index] = old;
				m_pGridData[n].fData = height;
			}
		}
		pMap->UpdateTerrainHeight(m_rcUpdateRect);
	}
	return true;
}