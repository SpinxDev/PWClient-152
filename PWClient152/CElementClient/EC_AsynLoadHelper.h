// Filename	: EC_AsynLoadHelper.h
// Creator	: Xu Wenbin
// Date		: 2014/11/28

#pragma once

#include <vector.h>

//	以下类用于多线程模型加载过程记录、控制，可用于减少重复加载等

//	加载标识：用于惟一定位，可用于查询判断、删除等用途
class CECAsynLoadID{
	int				m_loadType;		//	加载类型
	unsigned int	m_loadCounter;	//	加载计数
	unsigned long	m_requestTime;	//	请求加载时刻
public:
	CECAsynLoadID()
		: m_loadType(-1)
		, m_loadCounter(0)
		, m_requestTime(0)
	{
	}
	CECAsynLoadID(int type, int counter, unsigned long requestTime)
		: m_loadType(type)
		, m_loadCounter(counter)
		, m_requestTime(requestTime)
	{}
	int  GetLoadType()const{
		return m_loadType;
	}
	int  GetLoadCounter()const{
		return m_loadCounter;
	}
	unsigned long GetRequestTime()const{
		return m_requestTime;
	}
	bool operator == (const CECAsynLoadID &rhs)const{
		return GetLoadType() == rhs.GetLoadType()
			&& GetLoadCounter() == rhs.GetLoadCounter()
			&& GetRequestTime() == rhs.GetRequestTime();
	}
	bool IsValid()const{
		return m_requestTime > 0;
	}
};

//	加载标识生产工厂，单线程，同一工厂生产的才保证惟一性
class CECAsynLoadIDFactory{
	unsigned int	m_nextLoadCounter;
	
	//	禁用
	CECAsynLoadIDFactory(const CECAsynLoadIDFactory &);
	CECAsynLoadIDFactory & operator = (const CECAsynLoadIDFactory &);
public:
	CECAsynLoadIDFactory();
	CECAsynLoadID Generate(int loadType);
};

//	加载参数基类
class CECAsynLoadContent{
public:
	virtual ~CECAsynLoadContent()=0{}
	virtual CECAsynLoadContent * Clone()const=0;
	virtual bool operator == (const CECAsynLoadContent &)const=0;
	virtual float GetSortWeight()const=0;
	virtual void UpdateSortWeight()=0;
	virtual bool ThreadLoad()=0;
};

//	成功记录的加载命令
class CECAsynLoadCommand{
	CECAsynLoadID			m_id;
	CECAsynLoadContent	*	m_pContent;

	//	禁用
	CECAsynLoadCommand(const CECAsynLoadCommand &);
	CECAsynLoadCommand & operator = (const CECAsynLoadCommand &);
public:
	CECAsynLoadCommand(const CECAsynLoadID &id, CECAsynLoadContent *pContent)
		: m_id(id)
		, m_pContent(pContent)
	{}
	~CECAsynLoadCommand();
	const CECAsynLoadID & GetID()const{
		return m_id;
	}
	CECAsynLoadContent * GetContent()const{
		return m_pContent;
	}
	void UpdateSortWeight();
	bool ThreadLoad();
};

//	class CECAsynLoadCommandMatcher
class CECAsynLoadCommandMatcher{
public:
	virtual ~CECAsynLoadCommandMatcher(){}
	virtual bool IsMatch(const CECAsynLoadCommand *pCommand)const=0;
};

//	命令队列
class CECAsynLoadCommandArray{
public:
	typedef abase::vector<CECAsynLoadCommand *> CommandArray;
	typedef CommandArray::iterator	Iterator;

private:
	CommandArray	m_commandArray;

public:
	CECAsynLoadCommandArray();
	CECAsynLoadCommandArray(const CommandArray &);
	void Assign(const CommandArray &commandArray);
	
	void Append(CECAsynLoadCommand *pCommand);
	void Remove(Iterator it);
	int  RemoveIf(CECAsynLoadCommandMatcher *pMatcher);
	void Clear();

	void UpdateSortWeight();
	CECAsynLoadCommandArray FetchNearest(int count);
	CECAsynLoadCommandArray FetchAll();
	
	int Count()const;
	bool IsEmpty()const;
	CECAsynLoadCommand *GetAt(int index);

	Iterator Begin();
	Iterator End();
	Iterator Find(const CECAsynLoadID &id);								//	用于根据加载结果删除加载记录
	Iterator Find(int loadType, const CECAsynLoadContent *pContent);	//	用于查找是否在加载指定内容
};

//	加载队列
class CECAsynLoadCenter{
	CECAsynLoadCommandArray	m_commandArray;
	CECAsynLoadIDFactory	m_idFactory;

	//	禁用
	CECAsynLoadCenter(const CECAsynLoadCenter &);
	CECAsynLoadCenter & operator = (const CECAsynLoadCenter &);

public:
	CECAsynLoadCenter();
	~CECAsynLoadCenter();

	bool Append(int loadType, CECAsynLoadContent *pContent, CECAsynLoadID &id);
	void Clear();

	CECAsynLoadCommandArray & GetCommandArray(){
		return m_commandArray;
	}
};
