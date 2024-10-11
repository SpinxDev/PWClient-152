// Filename	: AUIFrame.h
// Creator	: Tom Zhou
// Date		: June 9, 2004
// Desc		: AUIFrame is the common class of window frame.

#ifndef _AUIFRAME_H_
#define _AUIFRAME_H_

#include "Windows.h"
#include "stdio.h"

#include "A3DTypes.h"

class A3DEngine;
class A3DDevice;
class A2DSprite;
class A3DFTFont;

enum
{
	AUIFRAME_RECT_UPPERLEFT = 0,
	AUIFRAME_RECT_TOP,
	AUIFRAME_RECT_UPPERRIGHT,
	AUIFRAME_RECT_LEFT,
	AUIFRAME_RECT_BACK,
	AUIFRAME_RECT_RIGHT,
	AUIFRAME_RECT_LOWERLEFT,
	AUIFRAME_RECT_BOTTOM,
	AUIFRAME_RECT_LOWERRIGHT,
	AUIFRAME_RECT_MAX
};
#define AUIFRAME_RECT_NUM	AUIFRAME_RECT_MAX

enum
{
	AUIFRAME_ALIGN_LEFT = 0,
	AUIFRAME_ALIGN_TOP = AUIFRAME_ALIGN_LEFT,
	AUIFRAME_ALIGN_CENTER,
	AUIFRAME_ALIGN_RIGHT,
	AUIFRAME_ALIGN_BOTTOM = AUIFRAME_ALIGN_RIGHT,
};

class AUIFrame
{
public:
	AUIFrame();
	virtual ~AUIFrame();

	bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszFilename, int nMode = MODE_UNKNOWN);
	bool InitIndirect(const char *pszFilename, int nMode = MODE_UNKNOWN);
	bool SetMode(int nMode);
	void SetScale(float fScale);
	bool Release(void);
	bool Save(FILE *file);
	bool Render(int x, int y, int nWidth, int nHeight, A3DFTFont *pFont,
		A3DCOLOR color,	const ACHAR *pszText, int nAlign, int xOffset,
		int yOffset, int nAlpha, bool bNoFlush = false, int nShadow = 0, bool bVertical = false);

	SIZE GetSizeLimit(void);
	A2DSprite * GetA2DSprite(void);
	DWORD GetPixel(int x, int y, int nDefRenderWidth, int nDefRenderHeight);
	void  SetSpriteOffset(const int xOffset, const int yOffset);

protected:
	bool DrawSprite(const int x, const int y, const bool bNoFlush);

protected:
	enum
	{
		MODE_UNKNOWN = 0,
		MODE_1X1,
		MODE_3X3,
		MODE_4X4,
		MODE_3X1,
		MODE_1X3,
	};

	int m_nMode;
	float m_fScale;
	A3DEngine *m_pA3DEngine;
	A3DDevice *m_pA3DDevice;
	A2DSprite *m_pA2DSprite;
	int		   m_iSpriteOffsetX;
	int		   m_iSpriteOffsetY;
};

typedef AUIFrame * PAUIFRAME;

#endif //_AUIFRAME_H_