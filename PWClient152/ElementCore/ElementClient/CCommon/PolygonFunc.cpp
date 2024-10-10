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
 * ���1���Ƿ��ڶ�������㷨ԭ��
 * �Ӽ�����X������������һ�����ߣ�������������εĽ���Ϊ������������ڶ�����ڣ�
 * �������Ϊż����������ڶ�����⡣
 * �����������غϣ�����Ϊ��ñ�û�н��㣻
 * ������ߴ������㣬��ͼ1��ʾ�����Ϊ��2��б�߶��ཻ����ͼ2��ʾ���ֻ����1��б���ཻ��
 *     \  /    \    /              __/__    __/____
 *    __\/__  __\__/__                \          \
 *          ͼ1                          ͼ2
 * Ϊ������㣬���㷨�б߽��ϵĵ���Ϊ�ڶ�����⡣
 */

#include "PolygonFunc.h"


// �ж�һ���Ƿ��ڶ������
// pt:�����ꣻptLines:����Σ��õ㼯��ʾ��nNumLine:����ζ��㣨�ߣ��ĸ���
bool IsInPolyGon(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nNumLine)
{
	if (nNumLine < 3)// ���Ƕ����
		return false;

	int nNumCross = 0;// �������ν������

	for (int i=0; i<nNumLine; i++)
	{
		if (IsCrossLine(pt, ptLines, i, nNumLine))// �Ӽ������Ҳ������������Ƿ���ñ��ཻ
			nNumCross++;
	}

	if ((nNumCross & 1) == 0)// ����ż�������㣬���ڶ������
		return false;
	else// �������������㣬�ڶ������
		return true;
}

// �ж������Ƿ�����ཻ
// pt:���Ե㣻ptLines:����ζ��㼯��nIndex:�ߣ���һ�����㣩����ţ�nNumLine:�ܱߣ����㣩��
bool IsCrossLine(POINT_FLOAT pt, POINT_FLOAT* ptLines, int nIndex, int nNumLine)
{
	POINT_FLOAT ptLine1 = ptLines[nIndex];// �߶ζ���1
	POINT_FLOAT ptLine2 = ptLines[(nIndex+1)%nNumLine];// �߶ζ���2

	if (ptLine1.x < pt.x && ptLine2.x < pt.x)// ����߶����������
		return false;

	if (ptLine1.y < pt.y && ptLine2.y < pt.y)// ����߶��������ϱ�
		return false;

	if (ptLine1.y > pt.y && ptLine2.y > pt.y)// ����߶��������±�
		return false;

	if (ptLine1.y == ptLine2.y)// ����߶���������ͬһˮƽ����
		return false;

	if (pt.y == ptLine1.y)// ������ߴ����߶ζ���1
	{
		// �ҵ�����1֮ǰ����������߲���ͬһˮƽ���ϵĵ�
		POINT_FLOAT ptLinePre;// ����1֮ǰ�ĵ�
		bool bFind = false;
		for (int i=1; i<nNumLine; i++)
		{
			ptLinePre = ptLines[(nIndex + nNumLine - i) % nNumLine];
			if (ptLinePre.y != pt.y)// ����ɨ������
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)// û�ҵ����������ĵ㣨��������е���ͬһˮƽ���ϣ�
			return false;

		if ((ptLinePre.y < pt.y && ptLine2.y > pt.y)
			|| (ptLinePre.y > pt.y && ptLine2.y < pt.y))// �������1֮ǰ���붥��2����������
			return false;
	}

	// ����������ߵĽ���
	float fSlope = (ptLine2.x - ptLine1.x) / (ptLine2.y - ptLine1.y);
	float fCrossX = (pt.y - ptLine1.y) * fSlope + ptLine1.x;
	if (fCrossX <= pt.x)//�����ڲ��Ե����
		return false;

	return true;
}