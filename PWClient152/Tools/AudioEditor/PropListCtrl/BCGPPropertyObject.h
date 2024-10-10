#ifndef _BCGP_PROPERTY_OBJECT_H_
#define _BCGP_PROPERTY_OBJECT_H_

#include <comutil.h>
#include <list>
#include <vector>
#include <string>
#include "BoostAny.h"
#include <A3DFontMan.h>
#include <A3DVector.h>
#include <A3DMatrix.h>

typedef std::string PropertyString;

typedef void (*TipFunc)(double val, CString& strTip);

class prop_set
{
public:

	virtual ~prop_set() = 0 {}
	size_t GetCount() const { return m_arrPropSet.size(); }
	const any& GetValueByIdx(size_t idx) const { return m_arrPropSet.at(idx).m_val; }
	const char* GetNameByIdx(size_t idx) const { return m_arrPropSet.at(idx).m_Name.c_str(); }

	//	Return index if find, or return prop count if not exist
	virtual size_t FindValue(const any& val) const = 0;
	virtual size_t FindName(const char* szName) const = 0;

protected:
	struct prop_set_entry
	{
		template<class T>
		prop_set_entry(const T& val, const char* szName)
			: m_val(val)
			, m_Name(szName)
		{

		}

		any m_val;
		PropertyString m_Name;
	};

	typedef std::vector<prop_set_entry> PropEntryArray;
	PropEntryArray m_arrPropSet;
};

template<class T>
class prop_set_template : public prop_set
{
	// Override operations
public:

	void AddElement(const T& val, const char* szName)
	{
		m_arrPropSet.push_back(prop_set_entry(val, szName));
	}

	void RemoveAll()
	{
		m_arrPropSet.clear();
	}

	//	Override operations
protected:
	//	Return index if find, or return prop count if not exist
	virtual size_t FindValue(const any& val) const;
	virtual size_t FindName(const char* szName) const;
};

template<class T>
size_t prop_set_template<T>::FindValue(const any& val) const
{
	size_t nCount = 0;
	for (PropEntryArray::const_iterator itr = m_arrPropSet.begin()
		; itr != m_arrPropSet.end()
		; ++itr, ++nCount)
	{
		if (any_cast<T>((*itr).m_val) == any_cast<T>(val))
			break;
	}
	return nCount;
}

template<class T>
size_t prop_set_template<T>::FindName(const char* szName) const
{
	size_t nCount = 0;
	for (PropEntryArray::const_iterator itr = m_arrPropSet.begin()
		; itr != m_arrPropSet.end()
		; ++itr, ++nCount)
	{
		if ((*itr).m_Name == szName)
			break;
	}
	return nCount;
}


class prop_range
{
protected:
	prop_range() : m_step(1.0), m_TipFunc(0) {}

public:
	any GetMax() const { return m_max; }
	any GetMin() const { return m_min; }
	double GetStep() const { return m_step; }
	const char* GetUnit() const { return m_strUnit.c_str(); }
	TipFunc GetTipFunc() const { return m_TipFunc; }

protected:
	any m_min;
	any m_max;
	double m_step;
	PropertyString m_strUnit;
	TipFunc m_TipFunc;
};

template<class T>
class prop_range_template : public prop_range
{
public:
	void SetRange(const T& min_val, const T& max_val) 
	{
		if (min_val > max_val)
		{
			m_min = max_val;
			m_max = min_val;
		}
		else
		{
			m_min = min_val;
			m_max = max_val;
		}
	}
	void SetStep(const double step)
	{
		m_step = step;
	}

	void SetUnit(const char* szUnit)
	{
		m_strUnit = szUnit;
	}

	void SetTipFunc(TipFunc tipFunc)
	{
		m_TipFunc = tipFunc;
	}
};

enum {
	BCGP_PROP_WAY_DEFAULT = 0,	// default edit control
	BCGP_PROP_WAY_FILENAME,		// when clicked, an AFileDialog would be created
	BCGP_PROP_WAY_OPTIONS,		// when clicked, a combo box would be shown
	BCGP_PROP_WAY_COLOR,		// when clicked, a CCustomColorDialog would be created
	BCGP_PROP_WAY_RANGE,		// when clicked, a slider control would be created
	BCGP_PROP_WAY_CUSTOM,		// show a button, user define what would happen when clicked, when getting value, setting value
};


struct prop_custom_funcs;

struct PROP_ITEM {
	PROP_ITEM()
		: iWay(BCGP_PROP_WAY_DEFAULT)
		, pOptions(NULL)
		, pRange(NULL)
		, bIsReadOnly(false)
		, bIsVisible(true) {}
	any Val;
	int iWay;
	std::string strName;
	std::string strDesc;
	std::string strPath;
	std::string strFileExt;
	prop_set* pOptions;
	prop_range* pRange;

	bool bIsReadOnly;
	bool bIsVisible;
};

class CPropertyArray;
class CPropertyItem
{
public:
	CPropertyItem(const any& val
		, const char* szName
		, const char* szDesc
		, int iWay
		, const char* szPath
		, prop_set* pOptions
		, prop_range* pRange
		, bool bIsReadonly
		, bool bIsVisible);

	CPropertyItem(const any& val
		, const char* szName
		, const char* szDesc
		, int iWay
		, const char* szPath
		, const char* szFileExt
		, prop_set* pOptions
		, prop_range* pRange
		, bool bIsReadonly
		, bool bIsVisible);

	CPropertyItem(const PROP_ITEM& propItem);
	CPropertyItem(const CPropertyItem& rhs);

	const char * GetName() const;
	const char * GetDesc() const;
	int GetWay() const;
	bool IsReadonly() const { return m_bIsReadonly; }
	// why this? because the custom way will make the m_val.type() diff from the GetValue().type()
	const type_info& GetValueType() const;
	any& GetValue();
	const any& GetValue() const;
	void SetValue(const any& val);
	const char* GetPath() const;
	const char* GetFileExt() const;
	const prop_set* GetSet() const { return m_pOptions; }
	const prop_range* GetRange() const { return m_pRange; }
	prop_custom_funcs* GetCustomFuncs() const { return m_pCustomFuncs; }
	void SetReadOnly(bool bReadOnly);
private:
	void init_check_state();

private:

	// Basic props
	PropertyString m_strName;
	PropertyString m_strDesc;
	any m_val;
	int m_iWay;
	bool m_bIsReadonly;
	bool m_bIsVisible;

	// Additional props
	PropertyString m_strPath;
	PropertyString m_strFileExt;
	prop_set* m_pOptions;
	prop_range* m_pRange;
	prop_custom_funcs* m_pCustomFuncs;
};

class CPropertyArray
{
	friend class CPropertyItem;
public:	

	//	Message handler will derive from this listener
	struct Listener
	{
		virtual void OnPropUpdated(bool bIsCommit) const = 0;
	};

	void AddListener(Listener* pListener);
	void RemoveListener(Listener* pListener);
	void RaisePropUpdatedMsg(bool bIsCommit);

public:

	static CPropertyArray* CreatePropertyArray();
	static void DestroyPropertyArray(CPropertyArray* pPropArray);

	CPropertyArray();
	CPropertyArray(const CPropertyArray& rhs);
	~CPropertyArray();

	//	Property concerned operations
	size_t AddProperty(bool val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(char val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(unsigned char val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(short val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(unsigned short val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(int val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(unsigned int val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(long val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(unsigned long val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(__int64 val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(unsigned __int64 val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(float val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(double val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(A3DVECTOR3 val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(const A3DMATRIX4& val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(const std::string& val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(A3DFontMan::FONTTYPE* val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(CPropertyArray* val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(const CPropertyArray& val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

	//	Property concerned operations
	size_t AddProperty(prop_custom_funcs* val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);


	size_t AddProperty(const PROP_ITEM& propItem);
	void SetReadOnly(bool bRdOnly) { m_bReadOnly = bRdOnly; }
	bool IsReadOnly() const { return m_bReadOnly; }

	const CPropertyItem& GetProperty(size_t iIdx) const;
	CPropertyItem& GetProperty(size_t iIdx);
	CPropertyItem* GetPropertyPtr(size_t iIdx); 
	const any& GetPropVal(size_t iIdx) const;
	any& GetPropVal(size_t iIdx);
	void SetPropVal(size_t iIdx, const any& val);
	size_t GetCount() const;
	void Clear();

private:

	//	Property concerned operations
	size_t AddProperty(any val
		, const char* szName
		, const char* szDesc = NULL
		, int way = BCGP_PROP_WAY_DEFAULT
		, const char* szPath = NULL
		, prop_set* pOptions = NULL
		, prop_range* pRange = NULL
		, bool isReadonly = false
		, bool isVisible = true);

private:

	typedef std::vector<CPropertyItem> PropertyPointerArray;
	PropertyPointerArray m_arrPropItems;
	typedef std::vector<Listener*> ListenerList;
	ListenerList m_lstListeners;
	bool m_bReadOnly;
};

class CPropertyInterface;
class CPropertyUpdateListener : public CPropertyArray::Listener
{
public:
	CPropertyUpdateListener(CPropertyInterface* pPropertyInterface);	

	//	Override operation
	virtual void OnPropUpdated(bool bIsCommit) const;
private:
	CPropertyInterface* m_pPropertyInterface;
};

#endif