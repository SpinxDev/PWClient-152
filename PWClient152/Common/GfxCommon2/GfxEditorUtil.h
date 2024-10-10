/*
* FILE: GfxEditorUtil.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _GfxEditorUtil_H_
#define _GfxEditorUtil_H_

#include <AString.h>
#include <vector.h>

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

typedef abase::hash_map<AString, int> ExpFileContainer;

struct EXPFILEDATA
{
	EXPFILEDATA(const char* strRelFile
		, A3DEngine* pEngine
		, bool bExpChildModels
		, bool bExpAdditionalSkin
		, bool bExpPhysics
		, abase::hash_map<AString, int>& aFiles);
	AString		m_strRelFile;						//	In: File path relative to Angelica Base Dir
	A3DEngine*	m_pEngine;							//	In: Pointer to a valid a3d engine
	const bool	m_bExpChildModels;					//	In: Collect ecm's child model set in editor (normally child model is only used in editor)
	const bool	m_bExpAdditionalSkin;				//	In: Collect ecm's additional skin set in editor (normally additional skin is only used in editor)
	const bool	m_bExpPhysics;						//	In: Collect gfx's gphy / ecm's mphy / ski's sphy files for physics
	ExpFileContainer& m_aFiles;						//	Out: RelatedfFile paths relative to Angelica Base Dir
													//		 Note: in fact we need a std::set here, to avoid using stl , we take abase::hash_map
													//			   as the alternative one
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

//	Collect files the .smd file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectSMDFiles(EXPFILEDATA* pExpData);

//	Collect files the .ecm file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to Angelica base dir
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectECMFiles(EXPFILEDATA* pExpData);

//	Collect files the .gfx file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to AngelicaBaseDir\Gfx\
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectGFXFiles(EXPFILEDATA* pExpData);

//	Collect files the .att file needed (for export)
//	EXPFILEDATA::strRelFile is a path relative to AngelicaBaseDir\Gfx\SkillAttack\
//	Return true for successfully collected files, false for error
//	This function is only defined in the editor version of GFX library
bool a_ExpCollectATTFiles(EXPFILEDATA* pExpData);

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_GfxEditorUtil_H_


