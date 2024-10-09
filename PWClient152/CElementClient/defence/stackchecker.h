#ifndef __STACKCHECKER_H
#define __STACKCHECKER_H

#include <windows.h>

namespace StackChecker
{

void ACStackCheckInit(); // 初始化
void ACTrace(int id); // 公共 trace 函数

VOID WINAPI GetStackCheckData(void*);			// 需要注册到APILoader，由 gacd 动态调用
VOID WINAPI UpdateStackCheckCode(void*);		// 需要注册到APILoader，由 gacd 动态调用

};

#endif