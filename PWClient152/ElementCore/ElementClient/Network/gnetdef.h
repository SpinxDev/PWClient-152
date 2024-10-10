#ifndef __GNETDEF_H__
#define __GNETDEF_H__

namespace GNET{

#define WM_IOEVENT      (WM_USER + 100)
#define WM_IOPROTOCOL   (WM_USER + 101)
#define WM_SHOWWIND		(WM_USER + 102)

#define EVENT_ADDSESSION    1
#define EVENT_DELSESSION    2
#define EVENT_ABORTSESSION  3
#define EVENT_PINGRETURN    4
#define EVENT_DISCONNECT    5

#define _STATUS_OFFLINE         0
#define _STATUS_ONLINE          3
#define _STATUS_HIDDEN          6
#define _STATUS_BUSY            8
#define _STATUS_DEPART          9

#define _HANDLE_BEGIN       -1
#define _HANDLE_END         -1
#define _SID_INVALID        0

//邮件类型定义
#define _MA_UNREAD           0x0001
#define _MA_ATTACH_OBJ       0x0002
#define _MA_ATTACH_MONEY     0x0004
#define _MA_PRESERVE         0x0010

enum ErrCode {
	ERR_SUCCESS = 0,            //成功
	ERR_TOBECONTINUE = 1,       //成功，并且还有更多数据未传递完，目前未用

	ERR_INVALID_ACCOUNT = 2,    //帐号不存在
	ERR_INVALID_PASSWORD = 3,   //密码错误
	ERR_TIMEOUT = 4,            //超时
	ERR_INVALID_ARGUMENT = 5,   //参数错误
	ERR_FRIEND_SYNCHRONIZE = 6, //好友信息保存到数据库时无法同步
	ERR_SERVERNOTSUPPLY = 7,    //该服务器不支持该请求
	ERR_COMMUNICATION=8,        //网络通讯错误
	ERR_ACCOUNTLOCKED=9,        //多次重复登陆，当前用户的一个登陆正在被处理，处于锁定状态
	ERR_MULTILOGIN  =10,        //多次重复登陆，且用户选择自动下线
	// keyexchange      
	ERR_INVALID_NONCE    = 11,  //无效的nonce值
	// deliver use
	ERR_DELIVER_SEND     = 21,  //转发失败
	ERR_DELIVER_TIMEOUT  = 22,  //转发超时
	//player login
	ERR_LOGINFAIL   =   31,     //登陆游戏失败
	ERR_KICKOUT     =   32,     //被踢下线
	ERR_CREATEROLE  =   33,     //创建角色失败
	ERR_DELETEROLE  =   34,     //删除角色失败
	ERR_ROLELIST    =   35,     //获得角色列表失败
	ERR_UNDODELROLE =   36,     //撤销删除角色失败
	ERR_LINKISFULL	=	39,		//服务器人数已满
	//add friend
	ERR_ADDFRD_REQUEST     = 51,  //请求加为好友
	ERR_ADDFRD_REFUSE      = 52,  //拒绝加为好友
	ERR_ADDFRD_AGREE       = 53,  //同意加为好友
	ERR_ADDFRD_AGREEANDADD = 54,  //同意并希望将对方加为好友

	//QQ DB retcode
	ERR_FAILED            = 41,
	ERR_EXCEPTION         = 42,
	ERR_NOTFOUND          = 43,
	ERR_INVALIDHANDLE     = 44,
	ERR_DUPLICATRECORD    = 45,
	ERR_NOFREESPACE       = 46,

	//game DB
	ERR_DATANOTFIND = 60,  //数据不存在        
	ERR_GENERAL		= 61,
	ERR_ERR_UNAVAILABLE        = 65,  //已婚人士不能删除角色

    //faction error code (101-200)  
    ERR_FC_NETWORKERR       =   101,    //服务器网络通讯错误
    ERR_FC_INVALID_OPERATION=   102,    //无效的操作类型
    ERR_FC_OP_TIMEOUT       =   103,    //操作超时
    ERR_FC_CREATE_ALREADY   =   104,    //玩家已经是某个帮派的成员，不能再创建帮派
    ERR_FC_CREATE_DUP       =   105,    //帮派名称重复
    ERR_FC_DBFAILURE        =   106,    //数据库IO错误
    ERR_FC_NO_PRIVILEGE     =   107,    //没有相关操作的权限
	ERR_FC_INVALIDNAME      =   108,    //不能使用此名称
	ERR_FC_FULL             =   109,    //人数已达上限
    ERR_FC_APPLY_REJOIN     =   110,    //已经是某个帮派的成员，申请失败
    ERR_FC_JOIN_SUCCESS     =   111,    //成功加入帮派
    ERR_FC_JOIN_REFUSE      =   112,    //申请被拒绝
    ERR_FC_ACCEPT_REACCEPT  =   113,    //被批准加入帮派的玩家已经加入帮派
    ERR_FC_FACTION_NOTEXIST =   114,    //帮派不存在or玩家没有申请过加入本帮派
    ERR_FC_NOTAMEMBER       =   115,    //玩家不是本帮派的帮众
    ERR_FC_CHECKCONDITION   =   116,    //不满足操作条件，如SP不够，资金不够
    ERR_FC_DATAERROR        =   117,    //操作参数类型错误，客户端提交的操作参数格式错误
	ERR_FC_OFFLINE          =   118,    //玩家不在线
	
	ERR_FC_INVALID_IN_PVP	=	441,	//帮派开启掠夺模式期间不能进行该操作


	//聊天室错误代码
	ERR_CHAT_CREATE_FAILED     =   151, //创建失败
	ERR_CHAT_INVALID_SUBJECT   =   152, //非法主题
	ERR_CHAT_ROOM_NOT_FOUND    =   153, //聊天室不存在
	ERR_CHAT_JOIN_REFUSED      =   154, //加入请求被拒绝
	ERR_CHAT_INVITE_REFUSED    =   155, //聊天邀请被拒绝
	ERR_CHAT_INVALID_PASSWORD  =   156, //聊天室密码错误
	ERR_CHAT_INVALID_ROLE      =   157, //角色未找到
	ERR_CHAT_PERMISSION_DENY   =   158, //没有权限
	ERR_CHAT_EXCESSIVE         =   159, //加入聊天室过多
	ERR_CHAT_ROOM_FULL         =   160, //人数已达上限
	ERR_CHAT_SEND_FAILURE      =   161, //发送失败


	//邮箱系统错误代码
	ERR_MS_DBSVR_INV           =   211, //数据库服务不可连接
	ERR_MS_MAIL_INV            =   212, //邮件不存在
	ERR_MS_ATTACH_INV          =   213, //错误的附件信息
	ERR_MS_SEND_SELF           =   214, //禁止给自己发送邮件
	ERR_MS_ACCOUNTFROZEN       =   215, //目标邮箱已经冻结
	ERR_MS_AGAIN               =   216, //邮箱服务暂时不可用
	ERR_MS_BOXFULL             =   217, //目标邮箱已满

	//拍卖错误代码
	ERR_AS_MAILBOXFULL         =   220, //玩家信箱无剩余空间
	ERR_AS_ITEM_INV            =   221, //错误的拍卖物品信息
	ERR_AS_MARKET_UNOPEN       =   222, //拍卖场未开放（未完成初始化）
	ERR_AS_ID_EXHAUSE          =   223, //拍卖号用尽
	ERR_AS_ATTEND_OVF          =   224, //参与的拍卖数达到上限
	ERR_AS_BID_LOWBID          =   225, //出价过低竞拍失败
	ERR_AS_BID_NOTFOUND        =   226, //未找到该拍卖事件
	ERR_AS_BID_BINSUCCESS      =   227, //一口价买断
	ERR_AS_BID_UNREDEEMABLE    =   228, //不能赎回
	ERR_AS_BID_INVALIDPRICE    =   229, //竞价增幅超过系统允许安全设定，请检查输入价格
	
	ERR_SP_NOT_INIT            =   231, //系统没有初始化完成
    ERR_SP_SPARETIME           =   232, //剩余时间不满足挂售条件
    ERR_SP_INVA_POINT          =   233, //无效的挂售点数，必须是30元的整数倍
    ERR_SP_EXPIRED             =   234, //该点卡已经过期
    ERR_SP_NOMONEY             =   237, //虚拟币不足
    ERR_SP_SELLING             =   239, //点卡已经处于销售状态
    ERR_SP_MONEYEXCEED         =   242, //金钱数达到上限
    ERR_SP_BUYSELF             =   243, //不能购买自己挂售的点卡
    ERR_SP_EXCESS              =   245, //禁止过度交易炒卖点卡

    //城战错误代码
    ERR_BS_INVALIDROLE         =   260, //角色身份不符合
    ERR_BS_FAILED              =   261, //竞价失败
    ERR_BS_OUTOFSERVICE        =   262, //城战服务暂时不可用
	ERR_BS_NEWBIE_BANNED       =   263, //加入帮派72小时内不允许进入城战
	
	//线上推广(online promote)错误代码 (对应server_error.txt中的错误信息)
	ERR_OP_DOWN_OFFLINE			=	281, //"玩家没有在线的下线"
	ERR_OP_UP_NOT_LOAD			=	282, //"玩家的上线信息还未加载"
	ERR_OP_PICKINGUP			=	283, //"玩家正在提取鸿利值"
	ERR_OP_NO_DIVIDEND			=	284, //"上线没有可提取的鸿利值"
	ERR_OP_DB_ERROR				=	285, //"数据库访问错误"
	ERR_OR_INVALID_PCODE		=	286, //"推广码不正确"
	ERR_OP_NOT_IN_CURR_SERVER	=	287, //"推广人不在本服务器"
	ERR_OP_INVALID_LEVEL		=	288, //"20级以下玩家不能推广下线"
	ERR_OP_NOT_IN_CURR_REGION	=	289, //"推广人不在本区"
	ERR_OP_ALREADY_BINDED		=	290, //"上线已经绑定，无法再次绑定"
	ERR_OP_REPUATION			=	291, //"获取推广链接声望值不够"

	//消费返积分(consume return points)错误代码 (对应server_error.txt中的错误信息)
	ERR_CRP_PLAYER_OFFLINE		=	301, //"玩家已经离线"
	ERR_CRP_DB_BUSY				=	302, //"数据库繁忙"
	ERR_CRP_DB_ERROR			=	303, //"数据库访问错误"
	ERR_CRP_NOT_ENOUGH_POINTS	=	304, //"积分不足"
	ERR_CRP_INVALID_CHANGE_TIME	=	305, //"未在活动期内不能兑换积分"
	ERR_CRP_INVALID_RETURN_TYPE	=	306, //"无效的返还方式"

	//交易平台错误代码 (对应server_error.txt中的错误信息)
	ERR_WT_UNOPEN				=	311,	//	交易未开放
	ERR_WT_SN_EXHAUSE			=	312,	//	无法获得有效交易号
	ERR_WT_CANNOT_POST			=	313,	//	无法寄售
	ERR_WT_CANNOT_CANCELPOST	=	314,	//	无法取消寄售
	ERR_WT_NOT_ENOUGH_DEPOSIT	=	315,	//	交易保证金不足
	ERR_WT_ILLEGAL_SELL_PERIOD	=	316,	//	交易上架时间不合法
	ERR_WT_ENTRY_NOT_FOUND		=	317,	//	交易号对应物品不存在
	ERR_WT_ENTRY_IS_BUSY		=	318,	//	同步操作中，无法接受交易请求
	ERR_WT_SN_ROLEID_MISMATCH	=	319,	//	交易号与角色不匹配
	ERR_WT_TIMESTAMP_MISMATCH	=	320,	//	交易操作时戳不匹配
	ERR_WT_ENTRY_HAS_BEEN_SOLD	=	321,	//	物品已售出
	ERR_WT_BUYER_NOT_EXIST		=	322,	//	买家不存在
	ERR_WT_SN_USERID_MISMATCH	=	323,	//	交易号与帐号不匹配
	ERR_WT_ILLEGAL_SELL_PRICE	=	324,	//	上架价格不合法
	ERR_WT_TOO_MANY_ATTEND_SELL	=	325,	//	寄售物品过多
	ERR_WT_MAILBOX_FULL			=	326,	//	邮箱已满，不可进行操作
	ERR_WT_BUYER_STATUS_INAPPROPRIATE,
	ERR_WT_BUYER_NOT_EXIST_2,
	ERR_WT_SELLER_NOT_EXIST,
	ERR_WT_BUYER_CONDITION_NOT_MATCH,
	ERR_WT_HAS_ATTEND_SELL,
	ERR_WT_USER_OTHER_ROLE_ON_SALE,

	//商城竞拍错误代码 (对应server_error.txt中的错误信息)
	ERR_SA_ENTRY_NOT_FOUND		=	341,	//	竞价物品不存在
	ERR_SA_ENTRY_IS_BUSY		=	342,	//	竞价物品同步操作中，无法接受竞价请求
	ERR_SA_CANNOT_BID			=	343,	//	当前无法竞价
	ERR_SA_LOW_BIDPRICE			=	344,	//	竞价过低
	ERR_SA_BID_FAILED			=	345,	//	竞价失败，有人给了更高的竞价
	ERR_SA_USER_NOT_FOUND		=	346,	//	参与竞价账号不存在
	ERR_SA_USER_IS_BUSY			=	347,	//	账号同步操作中，无法接受竞价请求
	ERR_SA_CASH_NOT_ENOUGH		=	348,	//	竞价元宝不足
	ERR_SA_CASH_OVERFLOW		=	349,	//	竞价元宝相关的溢出错误（存入或取出）
	ERR_SA_CASH_NOT_ENOUGH_FOR_TRANSFER,

	// 国王选举错误代码
	ERR_KE_CANNOT_APPLY_CANDIDATE   = 471,
	ERR_KE_CANDIDATE_APPLY_EXCEED_UPPER_LIMIT,
	ERR_KE_ALREADY_APPLY_CANDIDATE,
	ERR_KE_CANNOT_VOTE,
	ERR_KE_CANDIDATE_NOT_EXIST,
	
	ERR_TANK_BATTLE_NOT_OPEN	= 490,
	ERR_TANK_BATTLE_ALREADY_APPLY,
	ERR_TANK_BATTLE_SWITCH_TIMEOUT,
	ERR_TANK_BATTLE_NOT_APPLY_MAP,
};

enum ERR_STOCK
{
    ERR_STOCK_CLOSED        = 1,        //元宝交易系统未开放
    ERR_STOCK_ACCOUNTBUSY   = 2,        //相关账户正在进行交易，请稍候
    ERR_STOCK_INVALIDINPUT  = 3,        //输入数据错误
    ERR_STOCK_OVERFLOW      = 4,        //超过金额上限
    ERR_STOCK_DATABASE      = 5,        //数据库忙，请稍候
    ERR_STOCK_NOTENOUGH     = 6,        //账户余额不足
	ERR_STOCK_MAXCOMMISSION = 7,        //交易数目已达上限
	ERR_STOCK_NOTFOUND      = 8,        //不存在的帐号
	ERR_STOCK_CASHLOCKED    = 9,        //帐号已锁定,不能交易
	ERR_STOCK_CASHUNLOCKFAILED = 10,    //帐号解锁失败
};

#define _MSG_CONVERSATION   1
#define _MSG_ADDFRD_RQST    2
#define _MSG_ADDFRD_RE      3

#define _INFO_PUBLIC        1
#define _INFO_PRIVATE       2
#define _INFO_PROTECTED     3

enum{
	//Trade
	ERR_TRADE_PARTNER_OFFLINE=  68,     //对方已经下线
	ERR_TRADE_AGREE         =   0,      //同意交易
	ERR_TRADE_REFUSE        =   69,     //对方拒绝交易
	ERR_TRADE_BUSY_TRADER   =   70,     //trader 已经在交易中
	ERR_TRADE_DB_FAILURE    =   71,     //读写数据库失败
	ERR_TRADE_JOIN_IN       =   72,     //加入交易失败，交易对象的双方已经存在
	ERR_TRADE_INVALID_TRADER=   73,     //无效的交易者
	ERR_TRADE_ADDGOODS      =   74,     //增加交易物品失败
	ERR_TRADE_RMVGOODS      =   75,     //减少交易物品失败
	ERR_TRADE_READY_HALF    =   76,     //提交完成一半，等待对方提交
	ERR_TRADE_READY         =   77,     //提交完成
	ERR_TRADE_SUBMIT_FAIL   =   78,     //提交失败
	ERR_TRADE_CONFIRM_FAIL  =   79,     //确认失败
	ERR_TRADE_DONE          =   80,     //交易完成
	ERR_TRADE_HALFDONE      =   81,     //交易完成一半，等待另一方确认
	ERR_TRADE_DISCARDFAIL   =   82,     //取消交易失败
	ERR_TRADE_MOVE_FAIL     =   83,     //移动物品失败
	ERR_TRADE_SPACE         =   84,     //物品栏空间不足
	ERR_TRADE_SETPSSN       =   85,     //设置交易者财产错误
	ERR_TRADE_ATTACH_HALF   =   86,     //成功加入一个一个交易者
	ERR_TRADE_ATTACH_DONE   =   87,     //成功加入两个交易者
};

#define _TRADE_END_TIMEOUT  0
#define _TRADE_END_NORMAL   1       

//player logout style
#define _PLAYER_LOGOUT_FULL 0
#define _PLAYER_LOGOUT_HALF 1

#define  _ROLE_STATUS_NORMAL    1
#define  _ROLE_STATUS_MUSTDEL   2
#define  _ROLE_STATUS_READYDEL  3
#define  _ROLE_STATUS_FROZEN    4
#define  _ROLE_STATUS_ON_CROSSSERVER 5
enum FS_ERR
{
	// Friend error code (201-250)
	ERR_FS_OFFLINE          =  1,     //玩家不在线
	ERR_FS_REFUSE           =  2,     //被拒绝
	ERR_FS_TIMEOUT          =  3,     //超时
	ERR_FS_NOSPACE          =  4,     //无剩余空间
	ERR_FS_NOFOUND          =  5,     //未找到
	ERR_FS_ERRPARAM         =  6,     //参数错误
	ERR_FS_DUPLICATE        =  7,     //重复
};

enum AddFriendRemarksErrorCode
{
	ERR_FRIEND_REMARKS_LEN = 1,		//	长度不合法
};

enum ONLINE_STATUS
{
	GAME_ONLINE = 0x01,			//	游戏在线
	GT_ONLINE		= 0x02,			//	GT 在线
	WEB_ONLINE = 0x04,			//	WEB 在线
	GT_INVISIBLE = 0x80,			//	GT 隐身
};

enum ERR_LOGIN_KOREAN
{
	CMD_LOGIN_DENY_CANT          = 231,  //无法登陆，请在Netmarble网站上进行登陆，并确认详细信息。
	CMD_FAULT_AGECHECK           = 232,  //年龄不在可以游戏的范围内。
	CMD_LOGIN_DENY_SERVERBUSY    = 233,  //无法登陆，过后请重新尝试。
	CMD_FAULT_BETA_TESTER        = 234,  //非内测玩家。
	CMD_LOGIN_DENY_IDERR         = 235,  //登陆失败。请确认帐号/密码后重新登陆。
	CMD_LOGIN_DENY_PASSERRLIMIT  = 236,  //错误输入帐号，密码超出10次。请在Netmarble网站上重新登陆。
	CMD_LOGIN_DENY_PASSERR       = 237,  //请确认帐号/密码后，重新登陆。
};

enum PRIVATE_CHANNEL
{
	CHANNEL_NORMAL			= 0,		//非好友
	CHANNEL_NORMALRE		= 1,		//非好友自动回复
	CHANNEL_FRIEND			= 2,		//好友
	CHANNEL_FRIEND_RE		= 3,		//好友自动回复
	CHANNEL_USERINFO		= 4,		//好友相关信息
	CHANNEL_GAMETALK	=	6,			//	与GT聊天
	CHANNEL_GT_NORMAL	=	7,		//	只发往服务器，表明消息将被同时转发到GT客户端和游戏客户端密语频道
	CHANNEL_GT_FRIEND	=	8,		//	只发往服务器，表明消息将被同时转发到GT客户端和游戏客户端好友频道
};

enum AUCTION_INFORM
{
    _AUCTION_ITEM_SOLD,
    _AUCTION_BID_WIN,
    _AUCTION_BID_LOSE,
    _AUCTION_BID_CANCEL,
	_AUCTION_BID_TIMEOUT,   
};

enum BATTLE_INFORM
{
    _BATTLE_BONUS,           // 领地分红
    _BATTLE_WIN_PRIZE,       // 城战胜利,获得奖金
    _BATTLE_BID_FAILED,      // 竞价失败,退还押金
    _BATTLE_BID_WITHDRAW,    // 城战取消,退还押金
};

enum WEBTRADE_INFORM
{
	WEBTRADE_CANCEL_POST,	//	取消寄售后通过其返还卖家物品或金币
	WEBTRADE_POST_EXPIRE,	//	寄售过期后通过其返还卖家物品或金币
	WEBTRADE_SOLD,			//	交易成功返还卖家保证金（金币）
	WEBTRADE_BUY,			//	交易成功给买家邮寄物品或金币
	WEBTRADE_POST_FAILED,	//	寄售失败，返还卖家保证金和物品或金币
};

enum SYSAUCTION_INFORM
{
	SYSAUCTION_BID_SUCCESS,	//	竞拍成功邮寄物品
};

enum COUNTRYBATTLE_INFORM
{
	COUNTRYBATTLE_PRIZE,	//	国战军资奖励
	COUNTRYBATTLE_NOPRIZE,	//	国战无奖励
};

enum PLAYERPRESENT_INFORM
{
    PLAYERPRESENT_GIVE,	//该邮件是赠送邮件
	PLAYERPRESENT_ASK,	//该邮件是索取邮件
};

enum TANKBATTLE_INFORM
{
	TANKBATTLE_BONOUSE,
	TANKBATTLE_NO_BONOUSE,
};

enum FACTIONRESOURCEBATTLE_INFORM
{
    FACTIONRESOURCEBATTLE_BONUS,
	FACTIONRESOURCEBATTLE_NO_BONUS,
};

enum MAIL_SENDER_TYPE
{
    _MST_PLAYER=0,
    _MST_NPC,
    _MST_AUCTION,
    _MST_WEB,
    _MST_BATTLE,
	_MST_WEBTRADE,
	_MST_SYSAUCTION,
	_MST_COUNTRYBATTLE,
	_MST_PLAYERPRESENT,
	_MST_PLAYEROFFLINESHOP,// 寄卖，离线商店
	_MST_TANKBATTLE,
	_MST_ANTICHEAT,
	_MST_FACTIONRESOURCEBATTLE,
    _MST_TYPE_NUM,
};

enum PSHOP_INFORM // 寄卖，离线商店
{
	_PSHOP_TIMEOUT,
	_PSHOP_DELETED,
	_PSHOP_RETURN_ITEM,
	_PSHOP_RETURN_MONEY,
};

}

#endif	//	__GNETDEF_H__
