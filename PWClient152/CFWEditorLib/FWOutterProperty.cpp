// OutterPropertyList.cpp: implementation of the FWOutterPropertyList class.
//
//////////////////////////////////////////////////////////////////////

#include "FWOutterProperty.h"
#include <AScriptFile.h>

#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// FWOutterProperty
//////////////////////////////////////////////////////////////////////


bool FWOutterProperty::Load(AScriptFile *pFile, int nID)
{
	struct _MAP
	{
		const char * s;
		int i;
		const char * format;
		bool haveRange;
	};
	static const _MAP _map[] = 
	{
		{"INT",		GFX_VALUE_INT,		"%d",		true},
		{"FLOAT",	GFX_VALUE_FLOAT,	"%f",		true},
		{"bool",	GFX_VALUE_BOOL,		"%d",		false},
		{"COLOR",   GFX_VALUE_COLOR,	"%u",		false},
		{"VECTOR",  GFX_VALUE_VECTOR3,	"%f:%f:%f",	false},
		{"STRING",  GFX_VALUE_STRING,	"",			false},
	};
	static const int _count = sizeof(_map) / sizeof(_map[0]);

	BEGIN_FAKE_WHILE;

	// read name
	CHECK_BREAK(pFile->GetNextToken(true));
	Name = pFile->m_szToken;

	// read type
	CHECK_BREAK(pFile->GetNextToken(false));
	AString type = pFile->m_szToken;
	
	int nIndex = -1;
	for (int i = 0; i < _count; i++)
	{
		if (type == _map[i].s)
		{
			nIndex = i;
			break;
		}
	}
	CHECK_BREAK(nIndex != -1);

	// read val
	CHECK_BREAK(pFile->GetNextToken(false));
	if (_map[nIndex].i != GFX_VALUE_STRING)
	{
		// trick:
		// when type is not VECTOR, the last two param will be ignored 
		// due to the format string have only 1 placeholder
		CHECK_BREAK(EOF != sscanf(pFile->m_szToken, _map[nIndex].format, &Data.m_Data.vec.x, &Data.m_Data.vec.y, &Data.m_Data.vec.z));
		Data.SetType(GfxValueType(_map[nIndex].i));
	}
	else
	{
		Data = GFX_PROPERTY(AString(pFile->m_szToken));	
	}

	// read min/max val, only valid when type is INT/FLOAT
	if (_map[nIndex].haveRange)
	{
		CHECK_BREAK(pFile->GetNextToken(false));
		CHECK_BREAK(EOF != sscanf(pFile->m_szToken, _map[nIndex].format, &DataMin.m_Data));
		DataMin.SetType(GfxValueType(_map[nIndex].i));
		
		CHECK_BREAK(pFile->GetNextToken(false));
		CHECK_BREAK(EOF != sscanf(pFile->m_szToken, _map[nIndex].format, &DataMax.m_Data));
		DataMax.SetType(GfxValueType(_map[nIndex].i));
	}
	
	// set id
	ID = nID;

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

//////////////////////////////////////////////////////////////////////
// FWOutterPropertyList
//////////////////////////////////////////////////////////////////////

FWOutterPropertyList::FWOutterPropertyList()
{

}

FWOutterPropertyList::~FWOutterPropertyList()
{

}

FWOutterPropertyList& FWOutterPropertyList::operator = (const FWOutterPropertyList &src)
{
	if (GetCount() == src.GetCount()) // lcoal list is inited
	{
		ALISTPOSITION posLocal = GetHeadPosition();
		ALISTPOSITION posSrc = src.GetHeadPosition();
		while (posLocal && posSrc)
		{
			*GetNext(posLocal) = *src.GetNext(posSrc);
		}
	}
	else // local list is empty, copy from src
	{
		DeleteAll(); // just for safe
		ALISTPOSITION pos = src.GetHeadPosition();
		while (pos)
		{
			AddTail(static_cast<FWOutterProperty *>(src.GetNext(pos)->Clone()));
		}
	}
	return *this;
}

bool FWOutterPropertyList::Load(AScriptFile *pFile)
{
	ASSERT(pFile);

	DeleteAll();

	BEGIN_FAKE_WHILE;

	// read first line, this is the count of outterproperty
	CHECK_BREAK(pFile->GetNextToken(false));
	CHECK_BREAK(AString(pFile->m_szToken) == "OutterPropertyCount");
	CHECK_BREAK(pFile->GetNextToken(false));
	CHECK_BREAK(AString(pFile->m_szToken) == "=");
	int nCount = pFile->GetNextTokenAsInt(false);
	if (0 == nCount) return true;

	// read each outterproperty
	int i(0);
	for (i = 0; i < nCount; i++)
	{
		FWOutterProperty *pProp = new FWOutterProperty;
		if (!pProp->Load(pFile, i))
		{
			SAFE_DELETE(pProp);
			break;
		}
		else
		{
			AddTail(pProp);
		}
	}
	CHECK_BREAK(i == nCount);

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	DeleteAll();
	
	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

void FWOutterPropertyList::FillParamArray(FWParamArray &arrayParam) const
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWOutterProperty *pProp = GetNext(pos);
		arrayParam.Add(FWParam(pProp->Data, pProp->DataMin, pProp->DataMax, AS2S(pProp->Name)));
	}
}

void FWOutterPropertyList::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	int nIndex = 0;
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWOutterProperty *pProperty = GetNext(pos);
		pProperty->Data = arrayParam[nIndex++].GetVal();
	}
}

