/********************************************************************
	created:	2005/08/31
	created:	31:8:2005   11:00
	file name:	CodeTemplate.h
	author:		yaojun
	
	purpose:	some frequently used utilities
*********************************************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////
// code structure helper
//////////////////////////////////////////////////////////////////////////

#define SAFE_DELETE(p) {delete (p); (p) = NULL;}
#define SAFE_DELETE_ARRAY(p) {delete [] (p); (p) = NULL;}

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

#define CHECK_BREAK(p) {if (!(p)) break;}

#define BEGIN_FAKE_WHILE	\
	bool bSucceed = false;  \
do {

#define END_FAKE_WHILE		\
	bSucceed = true;	\
} while (false);

#define RETURN_FAKE_WHILE_RESULT \
return bSucceed;

#define BEGIN_ON_FAIL_FAKE_WHILE	\
if (!bSucceed) { 

#define END_ON_FAIL_FAKE_WHILE	\
}
