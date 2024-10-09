/********************************************************************
	created:	2005/08/30
	created:	30:8:2005   17:46
	file name:	ECScript.h
	author:		yaojun
	
	purpose:	CECScript represent a executable script object, created
				from a script file.
				CECScriptFile is a helper to read script file.
*********************************************************************/

#pragma once

#include <AString.h>
#include <vector.h>

class AFileImage;
class CECScriptUnit;
class CECScriptUnitCommonBlock;
class CECScriptUnitIfBlock;
class CECScriptUnitIfCondition;
class CECScriptUnitFunction;
class CECScriptUnitReturn;
class CECScriptUnitWait;
class CECScriptUnitFinish;
class CECScriptBoolExp;
class CECScriptUnitExecutor;
class CECScriptUnitStart;
template <class T, class ARG_T> class AStack;

//////////////////////////////////////////////////////////////////////////
// CECScriptFile
//////////////////////////////////////////////////////////////////////////

// returned "bool" value indicate whether the 
// physical read is succeed
// if the grammar is invalid a ASSERT will be triggered
class CECScriptFile
{
private:
	AFileImage * m_pFile;
	AString m_strToken;
	char m_char;
private:
	bool PeekChar(char &c);
	bool ReadChar();
	char GetChar() { return m_char; }

	bool ReadSkipSpace();
	bool ReadSkipLine();
	bool ReadTokenAsIdentifier();
	bool ReadTokenAsNumber();
	bool ReadTokenAsString();

	bool IsAlpha(char c);
	bool IsNumeric(char c);
	bool IsSign(char c);

	bool MoveFilePointBack();
	bool MoveFilePointForward();
public:
	CECScriptFile();
	~CECScriptFile();

	bool Open(AString strFileName);
	void Close();
	bool IsEOF();
	bool ReadNextToken();
	const AString & GetToken() { return m_strToken; }
	bool PeekNextToken(AString &token);
	typedef abase::vector<AString> StringParamArray;
	bool ReadParam(StringParamArray & params);
};


//////////////////////////////////////////////////////////////////////////
// CECScriptConditionParser
//////////////////////////////////////////////////////////////////////////

class CECScriptParser;

class CECScriptConditionParser
{
private:
	class CElement
	{
	public:
		enum
		{
			TYPE_OPERAND,
			TYPE_AND,
			TYPE_OR,
			TYPE_NOT
		};
		virtual int GetType() = 0;
	};
	class COperand : public CElement
	{
		CECScriptBoolExp * m_pExp;
	public:
		COperand(CECScriptBoolExp *pExp) : m_pExp(pExp) {}
		CECScriptBoolExp * GetExp() { return m_pExp; }
		virtual int GetType() { return TYPE_OPERAND; }
	};
	class COperator : public CElement
	{
	public:
		virtual int GetPriority() = 0;
		static COperator * ContructOperator(AString str);
	};
	class COperatorOr : public COperator
	{
	public:
		COperand * Calculate(COperand *p1, COperand *p2);
		virtual int GetPriority() { return 0;}
		virtual int GetType() { return TYPE_OR; }
	};
	class COperatorAnd : public COperator
	{
	public:
		COperand * Calculate(COperand *p1, COperand *p2);
		virtual int GetPriority() { return 1;}
		virtual int GetType() { return TYPE_AND; }
	};
	class COperatorNot : public COperator
	{
	public:
		COperand * Calculate(COperand *p);
		virtual int GetPriority() { return 2;}
		virtual int GetType() { return TYPE_NOT; }
	};
	typedef AStack<CElement *, CElement *> CElementStack;

	// make the following classes friends or they can not
	// access each other when their codes are in .cpp
	friend class CECScriptConditionParser::COperatorOr;
	friend class CECScriptConditionParser::COperatorAnd;
	friend class CECScriptConditionParser::COperatorNot;
private:
	CECScriptFile * m_pFile;
	CECScriptParser *m_pScriptParser;
private:
	void ReadNextToken();
	const AString & GetToken();
	COperand * ParseInBracket();
	COperand * ComputePostfixStack(CElementStack & stackPostfix);
public:
	CECScriptConditionParser(CECScriptParser *pScriptParser);
	~CECScriptConditionParser();
	CECScriptBoolExp * Parse();
};


//////////////////////////////////////////////////////////////////////////
// CECScriptParser
//////////////////////////////////////////////////////////////////////////
class CECScript;

class CECScriptParser
{
private:
	CECScriptFile * m_pFile;
	struct PrecompileVariable
	{
		AString identifier;
		AString value;
	};
	typedef abase::vector<PrecompileVariable> CPVArray;
	typedef CPVArray::iterator CPVArrayIterator;
	CPVArray m_vecPVs;

private:
	AString GetToken();
	void ReadNextToken();
	PrecompileVariable * FindPV(AString strVariableName);


	CECScriptBoolExp * ParseCondition();
	CECScriptUnitCommonBlock * ParseCommonBlock();
	CECScriptUnitIfCondition * ParseIfCondition();
	CECScriptUnitIfBlock * ParseIfBlock();
	CECScriptUnitFunction * ParseExecBlock();
	CECScriptUnitWait * ParseWaitBlock();
	CECScriptUnitReturn * ParseReturnBlock();
	CECScriptUnitFinish * ParseFinishBlock();
	CECScriptUnitStart * ParseStartBlock();
	void ParsePrecompileSymbol(CECScript *pScript);
public:	
	CECScriptParser(CECScriptFile *pFile) : m_pFile(pFile) {}
	CECScriptFile * GetFile() { return m_pFile; }
	void ParseTo(CECScript *pScript);
	void ParseParam(CECScriptFile::StringParamArray &paramArray);
};

//////////////////////////////////////////////////////////////////////////
// CECScript
//////////////////////////////////////////////////////////////////////////

class CECScript
{
	friend CECScriptParser;
private:
	CECScriptUnit * m_pScriptUnitTree;
	CECScriptUnitExecutor * m_pExecutor;
	int m_nID;
	int m_nType;
	bool m_bForcePop;
	AString m_strName;
	AString m_strSteps;  // not used any more
public:
	CECScript();
	virtual ~CECScript();
	bool LoadFromFile(AString strFileName, CECScriptUnitExecutor *pExecutor);
	void Run();
	
	int GetID() { return m_nID; }
	int GetType() { return m_nType; }
	bool IsForcePop() { return m_bForcePop; }
	AString GetName() { return m_strName; }
	AString GetSteps() { return m_strSteps; }

	void SetForcePop(bool bForcePop) { m_bForcePop = bForcePop; }
	CECScriptUnitExecutor * GetExecutor() { return m_pExecutor; }
};


