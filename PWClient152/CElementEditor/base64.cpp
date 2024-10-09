/*
	base64.cpp
	作者： 魏华 
	修改：崔铭
	功能： base64编/解码函数
	日期： 2001-10
	修改日期：2002-1-17， 除了表没有变之外,其他的都变了
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "base64.h"

////////////////////////////////////////////////////////////////////////////
// base64 codec
////////////////////////////////////////////////////////////////////////////
// 用数组是为了快！！！！
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
int	base64_encode(unsigned char *__in1, int __inlen, char *__out1)
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
			fp = *(__in1++);
			fp <<= 8;
			fp |= *(__in1++);
			fp <<= 8;
			fp += *(__in1++);
			rest -= 3;
			for(int j=0;j<4;j++)
			{
				*__out1 = base64_en[(fp & 0x00fc0000)>>18];
				fp <<= 6;
				__out1 ++;
			}

		}
		else			// 1 or to char -> 2 or 3 base64 char
		{
			int tmp = rest;
			fp = *(__in1++);
			rest --;
			if(rest)
			{
				fp <<=8;
				fp |= *(__in1++);
				rest --;
			}
			//rest现在一定是0
			//刚开始rest只可能等于1或者2
			if(tmp == 1)
			{
				*(__out1++) = base64_en[(fp & 0xfc)>>2];
				fp <<= 4;
				*(__out1++) = base64_en[fp & 0x3f];
				*(__out1++) = '=';	//填充空字符
				*(__out1++) = '=';	//填充空字符
			}
			else
			{
				*(__out1++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(__out1++) = base64_en[(fp & 0xfc00)>>10];
				fp <<= 6;
				*(__out1++) = base64_en[(fp & 0xfc00)>>10];
				*(__out1++) = '=';	//填充空字符
			}
		}
		outlen	+= 4;
	}
	// 现在__out1指向的是最后一个字符，且这个字串一定都是可显示的，所以加上0结尾
	*__out1	= 0;

	return	outlen;
}

// __in1中的字符数一定是4的整倍数
// __out1的大小应该是 __in1 * 3 /4
// 一次处理__in1的4个字符，变成__out1里的最多三个字符
//  前两个是不会为‘＝’的

int	base64_decode(char *__in1, int __inlen, unsigned char *__out1)
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

		fp |= base64_de[(int)*(__in1++)];
		fp <<= 6;
		fp |= base64_de[(int)*(__in1++)];
		*(__out1++) = (unsigned char)((fp >> 4) & 0xff);
		outlen ++;

		fp <<= 6;
		if((*__in1) != '=')
		{
			fp |= base64_de[(int)*(__in1++)];
			*(__out1++) =  (unsigned char)((fp >> 2)& 0xff);
			outlen++;
		}
		else
		{
			goto End;
		}
		fp <<= 6;		
		if((*__in1) != '=')
		{
			fp |= base64_de[(int)*(__in1++)];
			*(__out1++) = (unsigned char)(fp & 0xff);
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
