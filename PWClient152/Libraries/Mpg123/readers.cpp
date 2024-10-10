#include <stdlib.h>
#include <stdio.h>

#include "mpg123.h"
#include "common.h"
#include <fcntl.h>
#include <io.h>

#include "AFile.h"
#include "AFileImage.h"
#include "AFileUnicode.h"
#include "readers.h"


int CReaders::a_lseek(AFile * fd, int offset, int origin)
{
	return fd->Seek(offset, (AFILE_SEEK)origin) ? fd->GetPos() : -1;
}

/*******************************************************************
 * stream based operation
 */
int CReaders::fullread(AFile * fd,unsigned char *buf,int count)
{
  int cnt=0;

  unsigned long byte_read;
  while(cnt < count) {
    if( !fd->Read(buf+cnt,count-cnt,&byte_read) || !byte_read)
		break;
    cnt += byte_read;
  } 

  return cnt;
}

int CReaders::default_init(struct reader *rds)
{
  char buf[128];

  rds->filepos = 0;
  rds->filelen = get_fileinfo(rds, buf);
  
  if(rds->filelen > 0) {
    if(!strncmp(buf,"TAG",3)) {
      rds->flags |= READER_ID3TAG;
      memcpy(rds->id3buf,buf,128);
    }
  }
  return 0;
}

void CReaders::stream_close(struct reader *rds)
{
    if (rds->flags & READER_FD_OPENED)
	{
		if( rds->filept )
		{
			rds->filept->Close();
			delete rds->filept;
			rds->filept = NULL;
		}
	}
}

/**************************************** 
 * HACK,HACK,HACK: step back <num> frames 
 * can only work if the 'stream' isn't a real stream but a file
 */
int CReaders::stream_back_bytes(struct reader *rds,int bytes)
{
	return 0;
}
int CReaders::stream_back_frame(struct reader *rds,struct frame *fr,int num)
{
	return 0;
}

int CReaders::stream_head_read(struct reader *rds,unsigned long *newhead)
{
  unsigned char hbuf[4];

  if(fullread(rds->filept,hbuf,4) != 4)
    return FALSE;
  
  *newhead = ((unsigned long) hbuf[0] << 24) |
    ((unsigned long) hbuf[1] << 16) |
    ((unsigned long) hbuf[2] << 8)  |
    (unsigned long) hbuf[3];
  
  return TRUE;
}

int CReaders::stream_head_shift(struct reader *rds,unsigned long *head)
{
  unsigned char hbuf;

  if(fullread(rds->filept,&hbuf,1) != 1)
    return 0;
  *head <<= 8;
  *head |= hbuf;
  *head &= 0xffffffff;
  return 1;
}

int CReaders::stream_skip_bytes(struct reader *rds,int len)
{
	return a_lseek(rds->filept,len,SEEK_CUR);
}

int CReaders::stream_read_frame_body(struct reader *rds,unsigned char *buf,
				  int size)
{
  long l;

  if( (l=fullread(rds->filept,buf,size)) != size)
  {
    if(l <= 0)
      return 0;
    memset(buf+l,0,size-l);
  }

  return 1;
}

long CReaders::stream_tell(struct reader *rds)
{
  return a_lseek(rds->filept,0,SEEK_CUR);
}

void CReaders::stream_rewind(struct reader *rds)
{
  a_lseek(rds->filept,0,SEEK_SET);
}

/*
 * returns length of a file (if filept points to a file)
 * reads the last 128 bytes information into buffer
 */
int CReaders::get_fileinfo(struct reader *rds, char *buf)
{
	int len;

    if((len=a_lseek(rds->filept,0,SEEK_END)) < 0) 
	{
            return -1;
    }
    if(a_lseek(rds->filept,-128,SEEK_END) < 0)
            return -1;

    if(fullread(rds->filept,(unsigned char *)buf,128) != 128) 
	{
            return -1;
    }

    if(!strncmp(buf,"TAG",3)) 
	{
            len -= 128;
    }

    if(a_lseek(rds->filept,0,SEEK_SET) < 0)
            return -1;

    if(len <= 0)
            return -1;
	return len;
}


#ifdef READ_MMAP
/*********************************************************+
 * memory mapped operation 
 *
 */

int CReaders::mapped_init(struct reader *rds) 
{
	long len;
	char buf[128];

	len = get_fileinfo(rds,buf);
	if(len < 0)
		return -1;

	if(!strncmp(buf,"TAG",3)) {
	  rds->flags |= READER_ID3TAG;
	  memcpy(rds->id3buf,buf,128);
	}

        mappnt = mapbuf = (unsigned char *)
		mmap(NULL, len, PROT_READ, MAP_SHARED , rds->filept, 0);
	if(!mapbuf || mapbuf == MAP_FAILED)
		return -1;

	mapend = mapbuf + len;
	
	if(param.verbose > 1)
		fprintf(stderr,"Using memory mapped IO for this stream.\n");

	rds->filelen = len;
	return 0;
}

void CReaders::mapped_rewind(struct reader *rds)
{
	mappnt = mapbuf;
	if (param.usebuffer) 
		buffer_resync();	
}

void CReaders::mapped_close(struct reader *rds)
{
	munmap((void *)mapbuf,mapend-mapbuf);
	if (rds->flags & READER_FD_OPENED)
		close(rds->filept);
}

int CReaders::mapped_head_read(struct reader *rds,unsigned long *newhead) 
{
	unsigned long nh;

	if(mappnt + 4 > mapend)
		return FALSE;

	nh = (*mappnt++)  << 24;
	nh |= (*mappnt++) << 16;
	nh |= (*mappnt++) << 8;
	nh |= (*mappnt++) ;

	*newhead = nh;
	return TRUE;
}

int CReaders::mapped_head_shift(struct reader *rds,unsigned long *head)
{
  if(mappnt + 1 > mapend)
    return FALSE;
  *head <<= 8;
  *head |= *mappnt++;
  *head &= 0xffffffff;
  return TRUE;
}

int CReaders::mapped_skip_bytes(struct reader *rds,int len)
{
  if(mappnt + len > mapend)
    return FALSE;
  mappnt += len;
  if (param.usebuffer)
	  buffer_resync();
  return TRUE;
}

int CReaders::mapped_read_frame_body(struct reader *rds,unsigned char *buf,
				  int size)
{
  if(size <= 0) {
    fprintf(stderr,"Ouch. Read_frame called with size <= 0\n");
    return FALSE;
  }
  if(mappnt + size > mapend)
    return FALSE;
  memcpy(buf,mappnt,size);
  mappnt += size;

  return TRUE;
}

int CReaders::mapped_back_bytes(struct reader *rds,int bytes)
{
    if( (mappnt - bytes) < mapbuf || (mappnt - bytes + 4) > mapend)
        return -1;
    mappnt -= bytes;
    if(param.usebuffer)
	    buffer_resync();
    return 0;
}

int CReaders::mapped_back_frame(struct reader *rds,struct frame *fr,int num)
{
    long bytes;
    unsigned long newhead;


    if(!firsthead)
        return 0;

    bytes = (fr->framesize+8)*(num+2);

    /* Buffered mode is a bit trickier. From the size of the buffered
     * output audio stream we have to make a guess at the number of frames
     * this corresponds to.
     */
    if(param.usebuffer) 
		bytes += (long)(xfermem_get_usedspace(buffermem) /
			(buffermem->buf[0] * buffermem->buf[1] 
				* (buffermem->buf[2] & AUDIO_FORMAT_MASK ?
			16.0 : 8.0 )) 
			* (tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index] << 10));
/*
	    	bytes += (long)(compute_buffer_offset(fr)*compute_bpf(fr));  
*/

    if( (mappnt - bytes) < mapbuf || (mappnt - bytes + 4) > mapend)
        return -1;
    mappnt -= bytes;

    newhead = (mappnt[0]<<24) + (mappnt[1]<<16) + (mappnt[2]<<8) + mappnt[3];
    mappnt += 4;

    while( (newhead & HDRCMPMASK) != (firsthead & HDRCMPMASK) ) {
        if(mappnt + 1 > mapend)
            return -1;
        newhead <<= 8;
        newhead |= *mappnt++;
        newhead &= 0xffffffff;
    }
    mappnt -= 4;

    read_frame(fr);
    read_frame(fr);

    if(fr->lay == 3)
        set_pointer(512);

    if(param.usebuffer)
	    buffer_resync();
    
    return 0;
}

long CReaders::mapped_tell(struct reader *rds)
{
  return mappnt - mapbuf;
}

#endif

/*****************************************************************
 * read frame helper
 */

//struct reader *rd;
/*struct reader readers[] = {
#ifdef READ_SYSTEM
 { system_init,
   NULL,	// filled in by system_init() 
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL } ,
#endif
#ifdef READ_MMAP
 { mapped_init,
   mapped_close,
   mapped_head_read,
   mapped_head_shift,
   mapped_skip_bytes,
   mapped_read_frame_body,
   mapped_back_bytes,
   mapped_back_frame,
   mapped_tell,
   mapped_rewind } , 
#endif 
 { default_init,
   stream_close,
   stream_head_read,
   stream_head_shift,
   stream_skip_bytes,
   stream_read_frame_body,
   stream_back_bytes,
   stream_back_frame,
   stream_tell,
   stream_rewind } ,
 { NULL, }
};
*/
CReaders::CReaders()
{
		numframes = -1;
		sample_per_frame = 0;
		
		reader temp;

#ifdef READ_SYSTEM
		
		temp.init= system_init;
		temp.close = NULL;
		temp.head_read = NULL;
		temp.head_shift = NULL;
		temp.skip_bytes = NULL;
		temp.read_frame_body = NULL;
		temp.back_bytes = NULL;
		temp.back_frame = NULL;
		temp.tell = NULL;
		temp.rewind = NULL;

		readers.push_back(temp);
#endif

#ifdef READ_MMAP
 {
		temp.init= mapped_init;
		temp.close = mapped_close;
		temp.head_read = mapped_head_read;
		temp.head_shift = mapped_head_shift;
		temp.skip_bytes = mapped_skip_bytes;
		temp.read_frame_body = mapped_read_frame_body;
		temp.back_bytes = mapped_back_bytes;
		temp.back_frame = mapped_back_frame;
		temp.tell = mapped_tell;
		temp.rewind = mapped_rewind;

		readers.push_back(temp);
#endif 
		temp.init= &CReaders::default_init;
		temp.close = &CReaders::stream_close;
		temp.head_read = &CReaders::stream_head_read;
		temp.head_shift = &CReaders::stream_head_shift;
		temp.skip_bytes = &CReaders::stream_skip_bytes;
		temp.read_frame_body = &CReaders::stream_read_frame_body;
		temp.back_bytes = &CReaders::stream_back_bytes;
		temp.back_frame = &CReaders::stream_back_frame;
		temp.tell = &CReaders::stream_tell;
		temp.rewind = &CReaders::stream_rewind;
		readers.push_back(temp);

		temp.init= NULL;
		temp.close = NULL;
		temp.head_read = NULL;
		temp.head_shift = NULL;
		temp.skip_bytes = NULL;
		temp.read_frame_body = NULL;
		temp.back_bytes = NULL;
		temp.back_frame = NULL;
		temp.tell = NULL;
		temp.rewind = NULL;
		readers.push_back(temp);

		rd = NULL;
		m_decoder = NULL;
		fileoffset= 0;
		firsthead=0;
		
		fsizeold=0;
		bsnum=0;
		oldhead = 0;
		bsbuf=bsspace[1];
		
		halfphase = 0;
		mapbuf = NULL;
		mappnt = NULL;
		mapend = NULL;

		lastdir = NULL;
};
/* open the device to read the bit stream from it */
int CReaders::open_stream(char *bs_filenam,int fd)
{
    int i;
    int filept_opened = 1;
    AFile * filept;
	unsigned long tempfirsthead;

	if( 0 == _access(bs_filenam, 0) )
		filept = new AFile();
	else if( AFileUnicode::IsFileUnicodeExist(bs_filenam) )
		filept = new AFileUnicode();
	else
		filept = new AFileImage();

	if( !filept->Open(bs_filenam, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		delete filept;
		return 0;
	}

    rd = NULL;
    for(i=0;;i++) 
	{
		readers[i].filelen = -1;
		readers[i].filept  = filept;
		readers[i].flags = 0;
		if(filept_opened)
			readers[i].flags |= READER_FD_OPENED;

		if(NULL == readers[i].init)  // wyd
		{
			return 0;
		}
	//	if(readers[i].init(readers+i) >= 0) 
		if((this->*readers[i].init)(&readers[i]) >= 0) // wyd
		{
			rd = &readers[i];
			break;
		}
    }

    if(rd && rd->flags & READER_ID3TAG) 
	{
		print_id3_tag(rd->id3buf);
    }		   

	//Now we must skip some junks here, hehe :)
	fileoffset = 0;

Init_Resync:
	if( !(this->*rd->head_read)(rd, &tempfirsthead) )
		return 0;
	if( !head_check(tempfirsthead) )
	{
		/* I even saw RIFF headers at the beginning of MPEG streams ;( */
		if(tempfirsthead == ('R'<<24)+('I'<<16)+('F'<<8)+'F') 
		{
			if( !(this->*rd->head_read)(rd, &tempfirsthead) )
				return 0;
			while(tempfirsthead != ('d'<<24)+('a'<<16)+('t'<<8)+'a')
			{
				if(!(this->*rd->head_shift)(rd, &tempfirsthead))
					return 0;
			}
			if(!(this->*rd->head_read)(rd, &tempfirsthead))
				return 0;
		}
		else
		{
			/* step in byte steps through next 64K */
			for(i=0;i<65536;i++)
			{
				if(!(this->*rd->head_shift)(rd, &tempfirsthead))
					return 0;
				if(head_check(tempfirsthead))
					break;
			}
			if(i == 65536) 
			{
				//fprintf(stderr,"Giving up searching valid MPEG header\n");
				return 0;
			}
		}
	}

	//Check the first frame again;
	{
	unsigned long nexthead;

	if(!m_decoder->decode_header(&fr, tempfirsthead))
		goto Init_Resync;
	a_lseek(rd->filept, fr.framesize, SEEK_CUR);
	
	(this->*rd->head_read)(rd, &nexthead);
	a_lseek(rd->filept, -(fr.framesize + 4), SEEK_CUR);

	//If we search for 64K and does not find the head, we must abort :(
	if( rd->filept->GetPos() > 64 * 1024 ) 
		return 0;
	if( !head_check(nexthead) )
	{
		goto Init_Resync;
	}
	}
	if( !m_decoder->decode_header(&fr, tempfirsthead) )
		goto Init_Resync;

	fileoffset = rd->filept->GetPos() - 4;

	//Calculate framenum here;
	numframes = (rd->filelen - fileoffset) / (fr.framesize + 4);
	sample_per_frame = get_sample_per_frame(&fr);

	a_lseek(rd->filept, fileoffset, SEEK_SET);
	return 1;
}
