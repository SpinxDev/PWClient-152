#ifndef BASE64_H
#define BASE64_H

int	base64_encode(unsigned char *__in, int __inlen, char *__out);

// __in�е��ַ���һ����4��������
// __out�Ĵ�СӦ���� __in * 3 /4
// һ�δ���__in��4���ַ������__out�����������ַ�
//  ǰ�����ǲ���Ϊ��������
int	base64_decode(char *__in, int __inlen, unsigned char *__out);

#endif