#ifndef __STACKCHECKER_H
#define __STACKCHECKER_H

#include <windows.h>

namespace StackChecker
{

void ACStackCheckInit(); // ��ʼ��
void ACTrace(int id); // ���� trace ����

VOID WINAPI GetStackCheckData(void*);			// ��Ҫע�ᵽAPILoader���� gacd ��̬����
VOID WINAPI UpdateStackCheckCode(void*);		// ��Ҫע�ᵽAPILoader���� gacd ��̬����

};

#endif