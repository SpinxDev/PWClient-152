#ifndef __IMAGECHECKER_H
#define __IMAGECHECKER_H

#include <windows.h>

namespace ImageChecker
{

void ACImageCheckerInit(); // 初始化
void CheckImage(); // 反外挂线程需要每两秒调用一次

DWORD WINAPI UpdateImageChecker(void*);			// 需要注册到APILoader，由 gacd 动态调用

};

#endif