/*
	base64.cpp
	���ߣ� κ�� 
	�޸ģ�����
	���ܣ� base64��/���뺯��
	���ڣ� 2001-10
	�޸����ڣ�2002-1-17�� ���˱�û�б�֮��,�����Ķ�����
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "base64.h"

////////////////////////////////////////////////////////////////////////////
// base64 codec
////////////////////////////////////////////////////////////////////////////
// ��������Ϊ�˿죡������
static unsigned char base64_en[] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
	'='
};

static unsigned char base64_de[] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   62, 0xFF, 0xFF, 0xFF,   63, 
	  52,   53,   54,   55,   56,   57,   58,   59,   60,   61, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14, 
	  15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40, 
	  41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

// (ch_inlen + 2) / 3 * 4 + 1
int	base64_encode(unsigned char *ch_in, int ch_inlen, char *ch_out)
{
	int	i, outlen;
	int	rest;
	unsigned int fp;

	rest	= ch_inlen;
	outlen	= 0;
	for(i=0;i<ch_inlen;i+=3)
	{
		if(rest >= 3)		//3 char -> 4 base64 char
		{
			fp = *(ch_in++);
			fp <<= 8;
			fp |= *(ch_in++);
			fp <<= 8;
			fp += *(ch_in++);
			rest -= 3;
			for(int j=0;j<4;j++)
			{
				*ch_out = base64_en[(fp & 0x00fc0000)>>18];
				fp <<= 6;
				ch_out ++;
			}

		}
		else			// 1 or to char -> 2 or 3 base64 char
		{
			int tmp = rest;
			fp = *(ch_in++);
			rest --;
			if(rest)
			{
				fp <<=8;
				fp |= *(ch_in++);
				rest --;
			}
			//rest����һ����0
			//�տ�ʼrestֻ���ܵ���1����2
			if(tmp == 1)
			{
				*(ch_out++) = base64_en[(fp & 0xfc)>>2];
				fp <<= 4;
				*(ch_out++) = base64_en[fp & 0x3f];
				*(ch_out++) = '=';	//�����ַ�
				*(ch_out++) = '=';	//�����ַ�
			}
			else
			{
				*(ch_out++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(ch_out++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(ch_out++) = base64_en[(fp & 0xfc00)>>10];
				*(ch_out++) = '=';	//�����ַ�
			}
		}
		outlen	+= 4;
	}
	// ����ch_outָ��������һ���ַ���������ִ�һ�����ǿ���ʾ�ģ����Լ���0��β
	*ch_out	= 0;

	return	outlen;
}

// ch_in�е��ַ���һ����4��������
// ch_out�Ĵ�СӦ���� ch_in * 3 /4
// һ�δ���ch_in��4���ַ������ch_out�����������ַ�
//  ǰ�����ǲ���Ϊ��������

int	base64_decode(char *ch_in, int ch_inlen, unsigned char *ch_out)
{
	int i, outlen;
	unsigned int fp;

	if( ch_inlen % 4 )
	{
		return	-1;
	}
	outlen	= 0;
	for(i=0;i<ch_inlen;i+=4)
	{
		int tmp;
		int nullcounter;
		fp = 0;
		tmp = 0;
		nullcounter = 0;

		fp |= base64_de[(int)*(ch_in++)];
		fp <<= 6;
		fp |= base64_de[(int)*(ch_in++)];
		*(ch_out++) = (unsigned char)((fp >> 4) & 0xff);
		outlen ++;

		fp <<= 6;
		if((*ch_in) != '=')
		{
			fp |= base64_de[(int)*(ch_in++)];
			*(ch_out++) =  (unsigned char)((fp >> 2)& 0xff);
			outlen++;
		}
		else
		{
			goto End;
		}
		fp <<= 6;		
		if((*ch_in) != '=')
		{
			fp |= base64_de[(int)*(ch_in++)];
			*(ch_out++) = (unsigned char)(fp & 0xff);
			outlen++;
		}
		else
		{
			goto End;
		}
	}
End:
	return	outlen;
}
