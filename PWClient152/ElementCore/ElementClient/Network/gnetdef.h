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

//�ʼ����Ͷ���
#define _MA_UNREAD           0x0001
#define _MA_ATTACH_OBJ       0x0002
#define _MA_ATTACH_MONEY     0x0004
#define _MA_PRESERVE         0x0010

enum ErrCode {
	ERR_SUCCESS = 0,            //�ɹ�
	ERR_TOBECONTINUE = 1,       //�ɹ������һ��и�������δ�����꣬Ŀǰδ��

	ERR_INVALID_ACCOUNT = 2,    //�ʺŲ�����
	ERR_INVALID_PASSWORD = 3,   //�������
	ERR_TIMEOUT = 4,            //��ʱ
	ERR_INVALID_ARGUMENT = 5,   //��������
	ERR_FRIEND_SYNCHRONIZE = 6, //������Ϣ���浽���ݿ�ʱ�޷�ͬ��
	ERR_SERVERNOTSUPPLY = 7,    //�÷�������֧�ָ�����
	ERR_COMMUNICATION=8,        //����ͨѶ����
	ERR_ACCOUNTLOCKED=9,        //����ظ���½����ǰ�û���һ����½���ڱ�������������״̬
	ERR_MULTILOGIN  =10,        //����ظ���½�����û�ѡ���Զ�����
	// keyexchange      
	ERR_INVALID_NONCE    = 11,  //��Ч��nonceֵ
	// deliver use
	ERR_DELIVER_SEND     = 21,  //ת��ʧ��
	ERR_DELIVER_TIMEOUT  = 22,  //ת����ʱ
	//player login
	ERR_LOGINFAIL   =   31,     //��½��Ϸʧ��
	ERR_KICKOUT     =   32,     //��������
	ERR_CREATEROLE  =   33,     //������ɫʧ��
	ERR_DELETEROLE  =   34,     //ɾ����ɫʧ��
	ERR_ROLELIST    =   35,     //��ý�ɫ�б�ʧ��
	ERR_UNDODELROLE =   36,     //����ɾ����ɫʧ��
	ERR_LINKISFULL	=	39,		//��������������
	//add friend
	ERR_ADDFRD_REQUEST     = 51,  //�����Ϊ����
	ERR_ADDFRD_REFUSE      = 52,  //�ܾ���Ϊ����
	ERR_ADDFRD_AGREE       = 53,  //ͬ���Ϊ����
	ERR_ADDFRD_AGREEANDADD = 54,  //ͬ�Ⲣϣ�����Է���Ϊ����

	//QQ DB retcode
	ERR_FAILED            = 41,
	ERR_EXCEPTION         = 42,
	ERR_NOTFOUND          = 43,
	ERR_INVALIDHANDLE     = 44,
	ERR_DUPLICATRECORD    = 45,
	ERR_NOFREESPACE       = 46,

	//game DB
	ERR_DATANOTFIND = 60,  //���ݲ�����        
	ERR_GENERAL		= 61,
	ERR_ERR_UNAVAILABLE        = 65,  //�ѻ���ʿ����ɾ����ɫ

    //faction error code (101-200)  
    ERR_FC_NETWORKERR       =   101,    //����������ͨѶ����
    ERR_FC_INVALID_OPERATION=   102,    //��Ч�Ĳ�������
    ERR_FC_OP_TIMEOUT       =   103,    //������ʱ
    ERR_FC_CREATE_ALREADY   =   104,    //����Ѿ���ĳ�����ɵĳ�Ա�������ٴ�������
    ERR_FC_CREATE_DUP       =   105,    //���������ظ�
    ERR_FC_DBFAILURE        =   106,    //���ݿ�IO����
    ERR_FC_NO_PRIVILEGE     =   107,    //û����ز�����Ȩ��
	ERR_FC_INVALIDNAME      =   108,    //����ʹ�ô�����
	ERR_FC_FULL             =   109,    //�����Ѵ�����
    ERR_FC_APPLY_REJOIN     =   110,    //�Ѿ���ĳ�����ɵĳ�Ա������ʧ��
    ERR_FC_JOIN_SUCCESS     =   111,    //�ɹ��������
    ERR_FC_JOIN_REFUSE      =   112,    //���뱻�ܾ�
    ERR_FC_ACCEPT_REACCEPT  =   113,    //����׼������ɵ�����Ѿ��������
    ERR_FC_FACTION_NOTEXIST =   114,    //���ɲ�����or���û����������뱾����
    ERR_FC_NOTAMEMBER       =   115,    //��Ҳ��Ǳ����ɵİ���
    ERR_FC_CHECKCONDITION   =   116,    //�����������������SP�������ʽ𲻹�
    ERR_FC_DATAERROR        =   117,    //�����������ʹ��󣬿ͻ����ύ�Ĳ���������ʽ����
	ERR_FC_OFFLINE          =   118,    //��Ҳ�����
	
	ERR_FC_INVALID_IN_PVP	=	441,	//���ɿ����Ӷ�ģʽ�ڼ䲻�ܽ��иò���


	//�����Ҵ������
	ERR_CHAT_CREATE_FAILED     =   151, //����ʧ��
	ERR_CHAT_INVALID_SUBJECT   =   152, //�Ƿ�����
	ERR_CHAT_ROOM_NOT_FOUND    =   153, //�����Ҳ�����
	ERR_CHAT_JOIN_REFUSED      =   154, //�������󱻾ܾ�
	ERR_CHAT_INVITE_REFUSED    =   155, //�������뱻�ܾ�
	ERR_CHAT_INVALID_PASSWORD  =   156, //�������������
	ERR_CHAT_INVALID_ROLE      =   157, //��ɫδ�ҵ�
	ERR_CHAT_PERMISSION_DENY   =   158, //û��Ȩ��
	ERR_CHAT_EXCESSIVE         =   159, //���������ҹ���
	ERR_CHAT_ROOM_FULL         =   160, //�����Ѵ�����
	ERR_CHAT_SEND_FAILURE      =   161, //����ʧ��


	//����ϵͳ�������
	ERR_MS_DBSVR_INV           =   211, //���ݿ���񲻿�����
	ERR_MS_MAIL_INV            =   212, //�ʼ�������
	ERR_MS_ATTACH_INV          =   213, //����ĸ�����Ϣ
	ERR_MS_SEND_SELF           =   214, //��ֹ���Լ������ʼ�
	ERR_MS_ACCOUNTFROZEN       =   215, //Ŀ�������Ѿ�����
	ERR_MS_AGAIN               =   216, //���������ʱ������
	ERR_MS_BOXFULL             =   217, //Ŀ����������

	//�����������
	ERR_AS_MAILBOXFULL         =   220, //���������ʣ��ռ�
	ERR_AS_ITEM_INV            =   221, //�����������Ʒ��Ϣ
	ERR_AS_MARKET_UNOPEN       =   222, //������δ���ţ�δ��ɳ�ʼ����
	ERR_AS_ID_EXHAUSE          =   223, //�������þ�
	ERR_AS_ATTEND_OVF          =   224, //������������ﵽ����
	ERR_AS_BID_LOWBID          =   225, //���۹��;���ʧ��
	ERR_AS_BID_NOTFOUND        =   226, //δ�ҵ��������¼�
	ERR_AS_BID_BINSUCCESS      =   227, //һ�ڼ����
	ERR_AS_BID_UNREDEEMABLE    =   228, //�������
	ERR_AS_BID_INVALIDPRICE    =   229, //������������ϵͳ����ȫ�趨����������۸�
	
	ERR_SP_NOT_INIT            =   231, //ϵͳû�г�ʼ�����
    ERR_SP_SPARETIME           =   232, //ʣ��ʱ�䲻�����������
    ERR_SP_INVA_POINT          =   233, //��Ч�Ĺ��۵�����������30Ԫ��������
    ERR_SP_EXPIRED             =   234, //�õ㿨�Ѿ�����
    ERR_SP_NOMONEY             =   237, //����Ҳ���
    ERR_SP_SELLING             =   239, //�㿨�Ѿ���������״̬
    ERR_SP_MONEYEXCEED         =   242, //��Ǯ���ﵽ����
    ERR_SP_BUYSELF             =   243, //���ܹ����Լ����۵ĵ㿨
    ERR_SP_EXCESS              =   245, //��ֹ���Ƚ��׳����㿨

    //��ս�������
    ERR_BS_INVALIDROLE         =   260, //��ɫ��ݲ�����
    ERR_BS_FAILED              =   261, //����ʧ��
    ERR_BS_OUTOFSERVICE        =   262, //��ս������ʱ������
	ERR_BS_NEWBIE_BANNED       =   263, //�������72Сʱ�ڲ���������ս
	
	//�����ƹ�(online promote)������� (��Ӧserver_error.txt�еĴ�����Ϣ)
	ERR_OP_DOWN_OFFLINE			=	281, //"���û�����ߵ�����"
	ERR_OP_UP_NOT_LOAD			=	282, //"��ҵ�������Ϣ��δ����"
	ERR_OP_PICKINGUP			=	283, //"���������ȡ����ֵ"
	ERR_OP_NO_DIVIDEND			=	284, //"����û�п���ȡ�ĺ���ֵ"
	ERR_OP_DB_ERROR				=	285, //"���ݿ���ʴ���"
	ERR_OR_INVALID_PCODE		=	286, //"�ƹ��벻��ȷ"
	ERR_OP_NOT_IN_CURR_SERVER	=	287, //"�ƹ��˲��ڱ�������"
	ERR_OP_INVALID_LEVEL		=	288, //"20��������Ҳ����ƹ�����"
	ERR_OP_NOT_IN_CURR_REGION	=	289, //"�ƹ��˲��ڱ���"
	ERR_OP_ALREADY_BINDED		=	290, //"�����Ѿ��󶨣��޷��ٴΰ�"
	ERR_OP_REPUATION			=	291, //"��ȡ�ƹ���������ֵ����"

	//���ѷ�����(consume return points)������� (��Ӧserver_error.txt�еĴ�����Ϣ)
	ERR_CRP_PLAYER_OFFLINE		=	301, //"����Ѿ�����"
	ERR_CRP_DB_BUSY				=	302, //"���ݿⷱæ"
	ERR_CRP_DB_ERROR			=	303, //"���ݿ���ʴ���"
	ERR_CRP_NOT_ENOUGH_POINTS	=	304, //"���ֲ���"
	ERR_CRP_INVALID_CHANGE_TIME	=	305, //"δ�ڻ���ڲ��ܶһ�����"
	ERR_CRP_INVALID_RETURN_TYPE	=	306, //"��Ч�ķ�����ʽ"

	//����ƽ̨������� (��Ӧserver_error.txt�еĴ�����Ϣ)
	ERR_WT_UNOPEN				=	311,	//	����δ����
	ERR_WT_SN_EXHAUSE			=	312,	//	�޷������Ч���׺�
	ERR_WT_CANNOT_POST			=	313,	//	�޷�����
	ERR_WT_CANNOT_CANCELPOST	=	314,	//	�޷�ȡ������
	ERR_WT_NOT_ENOUGH_DEPOSIT	=	315,	//	���ױ�֤����
	ERR_WT_ILLEGAL_SELL_PERIOD	=	316,	//	�����ϼ�ʱ�䲻�Ϸ�
	ERR_WT_ENTRY_NOT_FOUND		=	317,	//	���׺Ŷ�Ӧ��Ʒ������
	ERR_WT_ENTRY_IS_BUSY		=	318,	//	ͬ�������У��޷����ܽ�������
	ERR_WT_SN_ROLEID_MISMATCH	=	319,	//	���׺����ɫ��ƥ��
	ERR_WT_TIMESTAMP_MISMATCH	=	320,	//	���ײ���ʱ����ƥ��
	ERR_WT_ENTRY_HAS_BEEN_SOLD	=	321,	//	��Ʒ���۳�
	ERR_WT_BUYER_NOT_EXIST		=	322,	//	��Ҳ�����
	ERR_WT_SN_USERID_MISMATCH	=	323,	//	���׺����ʺŲ�ƥ��
	ERR_WT_ILLEGAL_SELL_PRICE	=	324,	//	�ϼܼ۸񲻺Ϸ�
	ERR_WT_TOO_MANY_ATTEND_SELL	=	325,	//	������Ʒ����
	ERR_WT_MAILBOX_FULL			=	326,	//	�������������ɽ��в���
	ERR_WT_BUYER_STATUS_INAPPROPRIATE,
	ERR_WT_BUYER_NOT_EXIST_2,
	ERR_WT_SELLER_NOT_EXIST,
	ERR_WT_BUYER_CONDITION_NOT_MATCH,
	ERR_WT_HAS_ATTEND_SELL,
	ERR_WT_USER_OTHER_ROLE_ON_SALE,

	//�̳Ǿ��Ĵ������ (��Ӧserver_error.txt�еĴ�����Ϣ)
	ERR_SA_ENTRY_NOT_FOUND		=	341,	//	������Ʒ������
	ERR_SA_ENTRY_IS_BUSY		=	342,	//	������Ʒͬ�������У��޷����ܾ�������
	ERR_SA_CANNOT_BID			=	343,	//	��ǰ�޷�����
	ERR_SA_LOW_BIDPRICE			=	344,	//	���۹���
	ERR_SA_BID_FAILED			=	345,	//	����ʧ�ܣ����˸��˸��ߵľ���
	ERR_SA_USER_NOT_FOUND		=	346,	//	���뾺���˺Ų�����
	ERR_SA_USER_IS_BUSY			=	347,	//	�˺�ͬ�������У��޷����ܾ�������
	ERR_SA_CASH_NOT_ENOUGH		=	348,	//	����Ԫ������
	ERR_SA_CASH_OVERFLOW		=	349,	//	����Ԫ����ص�������󣨴����ȡ����
	ERR_SA_CASH_NOT_ENOUGH_FOR_TRANSFER,

	// ����ѡ�ٴ������
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
    ERR_STOCK_CLOSED        = 1,        //Ԫ������ϵͳδ����
    ERR_STOCK_ACCOUNTBUSY   = 2,        //����˻����ڽ��н��ף����Ժ�
    ERR_STOCK_INVALIDINPUT  = 3,        //�������ݴ���
    ERR_STOCK_OVERFLOW      = 4,        //�����������
    ERR_STOCK_DATABASE      = 5,        //���ݿ�æ�����Ժ�
    ERR_STOCK_NOTENOUGH     = 6,        //�˻�����
	ERR_STOCK_MAXCOMMISSION = 7,        //������Ŀ�Ѵ�����
	ERR_STOCK_NOTFOUND      = 8,        //�����ڵ��ʺ�
	ERR_STOCK_CASHLOCKED    = 9,        //�ʺ�������,���ܽ���
	ERR_STOCK_CASHUNLOCKFAILED = 10,    //�ʺŽ���ʧ��
};

#define _MSG_CONVERSATION   1
#define _MSG_ADDFRD_RQST    2
#define _MSG_ADDFRD_RE      3

#define _INFO_PUBLIC        1
#define _INFO_PRIVATE       2
#define _INFO_PROTECTED     3

enum{
	//Trade
	ERR_TRADE_PARTNER_OFFLINE=  68,     //�Է��Ѿ�����
	ERR_TRADE_AGREE         =   0,      //ͬ�⽻��
	ERR_TRADE_REFUSE        =   69,     //�Է��ܾ�����
	ERR_TRADE_BUSY_TRADER   =   70,     //trader �Ѿ��ڽ�����
	ERR_TRADE_DB_FAILURE    =   71,     //��д���ݿ�ʧ��
	ERR_TRADE_JOIN_IN       =   72,     //���뽻��ʧ�ܣ����׶����˫���Ѿ�����
	ERR_TRADE_INVALID_TRADER=   73,     //��Ч�Ľ�����
	ERR_TRADE_ADDGOODS      =   74,     //���ӽ�����Ʒʧ��
	ERR_TRADE_RMVGOODS      =   75,     //���ٽ�����Ʒʧ��
	ERR_TRADE_READY_HALF    =   76,     //�ύ���һ�룬�ȴ��Է��ύ
	ERR_TRADE_READY         =   77,     //�ύ���
	ERR_TRADE_SUBMIT_FAIL   =   78,     //�ύʧ��
	ERR_TRADE_CONFIRM_FAIL  =   79,     //ȷ��ʧ��
	ERR_TRADE_DONE          =   80,     //�������
	ERR_TRADE_HALFDONE      =   81,     //�������һ�룬�ȴ���һ��ȷ��
	ERR_TRADE_DISCARDFAIL   =   82,     //ȡ������ʧ��
	ERR_TRADE_MOVE_FAIL     =   83,     //�ƶ���Ʒʧ��
	ERR_TRADE_SPACE         =   84,     //��Ʒ���ռ䲻��
	ERR_TRADE_SETPSSN       =   85,     //���ý����߲Ʋ�����
	ERR_TRADE_ATTACH_HALF   =   86,     //�ɹ�����һ��һ��������
	ERR_TRADE_ATTACH_DONE   =   87,     //�ɹ���������������
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
	ERR_FS_OFFLINE          =  1,     //��Ҳ�����
	ERR_FS_REFUSE           =  2,     //���ܾ�
	ERR_FS_TIMEOUT          =  3,     //��ʱ
	ERR_FS_NOSPACE          =  4,     //��ʣ��ռ�
	ERR_FS_NOFOUND          =  5,     //δ�ҵ�
	ERR_FS_ERRPARAM         =  6,     //��������
	ERR_FS_DUPLICATE        =  7,     //�ظ�
};

enum AddFriendRemarksErrorCode
{
	ERR_FRIEND_REMARKS_LEN = 1,		//	���Ȳ��Ϸ�
};

enum ONLINE_STATUS
{
	GAME_ONLINE = 0x01,			//	��Ϸ����
	GT_ONLINE		= 0x02,			//	GT ����
	WEB_ONLINE = 0x04,			//	WEB ����
	GT_INVISIBLE = 0x80,			//	GT ����
};

enum ERR_LOGIN_KOREAN
{
	CMD_LOGIN_DENY_CANT          = 231,  //�޷���½������Netmarble��վ�Ͻ��е�½����ȷ����ϸ��Ϣ��
	CMD_FAULT_AGECHECK           = 232,  //���䲻�ڿ�����Ϸ�ķ�Χ�ڡ�
	CMD_LOGIN_DENY_SERVERBUSY    = 233,  //�޷���½�����������³��ԡ�
	CMD_FAULT_BETA_TESTER        = 234,  //���ڲ���ҡ�
	CMD_LOGIN_DENY_IDERR         = 235,  //��½ʧ�ܡ���ȷ���ʺ�/��������µ�½��
	CMD_LOGIN_DENY_PASSERRLIMIT  = 236,  //���������ʺţ����볬��10�Ρ�����Netmarble��վ�����µ�½��
	CMD_LOGIN_DENY_PASSERR       = 237,  //��ȷ���ʺ�/��������µ�½��
};

enum PRIVATE_CHANNEL
{
	CHANNEL_NORMAL			= 0,		//�Ǻ���
	CHANNEL_NORMALRE		= 1,		//�Ǻ����Զ��ظ�
	CHANNEL_FRIEND			= 2,		//����
	CHANNEL_FRIEND_RE		= 3,		//�����Զ��ظ�
	CHANNEL_USERINFO		= 4,		//���������Ϣ
	CHANNEL_GAMETALK	=	6,			//	��GT����
	CHANNEL_GT_NORMAL	=	7,		//	ֻ������������������Ϣ����ͬʱת����GT�ͻ��˺���Ϸ�ͻ�������Ƶ��
	CHANNEL_GT_FRIEND	=	8,		//	ֻ������������������Ϣ����ͬʱת����GT�ͻ��˺���Ϸ�ͻ��˺���Ƶ��
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
    _BATTLE_BONUS,           // ��طֺ�
    _BATTLE_WIN_PRIZE,       // ��սʤ��,��ý���
    _BATTLE_BID_FAILED,      // ����ʧ��,�˻�Ѻ��
    _BATTLE_BID_WITHDRAW,    // ��սȡ��,�˻�Ѻ��
};

enum WEBTRADE_INFORM
{
	WEBTRADE_CANCEL_POST,	//	ȡ�����ۺ�ͨ���䷵��������Ʒ����
	WEBTRADE_POST_EXPIRE,	//	���۹��ں�ͨ���䷵��������Ʒ����
	WEBTRADE_SOLD,			//	���׳ɹ��������ұ�֤�𣨽�ң�
	WEBTRADE_BUY,			//	���׳ɹ�������ʼ���Ʒ����
	WEBTRADE_POST_FAILED,	//	����ʧ�ܣ��������ұ�֤�����Ʒ����
};

enum SYSAUCTION_INFORM
{
	SYSAUCTION_BID_SUCCESS,	//	���ĳɹ��ʼ���Ʒ
};

enum COUNTRYBATTLE_INFORM
{
	COUNTRYBATTLE_PRIZE,	//	��ս���ʽ���
	COUNTRYBATTLE_NOPRIZE,	//	��ս�޽���
};

enum PLAYERPRESENT_INFORM
{
    PLAYERPRESENT_GIVE,	//���ʼ��������ʼ�
	PLAYERPRESENT_ASK,	//���ʼ�����ȡ�ʼ�
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
	_MST_PLAYEROFFLINESHOP,// �����������̵�
	_MST_TANKBATTLE,
	_MST_ANTICHEAT,
	_MST_FACTIONRESOURCEBATTLE,
    _MST_TYPE_NUM,
};

enum PSHOP_INFORM // �����������̵�
{
	_PSHOP_TIMEOUT,
	_PSHOP_DELETED,
	_PSHOP_RETURN_ITEM,
	_PSHOP_RETURN_MONEY,
};

}

#endif	//	__GNETDEF_H__
