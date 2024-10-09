class Base64Code
{
public:
	unsigned char b[256];
public:
	Base64Code();
	void Encode(char* cSrc,char *cDes);
	void Decode(char* cSrc,char *cDes);
	BOOL IsCode(char c);
};

class UTF8
{
public:
	char* Decode(char* cSrc);
};

