#include "gt_overlay.h"
#include <ALog.h>

namespace overlay {

void GTOverlay::OnLogin() {
  // TODO(game client developer):
  // 登录成功. 游戏可以根据这个消息enable界面上的进入组队/帮派的按钮.

  // GT sample code: (发布前请删除以下代码)
	m_bLogin = true;
	a_LogOutput(1, "GTOverlay::OnLogin()");
}

void GTOverlay::OnLogout() {
	// 登出. 游戏可以根据这个消息disable界面上的进入组队/帮派的按钮.
	m_bLogin = false;
	a_LogOutput(1, "GTOverlay::OnLogout()");
}

void GTOverlay::OnEnterTeam() {
	// 进入组队语音. 游戏可以根据这个消息更新界面上的组队语音按钮.
	m_bInTeam = true;
	a_LogOutput(1, "GTOverlay::OnEnterTeam()");
}

void GTOverlay::OnLeaveTeam() {
	// 离开组队语音. 游戏可以根据这个消息更新界面上的组队语音按钮.
	m_bInTeam = false;
	a_LogOutput(1, "GTOverlay::OnLeaveTeam()");
}

void GTOverlay::OnEnterFaction() {
	// 进入帮派语音. 游戏可以根据这个消息更新界面上的帮派语音按钮.
	m_bInFaction = true;
	a_LogOutput(1, "GTOverlay::OnEnterFaction()");
}

void GTOverlay::OnLeaveFaction() {
	// 离开帮派语音. 游戏可以根据这个消息更新界面上的帮派语音按钮.
	m_bInFaction = false;
	a_LogOutput(1, "GTOverlay::OnLeaveFaction()");
}

}  // namespace overlay
