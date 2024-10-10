/*
* FILE: CECModelShell.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/01/06
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _CECModelShell_H_
#define _CECModelShell_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECModel;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECModelShell
//	
///////////////////////////////////////////////////////////////////////////

class CECModelShell
{

public:		//	Types

public:		//	Constructor and Destructor

	//	Create an empty shell
	CECModelShell(void);
	//	Create the shell with a file
	CECModelShell(const char* szFile);
	virtual ~CECModelShell(void);

public:		//	Attributes

public:		//	Operations

	//	Swap the ECModel instance to another specified by the filename
	bool SwapECModel(const char* szFile);
	
	//	Get the current ECModel instance (return value might be NULL if there is no ECModel contained)
	//	DO NOT store the pointer you got here, it might be invalid after calling SwapECModel
	//	Each time you need an ECModel's pointer, you need to call this GetECModel()
	inline CECModel* GetECModel() const { return m_pECModelInst; }

protected:	//	Attributes

	CECModel* m_pECModelInst;

protected:	//	Operations

	//	Disable copy construct and operator = explicitly
	CECModelShell(const CECModelShell&);
	CECModelShell& operator = (const CECModelShell&);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_CECModelShell_H_


