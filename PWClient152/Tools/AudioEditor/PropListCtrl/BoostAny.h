// See http://www.boost.org/libs/any for Documentation.

#ifndef BOOST_ANY_INCLUDED
#define BOOST_ANY_INCLUDED

// what:  variant type boost::any
// who:   contributed by Kevlin Henney,
//        with features contributed and bugs found by
//        Ed Brey, Mark Rodgers, Peter Dimov, and James Curran
// when:  July 2001
// where: tested with BCC 5.5, MSVC 6.0, and g++ 2.95

#include <algorithm>
#include <typeinfo>
#include <string>

class any
{
	class placeholder;

public: // structors

	any()
		: content(0)
	{
	}

	template<typename ValueType>
	any(const ValueType & value)
		: content(new holder<ValueType>(value))
		//: content(new holder<ValueType>(value))
	{
	}

	any(const any & other)
		: content(other.content ? other.content->clone() : 0)
	{
	}

	~any()
	{
		if(content)
		{
			delete content;
			content = 0;
		}
		//delete content;
	}

public: // modifiers

	any & swap(any & rhs)
	{
		std::swap(content, rhs.content);
		return *this;
	}

	template<typename ValueType>
	any & operator=(const ValueType & rhs)
	{
		any(rhs).swap(*this);
		return *this;
	}

	any & operator=(const any & rhs)
	{
		any(rhs).swap(*this);
		return *this;
	}

public: // queries

	bool empty() const
	{
		return !content;
	}

	const std::type_info & type() const
	{
		return content ? content->type() : typeid(void);
	}

private: // types

	class placeholder
	{
	public: // structors

		virtual ~placeholder()
		{
		}

	public: // queries

		virtual const std::type_info & type() const = 0;

		virtual placeholder * clone() const = 0;

	};

	template<typename ValueType>
	class holder : public placeholder
	{
	public: // structors

		holder(const ValueType & value)
			: held(value)
		{
		}

	public: // queries

		virtual const std::type_info & type() const
		{
			return typeid(ValueType);
		}

		virtual placeholder * clone() const
		{
			//return WLD_NEW_T(holder<ValueType>, held);
			return new holder<ValueType>(held);
		}

	public: // representation

		ValueType held;

	};

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

private: // representation

	template<typename ValueType>
	friend ValueType * any_cast(any *);

	template<typename ValueType>
	friend ValueType * unsafe_any_cast(any *);

#else

public: // representation (public so any_cast can be non-friend)

#endif

	placeholder * content;

};

class bad_any_cast : public std::bad_cast
{
public:
	bad_any_cast(const char * type_src, const char* type_dest)
	{
		m_strMsg = "boost::bad_any_cast: "
			"failed conversion using boost::any_cast";
		if (type_src && type_dest) {
			m_strMsg += "\nwhen casting from ";
			m_strMsg += type_src;
			m_strMsg += " to ";
			m_strMsg += type_dest;
		}
	}

	virtual const char * what() const throw()
	{
		return m_strMsg.c_str();
	}
private:
	std::string m_strMsg;
};

template<typename ValueType>
ValueType * any_cast(any * operand)
{
	return operand && operand->type() == typeid(ValueType)
		? &static_cast<any::holder<ValueType> *>(operand->content)->held
		: 0;
}

template<typename ValueType>
inline const ValueType * any_cast(const any * operand)
{
	return any_cast<ValueType>(const_cast<any *>(operand));
}

template<typename ValueType>
ValueType any_cast(const any & operand)
{
	const ValueType * result = any_cast<ValueType>(&operand);
	if(!result) 
		throw bad_any_cast(operand.type().name(), typeid(ValueType).name());
	return *result;
}

// Note: The "unsafe" versions of any_cast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template<typename ValueType>
inline ValueType * unsafe_any_cast(any * operand)
{
	return &static_cast<any::holder<ValueType> *>(operand->content)->held;
}

template<typename ValueType>
inline const ValueType * unsafe_any_cast(const any * operand)
{
	return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
}


// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif
