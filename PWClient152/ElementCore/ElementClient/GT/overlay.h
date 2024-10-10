
#ifndef OVERLAY_OVERLAY_H_
#define OVERLAY_OVERLAY_H_

#include "hook_game.h"

namespace overlay {

// 回调函数执行在主线程
class OverlayListener {
 public:
  virtual void OnLogin() = 0;
  virtual void OnLogout() = 0;
  virtual void OnEnterTeam() = 0;
  virtual void OnLeaveTeam() = 0;
  virtual void OnEnterFaction() = 0;
  virtual void OnLeaveFaction() = 0;
};

// 注意:
//   SetHostWindow、GetListener、SetListener只能在主线程调用.
//   Login、Logout、Toggle、SendCommand接口允许在其它线程调用.
//   在接到OnLogin回调通知前不允许调用SendCommand接口，否则行为未知.
// 为了避免早期接口的不稳定性, 具体功能调用通过发生命令的软接口(协议)方式实现.
// 这样后期不断集成进入游戏的时候灵活性会更好, 不至于接口膨胀.
// Command(空格、换行等只为排版,无实际意义):
//   进组队频道:
//   {
//     "enter_team" : "team_id"
//   }
//   离开组队频道：
//   {
//     "leave_team" : ""
//   }
//   进入帮派:
//   {
//     "enter_faction" : ""
//   }
//   离开帮派频道：
//   {
//     "leave_faction" : ""
//   }
//   设置GT窗口位置:
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
  // 控制所有GT界面的显示开关, 主要用于临时性的UI屏蔽.
  // 比如: 切换游戏场景.
  virtual void Toggle(bool on) = 0;
  virtual bool SendCommand(const char* json_command) = 0;

  virtual void LeaveMain() = 0;
};

}  // namespace overlay


extern "C" {

OVERLAY_EXPORT overlay::Overlay* GetOverlay();

}  // extern "C"

#endif  // OVERLAY_OVERLAY_H_
