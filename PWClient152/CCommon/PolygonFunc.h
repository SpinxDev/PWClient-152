/*
 * FILE: PolygonFunc.h
 *
 * DESCRIPTION: Functions for polygon
 *
 * CREATED BY: Jiang Dalong, 2005/03/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _POLYGONFUNC_H_
#define _POLYGONFUNC_H_

typedef struct
{
	float x;
	float y;
}POINT_FLOAT;

// �ж�һ���Ƿ��ڶ������
// pt:�����ꣻptLines:����Σ��õ㼯��ʾ��nNumLine:����ζ��㣨�ߣ��ĸ���
bool IsInPolyGon(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nNumLine);
// �ж������Ƿ�����ཻ
// pt:���Ե㣻ptLines:����ζ��㼯��nIndex:�ߣ���һ�����㣩����ţ�nNumLine:�ܱߣ����㣩��
bool IsCrossLine(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nIndex, int nNumLine);

#endif // #ifndef _POLYGONFUNC_H_