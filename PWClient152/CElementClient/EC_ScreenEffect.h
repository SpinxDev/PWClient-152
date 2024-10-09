// Filename	: EC_ScreenEffectControl.h
// Creator	: zhangyitian
// Date		: 2014/8/5

#ifndef _ELEMENTCLIENT_EC_SCREENEFFECTCONTROL_H_
#define _ELEMENTCLIENT_EC_SCREENEFFECTCONTROL_H_

#include <ABaseDef.h>

class CECShakeCameraBase;

// 屏幕特效基类
class CECScreenEffect {
public:
	enum enumScreenEffectType {
		EFFECT_SCREENBLACKFADEIN,	// 进入黑屏
		EFFECT_SCREENBLACKFADEOUT,	// 离开黑屏
		EFFECT_SCREENSHAKE,			// 屏幕震动
		EFFECT_REDSPARK,			// 屏幕周围出现红色闪烁
		EFFECT_SCREENGRAY,			// 屏幕变灰，仅在全屏泛光开启时有效
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

// 屏幕特效管理类
class CECScreenEffectMan {	
public:
	CECScreenEffectMan();
	~CECScreenEffectMan();
	void StartEffect(CECScreenEffect::enumScreenEffectType type, int iTime);
	void FinishEffect(CECScreenEffect::enumScreenEffectType type);
	void Tick();
private:
	CECScreenEffect * m_pScreenEffect[CECScreenEffect::EFFECT_NUM];
	DWORD m_dwStartTime[CECScreenEffect::EFFECT_NUM];	// 效果开始的时间
	int m_iTime[CECScreenEffect::EFFECT_NUM];			// 效果的持续时间
};

// 屏幕震动效果
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

// 红色闪烁效果
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

// 黑屏效果
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

// 灰屏效果，只在全局泛光打开时有效
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