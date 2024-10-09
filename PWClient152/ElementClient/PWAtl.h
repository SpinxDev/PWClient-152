/*
 * FILE: PWAtl.h
 *
 * DESCRIPTION: general .h file to use the atl and web browser activex control
 *
 * CREATED BY:  linzhehui, 2009/06/10
 *
 * HISTORY:  2009/06/10, First edition by linzhehui
 *            
 * Decide to Use ATL to embed the web browser into the game ui for conviniance reasons.
 */

#pragma once	// be included (opened) only once by the compiler in a build


#include <atlbase.h>	// General ATL base .h
extern CComModule	_Module;
#if !defined(ANGELICA_2_2)
#include "my_atlcom.h"		// ATL COM Interface .h
#endif
#include <atlhost.h>	// ATL activex container .h
#include <EXDISPID.H>   // Web browser Interfaces .h
#include "SafeWinImpl.h"	// Safe Win Impl from internet