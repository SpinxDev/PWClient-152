#ifndef _COMMANDLIST_H_
#define _COMMANDLIST_H_

#include "FWConfig.h"
#include <AList2.h>


class FWCommand;
class FWObject;


class FWCommandList : public APtrList<FWCommand *>
{
public:
	// Check whether the document has been
	// modified or not, according to current
	// command list. 
	// In order to use this function, you must
	// call OnSaveDocument when the document 
	// is saved.
	bool IsModified();

	// When the document is saved,
	// call this function to store the position
	// so that IsModified() can return 
	// the correct value
	void OnSaveDocument();

	bool CanRedo();
	bool CanUndo();

	// Remove all pointers and delete entities 
	// from heap
	void DeleteAll();

	// Remove selectd pointer and delete the 
	// entity from heap
	void DeleteCommand(ALISTPOSITION pos);

	// if there are some commands having been 
	// undone, use this 
	// function can re-execute it again
	// else this function does nothing
	void ReExecuteCommand();

	//Undo the last command
	//if all command has been undone or the 
	//command list is 
	//empty
	//this function does nothing.

	void UnExecuteCommand();

	//Execute a new command and append it to the 
	//list
	void ExecuteCommand(FWCommand* pCommand);

	FWCommandList();
	virtual ~FWCommandList();

	void UnExecuteAllCommand();

private:
	// The position of the most lately executed command
	// when all command has been undone or the command 
	// list is empty m_lastCmdPos == NULL
	ALISTPOSITION m_lastCmdPos;

	// The value of m_lastCmdPos when the
	// document is saved. This value is used by
	// SetSavePosition() and IsModified()
	ALISTPOSITION m_savePos;
};

#endif 
