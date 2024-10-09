/********************************************************************
  created:	   25/10/2005  
  filename:	   EC_BrushMan.h
  author:      Wangkuiwu  
  description: brush manager for cd
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma  once

#include "EC_BrushManUtil.h"


//@desc : A interface for building brush manager. By Kuiwu[25/10/2005]
class CBrushProvider
{
	
public:
	CBrushProvider() 
	{
	};
	virtual ~CBrushProvider()
	{
	};
	virtual int GetBrushCount() = 0;
	virtual CCDBrush * GetBrush(int index) = 0;
protected:

private:


};


//@todo : REFACTOR!  use GOF bridge pattern to support 
//		  different discrete implementations(tree or grid, etc), 
//		  but it seems unnecessary.	  	 By Kuiwu[4/11/2005]
class CECBrushMan  
{
public:
	CECBrushMan();
	virtual ~CECBrushMan();
	void Release();
	void Build(const  A3DVECTOR3& vCenter, bool bForce = false);
	bool RemoveProvider(CBrushProvider * pBrushProvider);
	/*
	 * 
	 * @desc :
	 * @param :     
	 * @return :
	 * @note:  the caller should assure not to add duplicate providers.
	 * @todo:   
	 * @author: kuiwu [27/10/2005]
	 * @ref:
	 */
	void AddProvider(CBrushProvider * pBrushProvider);
	/*
	 *
	 * @desc :
	 * @param bCheckFlag: check the brush flag if true, UGLY, but for compatibility.    
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [27/10/2005]
	 * @ref:
	 */
	bool  Trace(BrushTraceInfo * pInfo, bool bCheckFlag = true);

#if BMAN_VERBOSE_STAT
	CBManStat * GetStat()
	{
		return m_pBrushGrid->GetStat();
	}
#endif

private:
	
	CBrushGrid    * m_pBrushGrid;
};

