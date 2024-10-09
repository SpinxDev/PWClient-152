// File		: EC_UIConfig.h
// Creator	: Xu Wenbin
// Date		: 2012/5/14

#pragma once
#include <vector.h>
#include <hashmap.h>
#include <AAssist.h>
#include <time.h>

//	configs/uiconfig.ini �ļ����ý��
class CECUIConfig
{
private:
	CECUIConfig(){}
	CECUIConfig(const CECUIConfig &);
	CECUIConfig & operator == (const CECUIConfig &);

public:

	void	Load(const char *filePath);

	//	���¼������ص�����
	struct LoginUI 
	{
		bool	bEnableOtherLogin;		//	�����������˺Ž����¼��ʽ
		bool	bAlwaysTryAutoLogin;	//	ÿ�ε�¼���ǳ���ʹ���Զ���¼
		bool	bEnableForceLogin;		//	��¼����ʼ�մ�ǿ�Ƶ�¼����
		bool	bSkipFirstLoadProgress;	//	�״ν�����Ϸ����ʾ���ؽ�����������Զ���¼��ʵ�ֽ�����Ϸ��������¼�б�
		bool	bAvoidLoginUI;			//	����ص���¼���棨��¼ʧ�ܻ���ߺ��˳��ͻ��ˣ�
		ACString strHotServerGroupName;	//	���·�������ʾ
		int		nMaxRecordAccount;		//	����¼�ɹ���¼�����˺�����
		int		nMinInputToRemindAccount;	//	���������˺������������˺��ַ���

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

	// ������������ͼMͼ���ֲ����Ʒ
	struct RandomMapItem
	{
		RandomMapItem(){ itemID = 0; count = 0;}
		int itemID;
		int count;
	} ;

	//	����Ϸ�߼�������ص�����
	struct GameUI 
	{
		bool	bMailToFriendsSwitch;			//	����ǧ�ﴫ��
		int		nMailToFriendsDaysNoLogin;		//  ����δ��½�������������ֵ������
		int		nMailToFriendsLevel;			//  ʹ��ǧ�ﴫ��ļ�������
		int		nMailToFriendsDaysSendMail;		//  �����ٻ��ʼ��������ֵ�����ٴη���

		bool	bActivityReminder;				//  ����Ů����Ƭ������IE���
		int		nActivityReminderLevel;			//	�ȼ�����
		int		nActivityReminderMaxLevelSoFar;	//	��ʷ��ߵȼ�����
		int		nActivityReminderLevel2;		//	����ȼ�����
		int		nActivityReminderReincarnationTimes;	//	ת����������
		int		nActivityReminderRealmLevel;	//	����ȼ�����
		int		nActivityReminderReputation;	//	��������
		struct tm tActivityReminderStartTime;	//	��ʼʱ�䣨������ʱ�䣩
		struct tm tActivityReminderEndTime;		//	����ʱ�䣨������ʱ�䣩
		
		bool	bEnableTalkToGM;				//	���������GM���ͷǱ�׼��Ϣ
		bool	bEnableTrashPwdRemind;			//	����������Ҳֿ�����û������
		
		enum WallowHintType // wallow hint info type
		{
			WHT_DEFAULT = 0,					//	Ĭ�Ϸ�ʽ
			WHT_KOREA = 1,						//	������ʽ
		};
		WallowHintType	nWallowHintType;		//	��������ʾ����

		bool	bEnableIE;						//	ʹ������ IE ��ʾ��ҳ����
		bool	bEnableShowIP;					//	�Ƿ���ʾ�ϴε�¼��Ϣ
		bool	bEnableCompleteAccount;			//	�Ƿ���ʾ�˺Ų�����Ϣ
		bool	bEnableFortressBuildDestroy;	//	�Ƿ�������հ��ɻ�����ʩ
		abase::vector<int>	nCountryWarBonus;	//	��ս�����ּ�
		bool	bShowNameInCountryWar;			//	��սս��������ʾ��������������Ƶ�
		int		nCountryWarEnterLevel;			//	�����ս�ȼ�����
		int		nCountryWarEnterItem;			//	�����ս������Ʒid
		int		nCountryWarEnterItemCount;		//	�����ս������Ʒ����
		bool	bEnableQuickPay;				//	�������֧��
		int		nEquipMarkMinInkNum;			//	װ��ǩ���޸�������Ҫ��īˮ����
		bool    bEnableReportPlayerSpeakToGM;   //  ���������GM�ٱ��������ҵķ�����Ϣ
		bool	bEnableReportPluginWithFeedback;//	������ϵͳ�����ľٱ����
		unsigned int nGTLoginCoolDown;			//	��Ϸ�� GT ������¼��ȴʱ�������룩
		bool	bEnableGTOnSpecialServer;		//	�����������Ƿ�����GT
		int		nCrossServerEnterLevel;			//	�������ȼ�����
		int		nCrossServerEnterLevel2;		//	��������������
		bool	bEnableWebTradeSort;			//	Ѱ���������Ʒʱ����������Ѱ������ҳ
		bool    bEnablePlayerRename;            //  ������Ҹ�������
		bool	bEnableCheckNewbieGift;			//	�����������ֿ���ʾ
		bool    bEnableQShopFilter;				//	�����̳���ʾ���˹��ܣ�������ҵȼ������ã�
		bool    bEnableGivingFor;               //  �����̳����ͺ���ȡ����
		bool    bEnableGivingForTaskLimitedItem;//  �����̳�ĳЩ����������Ʒ�����ͺ���ȡ����
		abase::vector<int>	nCountryWarPlayerLimit;	//	��ս��ս��������������
		int		nCountryWarKingMaxDomainLimit;		//	������������
		int		nFashionSplitCost;				//	ʱװ������Ľ��
		bool	bEnableTouch;					//	Touch �̳ǿ���
		bool	bEnableOptimize;				//  �Ƿ����ͻ����Ż�����
		int		nMemoryUsageLow;				//  ���ڴ�ֵ��ϵͳ���Զ��ָ������ģ��
		int		nMemoryUsageHigh;				//  ���ڴ�ֵ��ϵͳ�������ȼ�����ģ��
		int		nAutoSimplifySpeed;				//  ����೤ʱ���Ż�һ�����
		abase::vector<int>	nTouchEnabledMap;	//	TOUCH �̳ǿ����ĵ�ͼ
		bool	bEnableTWRecharge;				//	�̳ǳ�ֵ��ť�����ض�������̨�����������
		AString strTWRechargeAppID;				//	̨���ֵ�����в��� AppID�������̴��룩
		AString	strTWRechargeGame;				//	̨���ֵ�����в��� Game����Ϸ���
		AString	strTWRechargeKey;				//	̨���ֵ�����в��� Key�����Ӹ���У�������
		bool	bEnableTitle;					//  ����Title���UI
		abase::vector<int> nAutoTeamTransmitEnabledMap;	//  ������ϣ����Խ��д��͵ĵ�ͼID
		int		nChariotApplyLevel;					//	ս�������� �ȼ���
		int		nChariotApplyLevel2;				//	ս�������� ����
		int		nChariotApplyReincarnation;			//	ս�������� ת������
		int		nChariotReviveTimeout;				//	ս�����ʱʱ��
		int		nChariotAmount;						//	ս��ս��ս��������
		int		nHistoryQueryTimeInterval;			//	��ѯ��ʷ����ʱ����
		bool	bEnableAutoWiki;					//	���ܰٿƿ���
		int		nExitAutoExtractWikiStateTime;		//	���ܰٿ�: �˳��Զ�ץȡ�� 
		int		nCloseWikiPopDlgTime;				//	���ܰٿ�: �ر�ʱ�䣻
		int		nCloseWikiMsgInfoTime;				//	���ܰٿ�: ������ʧʱ��
		int		nOpenWikiPopDlgTime;				//	���ܰٿ�: ��ʱ��
		abase::vector<int> nTaskDisabledInMiniClient;	//	����ͻ����н��õ�����
		abase::vector<int> nItemDisabledInMiniClient;	//	����ͻ����н��õ���Ʒ
		abase::vector<int> nMeridianFreeItem;			//	������������ID�б�
		abase::vector<int> nMeridianNotFreeItem;		//	��������ʤ��ID�б�
		int		nMonsterSpiritGatherTimesPerWeekMax;	//	ÿ���ܲ�ȡԪ���������
		bool	bEnableAutoPolicy;						//	�Զ�����ϵͳ����
		bool	bEnablePWService;						//	����������濪��
		bool	bEnableActionSwitch;					//	�����л����أ� ���->���У�����->��ˣ�ʹ�ü���ǰ�Զ��ٻ����
		int		nCountryWarLiveShowUpdateInterval;		//	��ս�������������ݸ��¼��
		abase::vector<int> nDefaultSystemModuleIndex;	//	ϵͳ�����Ĭ�Ϲ���index
		bool	bEnableRecommendQShopItem;				//	���ʵ�ʱ���Ƽ��̳���Ʒ
		abase::vector<int> nRecommendShopItems;			//	�Ƽ����̳���Ʒ����
		bool	bEnableRandShop;						//	�Ƿ�������̳ǣ��ں����汾�������汾����Э��ԭ�򡢱����ֹ��
		int		nPokerShopConfig;						//	ս���̳����ñ�ID
		int		nPokerShopLevelLimit;					//	ս���̳�ʹ�õȼ�����
		int		nContributionTaskLevelLimit;			//	��Ծֵ����ϵͳʹ�õȼ�����
		abase::vector<AString> strFashionShopAdImage;	//	ʱװ�̳ǹ��ͼƬ
		bool	bEnableQShopFashionShop;				//	Ǭ����ʱװ�̳���������
		bool	bEnableBackShopFashionShop;				//	�����̳�ʱװ�̳���������
		bool	bEnableCeilPriceBeforeDiscountToGold;	//	�̳Ǵ�����Ʒԭ��ֻ��ʾ����ȡ����Ļƽ�ʹԭ�ۿ�������������г��⣩
		abase::vector<AString> strFullScreenGfxForeground;//ȫ��Ч��ǰ����Ч
		abase::vector<AString> strFullScreenGfxBackground;//ȫ��Ч��������Ч
		abase::vector<int> nRandomMaps;					// �����ͼ��
		abase::vector<int> nTaskIDForDisableWayPointUITips; // ��������Щ����ʱ����Ҫ�رմ��͵��tip��ʾ��
		RandomMapItem DefaultRandomMapItem; // Ĭ��������������ͼMͼ���ֲ����Ʒ
		abase::hash_map<int,RandomMapItem> SpecialRandomMapItems; // �ض������ͼ������������ͼMͼ���ֲ����Ʒ
		int		nMaxFriendRemarksNameLength;			//	���ѱ�ע���ֳ�������
		bool	bEnableQShopFashionShopFlashSale;			//	Ǭ����ʱװ�̳���������
		bool	bEnableBackShopFashionShopFlashSale;		//	�����̳�ʱװ�̳���������
		ACString	strQShopFashionShopFlashSaleTitle;		//	Ǭ����ʱװ�̳�������ť����
		ACString	strBackShopFashionShopFlashSaleTitle;	//	�����̳�ʱװ�̳�������ť����
		bool    bEnablePlayerChangeGender;				//  ������ұ��Է���

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
			RECOMMEND_REFINE_1,		//	װ����������
			RECOMMEND_REFINE_2,		//	װ����������
			RECOMMEND_REFINE_3,		//	װ����������
			RECOMMEND_REFINE_4,		//	װ����������
			RECOMMEND_REFINE_5,		//	װ����������
			RECOMMEND_SAVE_LIFE,	//	������
			RECOMMEND_ACTIVITY,		//	�����ǰ��
		};
		int	GetRecommendShopItem(int type)const;
	};
	struct GameUI m_gameUI;
	GameUI & GetGameUI() {return m_gameUI;}
	const GameUI & GetGameUI()const {return m_gameUI;}

	static CECUIConfig & Instance();
};

//	configs/qshopitem.ini �ļ����ý��
class CECQShopConfig
{
private:
	CECQShopConfig();
	CECQShopConfig(const CECQShopConfig &);
	CECQShopConfig & operator == (const CECQShopConfig &);

	// ��Ʒ���ȼ���������
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
	typedef abase::hash_map<int, LevelFilterCondition> LevelFilterMap;	// ����ƷID����Χ��ӳ�䣬�Լ��ٲ�ѯ
	LevelFilterMap m_levelFilters;
	
	//	��Ʒ���������ƹ�������
	typedef abase::hash_map<int, bool> IDFilterMap;
	IDFilterMap	m_idFilters;

	//	������ʶ�����Ʒ�߼�����
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
		CID_REFINE,				//	�������
		CID_QSHOP_FASHION,		//	Ǭ����ʱװ���
		CID_BACKSHOP_FASHION,	//	�����̳�ʱװ���
	};
	bool	FindCategory(int idCategory, int &mainType, int &subType)const;

	static CECQShopConfig & Instance();
};
