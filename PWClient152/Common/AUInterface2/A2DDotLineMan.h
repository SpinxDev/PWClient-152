// Filename	: A2DDotLineMan.h
// Creator	: daixinyu
// Date		: Dec 21, 2011
// Desc		: manager of A2DDotLine.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_A2DDOTLINEMAN_H__C8E89D9B_FFE4_416F_8ED9_05CC941A1115__INCLUDED_)
#define AFX_A2DDOTLINEMAN_H__C8E89D9B_FFE4_416F_8ED9_05CC941A1115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "A2DDotLine.h"

class A3DStream;
class A3DDevice;
class A2DDotLineMan  
{
public:
	A2DDotLineMan();
	virtual ~A2DDotLineMan();

	bool Create(A3DDevice* pA3DDevice);
	void Release();
	void Flush();

	friend class A2DDotLine;
protected:
	void AddLine(A2DDotLine* pLine);
	void RemoveLine(A2DDotLine* pLine);
	void RenderStream(int iVertSize, int iNumRect, const void* pSrc);
	A3DStream* CreateStream(A3DDevice* pA3DDevice);

protected:
	A3DStream* m_pA3DStream;
	A3DDevice* m_pA3DDevice;
	abase::vector<A2DDotLine*> m_aLines;
};

#endif // !defined(AFX_A2DDOTLINEMAN_H__C8E89D9B_FFE4_416F_8ED9_05CC941A1115__INCLUDED_)
