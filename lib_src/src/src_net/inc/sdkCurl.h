
#ifndef SDKCURL_H
#define SDKCURL_H

//#include <curl/curl.h>
#include "curl/curl.h"


#ifndef bool
#define bool char
#endif

#ifndef		true
#define		true		1
#endif

#ifndef		false
#define		false		0
#endif

#ifndef SDK_OK
#define SDK_OK                       (1)
#define SDK_ERR                      (-1)
#define SDK_TIME_OUT                 (-2)
#define SDK_PARA_ERR                 (-4)
#endif

//证书格式
#define SDK_CURL_PEM_TYPE       0   // PEM  格式的证书
#define SDK_CURL_DER_TYPE       1   // DER  格式的证书
#define SDK_CURL_P12_TYPE       3   // PKCS#12-encoded files 格式的证书 (OpenSSL versions 0.9.3 and later 支持)


#define URL_MAX_LEN		    1024
#define CA_PATH_MAX_LEN	    256
#define SDK_EXT_CURL_BUF_SIZE 2048+1024

//协议 http/https
typedef enum _sdk_curl_protocol
{
    SDK_CURL_PROTO_HTTP = 0,
    SDK_CURL_PROTO_HTTPS
}SDK_CURL_PROTOCOL;

//请求模式
typedef enum _sdk_curl_req_mode
{
    SDK_CURL_REQ_POST = 0,
    SDK_CURL_REQ_GET
}SDK_CURL_REQ_MODE;

typedef struct _sdk_curl_param
{
    SDK_CURL_PROTOCOL eCurlProtocol;
    SDK_CURL_REQ_MODE eReqMode;

    //for https only
    bool bIsVerifyServer;    //true: 终端验证服务器；false: 终端不验证服务器
    unsigned char asCAcertPath[CA_PATH_MAX_LEN];   //如果bisverifyserver为true，需要加载服务器CA根证书 ，路径ASCII格式
    unsigned char asCliCertPath[CA_PATH_MAX_LEN];  //双向认证、客户端
    unsigned char asCliCertPwd[CA_PATH_MAX_LEN];  //密码
    unsigned short  uiCertType;                                    //证书格式类型
    //end

    unsigned char asUrl[URL_MAX_LEN];      //url
    unsigned short  usPort;                 //端口号

    unsigned char *pheSendBuf;             //发送数据
    unsigned int uiSendLen;              //发送数据长度
    unsigned int    uiSendMaxBufSize;       //发送数据最大缓存
    unsigned char *pheSendMac;             //发送数据Mac

    unsigned char *pheRecvBuf;             //接收数据缓存
    unsigned int     uiRecvLen;              //接收数据长度
    unsigned int     uiRecvMaxBufSize;       //接收数据最大缓存

    unsigned int  uiTimeout;              //超时时间(ms)

    bool bIsGetProtelHead;    		//true: 需要返回协议头；false: 不需要返回协议头


}SDK_CURL_PARAM;

typedef struct _obj_curl
{
    SDK_CURL_PARAM *pstCurlParam;
    CURLcode        eRetCode;
    int             curl_flag;
}OBJ_CURL;

//http/https数据结构
typedef struct _sdk_ext_curl_data
{
    bool        bMethod;            //获取方式 true:GET方式；false: POST方式
    unsigned char    *psURL;            // 因特网地址
    unsigned short          nPort;         //服务器端口号

    unsigned char    *psSend;           	//发送数据的缓存
    unsigned int          nSendLen;       //发送数据的大小
    unsigned char    *psSendMac;           //发送数据的mac

    unsigned char          *psRecv;         // 接收数据的缓存区
    unsigned int          nRvcLen;         // 接收数据实际大小
    unsigned int          nRecvMaxLen;        // 接收缓存区的最大值, (根据CURL定义，最大接受值为16K)

    unsigned char    *psCaFile;         //  CA证书的文件，包括绝对路径；为NULL时，不进行服务器的认证
    unsigned int      nTimeout;      // 超时时间，ms

    unsigned char    *psCliCert;      //需要双向认证时，客户端的证书
    unsigned char    *psCliPwd;    	//需要双向认证时，客户端的密码

    bool bIsGetProtelHead;    		//true: 需要返回协议头；false: 不需要返回协议头

    int         nCertType;            //证书的格式, SDK_CURL_PEM_TYPE、SDK_CURL_DER_TYPE、SDK_CURL_P12_TYPE
}SDK_EXT_CURL_DATA;


extern int sdkExtCurl(SDK_EXT_CURL_DATA *pstData, bool bForceSSL);


#endif


