/*
 * FILE: A3DCBReleaseResRecorder.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 3
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */
#include "a3dcbreleaseresrecorder.h"

extern volatile bool _render_reset_flag;
A3DCBReleaseResRecorder::A3DCBReleaseResRecorder()
:A3DCBRecorderBase()
{
	InitializeCriticalSection(&m_CriticalSec);
}

A3DCBReleaseResRecorder::~A3DCBReleaseResRecorder()
{
	DeleteCriticalSection(&m_CriticalSec);
}

void A3DCBReleaseResRecorder::CopyCB2OtherRecorder(A3DCBRecorderBase* pCBRes)
{
	EnterCriticalSection(&m_CriticalSec);

	DWORD* pSrcMem = NULL;
	unsigned int nTmp = 0;
	m_pCB->GetCBMemory(pSrcMem, nTmp);
	int nNumDWORD = m_pCB->GetUsedNumDWORD();

	if( !nNumDWORD )
	{
		LeaveCriticalSection(&m_CriticalSec);
		return;
	}
	if(pCBRes)
	{
		//pCBRes->BeginCommandBuffer();
		m_pCB->ResetNext();
		for( int i = 0; i < nNumDWORD; i++)
		{
			DWORD dwTemp;
			m_pCB->GetDWORD(&dwTemp);

			pCBRes->GetCommandBuffer()->PutDWORD(dwTemp);
		}
	}

	BeginCommandBuffer();
	LeaveCriticalSection(&m_CriticalSec);
}

HRESULT STDMETHODCALLTYPE A3DCBReleaseResRecorder::ReleaseResource(IUnknown* pResource)
{
	assert(!_render_reset_flag);
	EnterCriticalSection(&m_CriticalSec);
	m_pCB->PutDWORD(A3DD3D_COMMANDS::ReleaseResource);
	m_pCB->PutDWORD(pResource);
	LeaveCriticalSection(&m_CriticalSec);
	assert(!_render_reset_flag);
	return D3D_OK;
}

int A3DCBReleaseResRecorder::GetUsedNumDWORD()
{
	return m_pCB->GetUsedNumDWORD();
}
