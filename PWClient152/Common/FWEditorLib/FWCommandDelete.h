#ifndef _FWCOMMANDDELETE_H_
#define _FWCOMMANDDELETE_H_

#include "FWCommand.h"
#include "FWObPtrList.h"

class FWGlyph;
class FWDoc;

// FWCreateDelete represents the action of 
// deleting a glyph.
// note : the class should just put/remove glyph to/from 
// docutment, but not actually new/delete glyphs, because 
// other command will contain pointers of glyphs, so 
// new/delete glyphs will make undo/redo operate incorrectly
class FWCommandDelete : public FWCommand     
{
public:
	FWCommandDelete(FWDoc *pDoc);
	virtual ~FWCommandDelete();

	virtual void Execute();
	virtual void Unexecute();
private:
	// glyphs to delete
	FWObPtrList<FWGlyph *> m_listGlyph;
	
	// indices of deleted glyphs in original glyphlist
	AList2<int, int> m_listIndex;

	// if the glyphs has been stored into document so
	// that the destructor should not delete them since
	// the document will do it
	bool m_bGlyphUsed;

	FWDoc * m_pDoc;
};

#endif 
