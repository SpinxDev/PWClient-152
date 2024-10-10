/********************************************************************
	created:	2005/04/29
	created:	29:4:2005   14:51
	filename: 	FTInterface.h
	file base:	FTInterface
	file ext:	h
	author:		yaojun
	
	purpose:	Encapsulation of Freetype2 library to provide 
				function similar to windows API: 

				TextOut()
				GetTextExtentExPoint()
	
	Usage:		Use CreateFTManager() as entry point to get a pointer
				of IFTManager type, call CreateFont() to create 
				drawer of specific font, use IFTFont interface to 
				output string. Each IFTManager should be released 
				after all  IFTFonts created by it having been released.
			
*********************************************************************/

#ifdef FTDRIVERDLL_EXPORTS
#define FTDRIVERDLL_API __declspec(dllexport)
#else
#define FTDRIVERDLL_API __declspec(dllimport)
#endif

#ifndef _FT_INTERFACE_H
#define _FT_INTERFACE_H

// typedef used by windows

typedef unsigned int UINT;
typedef int INT;
typedef int * LPINT;

typedef struct 
{
    INT cx;
    INT cy;
} FTDSIZE, *LPFTDSIZE;

typedef unsigned char BYTE;

typedef int BOOL;
#define TRUE 1
#define FALSE 0


// data structure

//	font style
#define STYLE_NORMAL	0x00000000
#define STYLE_BOLD		0x00000001
#define STYLE_ITALIC	0x00000010

//	Render mode
#define FONT_CREATE_FREETYPE		0x00000000
#define FONT_CREATE_NOFREETYPE		0x00000001


// structure used to specify the font characteristics
struct FontParam
{
	const char *FontFile;	//	1. Path to a Truetype font file when FontFileLength is 0
							//	2. or Pointer to a buffer of font file when FontFileLength is greater than 0
	int FontFileLength;		//	length of buffer that represented by FontFile
	int FaceIndex;			//	index of the face to retrive in the file
	int FontStyle;			//  one of or combine of macro STYLE_XXX
	UINT PointHeight;		//	height of char in point
	UINT PointWidth;		//	width of char in point
	UINT HorzDPI;			//	resolution in horizontal, 0 for default 96dpi
	UINT VertDPI;			//	resolution in vertical, 0 for default 96dpi
	int OutlineWidth;		//  if 0, solid char bitmap will be generated, or
	const wchar_t *FaceName;	//	The font facetype
							//  only the outline will returned
};

// interface

class FTDRIVERDLL_API IFTFont
{
public:
	virtual ~IFTFont() = 0 {}

	virtual void GetMaxSize(int& cx, int& cy) = 0;
	virtual BOOL GetCharExtent(wchar_t ch, LPFTDSIZE lpOffset, LPFTDSIZE lpSize, LPFTDSIZE lpBmpSize) = 0;

	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Exactly the same as windows API GetTextExtentExPoint(), except 
	//		that it only accept UNICODE string.
	//	<See MSND for detail>
	virtual BOOL GetTextExtentExPoint(const wchar_t *lpszStr, int cchString,  int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPFTDSIZE lpSize) = 0;
	

	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Similar to windows API TextOut except that it do not draw to HDC
	//		but a bitmap provided by user.
	//	<Input>
	//		nXStart		::	The left side x-coord to align text, in pixel.
	//		nYStart		::	The top side y-coord to align text, in pixel.
	//		lpString	::	Pointer to the string to be drawn, must be UNICODE.
	//		cbString	::	Specifies the length of the lpString. It is a 
	//						WORD count.
	//		nWidth		::	The width of pBitmap in pixel.
	//		nHeight		::	The height of pBitmap in pixel.
	//	<InOut>
	//		pBitmap		::	A buffer to draw string to. It represent a 256-level 
	//						gray, BYTE-aligned bitmap. Each pixel in it is 
	//						represented by one BYTE. So the size of the bitmap 
	//						should be ( nWidth * nHeight ) bytes.
	//	<Return>
	//		If the function succeeds, the return value is TRUE.
	//		If the function fails, the return value is FALSE.
	//	<NOTE>
	//		The bitmap do not need to be big enough to hold the whole string.
	//		The function will automatically clip the string (in pixel not in char). 
	virtual BOOL TextOut(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, int nWidth, int nHeight) = 0;

	virtual BOOL TextOutWithOutline(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, BYTE *pOutline, int nWidth, int nHeight) = 0;

	virtual BOOL DrawChar(wchar_t ch, BYTE* pBitmap, int nWidth, int nHeight) = 0;
	virtual BOOL DrawCharWithOutline(wchar_t ch, BYTE* pBitmap, BYTE* pOutline, int nWidth, int nHeight) = 0;

	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Release the font object associated with this pointer.
	virtual void Release() = 0;
};


class FTDRIVERDLL_API IFTManager
{
public:
	virtual ~IFTManager() = 0 {}

	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Create a font object and return a interface pointer.
	//	<Input>
	//		strFontFile		::	1. Path to a Truetype font file when nFontFileLength is 0
	//							2. or Pointer to a buffer of font file when nFontFileLength is greater than 0
	//		nFontFileLength ::	length of buffer that represented by FontFile
	//		uPoint			::	Point size of the character.
	//		nStyle			::	Font style. Can be one of the macro STYLE_XXX, or
	//							combine of them.
	//		nCreateMode		::	Yes/No	FreeType
	//	<Return>
	//		If the function succeeds, the return value is a interface pointer 
	//		to the font object.
	//		If the function fails, the return value is NULL.
	//	<NOTE>
	//		Call IFTFont::Release() when you don't need the font object anymore.
	
	virtual IFTFont * CreateFont(const char *strFontFile, int nFontFileLength, UINT uPoint, int nStyle, int nOutlineWidth,int nCreateMode = FONT_CREATE_FREETYPE,const wchar_t *strFaceName = NULL) = 0;

	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Create a font object and return a interface pointer.
	//	<Input>
	//		fontParam	::	A reference to a FontParam structure that defines
	//						the font characteristics.
	//		nCreateMode	::	Yes/No	FreeType
	//	<Return>
	//		If the function succeeds, the return value is a interface pointer 
	//		to the font object.
	//		If the function fails, the return value is NULL.
	//	<NOTE>
	//		Call IFTFont::Release() when you don't need the font object anymore.
	virtual IFTFont * CreateFontIndirect(const FontParam &fontParam,int nCreateMode = FONT_CREATE_FREETYPE) = 0;


	//////////////////////////////////////////////////////////////////////////
	//	<Description>
	//		Release the manager object associated with this pointer.
	//	<Note>
	//		You should release all font object create by manager before
	//		release the manager itself. 
	virtual void Release() = 0;
};


//////////////////////////////////////////////////////////////////////////
//	<Description>
//		Create a FTManager object and return a interface pointer.
//	<Input>
//		nMaxFontFamilyCountInCache	::
//			The max number of face info can be cached. 
//			set to 0 to use default value.
//		nMaxFontStyleInCache	::
//			The max number of size info can be cached. 
//			set to 0 to use default value.
//		nMaxCacheSizeBytes	::
//			The size in Byte to allocate. The cache is used to cache rendered
//			glyph images.
//	<Return>
//		If the function succeeds, the return value is a interface pointer 
//		to the manager object.
//		If the function fails, the return value is NULL.
//	<NOTE>
//		This is the entry point of the liberay. You can call this function
//		several times, each returned IFTManager are totally independent from
//		each other.
//		Call IFTManager::Release() when you don't need the font object anymore.
FTDRIVERDLL_API IFTManager * CreateFTManager(int nMaxFontFamilyCountInCache = 0, int nMaxFontSizeInCache = 0, int nMaxCacheSizeBytes = 0);

#endif
