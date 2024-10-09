#include "ECScript.h"
#include "ECScriptUnit.h"
#include "ECScriptUnitExecutor.h"
#include "CodeTemplate.h"
#include "ECScriptBoolExp.h"
#include "ECScriptFunctionBase.h"
#include "ECScriptCheckBase.h"
#include <AFileImage.h>
#include <AStack.h>
#include "EC_Global.h"



#define TOKEN_IF					"if"                                       
#define TOKEN_ELSE					"else"                                     
#define TOKEN_ELSEIF				"elseif"                                   
#define TOKEN_ENDIF					"endif"                                    
#define TOKEN_EXEC					"exec"                                     
#define TOKEN_WAIT					"wait"                                     
#define TOKEN_RETURN				"return"                                   
#define TOKEN_START					"start"
#define TOKEN_FINISH				"finish"
#define TOKEN_LEFT_BRACKET			'('                                       
#define TOKEN_RIGHT_BRACKET			')'                                       
#define TOKEN_COMMA					','                                       
#define TOKEN_AND					'&'                                       
#define TOKEN_OR					'|'                                       
#define TOKEN_NOT					'!'                                       
#define TOKEN_TRUE					"true"                                     
#define TOKEN_FALSE					"false"                                    
#define TOKEN_PRECOMPILE_SYMBOL		'#'                                       
#define TOKEN_PRECOMPILE_ID			"id"                                       
#define TOKEN_PRECOMPILE_TYPE		"type"                                       
#define TOKEN_PRECOMPILE_FORCE_POP	"force_pop"
#define TOKEN_PRECOMPILE_NAME		"name"
#define TOKEN_PRECOMPILE_DEFINE		"define"
#define TOKEN_PRECOMPILE_STEPS		"steps"


#define SYMBOL_SPACE			' '
#define SYMBOL_TAB				'\t'
#define SYMBOL_QUOTATION		'\"'
#define SYMBOL_NEW_LINE			'\n'
#define SYMBOL_RETURN			'\r'
#define SYMBOL_CHAR_NULL		'\0'				
#define SYMBOL_EOF				((char)0)
#define SYMBOL_SLASH			'/'
#define SYMBOL_BACKSLASH		'\\'
#define SYMBOL_CHAR_R			'r'

//////////////////////////////////////////////////////////////////////////
// CECScriptFile
//////////////////////////////////////////////////////////////////////////

CECScriptFile::CECScriptFile() :
	m_pFile(NULL),
	m_char(SYMBOL_SPACE),
	m_strToken("")
{
}

CECScriptFile::~CECScriptFile()
{
	Close();
}

bool CECScriptFile::Open(AString strFileName)
{
	Close();

	m_pFile = new AFileImage;
	bool bopen = m_pFile->Open(strFileName, AFILE_OPENEXIST | AFILE_TEXT | AFILE_NOHEAD); 
	// 添加对utf-8文件的支持。并保持对以前无格式头文件格式的支持。
	// 这里需要做的就是忽略utf-8文件的格式头：EF BB BF，但是如果不是EF开头，则不需忽略格式头
	// 2012/8/13 zhougaomin01305
	char c = 0;
	if(PeekChar(c))
	{
		unsigned char uc = (unsigned char)c;
		// 确实是utf-8格式则直接往前移动三个字节。
		// 此处不再检测EF后头格式是否正确（因为如果不正确，则后头无论如何都会解析错误，则无论此处任何处理都无效了，因此没必要检测）。
		if(uc == 0xEF)
		{
			MoveFilePointForward();
			MoveFilePointForward();
			MoveFilePointForward();
		}
	}
	return bopen;
}

void CECScriptFile::Close()
{
	SAFE_DELETE(m_pFile);
}

bool CECScriptFile::IsEOF()
{
	return m_pFile->GetFileLength() == m_pFile->GetPos();
}

bool CECScriptFile::ReadChar()
{
	if (IsEOF())
	{
		m_char = SYMBOL_EOF;
		return true;
	}
	else
	{
		DWORD dwReadLength;
		return m_pFile->Read(&m_char, 1, &dwReadLength);
	}
}

bool CECScriptFile::PeekChar(char &c)
{
	if (IsEOF())
	{
		c = SYMBOL_EOF;
		return true;
	}
	else
	{
		DWORD dwReadLength;
		if (!m_pFile->Read(&c, 1, &dwReadLength)) return false;
		if (!MoveFilePointBack()) return false;
		return true;
	}
}


bool CECScriptFile::ReadSkipSpace()
{
	char c;	
	if (!PeekChar(c)) return false;
	while (c == SYMBOL_SPACE ||
		c == SYMBOL_TAB ||
		c == SYMBOL_NEW_LINE ||
		c == SYMBOL_RETURN ||
		c == SYMBOL_SLASH)
	{
		if (c == SYMBOL_SLASH)
		{
			if (!ReadChar()) return false;
			ASSERT(GetChar() == SYMBOL_SLASH);
			if (!ReadChar()) return false;
			ASSERT(GetChar() == SYMBOL_SLASH);
			if (!ReadSkipLine()) return false;
		}
		else
		{
			if (!ReadChar()) return false;
		}
		if (!PeekChar(c)) return false;
	}
	return true;
}

bool CECScriptFile::ReadSkipLine()
{
	while (!IsEOF())
	{
		if (!ReadChar()) return false;
		if (GetChar() == SYMBOL_NEW_LINE || GetChar() == SYMBOL_RETURN)
			break;
	}
	return true;
}


bool CECScriptFile::IsAlpha(char c)
{
	return c >= 'a' && c <= 'z' ||
		c >= 'A' && c <= 'Z';
}

bool CECScriptFile::IsNumeric(char c)
{
	return c >= '0' && c <= '9' || c == '.';
}

bool CECScriptFile::IsSign(char c)
{
	return c == '+' || c == '-';
}

bool CECScriptFile::ReadTokenAsString()
{
	if (!ReadChar()) return false;
	ASSERT(GetChar() == SYMBOL_QUOTATION);

	m_strToken = "";
	if (!ReadChar()) return false;
	while (GetChar() != SYMBOL_QUOTATION &&
		GetChar() != SYMBOL_NEW_LINE &&
		GetChar() != SYMBOL_RETURN &&
		GetChar() != SYMBOL_EOF)
	{
		if (GetChar() == SYMBOL_BACKSLASH)
		{
			if (!ReadChar()) return false;
			if (GetChar() == SYMBOL_BACKSLASH)
				m_strToken += GetChar();
			else if (GetChar() == SYMBOL_QUOTATION)
				m_strToken += GetChar();
			else if (GetChar() == SYMBOL_CHAR_R)
				m_strToken += "\r";
			else if (GetChar() == SYMBOL_CHAR_NULL)
				m_strToken += "\0";
			else
			{
				m_strToken += GetChar();
//				ASSERT(!"invalid ESC");
			}
		}
		else
		{
			m_strToken += GetChar();
		}
		if (!ReadChar()) return false;
	}
	ASSERT(GetChar() == SYMBOL_QUOTATION);
	return true;
}

bool CECScriptFile::ReadTokenAsIdentifier()
{
	m_strToken = "";
	char c;
	if (!PeekChar(c)) return false;
	while (
		c != SYMBOL_SPACE &&
		c != SYMBOL_TAB &&
		c != SYMBOL_NEW_LINE &&
		c != SYMBOL_RETURN &&
		c != TOKEN_COMMA &&
		c != TOKEN_LEFT_BRACKET &&
		c != TOKEN_RIGHT_BRACKET &&
		c != TOKEN_AND &&
		c != TOKEN_OR &&
		c != TOKEN_NOT &&
		c != SYMBOL_EOF)
	{
		m_strToken += c;
		if (!ReadChar()) return false;
		if (!PeekChar(c)) return false;
	}
	return true;	
}

bool CECScriptFile::ReadTokenAsNumber()
{
	return ReadTokenAsIdentifier();
}

bool CECScriptFile::MoveFilePointBack()
{
	return m_pFile->Seek(-1, AFILE_SEEK_CUR);
}

bool CECScriptFile::MoveFilePointForward()
{
	return m_pFile->Seek(1, AFILE_SEEK_CUR);
}

bool CECScriptFile::ReadNextToken()
{
	BEGIN_FAKE_WHILE;

	CHECK_BREAK(ReadSkipSpace());

	char c;
	if (!PeekChar(c)) return false;

	if (c == SYMBOL_EOF)
	{
		return false;
	}
	if (IsAlpha(c)) 
	{
		CHECK_BREAK(ReadTokenAsIdentifier());
	}
	else if (c == SYMBOL_QUOTATION)
	{
		CHECK_BREAK(ReadTokenAsString());
	}
	else if (IsNumeric(c) || IsSign(c))
	{
		CHECK_BREAK(ReadTokenAsNumber());
	}
	else if (c == TOKEN_LEFT_BRACKET ||
		c == TOKEN_RIGHT_BRACKET ||
		c == TOKEN_COMMA ||
		c == TOKEN_AND ||
		c == TOKEN_OR ||
		c == TOKEN_NOT ||
		c == TOKEN_PRECOMPILE_SYMBOL)
	{
		m_strToken = c;
		if (!MoveFilePointForward()) return false;
	}
	else
	{
		ASSERT(!"invalid symbol");
	}

	CHECK_BREAK(ReadSkipSpace());

	END_FAKE_WHILE;
	
	RETURN_FAKE_WHILE_RESULT;
}

bool CECScriptFile::PeekNextToken(AString &token)
{
	DWORD dwOldPos = m_pFile->GetPos();
	AString strOldToken = m_strToken;
	
	bool bReadSucceed = ReadNextToken();
	if (bReadSucceed)
	{
		token = m_strToken;
	}
	m_strToken = strOldToken;
	m_pFile->Seek(dwOldPos, AFILE_SEEK_SET);

	return bReadSucceed;
}


bool CECScriptFile::ReadParam(StringParamArray & params)
{
	BEGIN_FAKE_WHILE;
	
	CHECK_BREAK(ReadNextToken());
	ASSERT(GetToken() == TOKEN_LEFT_BRACKET);
	
	if (!ReadNextToken()) return false;
	while (!IsEOF() && GetToken() != TOKEN_RIGHT_BRACKET)
	{
		params.push_back(GetToken());
		
		if (!ReadNextToken()) return false;
		
		if (GetToken() == TOKEN_RIGHT_BRACKET)	break;
		ASSERT(GetToken() == TOKEN_COMMA);
		
		if (!ReadNextToken()) return false;
	}
	ASSERT(GetToken() == TOKEN_RIGHT_BRACKET);
	
	END_FAKE_WHILE;
	
	RETURN_FAKE_WHILE_RESULT;
}

//////////////////////////////////////////////////////////////////////////
// CECScriptConditionParser
//////////////////////////////////////////////////////////////////////////


CECScriptConditionParser::COperand * CECScriptConditionParser::COperatorAnd::Calculate(COperand *p1, COperand *p2)
{
	ASSERT(p1); ASSERT(p2);
	CECScriptBoolExpAnd *pExp = new CECScriptBoolExpAnd(
		p1->GetExp(), p2->GetExp());
	return new COperand(pExp);
}

CECScriptConditionParser::COperand * CECScriptConditionParser::COperatorOr::Calculate(COperand *p1, COperand *p2)
{
	ASSERT(p1); ASSERT(p2);
	CECScriptBoolExpOr *pExp = new CECScriptBoolExpOr(
		p1->GetExp(), p2->GetExp());
	return new COperand(pExp);
}

CECScriptConditionParser::COperand * CECScriptConditionParser::COperatorNot::Calculate(COperand *p)
{
	ASSERT(p);
	CECScriptBoolExpNot *pExp = new CECScriptBoolExpNot(
		p->GetExp());
	return new COperand(pExp);
}

CECScriptConditionParser::COperator * CECScriptConditionParser::COperator::ContructOperator(AString str)
{
	if (str == TOKEN_NOT)
		return new COperatorNot;
	else if (str == TOKEN_OR)
		return new COperatorOr;
	else
	{
		ASSERT(str == TOKEN_AND);
		return new COperatorAnd;
	}
}


CECScriptConditionParser::CECScriptConditionParser(CECScriptParser *pScriptParser) :
	m_pScriptParser(pScriptParser),
	m_pFile(pScriptParser->GetFile())
{
	ASSERT(m_pScriptParser);
	ASSERT(m_pFile);
}

CECScriptConditionParser::~CECScriptConditionParser()
{
}

void CECScriptConditionParser::ReadNextToken()
{
	bool bSucceed = m_pFile->ReadNextToken();
	ASSERT(bSucceed && "read file error");
}

const AString & CECScriptConditionParser::GetToken()
{
	return m_pFile->GetToken();
}


CECScriptBoolExp * CECScriptConditionParser::Parse()
{
	ReadNextToken();
	ASSERT(GetToken() == TOKEN_LEFT_BRACKET);
	
	COperand *pOperand = ParseInBracket();
	CECScriptBoolExp *pExp = pOperand->GetExp();
	SAFE_DELETE(pOperand);

	ASSERT(pExp);
	return pExp;
}

CECScriptConditionParser::COperand * CECScriptConditionParser::ComputePostfixStack(CElementStack & stackPostfix)
{
	// reverse postfix stack
	CElementStack stackPostfixReverse;
	while (stackPostfix.GetElementNum() > 0)
	{
		stackPostfixReverse.Push(stackPostfix.Pop());
	}

	CElementStack stackResult;

	// compute
	while (stackPostfixReverse.GetElementNum() > 0)
	{
		CElement *pEle = stackPostfixReverse.Pop();
		switch(pEle->GetType()) 
		{
		case CElement::TYPE_OPERAND:
			stackResult.Push(pEle);
			break;
		case CElement::TYPE_NOT:
			{
				ASSERT(stackResult.GetElementNum() >= 1);
				COperand *pOperand = 
					dynamic_cast<COperand *>(stackResult.Pop());
				COperatorNot *pOperator =
					dynamic_cast<COperatorNot *>(pEle);
				stackResult.Push(pOperator->Calculate(pOperand));
				delete pOperand;
				delete pOperator;
			}
			break;
		case CElement::TYPE_AND:
			{
				ASSERT(stackResult.GetElementNum() >= 2);
				COperand *p1 = 
					dynamic_cast<COperand *>(stackResult.Pop());
				COperand *p2 = 
					dynamic_cast<COperand *>(stackResult.Pop());
				COperatorAnd *pOperator = 
					dynamic_cast<COperatorAnd *>(pEle);
				stackResult.Push(pOperator->Calculate(p1, p2));
				delete p1;
				delete p2;
				delete pOperator;
			}
			break;
		case CElement::TYPE_OR:
			{
				ASSERT(stackResult.GetElementNum() >= 2);
				COperand *p1 = 
					dynamic_cast<COperand *>(stackResult.Pop());
				COperand *p2 = 
					dynamic_cast<COperand *>(stackResult.Pop());
				COperatorOr *pOperator = 
					dynamic_cast<COperatorOr *>(pEle);
				stackResult.Push(pOperator->Calculate(p1, p2));
				delete p1;
				delete p2;
				delete pOperator;
			}
			break;
		default:
			ASSERT(!"invalud element in postfix stack for boolean expression");
		}
	}
	ASSERT(stackResult.GetElementNum() == 1);

	return dynamic_cast<COperand *>(stackResult.Pop());
}

CECScriptConditionParser::COperand * CECScriptConditionParser::ParseInBracket()
{
	// 第一步：将括号中的序列转换为一个后缀表达式
	// 第二步：计算后缀表达式，返回一个COperand
	// 
	// 转换为后缀表达式的算法：
	// 1)检查输入的下一元素。
	// 2)假如是一个开括号，则递归调用本函数形成一个操作数，加入
	//   后缀表达式栈，重复步骤1)
	// 3)假如是个操作数，加入后缀表达式栈。
	// 4)假如是个运算符，则
	//   i) 假如临时栈为空，将此运算符压入临时栈。
	//   iii) 假如此运算符比临时栈顶运算符优先级高，将此运算符压入临时栈中。
	//   iv) 否则临时栈顶运算符出栈并加入后缀表达式栈，重复步骤4。
	// 5)假如是个闭括号，临时栈中剩余运算符逐个出栈并加入后缀表达式栈，结束。

	CElementStack stackTemp;
	CElementStack stackPostfix;

	while (!m_pFile->IsEOF())
	{
		ReadNextToken();
		if (GetToken() == TOKEN_RIGHT_BRACKET)
		{
			while (stackTemp.GetElementNum() > 0)
			{
				stackPostfix.Push(stackTemp.Pop());
			}

			// now we have constructed a postfix stack, compute it
			return ComputePostfixStack(stackPostfix);
		}
		else if (
			GetToken() == TOKEN_AND ||
			GetToken() == TOKEN_OR ||
			GetToken() == TOKEN_NOT)
		{
			COperator *pOPT = COperator::ContructOperator(GetToken());
			while (true)
			{
				if (stackTemp.GetElementNum() == 0)
				{
					stackTemp.Push(pOPT);
					break;
				}
				
				COperator *pOPTInStackTop = 
					dynamic_cast<COperator *>(stackTemp.Peek());
				if (pOPT->GetPriority() > pOPTInStackTop->GetPriority())
				{
					stackTemp.Push(pOPT);
					break;
				}
				
				pOPTInStackTop = dynamic_cast<COperator *>(stackTemp.Pop());
				stackPostfix.Push(pOPTInStackTop);
			}
		}
		else // operand or left bracket (which will form a oprand)
		{
			if (GetToken() == TOKEN_LEFT_BRACKET)
			{
				stackPostfix.Push(ParseInBracket());
			}
			else if (GetToken() == TOKEN_TRUE)
			{
				stackPostfix.Push(new COperand(new CECScriptBoolExpConstant(true)));
			}
			else if (GetToken() == TOKEN_FALSE)
			{
				stackPostfix.Push(new COperand(new CECScriptBoolExpConstant(false)));
			}
			else // checker
			{
				CECScriptCheckBase *pChecker = 
					TheECSciptCheckFactory::Instance()->CreateObject(
						GetToken());

				CECScriptFile::StringParamArray params;
				bool bOK = m_pFile->ReadParam(params);
				ASSERT(bOK);
				m_pScriptParser->ParseParam(params);
				
				pChecker->PreparePushParamValue();
				CECScriptFile::StringParamArray::iterator iter = params.begin();
				for (; iter != params.end(); ++iter)
				{
					pChecker->PushParamValue(*iter);
				}
				stackPostfix.Push(new COperand(pChecker));
			}
		}
			
	}

	ASSERT(!"unclosed bracket in a boolean expression");
	return NULL;
}







//////////////////////////////////////////////////////////////////////////
// CECScriptParser
//////////////////////////////////////////////////////////////////////////

CECScriptParser::PrecompileVariable * CECScriptParser::FindPV(AString strVariableName)
{
	CPVArrayIterator iter = m_vecPVs.begin();
	for (; iter != m_vecPVs.end(); ++iter)
	{
		if (iter->identifier == strVariableName)
			return &(*iter);
	}
	return NULL;
}


AString CECScriptParser::GetToken()
{
	return m_pFile->GetToken();
}


void CECScriptParser::ParseTo(CECScript *pScript)
{
	ParsePrecompileSymbol(pScript);
	pScript->m_pScriptUnitTree = ParseCommonBlock();
}

void CECScriptParser::ParsePrecompileSymbol(CECScript *pScript)
{
	pScript->m_bForcePop = false;

	do
	{
		if (m_pFile->IsEOF())
			break;
		
		AString strPeek;
		bool bPeekOK = m_pFile->PeekNextToken(strPeek);
		ASSERT(bPeekOK);
		if (strPeek != TOKEN_PRECOMPILE_SYMBOL)
			break;

		ReadNextToken(); // read past TOKEN_PRECOMPILE_SYMBOL
		ReadNextToken(); // read precompile keyword
		if (GetToken() == TOKEN_PRECOMPILE_ID)
		{
			ReadNextToken();
			pScript->m_nID = GetToken().ToInt();
		}
		else if (GetToken() == TOKEN_PRECOMPILE_TYPE)
		{
			ReadNextToken();
			pScript->m_nType = GetToken().ToInt() - 1;
		}
		else if (GetToken() == TOKEN_PRECOMPILE_FORCE_POP)
		{
			pScript->m_bForcePop = true;
		}
		else if (GetToken() == TOKEN_PRECOMPILE_NAME)
		{
			ReadNextToken();
			pScript->m_strName = GetToken();
		}
		else if (GetToken() == TOKEN_PRECOMPILE_DEFINE)
		{
			PrecompileVariable pv;
			ReadNextToken();
			pv.identifier = GetToken();
			ReadNextToken();
			pv.value = GetToken();
			m_vecPVs.push_back(pv);
		}
		else if (GetToken() == TOKEN_PRECOMPILE_STEPS)
		{
			ReadNextToken();
			pScript->m_strSteps = GetToken();
		}
		else
		{
			ASSERT(!"invalid precompile keyword");
		}
	} while (true);
}

CECScriptUnitCommonBlock * CECScriptParser::ParseCommonBlock()
{
	CECScriptUnitCommonBlock *pBlock = new CECScriptUnitCommonBlock;

	while (!m_pFile->IsEOF())
	{
		AString strNextToken;
		bool bOK = m_pFile->PeekNextToken(strNextToken);
		ASSERT(bOK);
		if (strNextToken == TOKEN_ELSEIF ||
			strNextToken == TOKEN_ELSE ||
			strNextToken == TOKEN_ENDIF)
		{
			break;
		}

		ReadNextToken();

		CECScriptUnit * pUnit = NULL;
		if (GetToken() == TOKEN_IF)
		{
			pUnit = ParseIfBlock();
		}
		else if (GetToken() == TOKEN_EXEC)
		{
			pUnit = ParseExecBlock();
		}
		else if (GetToken() == TOKEN_WAIT)
		{
			pUnit = ParseWaitBlock();
		}
		else if (GetToken() == TOKEN_RETURN)
		{
			pUnit = ParseReturnBlock();
		}
		else if (GetToken() == TOKEN_FINISH)
		{
			pUnit = ParseFinishBlock();
		}
		else if (GetToken() == TOKEN_START)
		{
			pUnit = ParseStartBlock();
		}
		else
		{
			ASSERT(!"illegal keyword");
		}
		pBlock->PushBackChild(pUnit);
	}

	return pBlock;
}


CECScriptUnitIfCondition * CECScriptParser::ParseIfCondition()
{
	ASSERT(GetToken() == TOKEN_IF ||
		GetToken() == TOKEN_ELSEIF ||
		GetToken() == TOKEN_ELSE);
	
	CECScriptBoolExp *pBoolExp = NULL;
	if (GetToken() == TOKEN_ELSE)
	{
		// "else" equal to "elseif (true)"
		ASSERT(GetToken() == TOKEN_ELSE);
		pBoolExp = new CECScriptBoolExpConstant(true);
	}
	else 
	{
		pBoolExp = ParseCondition();
	}
	
	ASSERT(pBoolExp);
	return new CECScriptUnitIfCondition(pBoolExp);
}

CECScriptUnitIfBlock * CECScriptParser::ParseIfBlock()
{
	ASSERT(GetToken() == TOKEN_IF);

	// parse each branch
	CECScriptUnitIfBlock *pIfBlock = new CECScriptUnitIfBlock;
	
	while (GetToken() != TOKEN_ENDIF)
	{
		ASSERT(GetToken() == TOKEN_IF ||
			GetToken() == TOKEN_ELSEIF ||
			GetToken() == TOKEN_ELSE);

		CECScriptUnitIfCondition *pCondition = ParseIfCondition();
		CECScriptUnitCommonBlock *pBlock = ParseCommonBlock();

		pIfBlock->PushBackBranch(pCondition, pBlock);
		ReadNextToken();
	}
	
	return pIfBlock;
}

CECScriptBoolExp * CECScriptParser::ParseCondition()
{
	ASSERT(GetToken() == TOKEN_WAIT ||
		GetToken() == TOKEN_IF ||
		GetToken() == TOKEN_ELSEIF);

	CECScriptConditionParser parser(this);
	return parser.Parse();
}


CECScriptUnitFunction * CECScriptParser::ParseExecBlock()
{
	ASSERT(GetToken() == TOKEN_EXEC);

	// read function name and create it
	ReadNextToken();
	CECScriptFunctionBase *pFunction = 
		TheECScriptFunctionFacotry::Instance()->CreateObject(GetToken());

	// read params to a string list
	CECScriptFile::StringParamArray params;
	bool bOK = m_pFile->ReadParam(params);
	ASSERT(bOK);
	ParseParam(params);

	// push params to function
	pFunction->PreparePushParamValue();
	CECScriptFile::StringParamArray::iterator iter = params.begin();
	for (; iter != params.end(); ++iter)
	{
		pFunction->PushParamValue(*iter);
	}
	
	return new CECScriptUnitFunction(pFunction);
}

CECScriptUnitWait * CECScriptParser::ParseWaitBlock()
{
	// read condition
	CECScriptBoolExp *pCondition = ParseCondition();

	// return result
	return new CECScriptUnitWait(pCondition);
}

CECScriptUnitStart * CECScriptParser::ParseStartBlock()
{
	return new CECScriptUnitStart;
}

CECScriptUnitReturn * CECScriptParser::ParseReturnBlock()
{
	return new CECScriptUnitReturn;
}

CECScriptUnitFinish * CECScriptParser::ParseFinishBlock()
{
	return new CECScriptUnitFinish;
}

void CECScriptParser::ReadNextToken()
{
	bool bSucceed = m_pFile->ReadNextToken();
	ASSERT(bSucceed && "read file error");
}

void CECScriptParser::ParseParam(CECScriptFile::StringParamArray &paramArray)
{
	CECScriptFile::StringParamArray::iterator iter = paramArray.begin();
	for (; iter != paramArray.end(); ++iter)
	{
		PrecompileVariable *pPV = FindPV(*iter);
		if (pPV) 
			*iter = pPV->value;
	}
}

//////////////////////////////////////////////////////////////////////////
// CECScript
//////////////////////////////////////////////////////////////////////////

CECScript::CECScript() :
	m_pScriptUnitTree(NULL),
	m_pExecutor(NULL),
	m_nID(-1),
	m_nType(-1),
	m_bForcePop(false)
{
}

CECScript::~CECScript()
{
	delete m_pScriptUnitTree;
	delete m_pExecutor;
}

bool CECScript::LoadFromFile(AString strFileName, CECScriptUnitExecutor *pExecutor)
{
	CECScriptFile scriptFile;
	if (!scriptFile.Open(strFileName))
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECScript::LoadFromFile", __LINE__);
		return false;
	}

	CECScriptParser parser(&scriptFile);
	
	parser.ParseTo(this);

	m_pExecutor = pExecutor;
	m_pExecutor->SetRunningScript(this);

	return true;
}

void CECScript::Run()
{
	m_pExecutor->ChangeToNextState();
	m_pScriptUnitTree->Visit(m_pExecutor);
}
