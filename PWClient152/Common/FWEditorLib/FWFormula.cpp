// Formula.cpp: implementation of the FWFormula class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFormula.h"
#include "CodeTemplate.h"
#include "FWOperand.h"
#include "FWOperator.h"
#include "FWFormulaElement.h"
#include "FWInnerProperty.h"
#include "FWFormulaList.h"
#include <AScriptFile.h>

#define new A_DEBUG_NEW




FWFormula::FWFormula()
{

}

FWFormula::~FWFormula()
{
	ALISTPOSITION pos = m_lstPostfix.GetHeadPosition();
	while (pos)
	{
		FWFormulaElement *p = m_lstPostfix.GetNext(pos);
		SAFE_DELETE(p);
	}
	m_lstPostfix.RemoveAll();
}

bool FWFormula::Load(AScriptFile *pFile)
{
	BEGIN_FAKE_WHILE;

	CHECK_BREAK(pFile->GetNextToken(true));
	m_strLValue = pFile->m_szToken;

	CHECK_BREAK(pFile->GetNextToken(false));
	CHECK_BREAK(AString(pFile->m_szToken) == '=');

	ReadAndBuildPostfixExpression(pFile);

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

bool FWFormula::Compute() const
{
	AFEStack stkTemp;

	// process the postfix expression
	ALISTPOSITION pos = m_lstPostfix.GetHeadPosition();
	while (pos)
	{
		FWFormulaElement *pElem = m_lstPostfix.GetNext(pos);

		if (pElem->GetType() == FE_TYPE_OPERAND)
		{
			stkTemp.Push(pElem->Clone());
		}
		else
		{
			ASSERT(pElem->GetType() == FE_TYPE_OPERATOR);
			
			FWOperator *op = static_cast<FWOperator *>(pElem);

			FWOperand *p2 = static_cast<FWOperand *>(stkTemp.Pop());
			FWOperand *p1 = static_cast<FWOperand *>(stkTemp.Pop());

			FWOperand *p = NULL;
			try
			{
				p = new FWOperand(op->Operate(*p1, *p2));
			}
			catch (...) 
			{
				SAFE_DELETE(p1);
				SAFE_DELETE(p2);
				FWFormulaElement *p = NULL;
				while ((p = stkTemp.Pop()))
				{
					SAFE_DELETE(p);
				}
				return false;
			}
			
			stkTemp.Push(p);

			SAFE_DELETE(p1);
			SAFE_DELETE(p2);
		}
	}

	// get result
	FWOperand *pRet = static_cast<FWOperand *>(stkTemp.Pop());

	// modify lvalue
	// the object to modify is determined by FWFormulaList::GetEnvAssembly()
	FWInnerProperty lProp;
	if (!lProp.InitWithString(FWFormulaList::GetEnvAssembly(), m_strLValue))
	{
		SAFE_DELETE(pRet); 
		return false;
	}

	lProp = *pRet;
	SAFE_DELETE(pRet); 
	return true;
}

void FWFormula::ReadAndBuildPostfixExpression(AScriptFile *pFile)
{
	// assume the expression is correct

	// �㷨��
	// 1)����������һԪ�ء�
	// 2)�����Ǹ��������������
	// 3)�����Ǹ������ţ�����ѹջ��
	// 4)�����Ǹ����������
	//   i) ����ջΪ�գ����������ѹջ��
	//   ii) ����ջ���ǿ����ţ����������ѹջ��
	//   iii) ������������ջ����������ȼ��ߣ����������ѹ��ջ�С�
	//   iv) ����ջ���������ջ��������ظ�����4��
	// 5)�����Ǹ������ţ�ջ������������ջ�������ֱ�����������š������ų�ջ��������
	// 6)�������뻹δ��ϣ���ת������1��
	// 7)����������ϣ�ջ��ʣ������в�������ջ��������ǡ�

	AFEStack stkTemp;
	AString token;
	while (pFile->GetNextToken(false))
	{
		token = pFile->m_szToken;
		int nType = FWFormulaElement::DetermineType(token);
		switch(nType) 
		{
		case FE_TYPE_OPERAND:
			m_lstPostfix.AddTail(new FWOperand(token));
			break;
		case FE_TYPE_LEFT_BRACKET:
			stkTemp.Push(new FWFormulaElement(token, FE_TYPE_LEFT_BRACKET));
			break;
		case FE_TYPE_OPERATOR:
			{
				bool bContinue = false;
				do
				{
					bContinue = false;
					if (stkTemp.GetElementNum() == 0 ||
						stkTemp.Peek()->GetType() == FE_TYPE_LEFT_BRACKET)
					{
						stkTemp.Push(new FWOperator(token));
					}
					else
					{
						ASSERT(stkTemp.Peek()->GetType() == FE_TYPE_OPERATOR);
						if (FWOperator(token) > * (static_cast<FWOperator *>(stkTemp.Peek())))
						{
							stkTemp.Push(new FWOperator(token));
						}
						else
						{
							m_lstPostfix.AddTail(stkTemp.Pop());
							bContinue = true;
						}
					}
				} while(bContinue);
			}
			break;
		case FE_TYPE_RIGHT_BRACKET:
			{
				FWFormulaElement *pElem = NULL;
				while ((pElem = stkTemp.Pop())->GetType() != FE_TYPE_LEFT_BRACKET)
				{
					m_lstPostfix.AddTail(pElem);
				}
				SAFE_DELETE(pElem);
			}
			break;
		default:
			ASSERT(false);
			break;
		}
	}
	FWFormulaElement *pElem = NULL;
	while (stkTemp.GetElementNum() > 0)
	{
		m_lstPostfix.AddTail(stkTemp.Pop());
	}
}
