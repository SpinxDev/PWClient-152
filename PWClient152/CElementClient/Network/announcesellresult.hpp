
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
				printf("\n�������۵ĵ㿨(%d)�ѳɹ��۳���������Ϸ��%d���ܿ�۳������������%d�����´ε�½ʱ�Զ�����������С�",sellid,point,price);
			}
			else if ( retcode==ERR_TIMEOUT )
			{
				printf("\n�������۵ĵ㿨(%d)�ѳ�ʱ\n",sellid);
			}
		}
		else 
		{
			if ( retcode==ERR_SUCCESS )
			{
				printf("\n���Ѿ��ɹ��������%d���۵ĵ㿨%d,��õ���Ϸ����%d���ܿ����,�������ѵ������Ϊ%d\n",
						seller,sellid,point,price);
			}
			else
			{
				printf("\n��������������%d���۵ĵ㿨%dδ�ܳɹ����۸������������������%d�����´ε�½ʱ����.\n",
						seller,sellid,price);
			}
		}	*/
	}
};

};

#endif
