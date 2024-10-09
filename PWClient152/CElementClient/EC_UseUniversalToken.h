// File		: EC_UseUniversalToken.h
// Creator	: Xu Wenbin
// Date		: 2014/4/11

#ifndef _ELEMENTCLIENT_EC_USEUNIVERSALTOKEN_H_
#define _ELEMENTCLIENT_EC_USEUNIVERSALTOKEN_H_

#include "EC_Handler.h"
#include <vector.h>
#include <ABaseDef.h>

struct UNIVERSAL_TOKEN_ESSENCE;
class CECUseUniversalTokenCommand
{
protected:
	const UNIVERSAL_TOKEN_ESSENCE *	m_pDBEssence;
	int								m_usageIndex;

	const UNIVERSAL_TOKEN_ESSENCE *	GetDBEssence()const{ return m_pDBEssence; }
	int	GetUsageIndex()const{ return m_usageIndex; }
public:
	CECUseUniversalTokenCommand(const UNIVERSAL_TOKEN_ESSENCE *pDBEssence, int usageIndex)
		: m_pDBEssence(pDBEssence)
		, m_usageIndex(usageIndex){
	}
	virtual ~CECUseUniversalTokenCommand(){}
	virtual bool UseAndNeedFeedBack()=0;									//	���� true ʱ�����ɹ�ʹ�ã�������Ҫ�ȴ����� FeedBack��Ϊ false ʱ����ʹ��ʧ�ܡ���ɹ�������Ҫ FeedBack
	virtual bool FeedBack(int usageIndex, const void *p){ return false; }	//	ʹ�����˷������������Э�鷵�أ�
};

class CECIvtrUniversalToken;
class CECUseUniversalTokenCommandManager : public CECSSOTicketHandler
{
	struct UseCommandConfig{
		CECUseUniversalTokenCommand *	m_pCommand;
		DWORD							m_useTime;
		DWORD							m_maxUseTime;

		UseCommandConfig(CECUseUniversalTokenCommand *pCmd, DWORD maxUseTime);
		bool IsTimeUp(DWORD currentTime)const;
		void DeleteCommand();
	};
	typedef abase::vector<UseCommandConfig>	InUseCommands;
	InUseCommands		m_inUseCommands;

	CECUseUniversalTokenCommandManager();

	CECUseUniversalTokenCommandManager(const CECUseUniversalTokenCommandManager &);
	CECUseUniversalTokenCommandManager & operator=(const CECUseUniversalTokenCommandManager &);

	CECUseUniversalTokenCommand * Create(const UNIVERSAL_TOKEN_ESSENCE *pDBEssence, int usageIndex);

public:

	static CECUseUniversalTokenCommandManager & Instance();

	virtual void HandleRequest(const CECSSOTicketHandler::Request *p);
	bool Use(CECIvtrUniversalToken *pItem, int usageIndex, DWORD maxUseTime=5000);
	void Clear();
	void Tick();
};

#endif	//	_ELEMENTCLIENT_EC_USEUNIVERSALTOKEN_H_