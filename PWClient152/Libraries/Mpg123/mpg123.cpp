/* 
 * Mpeg Audio Player (see version.h for version number)
 * ------------------------
 * copyright (c) 1995,1996,1997,1998,1999 by Michael Hipp, All rights reserved.
 * See also 'README' !
 *
 */

#include <stdlib.h>
#include <io.h>
#include <AFile.h>

#include "mpg123.h"
#include "Mp3Lib.h"

static void print_title(void);
//extern int a_lseek(AFile * fd, int offset, int origin);

struct parameter param = {
  FALSE , /* aggressiv */
  FALSE , /* shuffle */
  FALSE , /* remote */
  0 , /* write samples to audio device */
  FALSE , /* silent operation */
  0 ,     /* second level buffer size */
  TRUE ,  /* resync after stream error */
  0 ,     /* verbose level */
#ifdef TERM_CONTROL
  FALSE , /* term control */
#endif
  -1 ,     /* force mono */
  0 ,     /* force stereo */
  0 ,     /* force 8bit */
  0 ,     /* force rate */
  0 , 	  /* down sample */
  FALSE , /* checkrange */
  0 ,	  /* doublespeed */
  0 ,	  /* halfspeed */
  0 ,	  /* force_reopen, always (re)opens audio device for next song */
  FALSE,  /* try to run process in 'realtime mode' */   
  { 0,},  /* wav,cdr,au Filename */
};

long outscale  = 32768;

//unsigned long startFrame= 0;

int  equalfile = 1;

//int  stereo = 1;
//int  frequency = 44100;

//static int intflag = FALSE;

int OutputDescriptor;

//struct frame fr;
#define FRAMEBUFUNIT (18 * 64 * 4)

void set_synth_functions(struct frame *fr);

/*
 * play a frame read by read_frame();
 * (re)initialize audio if necessary.
 *
 */
int CMpg123::play_frame(int init,struct frame *fr)
{
	int clip;
	
	if (fr->error_protection)
	{
		GetReaders()->getbits(16); /* skip crc */
	}

	/* do the decoding */
//	clip = (fr->do_layer)(fr, param.outmode,this);
	clip = m_decoder->DoLayer(fr,param.outmode,this); // wyd;
	
	return 1;
}

void set_synth_functions(struct frame *fr)
{
	typedef int (CDecoder::*func)(real *,int,unsigned char *,int *);
	typedef int (CDecoder::*func_mono)(real *,unsigned char *,int *);
	int ds = fr->down_sample;
	int p8=0;

	static func funcs[2][4] = { 
		{ &CDecoder::synth_1to1,
		  &CDecoder::synth_2to1,
		  &CDecoder::synth_4to1,
		  &CDecoder::synth_ntom } ,
		{ &CDecoder::synth_1to1_8bit,
		  &CDecoder::synth_2to1_8bit,
		  &CDecoder::synth_4to1_8bit,
		  &CDecoder::synth_ntom_8bit } 
	};

	static func_mono funcs_mono[2][2][4] = {    
		{ { &CDecoder::synth_1to1_mono2stereo ,
		    &CDecoder::synth_2to1_mono2stereo ,
		    &CDecoder::synth_4to1_mono2stereo ,
		    &CDecoder::synth_ntom_mono2stereo } ,
		  { &CDecoder::synth_1to1_8bit_mono2stereo ,
		    &CDecoder::synth_2to1_8bit_mono2stereo ,
		    &CDecoder::synth_4to1_8bit_mono2stereo ,
		    &CDecoder::synth_ntom_8bit_mono2stereo } } ,
		{ { &CDecoder::synth_1to1_mono ,
		    &CDecoder::synth_2to1_mono ,
		    &CDecoder::synth_4to1_mono ,
		    &CDecoder::synth_ntom_mono } ,
		  { &CDecoder::synth_1to1_8bit_mono ,
		    &CDecoder::synth_2to1_8bit_mono ,
		    &CDecoder::synth_4to1_8bit_mono ,
		    &CDecoder::synth_ntom_8bit_mono } }
	};

	fr->synth = funcs[p8][ds];
	fr->synth_mono = funcs_mono[param.force_stereo?0:1][p8][ds];
}

int CMpg123::mpg123_init()
{
	//Init Audio, should be get rid of;
	if (!(pcm_sample = (unsigned char *) malloc(audiobufsize * 2 + 1024))) 
	{
		return 0;
	}

	memset(m_readers->GetFR(), 0, sizeof(struct frame));
	m_readers->GetFR()->single = -1;
	m_readers->GetFR()->down_sample_sblimit = SBLIMIT;

	//Initialize equalizer
	if(equalfile) 
	{
		int i;
		for(i=0;i<32;i++) 
		{
			m_decoder->equalizer[0][i] = m_decoder->equalizer[1][i] = 1.0;
		}
	}

	set_synth_functions(m_readers->GetFR());
	m_decoder->make_decode_tables(outscale);
	m_decoder->init_layer2(); /* inits also shared tables with layer1 */
	m_decoder->init_layer3(m_readers->GetFR()->down_sample_sblimit);
	return 1;
}

int CMpg123::mpg123_reset()
{
	m_readers->read_frame_init();

	return 1;
}

 int CMpg123::mpg123_open(char * szfilename)
{
	if( !mpg123_init() )
		return 0;

	if( !m_readers->open_stream(szfilename,-1) )
		return 0;

	//Now we can use fr, numframes and sample_per_frame;
	stereo = (m_readers->GetFR()->mode == 3 ? 0 : 1);
	frequency = freqs[m_readers->GetFR()->sampling_frequency];

	mpg123_reset();
	return 1;
}

 int CMpg123::mpg123_release()
{
	if( pcm_sample )
	{
		free(pcm_sample);
		pcm_sample = NULL;
	}
	return 1;
}

 int CMpg123::mpg123_close()
{
	if( m_readers && m_readers->GetReader() && m_readers->GetReader()->filept )
	{
		m_readers->GetReader()->filept->Close();
		delete m_readers->GetReader()->filept;
		m_readers->GetReader()->filept = NULL;
	}

	mpg123_release();

	return 1;
}

 int CMpg123::mpg123_seek(int nSample)
{
	int nFrame = nSample / m_readers->GetSamplePerFrame();

	if( (unsigned long)nFrame + 4 >= m_readers->GetNumFrames() )  
	{
		m_readers->a_lseek(m_readers->GetReader()->filept, 0, SEEK_END);
		return 1;
	}

	m_readers->a_lseek(m_readers->GetReader()->filept, GetFileOffset() + nFrame * (m_readers->GetFR()->framesize + 4), SEEK_SET);

	m_readers->read_frame_init();

	m_readers->read_frame(m_readers->GetFR());
	play_frame(1, m_readers->GetFR());
	m_readers->read_frame(m_readers->GetFR());
	play_frame(1, m_readers->GetFR());
	m_readers->read_frame(m_readers->GetFR());
	play_frame(1, m_readers->GetFR());

	pcm_point = 0;

	return 1;
}

 int CMpg123::mpg123_read(char * buffer, long size, long * preadsize, int * reachend)
{
	int readSize;

	if( size == 0 )
		return 0;

	*preadsize = 0;
	while(size > 0)
	{
		readSize = 0;
		if( pcm_point > 0 )
		{
			readSize = size > pcm_point ? pcm_point : size;
			memcpy(buffer, pcm_sample, readSize);
			size -= readSize;
			pcm_point -= readSize;
			buffer += readSize;
			*preadsize += readSize;
		}

		if( size == 0 )
		{
			//now buffer is enough, we need just shift it;
			int i=0;
			for(i=0; i<pcm_point; i++)
			{
				pcm_sample[i] = pcm_sample[i + readSize];
			}
			break;
		}
		else
		{
			//Need to decode one more buffer;
			if( 0 == m_readers->read_frame(m_readers->GetFR()) )
			{
				*reachend = 1;
				return 1;
			}
			play_frame(1, m_readers->GetFR());
			if( pcm_point <= 0 )
			{
				int nPos = m_readers->GetReader()->filept->GetPos();
				if( nPos - GetFileOffset() >= m_readers->GetReader()->filelen - 1 ) 
					*reachend = 1;
				else
					*reachend = 0;
				return 1;
			}
		}
	}
							   
	*reachend = 0;
	return 1;
}

 int CMpg123::mpg123_get_total_sample()
{
	return m_readers->GetNumFrames() * m_readers->GetSamplePerFrame();
}

 int CMpg123::mpg123_getinfo(long * __frequency, int * __stereo)
{
	*__frequency = frequency;
	*__stereo = stereo;

	return 1;
}

 int CMpg123::mpg123_play(char * buffer, int size)
{
	return 1;
}

static void print_title(void)
{
    //fprintf(stderr,"High Performance MPEG 1.0/2.0/2.5 Audio Player for Layer 1, 2 and 3.\n");
    //fprintf(stderr,"Version %s (%s). Written and copyrights by Michael Hipp.\n", prgVersion, prgDate);
    //fprintf(stderr,"Uses code from various people. See 'README' for more!\n");
    //fprintf(stderr,"THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! USE AT YOUR OWN RISK!\n");
}

CMpg123::CMpg123()
{
	pcm_sample = NULL;
	pcm_point = 0;

	stereo = 1;
	frequency = 44100;
		
	m_readers = new CReaders();
	m_decoder = new CDecoder();
	m_decoder->SetReaders(m_readers);
	m_readers->SetDecoder(m_decoder);
}
CMpg123::~CMpg123()
{
	mpg123_close();
	delete m_decoder;
	delete m_readers;
}