// File		: EC_RewuShader.h
// Creator	: Xu Wenbin
// Date		: 2014/6/20

#ifndef _ELEMENTCLIENT_EC_REWUSHADER_H_
#define _ELEMENTCLIENT_EC_REWUSHADER_H_

class A3DShader;
class AString;

class CECSpecialShader
{
public:
	virtual ~CECSpecialShader(){}

	bool MatchTexture(const char * szTexMap, const char * szTexMap2)const;
	bool MatchShader(A3DShader *p)const;	
	A3DShader * Load()const;

	static void SetTexture(A3DShader *pShader, const char *szTexMap, const char *szTexMap2);

protected:
	virtual const char *GetTextureMapLowerCasePrefix()const=0;
	virtual const char *GetShaderFileLowerCase()const=0;
	
private:
	static AString GetFileTitleLowerCase(const char *szFilePath);
	static AString MakeTexMap2With(const char *szTexMap);
	static bool IsAlphaTexture(const char *szTexMap);
};

class CECRewuShader : public CECSpecialShader
{
public:
	virtual const char *GetTextureMapLowerCasePrefix()const{ return "rw_"; }
	virtual const char *GetShaderFileLowerCase()const{ return "rewu.sdr"; }
};

class CECWanmeiShader : public CECSpecialShader
{
public:
	virtual const char *GetTextureMapLowerCasePrefix()const{ return "wm_"; }
	virtual const char *GetShaderFileLowerCase()const{ return "wanmei.sdr"; }
};

#endif	//	_ELEMENTCLIENT_EC_REWUSHADER_H_