    /*
 * FILE: AProperty.h
 *
 * DESCRIPTION: 
 *	实现属性定义所需的类和接口定义
 *		ARange;		表示了一个数值范围的接口
 *		ASet;		表示了一个数值容器的接口
 *		AProperty;	表示了属性的接口
 *		ARangeTemplate;	数值范围的模板实现
 *		ASetTemplate;	数值容器的模板实现
 *		ASetElement;	数值容器里的元素
 *		APropertyTemplate;	属性的模板实现
 *
 * CREATED BY: 杨智盈, 2002/8/7
 *
 * MODIFY BY：崔铭，做了很少的修改，并将其他地方的一些宏挪到了这里，加入了少许注释 
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#pragma once

#pragma warning ( disable : 4786 )
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <amemory.h>
#include <vector.h>
#include "AVariant.h"


// 为实现代码简化而定义宏
#define EXTERN_DEFINESET(type, name) extern ASetTemplate<type> name;
#define	DEFINE_SETBEGIN(type, name) ASetTemplate<type> name(
#define DEFINE_SETELEMENT(type, name, value) ASetTemplate<type>::ASetElement<type>(name, value),
#define DEFINE_SETEND(type) ASetTemplate<type>::ASetElement<type>());
#define DEFINE_SETFILE(type,name,filename) ASetTemplate<type> name(filename);

// 编辑器对待属性的推荐方式
enum 
{
	WAY_DEFAULT,		// 缺省方式，由编辑器自动检测AVariant的类型并处理
	WAY_BOOLEAN,		// 布尔量处理方式
	WAY_INTEGER,		// 整型处理方式
	WAY_FLOAT,		// 浮点处理方式
	WAY_STRING,		// 字串处理方式
	WAY_FILENAME,		// 文件名
	WAY_COLOR,		// 颜色值
	WAY_VECTOR,		// 向量值
	WAY_BINARY,		// 二进制块
	WAY_OBJECT,		// 对象
	WAY_IDOBJECT,
	WAY_NAME,		// 对象的名字:)，因为使用SetName设置的所以有此限制，不能用通用的方法赋值
	WAY_SCRIPT,		// 脚本代码
	WAY_UNITID,		// 对象ID
	WAY_PATHID,		// 路径ID
	WAY_WAYPOINTID,		// 路点ID
	WAY_STRID,		// (文档内)字符串资源ID
	WAY_SFXID,		// (文档内)音效资源ID
	WAY_TRIGGEROBJECT,	// 触发器
	WAY_MEMO,		// 备注
	WAY_FONT,		// 字体
	WAY_CUSTOM,		// 用户自定义方式
	// 处理方式说明
	WAY_READONLY	= (1 << 31),// 属性只读访问
};

/////ARange接口
class ARange
{
public:

	virtual AVariant GetMaxValue() = 0;
	virtual AVariant GetMinValue() = 0;
	virtual bool IsInRange(AVariant value) = 0;
};

/////ASet接口
class ASet
{
	friend int InitPropertiesList();
	friend int ReadSetData(const char * filename, ASet *pSet);

	virtual int AddData(const char * name,const char * data){return -1;}
	virtual int ReadData() { return -1;};
public:

	virtual int GetCount() = 0;
	virtual AString GetNameByIndex(int index) = 0;
	virtual AVariant GetValueByIndex(int index) = 0;
	virtual AVariant GetValueByName(AString szName) = 0;
	virtual AString GetNameByValue(AVariant value) = 0;
	virtual int FindValue(AVariant value) = 0;
	virtual int FindName(AString szName) = 0;

};

int AddInitElement(ASet * pSet);
int InitPropertiesList();
int ReadSetData(const char * filename, ASet *pSet);

/////AProperty接口
class AProperty
{
public:
	enum
	{
		HAS_DEFAULT_VALUE	= 1,
		DEFAULT_VALUE		= 2,
		DO_NOT_SAVE		= 4,

//定义属性时用的内容
		USE_DEFINE_DEFAULT	= 3
		
	};

	virtual bool IsUserVisable() = 0;
	virtual const char * GetName() = 0;
	virtual const char * GetPath() = 0;
	virtual ASet * GetPropertySet() = 0;
	virtual ARange * GetPropertyRange() = 0;
	virtual int GetWay() = 0;
	virtual void SetWay(int way) = 0;
	virtual int GetState() = 0;
	virtual void SetState(int state) = 0;
	virtual AVariant GetValue(void * pBase) = 0;
	virtual void SetValue(void * pBase, AVariant value) = 0;
	virtual void SetStringValue(void *pBase,const char * str) = 0;
	virtual void GetStringValue(void *pBase,AString &str) = 0;
	virtual ~AProperty() {};
};

/////ARangeTemplate类
template <class TYPE> class ARangeTemplate : public ARange
{
public:

	ARangeTemplate(TYPE min, TYPE max) :m_min(min), m_max(max) {}
	AVariant GetMaxValue() {return AVariant(m_max);}
	AVariant GetMinValue() {return AVariant(m_min);}

	bool IsInRange(AVariant value)
	{
		if(TYPE(value) < m_min && TYPE(value) > m_max)
		{
			return false;
		}

		return true;
	}

private:
	
	TYPE m_min;
	TYPE m_max;
};

/////ASetTemplate类
template <class TYPE> class ASetTemplate : public ASet
{

	virtual int ReadData()
	{
		if(!m_FileName.IsEmpty())
		{
			return ReadSetData(m_FileName,this);
		}
		else
		{
			assert(false);
			return -1;
		}
	}

	virtual int AddData(const char * name,const char * value)
	{
		TYPE data;
		StringToValue(data,value);
		ASetElement<TYPE> element(name,data);
		m_List.push_back(element);
		return 0;
	}
public:

	// 内建元素类
	template <class TYPE> class ASetElement
	{
	public:

		ASetElement()
		{
			m_szName.Empty();
		}

		ASetElement(AString szName, TYPE data)
		{
			m_szName	= szName;
			m_data		= data;
		}

		ASetElement(const ASetElement & e)
		{
			m_szName	= e.m_szName;
			m_data		= e.m_data;
		}

		AString GetName() {return m_szName;}
		TYPE GetData() {return m_data;}

	private:

		AString m_szName;
		TYPE		 m_data;
	};

protected:

	abase::vector<ASetElement<TYPE> > m_List;
	AString	m_FileName;

public:

	// 构造函数
	ASetTemplate(const char * filename):m_FileName(filename)
	{
		AddInitElement(this);		
	}

	// 构造函数
	ASetTemplate(ASetElement<TYPE> first, ...)
	{
		va_list vlist;
		ASetElement<TYPE> element = first;
		va_start(vlist, first);
		while(!element.GetName().IsEmpty())
		{
			m_List.push_back(element);
			element = va_arg(vlist, ASetElement<TYPE>);
		}
		va_end(vlist);
	}

	ASetTemplate() {}

	~ASetTemplate()
	{
	}

	void AddElement(AString szName, TYPE val)
	{
		m_List.push_back(ASetElement<TYPE>(szName, val));
	}

	void AddElement(ASetElement<TYPE>& ele)
	{
		m_List.push_back(ele);
	}

	//元素个数
	int GetCount()
	{
		return m_List.size();
	}

	// 根据索引获取名称
	AString GetNameByIndex(int index)
	{
		return m_List[index].GetName();
	}

	// 根据索引获取值
	AVariant GetValueByIndex(int index)
	{
		return AVariant(m_List[index].GetData());
	}

	// 根据名称查值
	AVariant GetValueByName(AString szName)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(strcmp(szName, m_List[index].GetName()) == 0)
			{
				return AVariant(m_List[index].GetData());
			}
		}

		return AVariant();	//未找到元素的情况下，返回一个空量，其类型为AVT_INVALIDTYPE。
	}

	// 根据值查名称
	AString GetNameByValue(AVariant value)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetData() == (TYPE)value)
			{
				return m_List[index].GetName();
			}
		}

		return AString();
	}

	// 查值索引
	int FindValue(AVariant value)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetData() == (TYPE)value)
			{
				return index;
			}
		}

		return -1;
	}

	// 查名称索引
	int FindName(AString szName)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetName() == szName)
			{
				return index;
			}
		}

		return -1;
	}
};

void StringToValue(AVariant &var, const char * str);
void StringToValue(int &value, const char * str);
void StringToValue(float &value, const char * str);
void StringToValue(bool &value, const char * str);
void StringToValue(AString &value, const char * str);
void StringToValue(A3DVECTOR3 &value,const char * str); 
void StringToValue(A3DCOLOR &value,const char * str);
void StringToValue(AScriptCodeData &value,const char * str);

void ValueToString(const AVariant &var, AString & str);
void ValueToString(int value, AString & str);
void ValueToString(float value, AString & str);
void ValueToString(bool value, AString & str);
void ValueToString(const AString &value, AString & str);
void ValueToString(const A3DVECTOR3 &value, AString & str); 
void ValueToString(const A3DCOLOR &value, AString & str); 
void ValueToString(const AScriptCodeData &value, AString & str); 

template <class TYPE> class APropertyTemplate : public AProperty  
{
public:
	
	APropertyTemplate(
		const char * clsName,
		const char * szName,			// 属性名
		int iOffset,				// 数据区偏移量
		ASet * pSet = NULL,			// 集合
		ARange * pRange = NULL,			// 范围
		int way = WAY_DEFAULT,			// 属性的处理方式
		const char * szPath = NULL,		// 路径（文件链接的搜寻位置，为NULL时限制在执行程序开始的路径以下）
		bool isVisable = true,			//是否为用户可见
		int  iState = 0)			//属性的状态(有无默认值)
	{
		assert(szName);
		m_szName	= szName;
		m_iOffset	= iOffset;
		m_pRange	= pRange;
		m_pSet		= pSet;
		m_iWay		= way;
		m_bUserVisable	= isVisable;
		m_State 	= iState;
		if(szPath) m_szPath = szPath;
		/*
		char buf[30];
		AString str;
		str += clsName;
		str += "\t名字:";
		str += szName;
		str += "\t偏移:";
		str += itoa(iOffset,buf,10);
		str += "\n";		
		OutputDebugString(str);
		*/
	}



	bool IsUserVisable() { return m_bUserVisable;}
	int GetState()
	{
		return m_State;
	}
	void SetState(int state)
	{
		m_State = state;
	}

	AVariant GetValue(void * pBase)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		return AVariant(*(TYPE*)(pData + m_iOffset));
	}

	void SetValue(void * pBase, AVariant value)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		*(TYPE*)(pData + m_iOffset) = (TYPE)value;
	}

	void SetStringValue(void *pBase,const char * str)
	{
		::StringToValue(*(TYPE*)((char *)pBase + m_iOffset),str);
	}

	void GetStringValue(void *pBase,AString &str)
	{
		::ValueToString(*(TYPE*)((char *)pBase + m_iOffset),str);
	}

	const char * GetName()
	{
		return m_szName;
	}

	const char * GetPath()
	{
		return m_szPath;
	}

	ASet * GetPropertySet()
	{
		return m_pSet;
	}

	ARange * GetPropertyRange()
	{
		return m_pRange;
	}

	int GetWay()
	{
		return m_iWay;
	}

	void SetWay(int way)
	{
		m_iWay = way;
	}

private:

	AString			m_szName;		// 属性名
	AString			m_szPath;		// 路径(当属性为文件链接时，限制文件的搜寻 位置)
	int			m_iOffset;		// 数据偏移量
	int			m_iWay;			// 属性的处理方式
	ARange *		m_pRange;		// 属性值的范围
	ASet *			m_pSet;			// 属性值的集合
	bool			m_bUserVisable;
	int			m_State;
};


//定义所需要的宏

// 初始化静态属性集合（仅限于在构造函数中出现）
#define INITIALIZE_STATICPROPERIES(classname) 

// 定义静态属性集合起始点，参数：类名，基类名
#define STATICPROPERIES_BEGIN(classname,base) \
	protected:	\
	virtual int PropertiesCount() \
	{ \
		int count = 0;\
		classname::Properties(&count, NULL, count);\
		return base::PropertiesCount() + count;\
	}\
	virtual AProperty * Properties(int index)\
	{\
		AProperty * pProp = base::Properties(index);\
		if(pProp) return pProp;\
		index -= base::PropertiesCount();\
		return classname::Properties(NULL, NULL, index);\
	}\
	virtual AProperty * Properties(const char *  name,int *index = NULL)\
	{\
		AProperty * pProp = base::Properties(name);\
		if(pProp) return pProp;\
		int idx;\
		return classname::Properties(NULL, name, index?*index:idx);\
	}\
	struct _staticproperies\
	{\
		AProperty * properties(int * pCount, const char * pName, int &index)\
		{\
			static AProperty * pProps[] = \
			{

// 定义属性项目，参数：  类名，  属性名，变量类型，变量指针，属性值集合，属性值范围，属性的处理方式，文件名的搜寻位置（如果属性值为文件链接的话），是否为用户可见
#define STATICPROPERTY(classname, name, prop_type, prop_data, prop_set, prop_range, prop_way, prop_path,prop_visable,prop_state) \
			new APropertyTemplate<prop_type>(#classname,name, offsetof(classname, prop_data), prop_set, prop_range, prop_way, prop_path,prop_visable,prop_state),

// 定义静态属性终结点，参数：类名
#define STATICPROPERIES_END(classname) \
			};\
			static int count = sizeof(pProps) / sizeof(AProperty *);\
			if(pCount) {*pCount = count;return NULL;}\
			if(pName)\
			{\
				for(int a = 0;a < count;a ++)\
					if(strcmp(pProps[a]->GetName(), pName) == 0) {\
						index = a;\
						return pProps[a];\
					}\
				return NULL;\
			}\
			if(index >= 0 && index < count) return pProps[index];\
			return NULL;\
		}\
		~_staticproperies()\
		{\
			int count = 0;\
			properties(&count, NULL, count);\
			for(int a = 0;a < count;a ++)\
			{\
				AProperty * pProp = properties(NULL, NULL, a);\
				delete pProp;\
			}\
		}\
	};\
	friend struct _staticproperies;\
	static AProperty * Properties(int * pCount, const char * pName, int &index)\
	{\
		static classname::_staticproperies properies;\
		return properies.properties(pCount, pName, index);\
	}



/*
  典型的属性定义有三种：

  简单类型
  STATICPROPERTY("property", int, m_int_value, NULL, NULL, WAY_DEFAULT, NULL)

  简单类型但指定处理模式
  STATICPROPERTY("property", A3DCOLOR, m_a3c_value, NULL, NULL, WAY_COLOR, NULL)

  文件链接
  STATICPROPERTY("property", AUString, m_str_skytexture, NULL, NULL, WAY_FILENAME, "texture\\sky")

  关于处理方式的定义，如下：
	enum 
	{
		WAY_DEFAULT,		// 缺省方式，由编辑器自动检测AVariant的类型并处理
		WAY_BOOLEAN,		// 布尔量处理方式
		WAY_INTEGER,		// 整型处理方式
		WAY_FLOAT,			// 浮点处理方式
		WAY_STRING,			// 字串处理方式
		WAY_FILENAME,		// 文件名
		WAY_COLOR,			// 颜色值
		WAY_VECTOR,			// 向量值
		WAY_BINARY,			// 二进制块
		WAY_OBJECT,			// 对象
		WAY_UNITID,			// 对象ID
		WAY_PATHID,			// 路径ID
		WAY_STRID,			// (文档内)字符串资源ID
		WAY_SFXID,			// (文档内)音效资源ID
		// 处理方式说明
		WAY_READONLY	= (1 << 31),// 属性只读访问
	};
*/
