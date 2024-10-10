#ifndef _FWCOMMAND_H_
#define _FWCOMMAND_H_

#include "FWConfig.h"

// This class represents an action conducted by 
// the user. It is the common interface for all 
// commands.
class FWCommand
{
public:
	FWCommand() {}
	virtual ~FWCommand() {}

	// Override this function and take proper 
	// actions to 
	// execute this command.
	virtual void Execute() = 0;

	// When the function was called, the 
	// envirnment should be 
	// as same as the enviroment in the time 
	// which the command 
	// has just finished.
	// Override this function and take proper 
	// actions to 
	// cancel this command
	virtual void Unexecute() = 0;

};

#endif 
