#ifndef _GLYPHLIST_H_
#define _GLYPHLIST_H_

#include "FWTextMesh.h"
#include "FWConfig.h"
#include "FWObPtrList.h"
#include "FWGlyph.h"

class FWGlyph;
class FWGfx;
class FWAssemblySet;
class FWView;
class FWObject;
class FWArchive;

typedef FWObPtrList<FWGlyph *> FWGlyphListBase;

//The order of the element in this class is:
//The closer glyph is to the user, backwarder it will be in the list.
//The FWGlyphList insure the order in the selected list is also the
//same order as the glyph list
class FWGlyphList : public FWGlyphListBase
{
public:

	// find glyph by name
	ALISTPOSITION FindByName(LPCTSTR szName);

	// the format of name is "Ð§¹û" + itoa(index)
	// this function will generate a name whose index
	// is 1 greater than the largest index of all 
	// existed name
	ACString GetNextName() const;

	// property-get/set
	void SetScale(float fScale);
	float GetScale() const;

	// covnert glyphs to gfx
	// note : client should call SetCenterPoint() before
	// call this function for m_ptCenter will be used
	// in this function
	bool FillGfxSet(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime) const;

	// Remove all pointers and delete entities 
	// from heap
	void DeleteAll();
	
	// Remove select pointer and delete the 
	// entity from heap
	void DeleteGlyph(ALISTPOSITION pos);
	
	// Return the item which should be hit.
	// The backer the item in the list, the 
	// superior it will be in the hit selecting.
	FWGlyph* GetHitItem(APointI point);
	ALISTPOSITION GetHitItemPosition(APointI point);

#ifdef _FW_EDITOR
	// Draw all the glyphes onto the screen.
	// Not draw the handles
	void Draw(FWView *pView);
	// Draw the handles of the selected object.
	void DrawHandles(FWView *pView);
#endif
	// Get the minimum rect which can contain all 
	// the glyphes in the list.
	ARectI GetTrueRect() const;

	// serialization
	void Serialize(FWArchive& ar);	
	
	
	// base class interface
	// note : the base class have no virtual functions
	// so be carefull, never call such functions using 
	// a base class type pointer/reference!
	virtual void RemoveAt(ALISTPOSITION pos);
	virtual FWGlyph* RemoveTail();
	virtual FWGlyph* RemoveHead();
	virtual void RemoveAll();
	virtual ALISTPOSITION AddTail(FWGlyph *pGlyph);
	


	// sellist interface
	const AList2<ALISTPOSITION, ALISTPOSITION&> * GetSelList() const { return &m_selList; }
	ALISTPOSITION GetSelHeadPosition() const;
	ALISTPOSITION GetSelTailPosition() const;
	FWGlyph* GetSelNext(ALISTPOSITION& rPosition) const;
	FWGlyph*& GetSelNext(ALISTPOSITION& rPosition);
	FWGlyph* GetSelPrev(ALISTPOSITION& rPosition) const;
	FWGlyph*& GetSelPrev(ALISTPOSITION& rPosition);
	int GetSelCount() const	{ return m_selList.GetCount(); }
	FWGlyph* GetSelAt(ALISTPOSITION position) const { return GetAt(m_selList.GetAt(position)); }
	FWGlyph*& GetSelAt(ALISTPOSITION position) { return GetAt(m_selList.GetAt(position)); }
	FWGlyph* GetSelHead() const { return GetAt(m_selList.GetHead()); }
	FWGlyph*& GetSelHead() { return GetAt(m_selList.GetHead()); }
	FWGlyph* GetSelTail() const	{ return GetAt(m_selList.GetTail()); }
	FWGlyph* GetSelTail() { return GetAt(m_selList.GetTail()); }
	ALISTPOSITION GetSelPosition(ALISTPOSITION glyphPos, ALISTPOSITION selStartAfter = NULL) { return m_selList.Find(glyphPos, selStartAfter); }

	// sel operation
	ARectI GetSelTrueRect() const;
	void EmptySel();
	void AddSel(ALISTPOSITION pos);
	void RemoveSel(ALISTPOSITION pos);
	void RemoveAllSel();
	void InvertSel(ALISTPOSITION pos);
	void SelAll();

	FWGlyphList();
	virtual ~FWGlyphList();
private:
	// this value is used to convert glyph from screen coordinate
	// to game world coordinate when prepare firework
	float m_fScale;

	AList2<ALISTPOSITION, ALISTPOSITION&>	m_selList;
};

#endif 