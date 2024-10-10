// Triangle.cpp: implementation of the CTriangle class.
//
//////////////////////////////////////////////////////////////////////

#include "Triangle.h"



#define new A_DEBUG_NEW



CTriangle::~CTriangle()
{

}

CTriangle::CTriangle(int index0, int index1, int index2)
{
	Index0 = index0;
	Index1 = index1;
	Index2 = index2;
}

CTriangle::CTriangle(int index[3])
{
	memcpy(Index, index, sizeof(int) * 3);
}

CTriangle::CTriangle(const CTriangle& src)
{
	*this = src;
}

CTriangle & CTriangle::operator = (const CTriangle & src)
{
	memcpy(this, &src, sizeof(CTriangle));
	return *this;
}

bool CTriangle::operator == (const CTriangle & src)
{
	return (memcmp(Index, src.Index, sizeof(int) * 3) == 0);
}

