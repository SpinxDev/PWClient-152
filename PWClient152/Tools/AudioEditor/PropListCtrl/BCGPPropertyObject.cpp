#include "stdafx.h"
#include <algorithm>
#include <A3DVector.h>
#include <A3DMatrix.h>
#include <A3DFontMan.h>
//#include "scope_wrappers.h"
#include "BCGPPropertyObject.h"
#include "PropertyInterface.h"
#include "prop_custom_funcs.h"

//////////////////////////////////////////////////////////////////////////
//
//	Local Functions
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//	Impl for prop_set_template
//
//////////////////////////////////////////////////////////////////////////

//	Tell complier to instance the specified type of prop_set_template
static prop_set_template<char> s_l_set_char;
static prop_set_template<unsigned char> s_l_set_uchar;
static prop_set_template<short> s_l_set_short;
static prop_set_template<unsigned short> s_l_set_ushort;
static prop_set_template<int> s_l_set_int;
static prop_set_template<unsigned int> s_l_set_uint;
static prop_set_template<long> s_l_set_long;
static prop_set_template<unsigned long> s_l_set_ulong;
static prop_set_template<__int64> s_l_set_int64;
static prop_set_template<unsigned __int64> s_l_set_uint64;

//	Tell complier to instance the specified type of prop_range_template
static prop_range_template<char> s_l_range_char;
static prop_range_template<unsigned char> s_l_range_uchar;
static prop_range_template<short> s_l_range_short;
static prop_range_template<unsigned short> s_l_range_ushort;
static prop_range_template<int> s_l_range_int;
static prop_range_template<unsigned int> s_l_range_uint;
static prop_range_template<long> s_l_range_long;
static prop_range_template<unsigned long> s_l_range_ulong;
static prop_range_template<__int64> s_l_range_int64;
static prop_range_template<unsigned __int64> s_l_range_uint64;

//////////////////////////////////////////////////////////////////////////
//
//	Impl for CPropertyItem
//
//////////////////////////////////////////////////////////////////////////

CPropertyItem::CPropertyItem(const any& val
			  , const char* szName
			  , const char* szDesc
			  , int iWay
			  , const char* szPath
			  , prop_set* pOptions
			  , prop_range* pRange
			  , bool bIsReadonly
			  , bool bIsVisible)
			  : m_val(val)
			  , m_strName(szName ? szName : "")
			  , m_strDesc(szDesc ? szDesc : "")
			  , m_iWay(iWay)
			  , m_strPath(szPath ? szPath : "")
			  , m_strFileExt("")
			  , m_pOptions(pOptions)
			  , m_pCustomFuncs(NULL)
			  , m_pRange(pRange)
			  , m_bIsReadonly(bIsReadonly)
			  , m_bIsVisible(bIsVisible)
{
	init_check_state();
}

CPropertyItem::CPropertyItem(const any& val
			  , const char* szName
			  , const char* szDesc
			  , int iWay
			  , const char* szPath
			  , const char* szFileExt
			  , prop_set* pOptions
			  , prop_range* pRange
			  , bool bIsReadonly
			  , bool bIsVisible)
			  : m_val(val)
			  , m_strName(szName ? szName : "")
			  , m_strDesc(szDesc ? szDesc : "")
			  , m_iWay(iWay)
			  , m_strPath(szPath ? szPath : "")
			  , m_strFileExt(szFileExt ? szFileExt : "")
			  , m_pOptions(pOptions)
			  , m_pCustomFuncs(NULL)
			  , m_pRange(pRange)
			  , m_bIsReadonly(bIsReadonly)
			  , m_bIsVisible(bIsVisible)
{
	init_check_state();
}

CPropertyItem::CPropertyItem(const CPropertyItem& rhs)
: m_val(rhs.m_val)
, m_strName(rhs.m_strName)
, m_strDesc(rhs.m_strDesc)
, m_iWay(rhs.m_iWay)
, m_strPath(rhs.m_strPath)
, m_strFileExt(rhs.m_strFileExt)
, m_pOptions(rhs.m_pOptions)
, m_pCustomFuncs(rhs.m_pCustomFuncs)
, m_pRange(rhs.m_pRange)
, m_bIsReadonly(rhs.m_bIsReadonly)
, m_bIsVisible(rhs.m_bIsVisible)
{

}

CPropertyItem::CPropertyItem(const PROP_ITEM& propItem)
: m_val(propItem.Val)
, m_strName(propItem.strName)
, m_strDesc(propItem.strDesc)
, m_strPath(propItem.strPath)
, m_strFileExt(propItem.strFileExt)
, m_iWay(propItem.iWay)
, m_pOptions(propItem.pOptions)
, m_pCustomFuncs(NULL)
, m_pRange(propItem.pRange)
, m_bIsReadonly(propItem.bIsReadOnly)
, m_bIsVisible(propItem.bIsVisible)
{
	init_check_state();
}

void CPropertyItem::init_check_state()
{
	if (m_pOptions && m_pOptions->GetCount() && m_pOptions->GetValueByIdx(0).type() != m_val.type())
		throw std::logic_error("incompatible type of value and the prop_set.");
	if (m_pRange && m_pRange->GetMin().type() != m_val.type())
		throw std::logic_error("incompatible type of value and the prop_range.");
	if (m_iWay == BCGP_PROP_WAY_CUSTOM) {
		ASSERT(m_val.type() == typeid(prop_custom_funcs*));
		m_pCustomFuncs = any_cast<prop_custom_funcs*>(m_val);
	}
}

const char * CPropertyItem::GetName() const 
{ 
	return m_strName.c_str(); 
}

const char * CPropertyItem::GetDesc() const 
{ 
	return m_strDesc.c_str(); 
}

int CPropertyItem::GetWay() const 
{ 
	return m_iWay; 
}

// why this? because the custom way will make the m_val.type() diff from the GetValue().type()
const type_info& CPropertyItem::GetValueType() const
{
	return m_val.type();
}

any& CPropertyItem::GetValue() 
{
	if (GetWay() == BCGP_PROP_WAY_CUSTOM) {
		if (any* pVal = m_pCustomFuncs->OnGetValue())
			return *pVal;
		else
			throw std::logic_error("try to get a value that is not implemented in a custom prop.");
	}
	return m_val; 
}

const any& CPropertyItem::GetValue() const 
{ 
	if (GetWay() == BCGP_PROP_WAY_CUSTOM) {
		if (const any* pVal = m_pCustomFuncs->OnGetValue())
			return *pVal;
		else
			throw std::logic_error("try to get a value that is not implemented in a custom prop.");
	}
	return m_val; 
}

void CPropertyItem::SetValue(const any& val) 
{ 
	if (GetWay() == BCGP_PROP_WAY_CUSTOM) {
		m_pCustomFuncs->OnSetValue(val);
	}
	else
		m_val = val; 
}

const char* CPropertyItem::GetPath() const 
{ 
	return m_strPath.c_str(); 
}

const char* CPropertyItem::GetFileExt() const
{
	return m_strFileExt.c_str();
}

void CPropertyItem::SetReadOnly(bool bReadOnly)
{
	m_bIsReadonly = bReadOnly;
}

//////////////////////////////////////////////////////////////////////////
//
//	Impl for CPropertyArray
//
//////////////////////////////////////////////////////////////////////////

CPropertyArray* CPropertyArray::CreatePropertyArray()
{
	return new CPropertyArray;
}

void CPropertyArray::DestroyPropertyArray(CPropertyArray* pPropArray)
{
	delete pPropArray;
}

CPropertyArray::CPropertyArray()
: m_bReadOnly(false)
{
}

CPropertyArray::CPropertyArray(const CPropertyArray& rhs)
: m_arrPropItems(rhs.m_arrPropItems)
, m_lstListeners(rhs.m_lstListeners)
, m_bReadOnly(false)
{

}

CPropertyArray::~CPropertyArray()
{
}


//	Property concerned operations
size_t CPropertyArray::AddProperty(bool val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(char val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(unsigned char val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(short val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(unsigned short val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(int val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(unsigned int val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}


//	Property concerned operations
size_t CPropertyArray::AddProperty(long val
									, const char* szName
									, const char* szDesc /*= NULL*/
									, int way /*= BCGP_PROP_WAY_DEFAULT*/
									, const char* szPath /*= NULL*/
									, prop_set* pOptions /*= NULL*/
									, prop_range* pRange /*= NULL*/
									, bool isReadonly /*= false*/
									, bool isVisible /*= true*/)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(unsigned long val
									, const char* szName
									, const char* szDesc /*= NULL*/
									, int way /*= BCGP_PROP_WAY_DEFAULT*/
									, const char* szPath /*= NULL*/
									, prop_set* pOptions /*= NULL*/
									, prop_range* pRange /*= NULL*/
									, bool isReadonly /*= false*/
									, bool isVisible /*= true*/)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(__int64 val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(unsigned __int64 val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(float val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(double val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(A3DVECTOR3 val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(const A3DMATRIX4& val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(const std::string& val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(A3DFontMan::FONTTYPE* val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(CPropertyArray* val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(*val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(const CPropertyArray& val
								   , const char* szName
								   , const char* szDesc/* = NULL */
								   , int way/*  = BCGP_PROP_WAY_DEFAULT */
								   , const char* szPath/* = NULL */
								   , prop_set* pOptions/* = NULL */
								   , prop_range* pRange/* = NULL */
								   , bool isReadonly /* = false */
								   , bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(prop_custom_funcs* val
									, const char* szName
									, const char* szDesc/* = NULL */
									, int way/*  = BCGP_PROP_WAY_DEFAULT */
									, const char* szPath/* = NULL */
									, prop_set* pOptions/* = NULL */
									, prop_range* pRange/* = NULL */
									, bool isReadonly /* = false */
									, bool isVisible/* = true */)
{
	return AddProperty(any(val), szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible);
}

//	Property concerned operations
size_t CPropertyArray::AddProperty(any val
				   , const char* szName
				   , const char* szDesc
				   , int way 
				   , const char* szPath
				   , prop_set* pOptions
				   , prop_range* pRange
				   , bool isReadonly 
				   , bool isVisible)
{
	m_arrPropItems.push_back(CPropertyItem(val, szName, szDesc, way, szPath, pOptions, pRange, isReadonly, isVisible));
	return m_arrPropItems.size() - 1;
}

size_t CPropertyArray::AddProperty(const PROP_ITEM& propItem)
{
	CPropertyItem tmpItem(propItem);
	m_arrPropItems.push_back(tmpItem);
	return m_arrPropItems.size() - 1;
}

const CPropertyItem& CPropertyArray::GetProperty(size_t iIdx) const 
{ 
	return m_arrPropItems.at(iIdx); 
}

CPropertyItem& CPropertyArray::GetProperty(size_t iIdx) 
{ 
	return m_arrPropItems.at(iIdx); 
}

CPropertyItem* CPropertyArray::GetPropertyPtr(size_t iIdx)
{
	return &m_arrPropItems.at(iIdx); 
}

const any& CPropertyArray::GetPropVal(size_t iIdx) const 
{
	return m_arrPropItems.at(iIdx).GetValue(); 
}

any& CPropertyArray::GetPropVal(size_t iIdx) 
{ 
	return m_arrPropItems.at(iIdx).GetValue(); 
}

void CPropertyArray::SetPropVal(size_t iIdx, const any& val) 
{ 
	m_arrPropItems.at(iIdx).SetValue(val); 
}

size_t CPropertyArray::GetCount() const 
{ 
	return m_arrPropItems.size(); 
}

void CPropertyArray::Clear()
{
	m_arrPropItems.clear();
}

void CPropertyArray::AddListener(Listener* pListener)
{
	m_lstListeners.push_back(pListener);
}

void CPropertyArray::RemoveListener(Listener* pListener)
{
	ListenerList::iterator itr = std::find(m_lstListeners.begin(), m_lstListeners.end(), pListener);
	if (itr == m_lstListeners.end())
		return;
	m_lstListeners.erase(itr);
}

void CPropertyArray::RaisePropUpdatedMsg(bool bIsCommit)
{
	for (ListenerList::const_iterator itr = m_lstListeners.begin()
		; itr != m_lstListeners.end()
		; ++itr)
	{
		(*itr)->OnPropUpdated(bIsCommit);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//	Impl for CPropertyUpdateListener
//
//////////////////////////////////////////////////////////////////////////

CPropertyUpdateListener::CPropertyUpdateListener(CPropertyInterface* pPropertyInterface)
: m_pPropertyInterface(pPropertyInterface)
{
}

void CPropertyUpdateListener::OnPropUpdated(bool bIsCommit) const
{
	m_pPropertyInterface->UpdateProperty(true, bIsCommit);
}

