// Filename	: EC_ScreenEffectControl.h
// Creator	: zhangyitian
// Date		: 2014/8/5

#ifndef _ELEMENTCLIENT_EC_SCREENEFFECTCONTROL_H_
#define _ELEMENTCLIENT_EC_SCREENEFFECTCONTROL_H_

#include <ABaseDef.h>

class CECShakeCameraBase;

// ��Ļ��Ч����
class CECScreenEffect {
public:
	enum enumScreenEffectType {
		EFFECT_SCREENBLACKFADEIN,	// �������
		EFFECT_SCREENBLACKFADEOUT,	// �뿪����
		EFFECT_SCREENSHAKE,			// ��Ļ��
		EFFECT_REDSPARK,			// ��Ļ��Χ���ֺ�ɫ��˸
		EFFECT_SCREENGRAY,			// ��Ļ��ң�����ȫ�����⿪��ʱ��Ч
		EFFECT_NUM,
	};
public:
	static CECScreenEffect* CreateEffect(enumScreenEffectType type, int iTime);
	static void DeleteEffect(CECScreenEffect* pEffect);
	virtual void Tick() = 0;
	virtual void Start() = 0;
	virtual void Finish() = 0;
protected:
	virtual ~CECScreenEffect() {}
};

// ��Ļ��Ч������
class CECScreenEffectMan {	
public:
	CECScreenEffectMan();
	~CECScreenEffectMan();
	void StartEffect(CECScreenEffect::enumScreenEffectType type, int iTime);
	void FinishEffect(CECScreenEffect::enumScreenEffectType type);
	void Tick();
private:
	CECScreenEffect * m_pScreenEffect[CECScreenEffect::EFFECT_NUM];
	DWORD m_dwStartTime[CECScreenEffect::EFFECT_NUM];	// Ч����ʼ��ʱ��
	int m_iTime[CECScreenEffect::EFFECT_NUM];			// Ч���ĳ���ʱ��
};

// ��Ļ��Ч��
class CECScreenEffectShake : public CECScreenEffect {
public:
	CECScreenEffectShake();
	virtual ~CECScreenEffectShake();
	virtual void Tick();
	virtual void Start();
	virtual void Finish();
private:
	CECShakeCameraBase * m_pShakeCamera;
	DWORD m_dwLastTime;
};

// ��ɫ��˸Ч��
class AMSoundBuffer;
class CECScreenEffectRedSpark : public CECScreenEffect {
public:
	CECScreenEffectRedSpark();
	virtual void Tick();
	virtual void Start();
	virtual void Finish();
private:
	AMSoundBuffer* m_pSoundBuffer;
};

// ����Ч��
class CECScreenEffectBlackScreen : public CECScreenEffect {
public:
	void SetParam(int iStayTime, bool bFadeIn);
	virtual void Tick();
	virtual void Start();
	virtual void Finish();
private:
	bool m_bFadeIn;
	int m_iStayTime;
	DWORD m_dwStartTime;
};

// ����Ч����ֻ��ȫ�ַ����ʱ��Ч
class CECScreenEffectGrayScreen : public CECScreenEffect {
public:
	static bool IsEnableGrayEffect();
	virtual void Tick();
	virtual void Start();
	virtual void Finish();
private:
	static bool s_bEnableGrayEffect;
};

#endif