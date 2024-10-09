
#ifndef OVERLAY_HOOK_GAME_H_
#define OVERLAY_HOOK_GAME_H_

#include <unknwn.h>
#include "overlay_export.h"

namespace overlay {

class HookGameListener {
 public:
  virtual void OnPreReset(IUnknown* d3d_device) = 0;
  virtual void OnPostReset(IUnknown* d3d_device) = 0;
  virtual void OnRenderGUI(IUnknown* d3d_device) = 0;
  virtual void OnDeleteDevice(IUnknown* d3d_device) = 0;
};

// 注意:
//   AddListener、RemoveListener只能在主线程调用.
class HookGame {
 public:
  virtual bool AddListener(HookGameListener* listener) = 0;
  virtual bool RemoveListener(HookGameListener* listener) = 0;
};

}  // namespace overlay


extern "C" {

OVERLAY_EXPORT overlay::HookGame* GetHookGame();

}  // extern "C"

#endif  // OVERLAY_HOOK_GAME_H_
