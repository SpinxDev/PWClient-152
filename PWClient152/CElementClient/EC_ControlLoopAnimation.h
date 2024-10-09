// Filename	: EC_ControlLoopAnimation.h
// Creator	: Xu Wenbin
// Date		: 2014/3/13

#pragma once

#include <vector.h>

class CECControlLoopAnimation;
class CECControlLoopAnimationObserver;
class CECControlLoopAnimationObserverNotifyCmd
{
protected:
	CECControlLoopAnimation	* m_pParent;
public:
	CECControlLoopAnimationObserverNotifyCmd(CECControlLoopAnimation *p)
		: m_pParent(p){}
	virtual ~CECControlLoopAnimationObserverNotifyCmd(){}
	virtual void Notify(CECControlLoopAnimationObserver *p)=0;
};

//	class CECControlLoopAnimationObserver
class CECControlLoopAnimation;
class CECControlLoopAnimationObserver
{
public:
	virtual ~CECControlLoopAnimationObserver(){}
	virtual void OnOneRound(const CECControlLoopAnimation *p, int nRound)=0;
	virtual void OnFinished(const CECControlLoopAnimation *p)=0;
};

//	class CECControlLoopAnimation
class AUIDialog;
class AUIObject;
class CECUIControlData;
class CECControlAnimation;
class OneRoundAnimation;
class CECControlLoopAnimation
{
	CECControlLoopAnimation(const CECControlLoopAnimation &);
	CECControlLoopAnimation & operator=(const CECControlLoopAnimation &);
public:
	typedef abase::vector<AUIObject *> AnimateCtrls;
	CECControlLoopAnimation(AUIDialog *pDlg, const AnimateCtrls & aCtrls);
	~CECControlLoopAnimation();
	
	bool IsInited()const;
	bool IsMoving()const;
	void Start();
	void Stop();
	void Reset();
	bool Tick();
	const AnimateCtrls & GetCtrls()const;
	
	void OnChangeLayoutBegin();
	void OnChangeLayoutEnd();
	
	//	观察者
	bool IsObserverRegistered(const CECControlLoopAnimationObserver *pObserver)const;
	bool RegisterObserver(CECControlLoopAnimationObserver *pObserver);
	bool UnregisterObserver(CECControlLoopAnimationObserver *pObserver);

private:
	bool Init(AUIDialog *pDlg, const AnimateCtrls & aCtrls);
	void Release();
	void CreateAnimator();
	void RemoveAnimator();
	
	class CreatePolicy
	{
	public:
		virtual ~CreatePolicy(){}
		virtual bool CanContinue()const=0;
	};
	void StartAnimation(CreatePolicy *p);

private:
	AUIDialog *			m_pDlg;
	AnimateCtrls		m_aCtrls;
	CECUIControlData *	m_pObjBackup;

	OneRoundAnimation * m_pOneRound;
	CreatePolicy	*	m_pCreatePolicy;
	int					m_nRound;			//	已经 Loop 的轮数
	
	//	观察者
	typedef CECControlLoopAnimationObserver *	Observer;
	typedef abase::vector<Observer>	Observers;
	Observers	m_observers;

	void NotifyObservers(CECControlLoopAnimationObserverNotifyCmd &cmd);
};

//	从控件一移向某一位置
class OneStepAnimation
{
	typedef abase::vector<CECControlAnimation *> OneStepArray;
	OneStepArray	m_array;
public:
	~OneStepAnimation();
	void Clear();
	void Append(CECControlAnimation *p);
	void Swap(OneStepAnimation &rhs);

	int GetCount()const{ return (int) m_array.size(); }
	const CECControlAnimation * Get(int index)const{ return m_array[index]; }
	CECControlAnimation * Get(int index){ return m_array[index]; }

	bool Tick();
	void ChangeMoveMethod(int newMethod);
};

//	一系列控件不断移向下一控件、直到移回原先位置
class OneRoundAnimation
{
	OneStepAnimation	m_toNext;
	int					m_step;
	bool				m_needSlowDown;
public:
	OneRoundAnimation();
	~OneRoundAnimation();
	void FastMove(const CECControlLoopAnimation::AnimateCtrls &aCtrls);
	void SlowDown();
	bool Tick();
	bool IsMoving()const;
	void Clear();
private:
	void NextStep();
	int  GetMoveMethod(int step)const;
	bool IsSlowingDown(int step)const;
};
