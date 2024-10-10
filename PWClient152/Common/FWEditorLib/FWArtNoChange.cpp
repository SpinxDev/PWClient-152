// FWArtNoChange.cpp: implementation of the FWArtNoChange class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtNoChange.h"
#include "FWArchive.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWArtNoChange, FWArt)

FWArtNoChange::FWArtNoChange()
{

}

FWArtNoChange::~FWArtNoChange()
{

}

FWArtNoChange & FWArtNoChange::operator = (const FWArtNoChange & src)
{
	return *this;
}
