#pragma once

#include "IconButton.h"
#include "BackBufWnd.h"
#include <vector>

class CMapGridSelectPolicy;

class CMapGrid : public CBackBufWnd
{
public:		//	Types

	//	Some constants
	enum
	{
		SIZE_RULERTHICK = 30,	//	Ruler thick in pixels
	};

public:		//	Constructor and Destructor

	CMapGrid();
	virtual ~CMapGrid();

public:		//	Attributes

public:		//	Operations

	bool Create(CWnd* pParent, const RECT& rc, int iTileSize);
	void SetSelectPolicy(CMapGridSelectPolicy *policy);
	typedef std::vector<CString> ProjList;
	bool SetTileAndRow(const ProjList &projList, int iNumCol, bool bDefaultCanSelect);
	void ResetSelect();
	void EnableSelect(int iRow, int iCol, bool bEnable);
	void EnableSelect(int iTileIndex, bool bEnable);

	bool GetProjName(int iProj,CString& name);
	bool IsSelected(int iTileIndex);
	bool CanSelect(int iTileIndex);

public:

	// Generated message map functions
	
protected:	//	Attributes

	ProjList	m_projList;
	int		m_iNumRow;		//	Row number
	int		m_iNumCol;		//	Col number
	int		m_iNumTile;		//	Tile number
	int		m_iTileSize;	//	Tile size in pixel
	int		m_iOffsetX;		//	Offset of table in tiles
	int		m_iOffsetY;
	int		m_iNumVisRow;	//	Complete visible row number
	int		m_iNumVisCol;	//	Complete visible column number

	CMapGridSelectPolicy *	m_pSelectPolicy;

protected:	//	Operations

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);

	//	Reset scroll bar
	void ResetScrollBar();
	//	Get tile index of specified position
	int TestTile(int x, int y);
	void GetRowCol(int iTileIndex, int &iRow, int &iCol);
	int	GetTile(int iRow, int iCol);


	//{{AFX_MSG(CMapGrid)
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//	地图区域选择策略
class CMapGridSelectPolicy
{
public:
	CMapGridSelectPolicy();
	virtual ~CMapGridSelectPolicy() = 0{}
	
	void Reset(int nRow, int nCol, bool bEnableSelect);
	void ClearSelectEnable(bool bEnable);
	
	int  GetRowNum()const{ return m_iNumRow; }
	int	 GetColNum()const{ return m_iNumCol; }
	int	 GetTileNum()const{ return m_iNumTile; }
	void GetRowCol(int iTileIndex, int &iRow, int &iCol)const;
	bool IsValid(int iTileIndex)const;
	bool IsValid(int iRow, int iCol)const;
	
	bool CanSelect(int iTileIndex);
	void EnableSelect(int iRow, int iCol, bool bEnable);
	void EnableSelect(int iTileIndex, bool bEnable);
	
	virtual bool IsSelected(int iTileIndex)=0;
	virtual void ClearSelect()=0;
	virtual bool OnLButtonDown(int iTile, UINT nFlags)=0;
	virtual bool OnMouseMove(int iTile, UINT nFlags)=0;
	virtual bool OnRButtonDown(int iTile,UINT nFlags) { return true;}
	
protected:
	virtual void ResetMore()=0;
	virtual void ClearSelectEnableMore(bool bEnable)=0;
	
private:	
	std::vector<bool>	m_bEnableSelect;
	
	int		m_iNumRow;
	int		m_iNumCol;
	int		m_iNumTile;
};

//	选择单个连续的矩形地图块区域
class CRangedMapGridSelectPolicy : public CMapGridSelectPolicy
{
public:
	CRangedMapGridSelectPolicy();
	
	//	继承自父类
	virtual bool IsSelected(int iTileIndex);
	virtual void ClearSelect();
	virtual bool OnLButtonDown(int iTileMouseOn, UINT nFlags);
	virtual bool OnMouseMove(int iTileMouseOn, UINT nFlags);
	
	//	新增
	bool GetSelect(int &iRow1, int &iRow2, int &iCol1, int &iCol2);
	
protected:
	
	//	继承自父类
	virtual void ResetMore();
	virtual void ClearSelectEnableMore(bool bEnable);
	
private:
	
	bool UpdateSelectRange(int iTileMouseOn);
	
private:
	int		m_iStartTile;	//	Current selected tile
	int		m_iEndTile;		//	Current selected tile
};

//	选择随意多个可分散地图块
class CSeperateMapGridSelectPolicy : public CMapGridSelectPolicy
{
public:
	
	//	继承自父类
	virtual bool IsSelected(int iTileIndex);
	virtual void ClearSelect();
	virtual bool OnLButtonDown(int iTileMouseOn, UINT nFlags);
	virtual bool OnMouseMove(int iTileMouseOn, UINT nFlags);

	//	新增
	void SelectAll(bool bSelect);
		
protected:
	
	//	继承自父类
	virtual void ResetMore();
	virtual void ClearSelectEnableMore(bool bEnable);
	
private:
	
	std::vector<bool>	m_bSelected;
};
