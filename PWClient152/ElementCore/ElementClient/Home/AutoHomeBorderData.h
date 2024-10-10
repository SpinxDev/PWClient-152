/*
 * FILE: AutoHomeBorderData.h
 *
 * DESCRIPTION: Class for lines of home border data
 *
 * CREATED BY: Jiang Dalong, 2006/06/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHOMEBORDERDATA_H_
#define _AUTOHOMEBORDERDATA_H_

extern const int NUM_BORDER_BLOCK;
extern const int NUM_HOME_BORDER_POINT;

class CAutoHomeBorderData  
{
public:
	CAutoHomeBorderData();
	virtual ~CAutoHomeBorderData();

	void Release();

	bool LoadPoints(const char* szFile);
	bool SavePoints(const char* szFile);


	void SetPointCrossByIndex(int nIndex, float fValue);
	void SetPointRowByIndex(int nIndex, float fValue);
	void SetPointColByIndex(int nIndex, float fValue);
	inline float GetPointCrossByIndex(int nIndex)	{ return m_pPointCross[nIndex]; }
	inline float GetPointRowByIndex(int nIndex)		{ return m_pPointRow[nIndex]; }
	inline float GetPointColByIndex(int nIndex)		{ return m_pPointCol[nIndex]; }
	inline bool IsEdited()		{ return m_bEdited; }

protected:
	float*					m_pPointCross;	// Cross points
	float*					m_pPointRow;	// Row points
	float*					m_pPointCol;	// Column points
	int						m_nNumPointCross;
	int						m_nNumPointRow;
	int						m_nNumPointCol;

	bool					m_bEdited;

protected:
	// Get home top points
	bool GetHomeTopPoints(int r, int c, float* pPoints);
	// Get home bottom points
	bool GetHomeBottomPoints(int r, int c, float* pPoints);
	// Get home left points
	bool GetHomeLeftPoints(int r, int c, float* pPoints);
	// Get home right points
	bool GetHomeRightPoints(int r, int c, float* pPoints);
	// Get home top value
	float GetTopValue(int r, int c, float fWeight);
	// Get home bottom value
	float GetBottomValue(int r, int c, float fWeight);
	// Get home left value
	float GetLeftValue(int r, int c, float fWeight);
	// Get home right value
	float GetRightValue(int r, int c, float fWeight);

};

#endif // #ifndef _AUTOHOMEBORDERDATA_H_
