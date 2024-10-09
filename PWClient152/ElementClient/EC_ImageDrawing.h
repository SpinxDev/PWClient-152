/*
 * FILE: EC_ImageDrawing.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: LiaoYangguang, 2014/8/6
 */

#include "A3DTypes.h"

class A2DSprite;
class A2DSpriteBuffer;

class CECImageDrawing
{
public:
	CECImageDrawing();
	virtual ~CECImageDrawing();

	bool LoadImageToDraw(const char* szFile, int iWidth, int iHeight, int iNumElem=0 );
	bool LoadImageToDraw(const char* szFile, int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem=0);
	bool HasImage()const;
	void DrawImage(int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f, int iDstWid=0, int iDstHei=0);
	void Flush();
	void ReleaseImage();
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }

protected:
	A2DSprite*			m_pImage;
	A2DSpriteBuffer*	m_pImageBuf;	//	Image buffer
	
	int					m_iWidth;		//	Image width
	int					m_iHeight;	//	Image height
	int					m_iItemNum;
	int					m_iCurItem;
};

class CECBarDrawing : public CECImageDrawing 
{
	bool ResetImage(int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem=0);
public:
	CECBarDrawing();

	bool InitialImage(const char* szFile);
	void DrawingBoard(int sx, int sy, int sizex, int sizey, float sz);
};