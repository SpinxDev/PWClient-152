#ifndef __GNET_PRIVILEGE_H
#define __GNET_PRIVILEGE_H
	
#include <string.h>
#define PRIVILEGE_NUM 256	
	
namespace GNET
{
class Privilege
{
public:
	enum
	{
		PRV_TOGGLE_NAMEID		= 0,	//切换玩家名字与ID
		PRV_HIDE_BEGOD		= 1,	//进入隐身或无敌状态
		PRV_ONLINE_ORNOT		= 2,	//切换是否在线
		PRV_CHAT_ORNOT		= 3,	//切换是否可以密语
		PRV_MOVETO_ROLE		= 4,	//移动到指定角色身边
		PRV_FETCH_ROLE		= 5,	//将指定角色召唤到GM身边
		PRV_MOVE_ASWILL		= 6,	//移动到指定位置
		PRV_MOVETO_NPC		= 7,	//移动到指定NPC位置
		PRV_MOVETO_MAP		= 8,	//移动到指定地图（副本）
		PRV_ENHANCE_SPEED		= 9,	//移动加速
		PRV_FOLLOW		= 10,	//跟随玩家
		PRV_LISTUSER		= 11,	//获取在线玩家列表
		PRV_FORCE_OFFLINE		= 100,	//强制玩家下线，并禁止在一定时间上线
		PRV_FORBID_TALK		= 101,	//禁言
		PRV_FORBID_TRADE		= 102,	//禁止玩家间、玩家与NPC交易
		PRV_FORBID_SELL		= 103,	//禁卖
		PRV_BROADCAST		= 104,	//系统广播
		PRV_SHUTDOWN_GAMESERVER		= 105,	//关闭游戏服务器
		PRV_SUMMON_MONSTER		= 200,	//召唤怪物
		PRV_DISPEL_SUMMON		= 201,	//驱散被召唤物体
		PRV_PRETEND		= 202,	//伪装
		PRV_GMMASTER		= 203,	//GM管理员
		PRV_DOUBLEEXP		= 204,	//双倍经验
		PRV_LAMBDA		= 205,	//并发连接数
		PRV_ACTIVITYMANAGER		= 206,	//活动管理员
	};
private:	
	unsigned char privileges[PRIVILEGE_NUM];
public:
	void Init(const void* buf,size_t size)
	{
		Reset();
		const unsigned char* pPrv=(const unsigned char*) buf;
		for (size_t i=0;i<size;i++)
			SetPrivilege(pPrv[i]);
	}
	int GetBuf(void * buf, size_t size)
	{
		if(!size) return -1;
		unsigned char * pPrv = (unsigned char *)buf;
		size_t index = 0;
		for(size_t i= 0; i < PRIVILEGE_NUM; i ++)
		{
			if(privileges[i])
			{
				pPrv[index++] = i;
				if(index >= size) return -1;
			}
		}
		return (int)index;
	}	
	Privilege()
	{
		Reset();
	}
	void operator=(const Privilege& rhs)
	{
		memcpy(this->privileges,rhs.privileges,PRIVILEGE_NUM);
	}
	void Reset()
	{
		memset(privileges,0,PRIVILEGE_NUM);
	}
	void SetPrivilege(int prv_type)
	{
		if (prv_type>=0 && prv_type<PRIVILEGE_NUM)
			privileges[prv_type]=1;
	}	
	bool Has_Toggle_NameID()
	{
		return privileges[PRV_TOGGLE_NAMEID]==1;
	}
	bool Has_Hide_BeGod()
	{
		return privileges[PRV_HIDE_BEGOD]==1;
	}
	bool Has_Online_OrNot()
	{
		return privileges[PRV_ONLINE_ORNOT]==1;
	}
	bool Has_Chat_OrNot()
	{
		return privileges[PRV_CHAT_ORNOT]==1;
	}
	bool Has_MoveTo_Role()
	{
		return privileges[PRV_MOVETO_ROLE]==1;
	}
	bool Has_Fetch_Role()
	{
		return privileges[PRV_FETCH_ROLE]==1;
	}
	bool Has_Move_AsWill()
	{
		return privileges[PRV_MOVE_ASWILL]==1;
	}
	bool Has_MoveTo_NPC()
	{
		return privileges[PRV_MOVETO_NPC]==1;
	}
	bool Has_MoveTo_Map()
	{
		return privileges[PRV_MOVETO_MAP]==1;
	}
	bool Has_Enhance_Speed()
	{
		return privileges[PRV_ENHANCE_SPEED]==1;
	}
	bool Has_Follow()
	{
		return privileges[PRV_FOLLOW]==1;
	}
	bool Has_ListUser()
	{
		return privileges[PRV_LISTUSER]==1;
	}
	bool Has_Force_Offline()
	{
		return privileges[PRV_FORCE_OFFLINE]==1;
	}
	bool Has_Forbid_Talk()
	{
		return privileges[PRV_FORBID_TALK]==1;
	}
	bool Has_Forbid_Trade()
	{
		return privileges[PRV_FORBID_TRADE]==1;
	}
	bool Has_Forbid_Sell()
	{
		return privileges[PRV_FORBID_SELL]==1;
	}
	bool Has_Broadcast()
	{
		return privileges[PRV_BROADCAST]==1;
	}
	bool Has_Shutdown_GameServer()
	{
		return privileges[PRV_SHUTDOWN_GAMESERVER]==1;
	}
	bool Has_Summon_Monster()
	{
		return privileges[PRV_SUMMON_MONSTER]==1;
	}
	bool Has_Dispel_Summon()
	{
		return privileges[PRV_DISPEL_SUMMON]==1;
	}
	bool Has_Pretend()
	{
		return privileges[PRV_PRETEND]==1;
	}
	bool Has_GMMaster()
	{
		return privileges[PRV_GMMASTER]==1;
	}
	bool Has_DoubleExp()
	{
		return privileges[PRV_DOUBLEEXP]==1;
	}
	bool Has_Lambda()
	{
		return privileges[PRV_LAMBDA]==1;
	}
	bool Has_ActivityManager()
	{
		return privileges[PRV_ACTIVITYMANAGER]==1;
	}
};//end of class

}; //end name space

#endif	