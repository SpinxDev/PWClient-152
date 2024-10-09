
#ifndef OVERLAY_OVERLAY_H_
#define OVERLAY_OVERLAY_H_

#include "hook_game.h"

namespace overlay {

// �ص�����ִ�������߳�
class OverlayListener {
 public:
  virtual void OnLogin() = 0;
  virtual void OnLogout() = 0;
  virtual void OnEnterTeam() = 0;
  virtual void OnLeaveTeam() = 0;
  virtual void OnEnterFaction() = 0;
  virtual void OnLeaveFaction() = 0;
};

// ע��:
//   SetHostWindow��GetListener��SetListenerֻ�������̵߳���.
//   Login��Logout��Toggle��SendCommand�ӿ������������̵߳���.
//   �ڽӵ�OnLogin�ص�֪ͨǰ���������SendCommand�ӿڣ�������Ϊδ֪.
// Ϊ�˱������ڽӿڵĲ��ȶ���, ���幦�ܵ���ͨ�������������ӿ�(Э��)��ʽʵ��.
// �������ڲ��ϼ��ɽ�����Ϸ��ʱ������Ի����, �����ڽӿ�����.
// Command(�ո񡢻��е�ֻΪ�Ű�,��ʵ������):
//   �����Ƶ��:
//   {
//     "enter_team" : "team_id"
//   }
//   �뿪���Ƶ����
//   {
//     "leave_team" : ""
//   }
//   �������:
//   {
//     "enter_faction" : ""
//   }
//   �뿪����Ƶ����
//   {
//     "leave_faction" : ""
//   }
//   ����GT����λ��:
//   {
//     "set_gtwindow_position" : {
//       "x" : pixel_x,
//       "y" : pixel_y
//     }
//   }
class Overlay : public HookGameListener {
 public:
  virtual void SetListener(OverlayListener* listener) = 0;
  virtual OverlayListener* GetListener() const = 0;
  virtual void SetHostWindow(HWND host_window) = 0;

  virtual void Login(int zone_id, __int64 role_id, const char* passport,
                     const char* token) = 0;
  virtual void Logout() = 0;
  // ��������GT�������ʾ����, ��Ҫ������ʱ�Ե�UI����.
  // ����: �л���Ϸ����.
  virtual void Toggle(bool on) = 0;
  virtual bool SendCommand(const char* json_command) = 0;

  virtual void LeaveMain() = 0;
};

}  // namespace overlay


extern "C" {

OVERLAY_EXPORT overlay::Overlay* GetOverlay();

}  // extern "C"

#endif  // OVERLAY_OVERLAY_H_
