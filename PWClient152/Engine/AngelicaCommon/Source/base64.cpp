/*
	base64.cpp
	���ߣ� κ�� 
	�޸ģ�����
	���ܣ� base64��/���뺯��
	���ڣ� 2001-10
	�޸����ڣ�2002-1-17�� ���˱�û�б�֮��,�����Ķ�����
*/

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

// (__inlen + 2) / 3 * 4 + 1
int	base64_encode(unsigned char *__instr, int __inlen, char *__outstr)
{
	int	i, outlen;
	int	rest;
	unsigned int fp;

	rest	= __inlen;
	outlen	= 0;
	for(i=0;i<__inlen;i+=3)
	{
		if(rest >= 3)		//3 char -> 4 base64 char
		{
			fp = *(__instr++);
			fp <<= 8;
			fp |= *(__instr++);
			fp <<= 8;
			fp += *(__instr++);
			rest -= 3;
			for(int j=0;j<4;j++)
			{
				*__outstr = base64_en[(fp & 0x00fc0000)>>18];
				fp <<= 6;
				__outstr ++;
			}

		}
		else			// 1 or to char -> 2 or 3 base64 char
		{
			int tmp = rest;
			fp = *(__instr++);
			rest --;
			if(rest)
			{
				fp <<=8;
				fp |= *(__instr++);
				rest --;
			}
			//rest����һ����0
			//�տ�ʼrestֻ���ܵ���1����2
			if(tmp == 1)
			{
				*(__outstr++) = base64_en[(fp & 0xfc)>>2];
				fp <<= 4;
				*(__outstr++) = base64_en[fp & 0x3f];
				*(__outstr++) = '=';	//�����ַ�
				*(__outstr++) = '=';	//�����ַ�
			}
			else
			{
				*(__outstr++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(__outstr++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(__outstr++) = base64_en[(fp & 0xfc00)>>10];
				*(__outstr++) = '=';	//�����ַ�
			}
		}
		outlen	+= 4;
	}
	// ����__outstrָ��������һ���ַ���������ִ�һ�����ǿ���ʾ�ģ����Լ���0��β
	*__outstr	= 0;

	return	outlen;
}

// __instr�е��ַ���һ����4��������
// __outstr�Ĵ�СӦ���� __instr * 3 /4
// һ�δ���__instr��4���ַ������__outstr�����������ַ�
//  ǰ�����ǲ���Ϊ��������

int	base64_decode(char *__instr, int __inlen, unsigned char *__outstr)
{
	int i, outlen;
	unsigned int fp;

	if( __inlen % 4 )
	{
		return	-1;
	}
	outlen	= 0;
	for(i=0;i<__inlen;i+=4)
	{
		int tmp;
		int nullcounter;
		fp = 0;
		tmp = 0;
		nullcounter = 0;

		fp |= base64_de[(int)*(__instr++)];
		fp <<= 6;
		fp |= base64_de[(int)*(__instr++)];
		*(__outstr++) = (unsigned char)((fp >> 4) & 0xff);
		outlen ++;

		fp <<= 6;
		if((*__instr) != '=')
		{
			fp |= base64_de[(int)*(__instr++)];
			*(__outstr++) =  (unsigned char)((fp >> 2)& 0xff);
			outlen++;
		}
		else
		{
			goto End;
		}
		fp <<= 6;		
		if((*__instr) != '=')
		{
			fp |= base64_de[(int)*(__instr++)];
			*(__outstr++) = (unsigned char)(fp & 0xff);
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
