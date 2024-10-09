#include "aproperty.h"
#include <AString.h>
#include <AScriptFile.h>

void StringToValue(AVariant &var, const char * str)
{
	assert(false);
}

void StringToValue(int &val, const char * str)
{
	sscanf(str,"%d",&val);
}

void StringToValue(float &val, const char * str)
{
	sscanf(str,"%f",&val);
}

void StringToValue(bool &val, const char * str)
{
	AString tmp = str;
	tmp.MakeLower();
	if(tmp == "true")
	{
		val = true;
	}
	else
	{
		val = false;
	}
}

void StringToValue(AString &val, const char * str)
{
	val = str;
}

void StringToValue(A3DVECTOR3 &val,const char * str)
{
	float x=0,y=0,z=0;
	sscanf(str,"%f,%f,%f",&x,&y,&z);
	val.x = x;
	val.y = y;
	val.z = z;
}

void StringToValue(A3DCOLOR &val,const char * str)
{
	int r=255,g=255,b=255,a=0;
	sscanf(str,"%d,%d,%d,%d",&r,&g,&b,&a);
	val = A3DCOLORRGBA(r,g,b,a);
}

void ValueToString(const AVariant &var, AString & str)
{
	assert(false);
}

void ValueToString(int value, AString & str)
{
	str.Format("%d",value);
}

void ValueToString(float value, AString & str)
{
	str.Format("%g",value);
}

void ValueToString(bool value, AString & str)
{
	str = value?"true":"false";
}

void ValueToString(const AString &value, AString & str)
{
	str = value;
}

void ValueToString(const A3DVECTOR3 &value, AString & str)
{
	str.Format("%g,%g,%g",value.x,value.y,value.z);
}


void ValueToString(const A3DCOLOR &value, AString & str)
{
	int r,g,b,a;
	r = A3DCOLOR_GETRED(value);
	g = A3DCOLOR_GETGREEN(value);
	b = A3DCOLOR_GETBLUE(value);
	a = A3DCOLOR_GETALPHA(value);
	str.Format("%d,%d,%d,%d",r,g,b,a);
}


void StringToValue(AScriptCodeData &value,const char * str)
{
	assert(false);
}

void ValueToString(const AScriptCodeData &value, AString & str)
{
	assert(false);
}

namespace __CLOAK__
{
struct ASetNode
{
	ASet * pSet;
	ASetNode * pNext;

};
}

static __CLOAK__::ASetNode *initList = NULL;

int AddInitElement(ASet * pSet)
{
	__CLOAK__::ASetNode * pNode = new __CLOAK__::ASetNode();
	pNode->pSet = pSet;
	pNode->pNext = initList;
	initList = pNode;
	return 0;
}

int InitPropertiesList()
{
	int rst = 0;
	while(initList)
	{
		__CLOAK__::ASetNode * pNode = initList;
		initList = initList->pNext;
		rst += pNode->pSet->ReadData();
		delete pNode;
	}
	return rst;
}

int ReadSetData(const char * filename, ASet *pSet)
{
	AScriptFile sFile;
	if(!sFile.Open(filename)){
		assert(false &&"无法打开配置文件");
		return -1;
	}
	
	while(sFile.GetNextToken(true))
	{
		AString token1 = sFile.m_szToken;
		if(!sFile.GetNextToken(true))
		{
			assert(false && "配置文件格式不正确");
			sFile.Close();
			return -1;
		}
		pSet->AddData(token1,sFile.m_szToken);
	}
	sFile.Close();
	return 0;

}

