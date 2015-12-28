#include "test.h"
#include <Windows.h>


int test_read_file(char const *fileName, char *buf)
{
	FILE *pFile = fopen(fileName, "a+");
	char pBuf[MAX_LEN];
	memset(pBuf, 0, sizeof(pBuf));
	char single[1024];
	memset(single, 0, sizeof(single));
	while ((fgets(single, MAX_LEN, pFile)) != NULL)
	{
		strcat(pBuf, single);
	}

	//fread(pBuf, 1, MAX_LEN, pFile);
	fclose(pFile);
	memcpy(buf, pBuf, strlen(pBuf));
	return strlen(pBuf);
}
int test_Union_Pay(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");

	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "1234567890123456";

	ret = packmac(macTest, recv);
	printf("Print pack recv : \n%s \n", recv);

	printf("解包函数测试:");
	char *unpackStr = "009f60000000066031009999990210703e02810ed082131962122602000241255730000000000000073700002332203217082723020827000100084803100032303332313734393138323730313233343530303030303036353930373133313030303538313230363131223031303230303030202020343830333130303020202031353600059f3602004700142200006500050100034355503635304341433741";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}

int test_Signup(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");

	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("签到不需要MAC校验 :\n%s\n",recv);

	printf("解包函数测试:\n");
	char *unpackStr = "007460000000026031003100300810003800000ac0001400044109130909083135303930383833363032383030303030303131343231323334353637383930313233343500110000000600300040279be86aaeb174b80bbe2b24e80f888cebe8b6c2be7bc61956bbf2480000000000000000a2f2cabb";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}

int test_Signoff(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");

	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("签退不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "004a60000000026031003100300830003800000ac000100004471608130908313530393038383336303939303030303030313134323132333435363738393031323334350011000000070020";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}

int test_Settle(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("结算不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "006e60000000026031003100300510003a000008c180100004451608030908090831353039303838333630393730303030313134323132333435363738393031323334350062000000000011011000000000002002100000000000000000000000000000013135360011000000062010";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}

int test_IcparaQuery(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("IC卡参数查询不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "00b260000000026031003100300830001800000ac0001414431609093135303930393833363436313030303030303131343031323334353637383930313233343500110000000138200105319f0608a0000003330101069f0608a0000003330101039f0608a0000003330101029f0608a0000003330101019f0607a00000006510109f0607a00000000430609f0607a00000000410109f0607a00000000330109f0607a00000000320109f0607a0000000031010";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}

int test_IcparaDown(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("IC卡参数下载不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "00b960000000026031003100300810001800000ac0001414431609093135303930393833363436323030303030303131343031323334353637383930313233343500110000000138000112319f0607a0000000031010df0101009f08020140df1105d84000a800df1205d84004f800df130500100000009f1b0400000000df150400000000df160199df170199df14039f3704df1801019f7b06000000010000df1906000000000001df2006000000010000df2106000000010000";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}
int test_IcparaDownEnd(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("IC卡参数下载结束不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "004760000000026031003100300810001800000ac000100931130910313530393130343437323530303030303030303139373934373130303037333932303030310011000001033810";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}

int test_KeyQuery(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("密钥查询不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "00bd60000000026031003100300830001800000ac0001414431009093135303930393833363435333030303030303131343031323334353637383930313233343500110000000137200116319f0605a0000000039f220109df050832303136313233319f0605a0000000049f220106df050832303136313233319f0605a0000000659f220114df050832303136313233319f0605a0000003339f22010bdf050832303136313233319f0605a0000003339f220108df05083230323031323331";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}
int test_KeyDown(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("密钥下载不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "011860000000026031003100300810001800000ac0001414431009093135303930393833363435343030303030303131343031323334353637383930313233343500110000000137000207319f0605a0000003339f220108df05083230323031323331df060101df070101df028190b61645edfd5498fb246444037a0fa18c0f101ebd8efa54573ce6e6a7fbf63ed21d66340852b0211cf5eef6a1cd989f66af21a8eb19dbd8dbc3706d135363a0d683d046304f5a836bc1bc632821afe7a2f75da3c50ac74c545a754562204137169663cfcc0b06e67e2109eba41bc67ff20cc8ac80d7b6ee1a95465b3b2657533ea56d92d539e5064360ea4850fed2d1bfdf040103df0314ee23b616c95c02652ad18860e48787c079e8e85a";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}
int test_KeyDownEnd(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("密钥下载结束不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "004760000004016000001508200810001800000ac000101529190910313530393130343532393938303030303030383835333731333130303038323939303032390011000000013710";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}

int test_IcScriptUp(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("IC卡脚本上送不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "006360000000066031009999990330503802010ac00010196228480018552050470000000200000000308909525209110000080003100030393532353236343531373430303030303034373832373133313030303538313230343539001300000253203500";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;

}
int test_SettleNotify(char const *fileName)
{
	char injson[MAX_LEN];
	memset(injson, 0, sizeof(injson));
	test_read_file(fileName, injson);
	HINSTANCE Hint = LoadLibrary("mposSDK.dll");
	ipaynowPack8583 pack = (ipaynowPack8583)GetProcAddress(Hint, "ipaynowPack8583");
	ipaynowGet8583Msg packmac = (ipaynowGet8583Msg)GetProcAddress(Hint, "ipaynowGet8583Msg");
	ipaynowUnpack8583 unpack = (ipaynowUnpack8583)GetProcAddress(Hint, "ipaynowUnpack8583");
	char recv[MAX_LEN];
	memset(recv, 0, sizeof(recv));
	int ret = 0;
	ret = pack(injson, recv);
	printf("Print pack recv : \n%s \n", recv);
	char *macTest = "";

	ret = packmac(macTest, recv);
	printf("批上送脚本通知不需要MAC校验 :\n%s\n", recv);

	printf("解包函数测试:\n");
	char *unpackStr = "004e60000000026031003100300330003800000ac10010000446160804090831353039303838333630393830303030303031313432313233343536373839303132333435000400000011000000062070";
	char jsonout[MAX_LEN];
	memset(jsonout, 0, sizeof(jsonout));
	strcpy(jsonout, unpackStr);
	unpack(jsonout);
	printf(jsonout);
	return 0;
}