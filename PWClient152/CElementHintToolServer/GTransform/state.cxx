#include "callid.hxx"

#ifdef WIN32
#include <winsock2.h>
#include "gnproto.h"
#include "gncompress.h"
#else
#include "protocol.h"
#include "binder.h"
#endif

namespace GNET
{

static GNET::Protocol::Type _state_GTransformServer[] = 
{
	PROTOCOL_GETITEMDESC_RE,
	PROTOCOL_KEEPALIVE,
	PROTOCOL_TOOLANNOUNCEAID,
};

GNET::Protocol::Manager::Session::State state_GTransformServer(_state_GTransformServer,
						sizeof(_state_GTransformServer)/sizeof(GNET::Protocol::Type), 86400);


};

