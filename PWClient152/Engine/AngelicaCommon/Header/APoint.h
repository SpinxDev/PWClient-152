/*
 * FILE: APoint.h
 *
 * DESCRIPTION: Point class
 *
 * CREATED BY: duyuxin, 2003/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _APOINT_H_
#define _APOINT_H_

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
//	Class APoint
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class APoint
{
public:		//	Types

public:		//	Constructions and Destructions

	APoint() {}
	APoint(const T& _x, const T& _y) { x = _x; y = _y; }
	APoint(const APoint& pt) { x = pt.x; y = pt.y; }
	
public:		//	Attributes

	T	x, y;

public:		//	Operaitons

	//	Offset point
	void Offset(const T& ox, const T& oy) { x += ox; y += oy; }
	//	Set point
	void Set(const T& _x, const T& _y) { x = _x; y = _y; }

	//	== and != operator
	friend bool operator != (const APoint& pt1, const APoint& pt2) { return pt1.x != pt2.x || pt1.y != pt2.y; }
	friend bool operator == (const APoint& pt1, const APoint& pt2) { return pt1.x == pt2.x && pt1.y == pt2.y; }

	//	+ and - operator
	friend APoint operator + (const APoint& pt1, const APoint& pt2) { return APoint(pt1.x+pt2.x, pt1.y+pt2.y); }
	friend APoint operator - (const APoint& pt1, const APoint& pt2) { return APoint(pt1.x-pt2.x, pt1.y-pt2.y); }

	APoint operator + () const { return *this; }
	APoint operator - () const { return APoint(-x, -y); }

	//	= operator
	const APoint& operator = (const APoint& pt) { x = pt.x; y = pt.y; return *this; }

	//	+= and -= operator
	const APoint& operator += (const APoint& pt) { x += pt.x; y += pt.y; return *this; }
	const APoint& operator -= (const APoint& pt) { x -= pt.x; y -= pt.y; return *this; }

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Predefined type
//
///////////////////////////////////////////////////////////////////////////

typedef APoint<int>		APointI;
typedef APoint<float>	APointF;


#endif	//	_APOINT_H_
