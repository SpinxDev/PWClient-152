#ifndef _A3DFTFONT_H_
#define _A3DFTFONT_H_

#include "vector.h"
#include "hashmap.h"
#include "A3DTexture.h"
#include "A3DVertex.h"
#include "A3DStream.h"
#include "A3DTypes.h"
#include "A3DMacros.h"
#include "FTInterface.h"
#include "AWString.h"

#define FVF_TLVERTEX_FONT_SHADOW  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX5)

struct TLVERTEX_FONT_SHADOW
{
	FLOAT x, y, z, rhw;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu1, tv1;
	FLOAT tu2, tv2;
	FLOAT tu3, tv3;
	FLOAT tu4, tv4;
	FLOAT tu5, tv5;
};

typedef abase::vector<A3DTLVERTEX> FontVertBuffArr;
typedef abase::vector<TLVERTEX_FONT_SHADOW> FontShadowVertBufArr;

struct CHAR_TEX_INFO
{
	wchar_t	ch;
	int		x;
	int		y;
	DWORD	tl;
};

typedef abase::vector<CHAR_TEX_INFO> CharTexInfoArr;

class A3DFTFontMan;
class IFTFont;
class AFileImage;

class A3DFTFont
{
public:

	//	Texture slot
	struct TEXTURESLOT
	{
		A3DTexture*				pTexture;
		CharTexInfoArr			CharInfo;
		FontVertBuffArr			VertBuf;
		FontShadowVertBufArr	ShadowBuf;
		DWORD					FlushCount;
	};

	//	Character properties
	struct CHARPROP
	{
		wchar_t			m_char;
		TEXTURESLOT*	m_pSlot;
		CHAR_TEX_INFO*	m_pTexInfo;
		float			tu1;
		float			tv1;
		float			tu2;
		float			tv2;
		FTDSIZE			offset;
		FTDSIZE			size;
		FTDSIZE			bmp_size;
	};

	typedef abase::hash_map<wchar_t, CHARPROP*> CharMap;
	typedef abase::vector<TEXTURESLOT*> TexSlotArr;

protected:

	A3DFTFont(A3DFTFontMan* pFontMan);
	~A3DFTFont() { ::DeleteCriticalSection(&m_cs); }

protected:

	CharMap			m_CharMap;
	IFTFont*		m_pFTFont;
	A3DFTFontMan*	m_pFontMan;
	int				m_iFontWid;
	int				m_iFontHei;
	TexSlotArr		m_aSlotPool;
	int				m_iPointSize;
	int				m_iOutline;
	DWORD			m_clOutline;
	DWORD			m_clInner;
	bool			m_bInterpColor;
	BYTE			m_clUpper[3];
	BYTE			m_clLower[3];
	A3DCOLOR		m_clOutlineReserve;
	A3DCOLOR		m_clInnerReserve;
	A3DCOLOR		m_clUpperReserve;
	A3DCOLOR		m_clLowerReserve;
	A3DHSVCOLORVALUE	m_clHSVUpperReserve;
	A3DHSVCOLORVALUE	m_clHSVLowerReserve;
	int				m_iStyle;
	char			m_szFontPath[MAX_PATH];
	int				m_nCreateMode;
	AWString		m_strFaceName;
	bool			m_bStereo;
	A3DEngine*		m_pA3DEngine;

	unsigned int	m_nRefCnt;

	CRITICAL_SECTION m_cs;
	volatile bool	m_bLoaded;
	volatile bool	m_bInLoadThread;
	DWORD			m_dwCurNewCharCount;

protected:

	friend class A3DFTFontMan;

	bool Create(
		int nPointSize,
		const char* szFontPath,
		int nStyle,
		int nOutlineWidth,
		A3DCOLOR clOutline,
		A3DCOLOR clInnerText,
		bool bInterpColor,
		A3DCOLOR clUpper,
		A3DCOLOR clLower,
		int nCreateMode,
		const wchar_t* strFaceName,
		bool bStereo,
		A3DEngine* pA3DEngine );// For underline

	void SetParam(
		int nPointSize,
		const char* szFontPath,
		int nStyle,
		int nOutlineWidth,
		A3DCOLOR clOutline,
		A3DCOLOR clInnerText,
		bool bInterpColor,
		A3DCOLOR clUpper,
		A3DCOLOR clLower,
		int nCreateMode,
		const wchar_t* strFaceName,
		bool bStereo);

	void Release();
	void ResetCharMap()
	{
		CharMap::iterator it = m_CharMap.begin();

		for (; it != m_CharMap.end(); ++it)
			delete it->second;

		m_CharMap.clear();
	}
	void ResetCharProp(wchar_t ch)
	{
		CharMap::iterator it = m_CharMap.find(ch);

		if (it == m_CharMap.end())
			return;

		it->second->m_pSlot = NULL;
	}

	TEXTURESLOT* AllocTextureSlot();
	void FreeTextureSlot(TEXTURESLOT* pSlot);
	TEXTURESLOT* GetAvailableSlot(CHAR_TEX_INFO*& pInfo);
	void AddVertsToOutputBuf(TEXTURESLOT* pSlot, A3DTLVERTEX* pVerts, bool bShadowed, A3DCOLOR ShadowColor);
	bool FlushOutputBuf(TEXTURESLOT* pSlot);
	CHARPROP* AddCharToMap(wchar_t ch, bool bSetTex, bool& bNewChar);
	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }

public:

	void TextOut (
		int x,
		int y,
		const wchar_t* szText,
		A3DCOLOR color,
		bool bShadowed = false,
		A3DCOLOR ShadowColor = A3DCOLORRGBA(0, 0, 0, 255),
		float z = 0,
		int nCharCount = -1,
		float rhw = 1.0f,
		bool bUnderLine = false,
		A3DCOLOR UnderLineColor = A3DCOLORRGBA(0, 0, 0, 255),
		float fUnderLineThick = 0.08f
		);

	void TextOutVert (
		int x,
		int y,
		const wchar_t* szText,
		A3DCOLOR color,
		bool bShadowed = false,
		A3DCOLOR ShadowColor = A3DCOLORRGBA(0, 0, 0, 255),
		float z = 0,
		int nCharCount = -1,
		float rhw = 1.0f,
		bool bUnderLine = false,
		A3DCOLOR UnderLineColor = A3DCOLORRGBA(0, 0, 0, 255),
		float fUnderLineThick = 0.08f,
		DWORD eRotate = 0					// 0 - do not rotate, 1 - force rotate, 2 - conditional rotate.
		);
	
	void TextOutF (
		float x,
		float y,
		const wchar_t* szText,
		A3DCOLOR color,
		bool bShadowed = false,
		A3DCOLOR ShadowColor = A3DCOLORRGBA(0, 0, 0, 255),
		float z = 0,
		int nCharCount = -1,
		float rhw = 1.0f
		);

	void DrawText(
		int x,
		int y,
		const wchar_t* szText,
		A3DCOLOR color,
		A3DRECT* pClip,
		bool bShadowed = false,
		A3DCOLOR ShadowColor = A3DCOLORRGBA(0, 0, 0, 255),
		float z = 0,
		int nCharCount = -1,
		float rhw = 1.0f,
		bool bUnderLine = false,
		A3DCOLOR UnderLineColor = A3DCOLORRGBA(0, 0, 0, 255),
		float fUnderLineThick = 0.08f);

	void Flush();	
	A3DEngine* GetA3DEngine() const {return m_pA3DEngine;}

	//有可能很多地方使用同一字体对象，其中一处擅自调用Resize不合理，该函数应废弃掉。
	bool Resize(float fPointSize);
	bool Resize(float fPointSize, int nCreateMode);

	A3DPOINT2 GetTextExtent(const wchar_t* szText, int nCharCount = -1, bool bVertical = false);
	int GetFontHeight() const { return m_iFontHei; }
	int GetFontWidth() const { return m_iFontWid; }
	int GetPointSize() const { return m_iPointSize; }

	unsigned int GetRefCnt() const { return m_nRefCnt; }
	void IncRefCnt() { ++m_nRefCnt; }
	void DecRefCnt() { if (m_nRefCnt > 0) --m_nRefCnt; }
	bool IsZeroRef() const { return m_nRefCnt == 0; }

	bool IsLoaded() { return m_bLoaded; }
	bool CanTextOutNow();
	bool IsInLoadThread() { return m_bInLoadThread; }
	void SetInLoadThread(bool b) { m_bInLoadThread = b; }

	const AWString& GetFaceName() const { return m_strFaceName; }
	int GetStyle() const { return m_iStyle; }
	int GetOutlineWidth() const { return m_iOutline; }
	A3DCOLOR GetOutlineColor() const { return m_clOutline; }
	A3DCOLOR GetInnerColorReserve() const { return m_clInnerReserve; }
	bool IsInterpColor() const { return m_bInterpColor; }
	A3DCOLOR GetUpperColorReserve() const { return m_clUpperReserve; }
	A3DCOLOR GetLowerColorReserve() const { return m_clLowerReserve; }
	int GetCreateMode() const { return m_nCreateMode; }
	bool IsStereo() const { return m_bStereo; }

	bool IsEqual(const A3DFTFont& rhs, bool bCheckSize) const
	{
		return bCheckSize ? (m_iPointSize == rhs.m_iPointSize) : true
			 && m_iStyle == rhs.m_iStyle
			 && m_iOutline == rhs.m_iOutline
			 && m_clOutlineReserve == rhs.m_clOutlineReserve
			 && m_clInnerReserve == rhs.m_clInnerReserve
			 && m_bInterpColor == rhs.m_bInterpColor
			 && m_clUpperReserve == rhs.m_clUpperReserve
			 && m_clLowerReserve == rhs.m_clLowerReserve
			 && m_nCreateMode == rhs.m_nCreateMode
			 //&& stricmp(m_szFontPath, rhs.m_szFontPath) == 0
			 && m_bStereo == rhs.m_bStereo;
	}

	const bool operator == (const A3DFTFont& rhs) const
	{
		return IsEqual(rhs, true);
	}
};



#endif
