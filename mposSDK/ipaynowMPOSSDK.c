#include "innerInterface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32

int ipaynowPack8583(char const *input, char *out)
{
	int ret = 0;
	ret = INNER_OUT_SEND_MACSTR_BCD(input, out);
	return ret;
}
// 用户输入的时候 参数一:mac  参数二: 源串.  输出:参数二
int ipaynowGet8583Msg(char const *input, char *out)
{
	char outbuf[MAX_LEN];
	memset(outbuf, 0, sizeof(outbuf));
	int ret = 0;
	ret = INNER_OUT_GET_MAC_BCD(out, input, outbuf);
	memset(out,0,strlen(out));
	memcpy(out, outbuf, strlen(outbuf));
	return ret;
}

int ipaynowUnpack8583(char *out)
{
	char outbuf[MAX_LEN];
	memset(outbuf, 0, sizeof(outbuf));
	int ret = 0;
	ret = INNER_OUT_PARSE_BCD(out, outbuf);
	memset(out, 0, strlen(out));
	memcpy(out, outbuf, strlen(outbuf));
	return ret;
}

#else

int __declspec(dllexport) ipaynowPack8583(char const *input, char *out)
{
	int ret = 0;
	ret = INNER_OUT_SEND_MACSTR_BCD(input, out);
	return ret;
}

int __declspec(dllexport) ipaynowGet8583Msg(char const *input, char *out)
{
	char outbuf[MAX_LEN];
	memset(outbuf, 0, sizeof(outbuf));
	int ret = 0;
	ret = INNER_OUT_GET_MAC_BCD(out, input, outbuf);
	memset(out, 0, strlen(out));
	memcpy(out, outbuf, strlen(outbuf));
	return ret;
}

int __declspec(dllexport) ipaynowUnpack8583(char *out)
{
	char outbuf[MAX_LEN];
	memset(outbuf, 0, sizeof(outbuf));
	int ret = 0;
	ret = INNER_OUT_PARSE_BCD(out, outbuf);
	memset(out, 0, strlen(out));
	strcpy(out, outbuf);
	//memcpy(out, outbuf, strlen(outbuf));
	return ret;
}

#endif