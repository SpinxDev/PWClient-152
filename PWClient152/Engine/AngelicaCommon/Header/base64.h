#ifndef _BASE64_H_
#define _BASE64_H_

int	base64_encode(unsigned char *__instr, int __inlen, char *__outstr);

// __in�е��ַ���һ����4��������
// __out�Ĵ�СӦ���� __in * 3 /4
// һ�δ���__in��4���ַ������__out�����������ַ�
//  ǰ�����ǲ���Ϊ��������
int	base64_decode(char *__instr, int __inlen, unsigned char *__outstr);

#endif