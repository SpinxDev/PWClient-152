/********************************************************************
	created:	2013/1/10
	author:		liudong
	
	purpose:    common code about client memory log
	Copyright (C) All Rights Reserved
*********************************************************************/
#include "ClientMemoryLog.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void client_memory_operate_info::clear()
{ 
	memset(this, 0, sizeof(*this)); 
}

void client_memory_operate_info::clear_call_stack()
{
	memset(call_stack, 0, sizeof(call_stack)); 
}

bool client_memory_operate_info::init(const char* buffer, const bool with_detail)
{
	clear();
	if (!buffer)
	{
		assert(0);
		return false;
	}

	unsigned __int64 temp_index = 0;
	char operate_type = 0;
	sscanf(buffer, "%I64d %c", &temp_index, &operate_type);
	switch (operate_type)
	{
	case 'A': type = TYPE_ALLOC;			break;
	case 'F': type = TYPE_FREE;				break;
	case 'D': type = TYPE_DELETE_BY_REALLOC;break;
	case 'R': type = TYPE_REALLOC;			break;
	default: assert(0); clear(); return false;
	}

	if (with_detail || TYPE_ALLOC==type || TYPE_REALLOC==type)
	{
		sscanf(buffer, "%I64d %c 0x%08X 0x%08X 0x%08X %11d %11d 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X", 
			&log_index,
			&operate_type, 
			&memory_address, 
			&hHeap, 
			&dwFlags,
			&time, 
			&memory_size, 
			&call_stack[0], 
			&call_stack[1], 
			&call_stack[2], 
			&call_stack[3],	
			&call_stack[4], 
			&call_stack[5], 
			&call_stack[6], 
			&call_stack[7],
			&call_stack[8], 
			&call_stack[9], 
			&call_stack[10], 
			&call_stack[11], 
			&call_stack[12], 
			&call_stack[13], 
			&call_stack[14], 
			&call_stack[15],
			&call_stack[16], 
			&call_stack[17], 
			&call_stack[18], 
			&call_stack[19],	
			&call_stack[20], 
			&call_stack[21], 
			&call_stack[22], 
			&call_stack[23],
			&call_stack[24], 
			&call_stack[25], 
			&call_stack[26], 
			&call_stack[27],	
			&call_stack[28], 
			&call_stack[29], 
			&call_stack[30], 
			&call_stack[31]);
		return true;
	}else if(TYPE_FREE==type || TYPE_DELETE_BY_REALLOC==type)
	{
		sscanf(buffer, "%I64d %c 0x%08X 0x%08X 0x%08X %11d %11d", 
			&log_index,
			&operate_type, 
			&memory_address, 
			&hHeap, 
			&dwFlags,
			&time,
			&memory_size);
		return true;
	}

	assert(0);
	clear();
	return false;
}

bool client_memory_operate_info::to_string(char* buffer, const bool with_detail) const
{
	if (!buffer)
	{
		assert(0);
		return false;
	}
	buffer[0] = 0;

	if (with_detail || TYPE_ALLOC==type || TYPE_REALLOC==type)
	{
		char operate_type = 0;
		if (TYPE_ALLOC==type)
		{
			operate_type = 'A';
		}else if (TYPE_FREE==type)
		{
			operate_type = 'F';
		}else if (TYPE_DELETE_BY_REALLOC==type)
		{
			operate_type = 'D';
		}else if (TYPE_REALLOC==type)
		{
			operate_type = 'R';
		}else
		{
			assert(0);
			return false;
		}
		sprintf(buffer, "%-21I64d %c 0x%08X 0x%08X 0x%08X %-11d %-11d 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X\n", 
			log_index,
			operate_type, 
			memory_address, 
			hHeap, 
			dwFlags,
			time, 
			memory_size, 
			call_stack[0], 
			call_stack[1], 
			call_stack[2], 
			call_stack[3],	
			call_stack[4], 
			call_stack[5], 
			call_stack[6], 
			call_stack[7],
			call_stack[8], 
			call_stack[9], 
			call_stack[10], 
			call_stack[11], 
			call_stack[12], 
			call_stack[13], 
			call_stack[14], 
			call_stack[15],
			call_stack[16], 
			call_stack[17], 
			call_stack[18], 
			call_stack[19],	
			call_stack[20], 
			call_stack[21], 
			call_stack[22], 
			call_stack[23],
			call_stack[24], 
			call_stack[25], 
			call_stack[26], 
			call_stack[27],	
			call_stack[28], 
			call_stack[29], 
			call_stack[30], 
			call_stack[31]);
		return true;
	}
	if (TYPE_FREE==type || TYPE_DELETE_BY_REALLOC==type)
	{
		const char operate_type = TYPE_FREE==type?'F':'D';
		sprintf(buffer, "%-21I64d %c 0x%08X 0x%08X 0x%08X %-11d %-11d\n", 
			log_index,
			operate_type, 
			memory_address, 
			hHeap, 
			dwFlags,
			time,
			memory_size);	
		return true;
	}
	assert(0);
	return false;
}

void angelica_memory_operate_info::clear()
{ 
	memset(this, 0, sizeof(*this)); 
}

bool angelica_memory_operate_info::init(const char* buffer)
{
	clear();
	if (!buffer)
	{
		assert(0);
		return false;
	}
	char operate_type = 0;
	sscanf(buffer, "%c %d %d 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X", 
		&operate_type, 
		&time, 
		&memory_size,
		&call_stack[0], 
		&call_stack[1], 
		&call_stack[2], 
		&call_stack[3],
		&call_stack[4], 
		&call_stack[5], 
		&call_stack[6], 
		&call_stack[7] );
	if ('A'==operate_type)
	{
		is_new = true;
		return true;
	}
	if('D'==operate_type)
	{
		is_new = false;
		return true;
	}
	assert(0);
	clear();
	return false;
}

bool angelica_memory_operate_info::to_string(char* buffer) const
{
	if (!buffer)
	{
		assert(0);
		return false;
	}
	buffer[0] = 0;

	char operate_type = is_new?'A':'D';
	sprintf(buffer, "%c %d %d 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X 0x%08X\n", 
		operate_type, 
		time, 
		memory_size,
		call_stack[0], 
		call_stack[1], 
		call_stack[2], 
		call_stack[3],
		call_stack[4], 
		call_stack[5], 
		call_stack[6], 
		call_stack[7]);
	return true;
}
