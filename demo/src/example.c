#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "oslib.h"
#include "main_api.h"
#include "define.h"


//二维码句柄
int qr_fd = 0;


//射频卡槽定义
#define RCFUSE 0

/**
 * @Author: hhl \@decard
 * @fn     	void MyHexA(unsigned char* hex, unsigned char* a, int length);
 * @brief:	hex->asc
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  15:34:52
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  15:34:52
*/
void MyHexA(unsigned char* hex, unsigned char* a, int length)
{
    unsigned char hLowbit, hHighbit;
    int i;
    for (i = 0; i < length *2; i += 2)
    {
        hLowbit = hex[i / 2] & 0x0f;
        hHighbit = hex[i / 2] / 16;
        if (hHighbit >= 10)
            a[i] = hHighbit + '7';
        else
            a[i] = hHighbit + '0';
        if (hLowbit >= 10)
            a[i + 1] = hLowbit + '7';
        else
            a[i + 1] = hLowbit + '0';
    }
    a[length * 2] = '\0';
}

/**
 * @Author: hhl \@decard
 * @fn     	int exp_set_call_bak(void);
 * @brief:	回调函数注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  18:51:13
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  18:51:13
*/
int exp_set_call_bak(void)
{
	/*
	//icc接口注册
	set_call_ICC_Init(ICC_Init);
	set_call_ICC_PowerUp(ICC_PowerUp);
	set_call_ICC_Close(ICC_Close);
	set_call_ICC_Exchange(ICC_Exchange);

	//PICC接口注册
	set_call_PICC_Open(PICC_Open);
	set_call_PICC_PowerOn(PICC_PowerOn);
	set_call_PICC_Close(PICC_Close);
	set_call_PICC_Exchange(PICC_Exchange);
	*/
	
	return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int exp_sys_init(void);
 * @brief:	系统初始化
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 01, 2019  18:24:58
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 01, 2019  18:24:58
*/
int exp_sys_init(void)
{
	int iret = 0;
	int rlen = 0;
	char RevBuff[512] = {0};

	//调试开启/设置日志路径
	api_set_uart_debug(1, "/home/root/xcfiletemp");

	//回调函数注册
	//exp_set_call_bak();
	
	//二维码串口
	qr_fd = QRCode_Open(0);
	if(qr_fd < 0)
	{
		qr_fd = QRCode_Open(1);
	}


	return iret;
}


/**
 * @Author: hhl \@decard
 * @fn     	void * child_thread(void);
 * @brief:	子线程
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  16:31:10
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  16:31:10
*/
void * child_thread(void)
{
	while(1)
	{
		//main_trans_update_details();

		sleep(5);
	}
}

/**
 * @Author: hhl \@decard
 * @fn     	int qr_retuen_goods(char *cpSerialNmb);
 * @brief:	二维码退货
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 18, 2019  15:39:34
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 18, 2019  15:39:34
*/
int qr_retuen_goods(char *cpSerialNmb)
{
	int iret = 0;
	char dateTime[14 +1] = {0};
	decard_ploymerize_pay ploymerize_pay;
	
	//退款(根据商户订单号)
	memset(&ploymerize_pay, 0, sizeof(decard_ploymerize_pay));
	memcpy(ploymerize_pay.outTradeNo, cpSerialNmb, strlen(cpSerialNmb));
	memset(dateTime, 0, sizeof(dateTime));
	Sys_GetRealTime(dateTime);
	sprintf(ploymerize_pay.out_refund_no, "%s000001", dateTime);
	iret = api_dc_ploymerize_pay_return_goods(&ploymerize_pay);
	if(iret)
	{	
		printf("api_dc_ploymerize_pay_return_goods err re : %d\n", iret);
		printf("MSG: %s\r\n", ploymerize_pay.errMsg);
		return iret;
	}
	
	printf("MSG: %s\r\n", ploymerize_pay.errMsg);

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int qrcode_business(void);
 * @brief:	//二维码业务
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 13, 2018  14:07:06
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2018  14:07:06
*/
int qrcode_business(void)
{
	char QRcode[1024] = "\0";
	char ctype = 0;
	char dateTime[14 +1] = {0};
	char caMerchOrderNmb[32];
	int QRcodeLen = 0;
	int back = 0;
	int iret = 0;

	decard_ploymerize_pay ploymerize_pay;
	memset(&ploymerize_pay, 0, sizeof(decard_ploymerize_pay));

	memset(QRcode, 0, sizeof(QRcode));
	QRcodeLen = 0;
	back = QRCode_RxStr(qr_fd, (unsigned char *)QRcode, 2048, 10);
	if(back > 0 && back <= 512)
	{
		Sys_Beep();

		memset(dateTime, 0, sizeof(dateTime));
		Sys_GetRealTime(dateTime);

		//二维码原始数据
		QRcodeLen = back;
		DumpData("QRCode:", QRcode, QRcodeLen);

		memset(&ploymerize_pay, 0, sizeof(decard_ploymerize_pay));
		strcpy(ploymerize_pay.scene, "bar_code");
		memcpy(ploymerize_pay.qrcode, QRcode, QRcodeLen);
		strcpy(ploymerize_pay.body, "停车场");
		strcpy(ploymerize_pay.totalFee, "1");
		strcpy(ploymerize_pay.terminalIp, "192.168.31.195");
		memset(caMerchOrderNmb, 0, sizeof(caMerchOrderNmb));
		sprintf(caMerchOrderNmb, "%s000001", dateTime);
		memcpy(ploymerize_pay.outTradeNo, caMerchOrderNmb, strlen(caMerchOrderNmb));
		strcpy(ploymerize_pay.terminalParams, " ");
		strcpy(ploymerize_pay.model, "T80-L2");
		strcpy(ploymerize_pay.sn, "002180802466");
		strcpy(ploymerize_pay.shop_num, "123456789123456789");
		iret = api_dc_ploymerize_pay(&ploymerize_pay);
		if(iret)
		{	
			printf("api_dc_ploymerize_pay err re : %d\n", iret);
			printf("MSG: %s\r\n", ploymerize_pay.errMsg);
			return iret;
		}

		back = 0;
		printf("\r\n------交易凭证---------\n");
		printf("MSG: %s\r\n", ploymerize_pay.errMsg);
		printf("*用户标识: %s\r\n", ploymerize_pay.openId);
		printf("*支付分类: %s\r\n", ploymerize_pay.payType);
		printf("*支付类型: %s\r\n", ploymerize_pay.pay_type);
		printf("*支付时间:	 %s\r\n", ploymerize_pay.timeEnd);
		printf("*交易流水: %s\r\n", ploymerize_pay.transactionId);
		printf("*交易金额: %s\r\n", 	   ploymerize_pay.totalFee);

		//退款(根据商户订单号)
		qr_retuen_goods(caMerchOrderNmb);
		
		//交易存储
		/*
		iret = main_mot_storage(&TrdeLogInfor);
		if(iret)
		{
			printf("main_mot_storage re error : %d\n", iret);
			continue;
		}
		*/

	}
	else
	{
		back = ERR_NO_QRCODE;
	}

TRADEEND:
	return back;
}

/**
 * @Author: hhl \@decard
 * @fn     	int main();
 * @brief:	测试main接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 01, 2019  18:24:42
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 01, 2019  18:24:42
*/
int main()
{
	int i = 0;
	int iret = 0;
	char CardType = 0;
	char ucCardSak = 0;
	char ucCardUid[9] = "\0";
	char TempBuff[128] = {0};
	pthread_t child_thread_d;
	pthread_attr_t a, b; //线程属性

	TermPara st_TermPara;
	
	//接口调用测试
	Uart_Printf("android lib 库测试\n");
	api_call_test(TempBuff);
	Uart_Printf(">>:%s\n", TempBuff);

	//系统硬件初始化
	iret = exp_sys_init();
	if(iret)
	{
		Uart_Printf("exp_sys_init re error : %d\n", iret);
		exit(iret);
	}

	//参数设置
	memset(&st_TermPara, 0, sizeof(TermPara));
	strcpy(st_TermPara.path_db, "/home/root/xcfiletemp");
	strcpy(st_TermPara.path_config_file, "/home/root/xcfiletemp");
	memcpy(st_TermPara.szTermID, "002180802466 ", 12);
	strcpy(st_TermPara.szTerm_mod, "T80-L2");
	memcpy(st_TermPara.shop_num, "123456789123456789", 18);
	strcpy(st_TermPara.serv_add, "dcpay.decard.com");
	strcpy(st_TermPara.serv_port, "40021");
	iret = api_set_term_para(&st_TermPara);
	if(iret)
	{
		Uart_Printf("main_set_term_para re error : %d\n", iret);
		exit(iret);
	}

	//公钥下载
	api_dc_ploymerize_pay_pubkey_download();

	//设备签到
	api_dc_ploymerize_pay_login();

	//数据统计
	/*
	{
		MotDataStatistics motDataStatistics;
		memset(&motDataStatistics, 0, sizeof(MotDataStatistics));
		iret = main_mot_data_statistics(&motDataStatistics);
		printf("main_mot_data_statistics re : %d\n", iret);
		if(iret == 0)
		{
			printf("当天笔数 : %ld\n", motDataStatistics.ulCrtDayMot_mnt);
			printf("当天金额 : %ld\n", motDataStatistics.ulCrtDayMot_mny);

			
			printf("总笔数 : %ld\n", motDataStatistics.ulTotalMot_mnt);
			printf("总金额 : %ld\n", motDataStatistics.ulTotalMot_mny);
		}
	}
	*/
	qr_retuen_goods("20200402141455000001");
	//qr_retuen_goods("20191219023711000001");

	//二维码消费测试
CONSUME_AG:
	//清空扫码头
	OSUART_Flush(qr_fd);

	while(1)
	{
		//二维码检测
		iret = qrcode_business();
		if(iret == 0 || (iret && ERR_NO_QRCODE != iret))
		{
			Uart_Printf("main_qrcode_business back : %d\n", iret);
			goto CONSUME_AG;
		}
		
		sleep(3);
	}
	
	//释放线程资源
	pthread_join(child_thread_d, NULL);

}
