/*
	AObject.h
	说明：AObject类是整个单根体系的根，它提供了RTTI的功能和串行化的功能。
	不过，要注意的是，需要把AObject的串行化和AData的串行化分开来看，
	由于AData需求多种串行化的方式（无论在编辑器还是在游戏中），
	此串行化方法并不应用于AData及其派生类。

	关于AObject的名字成员变量，由于需要无参数的构造函数，所以初值为0，
	但对于任何一个AObject对象，如果需要在管理器中使用，都应该有一个唯一
	不重名的名字。由于可能采用字符串表的方法来存储文档中的大量的字符串，
	在AObject对象里的名字变量可能是一个引用指针，这在SetName的参数中予以指定。
	
	Modified By 张羽 Jun/21/2004，增加了动态属性接口 
*/

#pragma once
#include <assert.h>
#include <vector.h>
#include "AProperty.h"

class APropertyObject
{
protected:
	int m_version;
	abase::vector<int> m_StateList;	

public:
//属性操作函数
	AProperty * GetProperty(int index)
	{
		return Properties(index);
	}
	AProperty * GetProperty(const char * name,int *index = NULL)
	{
		return Properties(name,index);
	}

	int GetPropertiesCount()
	{
		return PropertiesCount();
	}
	
	bool InitStateTable(); //初始化属性状态表	
	void SetState(int index, int state) {m_StateList[index] = state;} //设置属性状态	
	int GetState(int index) {return m_StateList[index];} //读取属性状态
protected:
	APropertyObject():m_version(0){}
	virtual int PropertiesCount() {return 0;}
	virtual AProperty * Properties(int index) {return NULL;}
	virtual AProperty * Properties(const char * name,int *index = NULL){return NULL;}
};

// 动态属性接口
class ADynPropertyObject : public APropertyObject
{
public:
	ADynPropertyObject() 
	{
		m_PropLst.reserve(200);
		m_VarLst.reserve(200);
	}
	virtual ~ADynPropertyObject() {}

protected:
// data
	typedef APropertyTemplate<AVariant> VarPropTemp;
	abase::vector<VarPropTemp> m_PropLst;
	abase::vector<AVariant> m_VarLst;

// functions
protected:
	virtual int PropertiesCount() { return m_PropLst.size(); }
	virtual AProperty * Properties(int index) { return &m_PropLst[index]; }
	virtual AProperty * Properties(const char * name, int *index = NULL)
	{
		assert(name);
		for (int i = 0; i < m_PropLst.size(); i++)
		{
			if (strcmp(m_PropLst[i].GetName(), name) == 0)
			{
				if (index) *index = i;
				return &m_PropLst[i];
			}
		}
		return NULL;
	}

public:
	virtual void DynAddProperty(const AVariant& varValue,
		const char* szName, ASet* pSet = NULL, ARange* pRange = NULL,
		int way = WAY_DEFAULT, const char* szPath = NULL, bool isVisable = true, int iState = 0)
	{
		m_VarLst.push_back(varValue);		
		m_PropLst.push_back(VarPropTemp("", 
										szName, 
										(int)&m_VarLst.back()-(int)this,
										pSet,
										pRange,
										way,
										szPath,
										isVisable,
										iState));
		m_StateList.push_back(m_PropLst.back().GetState() & AProperty::DEFAULT_VALUE ? 1 : 0);
	}

	AVariant& GetPropVal(int nIndex)
	{
		assert(nIndex >= 0 && nIndex < m_VarLst.size());
		return m_VarLst[nIndex];
	}
	
	void SetPropVal(int nIndex, const AVariant& var)
	{
		m_VarLst[nIndex] = var;
	}
};

struct CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void) = 0;
	virtual LPCTSTR CALLBACK OnGetShowString(void) const = 0;
	virtual AVariant CALLBACK OnGetValue(void) const = 0;
	virtual void CALLBACK OnSetValue(const AVariant& var) = 0;
};