// Filename	: EC_DomainBase.h
// Creator	: Han Guanghui
// Date		: 2012/7/18

#ifndef _EC_DOMAIN_BASE_H_
#define _EC_DOMAIN_BASE_H_

#include "AUIImagePicture.h"
#include "globaldataman.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
	TypeName(const TypeName&);\
	void operator=(const TypeName&)

class CECDomainInfo
{
public:
	virtual ~CECDomainInfo(){};
	
	typedef abase::vector<A3DPOINT2> DomainVerts;
	typedef abase::vector<int> DomainFaces;
	virtual const DomainVerts&			GetVerts(int iIndex)	const = 0;
	virtual const DomainFaces&			GetFaces(int iIndex)	const = 0;
	virtual const A3DPOINT2&			GetCenter(int iIndex)	const = 0;
	virtual const void*					GetInfo(int iIndex)		const = 0;
	virtual const void*					FindInfo(int iDomainID) const = 0;
	virtual size_t						GetSize() const = 0;
};


class CECDomain
{
public:
	
	CECDomain():m_pImgDomain(NULL), m_pDomainInfo(NULL){}
	virtual ~CECDomain();

	void								SetDomainInfo(PAUIIMAGEPICTURE pImgDomain, CECDomainInfo* pDomainInfo);

	enum
	{
		OUT_OF_DOMAIN = -1,
	};
	int									GetDomainIndexByCoordinate(A3DPOINT2 pt) const;
	void								RenderArea(int nIndex, A3DCOLOR cl);
	void								RenderArrow(int nIndex1, int nIndex2, A2DSprite *pArrow);
	void								RenderImage(int nIndex, A2DSprite *pImg);
	void								RenderImageOffsetCenter(int nIndex, const A3DPOINT2 &offset, A2DSprite* pImg, bool bRePosition = false);
	CECDomainInfo*						GetDomainInfo(){return m_pDomainInfo;}
	const PAUIIMAGEPICTURE				GetDomainImg() const {return m_pImgDomain;}
	A3DPOINT2							PixelToScreen(A3DPOINT2 pt) const;
	A3DPOINT2							ScreenToPixel(A3DPOINT2 pt) const;
	A3DPOINT2							CalcImgOffset(const A3DPOINT2 &ptPixel, const A3DPOINT2 &ptMouse)const;

protected:

	PAUIIMAGEPICTURE			m_pImgDomain;
	abase::vector<A3DRECT>		m_vecAreaRect;
	CECDomainInfo*				m_pDomainInfo;


private:
	DISALLOW_COPY_AND_ASSIGN(CECDomain);
};

#endif //_EC_DOMAIN_BASE_H_

