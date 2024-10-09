// File		: EC_UIConfig.h
// Creator	: Xu Wenbin
// Date		: 2012/5/14

#pragma once
#include <vector.h>
#include <hashmap.h>
#include <AAssist.h>
#include <time.h>

//	configs/uiconfig.ini 文件配置结果
class CECUIConfig
{
private:
	CECUIConfig(){}
	CECUIConfig(const CECUIConfig &);
	CECUIConfig & operator == (const CECUIConfig &);

public:

	void	Load(const char *filePath);

	//	与登录密切相关的配置
	struct LoginUI 
	{
		bool	bEnableOtherLogin;		//	开启第三方账号接入登录方式
		bool	bAlwaysTryAutoLogin;	//	每次登录总是尝试使用自动登录
		bool	bEnableForceLogin;		//	登录界面始终打开强制登录开关
		bool	bSkipFirstLoadProgress;	//	首次进入游戏不显示加载进度条（配合自动登录、实现进入游戏即看到登录列表）
		bool	bAvoidLoginUI;			//	避免回到登录界面（登录失败或掉线后退出客户端）
		ACString strHotServerGroupName;	//	火爆新服文字显示
		int		nMaxRecordAccount;		//	最多记录成功登录过的账号名称
		int		nMinInputToRemindAccount;	//	提醒输入账号最少需输入账号字符数

		LoginUI(): bEnableOtherLogin(true)
			, bAlwaysTryAutoLogin(false)
			, bEnableForceLogin(false)
			, bSkipFirstLoadProgress(false)
			, bAvoidLoginUI(false)
			, strHotServerGroupName(_AL("Hot"))
			, nMaxRecordAccount(100)
			, nMinInputToRemindAccount(5)
		{}
	};
	struct LoginUI m_loginUI;
	LoginUI & GetLoginUI() { return m_loginUI; }
	const LoginUI & GetLoginUI() const { return m_loginUI; }

	// 用来清除随机地图M图遮罩层的物品
	struct RandomMapItem
	{
		RandomMapItem(){ itemID = 0; count = 0;}
		int itemID;
		int count;
	} ;

	//	与游戏逻辑密切相关的配置
	struct GameUI 
	{
		bool	bMailToFriendsSwitch;			//	开启千里传情
		int		nMailToFriendsDaysNoLogin;		//  好友未登陆天数，超过这个值给提醒
		int		nMailToFriendsLevel;			//  使用千里传情的级别限制
		int		nMailToFriendsDaysSendMail;		//  发送召回邮件超过这个值可以再次发送

		bool	bActivityReminder;				//  开启女王卡片等内置IE活动等
		int		nActivityReminderLevel;			//	等级限制
		int		nActivityReminderMaxLevelSoFar;	//	历史最高等级限制
		int		nActivityReminderLevel2;		//	修真等级限制
		int		nActivityReminderReincarnationTimes;	//	转生次数限制
		int		nActivityReminderRealmLevel;	//	境界等级限制
		int		nActivityReminderReputation;	//	声望限制
		struct tm tActivityReminderStartTime;	//	开始时间（服务器时间）
		struct tm tActivityReminderEndTime;		//	结束时间（服务器时间）
		
		bool	bEnableTalkToGM;				//	开启玩家向GM发送非标准信息
		bool	bEnableTrashPwdRemind;			//	开启提醒玩家仓库密码没有设置
		
		enum WallowHintType // wallow hint info type
		{
			WHT_DEFAULT = 0,					//	默认方式
			WHT_KOREA = 1,						//	韩国方式
		};
		WallowHintType	nWallowHintType;		//	防沉迷提示类型

		bool	bEnableIE;						//	使用内置 IE 显示网页内容
		bool	bEnableShowIP;					//	是否显示上次登录信息
		bool	bEnableCompleteAccount;			//	是否显示账号补填信息
		bool	bEnableFortressBuildDestroy;	//	是否允许回收帮派基地设施
		abase::vector<int>	nCountryWarBonus;	//	国战奖励分级
		bool	bShowNameInCountryWar;			//	国战战斗场景显示其它国家玩家名称等
		int		nCountryWarEnterLevel;			//	加入国战等级限制
		int		nCountryWarEnterItem;			//	加入国战消耗物品id
		int		nCountryWarEnterItemCount;		//	加入国战消耗物品个数
		bool	bEnableQuickPay;				//	开启快捷支付
		int		nEquipMarkMinInkNum;			//	装备签名修改最少需要的墨水数量
		bool    bEnableReportPlayerSpeakToGM;   //  开启玩家向GM举报另外的玩家的发言信息
		bool	bEnableReportPluginWithFeedback;//	开启有系统反馈的举报外挂
		unsigned int nGTLoginCoolDown;			//	游戏内 GT 语音登录冷却时长（毫秒）
		bool	bEnableGTOnSpecialServer;		//	跨服中央服上是否启用GT
		int		nCrossServerEnterLevel;			//	加入跨服等级限制
		int		nCrossServerEnterLevel2;		//	加入跨服修真限制
		bool	bEnableWebTradeSort;			//	寻宝网浏览商品时排序点击访问寻宝网网页
		bool    bEnablePlayerRename;            //  开启玩家改名服务
		bool	bEnableCheckNewbieGift;			//	开启补绑新手卡提示
		bool    bEnableQShopFilter;				//	开启商城显示过滤功能（按照玩家等级和配置）
		bool    bEnableGivingFor;               //  开启商城赠送和索取功能
		bool    bEnableGivingForTaskLimitedItem;//  开启商城某些任务限制物品的赠送和索取功能
		abase::vector<int>	nCountryWarPlayerLimit;	//	国战各战场单方人数限制
		int		nCountryWarKingMaxDomainLimit;		//	国王条件限制
		int		nFashionSplitCost;				//	时装拆分消耗金币
		bool	bEnableTouch;					//	Touch 商城开关
		bool	bEnableOptimize;				//  是否开启客户端优化功能
		int		nMemoryUsageLow;				//  低于此值，系统将自动恢复精简的模型
		int		nMemoryUsageHigh;				//  高于此值，系统将按优先级精简模型
		int		nAutoSimplifySpeed;				//  相隔多长时间优化一个玩家
		abase::vector<int>	nTouchEnabledMap;	//	TOUCH 商城开启的地图
		bool	bEnableTWRecharge;				//	商城充值按钮将以特定参数打开台湾给出的链接
		AString strTWRechargeAppID;				//	台湾充值链接中参数 AppID（合作商代码）
		AString	strTWRechargeGame;				//	台湾充值链接中参数 Game（游戏类别）
		AString	strTWRechargeKey;				//	台湾充值链接中参数 Key（链接辅助校验参数）
		bool	bEnableTitle;					//  开启Title相关UI
		abase::vector<int> nAutoTeamTransmitEnabledMap;	//  活动界面上，可以进行传送的地图ID
		int		nChariotApplyLevel;					//	战车条件， 等级；
		int		nChariotApplyLevel2;				//	战车条件， 修真
		int		nChariotApplyReincarnation;			//	战车条件， 转生次数
		int		nChariotReviveTimeout;				//	战车复活超时时间
		int		nChariotAmount;						//	战车战场战车总数量
		int		nHistoryQueryTimeInterval;			//	查询历史变量时间间隔
		bool	bEnableAutoWiki;					//	智能百科开关
		int		nExitAutoExtractWikiStateTime;		//	智能百科: 退出自动抓取； 
		int		nCloseWikiPopDlgTime;				//	智能百科: 关闭时间；
		int		nCloseWikiMsgInfoTime;				//	智能百科: 文字消失时间
		int		nOpenWikiPopDlgTime;				//	智能百科: 启时间
		abase::vector<int> nTaskDisabledInMiniClient;	//	迷你客户端中禁用的任务
		abase::vector<int> nItemDisabledInMiniClient;	//	迷你客户端中禁用的物品
		abase::vector<int> nMeridianFreeItem;			//	灵脉：玄鉴丹ID列表
		abase::vector<int> nMeridianNotFreeItem;		//	灵脉：决胜丹ID列表
		int		nMonsterSpiritGatherTimesPerWeekMax;	//	每周能采取元魂的最大次数
		bool	bEnableAutoPolicy;						//	自动修炼系统开关
		bool	bEnablePWService;						//	完美服务界面开关
		bool	bEnableActionSwitch;					//	动作切换开关： 骑乘->飞行，飞行->骑乘，使用技能前自动召回骑宠
		int		nCountryWarLiveShowUpdateInterval;		//	国战排名、死亡数据更新间隔
		abase::vector<int> nDefaultSystemModuleIndex;	//	系统快捷栏默认功能index
		bool	bEnableRecommendQShopItem;				//	在适当时候推荐商城物品
		abase::vector<int> nRecommendShopItems;			//	推荐的商城物品配置
		bool	bEnableRandShop;						//	是否开启随机商城（在韩国版本和南美版本中因协议原因、必须禁止）
		int		nPokerShopConfig;						//	战灵商城配置表ID
		int		nPokerShopLevelLimit;					//	战灵商城使用等级限制
		int		nContributionTaskLevelLimit;			//	活跃值任务系统使用等级限制
		abase::vector<AString> strFashionShopAdImage;	//	时装商城广告图片
		bool	bEnableQShopFashionShop;				//	乾坤袋时装商城启动控制
		bool	bEnableBackShopFashionShop;				//	鸿利商城时装商城启动控制
		bool	bEnableCeilPriceBeforeDiscountToGold;	//	商城打折商品原价只显示向上取整后的黄金（使原价看起来更合理更有诚意）
		abase::vector<AString> strFullScreenGfxForeground;//全屏效果前景特效
		abase::vector<AString> strFullScreenGfxBackground;//全屏效果背景特效
		abase::vector<int> nRandomMaps;					// 随机地图号
		abase::vector<int> nTaskIDForDisableWayPointUITips; // 身上有这些任务时，需要关闭传送点的tip提示框
		RandomMapItem DefaultRandomMapItem; // 默认用来清除随机地图M图遮罩层的物品
		abase::hash_map<int,RandomMapItem> SpecialRandomMapItems; // 特定随机地图用来清除随机地图M图遮罩层的物品
		int		nMaxFriendRemarksNameLength;			//	好友备注文字长度限制
		bool	bEnableQShopFashionShopFlashSale;			//	乾坤袋时装商城启动控制
		bool	bEnableBackShopFashionShopFlashSale;		//	鸿利商城时装商城启动控制
		ACString	strQShopFashionShopFlashSaleTitle;		//	乾坤袋时装商城闪购按钮标题
		ACString	strBackShopFashionShopFlashSaleTitle;	//	鸿利商城时装商城闪购按钮标题
		bool    bEnablePlayerChangeGender;				//  开启玩家变性服务

		GameUI();

		int	GetCountryWarBonusLevel(int currentBonus)const;
		int	GetCountryWarPlayerLimit(int warType)const;
		bool GetCanShowTouchShop(int idInst)const;
		bool IsTaskDisabledInMiniClient(int task_id) const;
		bool IsItemDisabledInMiniClient(int item_id) const;
		bool IsMeridianFreeItem(int item_id) const;
		bool IsMeridianNotFreeItem(int item_id) const;
		bool IsRandomMap(int mapid) const;
		int GetRandomMapCount() const { return nRandomMaps.size();}
		int GetRandomMapID(int idx) { return nRandomMaps[idx];}
		bool GetRandomMapItemInfo(int mapID,RandomMapItem& info);
		int GetTaskIDDisableWayPointsUITipsCount() { return nTaskIDForDisableWayPointUITips.size();}
		int GetTaskIDDisableWayPointsUITips(int idx) { return nTaskIDForDisableWayPointUITips[idx];}
		

		enum RecommendShopItemType{
			RECOMMEND_REFINE_1,		//	装备精炼道具
			RECOMMEND_REFINE_2,		//	装备精炼道具
			RECOMMEND_REFINE_3,		//	装备精炼道具
			RECOMMEND_REFINE_4,		//	装备精炼道具
			RECOMMEND_REFINE_5,		//	装备精炼道具
			RECOMMEND_SAVE_LIFE,	//	保命符
			RECOMMEND_ACTIVITY,		//	活动即刻前往
		};
		int	GetRecommendShopItem(int type)const;
	};
	struct GameUI m_gameUI;
	GameUI & GetGameUI() {return m_gameUI;}
	const GameUI & GetGameUI()const {return m_gameUI;}

	static CECUIConfig & Instance();
};

//	configs/qshopitem.ini 文件配置结果
class CECQShopConfig
{
private:
	CECQShopConfig();
	CECQShopConfig(const CECQShopConfig &);
	CECQShopConfig & operator == (const CECQShopConfig &);

	// 商品按等级过滤条件
	struct LevelFilterCondition
	{
		int minLevel;
		int maxLevel;

		LevelFilterCondition()
		{
			minLevel = 1;
			maxLevel = 1;
		}

		bool Meet(int level)const
		{
			return level >= minLevel && level <= maxLevel;
		}
	};
	typedef abase::hash_map<int, LevelFilterCondition> LevelFilterMap;	// 从商品ID到范围的映射，以加速查询
	LevelFilterMap m_levelFilters;
	
	//	商品按任务限制过滤条件
	typedef abase::hash_map<int, bool> IDFilterMap;
	IDFilterMap	m_idFilters;

	//	程序需识别的商品逻辑分类
	struct CategoryPosition 
	{
		int	mainType;
		int	subType;
		CategoryPosition()
		{
			mainType = 0;
			subType = 0;
		}
		CategoryPosition(int m, int s): mainType(m), subType(s){}
	};
	typedef abase::hash_map<int, CategoryPosition>	CategoryMap;
	CategoryMap	m_catMap;

public:

	bool	Load(const char *filePath);
	bool	IsFilteredByLevel(int id, int myLevel)const;

	bool	IsFilteredByID(int id)const;
	bool	CanFilterID(int id)const;
	void	OnItemBuyed(int id);
	void	ClearBuyedItem();

	enum	CategoryID
	{
		CID_REFINE,				//	精炼相关
		CID_QSHOP_FASHION,		//	乾坤袋时装相关
		CID_BACKSHOP_FASHION,	//	鸿利商城时装相关
	};
	bool	FindCategory(int idCategory, int &mainType, int &subType)const;

	static CECQShopConfig & Instance();
};
