#include "iso8583.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 组包解包规则定义 */
typedef struct rule
{
	int lengthMethods;																		/* 长度解析方法*/
	int contentMethods;																		/* 内容处理办法*/
	int maxLength;																			/* 定长域长度,或者变长域最大长度(压缩之前)*/
}RULE;

/* 全局组解包规则*/
RULE const rules[65] = 
{
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 4 },									/* 0  消息类型*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 16 },									/* 1  位图*/
	{ ISO8583_LENGTH_2_CHANGE, ISO8583_CONTENT_BCD_LEFT, 19 },								/* 2  主账号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 6 },									/* 3  交易处理码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 12 },									/* 4  交易金额*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 5  填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 6  填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 7  填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 8  填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 9  填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 10 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 6 },									/* 11 受卡方系统跟踪号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 6 },									/* 12 受卡方所在地时间*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 4 },									/* 13 受卡方所在地日期*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 4 },									/* 14 卡有效期*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 4 },									/* 15 清算日期*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 16 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 17 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 18 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 19 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 20 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 21 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_LEFT, 3  },									/* 22 服务点输入方式*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 4 },									/* 23 卡序列号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 24 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 2 },									/* 25 服务点条件码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 2 },									/* 26 服务点PIN获取码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 27 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 28 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 29 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 30 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 31 填充空域*/
	{ ISO8583_LENGTH_2_CHANGE, ISO8583_CONTENT_BCD_RIGHT, 11 },								/* 32 受理方标识码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 33 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 34 填充空域*/
	{ ISO8583_LENGTH_2_CHANGE, ISO8583_CONTENT_BCD_LEFT, 37 },								/* 35 2磁道数据*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_BCD_LEFT, 104 },								/* 36 3磁道数据*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 12 },										/* 37 检索参考号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 6 },										/* 38 授权标识应答码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 2 },										/* 39 应答码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 40 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 8 },										/* 41 终端号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 15 },										/* 42 商户号*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 43 填充空域*/
	{ ISO8583_LENGTH_2_CHANGE, ISO8583_CONTENT_NO_BCD, 25 },								/* 44 附加响应数据*/
	{ ISO8583_LENGTH_2_CHANGE, ISO8583_CONTENT_NO_BCD, 38 },								/* 45 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 46 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 47 填充空域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_BCD_LEFT, 322 },								/* 48 附加数据私有*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_NO_BCD, 3 },										/* 49 交易货币代码*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 50 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 51 填充空域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_ASC, 8 },						    				/* 52 个人标识码数据*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 16 },									/* 53 安全控制信息*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_NO_BCD, 20 },								/* 54 余额*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_ASC, 255 },									/* 55 IC卡数据域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_ASC, 999 },									/* 56 广告信息-订单信息域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_ASC, 999 },									/* 57 微信验券信息域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_NO_BCD, 100 },								/* 58 PBOC电子钱包*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 0 },									/* 59 填充空域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_BCD_LEFT, 17 },								/* 60 自定义域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_BCD_LEFT, 29 },								/* 61 原始信息域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_ASC, 512 },									/* 62 自定义域*/
	{ ISO8583_LENGTH_3_CHANGE, ISO8583_CONTENT_NO_BCD, 163 },								/* 63 自定义域*/
	{ ISO8583_LENGTH_FIX, ISO8583_CONTENT_BCD_RIGHT, 8 }									/* 64 报文鉴别码*/
};


int ISO_FILEDS_INIT(FILEDS *init)
{
	memset(init, 0, sizeof(FILEDS));
	for (int i = 0; i < 65; i++)
	{
		init->content[i].is_exist = 0;
		init->content[i].dataLen = 0;
	}
	return 0;
}

int ISO_MSG_INIT(MESSAGE *init)
{
	memset(init, 0, sizeof(MESSAGE));
	memcpy(init->tpdu, ISO8583_TPDU, sizeof(init->tpdu));
	memcpy(init->header, ISO8583_HEADER, sizeof(init->header));
	return 0;
}

/* 组建8583流*/
int ISO8583_PackToStream(PACKSOURCE *packsource, char *output, int *outputLen)
{
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(packsource, &ms);
	ISO8583_MSGSEND2bytes(&ms, output, outputLen);
	return *outputLen;
}

/* 解析8583流*/
int ISO8583_StreamToPack(char *input, int inputLen, UNPACKRESULT *ur)
{
	MSGRECV mr;
	memset(&mr, 0, sizeof(MSGSEND));
	ISO8583_bytes2MSGRECV(input, inputLen, &mr);
	ISO_FILEDS_INIT(ur);
	ISO8583_unpack8583(&mr, ur);
	return 0;
}

/* 组建8583报文 输出要压缩发送的结构体*/
int ISO8583_pack8583(PACKSOURCE *packsource, MSGSEND *msgsend)
{
	ISO_MSG_INIT(msgsend);																	/* 初始化结果结构体 TPDU HEADER */
	int contentFlag = 0;																	/* 正文偏移量,记录当前写入位置*/
	char temp[ISO8583_FILED_DATA_LENGTH];													/* 临时存放域组合内容*/
	int	 filedLen = 0;																		/* 临时域长度*/
	/* 计算位图 正向计算*/
	ISO8583_CALCBITMAT(packsource, NULL);
	/* 组建报文体*/
	for ( int  i = 0; i < 64; ++i)
	{
		memset(temp, 0, sizeof(temp));
		if (packsource->content[i].is_exist == 0) continue;									/* 如果域不存在,则跳过此次处理*/
		filedLen = ISO8583_COMBINE_FILED(&(packsource->content[i]), i, temp);				/* 组合结果放入temp*/
		memcpy(msgsend->content + contentFlag, temp, filedLen);
		contentFlag += filedLen;															/* 运行结束时,contentFlag为总长度*/
	}
	/* 计算总长度 并将结果写入结果*/
	msgsend->length = sizeof(msgsend->tpdu) + sizeof(msgsend->header) + contentFlag;
	return 0;
}

/* 解析8583报文 输出解析后的各域*/
int ISO8583_unpack8583(MSGRECV const *msgrecv, UNPACKRESULT *result)
{
	ISO_FILEDS_INIT(result);
	int startPoint = 0;																		/* 当前解包起始位置*/
	int shiftLength = 0;																	/* 偏移向量*/
	char input[ISO8583_MAX_DATA_LENGTH];													/* 存放收到的正文数据*/
	memset(input, 0, sizeof(input));
	memcpy(input, msgrecv->content, sizeof(msgrecv->content));
	/* 前四字节为消息类型 */
	shiftLength += rules[0].maxLength;
	memcpy(result->content[0].data, input + startPoint, shiftLength);						/* 解析消息类型*/
	result->content[0].dataLen = rules[0].maxLength;
	result->content[0].is_exist = 1;
	startPoint += shiftLength;
	/* 开始解析位图信息 并设置存在域标志*/
	char bitmap[33];																		/* 虽然只有64域,但是考虑128域情况*/
	memset(bitmap, 0, sizeof(bitmap));
	shiftLength = rules[1].maxLength;														/* 乘2 是因为这个为8字节 16位 非BCD*/
	memcpy(bitmap, input+startPoint, shiftLength);
	ISO8583_CALCBITMAT(result, bitmap);
	startPoint += shiftLength;
	/* 开始解析其他域*/
	for (int i = 2; i < 65; i++)
	{
		if (result->content[i].is_exist == 0) continue;
		shiftLength = ISO8583_PRASE_FILED(&result->content[i], i, input + startPoint);
		startPoint += shiftLength;
	}
	// TODO: mac 校验
	/* 解析完毕,返回 */
	return 0;
}

/* 将MSGSEND压缩为发送字节流*/
int ISO8583_MSGSEND2bytes(MSGSEND const *msgsend, char* outbuf, int *outbufLen)
{
	char temp[ISO8583_MAX_DATA_LENGTH];														/* 临时存放源*/	
	memset(temp, 0, sizeof(temp));
	int shift = 0;																			/* 写入偏移*/
	//int msgLen = 0;																		/* 由msgsend->length 获取*/
	char len[5];
	memset(len, 0, sizeof(len));
	sprintf(len, "%04x", msgsend->length/2);
	memcpy(temp + shift, len, 4);															/* 复制长度域信息*/
	shift += 4;
	memcpy(temp + shift, msgsend->tpdu, sizeof(msgsend->tpdu));								
	shift += sizeof(msgsend->tpdu);
	memcpy(temp + shift, msgsend->header, sizeof(msgsend->header));
	shift += sizeof(msgsend->header);
	memcpy(temp + shift, msgsend->content, msgsend->length - sizeof(msgsend->tpdu) - sizeof(msgsend->header));			
	/* 此时temp有效区域的长度应该等于 msgLen + 4 */
	char resultTemp[ISO8583_MAX_DATA_LENGTH];												/* 结果串*/
	memset(resultTemp, 0, sizeof(resultTemp));	
	*outbufLen = ISO8583_BcdStr2Str(temp, msgsend->length + 4, resultTemp);
	memcpy(outbuf, resultTemp, *outbufLen);
	return *outbufLen;
}

/* 将接收字节流解析为MSGRECV*/
int ISO8583_bytes2MSGRECV(char const *recvbuf, int recvbufLen, MSGRECV *msgrecv)
{
	char recv[ISO8583_MAX_DATA_LENGTH];
	memset(recv, 0, sizeof(recv));
	int totalLen = 0;																		/* 总长度*/
	totalLen = ISO8583_Str2BcdStr(recvbuf, recvbufLen, recv);
	if (totalLen <= 26) return -1;
	int shift = 0;																			/* 信息偏移 */
	char lenStr[5];																			/* 长度*/
	memset(lenStr, 0, sizeof(lenStr));
	memcpy(lenStr, recv + shift, sizeof(msgrecv->length));	
	msgrecv->length = ISO8583_HEXStrToInt(lenStr) * 2;										
	shift += 4;
	memcpy(msgrecv->tpdu, recv + shift, sizeof(msgrecv->tpdu));
	shift += sizeof(msgrecv->tpdu);
	memcpy(msgrecv->header, recv + shift, sizeof(msgrecv->header));
	shift += sizeof(msgrecv->header);
	memcpy(msgrecv->content, recv + shift, totalLen - shift);
	return 0;
}


int ISO8583_COMBINE_FILED(FILED const *filed, int filedNum, char *outbuf)
{
	char temp[ISO8583_FILED_DATA_LENGTH];													/* 临时存放域信息*/
	memset(temp, 0, sizeof(temp));
	int ret = 0;																			/* 此域总长度,  Length + Content*/
	RULE rule = rules[filedNum];															/* 得到此域的组包规则*/
	int lengthFiledShift = 0;																/* 长度片段便宜 取值 0 2 4*/
	// 获取长度部分偏移
	switch (rule.lengthMethods)
	{
	case ISO8583_LENGTH_FIX:
		lengthFiledShift = 0;
		break;
	case ISO8583_LENGTH_2_CHANGE:
		lengthFiledShift = 2;
		break;
	case ISO8583_LENGTH_3_CHANGE:
		lengthFiledShift = 4;
		break;
	default:
		lengthFiledShift = 0;
		break;
	}
	// 组建长度部分
	ISO8583_COMBINE_LENGTH(filed->dataLen, rule.lengthMethods, temp);
	// 组建content部分
	char format[12];																		/* 格式化字符串*/
	memset(format, 0, sizeof(format));
	switch (rule.contentMethods)
	{
	case ISO8583_CONTENT_BCD_RIGHT:
		sprintf(format, "%%0%ds", rule.maxLength);
		sprintf(temp + lengthFiledShift, format, filed->data);
		ret = lengthFiledShift + rule.maxLength;
		break;
	case ISO8583_CONTENT_BCD_LEFT:
		if (filed->dataLen % 2 == 0)															/* 长度为偶数 直接复制*/
		{
			sprintf(temp + lengthFiledShift, "%s", filed->data);
			ret = lengthFiledShift + filed->dataLen;
		}
		else																				/* 长度为奇数 复制后并且补 '0' */
		{
			sprintf(temp + lengthFiledShift, "%s0", filed->data);
			ret = lengthFiledShift + filed->dataLen + 1;
		}
		break;
	case ISO8583_CONTENT_NO_BCD:
		ret = lengthFiledShift + ISO8583_Str2BcdStr(filed->data, filed->dataLen, temp + lengthFiledShift);
		break;
	case ISO8583_CONTENT_ASC:
		memcpy(temp + lengthFiledShift, filed->data, filed->dataLen * 2);
		ret = lengthFiledShift + filed->dataLen*2;
		break;
	default:
		break;
	}
	memcpy(outbuf, temp, ret);
	return ret;
}
int ISO8583_PRASE_FILED(FILED *filed, int filedNum, char const *inbuf)
{
	/* 取到此域的解包规则 */
	RULE rule = rules[filedNum];
	/* 根据长度规则取得正文长度 */
	int contLen = 0;
	int contLenLen = 0;																		/* 长度的长度 */
	char lenChar[5]; memset(lenChar, 0, sizeof(lenChar));									/* 标识长度的字节*/
	int filedLength = 0;																	/* 域总长度*/
	switch (rule.lengthMethods)
	{
	case ISO8583_LENGTH_FIX:
		contLenLen = 0;									
		contLen = rule.maxLength;
		filed->dataLen = contLen;
		break;
	case ISO8583_LENGTH_2_CHANGE:
		contLenLen = 2;
		memcpy(lenChar, inbuf, contLenLen);
		contLen = atoi(lenChar);
		filed->dataLen = contLen;
		break;
	case ISO8583_LENGTH_3_CHANGE:
		contLenLen = 4;
		memcpy(lenChar, inbuf, contLenLen);
		contLen = atoi(lenChar);
		filed->dataLen = contLen;
		break;
	default:
		contLenLen = 0;
		break;
	}
	/* 根据正文规则截取正文*/
	filedLength += contLenLen;
	char temp[ISO8583_FILED_DATA_LENGTH]; memset(temp, 0, sizeof(temp));					/* 临时存放截取到的信息*/
	switch (rule.contentMethods)
	{
	case ISO8583_CONTENT_BCD_RIGHT:
			memcpy(temp, inbuf + contLenLen, filed->dataLen);
			filedLength += filed->dataLen;
			memcpy(filed->data, temp, filedLength);
	
		break;
	case ISO8583_CONTENT_BCD_LEFT:
		if (filed->dataLen % 2 == 0)															/* 位数为偶数*/
		{
			memcpy(temp, inbuf + contLenLen, filed->dataLen);
			filedLength += filed->dataLen;
			memcpy(filed->data, temp, filedLength);
		}
		else																				/* 位数为奇数*/
		{
			memcpy(temp, inbuf + contLenLen, filed->dataLen);
			filedLength += filed->dataLen + 1;
			memcpy(filed->data, temp, filedLength);
		}
		break;
	case ISO8583_CONTENT_NO_BCD:
		memcpy(temp, inbuf + contLenLen, filed->dataLen * 2);								/* 长度乘二 */
		filedLength += filed->dataLen * 2;	
		ISO8583_BcdStr2Str(temp, filed->dataLen * 2, filed->data);
		//memcpy(filed->data, temp, filedLength);
		break;
	case ISO8583_CONTENT_ASC:
		memcpy(temp, inbuf + contLenLen, filed->dataLen * 2);
		filedLength += filed->dataLen * 2;
		memcpy(filed->data, temp, filedLength);
		break;
	default:
		break;
	}
	return filedLength;
}

int ISO8583_COMBINE_LENGTH(int len, int lengthRule, char *outbuf)
{
	char temp[5];																			/* 长度最大占4字节 "0123" 并且全部为右靠*/
	int ret = 0;																			/* 返回长度的位数(压缩前,只有0,2,4)*/
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%04d", len);																/* 将长度信息写入temp*/
	switch (lengthRule)
	{
	case ISO8583_LENGTH_FIX:
		ret = 0;
		break;
	case ISO8583_LENGTH_2_CHANGE:
		ret = 2;
		memcpy(outbuf, temp + 2, 2);
		break;
	case ISO8583_LENGTH_3_CHANGE:
		ret = 4;
		memcpy(outbuf, temp, 4);
		break;
	case ISO8583_LENGTH_4_CHANGE:
		ret = 4;
		memcpy(outbuf, temp, 4);
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
}
/* "123"->"313233" */
int ISO8583_Str2BcdStr(char const* input, int inputLen, char* output)
{
	char temp[ISO8583_FILED_DATA_LENGTH];													/* 临时存放转化结果*/
	memset(temp, 0, sizeof(temp));
	for (int i = 0; i < inputLen; i++)
		sprintf(temp + 2 * i, "%02X", (unsigned char)input[i]);
	int retLen = strlen(temp);
	memcpy(output, temp, retLen);
	return retLen;
}

/* "313233" -> "123" */
int ISO8583_BcdStr2Str(char const* input, int inputLen, char* output)
{
	for (int i = 0; i < inputLen/2; i += 1)
	{
		output[i] = (ISO8583_Hex2Int(input[2 * i]) << 4) | (ISO8583_Hex2Int(input[2 * i + 1]));
	}
	return inputLen / 2;
}

/* 16进制字符转int   'a' => 10 */
int ISO8583_Hex2Int(char ch)
{
	if (ch >= '0' && ch <= '9') return ch - '0';
	if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;																				/* 如果从这里返回就完蛋啦 */
}

int ISO8583_CALCBITMAT(FILEDS *fileds, char *bitmap)
{
	int ret = 0;
	char bit_map[8];																		/* 临时存放bitmap*/
	memset(bit_map, 0, sizeof(bit_map));
	if (fileds->content[1].is_exist == 1)													/* 正向计算bitmap 并且写入2域以及bitmap*/
	{
		int flag = 0;																		/* flag 标识位偏移*/
		int index = 7;																		/* 数组下标*/
		for (int i = 64; i >= 2; i--)
		{
			if (flag > 7)
			{
				flag = 0;
				index--;
			}
			if (fileds->content[i].is_exist == 1)
			{
				bit_map[index] = (bit_map[index]) | (0x01 << flag);
			}
			flag++;
		}
		int retLen = ISO8583_Str2BcdStr(bit_map, sizeof(bit_map), fileds->content[1].data);
		fileds->content[1].dataLen = retLen;
		if (bitmap != NULL)
		{
			memcpy(bitmap, bit_map, sizeof(bit_map));
		}
		ret = retLen;
	}
	else																					/* 反向计算,根据bitmap填充fileds*/
	{
		/* 处理BCD字符串格式的bitmap,需要压缩成字节数组*/
		ISO8583_BcdStr2Str(bitmap, strlen(bitmap), bit_map);
		memcpy(fileds->content[1].data, bitmap, strlen(bitmap));
		fileds->content[1].is_exist = 1;
		for (int i = 0; i < 8; i++)
		{
			for (int shift = 0; shift < 8; shift++)
			{
				if ((bit_map[i] << shift & 0x80) == 0x80)
					fileds->content[i * 8 + shift + 1].is_exist = 1;
			}
		}
		ret = 0;
	}
	return ret;
}


/* HEX字符串转int */
int ISO8583_HEXStrToInt(char *hex)
{
	int len;
	int num = 0;
	int bits;
	len = strlen(hex);
	for (int i = 0, temp = 0; i<len; i++, temp = 0)
	{
		temp = ISO8583_Hex2Int(*(hex + i));
		bits = (len - i - 1) * 4;
		temp = temp << bits;
		num = num | temp;
	}
	return num;
}

/*从字符串的中间截取n个字符  n为长度，m为位置 */
char *ISO8583_strMid(char *dst, char const *src, int n, int m)
{
	char const *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n > len) n = len - m;   
	if (m < 0) m = 0;   
	if (m > len) return NULL;
	p += m;
	while (n--) *(q++) = *(p++);
	*(q++) = '\0'; 
	return dst;
}
