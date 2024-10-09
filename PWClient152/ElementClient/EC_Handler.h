// File		: EC_Handler.h
// Creator	: Xu Wenbin
// Date		: 2014/4/11

#ifndef _ELEMENTCLIENT_EC_HANDLER_H_
#define _ELEMENTCLIENT_EC_HANDLER_H_

#include <vector.h>

//	class CECHandler
//	责任链基类：针对数据 Model
template <typename T>
class CECHandler
{
	CECHandler	* m_pSuccessor;
public:
	typedef T Request; 
	CECHandler(CECHandler *pSuccessor=NULL): m_pSuccessor(pSuccessor){}
	void SetSuccessor(CECHandler *pSuccessor){
		m_pSuccessor = pSuccessor;
	}
	virtual ~CECHandler(){}

	virtual void HandleRequest(const Request *p){
		if (m_pSuccessor){
			m_pSuccessor->HandleRequest(p);
		}
	}
};

//	class CECHandlerChainConstructor
//	辅助构建 CECHandler 处理链
template <typename T>
class CECHandlerChainConstructor
{
public:
	typedef CECHandler<T>							CECConcreteHandler;
	typedef CECConcreteHandler	*					CECConcreteHandlerPtr;
private:
	typedef abase::vector<CECConcreteHandlerPtr>	HandlerChains;
	HandlerChains									m_handlers;
public:
	void Append(CECConcreteHandlerPtr p){
		if (p){
			m_handlers.push_back(p);
		}
	}
	CECConcreteHandlerPtr Construct(){
		CECConcreteHandlerPtr pHead = NULL;
		if (!m_handlers.empty()){
			for (size_t i(0); i+1 < m_handlers.size(); ++ i)
			{
				CECConcreteHandlerPtr pCurrent = m_handlers[i];
				CECConcreteHandlerPtr pNext = m_handlers[i+1];
				pCurrent->SetSuccessor(pNext);
			}
			m_handlers.back()->SetSuccessor(NULL);
			pHead = m_handlers.front();
		}
		return pHead;
	}
};

//	SSOGetTicket_Re 协议的处理基类
namespace GNET{
	class SSOGetTicket_Re;
}
typedef CECHandler<GNET::SSOGetTicket_Re> CECSSOTicketHandler;

/*
样例代码	
#include "EC_Handler.h"

typedef CECHandler<int>	CECIntHandler;

class CECOddNumberHandler : public CECIntHandler
{
public:
	CECOddNumberHandler(CECIntHandler *pSuccessor) : CECIntHandler(pSuccessor) {}
	virtual void HandleRequest(const int *p){
		a_LogOutput(1, "In CECOddNumberHandler...");
		if (p && (*p%2)!=0){
			a_LogOutput(1, "Processed:%d", *p);
		}else{
			CECIntHandler::HandleRequest(p);
		}
	}
};

class CECEvenNumberHandler : public CECIntHandler
{
public:
	CECEvenNumberHandler(CECIntHandler *pSuccessor) : CECIntHandler(pSuccessor) {}
	virtual void HandleRequest(const int *p){
		a_LogOutput(1, "In CECEvenNumberHandler...");
		if (p && (*p%2)==0){
			a_LogOutput(1, "Processed:%d", *p);
		}else{
			CECIntHandler::HandleRequest(p);
		}
	}
};

class CECInvalidNumberHandler : public CECIntHandler
{
public:
	CECInvalidNumberHandler() : CECIntHandler(NULL) {}
	virtual void HandleRequest(const int *p){
		a_LogOutput(1, "In CECInvalidNumberHandler...");
		a_LogOutput(1, "Processed:p is invalid");
	}
};
  
CECInvalidNumberHandler invalidHandler;
CECOddNumberHandler		oddHandler(&invalidHandler);
CECEvenNumberHandler	evenHandler(&oddHandler);
int number = 1;
evenHandler.HandleRequest(&number);
number = 2;
evenHandler.HandleRequest(&number);
evenHandler.HandleRequest(NULL);
*/

#endif	//	_ELEMENTCLIENT_EC_HANDLER_H_