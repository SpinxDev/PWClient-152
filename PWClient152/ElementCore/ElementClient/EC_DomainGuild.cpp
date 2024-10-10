// Filename	: EC_DomainGuild.cpp
// Creator	: XuWenbin
// Date		: 2012/7/23

#include "EC_DomainGuild.h"
#include "globaldataman.h"

//	class CECDomainGuildInfo

CECDomainGuildInfo & CECDomainGuildInfo::Instance()
{
	static CECDomainGuildInfo s_instance;
	return s_instance;
}

CECDomainGuildInfo::CECDomainGuildInfo()
{
	m_pDomainInfo = globaldata_getdomaininfos();
	m_DomainSize = m_pDomainInfo->size();
	for (int i(0); i < m_DomainSize; ++ i)
	{
		const DOMAIN_INFO &info = (*m_pDomainInfo)[i];
		m_mDomain[info.id] = &info;
	}
}

CECDomainGuildInfo::~CECDomainGuildInfo()
{
}

const void* CECDomainGuildInfo::FindInfo(int iDomainID) const
{
	return Find(iDomainID);
}

const DOMAIN_INFO* CECDomainGuildInfo::Find(int iDomainID)const
{
	const DOMAIN_INFO *ret = NULL;
	DOMAINMAP::const_iterator cit = m_mDomain.find(iDomainID);
	if (cit != m_mDomain.end())
		ret = cit->second;
	return ret;
}

int	CECDomainGuildInfo::GetCountForOwner(int idOwner)const
{
	int count(0);
	for (int i(0); i < m_DomainSize; ++ i)
	{
		const DOMAIN_INFO &info = (*m_pDomainInfo)[i];
		if (info.id_owner == idOwner){
			++ count;
		}
	}
	return count;
}

//	判断帮派 factionID（不为0时有效）当前是否是 domainID 的拥有者
bool CECDomainGuildInfo::IsDomainOwner(int domainID, int factionID)const{
	bool bRet(false);
	if (factionID > 0){
		if (const DOMAIN_INFO *pDomain = Find(domainID)){
			bRet = (pDomain->id_owner == factionID);
		}
	}
	return bRet;
}

//	查找帮派 factionID（不为0时有效）对应的帮派颜色
bool CECDomainGuildInfo::FindFactionColorIndex(int factionID, char &colorIndex)const{
	if (factionID > 0){
		for (int i(0); i < GetSize(); ++ i){
			const DOMAIN_INFO *pDomain = (const DOMAIN_INFO *)GetInfo(i);
			if (pDomain->id_owner == factionID){
				colorIndex = pDomain->color;
				return true;
			}
		}
	}
	return false;
}