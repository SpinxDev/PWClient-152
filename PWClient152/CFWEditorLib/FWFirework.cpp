// Firework.cpp: implementation of the FWFirework class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFirework.h"
#include "A3DGFXElement.h"
#include "A3DGFXEx.h"
#include "FWArchive.h"
#include <math.h>
#include <A3DMatrix.h>

#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNAMIC(FWFirework, FWObject)





FWFirework::FWFirework()
{
	for (int i = 0; i < OFFSET_COUNT; i++)
	{
		m_fOffsetTime[i] = 0.f;
		m_vOffsetPos[i].Clear();
	}
}

FWFirework::~FWFirework()
{

}


bool FWFirework::UpdateAssemblyStartParam(FWAssembly *pAssembly, const A3DMATRIX4& matTM, float fStartTime)
{
	// calculate sum of offset time and pos
	float fTime = GetSumOfOffsetTime() + fStartTime;
	A3DVECTOR3 vOffset = GetSumOfOffsetPos();
	A3DMATRIX4 matOffset;
	matOffset.Translate(vOffset.x, vOffset.y, vOffset.z);

	// set start time
	int nEleCount = pAssembly->GetElementCount();
	for (int i = 0; i < nEleCount; i++)
	{
		A3DGFXElement *pEle = pAssembly->GetElement(i);
		pEle->SetTimeStart(DWORD(pEle->GetTimeStart() + fTime * 1000));
	} 

	// set start pos
	return pAssembly->SetParentTM(matOffset * matTM);
}



void FWFirework::Serialize(FWArchive &ar)
{
	FWObject::Serialize(ar);

 	if (ar.IsStoring())
	{
		for (int i = 0; i < OFFSET_COUNT; i++)
		{
			ar << m_vOffsetPos[i];
			ar << m_fOffsetTime[i];
		}
	}
	else
	{
		for (int i = 0; i < OFFSET_COUNT; i++)
		{
			ar >> m_vOffsetPos[i];
			ar >> m_fOffsetTime[i];
		}
	}
}

float FWFirework::GetSumOfOffsetTime()
{
	float fTime = 0;
	for (int i = 0; i < OFFSET_COUNT; i++)
		fTime += m_fOffsetTime[i];
	return fTime;
}

A3DVECTOR3 FWFirework::GetSumOfOffsetPos()
{
	A3DVECTOR3 vec(0.f, 0.f, 0.f);
	for (int i = 0; i < OFFSET_COUNT; i++)
		vec += m_vOffsetPos[i];
	return vec;
}

