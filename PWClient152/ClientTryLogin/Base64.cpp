#include "stdafx.h"
#include "Base64.h"

const char Base64Code_alphabet[]= 
			{'Q','R','S','T','U',':','W','X','Y','Z','a','b','c','d','e','f', 
			'A','B','C','D','E','F','G','H','I','J',';','L','M','N','O','P', 
			'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
			'w','x','y','z','0','1','2',',','4','5','6','7','8','9','+','-','='}; 

Base64Code::Base64Code()
{
	int i;
	for(i=0;i<255;i++)
		b[i]=255;
	for(i=0;i<64;i++)
		b[Base64Code_alphabet[i]]=i;
	b['=']='\0';
}

BOOL Base64Code::IsCode(char c)
{
	return b[c]!=255;
}

void Base64Code::Encode(char *cSrc,char *cDes)
{
	int iLen=strlen(cSrc);
	int mLen=iLen;
	int c;
	while(mLen%3!=0)
		mLen--;
	int i;
	int j=0;
	int c1,c2,c3;
	for(i=0;i<mLen;i+=3)
	{
		c1=(unsigned char)cSrc[i];
		c2=(unsigned char)cSrc[i+1];
		c3=(unsigned char)cSrc[i+2];
		c=(c1<<16)+(c2<<8)+c3;
		cDes[j+3]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j+2]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j+1]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j]=Base64Code_alphabet[c&63];
		j+=4;
	}
	if(i+1==iLen)
	{
		c1=(unsigned char)cSrc[i];
		c=c1<<16;
		cDes[j+3]='=';
		cDes[j+2]='=';
		c=c>>12;
		cDes[j+1]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j]=Base64Code_alphabet[c&63];
		j+=4;
	}
	else if(i+2==iLen)
	{
		c1=(unsigned char)cSrc[i];
		c2=(unsigned char)cSrc[i+1];
		c=(c1<<16)+(c2<<8);
		cDes[j+3]='=';
		c=c>>6;
		cDes[j+2]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j+1]=Base64Code_alphabet[c&63];
		c=c>>6;
		cDes[j]=Base64Code_alphabet[c&63];
		j+=4;
	}
	cDes[j]='\0';
}

void Base64Code::Decode(char *cSrc,char *cDes)
{
	int iLen=strlen(cSrc);
	int c;
	int i;
	int j=0;
	int c1,c2,c3,c4;
	for(i=0;i<iLen;i+=4)
	{
		c1=b[cSrc[i]];
		c2=b[cSrc[i+1]];
		c3=b[cSrc[i+2]];
		c4=b[cSrc[i+3]];
		c=(c1<<18)+(c2<<12)+(c3<<6)+c4;
		cDes[j+2]=c&255 ;
		c=c>>8;
		cDes[j+1]=c&255;
		c=c>>8;
		cDes[j]=c&255;
		j+=3;
	}
	cDes[j]='\0';
}

char* UTF8::Decode(char* cSrc)
{
	char *cDes=new char[400];
	int iLen=strlen(cSrc);
	int i=0;
	int j=0;
	while(i<iLen)
	{
		if((cSrc[i]&0x80)==0)
		{
			cDes[j]=cSrc[i];
			j++;
			i++;
		}
		else
		{
			int l=0;
			int k=0x40;
			while((cSrc[i]&k)!=0)
			{
				k=k>>1;
				l++;
			}
			int p=cSrc[i]&(k-1);
			k=l*6+6-l;
			k=(k-1)/8+1;
			j+=k;
			i++;
			while(l>0)
			{
				p=(p<<6)+(cSrc[i]&0x3f);
				i++;
				l--;
			}
			for(l=1;l<=k;l++)
			{
				cDes[j-l]=p&0xff;
				p=p>>8;
			}
		}
	}
	cDes[j]='\0';
	return cDes;
}

