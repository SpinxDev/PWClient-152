#ifndef __IMAGECHECKER_H
#define __IMAGECHECKER_H

#include <windows.h>

namespace ImageChecker
{

void ACImageCheckerInit(); // ��ʼ��
void CheckImage(); // ������߳���Ҫÿ�������һ��

DWORD WINAPI UpdateImageChecker(void*);			// ��Ҫע�ᵽAPILoader���� gacd ��̬����

};

#endif