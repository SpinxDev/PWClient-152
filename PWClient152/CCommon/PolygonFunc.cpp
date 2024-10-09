/*
 * FILE: PolygonFunc.cpp
 *
 * DESCRIPTION: Functions for polygon
 *
 * CREATED BY: Jiang Dalong, 2005/03/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

/*
 * 检测1点是否在多边形内算法原理：
 * 从检测点沿X轴正方向引出一条射线，如果射线与多边形的交点为奇数，则检测点在多边形内，
 * 如果交点为偶数，则检测点在多边形外。
 * 如果射线与边重合，则认为与该边没有交点；
 * 如果射线穿过顶点，如图1所示情况认为与2个斜边都相交，如图2所示情况只跟第1个斜边相交。
 *     \  /    \    /              __/__    __/____
 *    __\/__  __\__/__                \          \
 *          图1                          图2
 * 为方便计算，本算法中边界上的点认为在多边形外。
 */

#include "PolygonFunc.h"


// 判断一点是否在多边形内
// pt:点坐标；ptLines:多边形，用点集表示；nNumLine:多边形顶点（边）的个数
bool IsInPolyGon(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nNumLine)
{
	if (nNumLine < 3)// 不是多边形
		return false;

	int nNumCross = 0;// 点与多边形交点个数

	for (int i=0; i<nNumLine; i++)
	{
		if (IsCrossLine(pt, ptLines, i, nNumLine))// 从检测点向右侧引出的射线是否与该边相交
			nNumCross++;
	}

	if ((nNumCross & 1) == 0)// 共有偶数个交点，则不在多边形内
		return false;
	else// 共有奇数个交点，在多边形内
		return true;
}

// 判断射线是否与边相交
// pt:测试点；ptLines:多边形顶点集；nIndex:边（第一个顶点）的序号；nNumLine:总边（顶点）数
bool IsCrossLine(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nIndex, int nNumLine)
{
	POINT_FLOAT ptLine1 = ptLines[nIndex];// 线段顶点1
	POINT_FLOAT ptLine2 = ptLines[(nIndex+1)%nNumLine];// 线段顶点2

	if (ptLine1.x < pt.x && ptLine2.x < pt.x)// 如果线段在射线左边
		return false;

	if (ptLine1.y < pt.y && ptLine2.y < pt.y)// 如果线段在射线上边
		return false;

	if (ptLine1.y > pt.y && ptLine2.y > pt.y)// 如果线段在射线下边
		return false;

	if (ptLine1.y == ptLine2.y)// 如果线段与射线在同一水平线上
		return false;

	if (pt.y == ptLine1.y)// 如果射线穿过线段顶点1
	{
		// 找到顶点1之前最近且与射线不在同一水平线上的点
		POINT_FLOAT ptLinePre;// 顶点1之前的点
		bool bFind = false;
		for (int i=1; i<nNumLine; i++)
		{
			ptLinePre = ptLines[(nIndex + nNumLine - i) % nNumLine];
			if (ptLinePre.y != pt.y)// 不在扫描线上
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)// 没找到符合条件的点（多边形所有点在同一水平线上）
			return false;

		if ((ptLinePre.y < pt.y && ptLine2.y > pt.y)
			|| (ptLinePre.y > pt.y && ptLine2.y < pt.y))// 如果顶点1之前点与顶点2在射线两侧
			return false;
	}

	// 计算射线与边的交点
	float fSlope = (ptLine2.x - ptLine1.x) / (ptLine2.y - ptLine1.y);
	float fCrossX = (pt.y - ptLine1.y) * fSlope + ptLine1.x;
	if (fCrossX <= pt.x)//交点在测试点左侧
		return false;

	return true;
}