#ifndef __GNET__NETCLIENT
#define __GNET__NETCLIENT

namespace GNET
{
	class NetClient: public Protocol::Manager
	{
	public:
		Octets nonce;
	};
};
#endif
