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
		unsigned int end_time; //(��/��/��/ʱ/��/��)-���Ϊ0���������Ч
		unsigned int time; //���ʱ�䣨�룬0��ʾ���ڣ�
		unsigned int start_time;//����ʱ�䣺(��/��/��/ʱ/��/��)-���Ϊ0�����ڽ�����Чʱ���ǰ����Ч		
		int type; //ʱ�����ͣ�0����ʱ�䣬1ÿ�ܣ�2ÿ��, -1 ��Ч
		unsigned int day; //��λ��ʾ�Ƿ�ѡ����ĳһ�죬�ɱ�ʾ��Ҳ�ɱ�ʾ�£��ɵ͵���
		unsigned int status; //��Ʒ״̬��0�ޣ�1��Ʒ��2������3�Ƽ���4-12Ϊ1~9�ۣ�13����
		unsigned int flag; //�������
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
		unsigned int end_time; //(��/��/��/ʱ/��/��)-���Ϊ0���������Ч
		unsigned int time; //���ʱ�䣨�룬0��ʾ���ڣ�
		unsigned int start_time;//����ʱ�䣺(��/��/��/ʱ/��/��)-���Ϊ0�����ڽ�����Чʱ���ǰ����Ч		
		int type; //ʱ�����ͣ�0����ʱ�䣬1ÿ�ܣ�2ÿ��, -1 ��Ч
		unsigned int day; //��λ��ʾ�Ƿ�ѡ����ĳһ�죬�ɱ�ʾ��Ҳ�ɱ�ʾ�£��ɵ͵���
		unsigned int status; //��Ʒ״̬��0�ޣ�1��Ʒ��2������3�Ƽ���4-12Ϊ1~9�ۣ�13����
		unsigned int flag; //�������
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

//	��ս������
enum Domain2Status
{
	D2S_NORMAL,		//	����ģʽ
	D2S_NEARWAR,	//	����ս��
	D2S_ATWAR,		//	ս����
	D2S_AFTERWAR,	//	ս����ȴ
};

typedef struct _DOMAIN2_INFO
{
	int							id;			//	����ID
	wide_char					name[16];	//	��������

	int							point;		//	����
	int							wartype;	//	ս������
	int							countryid;	//	��ʼ������Ӫ
	int							iscapital;	//	�Ƿ�����Ӫ�׶�
	A3DVECTOR3					mappos[4];	//	��Ӧ�ع�����άλ��
	
	abase::vector<A3DPOINT2>	verts;		//	��������λ�ã���ͼ����ռ䣩
	abase::vector<int>			faces;		//	������ʾ��Ƭ
	A3DPOINT2					center;		//	�������ģ���ͼ����ռ䣩
	abase::vector<int>			neighbours;	//	�ڽ�����ID
	abase::vector<int>			time_neighbours;	//	�ƶ����ڽ�����ʱ�䣨�룩
	
	enum {COUNTRY_COUNT = 4};				//	������Ŀ

	enum
	{
		DOMAIN_STATE_NUM = 4,
	};

	enum
	{
		STATE_NONE = 0,
		STATE_SMALLTOKEN = 0x01,	// С��Ԯ��
		STATE_BIGTOKEN = 0x02,		// ��Ԯ��
		STATE_RESTRICT = 0x80,		// ��������ս����������
	};

	int							owner;						//	ӵ���߹���ID
	int							challenger;					//	��ս�߹���ID
	Domain2Status				status;						//	������ǰ״̬
	DWORD						end_time;					//	����״̬�����֣�ʣ��ʱ��
	int							player_count[COUNTRY_COUNT];//	�������ڴ�����������

	BYTE byStateMask;				// ս��״̬����С��Ԯ�ս������������
	abase::vector<int> iGhost;		// ���������Ļ�����������
	abase::vector<int> iPlayer;		// ���������������������
	
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
		return iscapital && !IsNormal()		//	�׶�����ս��
			|| owner == 0					//	�˵�����ӵ��
			|| IsChallenged() && !IsNearWar() && !IsAtWar()		//	������սȴ����ս��״̬
			|| !IsChallenged() && IsNearWar() && IsAtWar();		//	������սȴ��ս��״̬
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
		//	idCountry ��������ս��������ʱ���� true
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return IsInvolved(idCountry)
			&& (IsNearWar() || IsAtWar())
			&& player_count[idCountry-1] >= nPlayerLimit;
	}

	bool IsCoolDownFor(DWORD now, DWORD elapsed, int idCountry)const{
		//	�� now ��ʱ�𣬾��� elapsed ��������Զ��������� idCountry ��ȴʱ���� true
		ASSERT(idCountry > 0 && idCountry <= COUNTRY_COUNT);
		return !IsOwner(idCountry) && IsAfterWar() &&
			elapsed < GetLeftTime(now);
	}

} DOMAIN2_INFO;

typedef struct _DOMAIN2_INFO_SERV
{
	int						id;
	int						point;			//	����
	int						wartype;		//	ս������
	int						countryid;		//	��ʼ������Ӫ
	int						iscapital;		//	�Ƿ�����Ӫ�׶�
	float					mappos[4][3];	//	��Ӧ�ع�����άλ��
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
