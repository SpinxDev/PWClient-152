#ifndef _WLD_BCGPPROPLIST_H_
#define _WLD_BCGPPROPLIST_H_

#include <map>
#include <string>
#include <typeinfo>
#include <hashmap.h>

class CBCGPPropList;
class CPropertyItem;
class CPropertyArray;
class CBCGPPropListEx;

class bcgp_bad_prop : public std::exception
{
public:
	bcgp_bad_prop(const std::string strMsg)
		: m_str(strMsg.c_str()) {}
	bcgp_bad_prop(const char* szMsg)
		: m_str(szMsg ? szMsg : "unknown bcgp_bad_prop.") {}
	const char* what() { return m_str.c_str(); }
private:
	std::string m_str;
};

class CBCGPPropertyListWrapper
{
public:

	typedef CBCGPProp* (*BCGPCreater)(CBCGPPropertyListWrapper* pListWrapper, const CPropertyItem* pProp);
	typedef void (*BCGPUpdater)(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp);

	typedef std::map<int, BCGPCreater> BCGPPropCreaterMap;
	typedef std::map<int, BCGPUpdater> BCGPPropUpdaterMap;

	friend void Attach_recur(CBCGPPropertyListWrapper* pListWrapper, CBCGPProp* pCurProp, const CPropertyArray* pData);
	friend void Update_recur(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyArray* pData, CBCGPProp* pBCGPGroup);

public:
	CBCGPPropertyListWrapper();
	~CBCGPPropertyListWrapper();

	void AttachTo(CPropertyArray* pData);
	//	bSave: true - from user interface to data
	//	bSave: false - from data to user interface
	//	bIsCommit: true - notify the real object to commit the data (to update the command)
	//	bIsCommit: false - only update the real object data but
	void UpdateData(bool bSave, bool bIsCommit = false);

	//	Create proplist
	BOOL OnCreate(const CRect& rc, CWnd* pParent);
	
	void OnSize(UINT flag, int cx, int cy);

	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);

private:

	void Init();
	
private:
	CPropertyArray* m_pData;
	CBCGPPropListEx* m_pList;
	typedef size_t DataIndex;
	typedef size_t UIIndex;
	typedef std::map<DataIndex, UIIndex> Data2UI;
	Data2UI m_mapData2UI;


	BCGPPropCreaterMap m_mapCreaterFuncs;
	BCGPPropUpdaterMap m_mapUpdaterFuncs;
};

#endif