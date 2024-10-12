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

static GNET::Protocol::Type _state_GTransformClient[] = 
{
	PROTOCOL_GETITEMDESC,
	PROTOCOL_GETROLEDESC,
	PROTOCOL_GETROLESKILLDESC,
	PROTOCOL_GETROLEEQUIPMENTDESC,
	PROTOCOL_GETROLEPETCORRALDESC,
	PROTOCOL_GETENCRYPTEDITEMDESC,
};

GNET::Protocol::Manager::Session::State state_GTransformClient(_state_GTransformClient,
						sizeof(_state_GTransformClient)/sizeof(GNET::Protocol::Type), 86400);


};

