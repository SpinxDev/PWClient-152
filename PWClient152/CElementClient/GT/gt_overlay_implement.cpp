#include "gt_overlay.h"
#include <ALog.h>

namespace overlay {

void GTOverlay::OnLogin() {
  // TODO(game client developer):
  // ��¼�ɹ�. ��Ϸ���Ը��������Ϣenable�����ϵĽ������/���ɵİ�ť.

  // GT sample code: (����ǰ��ɾ�����´���)
	m_bLogin = true;
	a_LogOutput(1, "GTOverlay::OnLogin()");
}

void GTOverlay::OnLogout() {
	// �ǳ�. ��Ϸ���Ը��������Ϣdisable�����ϵĽ������/���ɵİ�ť.
	m_bLogin = false;
	a_LogOutput(1, "GTOverlay::OnLogout()");
}

void GTOverlay::OnEnterTeam() {
	// �����������. ��Ϸ���Ը��������Ϣ���½����ϵ����������ť.
	m_bInTeam = true;
	a_LogOutput(1, "GTOverlay::OnEnterTeam()");
}

void GTOverlay::OnLeaveTeam() {
	// �뿪�������. ��Ϸ���Ը��������Ϣ���½����ϵ����������ť.
	m_bInTeam = false;
	a_LogOutput(1, "GTOverlay::OnLeaveTeam()");
}

void GTOverlay::OnEnterFaction() {
	// �����������. ��Ϸ���Ը��������Ϣ���½����ϵİ���������ť.
	m_bInFaction = true;
	a_LogOutput(1, "GTOverlay::OnEnterFaction()");
}

void GTOverlay::OnLeaveFaction() {
	// �뿪��������. ��Ϸ���Ը��������Ϣ���½����ϵİ���������ť.
	m_bInFaction = false;
	a_LogOutput(1, "GTOverlay::OnLeaveFaction()");
}

}  // namespace overlay
