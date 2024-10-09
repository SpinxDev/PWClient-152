//----------------------------------------------------------
// Filename	: UndoMaskAction.h
// Creator	: QingFeng Xin
// Date		: 2004.8.13
// Desc		: ����������չ��,��Ҫ���ڶԻҶ�ͼ�Ļָ�����,���
//            ����ʱû�ã���˶���MASK��UNDO��û�������������
//			  ������ӵ������У��ѳ������Թ�
//-----------------------------------------------------------

#ifndef _UNDO_MASK_ACTION_H
#define _UNDO_MASK_ACTION_H

#include "UndoAction.h"
#include "Bitmap.h"
#include "hashmap.h"


class CUndoMaskAction :public CUndoAction
{
public:
	struct GrayGridData
	{
		BYTE  byteData;
		int   nW;
		int   nH;
	};	

public:
	CUndoMaskAction( int iMapSize) : m_nLay(-1), m_iMapSize(iMapSize) {}
	virtual ~CUndoMaskAction()
	{
		Release();
	};

public:

	//	Release action
	virtual void Release();
	//	Undo action
	virtual bool Undo();
	//	Redo action
	virtual bool Redo();

public:
	void AddData(ARectI&rc, CELBitmap::LOCKINFO &lockInfor);
	void AddGrid(int w,int h,BYTE by);
	void SetUpdateRect( ARect<int> &rc, int nLay )
	{ 
		m_rcUpdateRect = rc; 
        m_nLay = nLay;
	};

protected:
	
private:
	
	int									m_nLay;
	int									m_iMapSize;
	ARectI							m_rcUpdateRect;
	abase::hash_map<int,GrayGridData>	m_mapGridData;
};


#endif
