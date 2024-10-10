/*
 * FILE: A3DBillBoard.h
 *
 * DESCRIPTION: An billboard to show effects such as open fire, bullets and etc.
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBILLBOARD_H_
#define _A3DBILLBOARD_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DMacros.h"
#include "A3DObject.h"
#include "AString.h"
#include "AAssist.h"

#define A3DBILLBOARD_MAXKEYNUMBER		16

class A3DDevice;
class A3DTexture;
class A3DStream;
class A3DViewport;
class A3DGFXMan;

enum A3DBILLBOARD_TYPE
{
	A3DBILLBOARD_BILL = 0,
	A3DBILLBOARD_FACING = 1,
	A3DBILLBOARD_MARK = 2,
	A3DBILLBOARD_AXISDECAL = 3,
	A3DBILLBOARD_DYNAMICLIGHT = 4,
	A3DBILLBOARD_SHADOW = 5
};

struct BILLBOARD_KEYINFO
{
	FLOAT		vSizeX;
	FLOAT		vSizeY;
	FLOAT		vSizeZ;
	FLOAT		vAngle; // In degree;
	A3DCOLOR	color;
	int			nTick;
};

class A3DBillBoard : public A3DObject
{
private:
	// Only for mark effect;
	A3DLVERTEX *		m_pMarkVerts;
	WORD *				m_pMarkIndices;
	A3DLVERTEX			m_aRenderVerts[12];
	int					m_nMarkVertCount;
	int					m_nMarkIndexCount;

	// General property;
	A3DDevice *			m_pA3DDevice;
	A3DTexture *		m_pTexture;
	A3DStream *			m_pStream;

	AString				m_strTextureMap;
	int					m_nTexRow;
	int					m_nTexCol;
	int					m_nTexCurRow;
	int					m_nTexCurCol;
	int					m_nTexInterval;

	A3DBLEND			m_SrcBlend;
	A3DBLEND			m_DestBlend;

	A3DMATRIX4			m_matRelativeTM;
	A3DMATRIX4			m_matAbsoluteTM;
	A3DMATRIX4			m_matParentTM;

	A3DVECTOR3			m_vecPos;
	A3DVECTOR3			m_vecDir;
	A3DVECTOR3			m_vecUp;
	A3DVECTOR3			m_vecLeft;

	int					m_nTicks;

	FLOAT				m_vScaleX;
	FLOAT				m_vScaleY;
	FLOAT				m_vScaleZ;

	FLOAT				m_vLength;
	FLOAT				m_vSizeX;
	FLOAT				m_vSizeY;
	FLOAT				m_vSizeZ;
	FLOAT				m_vAngle;
	A3DCOLOR			m_Color;

	int					m_nKeyNumber;
	int					m_nCurrentKey;
	BILLBOARD_KEYINFO	m_pKeyInfos[A3DBILLBOARD_MAXKEYNUMBER];

	int					m_nRepeatNumber;
	int					m_nRepeatCount;

	bool				m_bTextureRepeatFlag; // flag indicates whether texture will wrap back to first frame

	A3DBILLBOARD_TYPE	m_Type;
	int					m_nDLightID;
	bool				m_bExpired;

	DWORD				m_hVertexCollectorHandle;

protected:
	bool UpdateUp();
	bool UpdateInfo();

public:

	A3DBillBoard();
	~A3DBillBoard();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool TickAnimation();
	bool Render(A3DViewport * pCurrentViewport);

	bool SplitMark();
	bool RenderMark(A3DViewport * pCurrentViewport);
	
	bool UpdateParentTM(const A3DMATRIX4& matParentTM);
	bool UpdateRelativeTM();
	bool UpdateAbsoluteTM();

	bool Create();

	bool Start();
	bool Stop();

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);
	inline bool IsExpired() { return m_bExpired; }

	// Interface for setting and getting values;
	inline void SetKeyNumber(int nKeyNumber) { m_nKeyNumber = nKeyNumber; }
	inline int GetKeyNumber() { return m_nKeyNumber; }

	inline void SetKeyInfo(int nIndex, const BILLBOARD_KEYINFO& keyInfo){ m_pKeyInfos[nIndex] = keyInfo; UpdateInfo(); }
	inline void GetKeyInfo(int nIndex, BILLBOARD_KEYINFO * pKeyInfo){ *pKeyInfo = m_pKeyInfos[nIndex]; }

	inline void SetScaleX(FLOAT vScaleX) { m_vScaleX = vScaleX; }
	inline void SetScaleY(FLOAT vScaleY) { m_vScaleY = vScaleY; }
	inline void SetScaleZ(FLOAT vScaleZ) { m_vScaleZ = vScaleZ; }
	inline FLOAT GetScaleX() { return m_vScaleX; }
	inline FLOAT GetScaleY() { return m_vScaleY; }
	inline FLOAT GetScaleZ() { return m_vScaleZ; }

	inline void SetPosX(FLOAT x) { m_vecPos.x = x; UpdateRelativeTM(); }
	inline void SetPosY(FLOAT y) { m_vecPos.y = y; UpdateRelativeTM(); }
	inline void SetPosZ(FLOAT z) { m_vecPos.z = z; UpdateRelativeTM(); }
	inline FLOAT GetPosX() { return m_vecPos.x; }
	inline FLOAT GetPosY() { return m_vecPos.y; }
	inline FLOAT GetPosZ() { return m_vecPos.z; }
	inline void SetDir(FLOAT vDeg, FLOAT vPitch) 
	{
		m_vecDir.x = (FLOAT)(sin(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));
		m_vecDir.y = (FLOAT)sin(DEG2RAD(vPitch));
		m_vecDir.z = (FLOAT)(cos(DEG2RAD(vDeg)) * cos(DEG2RAD(vPitch)));
		UpdateUp(); 
		UpdateRelativeTM();
	}
	inline FLOAT GetDirDeg() 
	{
		return (FLOAT) RAD2DEG(atan2(m_vecDir.x, m_vecDir.z));
	}
	inline FLOAT GetDirPitch()
	{
		return (FLOAT) RAD2DEG(asin(m_vecDir.y));
	}
	inline void SetRepeatNumber(int nRepeatNum) 
	{ 
		m_bTextureRepeatFlag = nRepeatNum < 0 ? true : false; 
		m_nRepeatNumber = abs(nRepeatNum);
	}
	inline int GetRepeatNumber() 
	{ 
		return (m_bTextureRepeatFlag ? -1 : 1) * m_nRepeatNumber; 
	}

	inline void SetBillBoardType(A3DBILLBOARD_TYPE billType) { m_Type = billType; }
	inline A3DBILLBOARD_TYPE GetBillBoardType() { return m_Type; }

	inline void SetTextureMap(const char* szTextureMap) { m_strTextureMap =szTextureMap; }
	bool ChangeTextureMap(const char* m_strTextureMap);

	inline const char* GetTextureMap() { return m_strTextureMap; }
	inline int GetTexRow() { return m_nTexRow; }
	inline int GetTexCol() { return m_nTexCol; }
	inline int GetTexInterval() { return m_nTexInterval; }
	inline void SetTexRow(int nTexRow) { m_nTexRow = max2(nTexRow, 1); }
	inline void SetTexCol(int nTexCol) { m_nTexCol = max2(nTexCol, 1); }
	inline void SetTexInterval(int nTexInterval) { m_nTexInterval = max2(nTexInterval, 1); }

	inline void SetLength(FLOAT vLength) { m_vLength = vLength; }
	inline FLOAT GetLength() { return m_vLength; }

	inline A3DBLEND GetSrcBlend() { return m_SrcBlend; }
	inline void SetSrcBlend(A3DBLEND blend) { m_SrcBlend = blend; }
	inline A3DBLEND GetDestBlend() { return m_DestBlend; }
	inline void SetDestBlend(A3DBLEND blend) { m_DestBlend = blend; }
};

typedef A3DBillBoard * PA3DBillBoard;
#endif//_A3DBILLBOARD_H_