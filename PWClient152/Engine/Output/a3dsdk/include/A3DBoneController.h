/*
 * FILE: A3DBoneController.h
 *
 * DESCRIPTION: a set of class which used for control of bones
 *
 * CREATED BY: Hedi, 2004/12/2
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBONECONTROLLER_H_
#define _A3DBONECONTROLLER_H_

#include "A3DTypes.h"
#include "hashtab.h"

class A3DBone;

//////////////////////////////////////////////////////////////////////////////////////
// base class for bone controller
//////////////////////////////////////////////////////////////////////////////////////
class A3DBoneController
{
public:
	typedef abase::hashtab<int, const char *, abase::_hash_function> USERDATATAB;

protected:
	A3DBone *				m_pTargetBone;
	A3DBoneController *		m_pNextController;

	A3DMATRIX4				m_matController;		// matrix of this controller
	USERDATATAB	*			m_pUserData;			// a hash table that will store the user data, this object will be created after first time usage

public:
	inline const A3DMATRIX4& GetControllerMatrix()		{ return m_matController; }

public:
	inline A3DBoneController * GetNextController() { return m_pNextController; }
	inline void SetNextController(A3DBoneController * pController) { m_pNextController = pController; }

	void SetUserData(const char * key, int value);
	bool GetUserData(const char * key, int * pValue);

public:
	A3DBoneController(A3DBone * pTargetBone);
	~A3DBoneController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////
// Scale controller
//////////////////////////////////////////////////////////////////////////////////////
class A3DBoneScaleController : public A3DBoneController
{
protected:
	A3DVECTOR3				m_scaleStart;	// scale start
	A3DVECTOR3				m_scaleEnd;		// scale end
	A3DVECTOR3				m_scale;		// scale now
	
	int						m_nTransTime;	// transition time from start to end
	int						m_nCurTime;		// current time when in transition

public:
	bool Scale(const A3DVECTOR3& scale, int nTransTime);

public:
	A3DBoneScaleController(A3DBone * pTargetBone);
	~A3DBoneScaleController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

//////////////////////////////////////////////////////////////////////////////////////
// Local Scale controller
//////////////////////////////////////////////////////////////////////////////////////
class A3DBoneLocalScaleController : public A3DBoneController
{
protected:
	A3DVECTOR3				m_scaleStart;	// scale start
	A3DVECTOR3				m_scaleEnd;		// scale end
	A3DVECTOR3				m_scale;		// scale now
	
	int						m_nTransTime;	// transition time from start to end
	int						m_nCurTime;		// current time when in transition

public:
	bool Scale(const A3DVECTOR3& scale, int nTransTime);

public:
	A3DBoneLocalScaleController(A3DBone * pTargetBone);
	~A3DBoneLocalScaleController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};


//////////////////////////////////////////////////////////////////////////////////////
// Rotation controller
//////////////////////////////////////////////////////////////////////////////////////
class A3DBoneRotController : public A3DBoneController
{
protected:
	A3DQUATERNION			m_quStart;		// orientation start
	A3DQUATERNION			m_quEnd;		// orientation end
	A3DQUATERNION			m_qu;			// orientation now

	int						m_nTransTime;	// transition time from start to end
	int						m_nCurTime;		// current time when in transition

public:
	bool Rotate(const A3DQUATERNION& qu, int nTransTime);

public:
	A3DBoneRotController(A3DBone * pTargetBone);
	~A3DBoneRotController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

//////////////////////////////////////////////////////////////////////////////////////
// Position controller
//////////////////////////////////////////////////////////////////////////////////////
class A3DBonePosController : public A3DBoneController
{
protected:
	A3DVECTOR3				m_vecPosStart;			// start pos of the controller
	A3DVECTOR3				m_vecPosEnd;			// end pos of the controller
	A3DVECTOR3				m_vecPos;				// current pos of the controller

	int						m_nTransTime;			// transition time from start to end
	int						m_nCurTime;				// current time when in transition

public:
	// move to some place where the delta is vecPos
	bool Move(const A3DVECTOR3& vecPos, int nTransTime);

public:
	A3DBonePosController(A3DBone * pTargetBone);
	~A3DBonePosController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

// Matrix combine controller : composed by scale, rot and position controller
class A3DBoneMatrixController : public A3DBoneController
{
protected:
	// decomposed elements of the matrix
	A3DVECTOR3				m_vecPosStart;	// position start
	A3DVECTOR3				m_vecPosEnd;	// position end
	A3DVECTOR3				m_vecPos;		// position now

	A3DQUATERNION			m_quStart;		// orientation start
	A3DQUATERNION			m_quEnd;		// orientation end
	A3DQUATERNION			m_qu;			// orientation now

	A3DVECTOR3				m_scaleStart;	// scale start
	A3DVECTOR3				m_scaleEnd;		// scale end
	A3DVECTOR3				m_scale;		// scale now
	
	int						m_nTransTime;	// transition time from start to end
	int						m_nCurTime;		// current time when in transition

protected:
	bool CombineMatrixElements(const A3DVECTOR3& vecScale, const A3DQUATERNION& quOrient, const A3DVECTOR3& vecPos, int nTransTime);

public:
	bool CombineMatrix(const A3DMATRIX4& mat, int nTransTime);
	bool CombineMatrixList(const A3DMATRIX4 * mats, float * weights, int nNumMat, int nTransTime);

public:
	A3DBoneMatrixController(A3DBone * pTargetBone);
	~A3DBoneMatrixController();

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

class A3DBoneLookAtController : public A3DBoneController
{
protected:
	A3DVECTOR3		m_vecEyeAxisInBoneSpace;
	A3DVECTOR3		m_vecHeadUpAxisInBoneSpace;

	float			m_vDegMin;
	float			m_vDegMax;
	float			m_vDegScale;
	
	float			m_vPitchMin;
	float			m_vPitchMax;
	float			m_vPitchScale;

	bool			m_bOutRangeAutoReturn;

	bool			m_bHasTarget;
	A3DVECTOR3		m_vecTargetPos;
	
	A3DQUATERNION	m_quStart;		// orientation start
	A3DQUATERNION	m_quEnd;		// orientation end
	A3DQUATERNION	m_qu;			// orientation now

	int				m_nTransTime;	// transition time from start to end
	int				m_nCurTime;		// current time when in transition

protected:
	bool SolveLookAt();

public:
	bool LookAt(const A3DVECTOR3& vecTargetPos, int nTransTime);
	bool StopLookAt(int nTransTime);

public:
	A3DBoneLookAtController(A3DBone * pTargetBone);
	~A3DBoneLookAtController();

	void SetDegMaxMin(float vDegMax, float vDegMin=0.0f, float vDegScale=1.0f);
	void SetPitchMaxMin(float vPitchMax, float vPitchMin=0.0f, float vPitchScale=1.0f);
	void SetAxis(const A3DVECTOR3& vecEyeAxis, const A3DVECTOR3& vecHeadUpAxis);
	void SetOutRangeAutoReturn(bool bFlag) { m_bOutRangeAutoReturn = bFlag; }

	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

class A3DBoneBodyTurnController : public A3DBoneController
{
public:
	enum BODYTURN_TYPE
	{
		BODYTURN_AXIS_Y = 0,
		BODYTURN_AXIS_X,
		BODYTURN_AXIS_Z,
		BODYTURN_AXIS_BONE,
	};

protected:
	float			m_vDegTurn;
	BODYTURN_TYPE	m_type;

public:
	A3DBoneBodyTurnController(A3DBone * pTargetBone);
	~A3DBoneBodyTurnController();

	void SetTurnDeg(float vDegTurn);
	void SetTurnType(BODYTURN_TYPE type);
	
	///////////////////////////////////////////////////////////////////////////////
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime		IN		delta time from last update
	// pTargetBone		IN		target bone to be controlled
	// matControlled	OUT		output the matrix to be multiplied with the bone
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime, A3DBone * pTargetBone, A3DMATRIX4& matControlled);
};

#endif//_A3DBONECONTROLLER_H_
