#include <time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
//#include "config.h"
#include "log.h"
/*
功能：     获取当前系统时间
返回值：    0-成功，-1-失败
out:        保存返回的系统时间，格式由fmt决定
fmt:        0-返回：yyyy-mm-dd hh24:mi:ss, 1-返回：yyyy-mm-dd, 2-返回：hh24:mi:ss
*/
int getTime(char *out, int fmt)                 // 获取当前系统时间
{
	time_t t;
	struct tm *tp;

	if (out == NULL)
		return -1;
	t = time(NULL);

	tp = localtime(&t);
	if (fmt == 0)
		sprintf(out, "%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
	else if (fmt == 1)
		sprintf(out, "%2.2d-%2.2d-%2.2d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	else if (fmt == 2)
		sprintf(out, "%2.2d:%2.2d:%2.2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
	return 0;
}

/*
功能：     将str写入到文件
返回值：    写文件成功返回0,否则返回-1
fp:     文件指针
str:        待写入的字符串
bLog:       1-是日志文件，0-不是日志文件
说明：     如果是日志文件，将会在str前加上当前时间(格式如：2011-04-12 12:10:20)
*/
int writeFile(FILE *fp, const char *str, int bLog)          // 写字符串到文件,bLog表明是否为日志文件
{
	assert(fp != NULL && str != NULL);
	char curTime[100] = { 0 };
	int ret = -1;
	if (bLog) // 获取当前系统时间
	{
		getTime(curTime, 0);
		ret = fprintf(fp, "[%s] %s\n", curTime, str);
	}
	else
		ret = fprintf(fp, "%s\n", str);

	if (ret >= 0)
	{
		fflush(fp);
		return 0;               // 写文件成功
	}
	else
		return -1;
}

int LOG_WriteLog(char const *str)
{
	char fileName[256];
	memset(fileName, 0, sizeof(fileName));
	//CFG_GetProfileString("log", fileName);
	strcpy(fileName, "mposLog.log");
	//strcat(fileName, "testlog.txt");
	FILE *fp = fopen(fileName, "a+");
	if (fp == NULL)
	{
		return 0;
	}
	int ret = 0;
	ret = writeFile(fp, str, 1);
	fclose(fp);
	return ret;
}
