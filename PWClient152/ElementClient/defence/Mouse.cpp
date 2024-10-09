//#include "stdafx.h"
#include "mouse.h"
#include <math.h>
#include <time.h>

#include "EC_Configs.h"
#include "EC_TimeSafeChecker.h"

extern CECConfigs g_GameCfgs;

Mouse	Mouse::instance;

Mouse::Mouse()
{
	g_hMouseHook = NULL;

	g_tmBegin = 0;
	g_ptLast.x = g_ptLast.y = 0;
	g_lbuttondowns = 0;
	g_rbuttondowns = 0;
	g_mousemoves = 0;
	g_mousemovedistance = 0;
	g_regionlast = -1;
	g_ticklast = GetTickCount();
	int i;
	for( i=0; i<sizeof(g_counts)/sizeof(g_counts[0]); i++)	g_counts[i] = 0;
	for( i=0; i<sizeof(g_times)/sizeof(g_times[0]); i++)	g_times[i] = 0;
	for( i=0; i<sizeof(g_rects)/sizeof(g_rects[0]); i++)	g_rects[i].left = g_rects[i].right = g_rects[i].top = g_rects[i].bottom = 0;
}

Marshal::OctetsStream& Mouse::marshal(Marshal::OctetsStream& os) const
{
	os << CompactUINT(GetInstance().GetMouseLButtonDownFreq());
	os << CompactUINT(GetInstance().GetMouseRButtonDownFreq());
	os << CompactUINT(GetInstance().GetMouseMoveFreq());
	os << GetInstance().GetMouseMoveVelocity( );
	int i;
	for( i=0; i<RegionCount; i++ )
		os << CompactUINT(GetInstance().g_counts[i]);
	for( i=0; i<RegionCount; i++ )
		os << CompactUINT(GetInstance().g_times[i]);
	return os;
}

const Marshal::OctetsStream& Mouse::unmarshal(const Marshal::OctetsStream& os)
{
	return os;
}
Octets Mouse::GetOctets()
{
	Marshal::OctetsStream os;
	os << *this;
	return (Octets&)os;
}

size_t Mouse::GetMouseLButtonDowns( )		{	return g_lbuttondowns;	}
size_t Mouse::GetMouseRButtonDowns( )		{	return g_rbuttondowns;	}
size_t Mouse::GetMouseMoves( )			{	return g_mousemoves;	}
float Mouse::GetMouseMoveDistance( )	{	return g_mousemovedistance;	}

size_t Mouse::GetMouseLButtonDownFreq( )
{
	time_t now = time(NULL);
	return (now > g_tmBegin ? (size_t)(3600*g_lbuttondowns / (now-g_tmBegin)) : 0);
}
size_t Mouse::GetMouseRButtonDownFreq( )
{
	time_t now = time(NULL);
	return (now > g_tmBegin ? (size_t)(3600*g_rbuttondowns / (now-g_tmBegin)) : 0);
}
size_t Mouse::GetMouseMoveFreq( )
{
	time_t now = time(NULL);
	return (now > g_tmBegin ? (size_t)(3600*g_mousemoves / (now-g_tmBegin)) : 0);
}
float Mouse::GetMouseMoveVelocity( )
{
	time_t now = time(NULL);
	return (float)(now > g_tmBegin ? 3600*g_mousemovedistance / (now-g_tmBegin) : 0);
}

RECT MakeRECT( int l, int t, int r, int b )
{
	RECT	rect;
	rect.left = l;
	rect.top = t;
	rect.right = r;
	rect.bottom = b;
	return rect;
}

void Mouse::SetWindowRect( RECT rect )
{
	if( rect.left == rect.right || rect.top == rect.bottom )
		return;
	int x = (rect.right-rect.left)/3;
	int y = (rect.bottom-rect.top)/3;
	g_rects[0]	=	MakeRECT( rect.left, rect.top, rect.left+x, rect.top+y );
	g_rects[1]	=	MakeRECT( rect.left+x, rect.top, rect.left+x+x, rect.top+y );
	g_rects[2]	=	MakeRECT( rect.left+x+x, rect.top, rect.right, rect.top+y );

	g_rects[3]	=	MakeRECT( rect.left, rect.top+y, rect.left+x, rect.top+y+y );
	g_rects[4]	=	MakeRECT( rect.left+x, rect.top+y, rect.left+x+x, rect.top+y+y );
	g_rects[5]	=	MakeRECT( rect.left+x+x, rect.top+y, rect.right, rect.top+y+y );

	g_rects[6]	=	MakeRECT( rect.left, rect.top+y+y, rect.left+x, rect.bottom );
	g_rects[7]	=	MakeRECT( rect.left+x, rect.top+y+y, rect.left+x+x, rect.bottom );
	g_rects[8]	=	MakeRECT( rect.left+x+x, rect.top+y+y, rect.right, rect.bottom );
}

LRESULT CALLBACK Mouse::HookMouseProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if( nCode < 0 )
		return CallNextHookEx( GetInstance().g_hMouseHook, nCode, wParam, lParam );

	if( HC_ACTION == nCode )
	{
		if( WM_MOUSEMOVE == wParam )
		{
			MOUSEHOOKSTRUCT * pinfo = (MOUSEHOOKSTRUCT *)lParam;
			if( 0 != GetInstance().g_ptLast.x && 0 != GetInstance().g_ptLast.y )
				GetInstance().g_mousemovedistance += sqrt((float) (pinfo->pt.x - GetInstance().g_ptLast.x)*(pinfo->pt.x - GetInstance().g_ptLast.x) + (pinfo->pt.y - GetInstance().g_ptLast.y)*(pinfo->pt.y - GetInstance().g_ptLast.y) );
			GetInstance().g_mousemoves ++;

			int i;
			for( i=0; i<RegionCount; i++ )
			{
				if( ::PtInRect(&(GetInstance().g_rects[i]), pinfo->pt) )
				{
					GetInstance().g_counts[i] ++;
					if( i!=GetInstance().g_regionlast )
					{
						if( GetInstance().g_regionlast >= 0 && GetInstance().g_regionlast < RegionCount )
						{
							DWORD dwTick = GetTickCount();
							GetInstance().g_times[GetInstance().g_regionlast] += CECTimeSafeChecker::ElapsedTime(dwTick, GetInstance().g_ticklast);
							GetInstance().g_ticklast = dwTick;
						}
						GetInstance().g_regionlast = i;
					}
					break;
				}
			}
			if( RegionCount == i )
			{
				RECT rect;
				HWND hwnd = FindWindowA( "ElementClient Window", AC2AS(g_GameCfgs.GetWindowsTitle()));
				if( NULL != hwnd )
				{
					::GetWindowRect( hwnd, &rect );
					GetInstance().SetWindowRect( rect );
				}
				else if( 0 == GetInstance().g_ptLast.x && 0 == GetInstance().g_ptLast.y )
				{
				}
				else if( ::IsRectEmpty( &(GetInstance().g_rects[8]) ) )
				{
					rect = MakeRECT(min(pinfo->pt.x,GetInstance().g_ptLast.x),min(pinfo->pt.y,GetInstance().g_ptLast.y),
								max(pinfo->pt.x,GetInstance().g_ptLast.x)+1,max(pinfo->pt.y,GetInstance().g_ptLast.y)+1 );
					GetInstance().SetWindowRect( rect );
				}
				else
				{
					rect = MakeRECT(min(pinfo->pt.x,GetInstance().g_rects[0].left),min(pinfo->pt.y,GetInstance().g_rects[0].top),
								max(pinfo->pt.x,GetInstance().g_rects[8].right)+1,max(pinfo->pt.y,GetInstance().g_rects[8].bottom)+1 );
					GetInstance().SetWindowRect( rect );
				}
			}
			GetInstance().g_ptLast = pinfo->pt;
		}
		else if( WM_LBUTTONDOWN == wParam )
		{
			GetInstance().g_lbuttondowns ++;
		}
		else if( WM_RBUTTONDOWN == wParam )
		{
			GetInstance().g_rbuttondowns ++;
		}
	}

	return CallNextHookEx( GetInstance().g_hMouseHook, nCode, wParam, lParam );
}

BOOL Mouse::SetMouseHook( )
{
	g_tmBegin	=	time(NULL);
	g_hMouseHook = SetWindowsHookEx( WH_MOUSE, HookMouseProc, NULL, GetCurrentThreadId() );
	return NULL != g_hMouseHook;
}

BOOL Mouse::ReleaseMouseHook( )
{
	if( g_hMouseHook )
	{
		BOOL ret = UnhookWindowsHookEx( g_hMouseHook );
		g_hMouseHook = NULL;
		return ret;
	}
	return FALSE;
}

