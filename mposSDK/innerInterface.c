#include "cJSON.h"
#include "log.h"
#include "iso8583.h"
#include "innerInterface.h"
#include <time.h>
typedef struct msgTypeMap
{
	char msgChar[4];
	int	 msgInt;
}MSG_MAP;
#define TRANS_NUM 31
MSG_MAP msg_map[TRANS_NUM] =
  {
		{ "112", 12 },
		{ "113", 13 },
		{ "114", 14 },
		{ "115", 15 },
		{ "116", 16 },
		{ "117", 17 },
		{ "118", 18 },
		{ "119", 19 },
		{ "120", 20 },
		{ "121", 21 },
		{ "122", 22 },
		{ "123", 23 },
		{ "125", 25 },
		{ "132", 32 },
		{ "133", 33 },
		{ "135", 35 },
		{ "142", 42 },
		{ "143", 43 },
		{ "144", 44 },
		{ "145", 45 },
		{ "081", 81 },
		{ "082", 82 },
		{ "083", 83	},
		{ "084", 84 },
		{ "085", 85 },
		{ "086", 86 },
		{ "087", 87 },
		{ "088", 88 },
		{ "089", 89	},
		{ "090", 90	},
		{ "091", 91 }
  };
// 反向映射表
typedef struct backMapTrans
{
	char token[32];//交易对应字符串
	int  transFlag;//交易类型号
}BACKMAP;
#define BACKMAPNUM 32
// 映射顺序 消息类型&25域&60.1(管理类60.3)域&3域
BACKMAP backMap[BACKMAPNUM] = 
{	
	{ "0210&00&22&000011", 112 },//银联卡消费返回
	{ "0410&00&22&000011", 113 },//银联卡消费冲正返回
	{ "0210&00&23&200011", 114 },//银联卡撤销
	{ "0230&00&25&200011", 115 },//银联卡退货
	{ "0110&06&10&030011", 116 },//银联卡预授权
	{ "0110&06&11&200011", 117 },//银联卡预授权撤销
	{ "0210&06&20&000011", 118 },//银联卡预授权完成
	{ "0210&00&01&310000", 119 },//银联卡余额查询
	{ "0410&06&10&030011", 120 },//银联卡预授权冲正
	{ "0210&06&21&200011", 121 },//银联卡预授权完成撤销
	///	
	{ "0210&00&22&000000", 112 },//银联卡消费返回
	{ "0410&00&22&000000", 113 },//银联卡消费冲正返回
	{ "0210&00&23&200000", 114 },//银联卡撤销
	{ "0230&00&25&200000", 115 },//银联卡退货
	{ "0110&06&10&030000", 116 },//银联卡预授权
	{ "0110&06&11&200000", 117 },//银联卡预授权撤销
	{ "0210&06&20&000000", 118 },//银联卡预授权完成
	{ "0210&00&01&310000", 119 },//银联卡余额查询
	{ "0410&06&10&030000", 120 },//银联卡预授权冲正
	{ "0210&06&21&200000", 121 },//银联卡预授权完成撤销
	//{ "", 122 },//
	//{ "", 123 },//
	//{ "", 124 },//
	{ "0810&00&003&000000", 81  },//签到
	{ "0810&00&004&000000", 81  },//签到
	{ "0830&00&002&000000", 82  },//签退
	{ "0510&00&201&000000", 83  },//结算
	{ "0830&00&382&000000", 84  },//IC卡参数查询
	{ "0830&00&372&000000", 85  },//公钥查询
	{ "0810&00&380&000000", 86  },//IC卡参数下载
	{ "0810&00&370&000000", 87  },//公钥下载
	{ "0810&00&381&000000", 88  },//IC卡参数下载结束
	{ "0810&00&371&000000", 89  },//公钥下载结束
	{ "0330&00&203&000000", 90  },//IC卡脚本上送
	{ "0330&00&207&000000", 91  },//批上送脚本通知
};

int INNER_Json2Struct(char const *json, INMSG *inmsg)
{
  cJSON *jsonroot = NULL, *jsonfind = NULL;
  jsonroot = cJSON_Parse(json);
  if(!jsonroot)
    {// 用户输入串解析失败
      LOG_WriteLog("input json string can't parse correctly.");
      return -1;
    }
  /* 开始解析输入字符串,并且输入进结构体 */
  memset(inmsg, 0, sizeof(INMSG));
  /* 解析消息类型 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"msg_type");
  if(jsonfind == NULL)
    {
      LOG_WriteLog("input msg_type is null.");
      return -1;
    }
  memcpy(inmsg->msg_type, jsonfind->valuestring, strlen(jsonfind->valuestring));
  /* 解析主账号 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"pan");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->pan, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析卡有效期 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"exp_time");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->exp_time, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析服务点输入方法*/
  jsonfind = cJSON_GetObjectItem(jsonroot,"ent_code");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->ent_code, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /*  解析交易金额 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"txn_Amt");
  if(jsonfind != NULL)
    {
	  inmsg->txn_Amt = jsonfind->valueint;
    }
  /* 解析卡片序列号*/
  jsonfind = cJSON_GetObjectItem(jsonroot, "csn");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->csn, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析二磁 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "trk2");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->trk2, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析三磁 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "trk3");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->trk3, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析终端号 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "tid");
  if(jsonfind == NULL)
    {
      LOG_WriteLog("tid msg_type is null.");
      return -1;
    }
  memcpy(inmsg->tid, jsonfind->valuestring, strlen(jsonfind->valuestring));
  /* 解析商户号 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"mid");
  if(jsonfind == NULL)
    {
      LOG_WriteLog("input mid is null.");
      return -1;
    }
  memcpy(inmsg->mid, jsonfind->valuestring, strlen(jsonfind->valuestring));
  /* 解析个人标识码 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "pin_data");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->pin_data, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析IC卡数据域 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "ic_data");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->ic_data, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析批次号 */
  jsonfind = cJSON_GetObjectItem(jsonroot,"batch_no");
  if(jsonfind != NULL)
    {
	  memcpy(inmsg->batch_no, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析流水号                             */
  jsonfind = cJSON_GetObjectItem(jsonroot,"systrace");
  if(jsonfind != NULL)
    {
	  memcpy(inmsg->systrace, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析终端设备信息(机身号) */
  jsonfind = cJSON_GetObjectItem(jsonroot, "sn_info");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->sn_info, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析借记卡总金额 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "dc_total_amt");
  if(jsonfind != NULL)
    {
	  memcpy(inmsg->dc_total_amt, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 解析贷记卡总金额 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "cc_total_amt");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->cc_total_amt, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 贷记卡总笔额 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "cc_total_cnt");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->cc_total_cnt, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* 借记卡总笔额 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "dc_total_cnt");
  if(jsonfind != NULL)
    {
      memcpy(inmsg->dc_total_cnt, jsonfind->valuestring, strlen(jsonfind->valuestring));
    }
  /* IC卡参数下载 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "ic_data_item");
  if (jsonfind != NULL)
  {
	  memcpy(inmsg->ic_data_item, jsonfind->valuestring, strlen(jsonfind->valuestring));
  }
  /* 公钥下载 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "key_data_item");
  if (jsonfind != NULL)
  {
	  memcpy(inmsg->key_data_item, jsonfind->valuestring, strlen(jsonfind->valuestring));
  }
  /* 总笔数 */
  jsonfind = cJSON_GetObjectItem(jsonroot, "total_cnt");
  if (jsonfind != NULL)
  {
	  inmsg->total_cnt = jsonfind->valueint;
	 //memcpy(inmsg->total_cnt, jsonfind->valuestring, strlen(jsonfind->valuestring));
  }
  // todo: add some others
  cJSON_Delete(jsonroot);
  return 0;
}

int INNER_Struct2Json(OUTMSG *outmsg, char *json)
{
  cJSON *jsonroot = NULL;
  jsonroot = cJSON_CreateObject();
  // 开始转换 
  cJSON_AddNumberToObject(jsonroot,"txnAmt",outmsg->txnAmt);//金额
  if(strlen(outmsg->msg_type) != 0 ){  cJSON_AddStringToObject(jsonroot,"msg_type", outmsg->msg_type);}
  if(strlen(outmsg->respCode) != 0 ){  cJSON_AddStringToObject(jsonroot,"respCode", outmsg->respCode);}
  if(strlen(outmsg->pan) != 0 ){  cJSON_AddStringToObject(jsonroot,"pan", outmsg->pan);}
  if(strlen(outmsg->mchTransETime) != 0 ){  cJSON_AddStringToObject(jsonroot,"mchTransETime", outmsg->mchTransETime);}
  if(strlen(outmsg->systrace) != 0 ){  cJSON_AddStringToObject(jsonroot,"systrace", outmsg->systrace);}
  if(strlen(outmsg->rrn) != 0 ){  cJSON_AddStringToObject(jsonroot,"rrn", outmsg->rrn);}
  if(outmsg->txnAmt != 0 ){  cJSON_AddNumberToObject(jsonroot,"txnAmt", outmsg->txnAmt);}
  if(strlen(outmsg->auth_code) != 0 ){  cJSON_AddStringToObject(jsonroot,"auth_code", outmsg->auth_code);}
  if(strlen(outmsg->tid) != 0 ){  cJSON_AddStringToObject(jsonroot,"tid", outmsg->tid);}
  if(strlen(outmsg->mid) != 0 ){  cJSON_AddStringToObject(jsonroot,"mid", outmsg->mid);}
  if(strlen(outmsg->bcode) != 0 ){  cJSON_AddStringToObject(jsonroot,"bcode", outmsg->bcode);}
  if(strlen(outmsg->cre_code) != 0 ){  cJSON_AddStringToObject(jsonroot,"cre_code", outmsg->cre_code);}
  if(strlen(outmsg->batch_no) != 0 ){  cJSON_AddStringToObject(jsonroot,"batch_no", outmsg->batch_no);}
  if(strlen(outmsg->systrace) != 0 ){  cJSON_AddStringToObject(jsonroot,"systrace", outmsg->systrace);}
  if(strlen(outmsg->tdk) != 0 ){  cJSON_AddStringToObject(jsonroot,"tdk", outmsg->tdk);}
  if(strlen(outmsg->tdk_check) != 0 ){  cJSON_AddStringToObject(jsonroot,"tdk_check", outmsg->tdk_check);}
  if(strlen(outmsg->zpk) != 0 ){  cJSON_AddStringToObject(jsonroot,"zpk", outmsg->zpk);}
  if(strlen(outmsg->zpk_check) != 0 ){  cJSON_AddStringToObject(jsonroot,"zpk_check", outmsg->zpk_check);}
  if(strlen(outmsg->zak) != 0 ){  cJSON_AddStringToObject(jsonroot,"zak", outmsg->zak);}
  if(strlen(outmsg->zak_check) != 0 ){  cJSON_AddStringToObject(jsonroot,"zak_check", outmsg->zak_check);}
  if(strlen(outmsg->dc_total_amt) != 0 ){  cJSON_AddStringToObject(jsonroot,"dc_total_amt", outmsg->dc_total_amt);}
  if(strlen(outmsg->cc_total_amt) != 0 ){  cJSON_AddStringToObject(jsonroot,"cc_total_amt", outmsg->cc_total_amt);}
  if(strlen(outmsg->cc_total_cnt) != 0 ){  cJSON_AddStringToObject(jsonroot,"cc_total_cnt", outmsg->cc_total_cnt);}
  if(strlen(outmsg->dc_total_cnt) != 0 ){  cJSON_AddStringToObject(jsonroot,"dc_total_cnt", outmsg->dc_total_cnt);}
  if(strlen(outmsg->acc_st) != 0 ){  cJSON_AddStringToObject(jsonroot,"acc_st", outmsg->acc_st);}
  if(strlen(outmsg->ic_para_list) != 0 ){  cJSON_AddStringToObject(jsonroot,"ic_para_list", outmsg->ic_para_list);}
  if(strlen(outmsg->ic_data_que_flag) != 0 ){  cJSON_AddStringToObject(jsonroot,"ic_data_que_flag", outmsg->ic_data_que_flag);}
  if(strlen(outmsg->key_para_list) != 0 ){  cJSON_AddStringToObject(jsonroot,"key_para_list", outmsg->key_para_list);}
  if(strlen(outmsg->key_data_que_flag) != 0 ){  cJSON_AddStringToObject(jsonroot,"key_data_que_flag", outmsg->key_data_que_flag);}

  // 转换结束
  char *out = cJSON_Print(jsonroot);
  strcpy(json, out);
  free(out);
  cJSON_Delete(jsonroot);
  return 0;
}

// 对外接口-传入JOSN字符串,传出待MAC计算的BCD字符串以及长度.
int INNER_OUT_SEND_MACSTR_BCD(char const *injson, char *out)
{
  //将输入的json串解析为结构体
  INMSG inStruct;
  memset(&inStruct, 0, sizeof(inStruct));
  int ret = 0;
  ret = INNER_Json2Struct(injson, &inStruct);
  if(ret != 0)
    {      return ret; }
  //准备分发交易
  int  choice = 0;
  choice = atoi(inStruct.msg_type);
  char outTemp[MAX_LEN];
  memset(outTemp, 0, sizeof(outTemp));
  switch(choice)
    {
    case 112://银联卡消费
		ret = INNER_PACK_UNION_PAY(&inStruct, outTemp);
		break;
	case 113://银联卡冲正
		ret = INNER_PACK_UNION_FLUSH(&inStruct, outTemp);
		break;
	case 114://银联卡撤销
		ret = INNER_PACK_UNION_REVOKE(&inStruct, outTemp);
		break; 
	case 115://银联卡退货
		ret = INNER_PACK_UNION_REFUND(&inStruct, outTemp);
		break;
	case 116://银联卡预授权
		ret = INNER_PACK_UNION_PRE_PAY(&inStruct, outTemp);
		break;
	case 117://银联卡预授权撤销
		ret = INNER_PACK_UNION_PRE_PAY_REVOKE(&inStruct, outTemp);
		break;
	case 118://银联卡预授权完成
		ret = INNER_PACK_UNION_PRE_PAY_FINISH(&inStruct, outTemp);
		break;
	case 119://银联卡余额查询
		ret = INNER_PACK_UNION_QUERY(&inStruct, outTemp);
		break;
	case 120://预授权冲正
		ret = INNER_PACK_UNION_PRE_PAY_FLUSH(&inStruct, outTemp);
		break;
	case 121://预授权完成撤销
		ret = INNER_PACK_UNION_PRE_PAY_FINSHI_REVOKE(&inStruct, outTemp);
		break;
	case 122://支付宝支付
		ret = INNER_PACK_ALIPAY_PAY(&inStruct, outTemp);
		break;
	case 123://支付宝冲正
		ret = INNER_PACK_ALIPAY_FLUSH(&inStruct, outTemp);
		break;
	case 125://支付宝退货
		ret = INNER_PACK_ALIPAY_REFUND(&inStruct, outTemp);
		break;
	case 132://微信支付
		ret = INNER_PACK_WX_PAY(&inStruct, outTemp);
		break;
	case 133://微信冲正
		ret = INNER_PACK_WX_FLUSH(&inStruct, outTemp);
		break;
	case 135://微信退货
		ret = INNER_PACK_WX_REFUND(&inStruct, outTemp);
		break;
	case 142://外卡消费
		ret = INNER_PACK_OUTSIDE_PAY(&inStruct, outTemp);
		break;
	case 143://外卡消费冲正
		ret = INNER_PACK_OUTSIDE_FLUSH(&inStruct, outTemp);
		break;
	case 144://外卡撤销
		ret = INNER_PACK_OUTSIDE_REVOKE(&inStruct, outTemp);
		break;
	case 145://外卡退货
		ret = INNER_PACK_OUTSIDE_REFUND(&inStruct, outTemp);
		break;
	case 81://签到
		ret = INNER_PACK_SIGNUP(&inStruct, outTemp);
		break;
	case 82://签退
		ret = INNER_PACK_SIGNOFF(&inStruct, outTemp);
		break;
	case 83://结算
		ret = INNER_PACK_SETTLE(&inStruct, outTemp);
		break;
	case 84://IC卡参数查询
		ret = INNER_PACK_ICPARA_QUERY(&inStruct, outTemp);
		break;
	case 85://公钥查询
		ret = INNER_PACK_KEY_QUERY(&inStruct, outTemp);
		break;
	case 86://IC卡参数下载
		ret = INNER_PACK_ICPARA_DOWM(&inStruct, outTemp);
		break;
	case 87://公钥下载
		ret = INNER_PACK_KEY_DOWN(&inStruct, outTemp);
		break;
	case 88://IC卡参数下载结束
		ret = INNER_PACK_ICPARADOWN_END(&inStruct, outTemp);
		break; 
	case 89://公钥下载结束
		ret = INNER_PACK_KEYDOWN_END(&inStruct, outTemp);
		break;
	case 90://IC卡脚本上送
		ret = INNER_PACK_ICSCRIPTUP(&inStruct, outTemp);
		break;
	case 91://批上送脚本通知
		ret = INNER_PACK_SETTLE_NOTIFY(&inStruct, outTemp);
		break;
	default:
		break;
    }
  memcpy(out, outTemp, strlen(outTemp));

  return 0;
}

int INNER_OUT_GET_MAC_BCD(char const *SrcStr, char const *macStr, char *out)
{
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	strcpy(ms.content, SrcStr);
	strcat(ms.content, macStr);
	ms.length = strlen(ms.content) + sizeof(ms.tpdu) + sizeof(ms.header);
	char outTempBytes[MAX_LEN];
	memset(outTempBytes, 0, sizeof(outTempBytes));
	int outLen = 0;
	ISO8583_MSGSEND2bytes(&ms, outTempBytes, &outLen);
	int ret = 0;
	ret = ISO8583_Str2BcdStr(outTempBytes, outLen, out);
	return ret;
}

int INNER_OUT_PARSE_BCD(char const *recv, char *jsonout)
{
	int recvTransType = 0;//接收到的信息的交易类型
	UNPACKRESULT ur;
	ISO_FILEDS_INIT(&ur);
	char recvbytes[MAX_LEN];
	int realLen = 0;//二进制长度
	realLen = ISO8583_BcdStr2Str(recv, strlen(recv), recvbytes);
	ISO8583_StreamToPack(recvbytes, realLen, &ur);
	// TODO:: 分发解包交易 
	recvTransType = INNER_GET_TRANSTYPE(&ur);
	char transTypeStr[8];
	memset(transTypeStr, 0, sizeof(transTypeStr));
	sprintf(transTypeStr, "%03d", recvTransType);
	switch (recvTransType)
	{
	case 112://银联卡消费
	case 113://银联卡冲正
	case 114://银联卡撤销
	case 115://银联卡退货
	case 116://银联卡预授权
	case 117://银联卡预授权撤销
	case 118://银联卡预授权完成
	case 119://银联卡余额查询
	case 120://预授权冲正
	case 121://预授权完成撤销
		INNER_PARSE_TRANS(&ur, transTypeStr, jsonout);
		break;
	case 81: //签到
		INNER_PARSE_SIGNUP(&ur, transTypeStr, jsonout);
		break;
	case 82: //签退
		INNER_PARSE_SIGNOFF(&ur, transTypeStr, jsonout);
		break;
	case 83: //结算
		INNER_PARSE_SETTLE(&ur, transTypeStr, jsonout);
		break;
	case 84: //IC卡参数查询
		INNER_PARSE_ICPARA_QUERY(&ur, transTypeStr, jsonout);
		break;
	case 85: //公钥查询
		INNER_PARSE_KEY_QUERY(&ur, transTypeStr, jsonout);
		break;
	case 86: //IC卡参数下载
		INNER_PARSE_ICPARA_DOWNLOAD(&ur, transTypeStr, jsonout);
		break;
	case 87: //公钥下载
		INNER_PARSE_KEY_DOWNLOAD(&ur, transTypeStr, jsonout);
		break;
	case 88: //IC卡参数下载结束
		INNER_PARSE_PARADOWM_END(&ur, transTypeStr, jsonout);
		break;
	case 89: //公钥下载结束
		INNER_PARSE_KEYDOWN_END(&ur, transTypeStr, jsonout);
		break;
	case 90: //IC卡脚本上送
		INNER_PARSE_ICSCRIPT_UP(&ur, transTypeStr, jsonout);
		break;
	case 91: //批上送脚本通知
		INNER_PARSE_SETTLEUP_NOTIFY(&ur, transTypeStr, jsonout);
		break;
	default:
		break;
	}
	//INNER_PARSE_TRANS(&ur,"001" ,jsonout);
	return 0;
}


/* 银联交易*/
int INNER_PACK_UNION_PAY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为�?不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if(strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "000011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */
	sprintf(ps.content[22].data, "%s",inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}

	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;

	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data)/2;
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	//写入63域
	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联消费冲正*/
int INNER_PACK_UNION_FLUSH(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0400";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "000011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	strcpy(ps.content[39].data, "06");
	ps.content[39].dataLen = strlen(ps.content[39].data);
	ps.content[39].is_exist = 1;
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联消费退货 */
int INNER_PACK_UNION_REFUND(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0220";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "200011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "25%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联消费撤销*/
int INNER_PACK_UNION_REVOKE(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "200011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */
	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联卡余额查询 */
int INNER_PACK_UNION_QUERY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "310000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "01%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 银联卡预授权*/
int INNER_PACK_UNION_PRE_PAY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0100";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "030011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */
	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "10%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联卡预授权冲正*/
int INNER_PACK_UNION_PRE_PAY_FLUSH(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0400";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "030011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "10%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联卡预授权撤销*/
int INNER_PACK_UNION_PRE_PAY_REVOKE(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0100";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "200011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "11%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联卡预授权完成*/
int INNER_PACK_UNION_PRE_PAY_FINISH(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "000011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "20%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 银联卡预授权完成撤销*/
int INNER_PACK_UNION_PRE_PAY_FINSHI_REVOKE(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;
	// 位图 
	ps.content[1].is_exist = 1;
	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}
	/* 交易处理*/
	char *processCodePay = "200011";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 流水号 */
	sprintf(ps.content[11].data, "%06s", inStruct->systrace);
	ps.content[11].dataLen = 12;
	ps.content[11].is_exist = 1;

	/* 14 IC卡有效期 */
	if (strlen(inStruct->exp_time) != 0)
	{
		sprintf(ps.content[14].data, inStruct->exp_time, strlen(inStruct->exp_time));
		ps.content[14].dataLen = strlen(inStruct->exp_time);
		ps.content[14].is_exist = 1;
	}
	/* 22 服务点输入方 */

	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 26域 */
	if (strlen(inStruct->pin_data) != 0 && inStruct->ent_code[2] == '1')
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(inStruct->trk2) != 0)
	{
		sprintf(ps.content[35].data, inStruct->trk2, strlen(inStruct->trk2));
		ps.content[35].dataLen = strlen(inStruct->trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(inStruct->trk3) != 0)
	{
		sprintf(ps.content[36].data, inStruct->trk3, strlen(inStruct->trk3));
		ps.content[36].dataLen = strlen(inStruct->trk3);
		ps.content[36].is_exist = 1;
	}
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	/* 52 密码 */
	if (strlen(inStruct->pin_data) != 0)
	{
		sprintf(ps.content[52].data, inStruct->pin_data, strlen(inStruct->pin_data));
		ps.content[52].dataLen = strlen(inStruct->pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(inStruct->pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 IC卡数据域 */
	if (strlen(inStruct->ic_data) != 0)
	{
		sprintf(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
		ps.content[55].dataLen = strlen(inStruct->ic_data) / 2;
		ps.content[55].is_exist = 1;
	}

	int batchint = 0;
	batchint = atoi(inStruct->batch_no);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "21%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;

	ps.content[64].is_exist = 1;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 外卡交易*/
int INNER_PACK_OUTSIDE_PAY(INMSG *inStruct, char *outTemp)
{
	return 0;
}
/* 外卡消费冲正 */
int INNER_PACK_OUTSIDE_FLUSH(INMSG *inStruct, char *outTemp)
{

	return 0;
}
/* 外卡消费撤销 */
int INNER_PACK_OUTSIDE_REVOKE(INMSG *inStruct, char *outTemp)
{
	
	return 0;
}
/* 外卡消费退货 */
int INNER_PACK_OUTSIDE_REFUND(INMSG *inStruct, char *outTemp)
{
	
	return 0;
}

/* 支付宝交易 */
int INNER_PACK_ALIPAY_PAY(INMSG *inStruct, char *outTemp)
{

	return 0;
}
/* 支付宝交易查询 */
int INNER_PACK_ALIPAY_PAY_QUERY(INMSG *inStruct, char *outTemp)
{

	return 0;
}
/* 支付宝冲正*/
int INNER_PACK_ALIPAY_FLUSH(INMSG *inStruct, char *outTemp)
{
	return 0;
}
/* 支付宝退货 */
int INNER_PACK_ALIPAY_REFUND(INMSG *inStruct, char *outTemp)
{
	return 0;
}

/* 微信交易*/
int INNER_PACK_WX_PAY(INMSG *inStruct, char *outTemp)
{
	return 0;
}
/* 微信交易查询*/
int INNER_PACK_WX_PAY_QUERY(INMSG *inStruct, char *outTemp)
{
	return 0;
}
/* 微信冲正*/
int INNER_PACK_WX_FLUSH(INMSG *inStruct, char *outTemp)
{
	return 0;
}
/* 微信退货 */
int INNER_PACK_WX_REFUND(INMSG *inStruct, char *outTemp)
{
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/* 签到 */
int INNER_PACK_SIGNUP(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	//无TDK
	sprintf(filed60, "00%s0030", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	// SN号写入62域
	if (strlen(inStruct->sn_info) != 0)
	{//TODO:
		char temp62[512];
		memset(temp62, 0, sizeof(temp62));
		strcat(temp62, "Sequence No");
		char snLen[3];
		memset(snLen, 0, sizeof(snLen));
		sprintf(snLen, "%02d", strlen(inStruct->sn_info));
		strcat(temp62, snLen);
		strcat(temp62, inStruct->sn_info);
		char temp62BCD[1024];
		memset(temp62BCD, 0, sizeof(temp62BCD));
		ISO8583_Str2BcdStr(temp62, strlen(temp62), temp62BCD);
		strcat(ps.content[62].data, temp62BCD);
		ps.content[62].dataLen = strlen(ps.content[62].data)/2;
		ps.content[62].is_exist = 1;
	}
	

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;


	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}
/* 签退 */
int INNER_PACK_SIGNOFF(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	char *msgtype = "0820";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s0020", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	
	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);

	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 结算 */
int INNER_PACK_SETTLE(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	char *msgtype = "0500";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	//组建48域
	char dc_total_amt[13];//借记卡总金额
	char cc_total_amt[13];//贷记卡总金额
	char cc_total_cnt[4];//贷记卡总笔数
	char dc_total_cnt[4];//借记卡总笔数
	
	memset(dc_total_amt, 0, sizeof(dc_total_amt));
	memset(cc_total_amt, 0, sizeof(cc_total_amt));
	memset(cc_total_cnt, 0, sizeof(cc_total_cnt));
	memset(dc_total_cnt, 0, sizeof(dc_total_cnt));
	sprintf(dc_total_amt, "%012s", inStruct->dc_total_amt);
	sprintf(dc_total_cnt, "%03s", inStruct->dc_total_cnt);
	sprintf(cc_total_amt, "%012s", inStruct->cc_total_amt);
	sprintf(cc_total_cnt, "%03s", inStruct->cc_total_cnt);
	char filed48[322];
	memset(filed48, 0, sizeof(filed48));
	strcat(filed48, dc_total_amt);
	strcat(filed48, dc_total_cnt);
	strcat(filed48, cc_total_amt);
	strcat(filed48, cc_total_cnt);
	strcat(filed48, "0");//对账应答码
	strcat(filed48, "0000000000000000000000000000000");//外卡

	strcpy(ps.content[48].data, filed48);
	ps.content[48].dataLen = strlen(ps.content[48].data);
	ps.content[48].is_exist = 1;
	/* 写入49域 交易货币代码 */
	sprintf(ps.content[49].data, "156", 3);
	ps.content[49].dataLen = 3;
	ps.content[49].is_exist = 1;
	// 写入 60 域
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s2010", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	// 写入63域
	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;
	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* IC卡参数查询 */
int INNER_PACK_ICPARA_QUERY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	/* 组建签到包 */
	char *msgtype = "0820";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/
	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/
	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	// 382   60.3域 = 382 网络管理码 IC卡参数查询
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s3820", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 62 域 */
	char filed62[513];
	memset(filed62, 0, sizeof(filed62));
	strcat(filed62, "100");
	ISO8583_Str2BcdStr(filed62, strlen(filed62), ps.content[62].data);
	ps.content[62].dataLen = strlen(ps.content[62].data) / 2;
	ps.content[62].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 公钥查询 */
int INNER_PACK_KEY_QUERY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0820";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	// 60.3域 = 372 网络管理码 公钥查询
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s3720", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	// 62
	char filed62[513];
	memset(filed62, 0, sizeof(filed62));
	strcat(filed62, "100");
	ISO8583_Str2BcdStr(filed62, strlen(filed62), ps.content[62].data);
	ps.content[62].dataLen = strlen(ps.content[62].data) / 2;
	ps.content[62].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* IC卡参数下载 */
int INNER_PACK_ICPARA_DOWM(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/
	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/
	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;
	// 组建62 域
	/*TODO: 62域
	三、参数下载交易中采用如下方式构造该域内容：
	1、请求报文中填写需要下载参数的AID，采用TLV格式，每次只能请求一组参数信息
	。若需要请求多组参数，必须反复发送该报文
	2、应答报文中该域的第一个字节为0表示POS中心没有该参数，为1表示后续参数信息。
	公钥信息的格式为：
	AID＋应用选择指示符（ASI）＋TAC－缺省＋TAC－联机＋偏置随机选择的阈值＋TAC－拒绝＋
	终端最低限额＋偏置随机选择的最大目标百分数＋随机选择的目标百分数＋
	缺省DDOL＋终端联机PIN支持能力。
	每个元素都采用TLV的格式，每次只能返回一组参数信息。
	*/

	char ic_data_item[129];
	memset(ic_data_item, 0, sizeof(ic_data_item));
	strcat(ic_data_item, inStruct->ic_data_item);

	strcat(ps.content[62].data, ic_data_item);
	ps.content[62].dataLen = strlen(ps.content[62].data) / 2;
	ps.content[62].is_exist = 1;

	// 60.3域 = 380 网络管理码 IC参数下载
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s3800", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 公钥下载 */
int INNER_PACK_KEY_DOWN(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	/* TODO:
	二、IC卡公钥下载交易中采用如下方式构造该域内容：
	1、请求报文中填写需要下载的公钥RID和索引，采用TLV格式，每次只能请求一组公钥信息。
	若需要请求多组公钥，必须反复发送该报文
	2、应答报文中该域的第一个字节为0表示POS中心没有该公钥，
	为1表示后续有公钥信息。公钥信息的格式为：
	RID+索引＋有效期＋哈什算法标识＋公钥算法标识＋公钥模＋公钥指数＋公钥校验值。
	每个元素都采用TLV的格式，每次只能返回一组公钥信息。
	*/
	// 60.3域 = 370 网络管理码 公钥下载
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s3700", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	// 62 
	char key_data_item[129];
	memset(key_data_item, 0, sizeof(key_data_item));
	strcat(key_data_item, inStruct->key_data_item);
	strcat(ps.content[62].data, key_data_item);
	ps.content[62].dataLen = strlen(ps.content[62].data) / 2;
	ps.content[62].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* IC卡参数下载结束 */
int INNER_PACK_ICPARADOWN_END(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	
	// 60.3域 = 381 网络管理码 IC卡参数下载结束
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s3810", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 公钥下载结束 */
int INNER_PACK_KEYDOWN_END(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	// 流水号
	/*sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;*/

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	// 60.3域 = 371 网络管理码 公钥下载结束
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s3710", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* IC卡脚本上送 */
int INNER_PACK_ICSCRIPTUP(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	/* 组建签到包 */
	char *msgtype = "0320";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取PAN .如果PAN不为�?不取PAN.使用原PAN */
	if (strlen(inStruct->pan) == 0)
	{
		/* pan 为空 trk2不为空 */
		if (strlen(inStruct->trk2) != 0)
		{
			/* 由二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, inStruct->trk2, strlen(inStruct->trk2));
			//开始根据分隔符截取卡号. " d" 或 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(inStruct->pan, pantemp, strlen(pantemp));
			}
		}
	}

	/* 二域  卡号信息 */
	if (strlen(inStruct->pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, inStruct->pan, strlen(inStruct->pan));
		ps.content[2].dataLen = strlen(inStruct->pan);
	}

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	/* 金额 */
	sprintf(ps.content[4].data, "%012d", inStruct->txn_Amt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	// 流水号
	sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;

	/* 22 服务点输入方 */
	sprintf(ps.content[22].data, "%s", inStruct->ent_code);
	ps.content[22].dataLen = strlen(ps.content[22].data);
	ps.content[22].is_exist = 1;

	/* 23 IC卡序列号 */
	if (strlen(inStruct->csn) != 0)
	{
		sprintf(ps.content[23].data, inStruct->csn, strlen(inStruct->csn));
		ps.content[23].dataLen = strlen(inStruct->csn);
		ps.content[23].is_exist = 1;
	}

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	/* 终端号*/
	memcpy(ps.content[41].data, inStruct->tid, strlen(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;
	/* 商户号*/
	memcpy(ps.content[42].data, inStruct->mid, strlen(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;
	/* IC 卡数据域 */
	memcpy(ps.content[55].data, inStruct->ic_data, strlen(inStruct->ic_data));
	ps.content[55].dataLen = strlen(inStruct->ic_data)/2;
	ps.content[55].is_exist = 1;
	// 60.3域 = 203 网络管理码 对账平上送成功IC卡联机交易明细
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%06s2030", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	// 62 域
	char filed62[512];
	char filed62bcd[512];
	memset(filed62, 0, sizeof(filed62));
	memset(filed62bcd, 0, sizeof(filed62bcd));
	sprintf(filed62, "610000%012s156", ps.content[4].data);
	ISO8583_Str2BcdStr(filed62, strlen(filed62), filed62bcd);
	strcat(ps.content[62].data, filed62bcd);
	ps.content[62].dataLen = strlen(ps.content[62].data)/2;
	ps.content[62].is_exist = 1;

	//
	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}

/* 批上送脚本通知 */
int INNER_PACK_SETTLE_NOTIFY(INMSG *inStruct, char *outTemp)
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	/* 组建签到包 */
	char *msgtype = "0320";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;

	/* 交易处理码 */
	/*char *processCodePay = "000000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;*/

	// 流水号
	sprintf(ps.content[11].data, inStruct->systrace, sizeof(inStruct->systrace));
	ps.content[11].dataLen = strlen(inStruct->systrace);
	ps.content[11].is_exist = 1;

	/* 25域 */
	/*sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;*/

	sprintf(ps.content[41].data, inStruct->tid, sizeof(inStruct->tid));
	ps.content[41].dataLen = strlen(inStruct->tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, inStruct->mid, sizeof(inStruct->mid));
	ps.content[42].dataLen = strlen(inStruct->mid);
	ps.content[42].is_exist = 1;

	/* 48 域*/
	char total_cnt[5];
	memset(total_cnt, 0, sizeof(total_cnt));
	sprintf(total_cnt, "%04d", inStruct->total_cnt);
	strcat(ps.content[48].data,total_cnt);
	ps.content[48].dataLen = strlen(ps.content[48].data);
	ps.content[48].is_exist = 1;
	// 60.3域 = 207 网络管理码 对账平批上送结束
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s2070", inStruct->batch_no);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	ps.content[64].is_exist = 0;//64域不存在
	MSGSEND ms;
	ISO_MSG_INIT(&ms);
	ISO8583_pack8583(&ps, &ms);
	memcpy(outTemp, ms.content, strlen(ms.content));
	return 0;
}


/* 解析交易返回报文 */
int INNER_PARSE_TRANS(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{return -100;}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取发卡行代号
	if (strlen(ur->content[44].data) != 0)
	{
		char bcode[13]; memset(bcode, 0, sizeof(bcode));
		memcpy(bcode, ur->content[44].data , 11);
		cJSON_AddStringToObject(outroot, "bcode", bcode);
	}
	
	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析签到返回报文 */
int INNER_PARSE_SIGNUP(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{	return -100; }
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);
	// 获取发卡行代号
	if (strlen(ur->content[44].data) != 0)
	{
		char bcode[13]; memset(bcode, 0, sizeof(bcode));
		memcpy(bcode, ur->content[44].data, 11);
		cJSON_AddStringToObject(outroot, "bcode", bcode);
	}

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}

	char PIKStr[33], PIKCheckValue[9];										/* PIK密文 以及 校验值*/
	char MAKStr[17], MAKCheckValue[9];										/* MAK密文 以及 校验值*/
	char TDKStr[17], TDKCheckValue[9];										/* TDK密文 以及 校验值*/
	
	memset(PIKStr, 0, sizeof(PIKStr));
	memset(PIKCheckValue, 0, sizeof(PIKCheckValue));
	memset(MAKStr, 0, sizeof(MAKStr));
	memset(MAKCheckValue, 0, sizeof(MAKCheckValue));
	memset(TDKStr, 0, sizeof(TDKStr));
	memset(TDKCheckValue, 0, sizeof(TDKCheckValue));

	memcpy(PIKStr, ur->content[62].data, 32);
	memcpy(PIKCheckValue, ur->content[62].data + 32, 8);
	memcpy(MAKStr, ur->content[62].data + 40, 16);
	memcpy(MAKCheckValue, ur->content[62].data + 72, 8);
	memcpy(TDKStr, ur->content[62].data + 80, 16);
	memcpy(TDKCheckValue, ur->content[62].data + 112, 8);

	cJSON_AddStringToObject(outroot, "tdk", TDKStr);
	cJSON_AddStringToObject(outroot, "tdk_check", TDKCheckValue);
	cJSON_AddStringToObject(outroot, "zpk", PIKStr);
	cJSON_AddStringToObject(outroot, "zpk_check", PIKCheckValue);
	cJSON_AddStringToObject(outroot, "zak", MAKStr);
	cJSON_AddStringToObject(outroot, "zak_check", MAKCheckValue);

	char *outchar1 = cJSON_Print(outroot);
	memcpy(jsonout, outchar1, strlen(outchar1));
	free(outchar1);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析签退返回报文 */
int INNER_PARSE_SIGNOFF(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析批结算返回报文 */
int INNER_PARSE_SETTLE(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	// 开始处理结算返回报文
	char dc_total_amt[13];//借记卡总金额
	char cc_total_amt[13];//贷记卡总金额
	char cc_total_cnt[4];//贷记卡总笔数
	char dc_total_cnt[4];//借记卡总笔数
	char acc_st[2];//对账应答码
	memset(dc_total_amt, 0, sizeof(dc_total_amt));
	memset(cc_total_amt, 0, sizeof(cc_total_amt));
	memset(cc_total_cnt, 0, sizeof(cc_total_cnt));
	memset(dc_total_cnt, 0, sizeof(dc_total_cnt));
	memset(acc_st, 0, sizeof(acc_st));
	memcpy(dc_total_amt, ur->content[48].data, 12);
	memcpy(dc_total_cnt, ur->content[48].data + 12, 3);
	memcpy(cc_total_amt, ur->content[48].data + 15, 12);
	memcpy(cc_total_cnt, ur->content[48].data + 27, 3);
	memcpy(acc_st, ur->content[48].data + 30, 1);
	cJSON_AddStringToObject(outroot, "dc_total_amt", dc_total_amt);
	cJSON_AddStringToObject(outroot, "cc_total_amt", cc_total_amt);
	cJSON_AddStringToObject(outroot, "cc_total_cnt", cc_total_cnt);
	cJSON_AddStringToObject(outroot, "dc_total_cnt", dc_total_cnt);
	cJSON_AddStringToObject(outroot, "acc_st", acc_st);

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
	return 0;
}
/* 解析IC卡参数查询返回报文*/
int INNER_PARSE_ICPARA_QUERY(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	//开始处理IC卡参数查询特殊内容
	/*IC卡参数下载交易中采用如下方式构造该域内容：
	1、请求报文中该域占三个字节，第一个字节为数字1，表示是参数信息查询报文；
	后面两个字节联合起来表示POS收到的所有参数信息个数，所以首先上送该请求交易时，整个域取值应为100
	2、应答报文中该域的第一个字节为数字0表示POS中心没有参数信息，为数字1表示后续有参数信息，且一个报文就可以存放下所有的参数信息。
	后续IC参数信息的格式如下：
	AID1，AID2,...,AIDn。同样采用TLV的格式
	3、应答报文中该域的第一个字节为数字2表示一个报文无法存放所有的参数信息，需要POS终端再上送请求报文，
	在与该请求报文对应的应答报文中存放后续的参数信息，参数信息格式如2中所示。以2开头的应答报文可以反复出现。
	当参数信息是最后一组时，该位置需要填写
	3。当终端收到以3开头的应答报文后不再上送请求报文。
   */
	char ic_para_list[512];
	char ic_data_que_flag[2];
	memset(ic_para_list, 0, sizeof(ic_para_list));
	memset(ic_data_que_flag, 0, sizeof(ic_data_que_flag));

	memcpy(ic_data_que_flag, ur->content[62].data+1, 1);
	memcpy(ic_para_list, ur->content[62].data + 2, strlen(ur->content[62].data) - 1);
	cJSON_AddStringToObject(outroot, "ic_data_que_flag", ic_data_que_flag);
	cJSON_AddStringToObject(outroot, "ic_para_list", ic_para_list);

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析公钥查询返回报文 */
int INNER_PARSE_KEY_QUERY(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	//开始处理IC卡公钥查询特殊内容
	/*
	二、IC卡公钥下载交易中采用如下方式构造该域内容：
	1、请求报文中该域占三个字节，第一个字节为数字1，表示是认证中心公钥信息查询报文；
	后面两个字节联合起来表示POS收到的所有公钥信息个数，所以首先上送该请求交易时，整个域取值应为100。
	2、应答报文中该域的第一个字节为数字0表示POS中心没有公钥信息，为数字1表示后续有公钥信息，
	且一个报文就可以存放下所有的公钥信息。后续公钥信息的格式如下：
	RID1，索引1，有效期1；RID2，索引2，有效期2,...,RIDn，索引n，有效期n。同样采用TLV的格式
	3、应答报文中该域的第一个字节为数字2表示一个报文无法存放所有的公钥信息，需要POS终端再上送请求报文，
	在与该请求报文对应的应答报文中存放后续的公钥信息，公钥信息格式如2中所示。需要注意的是，公钥信息都必须是成组出现的，
	不能在第一个应答报文中存放一组公钥信息的前半部分，在第二个应答报文中存放其后半部分。以2开头的应答报文可以反复出现。
	当公钥信息是最后一组时，该位置需要填写3。当终端收到以3开头的应答报文后不再上送请求报文。
	*/
	char key_para_list[512];
	char key_data_que_flag[2];
	memset(key_para_list, 0, sizeof(key_para_list));
	memset(key_data_que_flag, 0, sizeof(key_data_que_flag));

	memcpy(key_data_que_flag, ur->content[62].data + 1, 1);
	memcpy(key_para_list, ur->content[62].data + 2, strlen(ur->content[62].data) - 2);
	cJSON_AddStringToObject(outroot, "key_data_que_flag", key_data_que_flag);
	cJSON_AddStringToObject(outroot, "key_para_list", key_para_list);

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析IC卡参数下载返回报文 */
int INNER_PARSE_ICPARA_DOWNLOAD(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	//开始处理IC卡公钥查询特殊内容
	/*
	三、参数下载交易中采用如下方式构造该域内容：
	1、请求报文中填写需要下载参数的AID，采用TLV格式，每次只能请求一组参数信息。若需要请求多组参数，必须反复发送该报文
	2、应答报文中该域的第一个字节为0表示POS中心没有该参数，为1表示后续参数信息。公钥信息的格式为：
	AID＋应用选择指示符（ASI）＋TAC－缺省＋TAC－联机＋偏置随机选择的阈值＋TAC－拒绝＋终端最低限额＋
	偏置随机选择的最大目标百分数＋随机选择的目标百分数＋缺省DDOL＋终端联机PIN支持能力。
	每个元素都采用TLV的格式，每次只能返回一组参数信息。
	*/
	char ic_data_exist_flag[2];
	//TODO:
	char ic_data_item[512];
	char ic_para_info[512];
	memset(ic_data_exist_flag, 0, sizeof(ic_data_exist_flag));
	memset(ic_para_info, 0, sizeof(ic_para_info));
	memset(ic_data_item, 0, sizeof(ic_data_item));

	memcpy(ic_data_exist_flag, ur->content[62].data+1, 1);
	// IC data item 
	char lenitem[3];
	memset(lenitem, 0, sizeof(lenitem));
	memcpy(lenitem, ur->content[62].data +2 + 4, 2);
	int lenitemint = atoi(lenitem) * 2;
	memcpy(ic_data_item, ur->content[62].data+2, 4 + 2 + lenitemint);
	//memcpy(key_data_que_flag, ur->content[62].data, 1);
	memcpy(ic_para_info, ur->content[62].data + 4 + 2 + 2 + lenitemint, strlen(ur->content[62].data) - 4 - 2 - 2 - lenitemint);
	cJSON_AddStringToObject(outroot, "ic_data_exist_flag", ic_data_exist_flag);
	cJSON_AddStringToObject(outroot, "ic_para_info", ic_para_info);
	cJSON_AddStringToObject(outroot, "ic_data_item", ic_data_item);
	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析公钥下载返回报文 */
int INNER_PARSE_KEY_DOWNLOAD(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	//开始处理IC卡公钥查询特殊内容
	/*
	二、IC卡公钥下载交易中采用如下方式构造该域内容：
	1、请求报文中填写需要下载的公钥RID和索引，采用TLV格式，每次只能请求一组公钥信息。若需要请求多组公钥，必须反复发送该报文
	2、应答报文中该域的第一个字节为0表示POS中心没有该公钥，为1表示后续有公钥信息。公钥信息的格式为：
	RID+索引＋有效期＋哈什算法标识＋公钥算法标识＋公钥模＋公钥指数＋公钥校验值。每个元素都采用TLV的格式，每次只能返回一组公钥信息。
	*/
	char key_data_exist_flag[2];
	//TODO:
	char key_data_item[512];
	char key_para_info[512];
	memset(key_data_exist_flag, 0, sizeof(key_data_exist_flag));
	memset(key_para_info, 0, sizeof(key_para_info));
	memset(key_data_item, 0, sizeof(key_data_item));

	memcpy(key_data_exist_flag, ur->content[62].data + 1, 1);
	// IC data item 
	char lenitem[3];
	memset(lenitem, 0, sizeof(lenitem));
	memcpy(lenitem, ur->content[62].data + 2 + 4, 2);
	int lenitemint = atoi(lenitem) * 2;
	memcpy(key_data_item, ur->content[62].data + 2, 4 + 2 + lenitemint);
	//memcpy(key_data_que_flag, ur->content[62].data, 1);
	memcpy(key_para_info, ur->content[62].data + 4 + 2 + 2 + lenitemint, strlen(ur->content[62].data) - 4 - 2 - 2 - lenitemint);
	cJSON_AddStringToObject(outroot, "key_data_exist_flag", key_data_exist_flag);
	cJSON_AddStringToObject(outroot, "key_para_info", key_para_info);
	cJSON_AddStringToObject(outroot, "key_data_item", key_data_item);

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析IC卡参数下载结束报文 */
int INNER_PARSE_PARADOWM_END(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	
	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析公钥下载结束报文 */
int INNER_PARSE_KEYDOWN_END(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析IC卡脚本上送 */
int INNER_PARSE_ICSCRIPT_UP(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}
/* 解析批上送脚本通知 */
int INNER_PARSE_SETTLEUP_NOTIFY(UNPACKRESULT *ur, char* msgType, char *jsonout)
{
	cJSON *outroot = NULL;
	outroot = cJSON_CreateObject();
	if (outroot == NULL)
	{
		return -100;
	}
	if (strlen(ur->content[39].data) != 0)	cJSON_AddStringToObject(outroot, "respCode", ur->content[39].data);
	if (strlen(ur->content[2].data) != 0)	cJSON_AddStringToObject(outroot, "pan", ur->content[2].data);
	/*开始组建返回时间*/
	char mchTransETime[17];
	memset(mchTransETime, 0, sizeof(mchTransETime));
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);
	cJSON_AddStringToObject(outroot, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(outroot, "msg_type", msgType);
	if (strlen(ur->content[11].data) != 0)	cJSON_AddStringToObject(outroot, "systrace", ur->content[11].data);
	if (strlen(ur->content[37].data) != 0)	cJSON_AddStringToObject(outroot, "rrn", ur->content[37].data);
	if (strlen(ur->content[4].data) != 0)	cJSON_AddNumberToObject(outroot, "txn_Amt", atoi(ur->content[4].data));
	if (strlen(ur->content[38].data) != 0)	cJSON_AddStringToObject(outroot, "auth_code", ur->content[38].data);
	if (strlen(ur->content[41].data) != 0)	cJSON_AddStringToObject(outroot, "tid", ur->content[41].data);
	if (strlen(ur->content[42].data) != 0)	cJSON_AddStringToObject(outroot, "mid", ur->content[42].data);
	if (strlen(ur->content[63].data) != 0)	cJSON_AddStringToObject(outroot, "cre_code", ur->content[63].data);

	// 获取批次号 60.2
	if (strlen(ur->content[60].data) != 0)
	{
		char batch[7]; memset(batch, 0, sizeof(batch));
		memcpy(batch, ur->content[60].data + 2, 6);
		cJSON_AddStringToObject(outroot, "batch_no", batch);
	}
	// 获取批上送脚本通知的 total_cnt
	char total_cnt_str[8];
	memset(total_cnt_str, 0, sizeof(total_cnt_str));
	memcpy(total_cnt_str, ur->content[48].data, 4);
	int total_cnt = 0;
	total_cnt = atoi(total_cnt_str);
	cJSON_AddNumberToObject(outroot, "total_cnt", total_cnt);

	char *outchar = cJSON_Print(outroot);
	memcpy(jsonout, outchar, strlen(outchar));
	free(outchar);
	cJSON_Delete(outroot);
	return 0;
}


int INNER_GET_TRANSTYPE(UNPACKRESULT const *ur)
{
	int ret = 0;
	char tokenStr[32];
	memset(tokenStr, 0, sizeof(tokenStr));
	//开始组建token串 消息类型&25域&60.1(管理类60.3)域&3域
	strcat(tokenStr, ur->content[0].data);
	strcat(tokenStr, "&");
	if (strlen(ur->content[25].data) == 0)
	{
		strcat(tokenStr, "00");
	}
	else
	{
		strcat(tokenStr, ur->content[25].data);
	}
	//strcat(tokenStr, ur->content[25].data);
	strcat(tokenStr, "&");
	// 根据 msgType 组建管理类与非管理类 token
	char token3[10];// 在token中的第三要素. 60.1 或者 60.3
	memset(token3, 0, sizeof(token3));
	if (strcmp(ur->content[0].data,"0210") == 0)
	{
		memcpy(token3, ur->content[60].data, 2);
	}
	else if (strcmp(ur->content[0].data, "0410") == 0)
	{
		memcpy(token3, ur->content[60].data, 2);
	}
	else if (strcmp(ur->content[0].data, "0210") == 0)
	{
		memcpy(token3, ur->content[60].data, 2);
	}
	else if (strcmp(ur->content[0].data, "0230") == 0)
	{
		memcpy(token3, ur->content[60].data, 2);
	}
	else if (strcmp(ur->content[0].data, "0110") == 0)
	{
		memcpy(token3, ur->content[60].data, 2);
	}
	else if (strcmp(ur->content[0].data, "0810") == 0)
	{	//
		memcpy(token3, ur->content[60].data + 8, 3);
	}
	else if (strcmp(ur->content[0].data, "0830") == 0)
	{	//
		memcpy(token3, ur->content[60].data + 8, 3);
	}
	else if (strcmp(ur->content[0].data, "0510") == 0)
	{	//
		memcpy(token3, ur->content[60].data + 8, 3);
	}
	else if (strcmp(ur->content[0].data, "0330") == 0)
	{	//
		memcpy(token3, ur->content[60].data + 8, 3);
	}
	else if (strcmp(ur->content[0].data, "0320") == 0)
	{	//
		memcpy(token3, ur->content[60].data + 8, 3);
	}
	else
	{
		//未知的消息类型
	}
	strcat(tokenStr, token3);
	strcat(tokenStr, "&");
	if (strlen(ur->content[3].data) == 0)
	{
		strcat(tokenStr, "000000");
	}
	else
	{
		strcat(tokenStr, ur->content[3].data);
	}
	for (int i = 0; i < BACKMAPNUM; i++)
	{
		if (strcmp(backMap[i].token,tokenStr) == 0)
		{
			ret = backMap[i].transFlag;
			break;
		}
	}
	return ret;
}


unsigned short FormXZZFTlv(char *lpOut, char *tag, char taglen, char *value, unsigned short valuelen)
{
	unsigned short outLen = 0;
	//char printbuf[64] = { 0 };
	memcpy(lpOut, tag, taglen);
	outLen += taglen;
	if (valuelen > 0)
	{
		if (valuelen > 255)
		{
			lpOut[outLen++] = 0x82;
			lpOut[outLen++] = (char)(valuelen >> 8);
		}
		else if (valuelen > 127)
		{
			lpOut[outLen++] = 0x81;
		}
		lpOut[outLen++] = (char)valuelen;
		memcpy(&lpOut[outLen], value, valuelen);
		outLen += valuelen;
	}
	else
	{
		outLen = 0;
	}
	return outLen;
}
