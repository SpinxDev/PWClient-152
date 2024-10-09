
#ifndef GT_OVERLAY_H_
#define GT_OVERLAY_H_

#include <windows.h>
#include "overlay.h"

// 说明:
//   SDK的使用对于不同的游戏工作室有很多共性, 所以我们实现了所有公用的
//   部分以减少游戏客户端集成的开发工作量. 这部分代码需要放在游戏客户端
//   代码编译. 主要包括GT动态库位置的查找、检测是否安装GT、以及升级提示
//   通知等. 一切的一切都是为了节省大家的时间!
//
// 万连文于2012.9.19

namespace overlay {

enum GameRenderType {
  kGameRenderDX81,
  kGameRenderDX9
};

class GTOverlay : public OverlayListener {
 public:
  static GTOverlay& Instance();
  ~GTOverlay();

  // 主线程调用
  bool EnterMain(GameRenderType render_type);  // 失败表明GT没有安装
  void LeaveMain();

  // 主窗口线程调用
  void EnterCreateGameWindow(HWND game_window);
  void LeaveDestroyGameWindow();
  
  // 任意线程调用
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
  
  void SetGTWindowPosition(int x, int y);  // 已失效，不需要使用了
  void ClickGTIcon();       // 点击GT图标时调用, 通知点击事件

  // OverlayListener:
  // 回调通知, 全部运行在主窗口线程
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

  volatile bool	m_bModuleLoaded;	//	是否成功加载
  volatile bool m_bLogin;			//	是否已登录
  volatile bool m_bInTeam;			//	是否已在队伍频道
  volatile bool m_bInFaction;		//	是否已在帮派频道
  volatile bool m_bShown;			//	GT主界面是否显示
};

}  // namespace overlay

#endif  // GT_OVERLAY_H_
