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
		PRV_TOGGLE_NAMEID		= 0,	//�л����������ID
		PRV_HIDE_BEGOD		= 1,	//����������޵�״̬
		PRV_ONLINE_ORNOT		= 2,	//�л��Ƿ�����
		PRV_CHAT_ORNOT		= 3,	//�л��Ƿ��������
		PRV_MOVETO_ROLE		= 4,	//�ƶ���ָ����ɫ���
		PRV_FETCH_ROLE		= 5,	//��ָ����ɫ�ٻ���GM���
		PRV_MOVE_ASWILL		= 6,	//�ƶ���ָ��λ��
		PRV_MOVETO_NPC		= 7,	//�ƶ���ָ��NPCλ��
		PRV_MOVETO_MAP		= 8,	//�ƶ���ָ����ͼ��������
		PRV_ENHANCE_SPEED		= 9,	//�ƶ�����
		PRV_FOLLOW		= 10,	//�������
		PRV_LISTUSER		= 11,	//��ȡ��������б�
		PRV_FORCE_OFFLINE		= 100,	//ǿ��������ߣ�����ֹ��һ��ʱ������
		PRV_FORBID_TALK		= 101,	//����
		PRV_FORBID_TRADE		= 102,	//��ֹ��Ҽ䡢�����NPC����
		PRV_FORBID_SELL		= 103,	//����
		PRV_BROADCAST		= 104,	//ϵͳ�㲥
		PRV_SHUTDOWN_GAMESERVER		= 105,	//�ر���Ϸ������
		PRV_SUMMON_MONSTER		= 200,	//�ٻ�����
		PRV_DISPEL_SUMMON		= 201,	//��ɢ���ٻ�����
		PRV_PRETEND		= 202,	//αװ
		PRV_GMMASTER		= 203,	//GM����Ա
		PRV_DOUBLEEXP		= 204,	//˫������
		PRV_LAMBDA		= 205,	//����������
		PRV_ACTIVITYMANAGER		= 206,	//�����Ա
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