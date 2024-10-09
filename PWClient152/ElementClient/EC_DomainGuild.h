// Filename	: EC_DomainGuild.h
// Creator	: XuWenbin
// Date		: 2012/7/23

#pragma once

#include "EC_DomainBase.h"
#include <hashmap.h>

class CECDomainGuildInfo : public CECDomainInfo
{
public:
	static CECDomainGuildInfo& Instance();

	virtual ~CECDomainGuildInfo();

	virtual const DomainVerts& GetVerts(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainInfo)[iIndex].verts;
	}
	virtual const DomainFaces& GetFaces(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainInfo)[iIndex].faces;
	}
	virtual const A3DPOINT2& GetCenter(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (*m_pDomainInfo)[iIndex].center;
	}
	virtual const void*		GetInfo(int iIndex) const
	{
		ASSERT(iIndex < m_DomainSize && iIndex >= 0);
		return (void*)(&(*m_pDomainInfo)[iIndex]);
	}
	virtual const void*		FindInfo(int iDomainID) const;
	virtual size_t GetSize() const
	{
		return m_pDomainInfo->size();
	}

	const DOMAIN_INFO* Find(int iDomainID)const;
	int	 GetCountForOwner(int idOwner)const;
	bool IsDomainOwner(int domainID, int factionID)const;
	bool FindFactionColorIndex(int factionID, char &colorIndex)const;
	
protected:
	abase::vector<DOMAIN_INFO>*	m_pDomainInfo;
	int							m_DomainSize;

	typedef abase::hash_map<int, const DOMAIN_INFO *>	DOMAINMAP;
	DOMAINMAP					m_mDomain;

private:
	CECDomainGuildInfo();
	CECDomainGuildInfo(const CECDomainGuildInfo&);
	CECDomainGuildInfo& operator=(const CECDomainGuildInfo&);
};
