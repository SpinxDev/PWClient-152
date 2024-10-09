#ifndef _FWSHAREDFILE_H_
#define _FWSHAREDFILE_H_

#include <AFile.h>

// similar to AMemFile, but its memory is allocated by ::GlobalAlloc
// note : do not access interface of AFile that are not listed
// in FWSharedFile
class FWSharedFile : public AFile
{
	DWORD m_dwAllocFlags;	// flag used to alloc memory
	HGLOBAL m_hMem;			// handle to memory block
	DWORD m_dwMemSize;		// size of memory block
	DWORD m_dwGrowBytes;	// bytes to grow
	bool m_bAllowGrow;		// enable/disable grow
	DWORD m_dwOffset;		// file pointer
public:
	FWSharedFile(
		DWORD dwGrowBytes = 4096,
		DWORD dwAllocFlags = GMEM_DDESHARE | GMEM_MOVEABLE);
	virtual ~FWSharedFile();

	HGLOBAL Detach();
	// this function will be always successful
	// even if the release operation on old memory block is failed
	void SetHandle( HGLOBAL hGlobalMemory, bool bAllowGrow = true );

	virtual bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength);
	virtual bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength);

	virtual DWORD GetPos()  { return m_dwOffset; }
	virtual bool ResetPointer() { return Seek(0, AFILE_SEEK_SET); }
	virtual bool Seek(int iOffset, int iOrigin);
protected:
	bool Alloc(DWORD dwBytes);
	bool Free();
	bool Realloc(DWORD dwBytes);
	bool IsMemAvailable();
};

#endif 