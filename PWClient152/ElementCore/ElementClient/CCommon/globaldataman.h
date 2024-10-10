/*
 * FILE: globaldataman.h
 *
 * DESCRIPTION: global data loader and manager
 *
 * CREATED BY: Hedi, 2005/7/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _GLOBALDATAMAN_H_
#define _GLOBALDATAMAN_H_

#ifdef _WINDOWS
typedef wchar_t wide_char;
#else
typedef unsigned short wide_char;
#endif

#include "A3DTypes.h"
#include "vector.h"
#include "AAssist.h"

// Data and methods for transport
typedef struct _TRANS_TARGET
{
	int			id;
	wide_char		name[32];
	int			world_id;
	A3DVECTOR3	vecPos;
	int			domain_id;

} TRANS_TARGET;

typedef struct _TRANS_TARGET_SERV
{
	int			id;
	int			world_id;
	A3DVECTOR3	vecPos;
	int			domain_id;

} TRANS_TARGET_SERV;

abase::vector<TRANS_TARGET> * globaldata_gettranstargets();
abase::vector<TRANS_TARGET_SERV> * globaldata_gettranstargetsserver();

// data and methods for domain
enum DOMAIN_TYPE
{
	DOMAIN_TYPE_NULL = 0,
	DOMAIN_TYPE_3RD_CLASS,
	DOMAIN_TYPE_2ND_CLASS,
	DOMAIN_TYPE_1ST_CLASS,
};

typedef struct _DOMAIN_INFO
{
	int						id;				// id of the domain
	DOMAIN_TYPE				type;			// type of the domain
	int						reward;			// money rewarded per week
	wide_char					name[16];		// name of the domain in Unicode

	abase::vector<A3DPOINT2>		verts;			// verts of the surrounding polygon
	abase::vector<int>				faces;			// faces of the surrounding polygon

	A3DPOINT2				center;			// center of the domain, it is the place we put the icon of the faction
	abase::vector<int>				neighbours;		// neighbours of this domain

	// these members will be set at run time
	int						id_owner;		
	int						id_challenger;
	int						battle_time;
	int						id_tempchallenger;
	int						deposit;
	int						maxbonus;
	int						cutoff_time;
	char					color;

} DOMAIN_INFO;

typedef struct _DOMAIN_INFO_SERV
{
	int						id;				// id of the domain
	DOMAIN_TYPE				type;			// type of the domain
	int						reward;			// money rewarded per week
	abase::vector<int>				neighbours;		// neighbours of this domain

} DOMAIN_INFO_SERV;

abase::vector<DOMAIN_INFO> * globaldata_getdomaininfos();
abase::vector<DOMAIN_INFO_SERV> * globaldata_getdomaininfosserver();

enum {
	TREASURE_ITEM_OWNER_NPC_COUNT = 8,
};

#pragma pack(push, GSHOP_ITEM_PACK, 1)
#if defined _ELEMENTCLIENT || defined _ELEMENTLOCALIZE || defined _ELEMENT_EDITOR_
typedef struct _GSHOP_ITEM
{
	int						local_id;		// id of this shop item, used only for localization purpose
	int						main_type;		// index into the main type array
	int						sub_type;		// index into the sub type arrray
	
	char					icon[128];		// icon file path

	unsigned int			id;				// object id of this item
	unsigned int			num;			// number of objects in this item

	struct {
		unsigned int		price;			// price of this item	
		unsigned int end_time; //(年/月/日/时/分/秒)-如果为0，则持续生效
		unsigned int time; //租借时间（秒，0表示无期）
		unsigned int start_time;//开启时间：(年/月/日/时/分/秒)-如果为0，则在结束生效时间点前都生效		
		int type; //时间类型，0持续时间，1每周，2每月, -1 无效
		unsigned int day; //由位表示是否选择了某一天，可表示周也可表示月，由低到高
		unsigned int status; //物品状态，0无，1新品，2促销，3推荐，4-12为1~9折，13闪购
		unsigned int flag; //分类控制
	} buy[4];
	
	wide_char					desc[512];		// simple description
	wide_char					szName[32];		// name of this item to show
	unsigned int idGift;
	unsigned int iGiftNum;
	unsigned int iGiftTime;
	unsigned int iLogPrice;
	unsigned int owner_npcs[TREASURE_ITEM_OWNER_NPC_COUNT];

} GSHOP_ITEM;
#else
typedef struct _GSHOP_ITEM
{
	int						local_id;		// id of this shop item, used only for localization purpose
	int						main_type;		// index into the main type array
	int						sub_type;		// index into the sub type arrray

	unsigned int			id;				// object id of this item
	unsigned int			num;			// number of objects in this item

	struct {
		unsigned int		price;			// price of this item	
		unsigned int end_time; //(年/月/日/时/分/秒)-如果为0，则持续生效
		unsigned int time; //租借时间（秒，0表示无期）
		unsigned int start_time;//开启时间：(年/月/日/时/分/秒)-如果为0，则在结束生效时间点前都生效		
		int type; //时间类型，0持续时间，1每周，2每月, -1 无效
		unsigned int day; //由位表示是否选择了某一天，可表示周也可表示月，由低到高
		unsigned int status; //物品状态，0无，1新品，2促销，3推荐，4-12为1~9折，13闪购
		unsigned int flag; //分类控制
	} buy[4];
	unsigned int idGift;
	unsigned int iGiftNum;
	unsigned int iGiftTime;
	unsigned int iLogPrice;
	unsigned int owner_npcs[TREASURE_ITEM_OWNER_NPC_COUNT];

} GSHOP_ITEM;
#endif

typedef struct _GSHOP_MAIN_TYPE
{
	wide_char					szName[64];		// name of this main type
	abase::vector<AWString>		sub_types;		// sub type name of this main type
} GSHOP_MAIN_TYPE;
#pragma pack(pop, GSHOP_ITEM_PACK)

DWORD globaldata_getgshop_timestamp();
abase::vector<GSHOP_ITEM> * globaldata_getgshopitems();
abase::vector<GSHOP_MAIN_TYPE> * globaldata_getgshopmaintypes();

DWORD globaldata_getgshop_timestamp2();
abase::vector<GSHOP_ITEM> * globaldata_getgshopitems2();
abase::vector<GSHOP_MAIN_TYPE> * globaldata_getgshopmaintypes2();

//	国战各领土
enum Domain2Status
{
	D2S_NORMAL,		//	正常模式
	D2S_NEARWAR,	//	即将战斗
	D2S_ATWAR,		//	战斗中
	D2S_AFTERWAR,	//	战后冷却
};

typedef struct _DOMAIN2_INFO
{
	int							id;			//	领土ID
	wide_char					name[16];	//	领土名称

	int							point;		//	积分
	int							wartype;	//	战争类型
	int							countryid;	//	初始归属阵营
	int							iscapital;	//	是否是阵营首都
	A3DVECTOR3					mappos[4];	//	对应地宫中三维位置
	
	abase::vector<A3DPOINT2>	verts;		//	领土顶点位置（地图纹理空间）
	abase::vector<int>			faces;		//	领土显示面片
	A3DPOINT2					center;		//	领土中心（地图纹理空间）
	abase::vector<int>			neighbours;	//	邻接领土ID
	abase::vector<int>			time_neighbours;	//	移动到邻接领土时间（秒）
	
	enum {COUNTRY_COUNT = 4};				//	国家数目

	enum
	{
		DOMAIN_STATE_NUM = 4,
	};

	enum
	{
		STATE_NONE = 0,
		STATE_SMALLTOKEN = 0x01,	// 小疾援令
		STATE_BIGTOKEN = 0x02,		// 大疾援令
		STATE_RESTRICT = 0x80,		// 有无设置战场限制条件
	};

	int							owner;						//	拥有者国家ID
	int							challenger;					//	挑战者国家ID
	Domain2Status				status;						//	领土当前状态
	DWORD						end_time;					//	领土状态（部分）剩余时间
	int							player_count[COUNTRY_COUNT];//	各国家在此领土中人数

	BYTE byStateMask;				// 战场状态（大小疾援令，战场进入条件）
	abase::vector<int> iGhost;		// 进入该区域的魂力限制条件
	abase::vector<int> iPlayer;		// 进入该区域的玩家人数限制
	
	void ClearState(){
		owner = 0;
		challenger = 0;
		status = D2S_NORMAL;
		end_time = 0;
		memset(player_count, 0, sizeof(player_count));

		byStateMask = STATE_NONE;
		iGhost.clear();
		iPlayer.clear();
	}

	bool IsInvalid()const{
		return iscapital && !IsNormal()		//	首都出现战争
			|| owner == 0					//	此地无人拥有
			|| IsChallenged() && !IsNearWar() && !IsAtWar()		//	有人挑战却不在战争状态
			|| !IsChallenged() && IsNearWar() && IsAtWar();		//	无人挑战却在战争状态
	}

	bool Validate()const{ return !IsInvalid(); }

	bool IsOwner(int idCountry)const{
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return idCountry == owner;
	}

	bool IsChallenger(int idCountry)const{
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return idCountry == challenger;
	}
	
	bool IsMyCapital(int idCountry)const{
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return iscapital && IsOwner(idCountry);
	}

	bool IsOthersCapital(int idMyCountry)const{
		ASSERT(idMyCountry > 0 && idMyCountry <= COUNTRY_COUNT);
		return iscapital && !IsOwner(idMyCountry);
	}

	bool IsInvolved(int idCountry)const{ return IsOwner(idCountry) || IsChallenger(idCountry);}
	bool IsChallenged()const{ return challenger != 0; }
	bool IsNormal()const{ return status == D2S_NORMAL; }
	bool IsNearWar()const{ return status == D2S_NEARWAR; }
	bool IsAtWar()const{ return status == D2S_ATWAR; }
	bool IsAfterWar()const{ return status == D2S_AFTERWAR; }
	bool HasEndTime()const{ return IsNearWar() || IsAfterWar(); }
	DWORD  GetLeftTime(DWORD now)const { return end_time > now ? (end_time-now) : 0; }

	bool IsFullFor(int idCountry, int nPlayerLimit)const{
		//	idCountry 方领土参战人数已满时返回 true
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return IsInvolved(idCountry)
			&& (IsNearWar() || IsAtWar())
			&& player_count[idCountry-1] >= nPlayerLimit;
	}

	bool IsCoolDownFor(DWORD now, DWORD elapsed, int idCountry)const{
		//	从 now 计时起，经过 elapsed 秒后领土仍对其它国家 idCountry 冷却时返回 true
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return !IsOwner(idCountry) && IsAfterWar() &&
			elapsed < GetLeftTime(now);
	}

} DOMAIN2_INFO;

typedef struct _DOMAIN2_INFO_SERV
{
	int						id;
	int						point;			//	积分
	int						wartype;		//	战争类型
	int						countryid;		//	初始归属阵营
	int						iscapital;		//	是否是阵营首都
	float					mappos[4][3];	//	对应地宫中三维位置
	abase::vector<int>		neighbours;
	abase::vector<int>		time_neighbours;
} DOMAIN2_INFO_SERV;

enum DOMAIN2_TYPE
{
	DOMAIN2_TYPE_SINGLE_SEV,
	DOMAIN2_TYPE_MULTI_SEV,
	DOMAIN2_TYPE_NUM,
};
abase::vector<DOMAIN2_INFO> * globaldata_getdomain2infos(int type);
DWORD globaldata_getdomain2_timestamp(int type);
abase::vector<DOMAIN2_INFO_SERV> * globaldata_getdomain2infosserver();

// common interface 
bool globaldata_load();
bool globaldata_save_to(const char * path);
bool globaldata_release();

bool globaldata_loadserver();
bool globaldata_releaseserver();

#endif//_GLOBALDATAMAN_H_
