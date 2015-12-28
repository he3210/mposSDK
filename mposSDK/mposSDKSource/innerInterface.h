#ifndef INNERINTERFACE_H
#define INNERINTERFACE_H

#define MAX_LEN 8192
#include "iso8583.h"
// 用户输入的JSON字符串转换成的结构体
typedef struct userInputStruct
{
  // 消息类型
	char msg_type[4];
  // 主账号
  char pan[20];
  // 卡有效期
  char exp_time[5];
  // 服务点输入方式
  char ent_code[4];
  // 交易金额
  int txn_Amt;
  // 卡片序列号
  char csn[4];
  // 二磁
  char trk2[38];
  // 三磁
  char trk3[105];
  // 终端号
  char tid[9];
  // 商户号
  char mid[16];
  // 个人标识码
  char pin_data[17];
  // IC 卡数据域
  char ic_data[256];
  // 批次号
  char batch_no[7];
  // 流水号
  char systrace[7];
  // 终端设备信息(机身号)
  char sn_info[51];
  // 借记卡总金额
  char dc_total_amt[13];
  // 贷记卡总金额
  char cc_total_amt[13];
  //  贷记卡总笔数
  char cc_total_cnt[4];
  // 借记卡总笔数
  char dc_total_cnt[4];
  // IC卡参数下载
  char ic_data_item[128];
  // 总笔数
  int total_cnt;
}INMSG;

// 处理结果结构体,将此结构体转换成json字符串返回给caller
typedef struct outputStruct
{
  // 消息类型
  char msg_type[4];
  // 返回码
  char respCode[4];
  // 卡号
  char pan[20];
  // 交易结束时间
  char mchTransETime[15];
  // 流水号
  char systrace[7];
  // 检索参考号
  char rrn[13];
  // 交易金额
  int txnAmt;
  // 授权码
  char auth_code[7];
  // 终端号
  char tid[9];
  // 商户号
  char mid[15];
  // 发卡行
  char bcode[4];
  // 信用卡公司代码
  char cre_code[4];
  // 批次号
  char batch_no[7];
  // 磁道密钥密文
  char tdk[33];
  // 磁道密钥校验值
  char tdk_check[9];
  // 识别码密钥密文
  char zpk[33];
  // 识别码密钥校验值
  char zpk_check[9];
  // 鉴别密钥密文
  char zak[33];
  // 鉴别密钥校验值
  char zak_check[9];
  // 借记卡总金额
  char dc_total_amt[13];
  // 贷记卡总金额
  char cc_total_amt[13];
  //  贷记卡总笔数
  char cc_total_cnt[4];
  // 借记卡总笔数
  char dc_total_cnt[4];
  // 对账应答码
  char acc_st[2];
  // IC卡参数列表
  char ic_para_list[512];
  // 后续是否需要继续请求IC参数表
  char ic_data_que_flag[2];
  // 公钥参数列表
  char key_para_list[512];
  // 后续是否需要继续请求公钥参数表
  char key_data_que_flag[2];
}OUTMSG;


// 对外接口-传入JOSN字符串,传出待MAC计算的BCD字符串以及长度.
int INNER_OUT_SEND_MACSTR_BCD(char const *injson, char *out);
// 对外接口-传入待MAC计算的BCD字符串,以及MAC值,传出要发往posp的BCD字符串以及长度
int INNER_OUT_GET_MAC_BCD(char const *SrcStr, char const *macStr, char *out);
// 对外接口,传入收到的信息,长度,输出解析结果的JSON字符串
int INNER_OUT_PARSE_BCD(char const *recv, char *jsonout);


int INNER_GET_TRANSTYPE(UNPACKRESULT const *ur);

// 将用户输入的json串转换为结构体
int INNER_Json2Struct(char const *json, INMSG *inmsg);
// 将结构体转换成json串
int INNER_Struct2Json(OUTMSG *outmsg, char *json);

/* 签到 */
int INNER_PACK_SIGNUP(INMSG *inStruct, char *outTemp);
/* 签退 */
int INNER_PACK_SIGNOFF(INMSG *inStruct, char *outTemp);

/* 结算 */
int INNER_PACK_SETTLE(INMSG *inStruct, char *outTemp);

/* IC卡参数查询 */
int INNER_PACK_ICPARA_QUERY(INMSG *inStruct, char *outTemp);

/* 公钥查询 */
int INNER_PACK_KEY_QUERY(INMSG *inStruct, char *outTemp);

/* IC卡参数下载 */
int INNER_PACK_ICPARA_DOWM(INMSG *inStruct, char *outTemp);

/* 公钥下载 */
int INNER_PACK_KEY_DOWN(INMSG *inStruct, char *outTemp);

/* IC卡参数下载结束 */
int INNER_PACK_ICPARADOWN_END(INMSG *inStruct, char *outTemp);

/* 公钥下载结束 */
int INNER_PACK_KEYDOWN_END(INMSG *inStruct, char *outTemp);

/* IC卡脚本上送 */
int INNER_PACK_ICSCRIPTUP(INMSG *inStruct, char *outTemp);

/* 批上送脚本通知 */
int INNER_PACK_SETTLE_NOTIFY(INMSG *inStruct, char *outTemp);


/* 支付宝交易 */
int INNER_PACK_ALIPAY_PAY(INMSG *inStruct, char *outTemp);
/* 支付宝交易查询 */
int INNER_PACK_ALIPAY_PAY_QUERY(INMSG *inStruct, char *outTemp);
/* 支付宝冲正 */
int INNER_PACK_ALIPAY_FLUSH(INMSG *inStruct, char *outTemp);
/* 支付宝退货 */
int INNER_PACK_ALIPAY_REFUND(INMSG *inStruct, char *outTemp);

/* 微信交易*/
int INNER_PACK_WX_PAY(INMSG *inStruct, char *outTemp);
/* 微信交易查询*/
int INNER_PACK_WX_PAY_QUERY(INMSG *inStruct, char *outTemp);
/* 微信冲正*/
int INNER_PACK_WX_FLUSH(INMSG *inStruct, char *outTemp);
/* 微信退货*/
int INNER_PACK_WX_REFUND(INMSG *inStruct, char *outTemp);

/* 银联交易*/
int INNER_PACK_UNION_PAY(INMSG *inStruct, char *outTemp);
/* 银联消费冲正*/
int INNER_PACK_UNION_FLUSH(INMSG *inStruct, char *outTemp);
/* 银联消费退货*/
int INNER_PACK_UNION_REFUND(INMSG *inStruct, char *outTemp);
/* 银联消费撤销*/
int INNER_PACK_UNION_REVOKE(INMSG *inStruct, char *outTemp);
/* 银联卡余额查询*/
int INNER_PACK_UNION_QUERY(INMSG *inStruct, char *outTemp);

/* 银联卡预授权*/
int INNER_PACK_UNION_PRE_PAY(INMSG *inStruct, char *outTemp);
/* 银联卡预授权冲正*/
int INNER_PACK_UNION_PRE_PAY_FLUSH(INMSG *inStruct, char *outTemp);
/* 银联卡预授权撤销*/
int INNER_PACK_UNION_PRE_PAY_REVOKE(INMSG *inStruct, char *outTemp);
/* 银联卡预授权完成*/
int INNER_PACK_UNION_PRE_PAY_FINISH(INMSG *inStruct, char *outTemp);
/* 银联卡预授权完成撤销*/
int INNER_PACK_UNION_PRE_PAY_FINSHI_REVOKE(INMSG *inStruct, char *outTemp);

/* 外卡交易*/
int INNER_PACK_OUTSIDE_PAY(INMSG *inStruct, char *outTemp);
/* 外卡消费冲正*/
int INNER_PACK_OUTSIDE_FLUSH(INMSG *inStruct, char *outTemp);
/* 外卡消费撤销*/
int INNER_PACK_OUTSIDE_REVOKE(INMSG *inStruct, char *outTemp);
/* 外卡消费退货*/
int INNER_PACK_OUTSIDE_REFUND(INMSG *inStruct, char *outTemp);

/* 解析交易返回报文 */
int INNER_PARSE_TRANS(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析签到返回报文 */
int INNER_PARSE_SIGNUP(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析签退返回报文 */
int INNER_PARSE_SIGNOFF(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析批结算返回报文 */
int INNER_PARSE_SETTLE(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析IC卡参数查询返回报文*/
int INNER_PARSE_ICPARA_QUERY(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析公钥查询返回报文 */
int INNER_PARSE_KEY_QUERY(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析IC卡参数下载返回报文 */
int INNER_PARSE_ICPARA_DOWNLOAD(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析公钥下载返回报文 */
int INNER_PARSE_KEY_DOWNLOAD(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析IC卡参数下载结束报文 */
int INNER_PARSE_PARADOWM_END(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析公钥下载结束报文 */
int INNER_PARSE_KEYDOWN_END(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析IC卡脚本上送 */
int INNER_PARSE_ICSCRIPT_UP(UNPACKRESULT *ur, char* msgType, char *jsonout);
/* 解析批上送脚本通知 */
int INNER_PARSE_SETTLEUP_NOTIFY(UNPACKRESULT *ur, char* msgType, char *jsonout);

#endif

