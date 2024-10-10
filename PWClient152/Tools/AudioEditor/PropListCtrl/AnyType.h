#pragma once

#include "BoostAny.h"

enum {
	ANY_VOID,
	ANY_BOOL,
	ANY_CHAR,
	ANY_UCHAR,
	ANY_SHORT,
	ANY_USHORT,
	ANY_INT,
	ANY_UINT,
	ANY_LONG,
	ANY_ULONG,
	ANY_INT64,
	ANY_UINT64,
	ANY_FLOAT,
	ANY_DOUBLE,
	ANY_A3DVECTOR3,
	ANY_A3DMATRIX4,
	ANY_STRING,
	ANY_FONTTYPE,
	ANY_SUBPROPARRAY,			// Sub property array
	ANY_CUSTOMFUNCPTR,			// Custom propbox interface
};

int TypeInfo2PropType(const type_info& ti);

//	only support change the basic types to _variant_t
//	string / A3DVECTOR / A3DMATRIX4 will lead to an empty _variant_t
_variant_t Any2VT(const any& val);
any VT2Any(const _variant_t& val, int iType);