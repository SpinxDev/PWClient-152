/*
 * FILE: AMVideoClip.cpp
 *
 * DESCRIPTION: The class standing for one video clip in Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/5/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#include "AMVideoClip.h"
#include "AMSurfaceRenderer.h"
#include "AMPI.h"
#include "AMVideoEngine.h"
#include "A3DDevice.h"
#include "A2DSprite.h"
#include "AAssist.h"
#include <streams.h>
#include <asyncio.h>
#include <asyncrdr.h>

///////////////////////////////////////////////////////// AsyncFilter /////////////////////////////////////////////////
class CEncryptedStream : public CAsyncStream
{
public:
    CEncryptedStream() :
        m_llPosition(0)
    {    }

    /*  Initialization */
    void Init(HANDLE hFile, LONGLONG llLength, DWORD dwKBPerSec = INFINITE)
    {
        m_hFile = hFile;
        m_llLength = llLength;
        m_dwKBPerSec = dwKBPerSec;
        m_dwTimeStart = timeGetTime();
    }

    HRESULT SetPointer(LONGLONG llPos)
    {
        if (llPos < 0 || llPos > m_llLength) {
            return S_FALSE;
        } else {
            m_llPosition = llPos;
			SetFilePointer(m_hFile, (LONG)m_llPosition, NULL, FILE_BEGIN);
            return S_OK;
        }
    }

    HRESULT Read(PBYTE pbBuffer,
                 DWORD dwBytesToRead,
                 BOOL bAlign,
                 LPDWORD pdwBytesRead)
    {
		if( dwBytesToRead == 0 )
			return S_FALSE;

        CAutoLock lck(&m_csLock);
        DWORD dwReadLength;

        /*  Wait until the bytes are here! */
        DWORD dwTime = timeGetTime();

        if (m_llPosition + dwBytesToRead > m_llLength) {
            dwReadLength = (DWORD)(m_llLength - m_llPosition);
        } else {
            dwReadLength = dwBytesToRead;
        }

		// if not infinite read rate, we will have to wait for...
		if( m_dwKBPerSec != INFINITE )
		{
			DWORD dwTimeToArrive =
				((DWORD)m_llPosition + dwReadLength) / m_dwKBPerSec;

			if (dwTime - m_dwTimeStart < dwTimeToArrive) {
				Sleep(dwTimeToArrive - dwTime + m_dwTimeStart);
			}
		}

		DWORD dwRead = 0;
		if( 0 == ReadFile(m_hFile, (PVOID)pbBuffer, dwReadLength, &dwRead, NULL) )
			return false;

		// decrypt
		BYTE keys[7] = {0x23, 0xd8, 0x63, 0x59, 0xe3, 0x92, 0xa5};
		DWORD dwKeys[7] = {0x5963d823, 0xe35963d8, 0x92e35963, 0xa592e359, 0x23a592e3, 0xd823a592, 0x63d823a5};

		DWORD dwBuffer = (DWORD)pbBuffer;
		DWORD dwHead = (4 - (dwBuffer & 0x3)) & 0x3;
		if( dwHead > dwRead )
			dwHead = dwRead;

		LONGLONG p = m_llPosition;

		// first decrypt head;
		for(DWORD j=0; j<dwHead; j++)
		{
			BYTE data = pbBuffer[j];
			data = ((data << 5) & 0xe0) | ((data >> 3) & 0x1f);
			pbBuffer[j] = data ^ keys[p % 7];
			p ++;
		}

		// then decrypt body, use DWORD to batch
		if( dwRead > dwHead )
		{
			DWORD dwBody = dwRead - dwHead;
			
			// then decrypt as a DWORD buffer
			DWORD dwLen = dwBody >> 2;
			DWORD* pdwBuffer = (DWORD *)(pbBuffer + dwHead);
			for(DWORD i=0; i<dwLen; i++)
			{
				DWORD data = pdwBuffer[i];
				data = ((data << 5) & 0xe0e0e0e0) | ((data >> 3) & 0x1f1f1f1f);
				pdwBuffer[i] = data ^ dwKeys[p % 7];
				p += 4;
			}

			// last decrypt the tail part
			for(DWORD j=(DWORD)(p-m_llPosition); j<dwRead; j++)
			{
				BYTE data = pbBuffer[j];
				data = ((data << 5) & 0xe0) | ((data >> 3) & 0x1f);
				pbBuffer[j] = data ^ keys[p % 7];
				p ++;
			}
		}
		
        m_llPosition += dwRead;
        *pdwBytesRead = dwRead;
        return S_OK;
    }

    LONGLONG Size(LONGLONG *pSizeAvailable)
    {
        LONGLONG llCurrentAvailable =
            static_cast <LONGLONG> (UInt32x32To64((timeGetTime() - m_dwTimeStart),m_dwKBPerSec));

		if( m_dwKBPerSec == INFINITE )
			llCurrentAvailable = m_llLength;
 
       *pSizeAvailable =  min(m_llLength, llCurrentAvailable);
        return m_llLength;
    }

    DWORD Alignment()
    {
        return 1;
    }

    void Lock()
    {
        m_csLock.Lock();
    }

    void Unlock()
    {
        m_csLock.Unlock();
    }

private:
    CCritSec       m_csLock;
    HANDLE		   m_hFile;
    LONGLONG       m_llLength;
    LONGLONG       m_llPosition;
    DWORD          m_dwKBPerSec;
    DWORD          m_dwTimeStart;
};

class CAsyncFilter : public CAsyncReader, public IFileSourceFilter
{
public:
    CAsyncFilter(LPUNKNOWN pUnk, HRESULT *phr) :
        CAsyncReader(NAME("Encrypted Reader"), pUnk, &m_Stream, phr),
        m_pFileName(NULL),
        m_hFile(NULL)
    {
    }

    ~CAsyncFilter()
    {
		if( m_hFile )
		{
			CloseHandle(m_hFile);
			m_hFile = NULL;
		}

        delete [] m_pFileName;
    }

    static CUnknown *CreateInstance(LPUNKNOWN, HRESULT *);

    DECLARE_IUNKNOWN

	//  We're not going to be CoCreate'd so we don't need registration
    //  stuff etc
    STDMETHODIMP Register()
    {
        return S_OK;
    }
    STDMETHODIMP Unregister()
    {
        return S_OK;
    }

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv)
    {
		if (riid == IID_IFileSourceFilter) {
            return GetInterface((IFileSourceFilter *)this, ppv);
        } else {
            return CAsyncReader::NonDelegatingQueryInterface(riid, ppv);
        }
    }

    /*  IFileSourceFilter methods */

    //  Load a (new) file
    STDMETHODIMP Load(LPCOLESTR lpwszFileName, const AM_MEDIA_TYPE *pmt)
    {
        CheckPointer(lpwszFileName, E_POINTER);

        // lstrlenW is one of the few Unicode functions that works on win95
        int cch = lstrlenW(lpwszFileName) + 1;

#ifndef UNICODE
        TCHAR *lpszFileName=0;
        lpszFileName = new char[cch * 2];
        if (!lpszFileName) {
      	    return E_OUTOFMEMORY;
        }
		ZeroMemory(lpszFileName, cch * 2);
        WideCharToMultiByte(GetACP(), 0, lpwszFileName, -1,
    			lpszFileName, cch, NULL, NULL);
#else
        TCHAR lpszFileName[MAX_PATH]={0};
        lstrcpy(lpszFileName, lpwszFileName);
#endif
        CAutoLock lck(&m_csFilter);

        /*  Check the file type */
        CMediaType cmt;
        if (NULL == pmt) {
            cmt.SetType(&MEDIATYPE_Stream);
            cmt.SetSubtype(&MEDIASUBTYPE_NULL);
        } else {		    
            cmt = *pmt;
        }

        if (!ReadTheFile(lpszFileName)) {
#ifndef UNICODE
            delete [] lpszFileName;
#endif
            return E_FAIL;
        }

#ifndef UNICODE
        delete [] lpszFileName;
#endif
        m_Stream.Init(m_hFile, m_llSize);

        m_pFileName = new WCHAR[cch];

        if (m_pFileName!=NULL)
    	    CopyMemory(m_pFileName, lpwszFileName, cch*sizeof(WCHAR));

        // this is not a simple assignment... pointers and format
        // block (if any) are intelligently copied
    	m_mt = cmt;

        /*  Work out file type */
        cmt.bTemporalCompression = TRUE;	       //???
        cmt.lSampleSize = 1;

        return S_OK;
    }

    // Modeled on IPersistFile::Load
    // Caller needs to CoTaskMemFree or equivalent.

    STDMETHODIMP GetCurFile(LPOLESTR * ppszFileName, AM_MEDIA_TYPE *pmt)
    {
        CheckPointer(ppszFileName, E_POINTER);
        *ppszFileName = NULL;

        if (m_pFileName!=NULL) {
        	DWORD n = sizeof(WCHAR)*(1+lstrlenW(m_pFileName));

            *ppszFileName = (LPOLESTR) CoTaskMemAlloc( n );
            if (*ppszFileName!=NULL) {
                  CopyMemory(*ppszFileName, m_pFileName, n);
            }
        }

        if (pmt!=NULL) {
            CopyMediaType(pmt, &m_mt);
        }

        return NOERROR;
    }

private:
    BOOL CAsyncFilter::ReadTheFile(LPCTSTR lpszFileName)
	{
		/*  Read the file */
		m_hFile = CreateFile(lpszFileName,
								  GENERIC_READ,
								  FILE_SHARE_READ,
								  NULL,
								  OPEN_EXISTING,
								  0,
								  NULL);
		if (m_hFile == INVALID_HANDLE_VALUE) {
			printf("error with code %d\n", GetLastError());
			return FALSE;
		}

		ULARGE_INTEGER uliSize;
		uliSize.LowPart = GetFileSize(m_hFile, &uliSize.HighPart);

		m_llSize = (LONGLONG)uliSize.QuadPart;
		
		return TRUE;
	}

private:
    LPWSTR     m_pFileName;
    LONGLONG   m_llSize;
    
	HANDLE				m_hFile;
    CEncryptedStream	m_Stream;
};

////////////////////////////////////////////////////////////// AsyncFilter ///////////////////////////////////////////////////

bool AMVideoClip::BuildGraphForMPX(IGraphBuilder * pGraphBuilder, LPCWSTR lpcwstrFile)
{
	HRESULT hr;
	IFilterGraph * pFilterGraph;
	hr = pGraphBuilder->QueryInterface(IID_IFilterGraph, (void **)&pFilterGraph);
    if (FAILED(hr))
	{
		AMERRLOG(("BuildGraphForMPX(), failed to get filter graph."));
        return false;
    }

	CAsyncFilter * pSrcFilter = new CAsyncFilter(NULL, &hr);
	if (FAILED(hr))
	{
		AMERRLOG(("BuildGraphForMPX(), failed to create async filter."));
		delete pSrcFilter;
		pFilterGraph->Release();
		return false;
	}

	hr = pSrcFilter->Load(lpcwstrFile, NULL);
	if (FAILED(hr))
	{
		AMERRLOG(("BuildGraphForMPX(), failed to load src."));
		delete pSrcFilter;
		pFilterGraph->Release();
		return false;
	}

	//  Make sure we don't accidentally go away!
    pSrcFilter->AddRef();

	/*  Add our filter */
    hr = pFilterGraph->AddFilter(pSrcFilter, NULL);
    if (FAILED(hr))
	{
		AMERRLOG(("BuildGraphForMPX(), failed to add src."));
		delete pSrcFilter;
		pFilterGraph->Release();
        return false;
    }

    /*  Render our output pin */
    hr = pGraphBuilder->Render(pSrcFilter->GetPin(0));
	if (FAILED(hr))
	{
		AMERRLOG(("BuildGraphForMPX(), failed to render left."));
		delete pSrcFilter;
		pFilterGraph->Release();
        return false;
    }

	// Done.
	pSrcFilter->Release();
	pFilterGraph->Release();
    return true;
}

// -- Begin Added by yaochunhui 2011/12/7 --

// Create the source filter for MPX and add it to filter graph
bool AMVideoClip::AddSourceFilterForMPX(IGraphBuilder * pGraphBuilder, LPCWSTR lpcwstrFile, IBaseFilter** ppFilter)
{
	HRESULT hr;
	IFilterGraph * pFilterGraph;
	hr = pGraphBuilder->QueryInterface(IID_IFilterGraph, (void **)&pFilterGraph);
    if (FAILED(hr))
	{
		AMERRLOG(("AddSourceFilterForMPX(), failed to get filter graph."));
        return false;
    }
	
	CAsyncFilter * pSrcFilter = new CAsyncFilter(NULL, &hr);
	if (FAILED(hr))
	{
		AMERRLOG(("AddSourceFilterForMPX(), failed to create async filter."));
		delete pSrcFilter;
		pFilterGraph->Release();
		return false;
	}
	
	hr = pSrcFilter->Load(lpcwstrFile, NULL);
	if (FAILED(hr))
	{
		AMERRLOG(("AddSourceFilterForMPX(), failed to load src."));
		delete pSrcFilter;
		pFilterGraph->Release();
		return false;
	}
	
	//  Make sure we don't accidentally go away!
    pSrcFilter->AddRef();
	
    hr = pFilterGraph->AddFilter(pSrcFilter, L"MPX File Source");
    if (FAILED(hr))
	{
		AMERRLOG(("AddSourceFilterForMPX(), failed to add src."));
		delete pSrcFilter;
		pFilterGraph->Release();
        return false;
    }
	
	pFilterGraph->Release();
	hr = pSrcFilter->QueryInterface(IID_IBaseFilter, (void**)ppFilter);
    if (FAILED(hr))
	{
		AMERRLOG(("AddSourceFilterForMPX(), failed to query IBaseFilter interface."));
		delete pSrcFilter;
		pFilterGraph->Release();
        return false;
    }
    m_pMPXSourceFilter = pSrcFilter;
	return true;
}

// Build a source filter according to file type
bool AMVideoClip::BuildSourceFilter(IGraphBuilder * pGraphBuilder, const char * szVideoFile, IBaseFilter** ppFilter)
{
	WCHAR wFileName[MAX_PATH] = {0};
    a_MultiByteToWideChar(szVideoFile, -1, wFileName, MAX_PATH);
	HRESULT hr;
	if (strstr(szVideoFile, ".mpx"))
	{
		// encrypted mpg file detected; use our own source filter now
		if( !AddSourceFilterForMPX(pGraphBuilder, wFileName, ppFilter) )
		{
			AMERRLOG(("BuildSourceFilter(), Could not add source filter for mpx!"));
			return false;
		}
	}
	else
	{
		// Auto get the source filter
		if (FAILED(hr = pGraphBuilder->AddSourceFilter(wFileName, L"FileSource", ppFilter)))
		{
			AMERRLOG(("BuildSourceFilter(), Could not add source filter for %s ! hr=0x%x", szVideoFile, hr));
			return false;
		}
	}

	return true;
}

IPin* AMVideoClip::GetPinByDirAndIndex(IBaseFilter* pFilter, int index, PIN_DIRECTION dir)
{
	IEnumPins  *pEnumPins;
	IPin *pPin;
	IPin *pOutPin = NULL;	
	pFilter->EnumPins(&pEnumPins);
	int nCount = 0;
	while( pEnumPins->Next(1, &pPin, 0) == S_OK && nCount <= index)
	{
		PIN_DIRECTION dirPin;
		pPin->QueryDirection(&dirPin);
		if (dir == dirPin)
		{
			if (nCount == index)
			{
				pOutPin = pPin;
				break;
			}
			nCount++;
		}
		pPin->Release();
	}
	pEnumPins->Release();
	return pOutPin;
}

// Connect two filters
bool AMVideoClip::ConnectFilterPin(IGraphBuilder * pGraphBuilder, IBaseFilter* pFilterInput, int iInputPinIndex, 
					  IBaseFilter* pFilterOutput, int iOutputPinIndex)
{
	HRESULT hr;

	// get input pin
	IPin* pInputPin = GetPinByDirAndIndex(pFilterInput, iInputPinIndex, PINDIR_INPUT);
	if (pInputPin == NULL)
	{
		AMERRLOG(("ConnectFilterPin(), Could not get input pin"));
		return false;
	}
	// get output pin
	IPin* pOutputPin = GetPinByDirAndIndex(pFilterOutput, iOutputPinIndex, PINDIR_OUTPUT);
	if (pOutputPin == NULL)
	{
		pInputPin->Release();
		AMERRLOG(("ConnectFilterPin(), Could not get output pin"));
		return false;
	}
	// connect them
	if (FAILED(hr = pGraphBuilder->Connect(pOutputPin, pInputPin)))
	{
		pInputPin->Release();
		pOutputPin->Release();
		AMERRLOG(("BuildSourceFilter(), Could not connect pins! hr=0x%x", hr));
		return false;
	}
	// ok
	pInputPin->Release();
	pOutputPin->Release();
	return true;
}
// -- End Added by yaochunhui 2011/12/7 --


//////////////////////////////////////////////////////////////// AMVideoClip ////////////////////////////////////////////////
AMVideoClip::AMVideoClip()
{
	m_pA3DDevice		= NULL;
	m_pAMVideoEngine	= NULL;
	m_pSurfaceRenderer	= NULL;
	m_pScreenSprite		= NULL;

	m_state				= AMVIDEOCLIP_STATE_STOPPED;

	m_pGraphBuilder		= NULL;
	m_pMediaControl		= NULL;
	m_pMediaEvent		= NULL;
	m_pMediaPosition	= NULL;
	m_pMediaAudio		= NULL;

	m_bHasSound = false;
	m_bLoop = false;
	m_dLoopStartPos = 0.0;
	m_duration = 0.0;
	m_dwVolume			= 100;
    m_pMPXSourceFilter  = NULL;
    InitializeCriticalSection(&m_SurfaceAccess);
}

AMVideoClip::~AMVideoClip()
{
	DeleteCriticalSection(&m_SurfaceAccess);
}

DWORD dwROTReg = 0xfedcba98;

HRESULT AddToROT(IUnknown *pUnkGraph) 
{
	return 0;
    IMoniker * pmk;
    IRunningObjectTable *pirot;
    if (FAILED(GetRunningObjectTable(0, &pirot))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
    if (SUCCEEDED(hr)) {
        hr = pirot->Register(0, pUnkGraph, pmk, &dwROTReg);
        pmk->Release();
    }
    pirot->Release();
    return hr;
}

bool AMVideoClip::Init(A3DDevice * pA3DDevice, AMVideoEngine * pAMVideoEngine, const char * szVideoFile, bool bNoSound)
{
    HRESULT hr = S_OK;

    IPin *		            pSRPinIn;			// Surface Renderer Input Pin
	IPin *					pVRPinIn;			// Default video renderer's input pin
	IPin *					pVRConnectedPin;	// Connected pin with default video renderer's input pin
	m_pA3DDevice			= pA3DDevice;
	m_pAMVideoEngine		= pAMVideoEngine;

    // Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                    IID_IGraphBuilder, (void **)&m_pGraphBuilder);
	if( hr != S_OK )
	{
		AMERRLOG(("AMVideoClip::Init(), Could not create graph builder object!  hr=0x%x", hr));
		return false;
	}

#ifdef _DEBUG
    // Register the graph in the Running Object Table (for debug purposes)
    AddToROT(m_pGraphBuilder);
#endif
    
	strncpy(m_szClipName, szVideoFile, MAX_PATH);

    WCHAR wFileName[MAX_PATH] = {0};
    a_MultiByteToWideChar(szVideoFile, -1, wFileName, MAX_PATH);

	if( strstr(szVideoFile, ".mpx") )
	{
		// encrypted mpg file detected; use our own source filter now
		if( !BuildGraphForMPX(m_pGraphBuilder, wFileName) )
		{
			AMERRLOG(("AMVideoClip::Init(), Could not build filter graph for mpx!"));
			return false;
		}
	}
	else
	{
		// Auto build the whole filter graph;
		if (FAILED(hr = m_pGraphBuilder->RenderFile(wFileName, NULL)))
		{
			AMERRLOG(("AMVideoClip::Init(), Could not build filter graph! hr=0x%x", hr));
			return false;
		}
	}

	// Enumerate filters sample
	/* 
	IEnumFilters *		pEnum = NULL;
    IBaseFilter *		pFilter;
    ULONG cFetched;
    m_pGraphBuilder->EnumFilters(&pEnum);
    while( pEnum->Next(1, &pFilter, &cFetched) == S_OK )
    {
        FILTER_INFO FilterInfo;
        char szName[256];
        
        pFilter->QueryFilterInfo(&FilterInfo);
        WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
        MessageBox(NULL, szName, "Filter name", MB_OK);

        FilterInfo.pGraph->Release();
        pFilter->Release();
    }
    pEnum->Release();
	*/

	// Now find the video renderer filter;
	IBaseFilter *		pVRFilter;
	if (FAILED(hr = m_pGraphBuilder->FindFilterByName(L"Video Renderer", &pVRFilter)))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not create find the video renderer filter!  hr=0x%x", hr));
        return false;
	}

	// Find the default video renderer's input pin; in fact you can use FindPin instead
	IEnumPins  *pEnumPins;
    IPin       *pPin;
    pVRFilter->EnumPins(&pEnumPins);
    while( pEnumPins->Next(1, &pPin, 0) == S_OK )
    {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
		if( PinDirThis == PINDIR_INPUT )
		{
			/*
			LPWSTR id;
			pPin->QueryId(&id);
			char szID[256];
			WideCharToMultiByte(CP_ACP, 0, id, -1, szID, 256, 0, 0);
			*/
			break;
		}
        pPin->Release();
    }
    pEnumPins->Release();
	pVRPinIn = pPin;

	// Find the connected pin of default video renderer's input pin;
	if (FAILED(hr = pVRPinIn->ConnectedTo(&pVRConnectedPin)))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not get connected pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	// If this addfilter is called before renderfile, 
	// the default video renderer will not be used!!!
	m_pSurfaceRenderer = new AMSurfaceRenderer(NULL, &hr);
    if (FAILED(hr))                                      
    {
        AMERRLOG(("AMVideoClip::Init(), Could not create AMSurfaceRenderer object! hr=0x%x", hr));
		return false;
    }

    if (FAILED(hr = m_pGraphBuilder->AddFilter(m_pSurfaceRenderer, L"SURFACERENDERER")))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not add renderer filter to graph!  hr=0x%x", hr));
        return false;
    }
    
	// Find the surface renderer's input pin
    if (FAILED(hr = m_pSurfaceRenderer->FindPin(L"In", &pSRPinIn)))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not find input pin!  hr=0x%x", hr));
        return false;
    }

    if (FAILED(hr = m_pGraphBuilder->Disconnect(pVRPinIn)))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not disconnect pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

	if (FAILED(hr = m_pGraphBuilder->Disconnect(pVRConnectedPin)))
	{
		AMERRLOG(("AMVideoClip::Init(), Could not disconnect connected pin of VRPinIn!  hr=0x%x", hr));
        return false;
	}

    // Connect these two filters, the pin connected with VR, and the input pin of texture renderer filter;
    if (FAILED(hr = m_pGraphBuilder->Connect(pVRConnectedPin, pSRPinIn)))
    {
        AMERRLOG(("AMVideoClip::Init(), Could not connect dsound renderer!  hr=0x%x", hr));
		return false;
    }

	// Get rid of unneccessary interface;
	pSRPinIn->Release();
	pVRPinIn->Release();
	pVRConnectedPin->Release();
	pVRFilter->Release();
	m_pGraphBuilder->RemoveFilter(pVRFilter);

	if (!InitCommon())
		return false;

	// see if we need to get rid of the sound stream
	if( bNoSound )
	{
		// Now find the video renderer filter;
		IBaseFilter *		pSoundFilter;
		if (SUCCEEDED(hr = m_pGraphBuilder->FindFilterByName(L"Default DirectSound Device", &pSoundFilter)))
		{
			pSoundFilter->Release();
			m_pGraphBuilder->RemoveFilter(pSoundFilter);
		}
	}

	m_state = AMVIDEOCLIP_STATE_STOPPED;
	
	SetVolume(m_dwVolume);

	return true;
}

// Added by yaochunhui 2011/12/7
bool AMVideoClip::InitSafe(A3DDevice * pA3DDevice, AMVideoEngine * pAMVideoEngine, const char * szVideoFile, bool bNoSound)
{

	if (!strstr(szVideoFile, ".mpx") && !strstr(szVideoFile, ".mpg"))
	{
		AMERRLOG(("AMVideoClip::InitSafe(), file type not supported: %s", szVideoFile));
	}

    HRESULT hr = S_OK;

	m_pA3DDevice			= pA3DDevice;
	m_pAMVideoEngine		= pAMVideoEngine;
	
	// Filters for build graph
	IBaseFilter* pSourceFilter = NULL;
	IBaseFilter* pSplitterFilter = NULL;
	IBaseFilter* pVideoDecoderFilter = NULL;
	IBaseFilter* pAudioDecoderFilter = NULL;
	IBaseFilter* pDirectSoundFilter = NULL;

    // Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                    IID_IGraphBuilder, (void **)&m_pGraphBuilder);
	if( hr != S_OK )
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not create graph builder object!  hr=0x%x", hr));
		return false;
	}

#ifdef _DEBUG
    // Register the graph in the Running Object Table (for debug purposes)
    AddToROT(m_pGraphBuilder);
#endif

	strncpy(m_szClipName, szVideoFile, MAX_PATH);
	
    WCHAR wFileName[MAX_PATH] = {0};
    a_MultiByteToWideChar(szVideoFile, -1, wFileName, MAX_PATH);

	// Build Filter Graph manually
	
	// Add source filter
	if (!BuildSourceFilter(m_pGraphBuilder, szVideoFile, &pSourceFilter))
		return false;

	// Add MEPG-I Stream Splitter
	hr = CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER, 
		IID_IBaseFilter, (void **)&pSplitterFilter);
	if( hr != S_OK )
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not create MEPG-I Stream Splitter object!  hr=0x%x", hr));
		return false;
	}

	if (FAILED(hr = m_pGraphBuilder->AddFilter(pSplitterFilter, L"MEPG-I Stream Splitter")))
    {
        AMERRLOG(("AMVideoClip::InitSafe(), Could not add MEPG-I Stream Splitter filter to graph!  hr=0x%x", hr));
        return false;
    }
	
	// Connect source - splitter
	if (!ConnectFilterPin(m_pGraphBuilder, pSplitterFilter, 0, pSourceFilter, 0))
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not connect source and splitter!"));
		return false;
	}

	// Add MEPG Video Decoder
	hr = CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER, 
		IID_IBaseFilter, (void **)&pVideoDecoderFilter);
	if( hr != S_OK )
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not create MEPG Video Decoder object!  hr=0x%x", hr));
		return false;
	}
	if (FAILED(hr = m_pGraphBuilder->AddFilter(pVideoDecoderFilter, L"MEPG Video Decoder")))
    {
        AMERRLOG(("AMVideoClip::InitSafe(), Could not add MEPG Video Decoder filter to graph!  hr=0x%x", hr));
        return false;
    }

	// Connect splitter - video decoder
	if (!ConnectFilterPin(m_pGraphBuilder, pVideoDecoderFilter, 0, pSplitterFilter, 0))
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not connect splitter and video decoder!"));
		return false;
	}

	// Add Surface Renderer filter
	m_pSurfaceRenderer = new AMSurfaceRenderer(NULL, &hr);
    if (FAILED(hr))                                      
    {
        AMERRLOG(("AMVideoClip::InitSafe(), Could not create AMSurfaceRenderer object! hr=0x%x", hr));
		return false;
    }

    if (FAILED(hr = m_pGraphBuilder->AddFilter(m_pSurfaceRenderer, L"SurfaceRenderer")))
    {
        AMERRLOG(("AMVideoClip::InitSafe(), Could not add renderer filter to graph!  hr=0x%x", hr));
        return false;
    }
    
	// Connect video decoder - renderer
	if (!ConnectFilterPin(m_pGraphBuilder, m_pSurfaceRenderer, 0, pVideoDecoderFilter, 0))
	{
		AMERRLOG(("AMVideoClip::InitSafe(), Could not connect video decoder and renderer!"));
		return false;
	}

	if (!bNoSound)
	{
		// Add MEPG Audio Decoder
		hr = CoCreateInstance(CLSID_CMpegAudioCodec, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void **)&pAudioDecoderFilter);
		if( hr != S_OK )
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not create MEPG Audio Decoder object!  hr=0x%x", hr));
			return false;
		}
		if (FAILED(hr = m_pGraphBuilder->AddFilter(pAudioDecoderFilter, L"MEPG Audio Decoder")))
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not add MEPG Audio Decoder filter to graph!  hr=0x%x", hr));
			return false;
		}
		
		// Connect splitter - audio decoder
		if (!ConnectFilterPin(m_pGraphBuilder, pAudioDecoderFilter, 0, pSplitterFilter, 1))
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not connect splitter and audio decoder!"));
			return false;
		}

		// Add DirectSounder Renderer
		hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void **)&pDirectSoundFilter);
		if( hr != S_OK )
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not create DirectSound object!  hr=0x%x", hr));
			return false;
		}
		if (FAILED(hr = m_pGraphBuilder->AddFilter(pDirectSoundFilter, L"DirectSound")))
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not add DirectSound filter to graph!  hr=0x%x", hr));
			return false;
		}

		// Connect audio decoder - DirectSound
		if (!ConnectFilterPin(m_pGraphBuilder, pDirectSoundFilter, 0, pAudioDecoderFilter, 0))
		{
			AMERRLOG(("AMVideoClip::InitSafe(), Could not connect audio decoder and DirectSound!"));
			return false;
		}

		pAudioDecoderFilter->Release();
		pDirectSoundFilter->Release();
	}

	pSourceFilter->Release();
	pSplitterFilter->Release();
	pVideoDecoderFilter->Release();

	if (!InitCommon())
		return false;

	return true;
}

bool AMVideoClip::InitCommon()
{
	// Get the graph's media control, event & position interfaces
	m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (void **)&m_pMediaEvent);
	m_pGraphBuilder->QueryInterface(IID_IMediaPosition, (void **)&m_pMediaPosition);

	HRESULT hr;
	// Get the graph's basic audio interface (for volume, balance)
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&m_pMediaAudio)))
	{
		AMERRLOG(("AMVideoClip::InitCommon(), Could not get BasicAudio interface! hr=0x%x", hr));
	}
	
	hr = m_pMediaPosition->get_Duration(&m_duration);
	if (FAILED(hr))
	{
		AMERRLOG(("AMVideoClip::InitCommon(), Could not get duration through media position interface!  hr=0x%x", hr));
	}
	
	m_nWidth = m_pSurfaceRenderer->GetVidWidth();
	m_nHeight = m_pSurfaceRenderer->GetVidHeight();
	m_VideoFormat = m_pSurfaceRenderer->GetVidFormat();
	
	
	// Now we must create a surface that will be the destination surface for rendering
	A3DRECT rectVideo;
	rectVideo.left		= 0; 
	rectVideo.top		= 0;  
	rectVideo.right		= m_nWidth;
	rectVideo.bottom	= m_nHeight;
	
	m_pScreenSprite = new A2DSprite();
	if( NULL == m_pScreenSprite )
	{
		AMERRLOG(("AMVideoClip::Init(), Not enough memory!"));
		return false;
	}
	
	if( !m_pScreenSprite->InitWithoutSurface(m_pA3DDevice, m_nWidth, m_nHeight, m_VideoFormat, 1, &rectVideo) )
	{
		AMERRLOG(("AMVideoClip::Init(), Can not init a sprite from the DestSurface!"));
		return false;
	}
	
	m_state = AMVIDEOCLIP_STATE_STOPPED;
	
	SetVolume(m_dwVolume);
	
	return true;
}

bool AMVideoClip::Release()
{
	Stop();

	if( m_pScreenSprite )
	{
		m_pScreenSprite->Release();
		delete m_pScreenSprite;
		m_pScreenSprite = NULL;
	}

	if (m_pMediaAudio)
	{
		m_pMediaAudio->Release();
		m_pMediaAudio = NULL;
	}

	if( m_pMediaPosition )
	{
		m_pMediaPosition->Release();
		m_pMediaPosition = NULL;
	}
	if( m_pMediaEvent )
	{
		m_pMediaEvent->Release();
		m_pMediaEvent = NULL;
	}
	if( m_pMediaControl )
	{
		m_pMediaControl->Release();
		m_pMediaControl = NULL;
	}
	if( m_pGraphBuilder )
	{
		m_pGraphBuilder->Release();
		m_pGraphBuilder = NULL;
	}

    if (m_pMPXSourceFilter)
    {
        delete m_pMPXSourceFilter;
        m_pMPXSourceFilter = NULL;
    }

	m_pAMVideoEngine = NULL;
	return true;
}

bool AMVideoClip::Start(bool bLoop)
{
	m_bLoop = bLoop;

	// Now let the surface renderer aim at out dest surface;
	m_pSurfaceRenderer->SetDestParameter(m_pScreenSprite, &m_SurfaceAccess);

	if (FAILED(m_pMediaControl->Run()))
	{
		AMERRLOG(("AMVideoClip::Start(), Can IMediaControl::Run() fail!"));
		return false;
	}

	m_state = AMVIDEOCLIP_STATE_PLAYING;
	return true;
}

bool AMVideoClip::Pause()
{
	if (FAILED(m_pMediaControl->Pause()))
	{
		AMERRLOG(("AMVideoClip::Pause(), Can IMediaControl::Pause() fail!"));
		return false;
	}

	m_state = AMVIDEOCLIP_STATE_PAUSED;
	return true;
}

bool AMVideoClip::Stop()
{
	if( m_pMediaControl )
	{
		if (FAILED(m_pMediaControl->Stop()))
		{
			AMERRLOG(("AMVideoClip::Stop(), Can IMediaControl::Stop() fail!"));
			return false;
		}
	}

	if( m_pSurfaceRenderer )
		m_pSurfaceRenderer->SetDestParameter(NULL, NULL);

	m_state = AMVIDEOCLIP_STATE_STOPPED;
	return true;
}

bool AMVideoClip::Tick()
{
	long evCode, param1, param2;
	HRESULT hr;

	if( m_pMediaEvent == NULL )
		return true;

	while( hr = m_pMediaEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr) )
	{
		if( EC_COMPLETE == evCode )
		{ 
			if( m_bLoop )
			{
				hr = m_pMediaPosition->put_CurrentPosition(m_dLoopStartPos);
				goto NEXT;
			}

			Stop();
			break;
		} 
		else if( EC_USERABORT == evCode )
		{
			// We will not handle this, because the user can not stop the
			// video playback directly;
		}

NEXT:
		hr = m_pMediaEvent->FreeEventParams(evCode, param1, param2);
	}
	return true;
}

bool AMVideoClip::DrawToBack()
{
	if( m_pScreenSprite )
	{
		EnterCriticalSection(&m_SurfaceAccess);
		m_pScreenSprite->DrawToBack();
		LeaveCriticalSection(&m_SurfaceAccess);
		return true;
	}
	return true;
}

bool AMVideoClip::SetAutoScaleFlags(DWORD dwFlags)
{
	if ( !m_pA3DDevice )
		return false;

	if( (dwFlags & AMVID_FIT_X) || (dwFlags & AMVID_FIT_Y) )
	{
		int nNewWidth = m_nWidth, nNewHeight = m_nHeight;
		
		if( dwFlags & AMVID_FIT_X )
		{
			nNewWidth = m_pA3DDevice->GetDevFormat().nWidth;
		}
		if( dwFlags & AMVID_FIT_Y )
		{
			nNewHeight = m_pA3DDevice->GetDevFormat().nHeight;
		}

		FLOAT vScaleX = nNewWidth * 1.0f / m_nWidth;
		FLOAT vScaleY = nNewHeight * 1.0f / m_nHeight;
		// Try to keep the contraint and center the frame;
		if( (dwFlags & AMVID_FIT_X ) && (dwFlags & AMVID_FIT_Y) &&
			(dwFlags & AMVID_FIT_KEEPCONSTRAINT) )
		{
			if( fabs(vScaleX - vScaleY) > 1e-3 )
			{
				A3DPOINT2 ptPos;
				ptPos.x = ptPos.y = 0;

				// We have to use the smaller scale factor;
				if( vScaleX > vScaleY )
				{
					vScaleX = vScaleY;
					ptPos.x = int((nNewWidth - m_nWidth * vScaleX) / 2);
				}
				else
				{
					vScaleY = vScaleX;
					ptPos.y = int((nNewHeight - m_nHeight * vScaleY) / 2);
				}
				SetVidPos(ptPos);
			}
		}
		SetScaleFactors(vScaleX, vScaleY);
	}
	else
	{

	}
	return true;
}

bool AMVideoClip::SetScaleFactors(FLOAT vScaleX, FLOAT vScaleY)
{
	m_vScaleX = vScaleX;
	m_vScaleY = vScaleY;
	
	if( m_pScreenSprite )
	{
		m_pScreenSprite->SetScaleX(m_vScaleX);
		m_pScreenSprite->SetScaleY(m_vScaleY);

		if( (m_vScaleX > 1.0f && fabs(m_vScaleX - 1.0f) > 1e-3f) ||
			(m_vScaleY > 1.0f && fabs(m_vScaleY - 1.0f) > 1e-3) )
		{
			m_pScreenSprite->SetLinearFilter(true);
		}
		else
		{
			m_pScreenSprite->SetLinearFilter(false);
		}

	}
	return true;
}

bool AMVideoClip::SetColor(A3DCOLOR color)
{
	m_color = color;

	if( m_pScreenSprite )
		m_pScreenSprite->SetColor(m_color);
	return true;
}

bool AMVideoClip::SetVidPos(A3DPOINT2 ptVidPos)
{
	m_ptVidPos = ptVidPos;

	if( m_pScreenSprite )
		m_pScreenSprite->SetPosition(m_ptVidPos.x, m_ptVidPos.y);
	return true;
}

bool AMVideoClip::SetBlendMode(A3DBLEND srcBlend, A3DBLEND destBlend)
{
	if( m_pScreenSprite )
	{
		m_pScreenSprite->SetSrcBlend(srcBlend);
		m_pScreenSprite->SetDestBlend(destBlend);
	}
	return true;
}

void AMVideoClip::SetVolume(DWORD dwVolume)
{
	a_Clamp(dwVolume, 0ul, 100ul);
	m_dwVolume = dwVolume;

	if ( !m_pMediaAudio )
		return;

	HRESULT hr = S_OK;
	if( m_dwVolume == 0 )
		m_pMediaAudio->put_Volume(DSBVOLUME_MIN);
	else
		m_pMediaAudio->put_Volume(AMSOUND_MINVOLUME + (AMSOUND_MAXVOLUME - AMSOUND_MINVOLUME) * m_dwVolume / 100);
}