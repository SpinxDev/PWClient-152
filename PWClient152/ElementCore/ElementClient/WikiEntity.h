// Filename	: WikiEntity.h
// Creator	: Feng Ning
// Date		: 2010/04/27

#pragma once

class WikiEntity
{
private:
	class UserData
	{
	public:
		virtual ~UserData(){}
	};

	template<class ValueType>
	class UserDataHolder:public UserData
	{
	public:
		UserDataHolder(ValueType* p):m_pHeld(p){}
		virtual ~UserDataHolder(){delete m_pHeld;}
		ValueType* m_pHeld;
	};

	template<class ValueType>
	class UserDataConstHolder:public UserData
	{
	public:
		UserDataConstHolder(const ValueType* p):m_pHeld(p){}
		const ValueType* m_pHeld;
	};

	UserData* m_pData;

public:
	virtual ~WikiEntity(){delete m_pData;}
	virtual bool operator==(const WikiEntity& rhs) const = 0;
	virtual bool operator!=(const WikiEntity& rhs) const {return !(*this == rhs);}

	template<class ValueType>
	void SetData(ValueType* p)
	{
		delete m_pData;
		m_pData = new UserDataHolder<ValueType>(p);
	}

	template<class ValueType>
	void SetConstData(const ValueType* p)
	{
		delete m_pData;
		m_pData = new UserDataConstHolder<ValueType>(p);
	}

	template<class ValueType>
	ValueType* GetData(ValueType*& ptr)
	{
		UserDataHolder<ValueType>* pHolder = dynamic_cast<UserDataHolder<ValueType>*>(m_pData);
		return ptr = !pHolder ? 0:pHolder->m_pHeld;
	}

	template<class ValueType>
	const ValueType* GetConstData(const ValueType*& ptr)
	{
		UserDataConstHolder<ValueType>* pHolder = dynamic_cast<UserDataConstHolder<ValueType>*>(m_pData);
		return ptr = !pHolder ? 0:pHolder->m_pHeld;
	}

protected:
	WikiEntity():m_pData(0){}
	
private:
	WikiEntity(const WikiEntity&);
	WikiEntity& operator=(const WikiEntity&);
};

// shared pointer for WikiEntity
class WikiEntityPtr
{
public:
	WikiEntityPtr()
	:m_Ptr(0),m_PtrRef(0){}

	WikiEntityPtr(WikiEntity* p);

	~WikiEntityPtr();

	WikiEntityPtr(const WikiEntityPtr& ptr);
	WikiEntityPtr& operator=(const WikiEntityPtr& ptr);

	operator void*() const // never throws
	{ return m_Ptr; }

	WikiEntity& operator*() const // never throws
	{ return *m_Ptr; }

	WikiEntity* operator->() const // never throws
	{ return m_Ptr; }

	WikiEntity* Get() const // never throws
	{ return m_Ptr; }
	
	bool operator! () const // never throws
	{ return !m_Ptr; }

private:
	
	// ref counter
	class Ref
	{
	public:
		Ref();

		void Release();
		void AddRef();
		unsigned int GetCount() const { return m_Ref; }

	private:
		Ref(const Ref&);
		Ref& operator=(const Ref&);
		~Ref(){};

		unsigned int  m_Ref;
	};

	Ref* m_PtrRef;
	WikiEntity* m_Ptr;
};