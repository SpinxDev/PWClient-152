// EditerBezierPoint.h: interface for the CEditerBezierPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITERBEZIERPOINT_H__5677C032_59BC_4E94_9FD9_D95F3A8131AF__INCLUDED_)
#define AFX_EDITERBEZIERPOINT_H__5677C032_59BC_4E94_9FD9_D95F3A8131AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INVALIDOBJECTID -1

#include "A3DBezierPoint.h"
#include "a3dmacros.h"

class CEditerBezierPoint : public A3DBezierPoint 
{
public:

	void SetID(int id);
	int  GetID();
	void SetPos(const A3DVECTOR3 & pos);
	void SetDirection(A3DVECTOR3 & dir){m_av3_dir = dir; };
	A3DVECTOR3 GetDirection(){ return m_av3_dir; };

	A3DVECTOR3 GetPos();
	bool Init(A3DDevice * pA3DDevice, const A3DVECTOR3& vecPos, A3DCOLOR color=A3DCOLORRGB(255,0,0), float vRadius=2.0f);
	
	CEditerBezierPoint();
	virtual ~CEditerBezierPoint();

protected:
	
	int					m_int_id;	// 对应的线路图上的点的ID
	A3DVECTOR3			m_av3_pos;	// 点的位置
	A3DVECTOR3          m_av3_dir;  // 摄像机在该点的方向

};

#endif // !defined(AFX_EDITERBEZIERPOINT_H__5677C032_59BC_4E94_9FD9_D95F3A8131AF__INCLUDED_)
