// FWArtCreator.cpp: implementation of the FWArtCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtCreator.h"
#include "FWArt.h"
#include "FWArtNoChange.h"
#include "FWArtTilt.h"
#include "FWArtArc.h"
#include "FWArtWave.h"
#include "FWArtDoubleArc.h"
#include "FWArtStepScale.h"
#include "FWArtStepScale.h"

#define new A_DEBUG_NEW

IMPLEMENT_SINGLETON(FWArtCreator)

// used for Init()
static const char * _Names[] = 
{
	"原始样式",
	"倾斜",
	"弧形",
	"双弧形",
	"波浪形",
	"渐变放缩",
};

// used for Init()
static const FWRuntimeClass * _Classes[] = 
{
	FW_RUNTIME_CLASS(FWArtNoChange),
	FW_RUNTIME_CLASS(FWArtTilt),
	FW_RUNTIME_CLASS(FWArtArc),
	FW_RUNTIME_CLASS(FWArtDoubleArc),
	FW_RUNTIME_CLASS(FWArtWave),
	FW_RUNTIME_CLASS(FWArtStepScale),
};

FWArtCreator::FWArtCreator()
{
	m_bInited = false;
}

FWArtCreator::~FWArtCreator()
{

}

FWArt * FWArtCreator::CreateArt(int nID)
{
	ASSERT(m_bInited);

	FWArtInfo *pInfo = FindByID(nID);
	if (!pInfo) return NULL;

	return static_cast<FWArt *>(pInfo->pClass->CreateObject());
}

bool FWArtCreator::Init()
{
	if (m_bInited) return true;

	for (int i = 0; i < ID_ART_COUNT; i++)
	{
		FWArtInfo *pInfo = new FWArtInfo;
		pInfo->ID = i;
		pInfo->Name = _Names[i];
		pInfo->pClass = _Classes[i];
		AddTail(pInfo);
	}

	m_bInited = true;
	return true;
}
