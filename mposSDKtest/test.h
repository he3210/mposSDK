#include <stdio.h>
#define MAX_LEN 8192
#include "ipaynowMPOSSDK.h"

int test_read_file(char const *fileName, char *buf);

int test_Union_Pay(char const *fileName);

int test_Signup(char const *fileName);
int test_Signoff(char const *fileName);
int test_Settle(char const *fileName);

int test_IcparaQuery(char const *fileName);
int test_IcparaDown(char const *fileName);
int test_IcparaDownEnd(char const *fileName);

int test_KeyQuery(char const *fileName);
int test_KeyDown(char const *fileName);
int test_KeyDownEnd(char const *fileName);

int test_IcScriptUp(char const *fileName);
int test_SettleNotify(char const *fileName);

