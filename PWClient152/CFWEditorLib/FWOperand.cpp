// Operand.cpp: implementation of the FWOperand class.
//
//////////////////////////////////////////////////////////////////////

#include "FWOperand.h"
#include "FWAssemblyProfile.h"
#include "FWInnerProperty.h"
#include "FWOutterProperty.h"
#include "A3DGFXEx.h"
#include "FWFormulaList.h"


#define new A_DEBUG_NEW



#pragma warning(disable : 4715)



FWOperand::FWOperand()
{

}

FWOperand::~FWOperand()
{

}

FWOperand & FWOperand::operator = (const FWOperand & src)
{
	FWFormulaElement::operator = (src);
	m_bIsVal = src.IsVal();
	if (src.IsVal())
	{
		m_Val = src.GetVal();
	}

	return *this;
}

FWOperand::operator GFX_PROPERTY() const
{
	if (IsVal())
	{
		return m_Val;
	}
	else
	{
		BEGIN_FAKE_WHILE;
		
		CHECK_BREAK(!m_strRaw.IsEmpty());

		// process sign
		if (m_strRaw[0] == '+')
		{
			FWOperand tmp(m_strRaw.Right(m_strRaw.GetLength() - 1));
			return GFX_PROPERTY(tmp);
		}
		else if (m_strRaw[0] == '-')
		{
			FWOperand tmp(m_strRaw.Right(m_strRaw.GetLength() - 1));
			return GFX_PROPERTY(tmp) * GFX_PROPERTY(-1L);
		}
		
		// constant
		if (m_strRaw[0] >= '0' && m_strRaw[0] <= '9' || m_strRaw[0] == '.') 
		{
			if (m_strRaw.Find('.') != -1)
			{
				return GFX_PROPERTY(m_strRaw.ToFloat());
			}
			else
			{
				return GFX_PROPERTY(m_strRaw.ToInt());
			}
		}
		// inner property
		else if (m_strRaw.Find('.') != -1) 
		{
			FWInnerProperty tmp;
			CHECK_BREAK(tmp.InitWithString(FWFormulaList::GetEnvAssembly(), m_strRaw));
			return GFX_PROPERTY(tmp);
		}
		// outter property
		else 
		{
			FWOutterProperty *pProp = FWFormulaList::GetEnvOutterPropertyList()->FindByName(m_strRaw);
			CHECK_BREAK(pProp);
			return GFX_PROPERTY(*pProp);
		}
		END_FAKE_WHILE;

		ASSERT(false);
	}
	
}

FWOperand operator + (const FWOperand &left, const FWOperand &right)
{
	return GFX_PROPERTY(left) + GFX_PROPERTY(right);
}

FWOperand operator - (const FWOperand &left, const FWOperand &right)
{
	return GFX_PROPERTY(left) - GFX_PROPERTY(right);
}

FWOperand operator * (const FWOperand &left, const FWOperand &right)
{
	return GFX_PROPERTY(left) * GFX_PROPERTY(right);
}

FWOperand operator / (const FWOperand &left, const FWOperand &right)
{
	return GFX_PROPERTY(left) / GFX_PROPERTY(right);
}

