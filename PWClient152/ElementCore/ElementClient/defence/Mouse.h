#pragma once

#pragma warning( disable : 4786 )

#include <windows.h>
#include <tchar.h>
#include <winsvc.h>
#include <vector>

#include "infocollection.h"

class Mouse : public InfoCollection
{
	enum { RegionCount = 9 };
public:
	static Mouse instance;
	static Mouse& GetInstance() { return instance; }

	Mouse::Mouse();

	OctetsStream& marshal(OctetsStream& os) const;
	const OctetsStream& unmarshal(const OctetsStream& os);
	Octets GetOctets();

	size_t GetMouseLButtonDowns( );
	size_t GetMouseRButtonDowns( );
	size_t GetMouseMoves( );
	float GetMouseMoveDistance( );

	size_t GetMouseLButtonDownFreq( );
	size_t GetMouseRButtonDownFreq( );
	size_t GetMouseMoveFreq( );
	float GetMouseMoveVelocity( );

	BOOL SetMouseHook( );
	BOOL ReleaseMouseHook( );
private:
	void SetWindowRect( RECT rect );
	static LRESULT CALLBACK HookMouseProc( int nCode, WPARAM wParam, LPARAM lParam );

	HHOOK	g_hMouseHook;

	time_t	g_tmBegin;
	POINT	g_ptLast;
	size_t	g_lbuttondowns;
	size_t	g_rbuttondowns;
	size_t	g_mousemoves;
	float	g_mousemovedistance;
	int		g_regionlast;
	DWORD	g_ticklast;
	DWORD	g_counts[RegionCount];
	DWORD	g_times[RegionCount];
	RECT	g_rects[RegionCount];
};
