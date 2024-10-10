#pragma once

using AudioEngine::PropCurves;

class CEffectValueChanger
{
public:
	CEffectValueChanger(PropCurves* pPropCurves, CWnd* pWnd);
	~CEffectValueChanger(void);
public:
	void				ScreenToLogical(int x, int y, float& fx, float& fy);
	void				LogicalToScreen(float fx, float fy, int& x, int& y);
protected:
	PropCurves*			m_pPropCurves;
	CWnd*				m_pWnd;
};
