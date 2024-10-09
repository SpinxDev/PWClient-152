/*
 * FILE: GL_Game.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2003/12/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <windows.h>
#include "A3DTypes.h"
#include "AArray.h"
#include "EC_StringTab.h"
#include "hashtab.h"

#include "AList2.h"
#include "AAssist.h"
#include <map>
#include <set>

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

class A3DEngine;
class A3DDevice;
class A3DLight;
class A3DSkinModel;
class A3DSkin;
class A3DMuscleData;
class AMSoundBufferMan;
class AWString;

class CELBuilding;
class CECViewport;
class CECGameRun;
class CECGameSession;
class CECRTDebug;
class A3DGFXEx;
class A3DGFXExMan;
class CECModelMan;
class A3DCursor;
class A3DFont;
class A3DSkinRender;
class CECShadowRender;
class CECPortraitRender;
class CECGFXCaster;
class CECImageRes;
class CECConfigs;
class ATaskTemplMan;
class CELBackMusic;
class CECSoundCache;
class CECModel;
class CECFactionMan;
class DataPathMan;
class A3DLitModelRender;
struct EC_SYSTEM_SETTING;

class elementdataman;
class itemdataman;

namespace GNET
{
	class Privilege;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGame
//	
///////////////////////////////////////////////////////////////////////////

class CECGame
{
public:	//	Types

	enum
	{
		MAXNUM_LIGHT = 8,	//	Maximum number of A3D Light
	};

	struct GAMEINIT
	{
		HINSTANCE	hInstance;
		HWND		hWnd;
		bool		bFullscreen;
		int			iRenderWid;
		int			iRenderHei;
		char		szIP[256];
		int			iPort;
		char		szServerName[256];
		int			iServerID;
		int			iLine;
	};
	
	struct OBJECT_COORD
	{
		ACString strMap;
		A3DVECTOR3 vPos;
		bool operator == (const ACString& rhsStr) const {return strMap == rhsStr;}
	};	
	typedef abase::vector<OBJECT_COORD> ObjectCoords;

	// consume reward means consume gold in shop, then return dividend points
	struct CONSUME_REWARD
	{
		bool bOpen;
		int  iBeginTime[4];	// year, month, day, hour
		int  iEndTime[4];
		int  iRewardType[7][2];
		int  iRewardTime[2];
	};

	typedef abase::hashtab<BYTE, int, abase::_hash_function> ItemExtPropTable;
	typedef abase::hashtab<int, int, abase::_hash_function> SuiteEquipTable;
	typedef abase::hashtab<BYTE, int, abase::_hash_function> ItemColTable;
	typedef std::set<int> PetAutoSkillTable;

	typedef std::multimap<ACString, OBJECT_COORD> CoordTable;

public:	//	Constructor and Destructor

	CECGame();
	virtual ~CECGame();

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(const GAMEINIT& GameInit);
	//	Release game
	void Release();
	//	Reset game, release all resources
	bool Reset();

	//	Run game
	int RunInRenderThread();
	bool RunInMainThread();
	bool OnceRun();

	//	Window message handler
	bool WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//	Output a run-time debug string
	void RuntimeDebugInfo(DWORD dwCol, const ACHAR* szMsg, ...);

	//	Dispatch windows message
	bool DispatchWindowsMessage();
	//	Add windows message
	void AddWindowsMessage(MSG& msg);
	bool PreTranslateMessage(MSG &msg);
	
	//	Resources control
	bool LoadLoginRes();
	void ReleaseLoginRes();
	bool LoadInGameRes();
	void ReleaseInGameRes();

	//	Create a A3D Light object
	A3DLight* CreateA3DLight(const A3DLIGHTPARAM* pParams);
	//	Release Light object
	void ReleaseA3DLight(A3DLight* pLight);
	//	Load A3D skin model
	A3DSkinModel* LoadA3DSkinModel(const char* szFile, int iSkinFlag=0);
	//	Release A3D skin model
	void ReleaseA3DSkinModel(A3DSkinModel* pModel);
	//	Load A3D skin
	A3DSkin* LoadA3DSkin(const char* szFile, bool bUnique=true);
	//	Release A3D skin
	void ReleaseA3DSkin(A3DSkin* pSkin, bool bUnique=true);
	//	Load a building model from file
	CELBuilding* LoadBuilding(const char* szFile);
	//	Release a building model
	void ReleaseBuilding(CELBuilding* pBuilding);
	//	Show or hide cursor
	void ShowCursor(bool bShow);
	//	Change current cursor
	int ChangeCursor(int iCursor);
	//	Get current cursor
	int GetCurCursor() { return m_iCurCursor; }
	//	Get font object
	A3DFont* GetFont(int iIndex);
	//	Get item's extend description string
	const wchar_t* GetItemExtDesc(int tid);
	//	Get buff description string
	const wchar_t* GetBuffDesc(int id);
	//	Get item name color
	int GetItemNameColorIdx(int tid, int iDefIndex=0);
	DWORD GetItemNameColor(int tid, int iDefIndex=0);
	//	Reset privilege
	bool ResetPrivilege(void* pBuf, int iDataSize);
	//	Render high-light model
	bool RenderHighLightModel(CECViewport* pView, CECModel* pModel);
	bool RenderHighLightModel(CECViewport* pView, A3DSkinModel* pModel);
	//	Get dynamic object path
	const char* GetDynObjectPath(DWORD dwDynObjID);

	//  Get object coordinates
	int GetObjectCoord(ACString strTargetID, abase::vector<OBJECT_COORD>& TargetCoord);

	//  Check whether the specified skill the pet's auto skill
	bool IsPetAutoSkill(int skill_id) const;

	ACString	GetFormattedPrice(__int64 i);

	//	Get interfaces
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }
	A3DLight* GetDirLight() { return m_pDirLight; }
	A3DLight* GetDynamicLight() { return m_pDynamicLight; }
	CECViewport* GetViewport() { return m_pViewport; }
	CECConfigs* GetConfigs() { return m_pConfigs; }
	CECGameRun* GetGameRun() { return m_pGameRun; }
	CECGameSession* GetGameSession() { return m_pGameSession; }
	CECRTDebug* GetRTDebug() { return m_pRTDebug; }
	A3DGFXExMan* GetA3DGFXExMan() { return m_pA3DGFXExMan; }
	CECModelMan* GetECModelMan() { return m_pECModelMan; }
	CECShadowRender* GetShadowRender() { return m_pShadowRender; }
	CECPortraitRender* GetPortraitRender() { return m_pPortraitRender; }
	CECStringTab* GetFixedMsgTab() { return &m_FixedMsgs; }
	CECStringTab* GetItemDesc() { return &m_ItemDesc; }
	CECStringTab* GetItemExtDesc() { return &m_ItemExtDesc; }
	CECStringTab* GetSkillDesc() { return &m_SkillDesc; }
	CECStringTab* GetBuffDesc() { return &m_BuffDesc; }
	CECGFXCaster* GetGFXCaster() { return m_pGFXCaster; }
	elementdataman * GetElementDataMan() { return m_pElementDataMan; }
	itemdataman * GetItemDataMan() { return m_pItemDataMan; }
	ATaskTemplMan* GetTaskTemplateMan() { return m_pTaskMan; }
	CECImageRes* GetImageRes() { return m_pImageRes; }
	A3DSkinRender* GetSkinRender1() { return m_pSkinRender1; }
	A3DSkinRender* GetSkinRender2() { return m_pSkinRender2; }
	A3DLitModelRender* GetLitModelRender1() { return m_pLitModelRender1; }
	CELBackMusic * GetBackMusic() { return m_pBackMusic; }
	CECSoundCache* GetSoundCache() { return m_pSoundCache; }
	GNET::Privilege* GetPrivilege() { return m_pPrivilege; }	
	AMSoundBufferMan* GetAMSoundBufferMan();
	CECFactionMan* GetFactionMan() { return m_pFactionMan; }
	DataPathMan* GetDynObjPathTab() { return m_pDynObjPath; }
	ItemExtPropTable& GetItemExtPropTable() { return m_ItemExtProps; }
	SuiteEquipTable& GetSuiteEquipTable() { return m_SuiteEquipTab; }
	SuiteEquipTable& GetSuiteFashionTable() { return m_SuiteFashionTab; }
	CONSUME_REWARD& GetConsumeReward()	{ return m_ConsumeReward; }

	//	Get game version string
	const char* GetVersionString() { return m_strAllVersion; }
	//	Get game version
	DWORD GetGameVersion();
	//	Get game build info
	DWORD GetGameBuild();
	//	Get active state
	bool IsActive() { return m_bActive; }
	//	Get initial data
	GAMEINIT& GetGameInit() { return m_GameInit; }
	//	Get logic tick time of current frame
	DWORD GetTickTime() { return m_dwTickTime; }
	//	Get real tick time of current frame
	DWORD GetRealTickTime() { return m_dwRealTickTime; }
	//	Get average frame rate
	float GetAverageFrameRate() { return m_fAverFRate; }
	//	Time error with server
	int GetTimeError() { return m_iTimeError; }
	//  Time zone bias between UTC and local time in minutes
	int GetTimeZoneBias() { return m_iTimeZoneBias; }
	// Set the time of the server
	void SetServerTime(int iSevTime, int iTimeZoneBias);
	//	Get server GMT(UTC) time
	int GetServerGMTTime();
	//  Get server GMT(UTC) time directly (without adding local time bias)
	int	GetServerAbsTime();
	//	Get server local time
	struct tm GetServerLocalTime();
	struct tm GetServerLocalTime(int t);
	//	Discard current frame
	void DiscardFrame() { m_bDiscardFrame = true; }
    //  Set tray icon hint
	void SetTrayIconHint(ACString StrHint);

	//	设置各种声音的音量
	void SetAllVolume(const EC_SYSTEM_SETTING &ss);
	//	自动根据当前状态、设置音量
	void SetAllVolume();

protected:	//	Attributes

	A3DEngine*			m_pA3DEngine;
	A3DDevice*			m_pA3DDevice;
	A3DLight*			m_pDirLight;		//	Directional light
	A3DLight*			m_pDynamicLight;	//	Dynamic point light
	CECViewport*		m_pViewport;		//	Main viewport
	CECConfigs*			m_pConfigs;			//	Configs
	CECGameRun*			m_pGameRun;			//	Game run object
	CECGameSession*		m_pGameSession;		//	Game session
	CECRTDebug*			m_pRTDebug;			//	Run-Time debug object
	A3DGFXExMan*		m_pA3DGFXExMan;		//	GFX object manager
	CECModelMan*		m_pECModelMan;		//	ECModel manager
	CECShadowRender*	m_pShadowRender;	//	Shadow renderer object
	CECPortraitRender*	m_pPortraitRender;	//	Portrait renderer object
	CECGFXCaster*		m_pGFXCaster;		//	GFX caster
	CECStringTab		m_FixedMsgs;		//	Fixed message table
	CECStringTab		m_ItemDesc;			//	Item desciption string table
	CECStringTab		m_ItemExtDesc;		//	Item extend description string table
	CECStringTab		m_SkillDesc;		//	Skill description string table
	CECStringTab		m_BuffDesc;			//	Buff description string table
	ItemExtPropTable	m_ItemExtProps;		//	Item extend properties table
	ItemColTable		m_ItemColTab;		//	Item color table	
	CoordTable			m_CoordTab;			//  Object(NPC,maps, etc) coordinates table
	CONSUME_REWARD		m_ConsumeReward;	//  Current consume reward configs
	itemdataman *		m_pItemDataMan;		//	global templates manager
	elementdataman *	m_pElementDataMan;	//	global element templates manager
	ATaskTemplMan*		m_pTaskMan;			//	Task template manager
	CECImageRes*		m_pImageRes;		//	Image resources
	A3DSkinRender*		m_pSkinRender1;		//	Skin render 1
	A3DSkinRender*		m_pSkinRender2;		//	Skin render 2
	CELBackMusic *		m_pBackMusic;		//	Back ground music
	CECSoundCache*		m_pSoundCache;		//	Sound cache
	GNET::Privilege*	m_pPrivilege;		//	Client privilege
	CECFactionMan*		m_pFactionMan;		//	Faction manager
	DataPathMan*		m_pDynObjPath;		//	Dynamic object path table
	SuiteEquipTable		m_SuiteEquipTab;	//	Suite equipment table
	SuiteEquipTable		m_SuiteFashionTab;
	A3DLitModelRender*	m_pLitModelRender1;	//	Backup lit model render
	PetAutoSkillTable	m_PetAutoSkills;	//  Pet Auto skill table

	CRITICAL_SECTION	m_csMsg;
	AList2<MSG, MSG&>	m_MsgList;

	GAMEINIT		m_GameInit;			//	Game init parameters
	bool			m_bActive;			//	true, Run game
	DWORD			m_dwTickTime;		//	Logic time of current tick
	DWORD			m_dwRealTickTime;	//	Real tick time
	bool			m_bDiscardFrame;	//	true, discard this frame
	float			m_fAverFRate;		//	Average frame rate
	int				m_iCurCursor;		//	Current cursor
	int				m_iTimeError;		//	Time error with server
	int				m_iTimeZoneBias;	//	Server time zone bias
	
	int				m_AbsTimeStart;
	DWORD			m_AbsTickStart;

	A3DLight*		m_aLights[MAXNUM_LIGHT];

	APtrArray<A3DCursor*>	m_aCursors;			//	Cursor objects
	APtrArray<A3DFont*>		m_aFonts;			//	Fonts array
	AString					m_strAllVersion;	//  All version's string

	NOTIFYICONDATA			m_nid;				//  Notify icon data  

	//	高亮显示模型相关
	//	高亮显示的模型，以前采取修改 Emissive 颜色、立即渲染、恢复 Emissive 颜色的方式
	//	但在 Alpha 处理上频频遇到问题，根本原因是跟非高亮显示情况下的先渲染非 Alpha 模型、
	//	再排序渲染 Alpha 模型的原则不相符。因此，修改高亮显示方式为修改 Emissive 颜色、非立即渲染、所有渲染完成后一起恢复的策略
	struct HighlightRecord								//	每次高亮显示的记录，用于恢复高亮显示前状态
	{
		A3DSkinModel *		pSkinModel;			//	本次渲染过程中高亮显示的模型
		A3DCOLORVALUE	lastEmissive;		//	高亮显示前的 emissive 颜色值
		HighlightRecord(A3DSkinModel *pModel, A3DCOLORVALUE col)
			: pSkinModel(pModel), lastEmissive(col)
		{}
	};
	typedef abase::vector<HighlightRecord>	HighlightRecords;
	HighlightRecords	m_HighlightRecords;
public:
	void PrepareHighlightModel();
	void RestoreHightlightModel();

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D engine
	void ReleaseA3DEngine();
	//	Load cursors
	bool LoadCursors();
	//	Release cursors
	void ReleaseCursors();
	//	Create fonts
	bool CreateFonts();
	//	Release fonts
	void ReleaseFonts();
	//	Load item extend properties
	bool LoadItemExtProps();

	//  Load objects coordinates data
	bool LoadObjectCoord();
	//	Build suite equipment table
	void BuildSuiteEquipTab();

	//  Load consume reward data
	bool LoadConsumeReward();
	//  Load pet auto skill data
	bool LoadPetAutoSkill();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

