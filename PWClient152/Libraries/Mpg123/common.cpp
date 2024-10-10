#include <stdlib.h>
#include <io.h>

#include "mpg123.h"
#include "Mp3Lib.h"
#include "genre.h"
#include "common.h"


//extern int a_lseek(AFile * fd, int offset, int origin);

int tabsel_123[2][3][16] = {
   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };


int audiobufsize = AUDIOBUFSIZE;

#ifdef VARMODESUPPORT
	/*
	 *   This is a dirty hack!  It might burn your PC and kill your cat!
	 *   When in "varmode", specially formatted layer-3 mpeg files are
	 *   expected as input -- it will NOT work with standard mpeg files.
	 *   The reason for this:
	 *   Varmode mpeg files enable my own GUI player to perform fast
	 *   forward and backward functions, and to jump to an arbitrary
	 *   timestamp position within the file.  This would be possible
	 *   with standard mpeg files, too, but it would be a lot harder to
	 *   implement.
	 *   A filter for converting standard mpeg to varmode mpeg is
	 *   available on request, but it's really not useful on its own.
	 *
	 *   Oliver Fromme  <oliver.fromme@heim3.tu-clausthal.de>
	 *   Mon Mar 24 00:04:24 MET 1997
	 */
int varmode = FALSE;
int playlimit;
#endif

//int decode_header(struct frame *fr,unsigned long newhead);

void audio_flush(int outmode, struct audio_info_struct *ai)
{
	return;
}

int CReaders::read_frame_init (void)
{
    oldhead = 0;
    firsthead = 0;

//	memset(buffs, 0, 2 * 2 * 0x110 * sizeof(real));
	memset(m_decoder->buffs, 0, 2 * 2 * 0x110 * sizeof(real));

	return 1;
}

int CReaders::head_check(unsigned long head)
{
    if( (head & 0xffe00000) != 0xffe00000)
		return FALSE;
    if(!((head>>17)&3))
		return FALSE;
    if( ((head>>12)&0xf) == 0xf)
		return FALSE;
    if( ((head>>10)&0x3) == 0x3 )
		return FALSE;
    if ((head & 0xffff0000) == 0xfffe0000)
		return FALSE;

    return TRUE;
}



/*****************************************************************
 * read next frame
 */
int CReaders::read_frame(struct frame *fr)
{
	unsigned long newhead;
//	static unsigned char ssave[34]; // wyd

	fsizeold = fr->framesize;       /* for Layer3 */

	if (param.halfspeed)
	{
//		static int halfphase = 0;
		if (halfphase--)
		{
			bsi.bitindex = 0;
			bsi.wordpointer = (unsigned char *) bsbuf;
			if (fr->lay == 3)
				memcpy (bsbuf, ssave, ssize);
			return 1;
		}
		else
			halfphase = param.halfspeed - 1;
	}

read_again:
	if(!(this->*rd->head_read)(rd, &newhead))
		return 0;

	fr->header_change = 1;
	if( !head_check(newhead) )
	{
		int trynum;

		/*if (!param.quiet)
			fprintf(stderr,"Illegal Audio-MPEG-Header 0x%08lx at offset 0x%lx.\n",
				newhead, rd->tell(rd)-4);
		*/
		/* and those ugly ID3 tags */
		if((newhead & 0xffffff00) == ('T'<<24)+('A'<<16)+('G'<<8)) 
		{
			(this->*rd->skip_bytes)(rd,124);
			//fprintf(stderr,"Skipped ID3 Tag!\n");
			goto read_again;
		}

		trynum = 0;
		/* Read more bytes until we find something that looks
		   reasonably like a valid header.  This is not a
		   perfect strategy, but it should get us back on the
		   track within a short time (and hopefully without
		   too much distortion in the audio output).  */
		do
		{
			trynum++;
			if(!(this->*rd->head_shift)(rd,&newhead))
				return 0;
		} while (!head_check(newhead));
	}

    if(!firsthead)
	{
		unsigned long nexthead;
		if(!m_decoder->decode_header(fr,newhead))
			goto read_again;
		//pos = tell(rd->filept);
		a_lseek(rd->filept, fr->framesize, SEEK_CUR);
		
		(this->*rd->head_read)(rd, &nexthead);
		a_lseek(rd->filept, -(fr->framesize + 4), SEEK_CUR);
		//pos = tell(rd->filept);

		if( !head_check(nexthead) )
		{
			oldhead = 0;
			firsthead = 0;
			goto read_again;
		}
		firsthead = newhead;
    }
    else if(!m_decoder->decode_header(fr,newhead))
		goto read_again;

	/* flip/init buffer for Layer 3 */
	bsbufold = bsbuf;
	bsbuf = bsspace[bsnum] + 512;
	bsnum = (bsnum + 1) & 1;

	/* read main data into memory */
	if(!(this->*rd->read_frame_body)(rd, bsbuf, fr->framesize))
		return 0;

	bsi.bitindex = 0;
	bsi.wordpointer = (unsigned char *) bsbuf;

	if (param.halfspeed && fr->lay == 3)
		memcpy (ssave, bsbuf, ssize);

	return 1;
}

/****************************************
 * HACK,HACK,HACK: step back <num> frames
 * can only work if the 'stream' isn't a real stream but a file
 */
 int CReaders::back_frame(struct reader *rds,struct frame *fr,int num)
{
  long bytes;
  unsigned long newhead;
  
  if(!firsthead)
    return 0;
  
  bytes = (fr->framesize+8)*(num+2);
  
  if((this->*rds->back_bytes)(rds,bytes) < 0)
    return -1;
  if(!(this->*rds->head_read)(rds,&newhead))
    return -1;
  
  while( (newhead & HDRCMPMASK) != (firsthead & HDRCMPMASK) ) {
    if(!(this->*rds->head_shift)(rds,&newhead))
      return -1;
  }
  
  if((this->*rds->back_bytes)(rds,4) <0)
    return -1;

  read_frame(fr);
  read_frame(fr);
  
  if(fr->lay == 3) {
    set_pointer(512);
  }
  
  return 0;
}


/*
 * decode a header and write the information
 * into the frame structure
 */
int CDecoder::decode_header(struct frame *fr,unsigned long newhead)
{
    if(!m_readers->head_check(newhead))
		return 0;

	//bit20 and bit19 are MPEG Audio version ID
    if( newhead & (1<<20) ) 
	{
		fr->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
		fr->mpeg25 = 0;
    }
    else 
	{
		fr->lsf = 1;
		fr->mpeg25 = 1;
    }
    
    if( 1 )//!param.tryresync || !oldhead )
	{
		/* If "tryresync" is true, assume that certain parameters do not change within the stream! */
		fr->lay = 4 - ((newhead>>17)&3);
		if( ((newhead>>10)&0x3) == 0x3) 
		{
			//fprintf(stderr,"Stream error\n");
			//exit(1);
			return 0;
		}
		if(fr->mpeg25) 
		{
			fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
		}
		else
			fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);
		fr->error_protection = ((newhead>>16)&0x1)^0x1;
	}

    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

//    oldhead = newhead;
	m_readers->SetOldHead(newhead);

    if(!fr->bitrate_index) {
      //fprintf(stderr,"Free format not supported: (head %08lx)\n",newhead);
      return (0);
    }

    switch(fr->lay) {
      case 1:
		  m_layer = 1; // wyd
//	fr->do_layer = do_layer1;
#ifdef VARMODESUPPORT
        if (varmode) {
          fprintf(stderr,"Sorry, layer-1 not supported in varmode.\n"); 
          return (0);
        }
#endif
        fr->framesize  = (long) tabsel_123[fr->lsf][0][fr->bitrate_index] * 12000;
        fr->framesize /= freqs[fr->sampling_frequency];
        fr->framesize  = ((fr->framesize+fr->padding)<<2)-4;
        break;
      case 2:
		   m_layer = 2; // wyd
	//	fr->do_layer = do_layer2;
        fr->framesize = (long) tabsel_123[fr->lsf][1][fr->bitrate_index] * 144000;
        fr->framesize /= freqs[fr->sampling_frequency];
        fr->framesize += fr->padding - 4;
        break;
      case 3:
		   m_layer = 3; // wyd
    //    fr->do_layer = do_layer3;
        if(fr->lsf)
          m_readers->SetSszie((fr->stereo == 1) ? 9 : 17);
        else
			m_readers->SetSszie((fr->stereo == 1) ? 17 : 32);
        //  ssize = (fr->stereo == 1) ? 17 : 32;

        if(fr->error_protection)
			m_readers->SetSszie(m_readers->GetSsize()+2);
          //ssize += 2;
        fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
        fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
        fr->framesize = fr->framesize + fr->padding - 4;
        break; 
      default:
        //fprintf(stderr,"Sorry, unknown layer type.\n"); 
        return (0);
    }
    return 1;
}

#ifdef MPG123_REMOTE
void print_rheader(struct frame *fr)
{
	static char *modes[4] = { "Stereo", "Joint-Stereo", "Dual-Channel", "Single-Channel" };
	static char *layers[4] = { "Unknown" , "I", "II", "III" };
	static char *mpeg_type[2] = { "1.0" , "2.0" };

	/* version, layer, freq, mode, channels, bitrate, BPF */
	fprintf(stderr,"@I %s %s %ld %s %d %d %d\n",
			mpeg_type[fr->lsf],layers[fr->lay],freqs[fr->sampling_frequency],
			modes[fr->mode],fr->stereo,
			tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index],
			fr->framesize+4);
}
#endif

void print_header(struct frame *fr)
{
	static char *modes[4] = { "Stereo", "Joint-Stereo", "Dual-Channel", "Single-Channel" };
	static char *layers[4] = { "Unknown" , "I", "II", "III" };

	fprintf(stderr,"MPEG %s, Layer: %s, Freq: %ld, mode: %s, modext: %d, BPF : %d\n", 
		fr->mpeg25 ? "2.5" : (fr->lsf ? "2.0" : "1.0"),
		layers[fr->lay],freqs[fr->sampling_frequency],
		modes[fr->mode],fr->mode_ext,fr->framesize+4);
	fprintf(stderr,"Channels: %d, copyright: %s, original: %s, CRC: %s, emphasis: %d.\n",
		fr->stereo,fr->copyright?"Yes":"No",
		fr->original?"Yes":"No",fr->error_protection?"Yes":"No",
		fr->emphasis);
	fprintf(stderr,"Bitrate: %d Kbits/s, Extension value: %d\n",
		tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index],fr->extension);
}

void print_header_compact(struct frame *fr)
{
	static char *modes[4] = { "stereo", "joint-stereo", "dual-channel", "mono" };
	static char *layers[4] = { "Unknown" , "I", "II", "III" };
 
	fprintf(stderr,"MPEG %s layer %s, %d kbit/s, %ld Hz %s\n",
		fr->mpeg25 ? "2.5" : (fr->lsf ? "2.0" : "1.0"),
		layers[fr->lay],
		tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index],
		freqs[fr->sampling_frequency], modes[fr->mode]);
}

void print_id3_tag(unsigned char *buf)
{
	struct id3tag {
		char tag[3];
		char title[30];
		char artist[30];
		char album[30];
		char year[4];
		char comment[30];
		unsigned char genre;
	};
	struct id3tag *tag = (struct id3tag *) buf;
	char title[31]={0,};
	char artist[31]={0,};
	char album[31]={0,};
	char year[5]={0,};
	char comment[31]={0,};
	char genre[31]={0,};

	if(param.quiet)
		return;

	strncpy(title,tag->title,30);
	strncpy(artist,tag->artist,30);
	strncpy(album,tag->album,30);
	strncpy(year,tag->year,4);
	strncpy(comment,tag->comment,30);

	if ( tag->genre <= sizeof(genre_table)/sizeof(*genre_table) ) {
		strncpy(genre, genre_table[tag->genre], 30);
	} else {
		strncpy(genre,"Unknown",30);
	}
	
	fprintf(stdout,"Title  : %-30s  Artist: %s\n",title,artist);
	fprintf(stderr,"Album  : %-30s  Year  : %4s\n",album,year);
	fprintf(stderr,"Comment: %-30s  Genre : %s\n",comment,genre);
}

#if 0
/* removed the strndup for better portability */
/*
 *   Allocate space for a new string containing the first
 *   "num" characters of "src".  The resulting string is
 *   always zero-terminated.  Returns NULL if malloc fails.
 */
char *strndup (const char *src, int num)
{
	char *dst;

	if (!(dst = (char *) malloc(num+1)))
		return (NULL);
	dst[num] = '\0';
	return (strncpy(dst, src, num));
}
#endif

/*
 *   Split "path" into directory and filename components.
 *
 *   Return value is 0 if no directory was specified (i.e.
 *   "path" does not contain a '/'), OR if the directory
 *   is the same as on the previous call to this function.
 *
 *   Return value is 1 if a directory was specified AND it
 *   is different from the previous one (if any).
 */

int CReaders::split_dir_file (const char *path, char **dname, char **fname)
{
	char *slashpos;

	if ((slashpos = (char *)strrchr(path, '/'))) {
		*fname = slashpos + 1;
		*dname = _strdup(path); /* , 1 + slashpos - path); */
		if(!(*dname)) {
			perror("memory");
			exit(1);
		}
		(*dname)[1 + slashpos - path] = 0;
		if (lastdir && !strcmp(lastdir, *dname)) {
			/***   same as previous directory   ***/
			free (*dname);
			*dname = lastdir;
			return 0;
		}
		else {
			/***   different directory   ***/
			if (lastdir)
				free (lastdir);
			lastdir = *dname;
			return 1;
		}
	}
	else {
		/***   no directory specified   ***/
		if (lastdir) {
			free (lastdir);
			lastdir = NULL;
		};
		*dname = NULL;
		*fname = (char *)path;
		return 0;
	}
}

void CReaders::set_pointer(long backstep)
{
  bsi.wordpointer = bsbuf + ssize - backstep;
  if (backstep)
    memcpy(bsi.wordpointer,bsbufold+fsizeold-backstep,backstep);
  bsi.bitindex = 0; 
}

/********************************/

double CMpg123::compute_bpf(struct frame *fr)
{
	double bpf;

        switch(fr->lay) {
                case 1:
                        bpf = tabsel_123[fr->lsf][0][fr->bitrate_index];
                        bpf *= 12000.0 * 4.0;
                        bpf /= freqs[fr->sampling_frequency] <<(fr->lsf);
                        break;
                case 2:
                case 3:
                        bpf = tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index];                        bpf *= 144000;
                        bpf /= freqs[fr->sampling_frequency] << (fr->lsf);
                        break;
                default:
                        bpf = 1.0;
        }

	return bpf;
}

double CMpg123::compute_tpf(struct frame *fr)
{
	static int bs[4] = { 0,384,1152,1152 };
	double tpf;

	tpf = (double) bs[fr->lay];
	tpf /= freqs[fr->sampling_frequency] << (fr->lsf);
	return tpf;
}

int CReaders::get_sample_per_frame(struct frame *fr)
{
	static int bs[4] = { 0,384,1152,1152 };
	
	int spf = bs[fr->lay];
	return spf / (1 << (fr->lsf));
}

int CMpg123::get_songlen(struct frame *fr,int no)
{
	double tpf;
	
	if(!fr)
		return 0;
	
	if(no < 0) {
		if(!m_readers->GetReader() || m_readers->GetReader()->filelen < 0)
			return 0;
		no = (int)((double) m_readers->GetReader()->filelen / compute_bpf(fr));
	}

	tpf = compute_tpf(fr);
	return (int)(no*tpf);
}

long CMpg123::GetFileOffset() const
{ 	
	return m_readers->GetFileOffset();	
}
