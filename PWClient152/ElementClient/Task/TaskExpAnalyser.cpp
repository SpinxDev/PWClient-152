#include "TaskExpAnalyser.h"
#include <ctype.h>
#include "hashmap.h"

TaskExpAnalyser::TaskExpAnalyser()
{
}

TaskExpAnalyser::~TaskExpAnalyser()
{
}

void TaskExpAnalyser::KickBlank(char *szSen) 
{ 
	char *copied; 
	for( copied = szSen; *szSen; szSen++ ) 
	{ 
		if ( *szSen != ' ' ) 
			*copied++ = *szSen; 
	} 
	
	*copied = 0x00; 	
	return; 
}

bool TaskExpAnalyser::Analyse(const char* szSentence, TaskExpressionArr& aExp)
{
	pos = 0;
	sentence = szSentence;
	exp_arr = &aExp;
	return add();
}

bool TaskExpAnalyser::add()
{
	if (!multiply())
		return false;

	while (sentence[pos] == '+' || sentence[pos] == '-')
	{
		TASK_EXPRESSION exp;
		exp.type = enumTaskExpOprt;
		exp.value = sentence[pos++];

		if (!multiply())
			return false;

		exp_arr->push_back(exp);
	}
 
	return true;
}

bool TaskExpAnalyser::multiply()
{
	if (!bracket())
		return false;

	while (sentence[pos] == '*' || sentence[pos] == '/')
	{
		TASK_EXPRESSION exp;
		exp.type = enumTaskExpOprt;
		exp.value = sentence[pos++];

		if (!bracket())
			return false;

		exp_arr->push_back(exp);
	}

	return true;
}

bool TaskExpAnalyser::bracket()
{
	if (sentence[pos] == '(')
	{
		pos++;
	
		if (!add())
			return false;
	
		if (sentence[pos++] != ')')
			return false;

		return true;
	}

	bool is_var = false;

	if (sentence[pos] == '$')
	{
		pos++;
		is_var = true;
	}

	if (!isdigit(sentence[pos]))
		return false;

	TASK_EXPRESSION exp;
	exp.type = is_var ? enumTaskExpVar : enumTaskExpConst;
	exp.value = (float)atof(sentence+pos);
	exp_arr->push_back(exp);

	while (isdigit(sentence[pos]) || sentence[pos] == '.')
		pos++;
 
	return true;
}

float TaskExpAnalyser::Run(TaskInterface* pTask, const TaskExpressionArr& aExp)
{
	float ret = 0.0f;
	abase::vector<float> stack;
	stack.reserve(256);
	abase::hash_map<int, float> dict;

	for (size_t i = 0; i < aExp.size(); i++)
	{
		const TASK_EXPRESSION& exp = aExp[i];

		if (exp.type == enumTaskExpOprt)
		{
			if (stack.size() < 2)
				throw "wrong operant num";

			float op2 = stack.back();
			stack.pop_back();
			float op1 = stack.back();
			stack.pop_back();

			if (exp.value == '+')
				ret = op1 + op2;
			else if (exp.value == '-')
				ret = op1 - op2;
			else if (exp.value == '*')
				ret = op1 * op2;
			else if (exp.value == '/')
			{
				if (op2 == 0)
					throw "divide by 0";

				ret = op1 / op2;
			}

			stack.push_back(ret);
		}
		else if (exp.type == enumTaskExpConst)
		{
			ret = exp.value;
			stack.push_back(ret);
		}
		else if (exp.type == enumTaskExpVar)
		{
			abase::hash_map<int, float>::iterator it = dict.find((int)exp.value);

			if (it != dict.end())
			{
				ret = it->second;
				stack.push_back(ret);
			}
			else
			{
				ret = (float)pTask->GetGlobalValue((long)exp.value);
				stack.push_back(ret);
				dict[(int)exp.value] = ret;
			}
		}
	}

	return ret;
}
