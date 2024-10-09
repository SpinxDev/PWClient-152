//----------------------------------------------------------
// Filename	: UndoMapAction.h
// Creator	: QingFeng Xin
// Date		: 2004.8.12
// Desc		: 撤消操作拓展类,主要用于对灰度图的恢复操作
//-----------------------------------------------------------

#ifndef _UNDO_MAP_ACTION_H
#define _UNDO_MAP_ACTION_H

#include "UndoAction.h"

struct GridData
{
	float fData;
	int   nIndex;
};

class CUndoMapAction :public CUndoAction
{

public:
	CUndoMapAction(){ m_pGridData = 0; m_nNumGridData = 0; };
	virtual ~CUndoMapAction()
	{ 
		if(m_pGridData) 
		{
			delete []m_pGridData; 
			m_pGridData = 0; 
		}
	};

public:

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

public:
	void AddGrid(int nIndex,float fData);
	void SetUpdateRect( ARect<float> &rc){ m_rcUpdateRect = rc; };

protected:
	
private:
	
	GridData*		m_pGridData;
	int				m_nNumGridData;
	ARect<float>    m_rcUpdateRect;

};


#endif
