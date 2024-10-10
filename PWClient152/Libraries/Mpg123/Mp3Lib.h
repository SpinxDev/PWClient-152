#ifndef __MPG123LIB_H__
#define __MPG123LIB_H__

#include <windows.h>
#include <stdio.h>
#include <mmreg.h>

class CReaders;
class CDecoder;

class CMpg123
{
public:
	// 外部接口 API
	int mpg123_open(char * szfilename);
	int mpg123_close();
	int mpg123_seek(int nSample);
	int mpg123_read(char * buffer, long size, long * preadsize, int * reachend);
	int mpg123_get_total_sample();
	int mpg123_getinfo(long *, int *);


	// 以下 API 仅供内部使用
public:	
	CMpg123();
	~CMpg123();
	CReaders* GetReaders() { return m_readers; 	}// just for internal use.

private:
	int mpg123_init();
	int mpg123_release();
	int get_songlen(struct frame *fr,int no);
	long GetFileOffset() const;
	int play_frame(int init,struct frame *fr);
	int mpg123_reset();
	int mpg123_play(char * buffer, int size);
	double compute_tpf(struct frame *fr);
	double compute_bpf(struct frame *fr);

private:
	CDecoder* m_decoder;
	CReaders* m_readers;
	int  stereo;
	int frequency;
public:
	unsigned char *pcm_sample;
	int pcm_point;
};
#endif