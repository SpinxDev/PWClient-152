/*
 * FILE: A3DCBReleaseResRecorder.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 2
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_CB_RELEASE_RES_RECORDER_H_
#define _A3D_CB_RELEASE_RES_RECORDER_H_

#include "A3DCBRecorderBase.h"


class A3DCBReleaseResRecorder : public A3DCBRecorderBase
{
public:
	A3DCBReleaseResRecorder();
	~A3DCBReleaseResRecorder();
	
	void CopyCB2OtherRecorder(A3DCBRecorderBase* pCBRes);
	virtual HRESULT STDMETHODCALLTYPE ReleaseResource(IUnknown* pResource);
	int GetUsedNumDWORD();

protected:
	CRITICAL_SECTION m_CriticalSec;

};

#endif //_A3D_CB_RELEASE_RES_RECORDER_H_