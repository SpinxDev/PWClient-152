#include "stdafx.h"
#include "AnyType.h"
#include "BCGPPropertyObject.h"

int TypeInfo2PropType(const type_info& ti)
{
	if (ti == typeid(void))
		return ANY_VOID;
	else if (ti == typeid(bool))
		return ANY_BOOL;
	else if (ti == typeid(char))
		return ANY_CHAR;
	else if (ti == typeid(unsigned char))
		return ANY_UCHAR;
	else if (ti == typeid(short))
		return ANY_SHORT;
	else if (ti == typeid(unsigned short))
		return ANY_USHORT;
	else if (ti == typeid(int))
		return ANY_INT;
	else if (ti == typeid(unsigned int))
		return ANY_UINT;
	else if (ti == typeid(long))
		return ANY_LONG;
	else if (ti == typeid(unsigned long))
		return ANY_ULONG;
	else if (ti == typeid(__int64))
		return ANY_INT64;
	else if (ti == typeid(unsigned __int64))
		return ANY_UINT64;
	else if (ti == typeid(float))
		return ANY_FLOAT;
	else if (ti == typeid(double))
		return ANY_DOUBLE;
	else if (ti == typeid(std::string))
		return ANY_STRING;	
	else if (ti == typeid(CPropertyArray))
		return ANY_SUBPROPARRAY;
	else if (ti == typeid(prop_custom_funcs*))
		return ANY_CUSTOMFUNCPTR;
	else if (ti == typeid(A3DVECTOR3))
		return ANY_A3DVECTOR3;
	else
		return ANY_VOID;
}

int Any2PropType(const any& val)
{
	return TypeInfo2PropType(val.type());
}


_variant_t Any2VT(const any& val)
{
	int iValType = Any2PropType(val);
	switch(iValType)
	{
	case ANY_CHAR:
		return any_cast<char>(val);
	case ANY_UCHAR:
		return any_cast<unsigned char>(val);
	case ANY_SHORT:
		return any_cast<short>(val);
	case ANY_USHORT:
		return any_cast<unsigned short>(val);
	case ANY_INT:
		return any_cast<int>(val);
	case ANY_UINT:
		return any_cast<unsigned int>(val);
	case ANY_LONG:
		return any_cast<long>(val);
	case ANY_ULONG:
		return any_cast<unsigned long>(val);
	case ANY_INT64:
		return any_cast<__int64>(val);
	case ANY_UINT64:
		return any_cast<unsigned __int64>(val);
	case ANY_FLOAT:
		return any_cast<float>(val);
	case ANY_DOUBLE:
		return any_cast<double>(val);
	case ANY_STRING:
		return any_cast<std::string>(val).c_str();
	default:
		return _variant_t();
	}
}

any VT2Any(const _variant_t& val, int iType)
{
	switch(iType)
	{
	case ANY_CHAR:
		return (char)val;
	case ANY_UCHAR:
		return (unsigned char)val;
	case ANY_SHORT:
		return (short)val;
	case ANY_USHORT:
		return (unsigned short)val;
	case ANY_INT:
		return (int)val;
	case ANY_UINT:
		return (unsigned int)val;
	case ANY_LONG:
		return (long)val;
	case ANY_ULONG:
		return (unsigned long)val;
	case ANY_INT64:
		return (__int64)val;
	case ANY_UINT64:
		return (unsigned __int64)val;
	case ANY_FLOAT:
		return (float)val;
	case ANY_DOUBLE:
		return (double)val;
	case ANY_STRING:
		return (_bstr_t)val;
	default:
		return any();
	}
}