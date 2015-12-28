/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
	|_|              |___/

处理ISO8583组包解包相关操作
author  : nxd
time	: 2015.8.5
changed : 2015.8.21  
change content : changed for mpos sdk .  removed decrypt function and mac calculate function.

*/

#ifndef ISO8583_H
#define ISO8583_H


/*
	常用长度定义
*/
#define ISO8583_FILED_DATA_LENGTH		 	2048											/* 域内信息长度*/
#define ISO8583_MAX_DATA_LENGTH				8192											/* 最大信息长度*/

/*
	组包解包方式 宏定义集合
*/
#define ISO8583_LENGTH_FIX					0												/* 定长 */
#define ISO8583_LENGTH_2_CHANGE				1												/* 两位变长, 右靠BCD 1字节*/
#define ISO8583_LENGTH_3_CHANGE				2												/* 三位变长, 右靠BCD 2字节*/  
#define ISO8583_LENGTH_4_CHANGE				3												/* 四位变长, 右靠BCD 2字节 只用于报文总长度组包*/
     
#define ISO8583_CONTENT_BCD_RIGHT			0												/* 内容为右靠BCD格式,不做变化,最后生成时进行转换--定长数字也使用这个*/
#define ISO8583_CONTENT_BCD_LEFT			1												/* 内容为左靠BCD格式,不做变化,最后生成时进行转换*/
#define ISO8583_CONTENT_NO_BCD				2												/* 内容为非BCD格式,  "3"->"31"  "A"->"65"  */
#define ISO8583_CONTENT_ASC					3												/* 原样输入输出 不转换 用于56域,57域等域 */
/* 定义写死字段 */
#define ISO8583_TPDU						"6000020000"								    /* 写死TPDU头*/
#define ISO8583_HEADER						"613100310030"									/* 写死报文头*/

/* 定义组包输入(输出)结构 */
typedef struct filed
{
	char data[ISO8583_FILED_DATA_LENGTH];													/* 单域信息内容*/
	int	 dataLen;																			/* 信息内容长度 有效信息,非 "3132" 而是"12" */
	int  is_exist;																			/* 域是否存在 0:不存在  1:存在 */
}FILED;

typedef struct fileds
{
	FILED content[65];
}FILEDS, PACKSOURCE, UNPACKRESULT;

/* 初始化组包输入(输出)结构 */
int ISO_FILEDS_INIT(FILEDS *init);

/* 定义 发送 或者 接收 结构体*/
typedef struct message
{
	int length;																				/* 发送信息总长度*/
	char tpdu[10];																			/* tpdu头*/
	char header[12];																		/* 报文头说明*/
	char content[ISO8583_MAX_DATA_LENGTH];													/* 主内容*/
}MESSAGE, MSGSEND, MSGRECV;

/* 初始化Message结构体 */
int ISO_MSG_INIT(MESSAGE *init);

/* 组建8583流*/
int ISO8583_PackToStream(PACKSOURCE *packsource, char *output, int *outputLen);

/* 解析8583流*/
int ISO8583_StreamToPack(char *input, int inputLen, UNPACKRESULT *ur);

/* 组建8583报文 输出要压缩发送的结构体*/
int ISO8583_pack8583(PACKSOURCE *packsource, MSGSEND *msgsend);
/* 解析8583报文 输出解析后的各域*/
int ISO8583_unpack8583(MSGRECV const *msgrecv, UNPACKRESULT *result);

/* 将MSGSEND压缩为发送字节流*/
int ISO8583_MSGSEND2bytes(MSGSEND const *msgsend, char* outbuf, int *outbufLen);
/* 将接收字节流解析为MSGRECV*/
int ISO8583_bytes2MSGRECV(char const *recvbuf, int recvbufLen, MSGRECV *msgrecv);

/* 根据域组包规则组合单域*/
int ISO8583_COMBINE_FILED(FILED const *filed, int filedNum, char *outbuf);
/* 根据解包规则解析单域 返回域总长度(包含长度信息与正文信息)*/
int ISO8583_PRASE_FILED(FILED *filed, int filedNum, char const *inbuf);
/* 组合长度字符串*/
int ISO8583_COMBINE_LENGTH(int len, int lengthRule, char *outbuf);
/* "123" -> "313233" */
int ISO8583_Str2BcdStr(char const* input, int inputLen, char* output);
/* "313233" -> "123" */
int ISO8583_BcdStr2Str(char const* input, int inputLen, char* output);
/* 16进制字符转int   'a' => 10 */
int ISO8583_Hex2Int(char ch);
/* 计算位图 根据二域存在标识选择 正向计算 or 反向计算*/
int ISO8583_CALCBITMAT(FILEDS *fileds, char *bitmap);

int ISO8583_HEXStrToInt(char *hex);
#endif // !ISO8583_H
