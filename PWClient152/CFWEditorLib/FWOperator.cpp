// Operator.cpp: implementation of the FWOperator class.
//
//////////////////////////////////////////////////////////////////////

#include "FWOperator.h"
#include "FWOperand.h"

#define new A_DEBUG_NEW






FWOperator::~FWOperator()
{

}

#pragma warning(disable : 4715)

FWOperand FWOperator::Operate(const FWOperand& left, const FWOperand& right)
{
	switch(GetOP()) 
	{
	case '+':
		return left + right;
	case '-':
		return left - right;
	case '*':
		return left * right;
	case '/':
		return left / right;
	}
	ASSERT(false);
}

int FWOperator::GetPRI() const
{
    switch (GetOP())
    {
    case '+': return 1;
    case '-': return 1;
    case '*': return 2;
    case '/': return 2;
    }
    return 0;
}


int Compare(int a, int b)
{
	if (a == b) 
		return 0;
	else
		return (a > b) ? 1 : -1;
}

int Compare(const FWOperator& left, const FWOperator& right)
{
	return Compare(left.GetPRI(), right.GetPRI());
}


bool operator > (const FWOperator& left, const FWOperator& right)
{
	return Compare(left, right) > 0;
}

bool operator < (const FWOperator& left, const FWOperator& right)
{
	return Compare(left, right) < 0;
}

bool operator == (const FWOperator& left, const FWOperator& right)
{
	return Compare(left, right) == 0;
}
