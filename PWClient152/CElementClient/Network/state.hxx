#ifndef __GNET_GLINKD_STATE
#define __GNET_GLINKD_STATE

#include "gnproto.h"

namespace GNET
{
// QQ
extern GNET::Protocol::Manager::Session::State state_LoginClient;
extern GNET::Protocol::Manager::Session::State state_DataExchangeClient;
// Game
extern GNET::Protocol::Manager::Session::State state_GLoginClient;
extern GNET::Protocol::Manager::Session::State state_GSelectRoleClient;
extern GNET::Protocol::Manager::Session::State state_GDataExchgClient;

};

#endif
