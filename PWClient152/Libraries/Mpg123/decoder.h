#ifndef _DECODER_H_
#define _DECODER_H_

#include "readers.h"

#define NTOM_MUL (32768)

class CReaders;


class CDecoder
{
public:
	CDecoder():step_synth_1to1(2),step_synth_2to1(2),step_synth_4to1(2),step_synth_ntom(2)
	{
		conv16to8_buf = NULL;
		//	pnts[] = { cos64,cos32,cos16,cos8,cos4 };
		pnts[0] = cos64;
		pnts[1] = cos32;
		pnts[2] = cos16;
		pnts[3] = cos8;
		pnts[4] = cos4;
		
		bo_synth_1to1 = 1;
		
		int i,j,m;
		
		for (i=0;i<2;i++)
		{
			for (j=0;j<2;j++)
			{
				for (m=0;m<SBLIMIT*SSLIMIT;m++)
				{
					block[i][j][m] = 0;
				}
			}
		}
		for (i=0;i<2;i++)
		{
			for (j=0;j<2;j++)
			{
				for (m=0;m<0x110;m++)
				{
					buffs_synth_2to1[i][j][m] = 0;
					buffs_synth_4to1[i][j][m] = 0;
					buffs_synth_ntom[i][j][m] = 0;
				}
			}
		}
		blc[0]=0;
		blc[1]=0;
		bo_synth_2to1 = 1;
		bo_synth_4to1 = 1;
		
		itable_init_layer2 = NULL;
		
		ntom_val[0] = NTOM_MUL>>1;
		ntom_val[0] = NTOM_MUL>>1;
		ntom_step = NTOM_MUL;
		bo_synth_ntom = 1;
	}
	~CDecoder()
	{
	}
	void SetReaders(CReaders* rds)
	{
		m_readers = rds;
		m_layer = 0;
	}
	int DoLayer(struct frame *fr,int outMode,CMpg123* player)
	{
		switch(m_layer)
		{
		case 1:
			return do_layer1(fr,outMode,player);
			break;
		case 2:
			return do_layer2(fr,outMode,player);
			break;
		case 3:
			return do_layer3(fr,outMode,player);
		default:
			assert(m_layer !=0);
			break;
		}
		return -1; // error exist.
	}
	int decode_header(struct frame *fr,unsigned long newhead);
	void make_decode_tables(long scaleval);
	void init_layer2(void);
	void init_layer3(int down_sample_sblimit);
	
	int do_layer1(struct frame *fr,int outmode,CMpg123* player);
	int do_layer2(struct frame *fr,int outmode,CMpg123* player);	 
	int do_layer3(struct frame *fr,int outmode,CMpg123* player);
	
	int synth_1to1_8bit(real *bandPtr,int channel,unsigned char *samples,int *pnt);
	int synth_1to1_8bit_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_1to1_8bit_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_1to1_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_1to1_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_1to1(real *bandPtr,int channel,unsigned char *out,int *pnt);
	void dct64_1(real *out0,real *out1,real *b1,real *b2,real *samples);
	void dct64(real *a,real *b,real *c);
	
	int synth_2to1_8bit(real *bandPtr,int channel,unsigned char *samples,int *pnt);
	int synth_2to1_8bit_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_2to1_8bit_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_2to1_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_2to1_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_2to1(real *bandPtr,int channel,unsigned char *out,int *pnt);
	
	int synth_4to1_8bit(real *bandPtr,int channel,unsigned char *samples,int *pnt);
	int synth_4to1_8bit_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_4to1_8bit_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_4to1_mono(real *bandPtr,unsigned char *samples,int *pnt);
	
	void synth_ntom_set_step(long m,long n);
	int synth_ntom_8bit(real *bandPtr,int channel,unsigned char *samples,int *pnt);
	int synth_ntom_8bit_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_ntom_8bit_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_ntom_mono(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_ntom_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	int synth_ntom(real *bandPtr,int channel,unsigned char *out,int *pnt);
	int synth_4to1_mono2stereo(real *bandPtr,unsigned char *samples,int *pnt);
	
	int synth_4to1(real *bandPtr,int channel,unsigned char *out,int *pnt);
	
	void do_equalizerband(real *bandPtr,int channel);
	void do_equalizer(real *bandPtr,int channel);
	
protected:
private:
	
	void dct36(real *inbuf,real *o1,real *o2,real *wintab,real *tsbuf);
	void dct12(real *in,real *rawout1,real *rawout2,register real *wi,register real *ts);
	void I_step_one(unsigned int balloc[], unsigned int scale_index[2][SBLIMIT],struct frame *fr);
	void I_step_two(real fraction[2][SBLIMIT],unsigned int balloc[2*SBLIMIT],unsigned int scale_index[2][SBLIMIT],struct frame *fr);
	
	void II_step_one(unsigned int *bit_alloc,int *scale,struct frame *fr);
	void II_step_two(unsigned int *bit_alloc,real fraction[2][4][SBLIMIT],int *scale,struct frame *fr,int x1);
	void II_select_table(struct frame *fr);
	
	void III_get_side_info(struct III_sideinfo *si,int stereo, int ms_stereo,long sfreq,int single,int lsf);
	int III_get_scale_factors_1(int *scf,struct gr_info_s *gr_info,int ch,int gr);
	int III_get_scale_factors_2(int *scf,struct gr_info_s *gr_info,int i_stereo);
	int III_dequantize_sample(real xr[SBLIMIT][SSLIMIT],int *scf,struct gr_info_s *gr_info,int sfreq,int part2bits);
	void III_i_stereo(real xr_buf[2][SBLIMIT][SSLIMIT],int *scalefac,struct gr_info_s *gr_info,int sfreq,int ms_stereo,int lsf);
	void III_antialias(real xr[SBLIMIT][SSLIMIT],struct gr_info_s *gr_info);
	void III_hybrid(real fsIn[SBLIMIT][SSLIMIT],real tsOut[SSLIMIT][SBLIMIT],int ch,struct gr_info_s *gr_info);
	void III_get_side_info_2(struct III_sideinfo *si,int stereo, int ms_stereo,long sfreq,int single);
private:
	CReaders* m_readers;
	int m_layer;
	
	int longLimit[9][23];
	int shortLimit[9][14];
	real muls[27][64];	/* also used by layer 1 */
	
	real ispow[8207];
	real aa_ca[8],aa_cs[8];
	real COS1[12][6];
	real win[4][36];
	real win1[4][36];
	real gainpow2[256+118+4];
	real COS9[9];
	real COS6_1,COS6_2;
	real tfcos36[9];
	real tfcos12[3];
#define NEW_DCT9
#ifdef NEW_DCT9
	real cos9[3],cos18[3];
#endif
	
	int mapbuf0[9][152];
	int mapbuf1[9][156];
	int mapbuf2[9][44];
	int *map[9][3];
	int *mapend[9][3];
	unsigned int n_slen2[512]; /* MPEG 2.0 slen for 'normal' mode */
	unsigned int i_slen2[256]; /* MPEG 2.0 slen for intensity stereo */
	
	real tan1_1[16],tan2_1[16],tan1_2[16],tan2_2[16];
	real pow1_1[2][16],pow2_1[2][16],pow1_2[2][16],pow2_2[2][16];
	
	const int step_synth_1to1;
	int bo_synth_1to1;
	
	
#ifdef USE_3DNOW
	real decwin[2*(512+32)];
	real cos64[32],cos32[16],cos16[8],cos8[4],cos4[2];
#else
	real decwin[512+32];
	real cos64[16],cos32[8],cos16[4],cos8[2],cos4[1];
#endif
	
	real *pnts[5];// = { cos64,cos32,cos16,cos8,cos4 };
	
	unsigned char *conv16to8_buf;
	unsigned char *conv16to8;
	
	real block[2][2][SBLIMIT*SSLIMIT];
    int blc[2];
	
	real buffs_synth_2to1[2][2][0x110];
	const int step_synth_2to1;
	int bo_synth_2to1 ;
	int *itable_init_layer2;
	unsigned int scfsi_buf_II_step_one[64];
	
	real buffs_synth_4to1[2][2][0x110];
	const int step_synth_4to1 ;
	int bo_synth_4to1 ;
	
	real buffs_synth_ntom[2][2][0x110];
	const int step_synth_ntom;
	int bo_synth_ntom;
	
	unsigned long ntom_val[2] ;
	unsigned long ntom_step ;
	

	real equalizer_sum[2][32];
	int equalizer_cnt;
	
	real equalizerband[2][SBLIMIT*SSLIMIT];
public:
	real buffs[2][2][0x110];
	real equalizer[2][32];
};
#endif