#ifndef _FWGLYPHBEZIER_H_
#define _FWGLYPHBEZIER_H_

#include "FWGlyphGraphBase.h"
#include "FWArray.h"

typedef FWArray<APointI, APointI&> FWPointArray;
typedef FWArray<int, int &> FWIntArray;

// FWGlyphBezier represents a closed bezier curve
// 
// the closed curve has at least two groups of handles.
// each group have three handle, the secondary one of which is called 
// main handle. 
// 
// every two adjacent groups of handles can form a cubic bezier curve
// using 4 handles as its control point which are 
// 1. main handle of the first group
// 2. the third handle of the first group
// 3. the first handle of the second group
// 4. the main handle of the second group
// 
// by using the first group as last group, we can close
// the bezier line. the sequence is like this :
// group0 <-> group1 <-> group2 <-> groupN <-> group0
// so N groups can form N segments of bezier curve
// 
// the closed bezier support four modes of corner 
// (the "corner" here means the joint of each segment of bezier curve,
// it should at the position of each main handle):
// 1. MODE_SHARP 
// automatically sharp the corner, user can only see the main handle
// 2. MODE_SMOOTH 
// automatically smooth the coner, user can only see the main handle
// 3. MODE_BEZIER 
// semiaotumatically smooth the corner, user can see all handles, when user moves 
// handles, the program will keep the three handles of a group to be in a line
// and keep the length from first handl to main handle and the length
// from main handle to third handle in proportion
// 4. MODE_BEZIER_CORNER
// user can see all handles, and move them freely

class FWGlyphBezier : public FWGlyphGraphBase
{
	FW_DECLARE_SERIAL(FWGlyphBezier);
	friend class FWStateCreateBezier;
protected:
	// indicate whether to adjust adjacent handle group
	bool m_bModifyBrother;
	// all handles, every three handle form a group logically
	FWPointArray m_aryHandle;
	// handles when angle is not 0
	FWPointArray m_aryHandleRotated;
	// sizerect
	ARectI m_SizeRect;
	// rotated sizerect
	ARectI m_SizeRectRotated;
	// backup when a move handle start
	APointI m_PointMoveHandleBak1, m_PointMoveHandleBak2;
	// mode for each handle group, see enum MODE_xxx
	FWIntArray m_aryMode;
	// change a single handle's position
	void SetHandle(int nHandle, APointI point);
	void UpdateFromRotatedData();
public:
	FWGlyphBezier();
	virtual ~FWGlyphBezier();
	enum {MODE_SHARP, MODE_SMOOTH, MODE_BEZIER, MODE_BEZIER_CORNER};
public:

	// get the main index of the group that 
	// nHandle belongs to
	int ConvertToMainIndex(int nHandle) const;
	
	// get the index of m_aryMode by a handle mode
	// which is infact the group index
	int ConvertToModeIndex(int nHandle) const;

	// get mode of the graoup that nHandle belongs to
	int GetMode(int nHandle) const;

	// get mode of the graoup that nHandle belongs to
	void SetMode(int nHandle, int nMode);

	// set all group's mode to be same as nMode
	void SetAllMode(int nMode);

	// determine whether the nIndex is exactly the
	// main index of the group it belongs to
	bool IsMainHandle(int nIndex) const;

	// recalculate the size rect
	void ComputeSizeRect();

public:
	// init m_aryMode same specified value
	void InitModeArray(int nMode = MODE_SMOOTH);

	// base class interface
	virtual ARectI GetHandleRect(int nIndex) const;
	virtual CCharOutline * CreateOutline() const;
	virtual void StartMoveHandleInState(int nHandle);
	virtual void EndMoveHandleInState(int nHandle);
	virtual void SetCentralPoint(APointI point);
	virtual int GetHitHandle(APointI point) const;
	virtual void SetAngle(double angle);
	virtual void MoveHandleTo(int nHandle, APointI newPoint);
	virtual ARectI GetSizeRect() const;
	virtual void SetSizeRect(ARectI rect);
	virtual ARectI GetTrueRect() const;
	virtual int GetHandleCount() const;
	virtual APointI GetHandle(int nPoint) const;
	virtual bool IsHit(const APointI& point) const;
	virtual void Serialize(FWArchive &ar);
	virtual void RemoveHandle(int nHandle, APointArray *pArrayHandleDeleted = NULL, void **ppDataOut = NULL);
	virtual void DuplicateHandle(int nHandle);
	virtual void InsertHandleBefore(int nHandle, const APointArray &aryHandle, void *pDataIn = NULL);
#ifdef _FW_EDITOR
	virtual void DrawHandles(FWView *pView) const;
	virtual const char * GetContextMenu();
#endif
};

#endif 