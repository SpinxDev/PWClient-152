#ifndef _BASE64_H_
#define _BASE64_H_

int	base64_encode(unsigned char *__instr, int __inlen, char *__outstr);

// __in中的字符数一定是4的整倍数
// __out的大小应该是 __in * 3 /4
// 一次处理__in的4个字符，变成__out里的最多三个字符
//  前两个是不会为‘＝’的
int	base64_decode(char *__instr, int __inlen, unsigned char *__outstr);

#endif