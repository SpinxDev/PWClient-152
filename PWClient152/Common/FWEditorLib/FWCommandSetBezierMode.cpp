// SetBezierModeCommand.cpp: implementation of the FWCommandSetBezierMode class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandSetBezierMode.h"
#include "FWGlyphBezier.h"
#include "FWDoc.h"
#include "FWView.h"

#define new A_DEBUG_NEW




FWCommandSetBezierMode::FWCommandSetBezierMode(FWGlyphBezier * pBezier, int nHandle, int nMode)
{
	m_pBezier = pBezier;
	m_nHandle = nHandle;
	m_nNewMode = nMode;
	if (m_nHandle > 0) // single mode
	{
		m_aryOldMode.SetSize(1, 0);
		m_aryOldMode[0] = m_pBezier->GetMode(nHandle);
	}
	else // all handles are affected
	{
		m_aryOldMode.SetSize(m_pBezier->GetHandleCount() / 3, 10);
		for (int i = 1; i <= m_pBezier->GetHandleCount(); i += 3)
			m_aryOldMode[(i - 1) / 3] = m_pBezier->GetMode(i);
	}
}

FWCommandSetBezierMode::~FWCommandSetBezierMode()
{

}

void FWCommandSetBezierMode::Execute()
{
	if (m_nHandle > 0) // single mode
	{
		m_pBezier->SetMode(m_nHandle, m_nNewMode);
	}
	else // all handles are affected
	{
		m_pBezier->SetAllMode(m_nNewMode);
	}
	m_pBezier->GenerateMeshParam();
}

void FWCommandSetBezierMode::Unexecute()
{
	if (m_nHandle > 0) // single mode
	{
		m_pBezier->SetMode(m_nHandle, m_aryOldMode[0]);
	}
	else // all handles ar affected
	{
		for (int i = 0; i < m_aryOldMode.GetSize(); i++)
		{
			m_pBezier->SetMode(i * 3 + 1, m_aryOldMode[i]);
		}
	}
	m_pBezier->GenerateMeshParam();
}
