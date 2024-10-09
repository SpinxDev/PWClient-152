/*
 * FILE: FaceBone.h
 *
 * DESCRIPTION: Class for one face bone of skin face model
 *
 * CREATED BY: Jiangdalong, 2004/12/09
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _FACEBONE_H_
#define _FACEBONE_H_

#include <A3DBoneController.h>
#include <AFile.h>

const int MAX_BACKUP_NUMBER			= 1;		// Matrix backup count

class CFaceBone  
{
public:
	CFaceBone();
	virtual ~CFaceBone();

// Operations
public:
	// Translate bone
	void Translate(A3DVECTOR3& v);
	// Rotate bone
	void Rotate(A3DVECTOR3& vPos, A3DVECTOR3& vRad);
	// Scale bone
	void Scale(A3DVECTOR3& vPos, A3DVECTOR3& v);
	// Update bone
	void Update(bool bSaveShape, int nTransTime);
	// Blend bone
//	void Blend(const A3DMATRIX4* mats, const float* weights, int nNumMat);
	// Blend bone for expression
//	void BlendExp(const A3DMATRIX4* mats, const float* weights, int nNumMat);
	// Add expression to current face
	void Add(const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration, bool bShape);

	// Store transform matrix to template matrix 
	void StoreTM();
	// Restore template transform matrix
	void RestoreOldTM();
	// Store transform matrix to shape
//	void StoreShapeTM();
	// Restore shape transform matrix as current matrix
	void RestoreShapeTM();

	// Save bone to file
	bool SaveBone(AFile* f, int nIndex);

	// Get transform matrix from stored TM to current TM
	A3DMATRIX4 GetTMFromOldToNow();

	// Initialize controller
	void InitController();
	// Release Controllers of a bone
	void ReleaseController();

	// Add some bones' changes on backuped face bone
	void ApplyBoneOnBakFace(int nIndex);
	// Reset transform matrix
	void ResetTM();

	inline void SetName(const char* szName) { m_strName = szName; }
	inline AString GetName() { return m_strName; }
	inline void SetSize(float fSize) { m_fSize = fSize; }
	inline float GetSize() { return m_fSize; }
	inline A3DMATRIX4& GetTransformMatrix() { return m_TM; }
	inline void SetTransformMatrix(A3DMATRIX4& m) { m_TM = m; }
	inline A3DMATRIX4& GetOldTM() { return m_OldTM; }
	inline void SetOldTM(A3DMATRIX4& m) { m_OldTM = m; }
	inline void SetPos(A3DVECTOR3& vector) { m_vPosition = vector; }
	inline A3DVECTOR3& GetPos() { return m_vPosition; }
	inline float GetScale() { return m_fScale; }
	inline void SetScale(float fScale) { m_fScale = fScale; }
	inline void SetBone(A3DBone* pBone) { m_pBone = pBone; }
	inline A3DBone* GetBone() { return m_pBone; }
	inline A3DVECTOR3& GetOldPos() { return m_vOldPos; }
	inline float GetOldScale() { return m_fOldScale; }
	inline A3DMATRIX4& GetShapeTM() { return m_ShapeTM; }
	inline void SetShapeTM(A3DMATRIX4& m) { m_ShapeTM = m; }
//	inline A3DVECTOR3 GetShapePos() { return m_vShapePos; }
//	inline void SetShapePos(A3DVECTOR3& v) { m_vShapePos = v; }
//	inline float GetShapeScale() { return m_fShapeScale; }
	inline void BackupTM(int nIndex) { m_BakTM[nIndex] = m_TM; }
	inline A3DMATRIX4& GetBackupTM(int nIndex) { return m_BakTM[nIndex]; }

// Attributes
protected:
	AString			m_strName;			// Bone name
	float			m_fSize;			// Half width
	A3DVECTOR3		m_vPosition;		// Position
	float			m_fScale;			// Scale
	A3DMATRIX4		m_TM;				// Current Transform matrix

	A3DMATRIX4		m_OldTM;			// Old Transform matrix
	float			m_fOldScale;		// Old scale
	A3DVECTOR3		m_vOldPos;			// old position

	A3DMATRIX4		m_BakTM[MAX_BACKUP_NUMBER];	// Backup Transform matrix

	A3DMATRIX4		m_ShapeTM;			// Shape Transform matrix
//	float			m_fShapeScale;		// Shape scale
//	A3DVECTOR3		m_vShapePos;		// Shape position

	A3DBone*		m_pBone;			// Corresponding bone in skeleton
	A3DBoneMatrixController*	m_pController;// Bone controller


// Operations
protected:
};

#endif // #ifndef _FACEBONE_H_