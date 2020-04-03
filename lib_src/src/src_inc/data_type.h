//
// Created by aaron on 18-3-5.
//

#ifndef A2TPUMAIN_DATA_TYPE_H
#define A2TPUMAIN_DATA_TYPE_H

#include <fcntl.h>
#include <pthread.h>
#include "define.h"

#define TRUE		1
#define FALSE		0

#define	STATUS_ERR	1
#define	STATUS_OK	0
typedef	void (* PrVoid)(void);
typedef	PrVoid (*PrPrVoid)(void);

typedef	unsigned char U8,U8_t;
typedef	unsigned char INT8U;
typedef unsigned short U16,INT16U;
typedef unsigned int U32,U32_t,INT32U;
typedef	signed char S8,INT8S;
typedef signed short S16,S16_t,INT16S;
typedef signed int S32,S32_t,INT32S;

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef byte
typedef unsigned char byte;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef ulong
typedef unsigned long ulong;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif
#ifndef uint64
typedef unsigned long long int uint64;
#endif

#define EXTERN		extern
#define SWAP16(x)	(((x & 0xff) << 8) | (x >> 8))
#define SWAP32(x)   (((x>>24)&0x000000ff) |((x>>8)&0x0000ff00)|((x<<8)&0x00ff0000)|((x<<24)&0xff000000))

#define PACK_STRUCT_FIELD(x) x __attribute__((packed))
#define PACK_STRUCT_STRUCT   __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define EN_FECIAL_CARD

#define HANDLE	int

#ifndef DWORD
#define DWORD	INT32U
#endif

#ifndef IN
#define IN
#define OUT
#endif

#ifndef UNUSED
#define UNUSED(x)	(void)(x)
#endif

typedef struct{
    INT8U cer;
    INT8U year;
    U8 month;
    U8 day;
    U8 hour;
    U8 min;
    U8 sec;
}TIME_STRUC;

typedef union
{
    INT8U	CharBuff[2];
    INT16U	IntBuff;
}TYPE_CHAR_INT ;

typedef union
{
    INT8U	CharBuff[4];
    INT16U	IntBuff[2];
    INT32U	LongBuff;
}TYPE_CHAR_LONG;

typedef struct
{
    unsigned char Command[4];
    unsigned char Lc;
    unsigned char  DataIn[512];
    unsigned char Le;
}APDU_SEND;

typedef struct
{
    unsigned char LenOut;
    unsigned char  DataOut[512];
    unsigned int  SW;
}APDU_RESP;

typedef struct
{
    unsigned char CLA;
    unsigned char INS;
    unsigned char P1;
    unsigned char P2;

    unsigned char LC;
    unsigned char DATA[240];

    unsigned char LE;

}ISO7816_ADPU_SEND;

typedef struct
{
    unsigned char LE;
    unsigned char DATA[240];

    unsigned char SW1;
    unsigned char SW2;
}ISO7816_ADPU_RESPONSE;


typedef struct
{
    INT8U	cSeLen;
    INT8U	cSendBuff[100];
    INT8U	cReLen;
    INT8U	cReBuff[100];
    INT8U	cTimeOut;
}DES_EXCHANGE;


typedef struct
{
    unsigned int	Le;
    unsigned char	inf[512];
    unsigned short	SW;
}APDU_RET;



#define APP_LOG_NAME		"JC5620"

#define new_free(p)					free(p)
#define new_malloc(s)				malloc(s)

#define NEW(T, nItems)					\
			(T *)new_malloc( sizeof(T)*(nItems) )
#define DELETE(p)						new_free(p)


//参数版本控制
typedef struct{
	unsigned char code[2+1];							//车载机状态监测结果， 00，不需要更新，01需要更新
	unsigned char NewAppBinVer[14+1];				//程序版本
	unsigned char NewBlacklistVer[14+1];			//黑名单版本
	unsigned char NewTicketsVer[14+1];				//票价参数
	unsigned char NewTermJurisVer[14+1];			//设备权限版本
	unsigned char NewOprJurisVer[14+1];				//操作员权限版本
	unsigned char NewTermParaVer[14+1];				//设备参数版本
	unsigned char NewWhiteListVer[128+1];			//mot白名单
	unsigned char NewStationInforVer[14+1];				//站点信息版本
	unsigned char NewLadderTicketVer[14+1];				//阶梯票价信息版本
	unsigned char NewChengeDisVer[14+1];				//换乘优惠版本.改为银联ODA黑名单
	unsigned char NewQrCertVer[14+1];					//二维码密钥证书版本
	unsigned char NewAppArgVer[14+1];					//二维码应用机构编码版本
	unsigned char NewUnionCAPKVer[14+1];				//银联密钥版本
	unsigned char NewUnionAIDVer[14+1];					//银联AID版本
	unsigned char NewSelfQrcodeVer[14+1];	//自有二维码版本
	unsigned char NewEMVBinBlackVer[14+1];	//银联卡bin黑名单
	unsigned char NewUnionTmKeyVer[14+1];	//银联双免主密钥版本
	unsigned char NewSysFileVer[14+1];				//系统文件版本：脚本文件、语音文件……
	unsigned char NewTradeReUploadVer[14+1];				//补采交易版本号
	unsigned char NewRFU[14+1];						//预留
	//...
}st_TEMP_VERSION_CTRL;
//extern st_TEMP_VERSION_CTRL ST_NewVerCtrlInfor, ST_ServVerCtrlInfor;		//版本控制信息


//终端状态 TRUE:正常 FALSE:失败
typedef struct{
	char TermNetState;				//网络状态
	char TermLogonState;			//终端签到状态(德卡前置)
	char TermUnionLogonState;		//终端签到状态(银联平台)
	char TermTicketState;			//票卡参数下载状态
	char TermBlackListState;	//黑名单下载状态
	char QrCodeKeyState;			//二维码密钥下载状态
	char TermTimeState;				//终端时间校准状态
	char PsamCardState;				//PSAM卡状态
	char DriverLogonState;			//司机签到状态
	char DriverLogonDateTime[15];			//司机签到时间
	char RFU[64];	//预留
}st_TERM_STATE;
extern st_TERM_STATE	ST_TermRunningState;	//终端运行状态


//数据库操作类
typedef enum
{
	DB_INSERT = 0,
	DB_DELETE = 1,//清空表
	DB_SELECT = 2,
	DB_UPDATE = 3,
	DB_DELETE_RECORD = 4,//删除记录
	DB_STATISTICAL_RECORD = 5,//统计未上送数量(交易表，统计未上送数量；参数表统计条数)
	DB_REUPLOAD_SELECT = 6,	//补采查询
	DB_TRANSACTION_BEGIN = 7,	//事务开启
	DB_TRANSACTION_COMIT = 8,	//事务结束
	DB_STATISTICAL_MOT_CRT_DAY = 9,	//交通部当天交易统计
	DB_STATISTICAL_MOT_TATOL = 10,	//交通部全部交易统计
	DB_REUPLOAD_CLR		//数据库清理

}DB_OPR_TYPE;

//ftp信息
typedef struct FTP_INFOR
{
    unsigned char FTPServIP[20];		    //ftp服务端IP
    unsigned char FTPServPort[10];		    //ftp服务端端口
    unsigned char FTPServPath[128];		    //ftp服务端路径
	unsigned char FTPServUser[128]; 	    //ftp用户名
	unsigned char FTPServPassword[128];	    //ftp密码

    unsigned char FTPLocalPath[256];		//本地存放路径
    unsigned char FTPLocalFileName[256];	//本地存放文件名

    //ftp文件信息
    unsigned char FTPFileName[256];		    //ftp服务端文件名
    unsigned char FTPAppFilemd5[65];	    //ftp app Md5值
    
}FTP_INFOT_T;

//卡类型控制参数
typedef struct XCXQGJ_CARDTYPERATE_struct
{
    uchar aucCardType[4];           //卡类型
    uchar aucWalletDiscount[6];     //钱包折扣
    uchar aucMonthDiscount[6];      //月票折扣
    uchar aucContStatus[4];         //连刷状态 1可以连刷，0不可以连刷
    uchar aucLimitTime[10];          //限制时间 单位秒
}XCXQGJ_CARDTYPERATE_T;//卡类折扣信息

//APP ID数据结构
#define SUM_APP_LIST_AID 5
typedef struct
{
    unsigned char AID[17];      //应用AID
    unsigned char AidLen;       //AID长度
    unsigned char Label[17];    //应用标签的值
    unsigned char LabelLen;     //应用标签的长度
    unsigned char prior;        //应用优先指示器
} CUP_APP_LIST;

typedef struct CUP_APP_LIST_struct
{
    unsigned int applist_num;   		//终端存的应用列表个数
    CUP_APP_LIST applist[SUM_APP_LIST_AID]; 	//终端存的应用列表
} CUP_APP_LIST_T;



//终端运行参数数据结构
typedef struct Struct_Once_Simple
{
	unsigned char szTermType[64+1];			//设备类型
	unsigned char szTicketsType[2+1];		//票制类型,	01:一票制  		02:分段计费
	unsigned char szTicketsPrice[8+1];		//票价[一票制]
	unsigned char szCarTeamID[10+1];			//路队编号
	unsigned char szCompanyNumb[7];			//公司编号，县区编号
	unsigned char szCarLineID[7];			//线路编号
	unsigned char szCarLineName[41];		//线路名称
	unsigned char szTermID[32];				//终端编号.车载机编号.SN号
	unsigned char szPosType[8];				//终端类型
	unsigned char szCarID[13];				//车辆编号
	unsigned char szDriverID[7];			//司机编号
	unsigned char szUnitNumber[7];			//单位编号
    unsigned char szStatusLogin[3];         //银联设备签到状态 01已签到 00未签到
    unsigned char szStatusAid[3];           //AID下载状态        01 已下载 00未下载
    unsigned char szStatusCapk[3];          //公钥下载状态 01 已下载 00未下载
    unsigned char szMerchantName[128];		//商户名称
    unsigned char szMerchantId[16];			//商户号
    unsigned char szTerminalId[9];			//终端号	银联单独使用
    unsigned char szBatchNumb[7];			//批次号
    unsigned char szSerilaNumb[7];			//流水号
	unsigned char szDcServIp[30];			//德卡服务IP
	unsigned char szDcServPort[6];			//德卡服务端口
	unsigned char szServIp[30];				//银联服务IP/域名
	unsigned char szServPort[6];			//银联服务端口
	unsigned char szAPN[32];			    //服务端APN
	unsigned char szTmMstkey[32+1];			//终端主密钥
	unsigned char ucaPinKey[32+1];		    ///PINLEY
	unsigned char ucaMacKey[32+1];			///MACKEY
	unsigned char ucaTDKey[32+1]; 			///TDKEY
	unsigned char szTPDU[16+1];				//银联TPDU
	unsigned char szTermMaxSerail[8+1];				//终端最大交易笔数
	unsigned char aucTermPubKey[256+1];				//sm公钥,用于德卡平台终端签到时对随机过程密钥(签到解密工作密钥)进行sm2加密使用
	unsigned char ucaDCMacKey[32+1];				//德卡报文签名密钥，用于德卡平台报文签名使用，签到时获取
	unsigned char szSIM_CCID[32+1];					//SIM卡CCID号码
	unsigned char szSIM_IMSI[32+1];					//SIM卡IMSI号码
	unsigned char szSIM_IMEI[32+1];					//SIM卡IMEI号码
    unsigned char aucDebitMny[12+1];        		//借记总金额
    unsigned char aucDebitAct[3+1];         		//借记总笔数
   	unsigned char aucPSAMID[12+1];        			//PSAM卡号
   	unsigned char aucdbPath[512+1];        			//数据库路径
   	unsigned char auccfgFilePath[512+1];        	//配置文件路径
  
	//德卡聚合支付平台通讯参数
    char shop_num[32 + 1]; 	//商户编码(商户项目编号)
	char serv_add[64 + 1];	//服务器地址
	char serv_port[8 + 1];  //服务端口
    char rand_nb[32 + 1]; 	//随机数(用于签到获取工作密钥时的密钥)
    char serv_pubkey[512 + 1]; 	//公钥
    char mak[128 + 1]; 		//工作密钥

	//ftp信息.线路参数下发时获取
	FTP_INFOT_T stFTPInfo;

	unsigned char szRFU[64];	//预留	
} STRUCT_ONCE_SIMPLE;



// Alt+/ :Look up reference
//   F9 :Ident left
//    F10 :Ident right
//shift+F8 : hilight word

#endif //A2TPUMAIN_DATA_TYPE_H
