/*
 * FILE: A3DImgModel.h
 *
 * DESCRIPTION: Class for Image model object
 *
 * CREATED BY: duyuxin, 2002/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DIMGMODEL_H_
#define _A3DIMGMODEL_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"

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

//	Image model action
struct IMGMODELACTION
{
	DWORD		dwID;			//	Action ID
	char		szName[64];		//	Name
	int			iNumFrame;		//	Number of frame
	int			iFirstCell;		//	First cell's index
	int			iLastCell;		//	Last cell's index
	int			iStart;			//	Start frame
	bool		bLoop;			//	Loop flag
	bool		bForward;		//	Animation direction. true, forward, false, backward
}; 

struct IMGMODELCELL
{
	int			iTexture;		//	Texture slot's index
	float		u1;				//	u of left-top corner
	float		v1;				//	v of left-top corner
	float		u2;				//	u of right-bottom corner
	float		v2;				//	v of right-bottom corner
	float		fWidth;			//	Size in metres
	float		fHeight;
	float		fOffx;			//	Offset in metres
	float		fOffy;
};

class AScriptFile;
class A3DDevice;
class A3DCameraBase;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DImgModel
//
///////////////////////////////////////////////////////////////////////////

class A3DImgModel
{
public:		//	Types

	enum
	{
		MAXNUM_ACTINGROUP	= 32	//	Maximum number of actions in a group
	};

	struct ACTGROUP
	{
		char	szName[64];		//	Name of action group
		DWORD	dwID;			//	ID of action group
		int		iNumAction;		//	Number of action
		bool	bLoop;			//	Loop flag
		
		DWORD	aActions[MAXNUM_ACTINGROUP];	//	Action ID list
		int		aIndices[MAXNUM_ACTINGROUP];	//	Action index list
	};

	//	Ray trace type
	enum
	{
		RAYTRACE_NONE		= 0,	//	Don't do ray trace
		RAYTRACE_SHEET		= 1,	//	Use sheet
		RAYTRACE_FIXAABB	= 2,	//	Use fixed AABB
		RAYTRACE_AABB		= 3,	//	Use variable AABB
		RAYTRACE_FIXOBB		= 4,	//	Use thickness fixed OBB
		RAYTRACE_OBB		= 5,	//	Use thickness variable OBB
	};

	//	Ray trace structure
	struct RAYTRACEINFO
	{
		int			iType;		//	Ray trace type
		bool		bTwoSide;	//	Two side flag for 'Sheet' ray trace
		A3DAABB		aabb;		//	Used by 'AABB' ray trace
		A3DOBB		obb;		//	Used by 'OBB' ray trace
		float		fThick;		//	obb's thick

		A3DVECTOR3	vStart;		//	ray's start postion
		A3DVECTOR3	vDir;		//	ray's normalized direction
		A3DVECTOR3	vDelta;		//	ray's move delta
		float		fFrac;		//	Hit fraction
		A3DVECTOR3	vHitPos;	//	Hit position
	};

public:		//	Constructors and Destructors

	A3DImgModel();
	virtual ~A3DImgModel();

public:		//	Attributes

	friend class A3DImageModelMan;

public:		//	Operations

	void		Release();		//	Release

	bool		Copy(A3DImgModel* pSrc);		//	Copy image model
	bool		BitCopy(A3DImgModel* pSrc);		//	Bit-Copy image model

	bool		PlayActionByName(char* szName, int iStart=-1);	//	Play specified action
	void		PauseAniamtion(bool bPause);		//	Pause animation
	void		SetVertexCol(int v, DWORD dwCol);	//	Set vertex color
	A3DVECTOR3	GetMarkPointPos(char* szName);		//	Get mark point position in world space
	int			GetCurFrame();						//	Get current frame

	bool		TickAnimation();		//	Tick animation
	bool		FakeRender(A3DViewport* pViewport, bool bUpdateShape);	//	Add model to render queue
	void		UpdateShape(A3DCameraBase* pCamera);	//	Update shape
	bool		RayTrace(A3DVECTOR3& vStart, A3DVECTOR3& vVel, float fTime,
						 A3DVECTOR3* pvHitPos, float* pfFrac);	//	Do ray trace

	const char*	GetName()					{	return m_szName;		}
	bool		IsVisible()					{	return m_bVisible;		}
	void		SetVisible(bool bVisible)	{	m_bVisible = bVisible;	}
	A3DVECTOR3	GetPos()					{	return m_vPos;			}
	void		SetPos(A3DVECTOR3& vPos)	{	m_vPos = vPos;			}
	A3DVECTOR3	GetRight()					{	return m_vRight;		}
	A3DLVERTEX	GetVertex(int v)			{	return m_aVerts[v];		}
	void		EnableViewCull(bool bCull)	{	m_bViewCull = bCull;	}
	bool		DoingViewCull()				{	return m_bViewCull;		}

	void		DisableRayTrace()			{	m_RayInfo.iType = RAYTRACE_NONE;	}

	void		SetRotateAxis(A3DVECTOR3& vAxis)	{	m_vRotateAxis = vAxis;	}
	A3DVECTOR3	GetRotateAxis()						{	return m_vRotateAxis;	}

	inline void	UseAABBRayTrace(A3DAABB* paabb);	//	Use AABB ray trace
	inline void UseOBBRayTrace(float fThickness);	//	Use OBB ray trace
	inline void UseSheetRayTrace(bool bTwoSide);	//	Use sheet ray trace

protected:	//	Attributes

	A3DImageModelMan*	m_pManager;		//	Mananger object
	int					m_iMGIndex;		//	Index of model group which this model belongs to

	char		m_szName[64];		//	Model's name
	DWORD		m_dwID;				//	Model's ID
	ACTGROUP*	m_aGroups;			//	Action groups
	int			m_iNumGroup;		//	Number of action group
	ACTGROUP	m_SoleGroup;		//	Used by sole action

	bool		m_bVisible;			//	Visible flag
	bool		m_bNoAction;		//	No action state

	bool		m_bPause;			//	Pause flag
	bool		m_bStop;			//	Stop flag
	ACTGROUP*	m_pCurGroup;		//	Current action group
	int			m_iCurAction;		//	Current played action
	int			m_iActIndex;		//	Current action's index returned by manager
	int			m_iFrameCnt;		//	Flame counter
	int			m_iCurCell;			//	Current cell
	
	IMGMODELACTION	m_CurAct;		//	Currect action's information

	A3DVECTOR3	m_vPos;				//	Position
	A3DVECTOR3	m_vRight;			//	Right
	A3DVECTOR3	m_vRotateAxis;		//	Rotate axis
	A3DVECTOR3	m_vCenter;			//	Center postion of image sheet
	A3DLVERTEX	m_aVerts[4];		//	Vertices
	bool		m_bViewCull;		//	View cull enable

	IMGMODELCELL	m_CellInfo;		//	Cell information
	RAYTRACEINFO	m_RayInfo;		//	Ray trace information

protected:	//	Operations

	//	These interfaces are prepared for image model manager

	bool		Load(A3DImageModelMan *pMan, char* szFile, int iMGIndex);	//	Load image model from file
	inline bool	PushVertsToRenderBuffer(A3DLVERTEX* pVertBuf);				//	Push vertices to render buffer

	DWORD		GetID()				{	return m_dwID;			}
	int			GetMGIndex()		{	return m_iMGIndex;		}
	int			GetFrameCnt()		{	return m_iFrameCnt;		}
	int			GetCurActIndex()	{	return m_iActIndex;		}
	bool		NoAction()			{	return m_bNoAction;		}
	int			GetCurCell()		{	return m_iCurCell;		}

	bool		RayTrace_Sheet();	//	Sheet ray trace
	bool		RayTrace_AABB();	//	AABB ray trace
	bool		RayTrace_OBB();		//	OBB ray trace

protected:	//	Operations

	bool		ParseActionGroup(AScriptFile* pFile, ACTGROUP* pGroup);		//	Parse action group
	void		SetCurrentAction(int iAction, int iStart);	//	Set current action
	void		SetCurrentFrame();							//	Set current frame
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Render vertices buffer

	Return true for success, otherwise return false.

	pVertBuf: buffer which ls length enough (at least 4 vertices) to receive
			  rendered vertices.
*/
bool A3DImgModel::PushVertsToRenderBuffer(A3DLVERTEX* pVertBuf)
{
	memcpy(pVertBuf, m_aVerts, sizeof (A3DLVERTEX) * 4);
	return true;
}

/*	Use AABB ray trace

	paabb (in): if aabb isn't NULL, ray trace will use the fixed AABB all the time
*/
void A3DImgModel::UseAABBRayTrace(A3DAABB* paabb)
{
	if (paabb)
	{
		m_RayInfo.iType = RAYTRACE_FIXAABB;
		m_RayInfo.aabb	= *paabb;
	}
	else
		m_RayInfo.iType = RAYTRACE_AABB;
}

/*	Use OBB ray trace

	fThickness: OBB's thickness along image model's diretion axis. if fThickness < 0.0f,
				thickness will always same as width
*/
void A3DImgModel::UseOBBRayTrace(float fThickness)
{
	if (fThickness >= 0.0f)
	{
		m_RayInfo.iType	 = RAYTRACE_FIXOBB;
		m_RayInfo.fThick = fThickness;
	}
	else
		m_RayInfo.iType	= RAYTRACE_OBB;
}

//	Use sheet ray trace
void A3DImgModel::UseSheetRayTrace(bool bTwoSide)
{
	m_RayInfo.iType		= RAYTRACE_SHEET;
	m_RayInfo.bTwoSide	= bTwoSide; 
}


#endif	//	_A3DIMGMODEL_H_


