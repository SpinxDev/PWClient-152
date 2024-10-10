/*
* FILE: AGPAPerfIntegration.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/2/22
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AGPAPerfIntegration_H_
#define _AGPAPerfIntegration_H_

#ifdef _DBGRELEASE
#include "ittnotify.h"
#include "ittxnotify.h"
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#ifdef _DBGRELEASE
#define _ENABLE_INTEL_GPA
#endif


#ifdef _ENABLE_INTEL_GPA
#define A3D_ITTX_SCOPED_TASK_NAMED(domain, name) \
	static __itt_string_handle* _ittx_scoped_task_string_handle_##__LINE__ = __itt_string_handle_create(name); \
	__ittx_scoped_task _ittx_scoped_task_##__LINE__(domain, __itt_null, __itt_null, _ittx_scoped_task_string_handle_##__LINE__)
#else
#define A3D_ITTX_SCOPED_TASK_NAMED(domain, name)
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct ___itt_domain;
typedef ___itt_domain __itt_domain;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AGPAPerfIntegration
//	
///////////////////////////////////////////////////////////////////////////

class AGPAPerfIntegration
{
public:		//	Types

private:	//	Constructor and Destructor

	AGPAPerfIntegration(void);
	virtual ~AGPAPerfIntegration(void);

public:		//	Attributes

public:		//	Operations

	static AGPAPerfIntegration& GetInstance();
	__itt_domain* GetA3DDomain() const { return m_pA3DDomain; }

private:	//	Attributes

	__itt_domain* m_pA3DDomain;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AGPAPerfIntegration_H_

