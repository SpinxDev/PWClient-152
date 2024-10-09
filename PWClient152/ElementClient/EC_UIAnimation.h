// Filename	: EC_UIAnimation.h
// Creator	: Feng Ning
// Date		: 2011/11/17

#pragma once

#include "AUIDialog.h"

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
//	Class CECUIAnimation
//	
///////////////////////////////////////////////////////////////////////////

class CECUIAnimation
{
public:
	struct KeyFrame
	{
		int tick;
		float scale;
		int ox;
		int oy;
		int alpha;

		KeyFrame()
			:tick(0),scale(1.0f),ox(0),oy(0), alpha(255)
		{}

		KeyFrame(const KeyFrame& rhs)
			:tick(rhs.tick),scale(rhs.scale),ox(rhs.ox),oy(rhs.oy),alpha(rhs.alpha)
		{}

		KeyFrame(int tick, float scale, int ox, int oy, int alpha)
			:tick(tick),scale(scale),ox(ox),oy(oy), alpha(alpha)
		{}

		KeyFrame operator-(const KeyFrame& rhs) const
		{
			KeyFrame f;
			f.tick = tick - rhs.tick;
			f.scale = scale - rhs.scale;
			f.ox = ox - rhs.ox;
			f.oy = oy - rhs.oy;
			f.alpha = alpha - rhs.alpha;
			return f;
		}

		KeyFrame operator+(const KeyFrame& rhs) const
		{
			KeyFrame f;
			f.tick = tick + rhs.tick;
			f.scale = scale + rhs.scale;
			f.ox = ox + rhs.ox;
			f.oy = oy + rhs.oy;
			f.alpha = alpha + rhs.alpha;
			return f;
		}
	};

	CECUIAnimation();

	void Reset();
	void Start(PAUIDIALOG pDlg);
	bool Update();
	void AddFrame(const KeyFrame& frame);
	bool BelongsTo(const PAUIDIALOG pDlg){return m_pDlg == pDlg;}
	void SetPos(const POINT& point){m_OriginPos = point;}

	bool IsStop();

	static CECUIAnimation* Load(const char* path);

protected:
	abase::vector<KeyFrame> m_Frames;
	int m_CurFrame;
	int m_Tick;

	PAUIDIALOG m_pDlg;
	POINT m_OriginPos;
	SIZE m_OriginSize;
	int	 m_OriginAlpha;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

