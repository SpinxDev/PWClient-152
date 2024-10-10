/*
 * FILE: EC_MsgDef.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Messages to gamerun
#define MSG_EXITGAME			100		//	Exit game. p1 - exit code, 
#define MSG_CLOSEALLDIALOG		101		//	Close all dialogs
#define MSG_SERVERTIME			102		//	Server time
#define MSG_INSTCHECKOUT		103		//	Instance data checkout
#define MSG_DOUBLETIME			104		//	Double time
#define MSG_COMMONDATA			105		//  Common data
#define MSG_MALLITEMINFO		106		//  Mall item info
#define MSG_DIVIDENDMALLITEMINFO	107	//	Dividend mall item info
#define MSG_PUBLICQUESTINFO		108		//  Public Quest task info
#define MSG_WORLDLIFETIME		109		//	World life time info
#define MSG_ENTERAUTOHOME		110		//	Enter autohome
#define MSG_LEAVEAUTOHOME		111		//	Leave authhome
#define MSG_CALCNETWORKDELAY	112		//	Network delay
#define MSG_PLAYERSPECITEMLIST	113		//	Player spec items
#define MSG_PARALLEL_WORLD_INFO	114		//	分线和负载信息
#define MSG_UNIQUE_DATA			115		//	历史推进数据
#define MSG_RANDOM_MAP_ORDER	116

//	Messages to host player
#define MSG_GST_MOVE			200		//	Ghost move. p1 - move dir (0 - 7)
#define MSG_GST_PITCH			201		//	Ghost pitch, p1 - degree
#define MSG_GST_YAW				202		//	Ghost yaw, p1 - degree
#define MSG_GST_MOVEABSUP		203		//	Ghost move absolute up, p1 - 1: up, 0: down

#define MSG_HST_CAMERAMODE		220		//	Host camera mode. p1 - camera mode
#define MSG_HST_PUSHMOVE		221		//	Host player is pushed to move, p1 - move dir (0 - 7)
#define MSG_HST_PITCH			222		//	Host camera pitch, p1 - degree; p2 - turn player flag
#define MSG_HST_YAW				223		//	Host camera yaw, p1 - degree; p2 - turn player flag
#define MSG_HST_LBTNCLICK		224		//	Left button click, p1 - x; p2 - y; p3 - SHIFT, ALT, CTRL key states; p4 - double click flag
#define MSG_HST_TURNCAM			225		//	Begin or end turning host's camera, p1 - 1: begin, 0: end. p2 - 1: left button, 0: right button
#define MSG_HST_WHEELCAM		226		//	Move camera using mouse's wheel button
#define MSG_HST_CAMDEFAULT		227		//	Retrieve system default camera state
#define MSG_HST_CAMPRESET		228		//	Retrieve system pre-defined camera state
#define MSG_HST_CAMUSERGET		229		//	Retrieve user defined camera state, p1 - channel
#define MSG_HST_CAMUSERSET		230		//	Store user defined camera state, p1 - channel
#define MSG_HST_QUICKVIEW		231		//	Call camera monitor state quickly
#define MSG_HST_ATKRESULT		232		//	Host attack result
#define MSG_HST_ATTACKED		233		//	Host was attacked, p1 - data address
#define MSG_HST_DIED			234		//	Host died
#define MSG_HST_ASKREVIVE		235		//	Ask revive
#define MSG_HST_PICKUPMONEY		236		//	Host pickup money
#define MSG_HST_PICKUPITEM		237		//	Host pickup item
#define MSG_HST_FOLLOW			238		//	Host follow some else
#define MSG_HST_RECEIVEEXP		239		//	Received experience, p1 - data address
#define MSG_HST_INFO00			240		//	Host information 00, p1 - data address
#define MSG_HST_GOTO			241		//	Host go to a position, p1 - data address
#define MSG_HST_OWNITEMINFO		242		//	Host item information, p1 - data address
#define MSG_HST_GAINITEM		243		//	Gain item in or after trade
#define MSG_HST_SELTARGET		244		//	Select / unselect target, p1 - data address; p2 - command ID
#define MSG_HST_FIXCAMERA		245		//	Fix camera
#define MSG_HST_OWNEXTPROP		246		//	Host extend properties
#define MSG_HST_ADDSTATUSPT		247		//	Add status point
#define MSG_HST_RBTNCLICK		248		//	Left button click, p1 - x; p2 - y; p2 - y; p3 - SHIFT, ALT, CTRL key states
#define MSG_HST_JOINTEAM		249		//	Host join team
#define MSG_HST_LEAVETEAM		250		//	Host leave team
#define MSG_HST_NEWTEAMMEM		251		//	New team member joined
#define MSG_HST_ITEMOPERATION	252		//	Inventory and equipment item operations
#define MSG_HST_TEAMINVITE		253		//	Host is invited by a team
#define MSG_HST_TEAMREJECT		254		//	Host's join requirment is rejected by a team
#define MSG_HST_TEAMMEMPOS		255		//	Notify host team member position
#define MSG_HST_TEAMMEMPICKUP	256		//	Host team member pickup item
#define MSG_HST_EQUIPDAMAGED	257		//	Host equipment damaged
#define MSG_HST_NPCGREETING		258		//	NPC say greeting to host
#define MSG_HST_TRADESTART		259		//	Trade with player start
#define MSG_HST_TRADEREQUEST	260		//	Other player's trade request
#define MSG_HST_TRADEMOVEITEM	261		//	Move item when trading with other player
#define MSG_HST_TRADECANCEL		262		//	Host cancel trading with other player
#define MSG_HST_TRADESUBMIT		263		//	Host submit trading
#define MSG_HST_TARDECONFIRM	264		//	Host confirm trading
#define MSG_HST_TRADEEND		265		//	Trade end
#define MSG_HST_TRADEADDGOODS	266		//	Add goods to trade bar
#define MSG_HST_TRADEREMGOODS	267		//	Remove goods from trade bar
#define MSG_HST_IVTRINFO		268		//	Host inventory information
#define MSG_HST_STARTATTACK		269		//	Host start attack
#define MSG_HST_PURCHASEITEMS	270		//	Host buy items form NPC or player
#define MSG_HST_SPENDMONEY		271		//	Host spend money
#define MSG_HST_ITEMTOMONEY		272		//	Host sell items to NPC
#define MSG_HST_REPAIR			273		//	Repair items
#define MSG_HST_USEITEM			274		//	Host use item
#define MSG_HST_SKILLDATA		275		//	Host skill data
#define MSG_HST_EMBEDITEM		276		//	Embed item
#define MSG_HST_CLEARTESSERA	277		//	Clear tessera
#define MSG_HST_COSTSKILLPT		278		//	Cost skill point
#define MSG_HST_LEARNSKILL		279		//	Learn skill
#define MSG_HST_FLYSWORDTIME	280		//	Notify fly sword time
#define MSG_HST_PRODUCEITEM		281		//	Host produce item
#define MSG_HST_BREAKITEM		282		//	Host break item
#define MSG_HST_TASKDATA		283		//	Task data
#define MSG_HST_TARGETISFAR		284		//	Attack target is too far
#define MSG_HST_PRESSCANCEL		285		//	User press cancel button
#define MSG_HST_CANCELACTION	286		//	Cancel host current action
#define MSG_HST_ROOTNOTIFY		287		//	Dispel root
#define MSG_HST_STOPATTACK		288		//	Host stop attack
#define MSG_HST_JUMP			289		//	Test code. jump
#define MSG_HST_HURTRESULT		290		//	Hurt result
#define MSG_HST_ATTACKONCE		291		//	Attack once notify	
#define MSG_HST_TRASHBOXOP		292		//	Trash box operation
#define MSG_HST_PLAYTRICK		293		//	Play trick action
#define MSG_HST_SKILLRESULT		294		//	Host skill attack result
#define MSG_HST_SKILLATTACKED	295		//	Host skill attacked
#define MSG_HST_ASKTOJOINTEAM	296		//	Some ask to join team
#define MSG_HST_FACTION			297		//	Faction messages
#define MSG_HST_TASKDELIVER		298		//	Task deliver
#define MSG_HST_REPUTATION		299		//	Reputation update
#define MSG_HST_ITEMIDENTIFY	300		//	Item identify
#define MSG_HST_SANCTUARY		301		//	Sanctuary
#define MSG_HST_CORRECTPOS		302		//	Correct host position
#define MSG_HST_FRIENDOPT		303		//	Friend operation
#define MSG_HST_TEAMINVITETO	304		//	Team invite timeout
#define MSG_HST_WAYPOINT		305		//	Way point
#define MSG_HST_BREATHDATA		306		//	Breath data
#define MSG_HST_SKILLABILITY	307		//	Skill ability
#define MSG_HST_COOLTIMEDATA	308		//	Cool time data
#define MSG_HST_REVIVALINQUIRE	309		//	Revival inquire
#define MSG_HST_SETCOOLTIME		310		//	Set cool time
#define MSG_HST_CHGTEAMLEADER	311		//	Change team leader
#define MSG_HST_EXITINSTANCE	312		//	Exit instance notify
#define MSG_HST_CHANGEFACE		313		//	Host change face
#define MSG_HST_TEAMMEMBERDATA	314		//	Team member data
#define MSG_HST_SETMOVESTAMP	315		//	Set move stamp
#define MSG_HST_CHATROOMOPT		316		//	Chatroom messages
#define MSG_HST_MAILOPT			317		//	Mail messages
#define MSG_HST_VENDUEOPT		318		//	Vendue messages
#define MSG_HST_VIEWOTHEREQUIP	319		//	Get player's equipments
#define MSG_HST_PARIAHTIME		320		//	Update pariah time
#define MSG_HST_PETOPT			321		//	Pet operation
#define MSG_HST_BATTLEOPT		322		//	Battle operation
#define MSG_HST_ACCOUNTPOINT	323		//	Account point operation
#define MSG_HST_GMOPT			324		//	GM operation
#define MSG_HST_REFINEOPT		325		//	Refine operation
#define MSG_HST_ITEMBOUND		326		//	Item bound result
#define MSG_HST_USEITEMWITHDATA	327		//	Host use item with data
#define MSG_HST_SAFELOCK		328		//	Safe lock operation
#define MSG_HST_GOBLININFO		329		//  Goblin information
#define MSG_HST_AUTOMOVE		330		//  Player auto move
#define MSG_HST_BUY_SELL_FAIL	331		//  Player buy or sell failed 
#define MSG_HST_PLAYERPROPERTY	332		//	Player property
#define MSG_HST_PASSWDCHECKED	333		//  Player's password has checked
#define MSG_HST_HPSTEAL			334		//  Player steal hp
#define MSG_HST_MULTI_EXP_INFO  335     //  Player multi exp info
#define MSG_HST_MULTI_EXP_STATE 336     //	Player change multi exp state
#define MSG_HST_WEBTRADELIST	337		//	Web trade list re
#define MSG_HST_WEBTRADEATTENDLIST	338	//	Web trade attend list re
#define MSG_HST_WEBTRADEGETITEM		339	//	Web trade get item re
#define	MSG_HST_WEBTRADEGETDETAIL	340	//	Web trade get item detail re
#define	MSG_HST_WEBTRADEPREPOST		341	//	Web trade pre-post re
#define	MSG_HST_WEBTRADEPRECANCELPOST	342	// Web trade cancel post re
#define	MSG_HST_WEBTRADEUPDATE		343	//	Web trade update re
#define MSG_HST_SYSAUCTIONACCOUNT	344	//	Sys Auction Account re
#define MSG_HST_SYSAUCTIONBID		345	//	Sys Auction Bid re
#define MSG_HST_SYSAUCTIONCASHTRANSFER	346	// Sys Auction Cash Transfer re
#define MSG_HST_SYSAUCTIONGETITEM	347	//	Sys Auction Get Item re
#define MSG_HST_SYSAUCTIONLIST		348	//	Sys Auction List re
#define MSG_HST_ONLINEAWARD			349 // online award
//	后面再继续，这里不要再加

//	Messages to player manager
#define MSG_PM_PLAYERINFO		350		//	Player information, p1 - data address; p2 - command ID
#define MSG_PM_PLAYERMOVE		351		//	Player move, p1 - data address
#define MSG_PM_PLAYERRUNOUT		352		//	Player run out of view area, p1 - data address
#define MSG_PM_PLAYEREXIT		353		//	Player exit game, p1 - data address
#define MSG_PM_CHANGENAMECOLOR	354		//	Change player's name color
#define MSG_PM_PLAYERROOT		355		//	Player is root
#define MSG_PM_PLAYERDIED		356		//	Player was killed, p1 - data address
#define MSG_PM_PLAYERREVIVE		357		//	Player revive, p1 - data address
#define MSG_PM_PLAYEROUTOFVIEW	358		//	Player is out of view, p1 - player's ID
#define MSG_PM_PLAYERDISAPPEAR	359		//	Player disappear, p1 - data address
#define MSG_PM_PLAYERSTOPMOVE	360		//	Player stop move, p1 - data address
#define MSG_PM_PLAYERLEVELUP	361		//	Player level up
#define MSG_PM_PLAYEREXTPROP	362		//	Player extend properties
//	#define MSG_PM_TEAMMEMBERDATA	363		//	Team member data
#define MSG_PM_LEAVETEAM		364		//	Player leave team
#define MSG_PM_PLAYEREQUIPDATA	365		//	Player equipment data
#define MSG_PM_CASTSKILL		366		//	Player cast skill
#define MSG_PM_PLAYERBASEINFO	367		//	Player custom data
#define MSG_PM_PLAYERFLY		368		//	Player take off or land on
#define MSG_PM_PLAYERSITDOWN	369		//	Player sit down / stand up
#define MSG_PM_PLAYERATKRESULT	370		//	Player attack result
#define MSG_PM_PLAYEREXTSTATE	371		//	Player extend states changed
#define MSG_PM_PLAYERDOEMOTE	372		//	Player do emotion
#define MSG_PM_PLAYERUSEITEM	373		//	Player use item
#define MSG_PM_PLAYERCUSTOM		374		//	Player custom data
#define MSG_PM_ENCHANTRESULT	375		//	Enchant result
#define MSG_PM_PLAYERDOACTION	376		//	Player do action
#define MSG_PM_PLAYERSKILLRESULT 377	//	Player skill attack result
#define MSG_PM_PLAYERADVDATA	378		//	Player advertisement data
#define MSG_PM_PLAYERINTEAM		379		//	Player in team notify
#define MSG_PM_PICKUPMATTER		380		//	Player pickup a matter
#define MSG_PM_PLAYERGATHER		381		//	Player gather mine
#define MSG_PM_DOCONEMOTE		382		//	Player do concurrent emotion
#define MSG_PM_PLAYERCHGSHAPE	383		//	Player change shape
#define MSG_PM_BOOTHOPT			384		//	Booth operation
#define MSG_PM_PLAYERTRAVEL		385		//	Player travel service
#define MSG_PM_PLAYERPVP		386		//	Player PVP messages
#define MSG_PM_FASHIONENABLE	387		//	Player enable/disable fashion
#define MSG_PM_INVALIDOBJECT	388		//	Object is invalid
#define MSG_PM_PLAYEREFFECT		389		//	Player effect association
#define MSG_PM_CHANGEFACE		390		//	Player change face
#define MSG_PM_DUELOPT			391		//	Duel operation
#define MSG_PM_PLAYERBINDOPT	392		//	Player bind operation
#define MSG_PM_PLAYERBINDSTART	393		//	Player bind start
#define MSG_PM_PLAYERMOUNT		394		//	Player mount state
#define MSG_PM_PLAYERDUELOPT	395		//	Player duel operation
#define MSG_PM_PLAYERDUELRLT	396		//	Player duel result
#define MSG_PM_PLAYERLEVEL2		397		//	Player level2 notify
#define MSG_PM_TANKLEADER		398		//	Tank leader notify
#define MSG_PM_USEITEMWITHDATA	399
#define MSG_PM_PLAYERCHANGESPOUSE 400	//	Player change the spouse
#define MSG_PM_GOBLINOPT		401		//  Goblin operation: Cast skill, change refine active...
#define MSG_PM_GOBLINLEVELUP	402		//	Goblin level up
#define MSG_PM_PLAYERINVISIBLE	403		//  Player invisible
#define MSG_PM_PLAYERKNOCKBACK	404		//	Player knock back
#define MSG_PM_PLAYEREQUIPDISABLED 405  //  Player equipment disabled
#define MSG_PM_CONGREGATE		406		//  Congregate related
#define MSG_PM_TELEPORT			407		//	teleport
#define MSG_PM_FORBIDBESELECTED	408		//	forbid to be selected
#define MSG_PM_FORCE_CHANGED	409		//	a player changed his force
#define MSG_PM_MULTIOBJ_EFFECT	410		//  player's multi object effect
#define MSG_PM_COUNTRY_CHANGED	411
#define MSG_PM_KINGCHANGED		412
#define MSG_PM_TITLE			413		//  player's titles
#define MSG_PM_REINCARNATION	414
#define MSG_PM_REALMLEVEL		415
#define MSG_PM_PLAYER_IN_OUT_BATTLE 416
#define MSG_PM_FACTION_PVP_MASK_MODIFY 417

//	Messages to NPC manager
#define MSG_NM_NPCINFO			400		//	NPC information, p1 - protocol object address
#define MSG_NM_NPCMOVE			401		//	NPC move, p1 - data address
#define MSG_NM_NPCRUNOUT		402		//	NPC run out of view area, p1 - data address
#define MSG_NM_NPCDIED			403		//	NPC died, p1 - data address
#define MSG_NM_NPCDISAPPEAR		404		//	NPC disappear, p1 - data address
#define MSG_NM_NPCCASTSKILL		405		//	NPC cast skill
#define MSG_NM_NPCROOT			406		//	NPC is root
#define MSG_NM_NPCOUTOFVIEW		407		//	NPC is out of view, p1 - NPC's ID
#define MSG_NM_NPCSTOPMOVE		408		//	NPC stop moving, p1 - data address
#define MSG_NM_NPCATKRESULT		409		//	NPC attack result
#define MSG_NM_NPCEXTSTATE		410		//	NPC extend states changed
#define MSG_NM_ENCHANTRESULT	411		//	Enchant result
#define MSG_NM_NPCSKILLRESULT	412		//	NPC skill attack result
#define MSG_NM_INVALIDOBJECT	413		//	Object is invalid
#define MSG_NM_NPCLEVELUP		414		//	NPC level up
#define MSG_NM_NPCINVISIBLE		415		//  NPC invisible
#define	MSG_NM_NPCSTARTPLAYACTION	416	//	NPC play policy action
#define MSG_NM_NPCSTOPPLAYACTION	417	//	NPC stop play policy action
#define MSG_NM_FORBIDBESELECTED	418		//	forbid to be selected
#define MSG_NM_MULTIOBJECT_EFFECT 419	//	NPC's multi object effect

//	Messages to matter manager
#define MSG_MM_MATTERINFO		500		//	Matter information, p1 - data address
#define MSG_MM_MATTERDISAPPEAR	501		//	Matter disappear, p1 - data address
#define MSG_MM_MATTERENTWORLD	502		//	Matter enter world, p1 - data address
#define MSG_MM_MATTEROUTOFVIEW	503		//	Matter is out of view, p1 - data address
#define MSG_MM_INVALIDOBJECT	504		//	Object is invalid

//	Messages to host player cont.
#define MSG_HST_WEDDINGBOOKLIST 600		//	wedding book list
#define MSG_HST_WEDDINGBOOKSUCCESS	601	//	wedding book success notify
#define MSG_HST_FACTIONCONTRIB		602	//	faction contrib
#define MSG_HST_FACTIONFORTRESSINFO 603 //	faction fortress info
#define MSG_HST_ENTERFACTIONFORTRESS 604 //	enter faction fortress
#define MSG_HST_ENGRAVEITEM				605			//	 engrave item
#define MSG_HST_DPS_DPH_RANK		606			//	current dps dph rank
#define MSG_HST_ADDONREGEN			607			//	regenerate item addon
#define MSG_HST_INVISIBLEOBJLIST	608			//	invisible object list notify
#define MSG_HST_SETPLAYERLIMIT	609			//	set player limit
#define MSG_HST_FORCE_DATA			610	//	Player force data (get or changed)
#define MSG_HST_INVENTORY_DETAIL	611	//	Get other player's inventory detail info
#define MSG_HST_ADD_MULTIOBJECT_EFFECT 612 //  Add multi object effect
#define MSG_HST_WEDDINGSCENE_INFO 613	// whose wedding scene the player is entering
#define MSG_HST_PROFITTIME		  614	// change the player's profit level
#define MSG_HST_PVPNOPENALTY	  615 // PVP no penalty
#define MSG_HST_COUNTRY_NOTIFY	616
#define MSG_HST_ENTER_COUNTRYBATTLE 617
#define MSG_HST_COUNTRYBATTLE_RESULT 618
#define MSG_HST_COUNTRYBATTLE_SCORE 619
#define MSG_HST_COUNTRYBATTLE_REVIVE_TIMES 620
#define MSG_HST_COUNTRYBATTLE_CARRIER_NOTIFY 621
#define MSG_HST_COUNTRYBATTLE_BECOME_CARRIER 622
#define MSG_HST_COUNTRYBATTLE_PERSONAL_SCORE 623
#define MSG_HST_COUNTRYBATTLE_FLAG_MSG_NOTIFY 624
#define MSG_HST_DEFENSE_RUNE_ENABLE	625
#define MSG_HST_COUNTRYBATTLE_INFO 626
#define MSG_HST_CASHMONEYRATE	627
#define MSG_HST_STONECHANGEEND	628
#define MSG_HST_KINGNOTIFY 629
#define MSG_HST_MERIDIANS_NOTIFY 630
#define MSG_HST_MERIDIANS_RESULT 631
#define MSG_HST_COUNTRYBATTLE_STRONGHOLD_STATE	632
#define MSG_HST_TOUCHPOINT			633	// Touch related
#define MSG_HST_TITLE				644 // Title related
#define MSG_HST_SIGNIN				645
#define MSG_HST_REINCARNATION_TOME_INFO 646
#define MSG_HST_REINCARNATION_TOME_ACTIVATE 647
#define MSG_HST_USE_GIFTCARD		648 // Use gift card
#define MSG_HST_REALM_EXP			649
#define MSG_HST_TRICKBATTLE	650
#define MSG_HST_CONTINUECOMBOSKILL	651
#define MSG_HST_GENERALCARD 652
#define MSG_HST_MONSTERSPIRIT_LEVEL 653
#define MSG_HST_COUNTRYBATTLE_LIVESHOW 654
#define MSG_HST_RAND_MALL_SHOPPING_RES	655
#define MSG_HST_WORLD_CONTRIBUTION		656
#define MSG_HST_CANINHERIT_ADDONS		657
#define MSG_HST_CLIENT_SCREENEFFECT		658
#define MSG_HST_COMBO_SKILL_PREPARE		659
#define MSG_HST_INSTANCE_REENTER_NOTIFY	660
#define MSG_HST_PRAY_DISTANCE_CHANGE	661

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
typedef unsigned long		DWORD;		//	32-bit;

class MsgDataBase;
class MsgDataTargetPosition;
class MsgDataTaskNPC;

//	Message struct
struct ECMSG
{
	DWORD		  dwMsg;			//	Message code
	int			  iManager;		//	Receiver manager's ID
	int			  iSubID;			//	Sub ID
	DWORD		  dwParam1;		//	Parameter 1
	DWORD	  	  dwParam2;		//	Parameter 2
	DWORD		  dwParam3;		//	Parameter 3
	DWORD		  dwParam4;		//	Parameter 4
	MsgDataBase*  pMsgData;		//	msg data structure for speical data

public:
	ECMSG();
	~ECMSG();	
	ECMSG( const ECMSG &rhs);
	ECMSG & operator = (const ECMSG &rhs);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

