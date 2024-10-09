#ifndef BASE64_H
#define BASE64_H

int	base64_encode(unsigned char *ch_in, int ch_inlen, char *ch_out);

// ch_in中的字符数一定是4的整倍数
// ch_out的大小应该是 ch_in * 3 /4
// 一次处理ch_in的4个字符，变成ch_out里的最多三个字符
//  前两个是不会为‘＝’的
int	base64_decode(char *ch_in, int ch_inlen, unsigned char *ch_out);

#endif