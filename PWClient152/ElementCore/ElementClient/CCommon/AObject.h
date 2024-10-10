/*
	AObject.h
	˵����AObject��������������ϵ�ĸ������ṩ��RTTI�Ĺ��ܺʹ��л��Ĺ��ܡ�
	������Ҫע����ǣ���Ҫ��AObject�Ĵ��л���AData�Ĵ��л��ֿ�������
	����AData������ִ��л��ķ�ʽ�������ڱ༭����������Ϸ�У���
	�˴��л���������Ӧ����AData���������ࡣ

	����AObject�����ֳ�Ա������������Ҫ�޲����Ĺ��캯�������Գ�ֵΪ0��
	�������κ�һ��AObject���������Ҫ�ڹ�������ʹ�ã���Ӧ����һ��Ψһ
	�����������֡����ڿ��ܲ����ַ�����ķ������洢�ĵ��еĴ������ַ�����
	��AObject����������ֱ���������һ������ָ�룬����SetName�Ĳ���������ָ����
	
	Modified By ���� Jun/21/2004�������˶�̬���Խӿ� 
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
//���Բ�������
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
	
	bool InitStateTable(); //��ʼ������״̬��	
	void SetState(int index, int state) {m_StateList[index] = state;} //��������״̬	
	int GetState(int index) {return m_StateList[index];} //��ȡ����״̬
protected:
	APropertyObject():m_version(0){}
	virtual int PropertiesCount() {return 0;}
	virtual AProperty * Properties(int index) {return NULL;}
	virtual AProperty * Properties(const char * name,int *index = NULL){return NULL;}
};

// ��̬���Խӿ�
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