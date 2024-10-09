#ifdef WIN32
#include <winsock2.h>
#include "gncompress.h"
#else
#include "binder.h"
#endif
#include "keepalive.hpp"
#include "getitemdesc_re.hpp"
#include "getroledesc_re.hpp"
#include "getroleskilldesc_re.hpp"
#include "getroleequipmentdesc_re.hpp"
#include "getrolepetcorraldesc_re.hpp"
#include "getencrypteditemdesc_re.hpp"
#include "toolannounceaid.hpp"
#include "getitemdesc.hpp"
#include "getroledesc.hpp"
#include "getroleskilldesc.hpp"
#include "getroleequipmentdesc.hpp"
#include "getrolepetcorraldesc.hpp"
#include "getencrypteditemdesc.hpp"

namespace GNET
{

static KeepAlive __stub_KeepAlive((void*)0);
static GetItemDesc_Re __stub_GetItemDesc_Re((void*)0);
static GetRoleDesc_Re __stub_GetRoleDesc_Re((void*)0);
static GetRoleSkillDesc_Re __stub_GetRoleSkillDesc_Re((void*)0);
static GetRoleEquipmentDesc_Re __stub_GetRoleEquipmentDesc_Re((void*)0);
static GetRolePetCorralDesc_Re __stub_GetRolePetCorralDesc_Re((void*)0);
static GetEncryptedItemDesc_Re __stub_GetEncryptedItemDesc_Re((void*)0);
static ToolAnnounceAid __stub_ToolAnnounceAid((void*)0);
static GetItemDesc __stub_GetItemDesc((void*)0);
static GetRoleDesc __stub_GetRoleDesc((void*)0);
static GetRoleSkillDesc __stub_GetRoleSkillDesc((void*)0);
static GetRoleEquipmentDesc __stub_GetRoleEquipmentDesc((void*)0);
static GetRolePetCorralDesc __stub_GetRolePetCorralDesc((void*)0);
static GetEncryptedItemDesc __stub_GetEncryptedItemDesc((void*)0);

};
