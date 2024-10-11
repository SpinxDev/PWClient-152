// Filename	: A2DDotLine.h
// Creator	: daixinyu
// Date		: Dec 21, 2011
// Desc		: draw 2d dot line
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_A2DDOTLINE_H__C3DB7493_409D_4C97_8F0E_0F76708AC481__INCLUDED_)
#define AFX_A2DDOTLINE_H__C3DB7493_409D_4C97_8F0E_0F76708AC481__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ABaseDef.h"
#include "A3DTypes.h"
#include "A3DVertex.h"

class A2DDotLineMan;
class A3DDevice;
class A3DTexture;
class A2DDotLine
{
public:
	A2DDotLine(A2DDotLineMan* pMan);
	virtual ~A2DDotLine();
	
	// Init
	bool Create(A3DDevice* pDevice, const char* szFileName);
	void Release(); // will be automatically released and deallocated in A2DDotLineMan::Release()

	// properties
	void SetLineWidth(int nLineWidth = 2);
	inline int GetLineWidth() const {return m_nLineWidth;}

	// screen coordinate
	bool DrawLine(const A3DPOINT2& ptFrom, const A3DPOINT2& ptTo, A3DCOLOR color);

	void Flush();

protected:
	void _PresetRenderState();
	void _PostSetRenderState();

protected:
	A3DDevice*			m_pA3DDevice;
	A2DDotLineMan*		m_pMan;
	int					m_nLineWidth;
	A3DTexture*			m_pA3DTexture;

	// helper data
	abase::vector<A3DTLVERTEX> m_aVtxBuffer;
};

#endif // !defined(AFX_A2DDOTLINE_H__C3DB7493_409D_4C97_8F0E_0F76708AC481__INCLUDED_)
