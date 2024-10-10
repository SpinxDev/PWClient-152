// Filename	: DlgIceThunderBall.h
// Creator	: Ruan ZhiJian
// Date		: 2014/10/11

#ifndef _ELEMENTCLIENT_DLGICETHUNDERBALL_H_
#define _ELEMENTCLIENT_DLGICETHUNDERBALL_H_

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "EC_GPDataType.h"

class CDlgIceThunderBall : public CDlgBase  
{	
public:
	CDlgIceThunderBall();
	virtual ~CDlgIceThunderBall();

	virtual bool Tick();

	void UpdateState(const S2C::IconState &it);
	int GetCurrentState(){ return m_iCurrentState; }
	void SetCurrentState(int currentState) { m_iCurrentState = currentState ;}

	void ShowCurrentBall(const S2C::IconState &icon_state);
	void ShowCurrentBall(int ball_state);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void HandleFirstBallEnter(const S2C::IconState &state);
	void HandleBallMove(const S2C::IconState &state);

	void PlayMoveGfx(const S2C::IconState &state);
	void PlayCurrentBallGfx(const S2C::IconState &state);

	void HideAllBall();

	// 辅助函数——用来获取一个整数特定位的数字
	int GetHundredsDigit(int state){ return state / 100 % 10 ;}
	int GetTensDigit(int state){ return state / 10 % 10 ;}
	int GetUnitsDigit(int state) { return state % 10 ;}

	// 显示特定位的球（并播放特效）
	void ShowHunderdsBall(int state_of_hundreds);
	void ShowTensBall(int state_of_tens);
	void ShowUnitsBall(int state_of_units);

	// 播放特定位置的移动特效
	void PlaySecondMoveGfx(int second_state);
	void PlayFirstMoveGfx(int first_state);

	int m_iCurrentState; // 记录当前的冰雷球状态

	PAUIIMAGEPICTURE m_pImg_Iceball1;
	PAUIIMAGEPICTURE m_pImg_Iceball2;
	PAUIIMAGEPICTURE m_pImg_Iceball3;
	
	PAUIIMAGEPICTURE m_pImg_ThunderBall1;
	PAUIIMAGEPICTURE m_pImg_ThunderBall2;
	PAUIIMAGEPICTURE m_pImg_ThunderBall3;
	
	PAUIIMAGEPICTURE m_pGfx_Move1;
	PAUIIMAGEPICTURE m_pGfx_Move2;
	
};

#endif // _ELEMENTCLIENT_DLGICETHUNDERBALL_H_
