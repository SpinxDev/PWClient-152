/*
 * FILE: ARect.h
 *
 * DESCRIPTION: Rectangle class
 *
 * CREATED BY: duyuxin, 2003/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ARECT_H_
#define _ARECT_H_

#include "APoint.h"
#include "AAssist.h"

#pragma warning (disable: 4786)

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class ARect
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class ARect
{
public:		//	Types

public:		//	Constructions and Destructions

	ARect() {}
	ARect(const T& _left, const T& _top, const T& _right, const T& _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
	ARect(const ARect& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
	
public:		//	Attributes

	T	left, top, right, bottom;

public:		//	Operaitons

	//	== and != operator
	friend bool operator != (const ARect& rc1, const ARect& rc2) { return rc1.left != rc2.left || rc1.top != rc2.top || rc1.right != rc2.right || rc1.bottom != rc2.bottom; }
	friend bool operator == (const ARect& rc1, const ARect& rc2) { return rc1.left == rc2.left && rc1.top == rc2.top && rc1.right == rc2.right && rc1.bottom == rc2.bottom; }

	//	+ and - operator
	friend ARect operator + (const ARect& rc1, const ARect& rc2) { return ARect(rc1.left+rc2.left, rc1.top+rc2.top, rc1.right+rc2.right, rc1.bottom+rc2.bottom); }
	friend ARect operator - (const ARect& rc1, const ARect& rc2) { return ARect(rc1.left-rc2.left, rc1.top-rc2.top, rc1.right-rc2.right, rc1.bottom-rc2.bottom); }
	friend ARect operator + (const ARect& rc1, const APoint<T>& pt) { return ARect(rc1.left+pt.x, rc1.top+pt.y, rc1.right+pt.x, rc1.bottom+pt.y); }
	friend ARect operator - (const ARect& rc1, const APoint<T>& pt) { return ARect(rc1.left-pt.x, rc1.top-pt.y, rc1.right-pt.x, rc1.bottom-pt.y); }

	//	&= and |= operator
	const ARect& operator &= (const ARect& rc) { *this = *this & rc; return *this; }
	const ARect& operator |= (const ARect& rc) { *this = *this | rc; return *this; }

	ARect operator + () const { return *this; }
	ARect operator - () const { return ARect(-left, -top, -right, -bottom); }

	//	= operator
	const ARect& operator = (const ARect& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; return *this; }

	//	+= and -= operator
	const ARect& operator += (const ARect& rc) { left += rc.left; top += rc.top; right += rc.right; bottom += rc.bottom; return *this; }
	const ARect& operator -= (const ARect& rc) { left -= rc.left; top -= rc.top; right -= rc.right; bottom -= rc.bottom; return *this; }
	const ARect& operator += (const APoint<T>& pt) { left += pt.x; top += pt.y; right += pt.x; bottom += pt.y; return *this; }
	const ARect& operator -= (const APoint<T>& pt) { left -= pt.x; top -= pt.y; right -= pt.x; bottom -= pt.y; return *this; }

	//	Get width of rectangle
	T Width() const { return right - left; }
	//	Get height of rectangle
	T Height() const { return bottom - top; }
	//	Get center point of rectangle
	APoint<T> CenterPoint() const { return APoint<T>((left+right) / 2, (top+bottom) / 2); }
	//	Set rectangle value
	void SetRect(const T& _left, const T& _top, const T& _right, const T& _bottom) { left=_left; top=_top; right=_right; bottom=_bottom; }
	
	//	Point in rectangle
	bool PtInRect(const T& x, const T& y) const { return (x >= left && x < right && y >= top && y < bottom) ? true : false; }
	bool PtInRect(const APoint<T>& pt) const { return PtInRect(pt.x, pt.y); }

	//	Normalize rectangle. Note: The following CRect member functions require 
	//	normalized rectangles in order to work properly: Height, Width, Size, 
	//	IsEmpty, PtInRect, SetUnion, SetIntersect, operator ==, operator !=, 
	//	operator |, operator |=, operator &, and operator &=
	void Normalize();

	//	All members are 0 ?
	bool IsRectNull() const { return (left == 0 && top == 0 && right == 0 && bottom == 0); }
	//	Rectangle is empty ?
	bool IsEmpty() const { return (Width() == 0 || Height() == 0); }
	//	Set all members to 0
	void Clear() { left = top = right = bottom = 0; }
	//	Deflate rectangle
	void Deflate(const T& x, const T& y) { left += x; top += y; right -= x; bottom -= y; }
	void Deflate(const ARect<T>& rc) { left += rc.left; top += rc.top; right -= rc.right; bottom -= rc.bottom; }
	void Deflate(const T& l, const T& t, const T& r, const T& b) { left += l; top += t; right -= r; bottom -= b; }
	//	Inflate rectangle
	void Inflate(const T& x, const T& y) { left -= x; top -= y; right += x; bottom += y; }
	void Inflate(const ARect& rc) { left -= rc.left; top -= rc.top; right += rc.right; bottom += rc.bottom; }
	void Inflate(const T& l, const T& t, const T& r, const T& b) { left -= l; top -= t; right += r; bottom += b; }
	//	Offset rectangle
	void Offset(const T& x, const T& y) { left += x; top += y; right += x; bottom += y; }
	void Offset(const APoint<T>& pt) { *this += pt; }
	//	Set rectangle as union result
	void SetUnion(const ARect<T>& rc1, const ARect<T>& rc2) { *this = rc1 | rc2; }
	//	Set rectangle as intersect result
	void SetIntersect(const ARect<T>& rc1, const ARect<T>& rc2) { *this = rc1 & rc2; }

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Predefined type
//
///////////////////////////////////////////////////////////////////////////

typedef ARect<int>		ARectI;
typedef ARect<float>	ARectF;

///////////////////////////////////////////////////////////////////////////
//
//	Implement AArray
//
///////////////////////////////////////////////////////////////////////////

//	operator & calculate the intersection of two rectangles, both rc1 and rc2 
//	need to be normlaized in order the result
template <class T>
ARect<T> operator & (const ARect<T>& rc1, const ARect<T>& rc2)
{
	if (rc1.IsEmpty() || rc2.IsEmpty())
		return ARect<T>(0, 0, 0, 0);

	if (rc1.left >= rc2.right || rc2.left >= rc1.right ||
		rc1.top >= rc2.bottom || rc2.top >= rc1.bottom)
		return ARect<T>(0, 0, 0, 0);

	return ARect<T>(rc1.left > rc2.left ? rc1.left : rc2.left,
				  rc1.top > rc2.top ? rc1.top : rc2.top,
				  rc1.right < rc2.right ? rc1.right : rc2.right,
				  rc1.bottom < rc2.bottom ? rc1.bottom : rc2.bottom);
}

//	operator | calculate the union of two rectangles, both rc1 and rc2 
//	need to be normlaized in order the result
template <class T>
ARect<T> operator | (const ARect<T>& rc1, const ARect<T>& rc2)
{
	if (rc1.IsEmpty())
		return rc2;

	if (rc2.IsEmpty())
		return rc1;
	
	return ARect<T>(rc1.left < rc2.left ? rc1.left : rc2.left,
				  rc1.top < rc2.top ? rc1.top : rc2.top,
				  rc1.right > rc2.right ? rc1.right : rc2.right,
				  rc1.bottom > rc2.bottom ? rc1.bottom : rc2.bottom);
}

//	Normalize rectangle, set both the height and width are positive
template <class T>
void ARect<T>::Normalize()
{
	if (left > right)
		a_Swap(left, right); 
	
	if (top > bottom) 
		a_Swap(top, bottom);
}


#endif	//	_ARECT_H_
