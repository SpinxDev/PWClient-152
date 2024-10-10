/*
 * mpg123 defines 
 * used source: musicout.h from mpegaudio package
 */
#ifndef _MPG123_H_
#define _MPG123_H_


#include        <stdio.h>
#include        <string.h>
#include		<stdlib.h>
#include        <math.h>


class AFile;
class CMpg123;


typedef unsigned char byte;

#ifdef OS2
#include <float.h>
#endif

#define MPG123_REMOTE
#ifdef HPUX
#define random rand
#define srandom srand
#endif

#define FRONTEND_SAJBER 1
#define FRONTEND_TK3PLAY 2

#define SKIP_JUNK 1

#ifdef _WIN32	/* Win32 Additions By Tony Million */
# undef WIN32
# define WIN32

# define M_PI       3.14159265358979323846
# define M_SQRT2	1.41421356237309504880
# define REAL_IS_FLOAT
# define NEW_DCT9

# define STDIN_FILENO	0

# define random rand
# define srandom srand

# define strcasecmp strcmp	/* It'll do for now */

# undef MPG123_REMOTE           /* Get rid of this stuff for Win32 */
#endif

#ifdef REAL_IS_FLOAT
#  define real float
#elif defined(REAL_IS_LONG_DOUBLE)
#  define real long double
#else
#  define real double
#endif

#if defined(__GNUC__) || defined(WIN32)
#define INLINE inline
#else
#define INLINE
#endif

/* AUDIOBUFSIZE = n*64 with n=1,2,3 ...  */
#define		AUDIOBUFSIZE		16384

#define         FALSE                   0
#define         TRUE                    1

#define         MAX_NAME_SIZE           81
#define         SBLIMIT                 32
#define         SCALE_BLOCK             12
#define         SSLIMIT                 18

#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3

#define MAXOUTBURST 32768

/* Pre Shift fo 16 to 8 bit converter table */
#define AUSHIFT (3)

class CDecoder;

struct al_table 
{
  short bits;
  short d;
};
struct frame {
    struct al_table *alloc;
    int (CDecoder::*synth)(real *,int,unsigned char *,int *);
    int (CDecoder::*synth_mono)(real *,unsigned char *,int *);
    int stereo;
    int jsbound;
    int single;
    int II_sblimit;
    int down_sample_sblimit;
    int lsf;
    int mpeg25;
    int down_sample;
    int header_change;
    int lay;
    int (*do_layer)(struct frame *fr,int,CMpg123*);  // will not use it for wyd!
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */
};

struct parameter {
  int aggressive; /* renice to max. priority */
  int shuffle;	/* shuffle/random play */
  int remote;	/* remote operation */
  int outmode;	/* where to out the decoded sampels */
  int quiet;	/* shut up! */
  long usebuffer;	/* second level buffer size */
  int tryresync;  /* resync stream after error */
  int verbose;    /* verbose level */
#ifdef TERM_CONTROL
  int term_ctrl;
#endif
  int force_mono;
  int force_stereo;
  int force_8bit;
  long force_rate;
  int down_sample;
  int checkrange;
  long doublespeed;
  long halfspeed;
  int force_reopen;
  long realtime;
  char filename[256];
};

#define READER_FD_OPENED 0x1
#define READER_ID3TAG    0x2


extern int equalfile;
extern int halfspeed;
/* ------ Declarations from "common.c" ------ */

extern void audio_flush(int, struct audio_info_struct *);
extern void (*catchsignal(int signum, void(*handler)()))();

extern void print_header(struct frame *);
extern void print_header_compact(struct frame *);
extern void print_id3_tag(unsigned char *buf);

extern int split_dir_file(const char *path, char **dname, char **fname);


extern int audiobufsize;

extern int OutputDescriptor;

#ifdef VARMODESUPPORT
extern int varmode;
extern int playlimit;
#endif

struct gr_info_s {
      int scfsi;
      unsigned part2_3_length;
      unsigned big_values;
      unsigned scalefac_compress;
      unsigned block_type;
      unsigned mixed_block_flag;
      unsigned table_select[3];
      unsigned subblock_gain[3];
      unsigned maxband[3];
      unsigned maxbandl;
      unsigned maxb;
      unsigned region1start;
      unsigned region2start;
      unsigned preflag;
      unsigned scalefac_scale;
      unsigned count1table_select;
      real *full_gain[3];
      real *pow2gain;
};

struct III_sideinfo
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct {
    struct gr_info_s gr[2];
  } ch[2];
};

//extern int open_stream(char *,int fd);
//extern int read_frame_init (void);
//extern int read_frame(struct frame *fr);
//extern int play_frame(int init,struct frame *fr);
//extern int do_layer3(struct frame *fr,int,CMpg123*);
//extern int do_layer2(struct frame *fr,int,CMpg123*);
//extern int do_layer1(struct frame *fr,int,CMpg123*);
/*
extern void do_equalizer(real *bandPtr,int channel);

#ifdef PENTIUM_OPT
extern int synth_1to1_pent (real *,int,unsigned char *);
#endif
#ifdef USE_3DNOW
extern int synth_1to1_3dnow (real *,int,unsigned char *);
#endif
extern int synth_1to1 (real *,int,unsigned char *,int *);
extern int synth_1to1_8bit (real *,int,unsigned char *,int *);
extern int synth_1to1_mono (real *,unsigned char *,int *);
extern int synth_1to1_mono2stereo (real *,unsigned char *,int *);
extern int synth_1to1_8bit_mono (real *,unsigned char *,int *);
extern int synth_1to1_8bit_mono2stereo (real *,unsigned char *,int *);

extern int synth_2to1 (real *,int,unsigned char *,int *);
extern int synth_2to1_8bit (real *,int,unsigned char *,int *);
extern int synth_2to1_mono (real *,unsigned char *,int *);
extern int synth_2to1_mono2stereo (real *,unsigned char *,int *);
extern int synth_2to1_8bit_mono (real *,unsigned char *,int *);
extern int synth_2to1_8bit_mono2stereo (real *,unsigned char *,int *);

extern int synth_4to1 (real *,int,unsigned char *,int *);
extern int synth_4to1_8bit (real *,int,unsigned char *,int *);
extern int synth_4to1_mono (real *,unsigned char *,int *);
extern int synth_4to1_mono2stereo (real *,unsigned char *,int *);
extern int synth_4to1_8bit_mono (real *,unsigned char *,int *);
extern int synth_4to1_8bit_mono2stereo (real *,unsigned char *,int *);

extern int synth_ntom (real *,int,unsigned char *,int *);
extern int synth_ntom_8bit (real *,int,unsigned char *,int *);
extern int synth_ntom_mono (real *,unsigned char *,int *);
extern int synth_ntom_mono2stereo (real *,unsigned char *,int *);
extern int synth_ntom_8bit_mono (real *,unsigned char *,int *);
extern int synth_ntom_8bit_mono2stereo (real *,unsigned char *,int *);
*/
//extern void rewindNbits(int bits);
//extern int  hsstell(void);
//extern void set_pointer(long);
//extern void huffman_decoder(int ,int *);
//extern void huffman_count1(int,int *);
//extern void print_stat(struct frame *fr,int no,long buffsize,struct audio_info_struct *ai);
//extern int get_songlen(struct frame *fr,int no);

//extern void init_layer3(int);
//extern void init_layer2(void);
//extern void make_decode_tables(long scale);
/*
extern void make_conv16to8_table(int);
extern void dct64(real *,real *,real *);

extern void synth_ntom_set_step(long,long);
*/
//extern int au_open(struct audio_info_struct *ai, char *aufilename);
//extern int au_close(void);

//extern int cdr_open(struct audio_info_struct *ai, char *cdrfilename);
//extern int cdr_close(void);

//extern unsigned char *conv16to8;
extern long freqs[9];
//extern real muls[27][64];
/*
#ifdef USE_3DNOW
extern real decwin[2*(512+32)];
#else
extern real decwin[512+32];
#endif
extern real *pnts[5];

extern real equalizer[2][32];
extern real equalizer_sum[2][32];
extern int equalizer_cnt;
*/
//extern struct audio_name audio_val2name[];

extern struct parameter param;

/* 486 optimizations */
#define FIR_BUFFER_SIZE  128
//extern void dct64_486(int *a,int *b,real *c);
//extern int synth_1to1_486(real *bandPtr,int channel,unsigned char *out,int nb_blocks);


#include "readers.h"
#include "decoder.h"



#endif