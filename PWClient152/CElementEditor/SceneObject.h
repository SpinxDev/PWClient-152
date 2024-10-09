// SceneObject.h: interface for the CSceneObject class.
//----------------------------------------------------------
// Filename	: SceneObject.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 所有场景对象的基本类
//-----------------------------------------------------------
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEOBJECT_H__D89A9C5C_147C_45D5_B88B_0B366A8822DA__INCLUDED_)
#define AFX_SCENEOBJECT_H__D89A9C5C_147C_45D5_B88B_0B366A8822DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditObject.h"
#include "ElementMap.h"
#include "AObject.h"
#include "EL_Archive.h"

#define BOX_WIDTH	0.5f

class CSceneObject :public CEditObject
{
public:
	enum
	{
		SO_TYPE_NO = 0,
		SO_TYPE_LIGHT,
		SO_TYPE_STATIC_MODEL,
		SO_TYPE_WATER,
		SO_TYPE_BEZIER,
		SO_TYPE_AREA,
		SO_TYPE_AUDIO,
		SO_TYPE_DUMMY,
		SO_TYPE_AIAREA,
		SO_TYPE_SPECIALLY,
		SO_TYPE_ECMODEL,
		SO_TYPE_CRITTER_GROUP,
		SO_TYPE_FIXED_NPC,
		SO_TYPE_PRECINCT,
		SO_TYPE_GATHER,
		SO_TYPE_RANGE,
		SO_TYPE_CLOUD_BOX,
		SO_TYPE_INSTANCE_BOX,
		SO_TYPE_DYNAMIC,
	};
	
public:
	ADynPropertyObject* m_pProperty;

	CSceneObject();
	virtual ~CSceneObject();

	virtual A3DVECTOR3			GetMin();
	virtual A3DVECTOR3			GetMax();
	virtual A3DVECTOR3			GetPos(){ return m_vPos; };
	virtual A3DMATRIX4			GetTransMatrix(){ return m_matAbs; };
	virtual A3DMATRIX4			GetRotateMatrix(){ return m_matRotate; };
	virtual A3DMATRIX4			GetScaleMatrix(){ return m_matScale; };
	//这儿更新属性表，如果bGet是真的话表示从表中取数据来更新对象数据
	virtual void				UpdateProperty(bool bGet)=0;

	virtual void				Translate(const A3DVECTOR3& vDelta);
	virtual void				RotateX(float fRad);
	virtual void				RotateY(float fRad);
	virtual void				RotateZ(float fRad);
	virtual void				Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void				Scale(float fScale);
	virtual void				SetPos(const A3DVECTOR3& vPos);

	virtual void				Release() {};
	virtual void				Tick(DWORD timeDelta){};
	virtual void				Render(A3DViewport* pA3DViewport);
	virtual void                RenderForLight(A3DViewport* pA3DViewport, float fOffsetX, float fOffsetZ){};
	virtual void				RenderForLightMapShadow(A3DViewport* pA3DViewport){};    
	virtual void				EntryScene(){};
	virtual void				LeaveScene(){};
	virtual void                ResetRotateMat(){ m_matRotate.Identity(); };
	
	virtual CSceneObject*		CopyObject(){ return NULL; };

	virtual void                SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up)
								{
									A3DVECTOR3 right;
									right.CrossProduct(up,dir);
		
									m_matRotate.SetRow(2,dir);
									m_matRotate.SetRow(1,up);
									m_matRotate.SetRow(0,right);
									m_matAbs = m_matScale * m_matRotate * m_matTrans;
								}
	virtual void                SetScale(const A3DVECTOR3& vs)
								{
									m_matScale._11 = vs.x;
									m_matScale._22 = vs.y;
									m_matScale._33 = vs.z;
									m_matAbs = m_matScale * m_matRotate * m_matTrans;
								}

	//	Load data
	virtual bool		Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags) { assert(false && "此类没有实现串行化"); return false;}
	//	Save data
	virtual bool		Save(CELArchive& ar,DWORD dwVersion) { assert(false && "此类没有实现串行化"); return false;}

	AString&			GetObjectName(){ return m_strName; }
	void				SetObjectName( const AString& name ){ m_strName = name; }
	
	inline int			GetSceneObjectType() { return m_nObjectType; }
	inline void			SetSelected( bool bSelected = true) { m_bSelected = bSelected; }

	inline int			GetObjectType(){ return m_nObjectType; }
	inline void			SetObjectID(DWORD id){ m_nObjectID = id; }
	inline int			GetObjectID() { return m_nObjectID; }

	inline void			SetExportID(DWORD id) { m_dwExportID = id; }
	inline int			GetExportID() { return m_dwExportID; }

	inline void         SetDeleted(bool bDel){ m_bDeleted = bDel; }
	inline bool         IsDeleted(){ return m_bDeleted; }
	inline bool         IsSelected(){ return m_bSelected; }

	virtual int					GetTriangleNum(){ return 0; }
	

protected:
	
	int				m_nObjectType;
	bool			m_bSelected;
	AString			m_strName;

	A3DMATRIX4		m_matRotate;
	A3DMATRIX4		m_matTrans;
	A3DMATRIX4		m_matScale;
	A3DMATRIX4      m_matAbs;

	A3DVECTOR3		m_vPos;
	A3DVECTOR3		m_vDirection;
	int				m_nObjectID; //对象唯一ID
	bool            m_bDeleted;  

	DWORD			m_dwExportID;	//	ID used when exporting
	
protected:
	float			m_fScaleUnit;
	void			DrawCenter();
};

typedef CSceneObject* PSCENEOBJECT;
extern float _getAngle(float X,float Y);

#endif // !defined(AFX_SCENEOBJECT_H__D89A9C5C_147C_45D5_B88B_0B366A8822DA__INCLUDED_)
