/*
 * FILE: A3DGraphicsFX.h
 *
 * DESCRIPTION: Graphics FX Class is representing the graphics effects used in game;
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DGRAPHICSFX_H_
#define _A3DGRAPHICSFX_H_

#include "A3DTypes.h"
#include "A3DObject.h"
#include "AList.h"

class A3DDevice;
class A3DParticleSystem;
class A3DSuperSpray;
class A3DSuperDecal;
class A3DBillBoard;
class A3DPArray;
class A3DModel;
class A3DFrame;
class A3DViewport;
class A3DGraphicsFX;
class AFile;

#define A3DGFX_CATEGORY_NORMALGFX	0x1
#define A3DGFX_CATEGORY_DLIGHTGFX	0x2
#define A3DGFX_CATEGORY_SHADOWGFX	0x4
#define A3DGFX_CATEGORY_MARKGFX		0x8
#define A3DGFX_CATEGORY_UIGFX		0x10

class A3DGFXPlayEvent : public A3DObject
{
public:		//	Type

	//	Event type
	enum EVENTTYPE
	{
		TYPE_NULL = -1,
		TYPE_ITEM = 0,
		TYPE_RANDITEM = 1,
		TYPE_VOLUMETRICITEM = 2  // volumetric item is composed by several items that will shift forward or backward a little
	};

	struct EVENTITEM
	{
		A3DObject*	pElement;
		DWORD		dwClassID;
	};

private:

	A3DGraphicsFX*	m_pHostGFX;
	bool			m_bIsActive;		// flag indicates whether it is active now

	// data specify the event;
	EVENTTYPE		m_Type;
	void*			m_pItem;
	int				m_nStartBegin;		// range of start playing the gfx, random select among it
	int				m_nStartEnd;

	int				m_nThisStart;		// start time of this instance

public:
	A3DGFXPlayEvent();
	virtual ~A3DGFXPlayEvent();

	bool Init(A3DGraphicsFX * pHostGFX);
	bool Release();

	bool Load(A3DGraphicsFX * pHostGFX, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	bool HasElement(A3DObject * pElement);	// method to query if a specified element is referenced by this event;
	bool GetRidOfElement(A3DObject * pElement);		// methods let the event to clean the reference to some element, return false will let gfx to delete this event

	inline EVENTTYPE GetType() { return m_Type; }
	inline void SetType(EVENTTYPE type) { m_Type = type; }

	inline void* GetItemPtr()				{ return m_pItem; }
	inline void SetItemPtr(void* pItem)		{ m_pItem = pItem; }

	inline int GetStartBegin()				{ return m_nStartBegin; }
	inline void SetStartBegin(int nBegin)	{ m_nStartBegin = nBegin; }

	inline int GetStartEnd()				{ return m_nStartEnd; }
	inline void SetStartEnd(int nEnd)		{ m_nStartEnd = nEnd; }

	inline int GetThisStart()				{ return m_nThisStart; }

	inline bool IsActive()					{ return m_bIsActive; }
	bool SetActive(bool bActive);
};

class A3DGraphicsFX : public A3DObject
{
	friend class A3DGFXPlayEvent;

private:
	A3DDevice			* m_pA3DDevice;
	bool				m_bHWIGFX;			//Flag indicates using outside graphics engine;

	A3DModel			* m_pHostModel;		//Host model on which this effect will apply
	A3DFrame			* m_pParentFrame;	//Parent frame in which this effect will be
	bool				m_bLinked;			//Flag to say if this effect will move together with its parent frame

	int					m_nEverTicks;		//ticks of the whole graphics object from start of playing;
	int					m_nDelayTicks;		//ticks that delay the start of playing;

	// Position and orientation;
	A3DVECTOR3			m_vecPos;
	A3DVECTOR3			m_vecDir;
	A3DVECTOR3			m_vecUp;

	A3DMATRIX4			m_matRelativeTM;
	A3DMATRIX4			m_matParentTM;
	A3DMATRIX4			m_matAbsoluteTM;

	ALISTELEMENT		* m_pStoredElement; //Where this graphics fx is stored; used when release it;
	ALISTELEMENT		* m_pStoredElementInHost; //Where this graphics fx is stored in its host model;

	FLOAT				m_vMaxDecalSize;
	AList				m_SuperDecalList;
	AList				m_BillBoardList;
	AList				m_SuperSprayList;
	AList				m_PArrayList;
	
	AList				m_PlayEventList;	// List for a predefined play schedule

	bool				m_bExpired; //Whether this effects has finished its life;
	bool				m_bDieOnExpired; //Whether this graphicsFX should be released when it stops;

	FLOAT				m_vScaleX; // How large the effect will be scaled;
	FLOAT				m_vScaleY; 
	FLOAT				m_vScaleZ;
	FLOAT				m_vScale;
	FLOAT				m_vLength; // The length for billboard object;

	bool				m_bHasCreated;

	// Category information;
	int					m_Category;			// this is the type flag bits of this gfx

	bool				m_bDrawByMan;		// flag indicates whether it will be rendered by gfx man;

protected:
	bool ActiveAllPlayEvents(bool bActive);
	bool FirePlayEvent(A3DGFXPlayEvent * pEvent);
	
public:
	A3DGraphicsFX();
	~A3DGraphicsFX();

	// Start this graphics FX's animating;
	bool Start(bool bDieOnExpired=true);
	// Stop it;
	bool Stop(bool bDieOnExpired=true);

	//Called from A3DGFXMan, just render and ticking;
	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();

	// Init this object;
	bool Init(A3DDevice * pA3DDevice);
	// Release this object;
	bool Release();

	bool SetPos(A3DVECTOR3 vecPos);
	bool SetDirAndUp(A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool SetDir(A3DVECTOR3 vecDir);// Set a dir and calculate a correct up auto;
	bool SetParentTM(A3DMATRIX4 parentTM);
	bool UpdateParentInfo(); // Called by an A3DModel to notify this gfx to update its parent's info;
	bool UpdateAbsoluteTM();

	// Create all graphics effects in this object;
	bool CreateAllFX(A3DModel * pHostModel, A3DFrame * pParentFrame, bool bLinked, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp);
	bool RecalculateAllFX(); // Recalculate all fx's info using new position, dir and up;

	// Load/Save Data From/To file;
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);
	bool Save(AFile * pFileToSave, bool bPackedUp=false);// bPackedUp is used when to short the texture path to a filename only

	bool CleanRefInEventList(A3DObject * pElement);

	bool AddSuperSpray(A3DSuperSpray * pSuperSpray);
	bool DeleteSuperSpray(A3DSuperSpray * pSuperSpray);
	bool AddPArray(A3DPArray * pPArray);
	bool DeletePArray(A3DPArray * pPArray);
	bool AddSuperDecal(A3DSuperDecal * pSuperDecal);
	bool DeleteSuperDecal(A3DSuperDecal * pSuperDecal);
	bool AddBillBoard(A3DBillBoard * pBillBoard);
	bool DeleteBillBoard(A3DBillBoard * pBillBoard);

	bool AddPlayEvent(A3DGFXPlayEvent * pEvent);
	bool DeletePlayEvent(A3DGFXPlayEvent * pEvent);

	A3DSuperSpray * GetSuperSpray(int index);
	A3DSuperSpray * GetSuperSprayByName(char * szName);

	A3DPArray * GetPArray(int index);
	A3DPArray * GetPArrayByName(char * szName);

	A3DSuperDecal * GetSuperDecal(int index);
	A3DSuperDecal * GetSuperDecalByName(char * szName);

	A3DBillBoard * GetBillBoard(int index);
	A3DBillBoard * GetBillBoardByName(char * szName);

	A3DGFXPlayEvent * GetPlayEvent(int index);
	A3DGFXPlayEvent * GetPlayEventByName(char * szName);

	inline A3DVECTOR3 GetPos() { return m_vecPos; }
	inline A3DVECTOR3 GetDir() { return m_vecDir; }
	inline A3DVECTOR3 GetUp()  { return m_vecUp; }

	inline void SetStoredElement(ALISTELEMENT * pStoredElement) { m_pStoredElement = pStoredElement; }
	inline ALISTELEMENT * GetStoredElement() { return m_pStoredElement; }

	inline AList * GetSuperSprayList() { return &m_SuperSprayList; }
	inline AList * GetPArrayList() { return &m_PArrayList; }
	inline AList * GetSuperDecalList() { return &m_SuperDecalList; }
	inline AList * GetBillBoardList() { return &m_BillBoardList; }
	inline AList * GetPlayEventList() { return &m_PlayEventList; }

	inline bool IsExpired() { return m_bExpired; }
	inline bool IsDead() { return m_bExpired & m_bDieOnExpired; }

	inline bool GetLinked() { return m_bLinked; }
	inline void SetLinked(bool bLinked) { m_bLinked = bLinked; }
	inline A3DFrame * GetParentFrame() { return m_pParentFrame; }
	inline void SetParentFrame(A3DFrame * pParentFrame) { m_pParentFrame = pParentFrame; }
	inline A3DModel * GetHostModel() { return m_pHostModel; }
	inline void SetHostModel(A3DModel * pHostModel) { m_pHostModel = pHostModel; }

	bool SetScale(FLOAT vScale);
	bool SetScaleAll(FLOAT vScaleX, FLOAT vScaleY, FLOAT vScaleZ);
	inline FLOAT GetScale() { return m_vScale; }

	bool SetLength(FLOAT vLength);
	inline FLOAT GetLength() { return m_vLength; }

	inline bool HasDecal() { return m_SuperDecalList.GetSize() ? true : false; }
	inline FLOAT GetMaxDecalSize() { return m_vMaxDecalSize * m_vScale; }

	inline int GetEverTicks()  { return m_nEverTicks; }
	inline int GetDelayTicks() { return m_nDelayTicks; } 

	inline int GetCategory()				{ return m_Category; }

	inline void SetDrawByMan(bool bFlag)	{ m_bDrawByMan = bFlag; }
	inline bool IsDrawByMan()				{ return m_bDrawByMan; }
};
typedef A3DGraphicsFX * PA3DGraphicsFX;

#endif//_A3DGRAPHICSFX_H_

