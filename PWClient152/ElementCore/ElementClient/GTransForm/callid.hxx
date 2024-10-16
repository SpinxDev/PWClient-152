
#ifndef __GNET_GTRANSFORMD_CALLID
#define __GNET_GTRANSFORMD_CALLID

namespace GNET
{

enum CallID
{
};

enum ProtocolType
{
	PROTOCOL_KEEPALIVE	=	8036,
	PROTOCOL_GETITEMDESC_RE	=	8051,
	PROTOCOL_GETROLEDESC_RE	=	8053,
	PROTOCOL_GETROLESKILLDESC_RE	=	8092,
	PROTOCOL_GETROLEEQUIPMENTDESC_RE	=	8094,
	PROTOCOL_GETROLEPETCORRALDESC_RE	=	8096,
	PROTOCOL_GETENCRYPTEDITEMDESC_RE	=	8061,
	PROTOCOL_TOOLANNOUNCEAID	=	8090,
	PROTOCOL_GETITEMDESC	=	8050,
	PROTOCOL_GETROLEDESC	=	8052,
	PROTOCOL_GETROLESKILLDESC	=	8091,
	PROTOCOL_GETROLEEQUIPMENTDESC	=	8093,
	PROTOCOL_GETROLEPETCORRALDESC	=	8095,
	PROTOCOL_GETENCRYPTEDITEMDESC	=	8060,
};

};
#endif
