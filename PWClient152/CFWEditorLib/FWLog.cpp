// FWLog.cpp: implementation of the FWLog class.
//
//////////////////////////////////////////////////////////////////////

#include "FWLog.h"



IMPLEMENT_SINGLETON(FWLog)

FWLog::FWLog()
{

}

FWLog::~FWLog()
{
	Release();
}
