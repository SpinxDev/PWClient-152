#ifndef _FWTEMPLATE_H
#define _FWTEMPLATE_H

#include <A3DMatrix.h>
#include <AAssist.h>

class FWAssemblySet;
class FWArchive;
class FWGlyphFirework;
class FWGlyphList;
class A3DDevice;
class AFile;

#define STYLE_NORMAL	0x00000000
#define STYLE_BOLD		0x00000001
#define STYLE_ITALIC	0x00000010

class FWTemplate
{
	friend class FWAssemblySet;
protected:
	// list of glyphs
	FWGlyphList * m_pGlyphList;

	// lanch is so special that we do not put it in glyphlist
	FWGlyphFirework * m_pGlyphLanch; 

	A3DMATRIX4 m_matParentTMLanch;
	A3DMATRIX4 m_matParentTMMain;
public:
	FWTemplate();
	~FWTemplate();

	// load glyphs from file
	virtual bool LoadFrom(AFile *pFile);
	
	// save glyphs to file
	virtual bool SaveTo(AFile *pFile);

	// the following functions must be called after
	// a successful call to LoadFrom()
	void SetText(const ACHAR * szText);
	void SetFontName(const ACHAR * szFontName);
	void SetPointSize(int nPoint);
	void SetFontStyle(int nStyle); // see STYLE_xxx above

	float GetBlastHeight();
	void SetParentTM(const A3DMATRIX4& matLanch, const A3DMATRIX4& matMain);

protected:
	// serialize
	virtual void Serialize(FWArchive& ar);	
};


bool AfxInitFWEditor(A3DDevice *pDevice);
void AfxReleaseFWEditor();

#endif
