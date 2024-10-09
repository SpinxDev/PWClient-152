/*
 * FILE: FaceCommon.h
 *
 * DESCRIPTION: Common definition header file for face
 *
 * CREATED BY: Hedi, 2005/3/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _FACECOMMON_H_
#define _FACECOMMON_H_

const int TOTAL_BONE_COUNT		= 111;				// Total bone count
const int RIGHT_BONE_COUNT		= 46;				// Right bone count
const int LEFT_BONE_COUNT		= RIGHT_BONE_COUNT;	// Left bone count
const int MIDDLE_BONE_COUNT		= 19;				// Middle bone count
const int ACTIVE_BONE_COUNT		= RIGHT_BONE_COUNT + MIDDLE_BONE_COUNT;
const int RIGHT_EYEBALL_INDEX	= 13;				// Index of right eyeball
const int LEFT_EYEBALL_INDEX	= 78;				// Index of left eyeball
const int NECK_INDEX			= 46;				// Index of neck
const int BONE_GROUP_COUNT		= 19;				// Bone group count
const int BONE_PART_COUNT		= 3;				// Bone part count
const int CHEEK_BONE_INDEX1		= 5;				// Cheek bone index 1
const int CHEEK_BONE_INDEX2		= 9;				// Cheek bone index 2
const int RIGHT_MOUTH_CORNER	= 32;				// Right mouth corner index
const int TOTAL_FAP_COUNT		= 13;				// Total FAP count

enum CENTERTYPE
{
	CT_SELF			= 0,		// Rotate(scale) around itself
	CT_ALL			= 1,		// Around center of all
	CT_SPECIAL		= 2,		// Around center of one bone
};

#endif//_FACECOMMON_H_