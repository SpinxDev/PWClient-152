// Filename	: EC_ControlLoopAnimation.cpp
// Creator	: Xu Wenbin
// Date		: 2014/3/13

#include "EC_ControlLoopAnimation.h"
#include "EC_UIHelper.h"
#include "EC_Game.h"

#include "DlgGeneralCard.h"

//	class CECControlLoopAnimation
CECControlLoopAnimation::CECControlLoopAnimation(AUIDialog *pDlg, const AnimateCtrls & aCtrls)
: m_pDlg(NULL)
, m_pObjBackup(NULL)
, m_pCreatePolicy(NULL)
, m_pOneRound(NULL)
, m_nRound(0)
{
	Init(pDlg, aCtrls);
}

CECControlLoopAnimation::~CECControlLoopAnimation()
{
	Release();
}

void CECControlLoopAnimation::Release()
{
	if (m_pDlg){
		m_pDlg = NULL;
		m_aCtrls.clear();
		delete m_pObjBackup;
		m_pObjBackup = NULL;
		delete m_pOneRound;
		m_pOneRound = NULL;
		delete m_pCreatePolicy;
		m_pCreatePolicy = NULL;
		m_nRound = 0;
	}
}

bool CECControlLoopAnimation::Init(AUIDialog *pDlg, const AnimateCtrls & aCtrls)
{
	//	清除已有内容
	Release();

	//	验证参数
	if (!pDlg || aCtrls.size() < 2){
		//	至少需要2个控件才可能有动画
		ASSERT(false);
		return false;
	}
	AnimateCtrls::const_iterator cit;
	for (cit = aCtrls.begin(); cit != aCtrls.end(); ++ cit)
	{
		if (!cit){
			//	传入的控件指针为空
			ASSERT(false);
			return false;
		}
	}

	//	保存参数
	m_pDlg = pDlg;
	m_aCtrls = aCtrls;
	m_pObjBackup = new CECUIControlData;
	for (cit = aCtrls.begin(); cit != aCtrls.end(); ++ cit)
	{
		m_pObjBackup->Register(*cit);
	}
	m_pOneRound = new OneRoundAnimation;
	return true;
}

bool CECControlLoopAnimation::IsInited()const
{
	return m_pDlg != NULL;
}

bool CECControlLoopAnimation::IsMoving()const
{
	return IsInited() && m_pCreatePolicy != NULL;
}

bool CECControlLoopAnimation::Tick()
{
	if (!IsMoving()){
		return false;
	}
	bool bFinished(false);
	if (m_pOneRound->Tick()){
		if (m_pCreatePolicy->CanContinue()){
			class OneRoundCmd : public CECControlLoopAnimationObserverNotifyCmd
			{
				int	m_nRound;
			public:
				OneRoundCmd(CECControlLoopAnimation *p, int nRound)
					: CECControlLoopAnimationObserverNotifyCmd(p), m_nRound(nRound){}
				virtual void Notify(CECControlLoopAnimationObserver *p){
					p->OnOneRound(m_pParent, m_nRound);
				}
			};
			NotifyObservers(OneRoundCmd(this, ++m_nRound));
			CreateAnimator();
		}else{
			class FinishedCmd : public CECControlLoopAnimationObserverNotifyCmd
			{
			public:
				FinishedCmd(CECControlLoopAnimation *p)
					: CECControlLoopAnimationObserverNotifyCmd(p){}
				virtual void Notify(CECControlLoopAnimationObserver *p){
					p->OnFinished(m_pParent);
				}
			};
			NotifyObservers(FinishedCmd(this));
			bFinished = true;
			RemoveAnimator();
			delete m_pCreatePolicy;
			m_pCreatePolicy = NULL;
		}
	}
	return bFinished;
}

const CECControlLoopAnimation::AnimateCtrls & CECControlLoopAnimation::GetCtrls()const
{
	return m_aCtrls;
}

void CECControlLoopAnimation::Start()
{
	if (!IsInited()){
		return;
	}
	class CreateAlwaysPolicy : public CreatePolicy
	{
	public:
		virtual bool CanContinue()const{
			//	无限循环移动
			return true;
		}
	};
	m_nRound = 0;
	StartAnimation(new CreateAlwaysPolicy());
}

void CECControlLoopAnimation::Stop()
{	
	if (!IsInited()){
		return;
	}
	if (!IsMoving()){
		return;
	}
	class CreateOncePolicy : public CreatePolicy
	{
	public:
		virtual bool CanContinue()const{
			//	只移动一次
			return false;
		}
	};
	StartAnimation(new CreateOncePolicy());
}

void CECControlLoopAnimation::StartAnimation(CreatePolicy *p)
{
	//	使用新策略继续或创建动画
	delete m_pCreatePolicy;
	m_pCreatePolicy = p;
	CreateAnimator();
}

void CECControlLoopAnimation::CreateAnimator()
{
	if (!m_pOneRound->IsMoving()){
		m_pOneRound->FastMove(m_aCtrls);
	}
	if (!m_pCreatePolicy->CanContinue()){
		m_pOneRound->SlowDown();
	}
}

void CECControlLoopAnimation::RemoveAnimator()
{
	//	清除所有动画控制
	m_pOneRound->Clear();

	//	将所有界面恢复为默认位置
	m_pObjBackup->UpdateUI();

	//	重置已旋转轮数
	m_nRound = 0;
}

void CECControlLoopAnimation::Reset()
{
	if (!IsMoving()){
		return;
	}
	RemoveAnimator();
	delete m_pCreatePolicy;
	m_pCreatePolicy = NULL;
}

void CECControlLoopAnimation::OnChangeLayoutBegin()
{
	//	更换界面前、恢复对话框位置
	if (!IsInited()){
		return;
	}
	m_pObjBackup->UpdateUI();
}

void CECControlLoopAnimation::OnChangeLayoutEnd()
{
	if (!IsInited()){
		return;
	}
	//	应用新对话框和控件位置
	AUIDialog *pDlg = m_pDlg;
	AnimateCtrls aCtrls = m_aCtrls;
	int	nRound = m_nRound;
	CreatePolicy * pCreatePolicy = m_pCreatePolicy;
	m_pCreatePolicy = NULL;
	Init(pDlg, aCtrls);
	m_nRound = nRound;

	//	继续动画播放状态
	if (pCreatePolicy){
		StartAnimation(pCreatePolicy);
	}
}

void CECControlLoopAnimation::NotifyObservers(CECControlLoopAnimationObserverNotifyCmd &cmd)
{
	Observers observers;
	for (Observers::iterator it = m_observers.begin(); it != m_observers.end(); ++ it)
	{
		Observer & o = *it;
		cmd.Notify(o);
	}
}

bool CECControlLoopAnimation::IsObserverRegistered(const CECControlLoopAnimationObserver *pObserver)const
{
	return std::find(m_observers.begin(), m_observers.end(), pObserver) != m_observers.end();
}

bool CECControlLoopAnimation::RegisterObserver(CECControlLoopAnimationObserver *pObserver)
{
	bool bRegistered(false);
	if (!IsObserverRegistered(pObserver)){
		m_observers.push_back(pObserver);
		bRegistered = true;
	}
	return bRegistered;
}

bool CECControlLoopAnimation::UnregisterObserver(CECControlLoopAnimationObserver *pObserver)
{
	bool bUnRegistered(false);
	Observers::iterator it = std::find(m_observers.begin(), m_observers.end(), pObserver);
	if (it != m_observers.end()){
		m_observers.erase(it);
		bUnRegistered = true;
	}
	return bUnRegistered;
}

//	OneStepAnimation
OneStepAnimation::~OneStepAnimation()
{
	Clear();
}

void OneStepAnimation::Clear()
{
	//	恢复第一个控件的显示
	if (!m_array.empty()){
		CECControlAnimation *p = m_array.front();
		PAUIOBJECT pCtrl = p->GetSrcControl();
		if (!pCtrl->IsShow()){
			pCtrl->Show(true);
		}
	}

	//	删除动画
	for (OneStepArray::iterator it = m_array.begin(); it != m_array.end(); ++ it)
	{
		delete *it;
	}
	m_array.clear();
}

void OneStepAnimation::Swap(OneStepAnimation &rhs)
{
	m_array.swap(rhs.m_array);
}

void OneStepAnimation::Append(CECControlAnimation *p)
{
	m_array.push_back(p);
}

bool OneStepAnimation::Tick()
{
	DWORD dwTick = CECUIHelper::GetGame()->GetRealTickTime();
	int nFinished(0);
	for (OneStepArray::iterator it = m_array.begin(); it != m_array.end(); ++ it)
	{
		CECControlAnimation *p = *it;
		if (p->Tick(dwTick)){
			++ nFinished;
		}
	}
	bool bFinished = (nFinished >= (int)m_array.size());

	//	最左边一个控件在移往最右边控件的过程中隐藏显示、移动完成后恢复显示
	CECControlAnimation *p = m_array.front();
	PAUIOBJECT pCtrl = p->GetSrcControl();
	if (bFinished){
		if (pCtrl->IsShow()){
			pCtrl->Show(false);
		}
	}else{
		if (!pCtrl->IsShow()){
			pCtrl->Show(true);
		}
	}
	return bFinished;
}

void OneStepAnimation::ChangeMoveMethod(int newMethod)
{
	for (OneStepArray::iterator it = m_array.begin(); it != m_array.end(); ++ it)
	{
		CECControlAnimation *p = *it;
		p->ChangeMoveType(newMethod);
	}
}

//	OneRoundAnimation
OneRoundAnimation::OneRoundAnimation()
: m_step(-1)
, m_needSlowDown(false)
{
}

OneRoundAnimation::~OneRoundAnimation()
{
	Clear();
}

void OneRoundAnimation::FastMove(const CECControlLoopAnimation::AnimateCtrls &aCtrls)
{
	Clear();
	if (aCtrls.size() < 2){
		ASSERT(false);
		return;
	}
	m_needSlowDown = false;
	m_step = 0;
	int moveMethod = GetMoveMethod(m_step);
	int count = (int)aCtrls.size();
	for (int i(0); i < count; ++ i)
	{
		CECControlAnimation *p = new CECControlAnimation(moveMethod);
		p->AddControl(aCtrls[i], aCtrls[(i+count-1)%count]);
		m_toNext.Append(p);
	}
}

void OneRoundAnimation::SlowDown()
{
	if (!IsMoving()){
		return;
	}
	if (m_needSlowDown){
		return;
	}
	m_needSlowDown = true;
	if (IsSlowingDown(m_step)){
		//	当前正处于可应用慢动画时期、需要切换
		m_toNext.ChangeMoveMethod(GetMoveMethod(m_step));
	}
}

bool OneRoundAnimation::Tick()
{
	if (IsMoving() && m_toNext.Tick()){
		NextStep();
	}
	return !IsMoving();
}

void OneRoundAnimation::NextStep()
{
	int count = m_toNext.GetCount();
	if (m_step >= count){
		return;
	}
	if (++ m_step < count){
		int moveMethod = GetMoveMethod(m_step);
		OneStepAnimation toNewNext;
		for (int i(0); i < count; ++ i)
		{
			CECControlAnimation *p = new CECControlAnimation(moveMethod);
			p->AddControl(m_toNext.Get((i+1)%count)->GetSrcControl(), m_toNext.Get(i)->GetSrcControl());
			toNewNext.Append(p);
		}
		m_toNext.Swap(toNewNext);
	}else{
		Clear();
	}
}

bool OneRoundAnimation::IsMoving()const
{
	return m_step >= 0 && m_step < m_toNext.GetCount();
}

bool OneRoundAnimation::IsSlowingDown(int step)const
{
	return m_needSlowDown && (step+1 == m_toNext.GetCount());	//	仅在最后一次使用慢动作
}

int  OneRoundAnimation::GetMoveMethod(int step)const
{
	return IsSlowingDown(step) ? CECControlAnimation::ANIM_MOVE_EXP : CECControlAnimation::ANIM_MOVE_FAST;
}

void OneRoundAnimation::Clear()
{
	m_toNext.Clear();
	m_step = -1;
	m_needSlowDown = false;
}