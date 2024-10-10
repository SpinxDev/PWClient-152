/********************************************************************
	created:	2013/1/10
	author:		liudong
	
	purpose:    common code about client memory log
	Copyright (C) All Rights Reserved
*********************************************************************/
#pragma once

struct client_memory_operate_info
{
	enum
	{
		ADDRESS_NUM = 32,
	};
	enum
	{
		TYPE_ERROR=0,
		TYPE_ALLOC,
		TYPE_FREE,
		TYPE_DELETE_BY_REALLOC,
		TYPE_REALLOC,
	};

	client_memory_operate_info(){ clear(); }
	bool init(const char* buffer, const bool with_detail=false);
	bool to_string(char* buffer, const bool with_detail=false) const;
	bool is_new() const { return TYPE_ALLOC==type || TYPE_REALLOC==type;}
	bool is_delete() const { return TYPE_FREE==type || TYPE_DELETE_BY_REALLOC==type;}
	void clear_call_stack();
	void clear();
	
	unsigned int	type;
	unsigned int    call_stack[ADDRESS_NUM];
	unsigned int	memory_address;
	unsigned int	memory_size;
	unsigned int	time;
	unsigned int	hHeap;
	unsigned int	dwFlags;
	unsigned __int64 log_index;
};

struct angelica_memory_operate_info
{
	enum
	{
		ADDRESS_NUM = 8,
	};	
	angelica_memory_operate_info(){ clear(); }
	bool init(const char* buffer);
	bool to_string(char* buffer) const;
	void clear();
	
	bool			is_new;
	unsigned int    call_stack[ADDRESS_NUM];
	unsigned int	memory_size;
	unsigned int	time;
};
