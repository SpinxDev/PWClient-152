#ifndef __GNET_FORBID_H
#define __GNET_FORBID_H
	
#include <string.h>
#define FORBID_NUM 256	
	
namespace GNET
{
class Forbid
{
public:
	enum
	{
		FBD_FORBID_LOGIN		= 100,	//强制玩家下线，并禁止在一定时间上线
		FBD_FORBID_TALK		= 101,	//禁言
		FBD_FORBID_TRADE		= 102,	//禁止玩家间、玩家与NPC交易
		FBD_FORBID_SELL		= 103,	//禁卖
		FBD_FORBID_SELLPTS	= 104,
	};
private:	
	unsigned char forbids[FORBID_NUM];
public:
	void Init(void* buf,size_t size)
	{
		Reset();
		unsigned char* pFbd=(unsigned char*) buf;
		for (size_t i=0;i<size;i++)
			SetForbid(pFbd[i]);
	}
	Forbid()
	{
		Reset();
	}
	void operator=(const Forbid& rhs)
	{
		memcpy(this->forbids,rhs.forbids,FORBID_NUM);
	}
	void Reset()
	{
		memset(forbids,0,FORBID_NUM);
	}
	void SetForbid(int fbd_type)
	{
		if (fbd_type>=0 && fbd_type<FORBID_NUM)
			forbids[fbd_type]=1;
	}	
	bool Has_Forbid_Login()
	{
		return forbids[FBD_FORBID_LOGIN]==1;
	}
	bool Has_Forbid_Talk()
	{
		return forbids[FBD_FORBID_TALK]==1;
	}
	bool Has_Forbid_Trade()
	{
		return forbids[FBD_FORBID_TRADE]==1;
	}
	bool Has_Forbid_Sell()
	{
		return forbids[FBD_FORBID_SELL]==1;
	}
};//end of class

}; //end name space

#endif	
