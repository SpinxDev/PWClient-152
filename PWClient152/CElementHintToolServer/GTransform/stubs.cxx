#ifdef WIN32
#include <winsock2.h>
#include "gncompress.h"
#else
#include "binder.h"
#endif
#include "getitemdesc.hpp"
#include "getitemdesc_re.hpp"
#include "keepalive.hpp"
#include "toolannounceaid.hpp"

namespace GNET
{

static GetItemDesc __stub_GetItemDesc((void*)0);
static GetItemDesc_Re __stub_GetItemDesc_Re((void*)0);
static KeepAlive __stub_KeepAlive((void*)0);
static ToolAnnounceAid __stub_ToolAnnounceAid((void*)0);

};
