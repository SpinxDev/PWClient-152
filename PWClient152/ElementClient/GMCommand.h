/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   17:07
	file name:	GMCommand.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "Param.h"

class CGMCommand  
{
protected:
	CParamArray m_paramArray;
protected:
	void AddParam(CParam *pParam)
	{
		m_paramArray.push_back(pParam);
	}
public:
	const CParamArray & GetParamArray() { return m_paramArray; }
	void SetParamArray(const CParamArray & paramArray) { m_paramArray = paramArray; }
	virtual ~CGMCommand() {}

	virtual bool HasPrivilege() = 0;
	virtual ACString GetDisplayName() = 0;
	virtual bool Execute(ACString &strErr, ACString &strDetail) = 0;
	
	// specially provided for UI
	// override it if the concreate command have
	// a playerid param
	virtual void TrySetPlayerIDParam(int nPlayerID) {}

	virtual void SetCurSelParam(int idx) {} // 选的哪个参数
};

