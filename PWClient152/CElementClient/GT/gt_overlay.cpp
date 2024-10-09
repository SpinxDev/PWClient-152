
#include "gt_overlay.h"
#include <ALog.h>

#include <atlbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

#pragma warning( disable : 4996 )

namespace {

const char* const kEnterTeamFormator = "{\"enter_team\" : \"%I64d\"}";
const char* const kLeaveTeamFormator = "{\"leave_team\" : \"\"}";
const char* const kEnterFactionFormator = "{\"enter_faction\" : \"\"}";
const char* const kLeaveFactionFormator = "{\"leave_faction\" : \"\"}";
const char* const kSetGTWindowPositionFormator =
    "{\"set_gtwindow_position\" : {\"x\" : %d, \"y\" : %d}}";
const char* const kClickGTIconFormator = "{\"click_gticon\" : \"\"}";

}  // namespace

namespace overlay {

GTOverlay& GTOverlay::Instance() {
  static GTOverlay overlay;
  return overlay;
}

GTOverlay::GTOverlay() : hook_module_(NULL),
                         overlay_module_(NULL),
                         get_hook_game_(NULL),
                         get_overlay_(NULL),
                         render_type_(kGameRenderDX9),
                         protocal_error_(false),
						 m_bModuleLoaded(false),
						 m_bLogin(false),
						 m_bInTeam(false),
						 m_bInFaction(false),
						 m_bShown(true)
{
  gt_installed_path_[0] = L'\0';

  wchar_t gt_installed_path[MAX_PATH];
  SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,
                   SHGFP_TYPE_CURRENT, gt_installed_path);
  PathAppendW(gt_installed_path, L"GT");
  
  wchar_t local_version_filepath[MAX_PATH];
  PathAppendW(local_version_filepath, gt_installed_path);
  PathAppendW(local_version_filepath, L"local_version");
  // 读取最新版本
  FILE* file = _wfopen(local_version_filepath, L"rb");
  if (file) {
    char version[80] = { 0 };
    fread(version, 1, sizeof(version), file);
    fclose(file);
    file = NULL;

    USES_CONVERSION;
    PathAppendW(gt_installed_path, A2CW(version));  // 版本号都是ASCII

    if (PathFileExistsW(gt_installed_path)) {
      PathAppendW(gt_installed_path_, gt_installed_path);
      PathAppendW(gt_installed_path_, L"bin");
    }
  }
}

GTOverlay::~GTOverlay() {
}

bool GTOverlay::EnterMain(GameRenderType render_type) {

  m_bModuleLoaded = false;

  render_type_ = render_type;

  if (wcslen(gt_installed_path_) == 0)
    return false;

  wchar_t old_current_directory[MAX_PATH] = { 0 };
  GetCurrentDirectoryW(MAX_PATH, old_current_directory);
  SetCurrentDirectoryW(gt_installed_path_);

  hook_module_ = LoadLibraryW(L"hook_game.dll");
  if (hook_module_)
    get_hook_game_ = reinterpret_cast<GetHookGame>(
        GetProcAddress(hook_module_, "GetHookGame"));
  
  switch (render_type_) {
    case kGameRenderDX81:
      overlay_module_ = LoadLibraryW(L"overlay_dx81.dll");
      break;
    case kGameRenderDX9:
      overlay_module_ = LoadLibraryW(L"overlay_dx9.dll");
      break;
  }
  if (overlay_module_)
    get_overlay_ = reinterpret_cast<GetOverlay>(
        GetProcAddress(overlay_module_, "GetOverlay"));

  SetCurrentDirectoryW(old_current_directory);

  if (get_hook_game_ != NULL && get_overlay_ != NULL)
	  m_bModuleLoaded = true;

  return true;
}

void GTOverlay::LeaveMain() {
  if (!get_hook_game_ || !get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->LeaveMain();
}

void GTOverlay::EnterCreateGameWindow(HWND game_window) {
  if (!get_hook_game_ || !get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->SetHostWindow(game_window);
  HookGame* hook_game = get_hook_game_();
  hook_game->AddListener(overlay);
  overlay->SetListener(this);
}

void GTOverlay::LeaveDestroyGameWindow() {
  if (!get_hook_game_ || !get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  HookGame* hook_game = get_hook_game_();
  hook_game->RemoveListener(overlay);
  overlay->SetHostWindow(NULL);
  overlay->SetListener(NULL);
}

void GTOverlay::Login(int zone_id, __int64 role_id, const char* passport,
                      const char* token) {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->Login(zone_id, role_id, passport, token);

  a_LogOutput(1, "GTOverlay::Login(%d, %I64d, %s, %s)", zone_id, role_id, passport, token);
}

void GTOverlay::Logout() {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->Logout();

  a_LogOutput(1, "GTOverlay::Logout()");
}
  
void GTOverlay::Toggle(bool on) {
  if (!get_overlay_)
    return;

  if (m_bShown == on) return;
  m_bShown = on;

  Overlay* overlay = get_overlay_();
  overlay->Toggle(on);
}

void GTOverlay::EnterTeam(__int64 team_id) {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  char command[128] = { 0 };
  sprintf(command, kEnterTeamFormator, team_id);
  overlay->SendCommand(command);

  a_LogOutput(1, "GTOverlay::EnterTeam(%I64d)", team_id);
}

void GTOverlay::LeaveTeam() {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->SendCommand(kLeaveTeamFormator);

  a_LogOutput(1, "GTOverlay::LeaveTeam()");
}

void GTOverlay::EnterFaction() {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->SendCommand(kEnterFactionFormator);

  a_LogOutput(1, "GTOverlay::EnterFaction()");
}

void GTOverlay::LeaveFaction() {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  overlay->SendCommand(kLeaveFactionFormator);
  a_LogOutput(1, "GTOverlay::LeaveFaction()");
}
  
void GTOverlay::SetGTWindowPosition(int x, int y) {
  if (!get_overlay_)
    return;

  Overlay* overlay = get_overlay_();
  char command[128] = { 0 };
  sprintf(command, kSetGTWindowPositionFormator, x, y);
  overlay->SendCommand(command);

  a_LogOutput(1, "GTOverlay::SetGTWindowPosition(%d, %d)", x, y);
}

void GTOverlay::ClickGTIcon() {
  if (!get_overlay_)
    return;
    
  Overlay* overlay = get_overlay_();
  overlay->SendCommand(kClickGTIconFormator);
  a_LogOutput(1, "GTOverlay::ClickGTIcon()");
}

}  // namespace overlay
