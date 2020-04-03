/*
 * define.h
 *	宏定义模块
 *  Created on: November 15, 2019  09:34:12
 *      Author: huhongliang@decard.com
 */


#ifndef _DEFINE_H_
#define _DEFINE_H_

//__LINUX 定义
#define __LINUX

#define PRINT_THE_FILE_INFOR() Uart_Printf("\n\n*****[%s][%s][%d]*****\n\n", __FILE__, __FUNCTION__, __LINE__);

//终端交易类型定义
#define WORKTYPE_8451 		"8451"	//普通电子钱包消费
#define WORKTYPE_8452		"8452"	//公交计次钱包消费
#define WORKTYPE_8460		"8460"	//复合交易开始(公交:上车、地铁:进站)
#define WORKTYPE_8461		"8460"	//复合交易结束(公交:下车、地铁:出站)
#define WORKTYPE_8465		"8465"	//补票
#define WORKTYPE_8470		"8470"	//锁卡
#define WORKTYPE_1001		"1001"	//电子现金
#define WORKTYPE_1002		"1002"	//双免
#define WORKTYPE_1003		"1003"	//ODA
#define WORKTYPE_1004		"1004"	//二维码

//终端交易数据状态定义
#define DC_HAVE_UP				"00"	//德卡平台上传成功
#define DC_NOT_UP				"01"	//德卡平台未上传(需要备份)

#define UNION_ERR_CORRECT		"E3"	//银联冲正失败的交易(指39域有明确的错误码)
#define UNION_HAVE_CORRECT		"02"	//银联已经冲正的交易
#define UNION_NOT_CORRECT		"03"	//银联未冲正的交易(需要冲正)

#define UNION_ERR_UP			"E5"	//银联上送失败交易(针对ODA)(指39域有明确的错误码)
#define UNION_HAVE_UP			"04"	//银联已经上送交易(针对ODA)
#define UNION_NOT_UP			"05"	//银联未上送交易(针对ODA)

#define DC_HAVE_RE_UP			"06"	//已补采(被前置平台进行补采)

#define DC_ERR_TRADE			"FF"	//德卡平台返回异常交易

//最大交易流水号
#define MAX_TRADE_SEQ	999999

//log级别
typedef enum   LOGLEVLE_EMUN{FLOW = 1, BUSINESS = 2, DUGLOG = 4 } LOGLEVLE_E;

//终端错误码定义
#define ERR_SUCCESS				0x0000	//成功
#define ERR_PROTOCOLHEAD		0x01B1	//协议头错误
#define ERR_PROTOCOLLEN			0x01B2	//协议长度错误
#define ERR_PROTOCOLCOMMAND		0x01B3	//协议命令字错误
#define ERR_PROTOCOLLRC			0x01B4	//协议校验错误
#define ERR_PROTOCOLETX			0x01B5	//协议尾错误
#define ERR_PROTOCOLFRAME		0x01B6	//协议帧错误
#define ERR_PROTOCOLPARA		0x01B7	//参数错误
#define ERR_PROTOCOLPARALEN		0x01B8	//参数长度错误

#define ERR_FUNCTIONCLOSE		0x01B9	//功能禁止
#define ERR_TRADEOVERLIMIT		0x01BA	//本地交易数量超限
#define ERR_TERMNOTLOGON		0x01BB	//终端未签到
#define ERR_TERMNOTFINDETRADE 		0x01BC	//终端未找到交易
#define ERR_TERMNOTSETMERCORTERMNMB 0x01BD	//终端未设置商户号和终端号
#define ERR_TERMCHECKLOG 		0x01BE	//查找交易读文件失败
#define ERR_TERMNETOVERTIME 	0x01C0	//终端网络超时.服务器连接失败
#define ERR_TERMNOTRCVSERV 		0x01C1	//服务器无响应。接收数据失败
#define ERR_TERMNETRCVSERV 		0x01C2	//网络接收数据非法

#define ERR_CPUCARDGREY_TRADE 	0x01C3 	//CPU卡灰锁交易
#define ERR_TERMNOTSETTLE	 	0x01C4	//升级前未结算
#define ERR_TERMUPAPPNOTFINISH 	0x01c5	//终端有未完成的app升级
#define ERR_PSAMCAEDINITERR 	0x01c6	//PSAM卡初始化失败
#define ERR_PKICAEDINITERR 		0x01c7	//PKI卡初始化失败

#define ERR_CARDCHECKWRONG 		0x01D0	//卡校验不合法
#define ERR_CARDTYPEERR 		0x01D1	//卡类型不合法
#define ERR_CARDOVERDUE			0x01D2	//卡片过期
#define ERR_CARDNOTENABLED		0x01D3	//卡片未启用
#define ERR_CARDBLACKLIST		0x01D4	//黑名单卡
#define ERR_CARDOVERBLANCE		0x01D5	//余额不足
#define ERR_CARDDEDUCTERR		0x01D6	//扣费失败
#define ERR_CARDOVERMEMORY		0x01D7	//卡片存储区满
#define ERR_CARDNOTFINDPARA		0x01D8	//未找到参数
#define ERR_CARDFINDERR			0x01D9	//寻卡失败
#define ERR_CARDNOTGETSTARTDATE	0x01DA	//钱包启用日期未到

#define ERR_TRADEFIAL			0x01DB  //交易失败
#define ERR_READTRADEINFOR  	0x01DC  //读记录失败
#define ERR_CARD_FAILURE	  	0x01DD  //卡失效  Card failure

#define ERR_CARDCDTIONERR		0x6A81	//功能不支持

//内部自定义错误代码
#define ERR_00 0
#define ERR_CARD_GETATS			0x0110	//读取ATS失败
#define ERR_CARD_SELECT3F00		0x0111	//3F00选择失败
#define ERR_CRD_ILLEGAL 		0x0112	//读取卡片发卡方信息失败
#define ERR_REFER_FMT			0x0113	//读取文件错误
#define ERR_READCOMMON_WALL 	0x0114	//读取公共钱包文件失败
#define ERR_GATBLANCE805C 		0x0115	//获取余额失败
#define ERR_INITCONSUMEWALLET 	0x0116//消费初始化失败

#define ERR_SAMSELE1001			0x0117//SAM卡1001文件选择失败
#define ERRR_SAMGETMAC1			0x0118//SAM卡计算MAC1失败

#define ERR_CPU8054CONSUME		0x0119//CPU卡扣款失败
#define ERR_CPUCHECKMAC2		0x0120//CPU验证MAC2失败

#define ERR_MIFARESELECT		0x0121//M1卡选卡失败
#define ERR_MIFAREREADSEC0		0x0122//M1卡0扇区读取失败
#define ERR_MIFAREREADSEC1		0x0123//M1卡1扇区读取失败
#define ERR_MIFAREREADSEC2		0x0124//M1卡2扇区读取失败
#define ERR_MIFAREREADSEC3		0x0125//M1卡3扇区读取失败
#define ERR_MIFAREREADSEC4		0x0126//M1卡4扇区读取失败
#define ERR_MIFAREREADSEC5		0x0127//M1卡5扇区读取失败
#define ERR_MIFAREREADSEC6		0x0128//M1卡6扇区读取失败
#define ERR_MIFAREREADSEC7		0x0129//M1卡7扇区读取失败
#define ERR_MIFAREREADSEC8		0x0130//M1卡8扇区读取失败
#define ERR_MIFAREREADSEC9		0x0131//M1卡9扇区读取失败
#define ERR_MIFAREREADSEC10		0x0132//M1卡10扇区读取失败
#define ERR_MIFAREREADSEC11		0x0133//M1卡11扇区读取失败
#define ERR_MIFAREREADSEC12		0x0134//M1卡12扇区读取失败
#define ERR_MIFAREREADSEC13		0x0135//M1卡13扇区读取失败
#define ERR_MIFAREREADSEC14		0x0136//M1卡14扇区读取失败
#define ERR_MIFAREREADSEC15		0x0137//M1卡15扇区读取失败
#define ERR_SAMGETOTHERKAY		0x0138//M1卡计算其他扇区密钥失败
#define ERR_CHECKBLOCK4			0x0139 //M1块4校验出错
#define ERR_CHECKBLOCK5			0x0140 //M1块5校验出错
#define ERR_CHECKBLOCK6			0x0141 //M1块6校验出错
#define ERR_CHECKBLOCK8			0x0142 //M1块8校验出错

#define ERR_M1_BREAKPOINT  		0x0143//断点错误
#define ERR_BALANCE 			0x0144//卡片余额错误:余额异常
#define ERR_MIFAREMAKETAC		0x0145//计算TAC失败

#define ERR_MIFAREFLOCK6WRITE	0x0146//块6写失败
#define ERR_MIFAREFLOCK7WRITE	0x0147//块7写失败
#define ERR_TRADCANCEL			0x0148//交易被取消
#define ERR_GAETCARDTIMEOUT		0x0149 //刷卡超时
#define ERR_TRANSLOG_SAVE		0x0150 //交易存储错误
#define ERR_8583FILE39			0x0151 //39域信息错误
#define ERR_CNTOVER				0x0152 //金额过大
#define ERR_GETCHARGEMAC2		0x0153 //获取MAC2失败
#define ERR_CNTNOTMULTIPLE  	0x0154 //金额非10的倍数
#define ERR_CHAEKKEYB  			0x0156 //KeyB 出错
#define ERR_TERMSENDDATA  		0x0157 //发送数据失败
#define ERR_CARDSAK  			0x0158 //卡片SAK错误
#define ERR_M1NOTSUPPORTCARDTYPE 0x0159 //不支持卡类型
#define ERR_RESERVETRADE		0x0160 	//交易已冲正
#define ERR_IMAGINATIONCARD		0x0161 	//畅想卡
#define ERR_ISNOTIMAGINATIONCARD 0x0162 	//非畅享畅想卡
#define ERR_CNTTOOSMALL			0x0163 	//金额过小
#define ERR_TRADEHAVEREVOKE		0x0164 	//交易已经撤销
#define ERR_NOTSAMECARD			0x0165	//非同一张卡
#define ERR_NEEDRESERVEDATA		0x0166	//有需要冲正的数据
#define ERR_NEEDTICKETCARD		0x0167	//非月票卡
#define ERR_TICKETOVERTIMES		0x0168	//充次超次
#define ERR_INPUT				0x0169	//输入错误
#define ERR_DONOTREVOKE			0x0170	//已经结算，无法撤销
#define ERR_UPDATEUPSUBSEC 		0x0171	//上车更新分段信息失败
#define ERR_UPDATEDOWNSUBSEC 	0x0172	//下车更新分段信息失败
#define ERR_READ0X1EFile		0x0173	//读取0x1E文件失败
#define ERR_UPDATE0X1E			0x0174	//更新0x1E文件失败
#define ERR_RCVDATAMAC      	0x0175	//接收数据MAC错误
#define ERR_RCVDATALEN      	0x0176	//接收数据长度错误
#define ERR_NOTINTERCARD      	0x0177	//不是互通卡
#define ERR_RIDECARD      		0x0178	//逃票卡，不允许交易
#define ERR_CONTINUITYSWIPCARD 0x0179	//禁止连刷
#define ERR_TICKETTYPE			0x0180	//票制错误
#define ERR_READ0X1AFile		0x0181	//读取0x1A文件失败
#define ERR_UPDATE0X1A			0x0182	//更新0x1A文件失败
#define ERR_MIFAREREDBIT		0x0183	//M1卡2扇区扣款失败
#define ERR_MIFARERERESTORE		0x0184	//M1卡2扇区备份失败
#define ERR_EMVSELECTAPP		0x0185	//EMV选应用失败
#define ERR_EMVINITPARA			0x0186	//EMV参数初始化
#define ERR_EMVGPO				0x0187	//EMVGPO失败
#define ERR_EMVREADDATA			0x0189	//EMV读数据失败
#define ERR_EMVAUTHEN			0x0190	//EMV数据认证失败
#define ERR_CARDDISABLE			0x0191	//卡片已经停用
#define ERR_CARDDLOCKED			0x0192	//卡片已锁
#define ERR_CHECKCITYCODE		0x0193	//城市代码校验错误
#define ERR_CHECKCARDNMB		0x0194	//卡号校验错
#define ERR_CHECKAUTHCODE		0x0195	//认证码校验错误
#define ERR_CARDSTRUCT			0x0196	//s6扇区错误 卡机构错误
#define ERR_CARDDIBITCNT		0x0197	//卡计数器达到最大	请投币
#define ERR_MIFARECHECKSEC2		0x0198	//M1卡扇区2交易失败	.钱包区错
#define ERR_CARDYPENOTSURPORT	0x0199	//卡类型不支持
#define ERR_DELETEDATAERR		0x0200	//数据库清除出错

#define ERR_QROVERDATE			0x0201	//二维码过期
#define ERR_QROVERAMT			0x0202	//二维码金额超限
#define ERR_ERRMAC2				0x0203	//MAC2无效
#define ERR_ERRBLICKLOCKCARD	0x0204	//黑名单锁卡失败
#define ERR_ERRCARDISSUERCODE	 0x0205	//发卡机构不支持
#define ERR_QRCODEUSEMODE		0x0206	//二维码使用范围不支持
#define ERR_QRCODELEN			0x0207	//二维码错误
#define ERR_QROVERNOTSTART		0x0208	//二维码未启用
#define ERR_ERRTERID			0x0209	//终端号非法

#define ERR_INPUT_PIN           0x0210	//要求PIN输入
#define ERR_DECLINETRANS     	0x0211    // 交易拒绝
#define ERROR_DISABLEODA    	0x0212    // 卡片不支持联机ODA
#define ERROR_SELECT_NO_APP  	0x0213    // 没有AID
#define ERROR_UNION_QR_AUTHEN  	0x0214    // 二维码验证失败
#define ERR_MACKEYCHECKERR		0x0215    // 银联签到MACKEY解密失败
#define ERR_TDKEYCHECKERR		0x0216    // 银联签到TDK解密失败
#define ERR_NO_UNION_QR_CERT  	0x0217    // 未找到二维码证书
#define ERR_FILED_39        	0x0218    // 39域错误

#define ERR_DATE_TIME  			0x0219    // 时间错误
#define ERR_MONTH_TICKET_START 0x0220    // 月票未启用
#define ERR_MONTH_TICKET_OVER    0x0221     // 月票已过期
#define ERR_SLEEP_CARD	   		0x0222     // 休眠卡
#define ERR_NO_QRCODE	   		0x0223     // 未发现二维码
#define ERR_NO_CARD	   	   		0x0224     // 未发现卡片

#define ERR_SERV	   	   		0x0225   // 服务器系统方错误
#define ERR_MOT_QR_REPEAT 	  	0x0226   // 二维码重重复
#define ERR_QR   	  			0x0227   // 二维码无效
#define ERR_DRIVER_NOT_LOGIN  	0x0228    // 司机未签到

#define ERR_MD5_CHACK  		0x0229   	 // MD5校验失败
#define ERR_FTP_ANALYSIS 	0x0230   	 // ftp文件解析失败

#define ERR_OUT_LIMIT_TIMES 	0x0231   	 // 超过次数限制
#define ERR_QR_SET_PARA 		0x0232   	 // 二维码配置参数
#define ERR_ICC_INIT 			0x0233   	 // PSAM卡初始化失败
#define ERR_ICC_POWER_ON		0x0234   	 // PSAM卡上电失败
#define ERR_PICC_OPEN			0x0235   	 // 射频模块上电失败
#define ERR_CALL_FUNC_RJ		0x0236   	 // 函数注册错误
#define ERR_DB_PATH		0x0237   	 		// 数据库路径错误
#define ERR_CONFIG_PATH		0x0238   	 	// 配置文件路径错误



//签到相应错误
#define ERR_CHECKPINKEYCHECKVALUE    0x01F2 //解密PINKEY错误
#define ERR_CHECKMACKEYCHECKVALUE    0x01F3 //解密MACKEY错误
#define ERR_CHECKTDKKEYCHECKVALUE    0x01F4 //解密TDKEY错误

/*
*************************************[结构体/全局变量定义/声明]****************************
*/

/**********************************************************************************************//**
* @name   出入参的说明
* @author jixiaolong \@decard
* @date   2018年3月15日 19:41:39
* @{
*/
#ifndef IN
///<[jixiaolong|2018年3月15日 19:42:13] | 入参
#define IN
///<[jixiaolong|2018年3月15日 19:42:15] | 出参
#define OUT
///<[jixiaolong|2018年3月15日 19:42:15] | 出入参
#define IO
#endif
/** @} */


/**********************[结构体/全局变量定义]****************************/


/**********************[结构体/全局变量引用声明]************************/


/**********************[接口定义]****************************************/


#endif /*define.h*/
/**************************************************************************
**********************************【 define.h 结束】*******************
*************************************************************************
*/

