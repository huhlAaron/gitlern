/*
 * work_main.h
 *	提供给外部调用的数据类型定义、接口声明
 *  Created on: November 06, 2019  10:07:32
 *      Author: huhongliang@decard.com
 */

#ifndef _API_MAIN_H
#define _API_MAIN_H

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


/**
* \defgroup ICC IC卡回调接口类型定义
*/
/*@{*/
/**
 * @brief ic卡或sam模块初始化
 * @param[in] slot 卡槽号
 */
typedef int (*call_ICC_Init)(unsigned char slot);

/**
 * @brief ic卡或sam模块上电
 * @param[in] slot 卡槽号
 * @param[in] baud 波特率，默认9600
 *            9600
 *            19200
 *            38400
 *            57600
 *            115200
 * @param[in] volt 操作电压，默认5.0V
 *              BIT1-BIT2
 *                  0x00：5V
 *                  0x01：3V
 *                  0x02：1.8V
 *              \li BIT8  1:支持PPS,0:不支持（缺省)
 * @param[out] atr 卡片复位应答信息
 * @param[out] len atr长度
 */
typedef int (*call_ICC_PowerUp)(unsigned char slot, unsigned int baud, unsigned int volt, unsigned char *atr, unsigned int *len);

/**
 * @brief 卡片APDU交互
 * @param[in] slot 卡槽号
 * @param[in] apdu  apdu数据
 * @param[in] apdu_len   apdu长度
 * @param[out] rpdu 返回数据
 * @param[out] rpdu_len 返回长度
 */
typedef int (*call_ICC_Exchange)(unsigned char slot,const unsigned char* apdu,const unsigned int apdu_len, unsigned char* rpdu, unsigned int* rpdu_len);

/**
 * @brief SAM卡关闭
 * @param[in] slot 卡槽号
 */
typedef int (*call_ICC_Close)(unsigned char slot);


/**
* \defgroup PICC IC卡接口类型定义
*/
/**
 * @brief 非接模块初始化
 * @param[in] slot 对于多个非接模块，使用0,1,2区分
 */
typedef int (*call_PICC_Open)(unsigned char slot);

/**
 * @brief 非接上电
 * @param[in] slot 卡槽号
 * @param[in] mode 寻卡模式
 *                \li 'A' 只寻A卡
 *                \li 'B' 只寻B卡
 *                \li 'C' 只寻Felica卡
 *                \li 0   寻AB卡
 * @param[out] cardType 卡类型 \sa #PICC_CARD_TYPE
 * @param[out] uid 卡片UID
 * @param[out] uid_size 卡片UID长度
 * @note 对于Felica,uid 返回 Manufacture ID + PMm, uid_size 为16
 */
typedef int (*call_PICC_PowerOn)(unsigned char slot, unsigned char mode, unsigned char *cardType, unsigned char *uid, unsigned char *uid_size);

/**
 * @brief PICC apdu通信
 * @param[in] slot 卡槽号
 * @param[in] apdu_buff  apdu数据
 * @param[in] apdu_len   apdu长度
 * @param[out] resp_buffer 返回数据
 * @param[out] resp_len 返回长度
 */
typedef int (*call_PICC_Exchange)(unsigned char slot,unsigned char * apdu_buff, unsigned int apdu_len, unsigned char *resp_buffer, unsigned int * resp_len);

/**
 * @brief 非接模块关闭
 * @param[in] slot 卡槽号
 * @note 暂时只做关场操作
 */
typedef int (*call_PICC_Close)(unsigned char slot);


//回调注册接口声明

/** @Author: hhl \@decard
 * @fn     	void set_call_ICC_Init(call_ICC_Init CallICCInitBack);
 * @brief:	接触式卡槽复位回调注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:43:51
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:43:51
*/
void set_call_ICC_Init(call_ICC_Init CallICCInit_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_ICC_PowerUp(call_ICC_PowerUp CallICCPowerUp_Back);
 * @brief:	 ic卡或sam模块上电回调注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:46:45
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:46:45
*/
void set_call_ICC_PowerUp(call_ICC_PowerUp CallICCPowerUp_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_ICC_Close(call_ICC_Close CallICCClose_Back);
 * @brief:	SAM卡关闭接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:49:09
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:49:09
*/
void set_call_ICC_Close(call_ICC_Close CallICCClose_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_ICC_Exchange(call_ICC_Close CallICCExchange_Back);
 * @brief:	卡片APDU交互接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:51:21
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:51:21
*/
void set_call_ICC_Exchange(call_ICC_Exchange CallICCExchange_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_PICC_Open(call_PICC_Open CallPICCOpen_Back);
 * @brief:	非接模块初始化接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:53:52
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:53:52
*/
void set_call_PICC_Open(call_PICC_Open CallPICCOpen_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_PICC_PowerOn(call_PICC_PowerOn CallPICCPowerOn_Back);
 * @brief:	非接上电接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:55:10
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:55:10
*/
void set_call_PICC_PowerOn(call_PICC_PowerOn CallPICCPowerOn_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_PICC_Close(call_PICC_Close CallPICCClose_Back);
 * @brief:	非接模块关闭接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:56:13
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:56:13
*/
void set_call_PICC_Close(call_PICC_Close CallPICCClose_Back);

/**
 * @Author: hhl \@decard
 * @fn     	void set_call_PICC_Exchange(call_PICC_Close CallPICCExchange_Back);
 * @brief:	PICC apdu通信接口注册
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  15:57:09
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  15:57:09
*/
void set_call_PICC_Exchange(call_PICC_Exchange CallPICCExchange_Back);

/**
* \defgroup //数据结构类型定义
*/
//卡物理类型
typedef enum
{
	CARD_MOT = 0,	//交通部卡
	CARD_UNION,		//银联卡
	CARD_UNKNOWN	//未知卡

}CARD_PHY_TYPE;

//交易状态
#define STATUS_SUCCESS "00"	//成功交易
#define STATUS_RETURN_GOODS "01"	//已经退货

//上送状态(第三方平台)
#define UPDATE_SUCCESS "00"	//上送成功
#define UPDATE_NOT "01"		//未上送

//交易汇总统计
typedef struct TRANSACTION_STATISTICS
{
	//mot未上送统计 笔数/金额
	unsigned long ulNotUpMot_mnt;
	unsigned long ulNotUpMot_mny;

	//mot卡当天统计 笔数/金额
	unsigned long ulCrtDayMot_mnt;
	unsigned long ulCrtDayMot_mny;

	//mot卡所有统计 笔数/金额
	unsigned long ulTotalMot_mnt;
	unsigned long ulTotalMot_mny;

}MotDataStatistics;

//聚合支付交易数据结构
typedef struct PLOYMERIZE_TRADE
{
	char scene[32];		//支付场景“bar_code”
	char qrcode[64];	//二维码数据
	char body[64];		//商品名称
	char totalFee[12];	//支付金额
	char terminalIp[15 + 1];//终端ip
	char outTradeNo[20 + 1];//商户订单号（唯一的订单流水号）
	char terminalParams[64];//终端其他信息
	char model[64];		//终端型号
	char sn[32];		//终端sn
	char shop_num[64];	//商户编号(商户项目编号)

	char openId[64];	//用户标识
	char payType[64];	//支付分类
	char pay_type[64];	//支付类型
	char timeEnd[32];	//支付时间
	char transactionId[64];	//交易流水号

	char out_refund_no[20 + 1];//退款流水号（唯一）
	char state[3];			//交易状态:成功/退货
	char upLoadFlag[3];		//上送状态
	char errMsg[256];		//错误信息
	char RFU[64];	//预留
}decard_ploymerize_pay;

//交易数据明细结构
typedef struct TRADE_INFOR
{
	//交易金额
	int i_in_trade_amt;

	//卡uid
	char c_in_card_uid[8+1];

	//业务类型
	char c_out_work_type[4+1];

	//城市代码
	char c_out_card_citycode[20+1];

	//交易卡号
	char c_out_card_id[20+1];

	 //发卡方标识
	char c_out_ussuer_id[16+1];  

	//卡主类型
	char c_out_card_index_type[2+1];
	
	//卡子类型
	char c_out_card_child_type[2+1];
	
	//卡计数器
	char c_out_card_cnt[4+1];
	
	//交易前余额	
	int i_out_bef_blance;

	//实际交易金额
	int i_out_real_trade_amt;

	//交易后余额	
	int i_out_aft_blance;

	//卡交易类型
	char c_out_trade_type[2+1];

	//卡交易随机数
	char c_out_trade_random[8+1];

	//上次交易类型
	char c_out_old_trade_type[8+1];
	
	//上次交易psam终端号
	char c_out_old_trade_termid[12+1];
	
	//上次交易时间
	char c_out_old_trade_dt[14+1];
	
	//上次交易金额
	char c_out_old_trade_amt[8+1];
	
	//上次交易计数器
	char c_out_old_trade_cnt[4+1];

	//交易时间
	char c_out_trade_datetime[14+1];

	//钱包类型00金额 01扣次
	char c_out_wallet_type[2+1];
	
	//交易流水
	char c_out_aq[6+1];
	
	//交易tac
	char c_out_tac[8+1];

	//psam终端号
	char c_out_psam_tern_id[12+1];

	//psam卡号
	char c_out_psam_id[20+1];

	//交易psam卡交易流水
	char c_out_psam_sq[8+1];

	//交易标识00 正常 01灰交易
	char c_out_trade_result[2+1];

	//县区编号
	char c_out_district_id[20+1];
	
	//商户号
	char c_out_merch_id[20+1];

	//司机编号
	char c_out_driver_id[20+1];
	
	//线路编号
	char c_out_line_id[8+1];
	
	//设备编号
	char c_out_dev_no[8+1];

	//设备SN
	char c_out_sn[20+1];
	
	//车辆编号
	char c_out_car_id[12+1];
	
	//班次顺序号
	char c_out_class_sq[8+1];
	
	//上送状态
	char c_out_update_flag[2+1];

	//交易统计
	MotDataStatistics motDataStatic;

	//预留
	unsigned char RFU[64];
}StTradeInfor;


//终端参数数据结构定义
typedef struct TERM_PARA
{
    char path_db[512 + 1];			//数据库路径
    char path_config_file[512 + 1];	//配置文件路径
    char shop_num[32 + 1]; 			//商户编码(商户项目编号)
    char szTermID[32 + 1];	//终端sn编号
    char szTerm_mod[64 + 1];//设备类型
    char serv_add[64 + 1];	//服务器地址
    char serv_port[8 + 1];	//服务端口

    char rfu[64];	//预留
} TermPara;


/**
 * @Author: hhl \@decard
 * @fn     	void api_call_test(char *rcvBuff);
 * @brief:	接口调用测试，看看你得到的数据是啥？悄悄告诉你，是"Hello World, Hello buss reader lib Module! \r\n"
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  11:09:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  11:09:20
*/
void api_call_test(char *rcvBuff);

/**
 * @Author: hhl \@decard
 * @fn     	void api_set_uart_debug(char ucFlag);
 * @brief:	调试开关设置
 * @param[in] :ucFlag:1开启 0关闭 path:日志路径
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  11:04:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  11:04:20
*/
void api_set_uart_debug(char ucFlag, char *path);

/**
 * @Author: hhl \@decard
 * @fn     	int api_set_term_para(TermPara *pTermPara);
 * @brief:	终端参数设置接口
 * @param[in] :
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:November 06, 2019  09:31:24
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  09:31:24
*/
int api_set_term_para(TermPara *pTermPara);


/**
 * @Author: hhl \@decard
 * @fn		int api_dc_ploymerize_pay(decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚会支付，支付接口
 * @param[in] :
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:December 12, 2019	15:52:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:52:20
*/
int api_dc_ploymerize_pay(decard_ploymerize_pay *tradeDate);


/**
 * @Author: hhl \@decard
 * @fn		int api_dc_ploymerize_pay_return_goods(decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚会支付，退款接口
 * @param[in] :
 * @param[out]:
 * @return:0成功 非0失败
 * @Date:December 13, 2019	10:08:51
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  10:08:51
*/
int api_dc_ploymerize_pay_return_goods(decard_ploymerize_pay *tradeDate);


/**
 * @Author: hhl \@decard
 * @fn		int api_dc_ploymerize_pay_pubkey_download(void);
 * @brief:	公钥下载
 * @param[in] :
 * @param[out]:
 * @return: 0成功 非0失败
 * @Date:March 30, 2020  15:13:04
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  15:13:04
*/
int api_dc_ploymerize_pay_pubkey_download(void);


/**
 * @Author: hhl \@decard
 * @fn		int api_dc_ploymerize_pay_login(void);
 * @brief:	设备签到
 * @param[in] :
 * @param[out]:
 * @return: 0成功 非0失败
 * @Date:March 30, 2020  17:37:07
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  17:37:07
*/
int api_dc_ploymerize_pay_login(void);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* _API_MAIN_H */
