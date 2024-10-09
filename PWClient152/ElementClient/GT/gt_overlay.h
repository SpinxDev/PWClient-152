
#ifndef GT_OVERLAY_H_
#define GT_OVERLAY_H_

#include <windows.h>
#include "overlay.h"

// ˵��:
//   SDK��ʹ�ö��ڲ�ͬ����Ϸ�������кܶ๲��, ��������ʵ�������й��õ�
//   �����Լ�����Ϸ�ͻ��˼��ɵĿ���������. �ⲿ�ִ�����Ҫ������Ϸ�ͻ���
//   �������. ��Ҫ����GT��̬��λ�õĲ��ҡ�����Ƿ�װGT���Լ�������ʾ
//   ֪ͨ��. һ�е�һ�ж���Ϊ�˽�ʡ��ҵ�ʱ��!
//
// ��������2012.9.19

namespace overlay {

enum GameRenderType {
  kGameRenderDX81,
  kGameRenderDX9
};

class GTOverlay : public OverlayListener {
 public:
  static GTOverlay& Instance();
  ~GTOverlay();

  // ���̵߳���
  bool EnterMain(GameRenderType render_type);  // ʧ�ܱ���GTû�а�װ
  void LeaveMain();

  // �������̵߳���
  void EnterCreateGameWindow(HWND game_window);
  void LeaveDestroyGameWindow();
  
  // �����̵߳���
  bool IsLoaded()const{ return m_bModuleLoaded; }
  bool IsLogin()const{ return m_bLogin; }
  bool IsInTeam()const{ return m_bInTeam; }
  bool IsInFaction()const{ return m_bInFaction; }

  void Login(int zone_id, __int64 role_id, const char* passport,
             const char* token);
  void Logout();
  
  void Toggle(bool on);
  
  void EnterTeam(__int64 team_id);
  void LeaveTeam();
  void EnterFaction();
  void LeaveFaction();
  
  void SetGTWindowPosition(int x, int y);  // ��ʧЧ������Ҫʹ����
  void ClickGTIcon();       // ���GTͼ��ʱ����, ֪ͨ����¼�

  // OverlayListener:
  // �ص�֪ͨ, ȫ���������������߳�
  void OnLogin();
  void OnLogout();
  void OnEnterTeam();
  void OnLeaveTeam();
  void OnEnterFaction();
  void OnLeaveFaction();

 private:
  GTOverlay();

  // disallow the copy constructor and operator= functions
  GTOverlay(const GTOverlay&);
  void operator=(const GTOverlay&);

  HMODULE hook_module_;
  HMODULE overlay_module_;
  typedef HookGame* (*GetHookGame)();
  typedef Overlay* (*GetOverlay)();
  GetHookGame get_hook_game_;
  GetOverlay get_overlay_;

  GameRenderType render_type_;
  wchar_t gt_installed_path_[MAX_PATH];

  bool protocal_error_;

  volatile bool	m_bModuleLoaded;	//	�Ƿ�ɹ�����
  volatile bool m_bLogin;			//	�Ƿ��ѵ�¼
  volatile bool m_bInTeam;			//	�Ƿ����ڶ���Ƶ��
  volatile bool m_bInFaction;		//	�Ƿ����ڰ���Ƶ��
  volatile bool m_bShown;			//	GT�������Ƿ���ʾ
};

}  // namespace overlay

#endif  // GT_OVERLAY_H_
