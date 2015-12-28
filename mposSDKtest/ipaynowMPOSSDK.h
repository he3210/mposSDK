#ifndef IPAYNOWMPOSSDK_H
#define IPAYNOWMPOSSDK_H


typedef int(*ipaynowPack8583)(char const *in, char *out);
typedef int(*ipaynowGet8583Msg)(char const *mac, char *szRespMsg);
typedef int(*ipaynowUnpack8583)(char *szRespMsg);

#endif