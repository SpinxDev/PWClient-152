/*
* FILE: A3DGFXECMInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/01
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXECMInterface_H_
#define _A3DGFXECMInterface_H_


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

struct GFX_LOAD_ECM_PARAM 
{
	GFX_LOAD_ECM_PARAM ()
		: nId(0)
		, pECModel(NULL)
	{

	}
	clientid_t nId;				// param passed in, specify the model's ID (Usually this id is the number set by CECModel::SetId)
	CECModel* pECModel;			// param passed in, specify an valid ECModel pointer (either loaded or unloaded, depends on the szUserCmd)
	AString strUserCmd;			// param passed in, specify actions that the artists wants to happen (an appointment made by artists and client programmers)
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXECMInterface
//	
///////////////////////////////////////////////////////////////////////////

class IGFXECMInterface
{
public:
	virtual ~IGFXECMInterface() = 0 {}

	//	Load the custom ECModel with the model id and strUserCmd which is agreed by both client programmers and artists
	//	parameters:
	//	@clientid_t nId - is the id set by client to all CECModel instances
	//	@CECModel* pECModel - is a pointer to an either empty or loaded ECModel instance 
	//						  client need to check it is loaded or not, need to be loaded or not, and perform appropriate operations
	//	@AString strUserCmd - a string agreed by both client programmers and artists, which may depends what model to be load
	virtual bool OnLoadCustomECModel(GFX_LOAD_ECM_PARAM* pLoadParam) = 0;

	//	Load the custom skin with model id and strUserCmd which is agreed by both client programmers and artists
	//	parameters
	//	@clientid_t nId - is the id set by client to all CECModel instances
	//	@CECModel* pECModel - In this case the pECModel is an already loaded ECModel pointer
	//	@AString strUserCmd - a string agreed by both client programmers and artists, which may depends what skin to be load
	virtual bool OnLoadCustomSkin(GFX_LOAD_ECM_PARAM* pLoadParam) = 0;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXECMInterface_H_


