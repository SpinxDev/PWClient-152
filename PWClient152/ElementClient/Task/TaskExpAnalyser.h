#ifndef _TASKEXPANALYSER_H_
#define _TASKEXPANALYSER_H_

#include "vector.h"
#include "TaskInterface.h"

#pragma pack(1)

enum
{
	enumTaskExpVar,
	enumTaskExpConst,
	enumTaskExpOprt,
};

struct TASK_EXPRESSION
{
	int type;
	float value;
	bool operator == (const TASK_EXPRESSION& src) const
	{
		return (type == src.type && value == src.value);
	}
};

#pragma pack()

typedef abase::vector<TASK_EXPRESSION> TaskExpressionArr;

class TaskExpAnalyser
{
public:

	TaskExpAnalyser();
	~TaskExpAnalyser();

protected:

	int pos;
	const char* sentence;
	TaskExpressionArr* exp_arr;

protected:

	bool add();
	bool multiply();
	bool bracket();

public:

	void KickBlank(char *szSen);
	bool Analyse(const char* szSentence, TaskExpressionArr& aExp);
	float Run(TaskInterface* pTask, const TaskExpressionArr& aExp);
};

#endif
