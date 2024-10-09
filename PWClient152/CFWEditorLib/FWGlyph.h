#ifndef _FWGLYPH_H
#define _FWGLYPH_H


#include "FWConfig.h"
#include "FWObject.h"
#include <AList2.h>
#include <AArray.h>
#include <APoint.h>
#include <ARect.h>
#include <AString.h>
#include <AAssist.h>

class CCharOutline;
struct MeshParam;
class FWFirework;
class FWAssemblyProfile;
class FWGfx;
class FWGlyphList;
class FWDialogPropBase;
class FWAssemblySet;
class FWView;
typedef AArray<APointI, APointI&> APointArray;
class A3DVECTOR3;
class A3DMATRIX4;

// The base of all graphic elements.
// all member stored in glyph are in logic coordinate
class FWGlyph : public FWObject  
{
	friend class FWGlyphList;
	friend class FWGlyphComposite;
	FW_DECLARE_DYNAMIC(FWGlyph)
protected:
	// data to be drawed on board
	MeshParam * m_pMeshParam;

	// inner firework object
	FWFirework * m_pFirework;

	// name used for user to identify different glyph
	ACString m_strName;


	double m_fAngle;
	
protected:

#ifdef _FW_EDITOR
	// draw a rect as the handle style
	void DrawHandleRect(ARectI rect, FWView *pView) const;
#endif

	// helper for CopyTo() to update firework data
	// before copy its content to final gfx
	virtual bool PrepareFireworkData(float fScale){ return true; }
public:
	FWGlyph();
	virtual ~FWGlyph();
	
	static const int DEFAULT_HANDLE_RADIUS;
	


	// some property-get/set
	const ACString & GetName() const { return m_strName; }
	void SetName(LPCTSTR szName) { m_strName = szName; }
	FWFirework * GetFirework() { return m_pFirework; }
	const FWFirework * GetFirework() const { return m_pFirework; }

	// insert handle before nHandle
	// see RemoveHandle() for parameter info
	virtual void InsertHandleBefore(int nHandle, const APointArray &aryHandle, void *pDataIn = NULL){}

	// remove specified handle
	// pArrayHandleDeleted is an array of handles that is deleted by the function
	// pDataOut are some info about deleted handle, caller is reponsebile of deleting it
	virtual void RemoveHandle(int nHandle, APointArray *pArrayHandleDeleted = NULL, void **ppDataOut = NULL) {}
	
	// copy specified handle and insert it right before nHandle
	virtual void DuplicateHandle(int nHandle) {}

	// return used partical count
	int GetParticalCount();
	// return used sound count
	int GetSoundCount();

	
	// if the glyph can be combined with other glyph
	virtual bool IsCombinable() { return false; }

	// init
	virtual void Init(FWFirework * pFirework, ACString strName);

	// helper to do the move handle and rotate operation
	virtual void StartMoveHandleInState(int nHandle) {}
	virtual void EndMoveHandleInState(int nHandle) {}
	virtual void StartRotateInState() {}
	virtual void EndRotateInState() {}

	// prepare mesh that will be used in default Draw()
	virtual void GenerateMeshParam();

	// called by default GenerateMeshParam()
	// meshparam will be generated according to the outline
	virtual CCharOutline * CreateOutline() const = 0;

	// This protected member is called by the 
	// default implement of DrawHandles() and 
	// GetHitHandle(). It provides the rect of 
	// the handle of the index.
	// The index is 1 based.
	virtual ARectI GetHandleRect(int nIndex) const;
	
	// When one of the handles is moved to a new 
	// position, the function is called.
	// The glyph implier should overrides the 
	// function to deal with this movement.
	// The index is 1 based.
	virtual void MoveHandleTo(int nHandle, APointI newPoint) = 0;

	// Get the cursor to display to the user when 
	// the mouse is on one of the handles.
	// nIndex is the index of the handle.
	// The index is 1 based.
	virtual HCURSOR GetHandleCursor(int nIndex) const;

	// Size rectangle is a special rectangle 
	// which defines the outline of the 
	// rectangle. It is not necessary corve all 
	// areas of the glyph, but always most of 
	// them. A size rectangle is used to give 
	// user a direct impression about the outline 
	// of the glyph, and is often used to adjust 
	// the position and size of the glyph.
	// This function return the size rect.
	// The size rect will keep constant when the 
	// glyph is rotated. It will always be the 
	// rect 
	// returned when angle = 0
	virtual ARectI GetSizeRect() const = 0;

	// Size rectangle is a special rectangle 
	// which defines the outline of the 
	// rectangle. It is not necessary corve all 
	// areas of the glyph, but always most of 
	// them. A size rectangle is used to give 
	// user a direct impression about the outline 
	// of the glyph, and is often used to adjust 
	// the position and size of the glyph.
	// The glyph will be moved and resized when 
	// SetSizeRect() is called. The offset is 
	// based on the ARectI returned by GetSizeRect
	// The size rect will keep constant when the 
	// glyph is rotated. It will always be the rect 
	// returned when angle = 0
	virtual void SetSizeRect(ARectI rect) = 0;

	// Return the current angle of the glyph.
	// The size rect should be upright when angle = 0
	// The Angle is measured by radian
	virtual double GetAngle() const;

	// Rotate the glyph to a new angle. When rotato, 
	// the centre of the rotation should be the point 
	// returned by GetCentralPoint().
	// NOTE:Parameter "angle" is an absolute angle.
	// The size rect should be upright when angle = 0
	// The Angle is measured by radian
	virtual void SetAngle(double angle);

	// Return the minimum rect the glyph has 
	// covered.
	virtual ARectI GetTrueRect() const = 0;

	// Move the glyph to a new position where its 
	// central point is specified by parameter 
	// "point".
	virtual void SetCentralPoint(APointI point);

	// Test whether the point is on one of the 
	// handles. Return zero if no handle is hit, 
	// else return the index of the handle.
	// The index is 1 based.
	virtual int GetHitHandle(APointI point) const;

	// Return the total number of handles
	virtual int GetHandleCount() const = 0;

	// Get the point of the handle specified by 
	// index.
	// The index is 1 based.
	virtual APointI GetHandle(int nPoint) const = 0;

	// Get the central point of the glyph
	virtual APointI GetCentralPoint() const;

	// Test whecther the point is inside glyph
	virtual bool IsHit(const APointI& point) const = 0;

	// serialization
	// note : the caller is responsible of call SetGlyphList()
	// after call Serialize() to line glyph to the list
	virtual void Serialize(FWArchive &ar);

	// interfaces when used as composite
	virtual ALISTPOSITION GetHeadPosition() { ASSERT(false); return NULL; }
	virtual FWGlyph * GetHead() { ASSERT(false); return NULL; }
	virtual ALISTPOSITION GetTailPosition() { ASSERT(false); return NULL; }
	virtual FWGlyph * GetTail() { ASSERT(false); return NULL; }
	virtual FWGlyph * GetNext(ALISTPOSITION &pos) { ASSERT(false); return NULL; }
	virtual FWGlyph * GetPrev(ALISTPOSITION &pos) { ASSERT(false); return NULL; }
	virtual FWGlyph * GetAt(ALISTPOSITION pos) { ASSERT(false); return NULL; }
	virtual void RemoveAll() { ASSERT(false); }
	virtual ALISTPOSITION Find(FWGlyph *pGlyph, ALISTPOSITION startAfter = NULL) { ASSERT(false); return NULL; }
	virtual ALISTPOSITION AddHead(FWGlyph *pGlyph) { ASSERT(false); return NULL; }
	virtual ALISTPOSITION AddTail(FWGlyph *pGlyph) { ASSERT(false); return NULL; }
	virtual int GetCount() { ASSERT(false); return 0; }
	virtual bool IsEmpty() { ASSERT(false); return true; }
	virtual void DeleteAll() {}


	// proxyed firework interface
	float GetOffsetTime(int nIndex) const;
	const A3DVECTOR3 & GetOffsetPos(int nIndex) const;
	virtual void SetOffsetTime(int nIndex, float fOffsetTime);
	virtual void SetOffsetPos(int nIndex, const A3DVECTOR3 & vOffsetPos);
	const FWAssemblyProfile & GetAssemblyProfile() const;
	void SetAssemblyProfile(const FWAssemblyProfile &src);
	bool CopyTo(FWAssemblySet *pSet, float fScale, const A3DMATRIX4& matTM, float fStartTime);
	A3DVECTOR3 GetSumOfOffsetPos();
	float GetSumOfOffsetTime();

#ifdef _FW_EDITOR
	// Draw this glyph
	virtual void Draw(FWView *pView) const;

	// Draw all the handles onto the screen.
	virtual void DrawHandles(FWView *pView) const;
	
	// part of prop dialog mechenism
	// determine which dialog to show
	virtual const char * GetDlgToShow();
	// part of prop dialog mechenism
	// called before show the dialog
	virtual void PrepareDlg(FWDialogPropBase * pDlg);
	// part of prop dialog mechenism
	// called after the dialog is closed by BTN_OK
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg);


	virtual const char * GetContextMenu();
#endif

};

#endif 
