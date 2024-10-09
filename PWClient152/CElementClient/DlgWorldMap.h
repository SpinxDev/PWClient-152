// Filename	: DlgWorldMap.h
// Creator	: Tom Zhou
// Date		: October 16, 2005

#pragma once

#include "hashmap.h"
#include "DlgBase.h"
#include "AUIImagePicture.h"


// 世界
#define MAJOR_MAP 1
// 仙魔
#define GOOD_EVIL_MAP(id)	(id == 121 || id == 122)
// 城战
#define GUILD_WAR_MAP(id)	(id >= 230 && id <= 235)
// 蓬莱
#define PENGLAI_MAP(id)		(id == 137)

#define IS_MAP_MARKABLE(mapID) \
		( MAJOR_MAP == mapID || GOOD_EVIL_MAP(mapID) || \
		GUILD_WAR_MAP(mapID) || PENGLAI_MAP(mapID) )	\
//

class A3DTexture;
struct NPC_TRANSMIT_SERVICE;
struct NPC_ESSENCE;
class CDlgWorldMap : public CDlgDynamicControl  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWorldMap();
	virtual ~CDlgWorldMap();
	virtual void OnTick();
	virtual bool Render(void);

	void OnCommand_makemark(const char * szCommand);
	void OnCommand_edit(const char * szCommand);
	void OnCommand_delete(const char * szCommand);
	void OnCommand_deleteall(const char * szCommand);
	void OnCommand_radar(const char * szCommand);
	void OnCommand_arrow(const char * szCommand);
	void OnCommand_inc(const char * szCommand);
	void OnCommand_dec(const char * szCommand);
	void OnCommand_guildmap(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_SwitchTaskShow(const char * szCommand);

	void OnEventLButtonUp_Flag(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblClk_Flag(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Control(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_World(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblClk_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Detail(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Travel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Sign(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void BuildTravelMap(DWORD dataType, void* pData);
	int GetTravelWorldID();
	void AddAutoMoveButton(const ACString& hint, const char* image, const A3DVECTOR3& pos, DWORD data = 0);
	enum {
		TASK_TYPE_ONE = -1,
		TASK_TYPE_NONE,
		TASK_TYPE_FINISHED,
		TASK_TYPE_UNFINISHED,
		TASK_TYPE_ALL,
		TASK_TYPE_TODO,
		TASK_TYPE_NUM,
	};
	void SetTaskShowType(int type) { m_iTaskShowType = type; }

protected:
	PAUIIMAGEPICTURE m_pImgHint;
	int				 m_iTaskShowType;
	const NPC_ESSENCE *m_pCurNPCEssence;		//	传送服务时启动时保存的传送 NPC
	const NPC_ESSENCE *GetCurNPCEssence(){ return m_pCurNPCEssence; }

	void BringUpTools();
	bool UpdateWorldMap();
	bool UpdateWorldMapDetail();
	bool UpdateWorldMapTravel();
	bool UpdateRandomMap(int iInst);

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	void OnFlagSelected();
	void UpdateDetailMapControlPos();
	void UpdateTaskControl();

	void RenderFactionPVPIcon(const void *posConverter
		, A2DSprite *pA2DSpriteMineBase, A2DSprite *pA2DSpriteMineBaseForMyFaction
		, A2DSprite *pA2DSpriteMine, A2DSprite *pA2DSpriteMineForMyFaction);
};

class CECNPCTransmitCache;
class CECMapDlgsMgr
{
public:
	CECMapDlgsMgr();
	~CECMapDlgsMgr();

	bool Init();

	// resize a dialog
	void ResizeFullScreenDialog(PAUIDIALOG pDlg, int iWidth, int iHeight);
	// resize all the map dialogs
	void ResizeWorldMap(A3DRECT *prc = NULL);

	// refresh flags on all map dialogs
	void RefreshWorldMapFlags(bool bCloseAll);

	// init detail map by specific position
	bool InitDetailMap(float fX, float fZ);
	// refresh texture in detail map 
	void RefreshDetailMap();
	// clear texture on detail map
	void ClearDetailMap();

	// convert position between screen and game world
	A3DPOINT2 GetPixelsByPos(float x, float y, int worldid);
	APointF GetPosByPixels(int x, int y, int worldid);
	struct PosConvertParam
	{
		int a;      // 屏幕上的地图横向能容下a个方块，纵向b个
		int b;
		float ox;   // 地图中心点对应的世界坐标为(ox,oy)
		float oy;
		int w;      // 地图实际占用分辨率 w*h
		int h;
	};
	void GetPosConvertParam(int worldid, PosConvertParam& param);
	bool IsMapCanShowHostPos(int worldid);
	struct PosConvertor
	{
		// convert map logic position to screen pixel
		virtual A3DPOINT2 operator()(const A3DVECTOR3& vecPos) const = 0;
		// convert screen pixel to map logic position
		virtual A3DVECTOR3 operator()(const A3DPOINT2& ret) const = 0;
	};
	// draw team member mark 
	void DrawTeamMark(const PosConvertor&);

	// draw players way-points and travel lines
	void DrawWayPoints(const NPC_ESSENCE *pNPC, const A3DPOINT2& ptCenter, int iWorldID, bool bDrawLines);

	// draw host player mark
	void DrawHostMark(const PosConvertor& conv);

	// draw the auto move flag
	void DrawAutoMoveFlag(const PosConvertor& conv);

	// update player way-points
	void UpdateWayPoints(WORD* pData, int iSize, bool bClear);

	// find world within NPC_TRANSMIT_SERVICE
	int FindTransmitWorldID(NPC_TRANSMIT_SERVICE* pData);
	int FindTransmitWorldID(int idTarget);

	// get current mode
	bool GetWorldMapMode() const { return m_WideMode; }

	// load world map (may change the WorldMapMode)
	enum MAP_TYPE { MAP_TYPE_NORMAL, MAP_TYPE_TRANSMIT, MAP_TYPE_SCROLL, };
	bool LoadMapTexture(A3DDevice* pDevice, PAUIIMAGEPICTURE pImage, MAP_TYPE type);
	AString GetMapTexture(MAP_TYPE type);

	bool IsWorldMapShown();
	void ShowWorldMap(bool bShow);
	void SwitchWorldMapShow();

	// get proper pos convertor for current map
	PosConvertor* GetCurConvertor();


public:
	struct TRANS_POINT
	{
		int id;
		int worldid;
		ACString strName;
		A3DVECTOR3 vecPos;
	};
	typedef APtrList<TRANS_POINT*>	TransWayPoints;

	class TransWay
	{
	public:
		enum TransMode
		{
			TM_INVALID = 0,
			TM_TRANSMITSCROLL = 1,				//	通过传送卷轴传送
			TM_NPC_TRANSMIT_NORMAL = 2,	//	通过 NPC 传送服务、向当前 NPC 连接点传送
			TM_NPC_TRANSMIT_DIRECT = 3,	//	通过 NPC 传送服务、向当前 NPC 可达点直接传送
		};
		TransMode			mode;			//	发送传送协议时应使用的传送模式（TM_NPC_TRANSMIT_NORMAL =2 与 wayPoints 个数大于1可能同时成立）
		int							nCost;			//	需要的完美币
		int							nLevel;			//	需求的等级
		TransWayPoints	wayPoints;	//	传送路线（不含起点、包括终点）（各模式下均可用于显示，TM_NPC_TRANSMIT_DIRECT 模式下才可用于协议）

		TransWay(){ Reset(); }

		void Reset()
		{
			mode = TM_INVALID;
			nCost = 0;
			nLevel = 0;
			wayPoints.RemoveAll();
		}

		bool IsValid()const{ return mode != TM_INVALID; }

		static bool HasDirectTransmitItem();
	};

	typedef abase::vector<TRANS_POINT> Points;
	typedef abase::hash_map<int, TRANS_POINT> PointMap;

	enum
	{
		WAYPOINT_ME = 0,
		WAYPOINT_TARGET
	};

	enum
	{
		MINIMAP_UL = 0,
		MINIMAP_UR,
		MINIMAP_LL,
		MINIMAP_LR,
		MINIMAP_MAX
	};

protected:
	A3DPOINT2 m_idxCorner[MINIMAP_MAX];

	int m_nCurFlagIndex;
	bool m_bMarking;
	bool m_bMapDragging;
	A2DSprite *m_pA2DSpriteWaypoint;
	A2DSprite *m_pA2DSpriteProfession;
	Points m_vecTeamMate;
	PointMap m_transPoints;
	abase::hash_map<AString, A3DTexture *> m_MapMap;
	typedef abase::hash_map<int, PosConvertParam*> MAP_PARAM;
	MAP_PARAM m_MapParam;

	int m_nMinSize;
	bool m_WideMode;

	CECNPCTransmitCache* m_pTransCache;

	TransWay			m_transWay;		//	到当前鼠标所在传送点的传送路径
	struct DefaultData
	{
		POINT p;
		SIZE s;
		int FontSize;

		DefaultData(POINT pp, SIZE ss, int fontsize):p(pp), s(ss), FontSize(fontsize){}
	};
	typedef abase::hash_map<PAUIDIALOG, abase::vector<DefaultData> > DlgDefault;
	DlgDefault m_DefaultPosAndSize;
	const abase::vector<DefaultData>& GetDlgDefaultData(PAUIDIALOG pDlg);
	void ResizeDlgKeepingMapRadio(PAUIDIALOG pDlg, PAUIOBJECT pMap, int iDlgWid, int iDlgHt, int iTargetW, int iTargetH, int iOffsetX, int iOffsetY);

	void RefreshDetailMapTexture();

	// set current mode
	void SetWorldMapMode(bool isWide);

	bool UpdateTransWayWithinNPC(const NPC_ESSENCE *pNPC, int idTarget);

public: // handle properties

	int GetCurMark();
	PAUIIMAGEPICTURE GetCurFlagImage();
	void SetCurFlagIndex(int val);
	
	bool IsMarking() const { return m_bMarking; }
	void SetMarking(bool val) { m_bMarking = val; }
	
	bool IsDraging() const { return m_bMapDragging; }
	void SetDraging(bool val) { m_bMapDragging = val; }

	const Points& GetTeamMate() const { return m_vecTeamMate; }
	const PointMap& GetTransPoint() const { return m_transPoints; }
	const A3DPOINT2& GetCorner(int index) { return m_idxCorner[index]; }
	const CECNPCTransmitCache& GetCache() const { return *m_pTransCache; }

	//	transmit way
	bool UpdateTransWay(const NPC_ESSENCE *pNPC, int idTarget);
	void ResetTransWay(){m_transWay.Reset();}
	const TransWay & GetTransWay(){return m_transWay;}
	
	// draw the inner way points
	void DrawTransWay(const A3DPOINT2& ptCenter);

	// adjust the size of a map texture
	void AdjustMapTexture(PAUIIMAGEPICTURE pImage, int defaultWidth, const SIZE& bgSize, const POINT& offset);
};
