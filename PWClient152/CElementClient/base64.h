#ifndef BASE64_H
#define BASE64_H

int	base64_encode(unsigned char *ch_in, int ch_inlen, char *ch_out);

// ch_in�е��ַ���һ����4��������
// ch_out�Ĵ�СӦ���� ch_in * 3 /4
// һ�δ���ch_in��4���ַ������ch_out�����������ַ�
//  ǰ�����ǲ���Ϊ��������
int	base64_decode(char *ch_in, int ch_inlen, unsigned char *ch_out);

#endif