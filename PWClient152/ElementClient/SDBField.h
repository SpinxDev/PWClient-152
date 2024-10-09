/********************************************************************
	created:	2005/10/14
	created:	14:10:2005   10:50
	file name:	Field.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "SDBArchive.h"
#include "SDBMisc.h"


namespace SimpleDB
{
	template<typename TValueType>
	struct Field
	{
	public:
		typedef TValueType ValueType;
		typedef Field<ValueType> ThisType;
	private:
		ValueType value_;
		bool isNull_;
	public:
		Field() : isNull_(true)
		{
		}

		Field(const ThisType &src)
		{
			*this = src;
		}

		ThisType& operator = (const ThisType & src)
		{
			if (&src != this)
			{
				if (src.IsNull())
					*this = DBNull();
				else
					*this = src.value_;
			}
			return *this;
		}

		const ValueType & operator = (const ValueType & src)
		{
			value_ = src;
			isNull_ = false;
			return src;
		}

		DBNull operator = (DBNull dbNull)
		{
			value_ = ValueType();
			isNull_ = true;
			return dbNull;
		}

		bool IsNull() const
		{
			return isNull_;
		}

		bool operator == (const ThisType & src) const
		{
			if (&src == this)
				return true;
			
			if (IsNull() || src.IsNull())
				return IsNull() && src.IsNull();
			else
				return value_ == src;
		}
		bool operator != (const ThisType & src) const
		{
			return !(*this == src);
		}

		ValueType & Value()
		{
			ASSERT(!IsNull());
			return value_;
		}

		const ValueType & Value() const
		{
			ASSERT(!IsNull());
			return value_;
		}

		operator ValueType() const
		{
			return Value();
		}

		void Save(Archive &ar) const
		{
			ar << isNull_;
			if (!IsNull())
				ar << value_;
		}

		void Load(Archive &ar)
		{
			ar >> isNull_;
			if (!IsNull())
				ar >> value_;
		}

		ACString ToString() const
		{
			return FieldPrivate::ToString(value_);
		}

		friend bool operator == (const ValueType &value, const Field<ValueType> & field)
		{
			if (field.IsNull())
				return false;
			else
				return value == field.value_;
		}
		friend bool operator == (const Field<ValueType> & field, const ValueType &value)
		{
			return value == field;
		}
		friend bool operator == (const DBNull &value, const Field<ValueType> & field)
		{
			return field.IsNull();
		}
		friend bool operator == (const Field<ValueType> & field, const DBNull &value)
		{
			return value == field;
		}
		friend bool operator != (const ValueType &value, const Field<ValueType> & field)
		{
			return !(value == field);
		}
		friend bool operator != (const Field<ValueType> & field, const ValueType &value)
		{
			return !(field == value);
		}
		friend bool operator != (const DBNull &value, const Field<ValueType> & field)
		{
			return !(value == field);
		}
		friend bool operator != (const Field<ValueType> & field, const DBNull &value)
		{
			return !(field == value);
		}
	};

	namespace FieldPrivate
	{
		inline ACString ToString(bool value)
		{
			return (value ? _AL("true") : _AL("false"));
		}
		inline ACString ToString(short value)
		{
			ACString result;
			result.Format(_AL("%d"), value);
			return result;
		}
		inline ACString ToString(int value)
		{
			ACString result;
			result.Format(_AL("%d"), value);
			return result;
		}
		inline ACString ToString(long value)
		{
			ACString result;
			result.Format(_AL("%d"), value);
			return result;
		}
		inline ACString ToString(unsigned short value)
		{
			ACString result;
			result.Format(_AL("%u"), value);
			return result;
		}
#ifdef ANGELICA_2_2
		inline ACString ToString(wchar_t value)
		{
			ACString result;
			result.Format(_AL("%u"), value);
			return result;
		}
#endif
		inline ACString ToString(unsigned int value)
		{
			ACString result;
			result.Format(_AL("%u"), value);
			return result;
		}
		inline ACString ToString(unsigned long value)
		{
			ACString result;
			result.Format(_AL("%u"), value);
			return result;
		}
		inline ACString ToString(__int64 value)
		{
			ACString result;
			result.Format(_AL("%I64d"), value);
			return result;
		}
		inline ACString ToString(float value)
		{
			ACString result;
			result.Format(_AL("%f"), value);
			return result;
		}
		inline ACString ToString(double value)
		{
			ACString result;
			result.Format(_AL("%g"), value);
			return result;
		}
		inline ACString ToString(AWString value)
		{
			return value;
		}
		inline ACString ToString(AString value)
		{
			return AS2AC(value);
		}
		template<class TType>
		inline ACString ToString(const TType & value)
		{
			return value.ToString();
		}
	}
}