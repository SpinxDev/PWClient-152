
#ifndef __GNET_ANNOUNCESELLRESULT_HPP
#define __GNET_ANNOUNCESELLRESULT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AnnounceSellResult : public GNET::Protocol
{
	#include "announcesellresult"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	/*	// TODO
		if ( roleid==seller )
		{
			if ( retcode==ERR_SUCCESS )
			{
				printf("\n您所挂售的点卡(%d)已成功售出，出售游戏点%d将很快扣除，所获虚拟币%d将在下次登陆时自动加入包裹栏中。",sellid,point,price);
			}
			else if ( retcode==ERR_TIMEOUT )
			{
				printf("\n您所挂售的点卡(%d)已超时\n",sellid);
			}
		}
		else 
		{
			if ( retcode==ERR_SUCCESS )
			{
				printf("\n您已经成功购买玩家%d出售的点卡%d,获得的游戏点数%d将很快添加,您所花费的虚拟币为%d\n",
						seller,sellid,point,price);
			}
			else
			{
				printf("\n您欲购买的由玩家%d出售的点卡%d未能成功销售给您，所付出的虚拟币%d将在下次登陆时返还.\n",
						seller,sellid,price);
			}
		}	*/
	}
};

};

#endif
