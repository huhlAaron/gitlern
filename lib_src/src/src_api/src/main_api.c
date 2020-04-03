/*
 * work_main.c
 *	主业务逻辑文件,提供给外部的接口都在此文档内定义
 *  Created on: October 31, 2019  13:55:29
 *      Author: huhongliang@decard.com
 */
#include <sqlite3.h>

#include "main_api.h"
#include "data_type.h"


//终端运行参数结构
static STRUCT_ONCE_SIMPLE term_para;

//终端参数版本控制 本地版本  / 服务端版本
static st_TEMP_VERSION_CTRL ST_NewVerCtrlInfor;
static st_TEMP_VERSION_CTRL ST_ServVerCtrlInfor;


/**
 * @Author: hhl \@decard
 * @fn     	void GetLocDate(unsigned char * pDateTime);
 * @brief:	//获取当前系统时间
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  19:14:38
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  19:14:38
*/
void GetLocDate(unsigned char * pDateTime)
{
    time_t timep;
    struct tm* p;

    time(&timep);
    p = localtime(&timep);

    sprintf(pDateTime, "%04d%02d%02d%02d%02d%02d", (1900 + p->tm_year), (1 + p->tm_mon), (p->tm_mday), p->tm_hour, p->tm_min, p->tm_sec);
    pDateTime[14] = '\0';

    return ;
}

/**
 * @Author: hhl \@decard
 * @fn     	void api_call_test(char *rcvBuff);
 * @brief:	接口调用测试，看看你得到的数据是啥……悄悄告诉你，是"Hello World, Hello buss reader lib Module! \r\n"
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  11:09:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  11:09:20
*/
void api_call_test(char *rcvBuff)
{
	post_hello_func(rcvBuff);
}

/**
 * @Author: hhl \@decard
 * @fn     	voi dapi_set_uart_debug(char ucFlag);
 * @brief:	调试开关设置
 * @param[in] :ucFlag:1开启 0关闭 path:日志路径
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  11:04:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  11:04:20
*/
void api_set_uart_debug(char ucFlag, char *path)
{
	Uart_SetDebug(ucFlag);
	Uart_SetLogPath(path);
}

/**
 * @Author: hhl \@decard
 * @fn     	static int api_so_init(void);
 * @brief:	动态库使用初始化，参数设置完之后，调用！
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  09:21:00
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  09:21:00
*/
static int api_so_init(void)
{
	int iret = 0;

	//设置参数检查
	if(strlen(term_para.aucdbPath) == 0)
	{
		File_Printf("db path err.\n");
		return ERR_DB_PATH;
	}
	db_set_path(term_para.aucdbPath);
	
	if(strlen(term_para.auccfgFilePath) == 0)
	{
		File_Printf("config file path err.\n");
		return ERR_CONFIG_PATH;
	}
	cfg_set_path(term_para.auccfgFilePath);

	//配置文件检测
	cfg_file_check();

	//自己本身维护的参数获取
	cfg_get_para(&term_para);

	//版本号获取
	//cfg_get_para_ver(&ST_NewVerCtrlInfor);

	//数据库初始化
	iret = db_init();
	if(iret)
	{
		File_Printf("db_init err : %d\n", iret);
	}
	
	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int api_set_term_para(TermPara *pTermPara);
 * @brief:	终端参数设置接口.开机调用
 * @param[in] :pTermPara参数结构体
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:November 06, 2019  09:31:24
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  09:31:24
*/
int api_set_term_para(TermPara *pTermPara)
{
	//参数缓存清空
	memset(&term_para, 0, sizeof(STRUCT_ONCE_SIMPLE));
	memset(&ST_NewVerCtrlInfor, 0, sizeof(st_TEMP_VERSION_CTRL));
	memset(&ST_ServVerCtrlInfor, 0, sizeof(st_TEMP_VERSION_CTRL));

	//外部设置部分
	///县区编码设置
	//memcpy(term_para.szCompanyNumb, pTermPara->districtCode, strlen(pTermPara->districtCode) >= 6 ? 6 : strlen(pTermPara->districtCode));

	///商户号设置
	//memcpy(term_para.szMerchantId, pTermPara->merchant, strlen(pTermPara->merchant) >= 15 ? 15 : strlen(pTermPara->merchant));

	///数据库路径
	memcpy(term_para.aucdbPath, pTermPara->path_db, strlen(pTermPara->path_db) >= 512 ? 512 : strlen(pTermPara->path_db));

	///配置文件路径
	memcpy(term_para.auccfgFilePath, pTermPara->path_config_file, strlen(pTermPara->path_config_file) >= 512 ? 512 : strlen(pTermPara->path_config_file));

	///设备sn码
	memcpy(term_para.szTermID, pTermPara->szTermID, strlen(pTermPara->szTermID) >= 32 ? 32 : strlen(pTermPara->szTermID));

	///设备类型
	memcpy(term_para.szTermType, pTermPara->szTerm_mod, strlen(pTermPara->szTerm_mod) >= 64 ? 64 : strlen(pTermPara->szTerm_mod));

	///聚合支付商户号
	memcpy(term_para.shop_num, pTermPara->shop_num, strlen(pTermPara->shop_num) >= 32 ? 32 : strlen(pTermPara->shop_num));
	
	///聚合支付服务地址
	memcpy(term_para.serv_add, pTermPara->serv_add, strlen(pTermPara->serv_add) >= 64 ? 64 : strlen(pTermPara->serv_add));

	///聚合支付服务端口
	memcpy(term_para.serv_port, pTermPara->serv_port, strlen(pTermPara->serv_port) >= 8 ? 8 : strlen(pTermPara->serv_port));
	
	///SIM卡SSID
	//memcpy(term_para.szSIM_CCID, pTermPara->szSIM_CCID, strlen(pTermPara->szSIM_CCID) >= 32 ? 32 : strlen(pTermPara->szSIM_CCID));

	///SIM卡IMSI
	//memcpy(term_para.szSIM_IMSI, pTermPara->szSIM_IMSI, strlen(pTermPara->szSIM_IMSI) >= 32 ? 32 : strlen(pTermPara->szSIM_IMSI));
	
	///SIM卡IMEI
	//memcpy(term_para.szSIM_IMEI, pTermPara->szSIM_IMEI, strlen(pTermPara->szSIM_IMEI) >= 32 ? 32 : strlen(pTermPara->szSIM_IMEI));

	return api_so_init();
}

/**
 * @Author: hhl \@decard
 * @fn     	int api_mot_data_statistics(MotDataStatistics *pmotDataStatistics);
 * @brief:	交易汇总统计
 * @param[in] :pmotDataStatistics 统计数据结构
 * @param[out]:
 * @return:
 * @Date:November 22, 2019  15:00:33
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 22, 2019  15:00:33
*/
int api_mot_data_statistics(MotDataStatistics *pmotDataStatistics)
{
	int iret = 0;
	StTradeInfor ST_TrdeLogInfor;
	char ucTradeTime[20] = {0};

	//获取交易时间
	GetLocDate(ucTradeTime);
	memset(&ST_TrdeLogInfor, 0, sizeof(StTradeInfor));
	memcpy(ST_TrdeLogInfor.c_out_old_trade_dt, ucTradeTime, 8);

	//当天交易统计
	iret = db_trade_mot_table_opr(DB_STATISTICAL_MOT_CRT_DAY, &ST_TrdeLogInfor);
	if(iret > 0)
	{
		pmotDataStatistics->ulCrtDayMot_mnt = ST_TrdeLogInfor.motDataStatic.ulCrtDayMot_mnt;
		pmotDataStatistics->ulCrtDayMot_mny = ST_TrdeLogInfor.motDataStatic.ulCrtDayMot_mny;
		iret = 0;
	}
	else
	{
		pmotDataStatistics->ulCrtDayMot_mnt = 0;
		pmotDataStatistics->ulCrtDayMot_mny = 0;
		iret = 0;
	}

	//未上送交易统计
	iret = db_trade_mot_table_opr(DB_STATISTICAL_RECORD, &ST_TrdeLogInfor);
	if(iret > 0)
	{
		pmotDataStatistics->ulNotUpMot_mnt = ST_TrdeLogInfor.motDataStatic.ulNotUpMot_mnt;
		pmotDataStatistics->ulNotUpMot_mny = ST_TrdeLogInfor.motDataStatic.ulNotUpMot_mny;
		iret = 0;
	}
	else
	{
		pmotDataStatistics->ulNotUpMot_mnt = 0;
		pmotDataStatistics->ulNotUpMot_mny = 0;
		iret = 0;
	}

	//全部数据统计
	memset(&ST_TrdeLogInfor, 0, sizeof(StTradeInfor));
	iret = db_trade_mot_table_opr(DB_STATISTICAL_MOT_TATOL, &ST_TrdeLogInfor);
	if(iret > 0)
	{
		pmotDataStatistics->ulTotalMot_mnt = ST_TrdeLogInfor.motDataStatic.ulTotalMot_mnt;
		pmotDataStatistics->ulTotalMot_mny = ST_TrdeLogInfor.motDataStatic.ulTotalMot_mny;
		iret = 0;
	}
	else
	{
		pmotDataStatistics->ulCrtDayMot_mnt = 0;
		pmotDataStatistics->ulCrtDayMot_mny = 0;
		iret = 0;
	}
	
	return iret;
}


/**
 * @Author: hhl \@decard
 * @fn     	int api_dc_ploymerize_pay(decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚会支付，支付接口
 * @param[in] :tradeDate 数据结构
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:December 12, 2019  15:52:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:52:20
*/
int api_dc_ploymerize_pay(decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	char dateTime[15] = {0};
	decard_ploymerize_pay ploy_pey_tradeDate;
		
	//检查订单号，如果么有，则内部自动生成
	if(strlen(tradeDate->outTradeNo) == 0)
	{
		memset(dateTime, 0, sizeof(dateTime));
		GetLocDate(dateTime);
		sprintf(tradeDate->outTradeNo, "%s000001", dateTime);
		
	}

	memset(&ploy_pey_tradeDate, 0, sizeof(decard_ploymerize_pay));
	memcpy(&ploy_pey_tradeDate, tradeDate, sizeof(decard_ploymerize_pay));

	//交易
	iret = dc_ploymerize_pay(&term_para, &ploy_pey_tradeDate);
	//iret = ERR_TERMNOTRCVSERV;
	if(iret)
	{
		File_Printf("dc_ploymerize_pay err : %d\n", iret);
		if(iret == ERR_TERMNOTRCVSERV)
		{	
			iret = dc_ploymerize_pay_chack(&term_para, &ploy_pey_tradeDate);
			if(iret)
			{
				File_Printf("dc_ploymerize_pay_chack err : %d\n", iret);
				
				//错误信息返回
				if(strlen(ploy_pey_tradeDate.errMsg) == 0)
				{
					strcpy(ploy_pey_tradeDate.errMsg, "交易失败");
				}
				return iret;
			}
		}

		//错误信息返回
		if(strlen(ploy_pey_tradeDate.errMsg) == 0)
		{
			strcpy(ploy_pey_tradeDate.errMsg, "交易失败");
		}
		return iret;
	}

	//交易成功状态赋值
	memcpy(ploy_pey_tradeDate.state, STATUS_SUCCESS, 2);
	memcpy(ploy_pey_tradeDate.upLoadFlag, UPDATE_NOT, 2);
	
	//交易数据存储
	iret = db_trade_decard_ploymerize_pay(DB_INSERT, &ploy_pey_tradeDate);
	if(iret <= 0)
	{
		File_Printf("dc_ploymerize_pay err : %d\n", iret);
		memset(ploy_pey_tradeDate.errMsg, 0, sizeof(ploy_pey_tradeDate.errMsg));
		strcpy(ploy_pey_tradeDate.errMsg, "交易存储失败");
		return ERR_DATA_STORE;
	}
	else
	{
		iret = 0;
	}

	//数据返回
	memcpy(tradeDate, &ploy_pey_tradeDate, sizeof(decard_ploymerize_pay));
	
	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int api_dc_ploymerize_pay_return_goods(decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚会支付，退款接口
 * @param[in] :tradeDate：退款数据结构，传入商户订单号
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:December 13, 2019  10:08:51
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  10:08:51
*/
int api_dc_ploymerize_pay_return_goods(decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	char dateTime[15] = {0};
	char caReGoodsOrdNmb[32] = {0};
	decard_ploymerize_pay ploy_pey_tradeDate;
	
	//检查退款订单号，如果么有，则内部自动生成
	if(strlen(tradeDate->out_refund_no) == 0)
	{
		memset(dateTime, 0, sizeof(dateTime));
		GetLocDate(dateTime);
		sprintf(tradeDate->out_refund_no, "%s000001", dateTime);
	}

	//数据拉取
	memset(&ploy_pey_tradeDate, 0, sizeof(decard_ploymerize_pay));
	memcpy(&ploy_pey_tradeDate, tradeDate, sizeof(decard_ploymerize_pay));
	memcpy(ploy_pey_tradeDate.state, STATUS_SUCCESS, 2);
	iret = db_trade_decard_ploymerize_pay(DB_SELECT, &ploy_pey_tradeDate);
	if(iret > 0)
	{
		memcpy(ploy_pey_tradeDate.out_refund_no, tradeDate->out_refund_no, strlen(tradeDate->out_refund_no));
		iret = dc_ploymerize_return_goods(&term_para, &ploy_pey_tradeDate);
		//iret = ERR_TERMNOTRCVSERV;
		if(iret)
		{
			File_Printf("dc_ploymerize_pay err : %d\n", iret);
			if(iret == ERR_TERMNOTRCVSERV)
			{
				iret = dc_ploymerize_return_goods_chack(&term_para, &ploy_pey_tradeDate);
				if(iret)
				{
					File_Printf("dc_ploymerize_pay_chack err : %d\n", iret);
					
					//错误信息返回
					if(strlen(ploy_pey_tradeDate.errMsg) == 0)
					{
						strcpy(ploy_pey_tradeDate.errMsg, "退款失败");
					}
					return iret;
				}
			}

			//错误信息返回
			if(strlen(ploy_pey_tradeDate.errMsg) == 0)
			{
				strcpy(ploy_pey_tradeDate.errMsg, "退款失败");
			}
			return iret;
		}

		//数据库标志更新
		memcpy(&ploy_pey_tradeDate.state, STATUS_RETURN_GOODS, 2);
		db_trade_decard_ploymerize_pay(DB_UPDATE, &ploy_pey_tradeDate);
	}
	else
	{
		memset(ploy_pey_tradeDate.errMsg, 0, sizeof(ploy_pey_tradeDate.errMsg));
		strcpy(ploy_pey_tradeDate.errMsg, "未查询到交易");
		iret = ERR_NOT_FIND_DATA;
	}
	
	//数据返回
	memcpy(tradeDate, &ploy_pey_tradeDate, sizeof(decard_ploymerize_pay));

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int api_dc_ploymerize_pay_pubkey_download(void);
 * @brief:	公钥下载
 * @param[in] :
 * @param[out]:
 * @return: 0成功 非0失败
 * @Date:March 30, 2020  15:13:04
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  15:13:04
*/
int api_dc_ploymerize_pay_pubkey_download(void)
{
	return dc_ploymerize_pay_pubkey_download(&term_para);
}

/**
 * @Author: hhl \@decard
 * @fn     	int api_dc_ploymerize_pay_login(void);
 * @brief:	设备签到
 * @param[in] :
 * @param[out]:
 * @return: 0成功 非0失败
 * @Date:March 30, 2020  17:37:07
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  17:37:07
*/
int api_dc_ploymerize_pay_login(void)
{
	return dc_ploymerize_pay_login(&term_para);
}


