// File		: EC_ServerSimulate.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/6

#include "EC_ServerSimulate.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_RTDebug.h"
#include "EC_Split.h"

#include "gnproto.h"

#include <AChar.h>

extern CECGame * g_pGame;

using namespace GNET;

//	辅助类型
//	将字符串转换成协议中的数据类型
class CECProtocolDataType : public Marshal
{
public:
	virtual ~CECProtocolDataType(){}
	virtual bool From(const ACHAR *)=0;
};
class CECProtocolDataTypeFactory
{
private:
	CECProtocolDataTypeFactory(){}
	CECProtocolDataTypeFactory (const CECProtocolDataTypeFactory &);
	CECProtocolDataTypeFactory & operator=(const CECProtocolDataTypeFactory &);
public:
	static CECProtocolDataTypeFactory & Instance(){
		static CECProtocolDataTypeFactory s_instance;
		return s_instance;
	}
	CECProtocolDataType * Create(const AString &s)
	{
		CECProtocolDataType *pData = NULL;
		CECSplitHelperW splitter(AS2WC(s), L":");
		if (splitter.Count() == 2){
			if (pData = CreateEmptyDataType(splitter.ToString(0))){
				if (!pData->From(splitter.ToString(1))){
					delete pData;
					pData = NULL;
				}
			}
		}
		return pData;
	}
private:
	CECProtocolDataType * CreateEmptyDataType(const AWString &ws);
};

//	class CECServerSimulate
CECServerSimulate & CECServerSimulate::Instance()
{
	static CECServerSimulate s_instance;
	return s_instance;
}

bool CECServerSimulate::Come(GNET::Marshal::OctetsStream &data)
{
	bool bOK(false);

	Protocol *protocol = NULL;
	Protocol::Type type = 0;
	try
	{
		data >> type;
		if (protocol = Protocol::Create(type)){
			data >> *protocol;
			g_pGame->GetGameSession()->AddNewProtocol(protocol);
			bOK = true;
		}else{
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Unknown type=%d"), type);
		}
	}
	catch (Marshal::Exception &)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Unmarshal exception"));
		if (protocol){
			protocol->Destroy();
			protocol = NULL;
		}
	}
	return bOK;
}

bool CECServerSimulate::Come(const AString *pTokens, int nToken)
{
	if (pTokens != NULL && nToken > 0){
		GNET::Marshal::OctetsStream data;
		Protocol::Type type = atoi(pTokens[0]);
		data << type;
		for (int i(1); i < nToken; ++ i)
		{
			if (CECProtocolDataType *pData = CECProtocolDataTypeFactory::Instance().Create(pTokens[i])){
				data << *pData;
				delete pData;
			}else{
				g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Invalid parameter at %d"), i);
				return false;
			}
		}
		return Come(data);
	}else{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Need protocol type"));
	}
	return false;
}

//	将字符串转换成整型
template <class T>
class CECProtocolInt : public CECProtocolDataType
{
	T		m_data;
public:
	CECProtocolInt() : m_data(0) {}
	virtual bool From(const ACHAR *s)
	{
		m_data = static_cast<T>(a_atoi(s));
		return true;
	}
	virtual Marshal::OctetsStream& marshal(Marshal::OctetsStream & os)const
	{
		os << m_data;
		return os;
	}
	virtual const Marshal::OctetsStream& unmarshal(const Marshal::OctetsStream &os)
	{
		os >> m_data;
		return os;
	}
};

//	CECProtocolDataTypeFactory 实现
CECProtocolDataType * CECProtocolDataTypeFactory::CreateEmptyDataType(const AWString &ws)
{
	if (ws == L"i"){
		return new CECProtocolInt<int>();
	}
	if (ws == L"s"){
		return new CECProtocolInt<short>();
	}
	if (ws == L"ui"){
		return new CECProtocolInt<unsigned int>();
	}
	if (ws == L"c"){
		return new CECProtocolInt<char>();
	}
	return NULL;
}