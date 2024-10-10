
#include "mpg123.h"


void CDecoder::do_equalizerband(real *bandPtr,int channel)
{
  int i;
  for(i=0;i<576;i++) {
    bandPtr[i] *= equalizerband[channel][i];
  }
}

void CDecoder::do_equalizer(real *bandPtr,int channel) 
{
	/*do_equalizerband(bandPtr, channel);
	return;*/
	int i;

	if(equalfile) {
		for(i=0;i<32;i++)
			bandPtr[i] *= equalizer[channel][i];
	} 

/*	if(param.equalizer & 0x2) {
		
		for(i=0;i<32;i++)
			equalizer_sum[channel][i] += bandPtr[i];
	}
*/
}

