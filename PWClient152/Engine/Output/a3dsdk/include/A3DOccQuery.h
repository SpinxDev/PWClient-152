/*
* FILE: A3DOccQuery.h
*
* DESCRIPTION: Occlusion query
*
* CREATED BY: Yaochunhui, 2012/9/10
*
* HISTORY:
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3DOCCQUERY_H_
#define _A3DOCCQUERY_H_

#include "ABaseDef.h"
#include "AString.h"
#include "A3DPlatform.h"

class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DOccQuery
//
///////////////////////////////////////////////////////////////////////////

enum OccQueryCommand
{
    OCC_CMD_BEGIN,  // begin query
    OCC_CMD_END,    // end query
    OCC_CMD_GET,    // get result
};

class IA3DOccQueryImpl : public IUnknown
{
public:
    // do a query
    virtual bool BeginQuery() = 0;
    virtual void EndQuery(LONG lID) = 0;
    //	Get query result, -1 for not finished
    virtual void GetData() = 0;
};

class A3DOccQuery
{
public:
    A3DOccQuery();
    virtual ~A3DOccQuery();

    bool Init(A3DDevice* pA3DDevice);
    void Release();
    // do a query 
    bool BeginQuery();
    void EndQuery();

    //	Get query result, returning -1 for not finished
    int GetDataAsync();

protected:
    IA3DOccQueryImpl* m_pImpl;
    A3DDevice* m_pDevice;
};

#endif	//	_A3DOCCQUERY_H_