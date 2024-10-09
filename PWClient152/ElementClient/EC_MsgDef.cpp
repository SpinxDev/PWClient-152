 /*
 * FILE: EC_MsgDef.cpp
 *
 * DESCRIPTION: Element client, Message data structure defination
 *
 * CREATED BY: liaoyangguang, 2014.07.16
 *
 * HISTORY:
 */

#include "EC_Game.h"
#include "EC_MsgDef.h"
#include "EC_MsgDataDef.h"


ECMSG::ECMSG()
:pMsgData(NULL)
{
}

ECMSG::~ECMSG()
{
	delete pMsgData;
}

ECMSG::ECMSG( const ECMSG &rhs)
:dwMsg(rhs.dwMsg)
,iManager(rhs.iManager)
,iSubID(rhs.iSubID)
,dwParam1(rhs.dwParam1)
,dwParam2(rhs.dwParam2)
,dwParam3(rhs.dwParam3)
,dwParam4(rhs.dwParam4)
{
	pMsgData = NULL;
	if (rhs.pMsgData){
		pMsgData = rhs.pMsgData->Clone();
	}
}

ECMSG & ECMSG::operator = (const ECMSG &rhs)
{
	dwMsg = rhs.dwMsg;
	iManager = rhs.iManager;
	iSubID = rhs.iSubID;
	dwParam1 = rhs.dwParam1;
	dwParam2 = rhs.dwParam2;
	dwParam3 = rhs.dwParam3;
	dwParam4 = rhs.dwParam4;
	delete pMsgData;
	if (rhs.pMsgData){
		pMsgData = rhs.pMsgData->Clone();
	}else{
		pMsgData = NULL;
	}
	
	return *this; 
}