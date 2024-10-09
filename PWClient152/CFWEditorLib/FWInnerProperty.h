#ifndef _FWINNERPROPERTY_H_
#define _FWINNERPROPERTY_H_

#include "A3DGFXEditorInterface.h"
#include "CodeTemplate.h"
#include "FWConfig.h"


class A3DGFXEx;
class A3DGFXElement;
class A3DGFXKeyPointCtrlBase;
class A3DGFXKeyPoint;
struct COutterProperty;
class CPropertyValue;

//////////////////////////////////////////////////////////////////////////
// inner property 
//////////////////////////////////////////////////////////////////////////



class FWInnerProperty  
{
	// raw property in string form
	// the string will be changed during the
	// parsing process
	AString m_strNameFromINI;

	// pointer to a real object that is used
	// during and after the parsing process
	union _POINTER
	{
		A3DGFXEx *pGfx;
		A3DGFXElement *pElem;
		A3DGFXKeyPointCtrlBase *pCtrl;
		A3DGFXKeyPoint *pKP;
	} m_Pointer;
	enum
	{
		POINTER_TYPE_UNKNOWN,
		POINTER_TYPE_GFX,
		POINTER_TYPE_ELEMENT,
		POINTER_TYPE_CTRL,
		POINTER_TYPE_KEYPOINT
	};
	// type of m_Pointer
	int m_pointerType;

	// index of property of some object in gfxcommon
	int m_nPropertyID;
	
	// index of sub property of some object in gfxcommon
	int m_nSubObjID;

	// whether the object is assiciated to a valid object in gfxcommon
	// this is true only when InitWithString() succeed
	bool m_bIsValid;
public:
	// some property-get method
	_POINTER GetPointer() const { return m_Pointer; }
	int GetPointerType() const { return m_pointerType; }
	int GetPropertyID() const { return m_nPropertyID; }
	int GetSubObjID() const { return m_nSubObjID; }
	bool IsValid() const { return m_bIsValid; }
	
	FWInnerProperty();
	FWInnerProperty(const FWInnerProperty &src);
	~FWInnerProperty();
	FWInnerProperty & operator = (const FWInnerProperty & src);
	FWInnerProperty & operator = (const GFX_PROPERTY  & val);
	operator GFX_PROPERTY() const;

	// init and parse
	bool InitWithString(A3DGFXEx *pGfx, const AString& strPropNameInINI);
protected:
	// used in parsing process 
	// get the postfix number in a string
	// and remove it from original string
	int DistillPostfixNum(AString &strName);

	// used in parsing process 
	// determin whether the string to be parsed is empty
	bool IsTheLastPart() const;

	// used in parsing process.
	// get the sub string before first "."
	// and remove it (including ".") from original string
	AString CutLeadingPart();

	// used in parsing process
	// called according to different type of m_pointerType
	bool KeypointParse();
	bool CtrlParse();
	bool ElementParse();
	bool GfxParse();
};


#endif 