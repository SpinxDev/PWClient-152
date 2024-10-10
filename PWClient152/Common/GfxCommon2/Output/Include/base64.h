#ifndef BASE64_H
#define BASE64_H

int	base64_encode(unsigned char *__in, int __inlen, char *__out);

// __in中的字符数一定是4的整倍数
// __out的大小应该是 __in * 3 /4
// 一次处理__in的4个字符，变成__out里的最多三个字符
//  前两个是不会为‘＝’的
int	base64_decode(char *__in, int __inlen, unsigned char *__out);

#endif