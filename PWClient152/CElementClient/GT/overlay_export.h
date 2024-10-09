
#ifndef OVERLAY_OVERLAY_EXPORT_H_
#define OVERLAY_OVERLAY_EXPORT_H_

#if defined(OVERLAY_IMPLEMENTATION)
#define OVERLAY_EXPORT __declspec(dllexport)
#else
#define OVERLAY_EXPORT __declspec(dllimport)
#endif  // defined(OVERLAY_IMPLEMENTATION)

#endif  // OVERLAY_OVERLAY_EXPORT_H_
