#include "innerInterface.h"

#ifdef linux

int ipaynowPack8583(char const *input, char *out);

int ipaynowGet8583Msg(char const *input, char *out);

int ipaynowUnpack8583(char *out);

#else

int __declspec(dllexport) ipaynowPack8583(char const *input, char *out);

int __declspec(dllexport) ipaynowGet8583Msg(char const *input, char *out);

int __declspec(dllexport) ipaynowUnpack8583(char *out);

#endif