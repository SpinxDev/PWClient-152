/*
 * FILE: FaceBoneController.h
 *
 * DESCRIPTION: Class for controlling face bones
 *
 * CREATED BY: Jiangdalong, 2004/12/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _FACEBONECONTROLLER_H_
#define _FACEBONECONTROLLER_H_

#include "FaceBone.h"
#include <A3DSkeleton.h>
#include "FaceAnimation.h"
#include "FaceCommon.h"

class CFaceBoneController  
{
public:
	enum EDITSCOPE
	{
		ESP_BOTH,				// Edit whole face
		ESP_LEFT,				// Edit only left face
		ESP_RIGHT,				// Edit only right face
	};

public:
	CFaceBoneController(A3DSkeleton* pSkeleton = NULL);
	virtual ~CFaceBoneController();

// Operations
public:
	// Set skeleton
	void SetSkeleton(A3DSkeleton* pSkeleton);
	// Init bone
	void InitBone();
	// Import bone info from skeleton
	bool ImportBoneFromSkeleton();
	// Release
	void Release();

	/*
	 * Bone selection
	 */
	// Clear selection
	void ClearSelection();
	// Add selection
	void AddSelection(int nIndex);
	// Remove selection
	void RemoveSelection(int nSelIndex);
	// Remove left bones
	void RemoveLeftBones();
	// Is in selection? Return -1 means not in selection.
	int PosInSelection(int nIndex);
	// Set special center
	void SetSpecialCenter(int nIndex);
	// Get center of selected bones
	A3DVECTOR3 GetSelectedCenter();
	// Select bone group
	void SelectBoneGroup(int nIndex);
	// Select a bone part
	void SelectBonePart(int nPart);
	// Mirror select from right to left
	void MirrorSelectFromRightToLeft();

	/*
	 * Bone operation
	 */
	// Translate bone
	bool TranslateBone(A3DVECTOR3& vector, bool bUpdate, bool bHalfFace);
	// Rotate bone
	bool RotateBone(A3DVECTOR3& v, CENTERTYPE type, bool bUpdate, bool bHalfFace);
	// Scale bone
	bool ScaleBone(A3DVECTOR3& v, CENTERTYPE type, bool bUpdate, bool bHalfFace);
	// Update bone
	void UpdateBone(int nIndex, bool bSaveShape);
	// Update all bones
	void UpdateAllBone(bool bSaveShape);
	// Move cheek
	bool MoveCheek(float fDis, bool bUpdate, bool bHalfFace);
	// Scale eyeball
	bool ScaleEyeball(A3DVECTOR3& vScale, bool bUpdate, EDITSCOPE scope);
	// Updown move mouth corner
	bool MoveMouthCorner(float fDis, bool bUpdate, EDITSCOPE scope);
	// Move nose bridge
	bool MoveNoseBridge(float fDis, bool bUpdate, bool bHalfFace);
	// Blend bone
//	void BlendBone(int nIndex, const A3DMATRIX4* mats, float* weights, int nNumMat);
	// Blend bone for expression
//	void BlendBoneExp(int nIndex, const A3DMATRIX4* mats, float* weights, int nNumMat);
	// Blend bone for shape
	void AddShape(int nIndex, const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration);
	// Blend bone for expression
	void AddExpr(int nIndex, const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration);

	/*
	 * Three parts operation
	 */
	// Scale part up
	void ScalePartUp(float fScale, bool bUpdate);
	// Scale part middle
	void ScalePartMiddle(float fScale, bool bUpdate);
	// Scale part down
	void ScalePartDown(float fScale, bool bUpdate);
	// Scale 3 parts
	void ScaleParts(float fUp, float fMid, float fDown);

	/*
	 * Save and load bones
	 */
	// Load bone from file
	bool LoadBoneFromFile(const char* szFile);
	// Load bone from file
//	bool LoadBoneFromFileEx(const char* szFile);
	// Save all bones
	bool SaveAllBoneToFile(const char* szFile);
	// Save selected bones
	bool SaveSelBoneToFile(const char* szFile, bool bHalfFace);

	/*
	 * Store and get bone transform matrix
	 */
	// Store current transform matrix for bones
	void StoreBoneTM();
	// Restore all old transform matrix
	void RestoreBoneTM();
	// Get bones tranform matrix change from last storage to current
	void GetBoneTMChange(int* pIndex, A3DMATRIX4* pMatrix);
	// Store current transform matrix as face shape
//	void StoreShapeTM();
	// Restore all shape transform matrix
	void RestoreShapeTM();
	// Save current transform matrix to backup face.
	void BackupBoneTM();
	// Apply other organs to backup face
	void ApplyBonesOnBakFace(bool bUpdate);
	// Reset selected bones
	void ResetSelectedBones(bool bUpdate, bool bHalfFace);
	// Reset all bones
	void ResetAllBones(bool bUpdate);

	inline A3DSkeleton* GetSkeleton() { return m_pSkeleton; }
	inline CFaceBone* GetFaceBone() { return m_FaceBone; }
	inline bool GetReady() { return m_bReady; }
	inline int GetSpecailCenter() { return m_nSpecialCenter; }
	inline int GetSelectedCount() { return m_nSelectedCount; }
	inline int* GetSelectedBone() { return m_SelectedBone; }
	inline AString* GetBoneGroupName() { return m_BoneGroupName; }

// Attributes
protected:
	A3DSkeleton*		m_pSkeleton;					// Modle skeleton
	CFaceBone			m_FaceBone[TOTAL_BONE_COUNT];	// Face bones
	bool				m_bReady;						// Initialize successful

	int					m_SelectedBone[TOTAL_BONE_COUNT];// Selected bone indexs
	int					m_nSelectedCount;				// Selected bone count
	int					m_nSpecialCenter;				// Special center(for rotate and scale)

	static bool			m_bStaticDataInited;			// flag indicate static data has been initialized

	static AString			m_BoneName[TOTAL_BONE_COUNT];	// Bone names
	static AArray<int, int>	m_BoneGroup[BONE_GROUP_COUNT];	// Bone groups
	static AString			m_BoneGroupName[BONE_GROUP_COUNT];// Group names
	static AArray<int, int>	m_BonePart[BONE_PART_COUNT];	// Bone parts

	float				m_BonePartScale[BONE_PART_COUNT];// Scale of 3 parts

// Operations
protected:
	/*
	 * Bone initialize
	*/
	// Load bone initial parameters
	static bool LoadBoneInitParam(const char* szFile);
	// Load bone group infomation
	static bool LoadBoneGroupInfo(const char* szFile);
	// Load bone part infomation
	static bool LoadBonePartInfo(const char* szFile);
	// Get bone index by name
	static int GetBoneIndexByName(const char* szName);

};

#endif // #ifndef _FACEBONECONTROLLER_H_
