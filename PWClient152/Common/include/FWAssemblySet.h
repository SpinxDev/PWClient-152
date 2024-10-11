#ifndef _FWASSEMBLYSET_H_
#define _FWASSEMBLYSET_H_


#include <AList2.h>

class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
class A3DGFXExMan;
typedef APtrList<FWAssembly *> FWAssemblySetBase;
class FWTemplate;

// this class is simply a container of all final gfx
// it has the ownership of all gfx it contains
class FWAssemblySet : public FWAssemblySetBase
{
public:
	FWAssemblySet(int iPoolSize = 10);
	~FWAssemblySet();
	
	// delete all gfx in container
	void DeleteAll();

	// create from template
	bool CreateFromTemplate(FWTemplate *pTemplate);
	
	// render helper
	// each function will simply call the method of same name 
	// for each gfx in the container
	bool Start(bool bForceStop);
	bool Pause();
	void Stop();
	bool TickAnimation(DWORD dwTickTime);
	void RegisterGfx(A3DGFXExMan *pGfxExMan);

	// check state of gfx in container
	bool IsAllAtState(int nState) const;
protected:
	DWORD m_dwTimeStart;
};

#endif 
