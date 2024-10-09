#ifndef __GNET_GTRANSFORMD_STATE
#define __GNET_GTRANSFORMD_STATE

#ifdef WIN32
#include "gnproto.h"
#else
#include "protocol.h"
#endif

namespace GNET
{

extern GNET::Protocol::Manager::Session::State state_GTransformClient;

};

#endif
