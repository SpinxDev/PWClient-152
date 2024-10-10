#ifndef _A3DFTFONTMAN_H_
#define _A3DFTFONTMAN_H_

#include "Vector.h"
#include "A3DMacros.h"
#include <hashmap.h>

class A3DDevice;
class IFTManager;
class A3DFTFont;
class A3DVertexShader;
class A3DPixelShader;
class A3DStream;

typedef void (*A3DFTFONT_MULTI_THREAD_LOAD_FUNC)(A3DFTFont* pFont);

typedef abase::vector<A3DFTFont*> FTFontArray;

struct A3DFTFONT_DATA_BUFFER
{
	char* buffer;
	char path[MAX_PATH];
	int length;
};

typedef abase::hash_map<AWString, A3DFTFONT_DATA_BUFFER> A3DFTFontDataBufferMap;

class A3DFTFontMan
{
public:

	A3DFTFontMan();
	~A3DFTFontMan() { ::DeleteCriticalSection(&m_cs); }

protected:

	A3DDevice*		m_pDevice;
	IFTManager*		m_pFTFontMan;
	A3DPixelShader*	m_pShadowShader;
	A3DVertexShader*m_pStereoShader;
	A3DVertexShader*m_pStereoShadowShader;
	A3DStream*		m_pFrontStream;
	A3DStream*		m_pShadowStream;
	int				m_nCurPosInFrontStream;
	int				m_nCurPosInShadowStream;

	FTFontArray		m_FontBuf;
	FTFontArray		m_RefCntFontBuf;

	A3DFTFont*		m_pDefaultFont;
	CRITICAL_SECTION	m_cs;

	A3DFTFONT_MULTI_THREAD_LOAD_FUNC	m_pMultiThreadLoadFunc;
	DWORD			m_dwMainThreadId;

	DWORD			m_dwMaxNewCharPerFrame;

	A3DFTFontDataBufferMap	m_DataBufferMap;
	abase::vector<char*>	m_DataBufferPtrVec;

	friend class A3DFTFont;

protected:

	void InitFaceNameMap();
	bool LoadConfig(const char* szConfigFile);
	A3DStream* CreateStream(DWORD dwVertType, int iVertSize);
	void ReleaseFont(A3DFTFont* pFTFont);

	void RenderStream(
		int iVertSize,
		int iNumRect,
		const void* pSrc,
		bool bShadow,
		bool bStereo);

	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }

public:

	bool Create(A3DDevice* pDevice, const char* szConfigFile = "Configs\\font.ini");
	void Release();
	void Flush();

	void SetMultiThreadLoadFunc(A3DFTFONT_MULTI_THREAD_LOAD_FUNC pLoadFunc)
	{
		m_pMultiThreadLoadFunc = pLoadFunc;
	}

	void SetMainThreadId(DWORD id) { m_dwMainThreadId = id; }

	A3DFTFont* CreatePointFont(
		float fPointSize,
		const wchar_t* wszFaceName,
		int nStyle,
		int nCreateMode = 0,
		int nOutlineWidth = 0,
		A3DCOLOR clOutline = 0,
		A3DCOLOR clInnerText = A3DCOLORRGB(255, 255, 255),
		bool bInterpColor = false,
		A3DCOLOR clUpper = 0,
		A3DCOLOR clLower = 0,
		bool bStereo = false,
		bool bNoCache = false,
		bool bLoadInThread = false
		);
	A3DFTFont* CreatePointFont(const float fPointSize, const A3DFTFont* pTmplFont);

	void ReleasePointFont(A3DFTFont* pFTFont);
	void CreateFontInThread(A3DFTFont* pFont);
	void SetMaxNewCharPerFrame(DWORD dwCount) { m_dwMaxNewCharPerFrame = dwCount; }

	const A3DFTFONT_DATA_BUFFER* GetFontDataBuffer(const wchar_t* wszFace)
	{
		A3DFTFontDataBufferMap::iterator it = m_DataBufferMap.find(wszFace);
		return it == m_DataBufferMap.end() ? 0 : &it->second;
	}

	void CreateFontDataBuffer(const wchar_t* wszFace, const char* szPath);
};

#endif
