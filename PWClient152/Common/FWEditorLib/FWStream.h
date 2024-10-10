#ifndef _FWSTREAM_H_
#define _FWSTREAM_H_

#include <A3DStream.h>
#include "CodeTemplate.h"
#include "FWConfig.h"

class A3DDevice;

class FWStream : public A3DStream
{
public:
	FWStream();
	virtual ~FWStream();

	bool Init(A3DDevice* pDevice);
	bool QueryVertBuf(int nVertBufSize);
	bool QueryIndexBuf(int nIndexBufSize);
};

#endif 