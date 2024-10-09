#pragma once

#include "ECScriptOption.h"


class CECScriptOptionInGame : public CECScriptOption
{
protected:
	virtual void LoadOption(PONRECEIVEOPTIONS pfnCallBack);
	virtual void SaveOption(const void* pDataBuf, int iSize);
public:
//	virtual bool Init(CECScriptContext *pContext);
//	virtual void LoadFromServer();	
};