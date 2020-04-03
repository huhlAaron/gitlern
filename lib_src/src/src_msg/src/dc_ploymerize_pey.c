/*
 * dc_ploymerize_pey.c
 *	德卡聚合支付平台协议定义
 *  Created on: December 12, 2019  15:03:17
 *      Author: huhongliang@decard.com
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "dc_ploymerize_pey.h"
#include "data_type.h"
#include "cjson.h"
#include "sdkCurl.h"
#include "main_api.h"
#include "config.h"
#include "gmalg_inc.h"

//报文mac数据缓存
static char MSG_MAC[64] = {0};

/**
 * @Author: hhl \@decard
 * @fn     	static int msg_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen);
 * @brief:	s
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:04:43
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:04:43
*/
static int msg_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen)
{
    int ret = -1;
	char rcvTemp[1024 * 8] = {0};
    uint nRevLen = 1024 * 8;
    SDK_EXT_CURL_DATA curl_data;

    Uart_Printf("-----* %s *-----\r\n", __func__);

    memset(&curl_data, 0, sizeof(SDK_EXT_CURL_DATA));
    
    curl_data.bMethod = false;      // post
    curl_data.psURL = pURL; 	    // URL
    curl_data.nPort = port;         // port
    curl_data.psSend = pSend;
    curl_data.nSendLen = Sendlen;
    curl_data.psRecv = pRev;
    curl_data.nRecvMaxLen = nRevLen;
    curl_data.psCaFile = NULL;   // 没有证书
    curl_data.nTimeout = 10*1000;//10秒

	//curl_data.psSendMac = send_data_mac;
	curl_data.bIsGetProtelHead = false;

    ret = sdkExtCurl(&curl_data, false);
    if(ret == SDK_OK)
    {
        Uart_Printf("recvie date:=========>[%d] [%d]\n %s \r\n", curl_data.nRvcLen, strlen((char *)pRev), pRev);

		if(curl_data.bIsGetProtelHead == true)
		{
			char TempBuff[64] = {0};

			memset(rcvTemp, 0, sizeof(rcvTemp));
			memcpy(rcvTemp, strchr(pRev, '{'), curl_data.nRvcLen);

			memset(pRev, 0, sizeof(pRev));
			memcpy(pRev, rcvTemp, curl_data.nRvcLen);
		}

    }
    else
    {
        Uart_Printf("recvie err ret = %d\r\n", ret);
        return ret;
    }
    
    return 0;
}



/*只交换cJSON中的数据内容，不交换next，prev 指针，前后顺数不变*/
int swapCont(cJSON *contSrc,cJSON *contDest)
{
	if(contSrc == NULL || contDest == NULL)
	{
		return -1;
	}

	cJSON tmpData;
	memset(&tmpData,0,sizeof(tmpData));
	tmpData.child = contSrc->child;
	tmpData.type = contSrc->type;
	tmpData.valuestring = contSrc->valuestring;
	tmpData.valueint = contSrc->valueint;
	tmpData.valuedouble = contSrc->valuedouble;
	tmpData.string = contSrc->string;

	contSrc->child = contDest->child;
	contSrc->type = contDest->type;
	contSrc->valuestring = contDest->valuestring;
	contSrc->valueint = contDest->valueint;
	contSrc->valuedouble = contDest->valuedouble;
	contSrc->string = contDest->string;

	contDest->child = tmpData.child;
	contDest->type = tmpData.type;
	contDest->valuestring = tmpData.valuestring;
	contDest->valueint = tmpData.valueint;
	contDest->valuedouble = tmpData.valuedouble;
	contDest->string = tmpData.string;

	return 0;
}

//json排序
int sortJson(cJSON *jsonData)
{
	if(jsonData == NULL)
	{
		return 0;
	}

	int i = 0;
    int countArrItem = 0;
	int ret = 0;

	cJSON *p = jsonData;  // p 向后遍历指针
	cJSON *srcData = p ;
    cJSON *destData = p;

	cJSON *head = p;
	cJSON *q = p;  		// q 向前遍历指针


	if(p->type == cJSON_Array)  /*数组内容排序*/
	{
		countArrItem = cJSON_GetArraySize(p);
		for(i = 0;i<countArrItem;i++)
		{
			cJSON *arrData = cJSON_GetArrayItem(p,i);
			ret = sortJson(arrData->child);
			if(ret < 0)
			{
				return -1;
			}
		}
	}

	p = p->next;
	if(p == NULL)
	{
		return 0;
	}
	
	while(p != NULL)   //向后遍历
    {
		if(p->type == cJSON_Array)
		{
			countArrItem = cJSON_GetArraySize(p);
			for(i = 0;i<countArrItem;i++)
			{
				cJSON *arrData = cJSON_GetArrayItem(p,i);
				ret = sortJson(arrData->child);
				if(ret < 0)
				{
					return -2;
				}
			}
		}	

		destData = p->prev;
		srcData = p;
	    if(strcmp(srcData->string,destData->string)<0) //升序 排序
		{
			ret = swapCont(srcData,destData);
			if (ret <0)
			{
				return -3;
			}
			
			q = p;
			while(q != head)   //向前遍历
			{
				destData = q->prev;
			    srcData = q;
				if(strcmp(srcData->string,destData->string)<0)
				{
					ret = swapCont(srcData,destData);
					if (ret <0)
					{
						return -4;
					}
				}
				q = q->prev;
			}
		}
		p = p->next;
	}
	return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int json_walk(cJSON *jsonData);
 * @brief:	//json遍历
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 19, 2019  13:46:40
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 19, 2019  13:46:40
*/
int json_walk(cJSON *jsonData)
{
	cJSON *p = jsonData;  // p向后遍历指针

	if(p == NULL)
	{
		return 0;
	}
	
	do
    {
    	if(p->type == cJSON_Number)
    	{
    		Uart_Printf("key:%s value:%d\n", p->string, p->valueint);
    	}
		else if(p->type == cJSON_String)
		{
			Uart_Printf("key:%s value:%s\n", p->string, p->valuestring);
		}

		p = p->next;
	}while(p != NULL);   //向后遍历

	return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int dc_get_rand(char *outBuf, int byte);
 * @brief:	产生随机数
 * @param[in] :byte:随机数长度（多少位）
 * @param[out]:outBuf:随机数缓存
 * @return:
 * @Date:March 30, 2020  17:01:08
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  17:01:08
*/
int dc_get_rand(char *outBuf, int byte)
{
	int i = 0;
	
	memset(outBuf, 0, byte);
	srand((int)time(0));
	for(i = 0; i < byte; i ++)
	{
		outBuf[i] = rand()%10 + 48;
	}
	outBuf[i] = '\0';

	return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	char *  dc_ploymerize_return_msg_mac(char *pOutMac);
 * @brief:	数据报文mac数据获取
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:April 01, 2020  16:30:00
 * @Last Modified by:hhl \@decard
 * @Last Modified time:April 01, 2020  16:30:00
*/
char * dc_ploymerize_return_msg_mac(char *pOutMac)
{
	memcpy(pOutMac, MSG_MAC, strlen(MSG_MAC));
	return pOutMac;
}

/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_get_mac(const char *mak, const char *inData, int inDataLen, char *outMac);
 * @brief:	计算报文mac数据
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:April 01, 2020  14:50:34
 * @Last Modified by:hhl \@decard
 * @Last Modified time:April 01, 2020  14:50:34
*/
int dc_ploymerize_get_mac(const char *mak, const char *inData, int inDataLen, char *outMac)
{
    uchar aucBuffer[128] = {0};
    uchar auc_mak_hex[128] = {0};

	char cMacData[4096] = {0};

	memset(cMacData, 0, sizeof(cMacData));
	memcpy(cMacData, inData, inDataLen);

	//mac计算
	memset(aucBuffer, 0, sizeof(aucBuffer));
	memset(auc_mak_hex, 0, sizeof(auc_mak_hex));
	
	MyAHex(mak, auc_mak_hex, strlen(mak));
	DumpData("mak", auc_mak_hex, strlen(mak)/2);
	
	DC_CreateSM4CBCMAC(auc_mak_hex, cMacData, inDataLen, aucBuffer);
	DumpData("MAC", aucBuffer, 16);
	memset(MSG_MAC, 0, sizeof(MSG_MAC));
	MyHexA(aucBuffer, MSG_MAC, 16);
	memcpy(outMac, MSG_MAC, 32);

	return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:04:43
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:04:43
*/
int dc_ploymerize_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen)
{
    int ret = -1;
	char rcvTemp[1024 * 8] = {0};
    uint nRevLen = 1024 * 8;
    SDK_EXT_CURL_DATA curl_data;

    Uart_Printf("-----* %s *-----\r\n", __func__);

    memset(&curl_data, 0, sizeof(SDK_EXT_CURL_DATA));
    
    curl_data.bMethod = false;      // post
    curl_data.psURL = pURL; 	    // URL
    curl_data.nPort = port;         // port
    curl_data.psSend = pSend;
    curl_data.nSendLen = Sendlen;
    curl_data.psRecv = pRev;
    curl_data.nRecvMaxLen = nRevLen;
    curl_data.psCaFile = NULL;   // 没有证书
    curl_data.nTimeout = 10*1000;//10秒

	//curl_data.psSendMac = send_data_mac;
	curl_data.bIsGetProtelHead = false;

    ret = sdkExtCurl(&curl_data, false);
    if(ret == SDK_OK)
    {
        Uart_Printf("recvie date:=========>[%d] [%d]\n %s \r\n", curl_data.nRvcLen, strlen((char *)pRev), pRev);

		if(curl_data.bIsGetProtelHead == true)
		{
			char TempBuff[64] = {0};

			memset(rcvTemp, 0, sizeof(rcvTemp));
			memcpy(rcvTemp, strchr(pRev, '{'), curl_data.nRvcLen);

			memset(pRev, 0, sizeof(pRev));
			memcpy(pRev, rcvTemp, curl_data.nRvcLen);
		}
    }
    else
    {
        Uart_Printf("recvie err ret = %d\r\n", ret);
        return ret;
    }
    
    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_pubkey_download_unpack(uchar *Msg, STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:March 30, 2020  16:12:57
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  16:12:57
*/
static int dc_ploymerize_pay_pubkey_download_unpack(uchar *Msg, STRUCT_ONCE_SIMPLE *pterm_para)
{
    int iRet = 0;
	
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Object)
		{
			return 1;
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "public_key");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("public_key:%s\n", Jsonrst->valuestring);
			memcpy(pterm_para->serv_pubkey, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
    }
    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_pay_pubkey_download(const STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	公钥下载
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:March 30, 2020  15:02:42
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  15:02:42
*/
int dc_ploymerize_pay_pubkey_download(STRUCT_ONCE_SIMPLE *pterm_para)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[32] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//数据组装
	{
		//无参调用
	}
	uiSendLen = strlen(aucSendBuff);
	
	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/user/public_key", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
       return iret;
    }

	//数据解析
	iret = dc_ploymerize_pay_pubkey_download_unpack(aucRecvBuff, pterm_para);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_pubkey_download_unpack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_login_pack(STRUCT_ONCE_SIMPLE *pterm_para, uchar *oJson);
 * @brief:	设备签到报文组装
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:March 30, 2020  16:53:00
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  16:53:00
*/
static int dc_ploymerize_pay_login_pack(STRUCT_ONCE_SIMPLE *pterm_para, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};
    uchar aucBuffer[256] = {0};
    uchar au_pubkey[128] = {0};
    uchar au_rand[128] = {0};
    uchar au_CipherText[128] = {0};
    int ipherTextlen = 0;

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }
	
	cJSON_AddStringToObject(cjRoot, "sn", pterm_para->szTermID);
	cJSON_AddStringToObject(cjRoot, "model", pterm_para->szTermType);
	cJSON_AddStringToObject(cjRoot, "shop_num", pterm_para->shop_num);
	//随机数
	memset(au_pubkey, 0, sizeof(au_pubkey));
	memset(au_CipherText, 0, sizeof(au_CipherText));
	MyAHex(pterm_para->serv_pubkey, au_pubkey, 128);
	DumpData("sm2 data:", pterm_para->rand_nb, 16);
	DumpData("sm2 pubkey:", au_pubkey, 64);
	GMAlg_Sm2Encrypt(SM2_Mode_C1C3C2, pterm_para->rand_nb, 16, au_pubkey, 64, au_CipherText, &ipherTextlen);
	//SM2Encrypt ( SM2_C1C3C2, pterm_para->rand_nb, 16, au_pubkey, 64, au_CipherText, &ipherTextlen );
	DumpData( "GMAlg_Sm2Encrypt Data:", au_CipherText, ipherTextlen );
	memset(aucBuffer, 0, sizeof(aucBuffer));
	MyHexA(au_CipherText, aucBuffer, ipherTextlen);
	cJSON_AddStringToObject(cjRoot, "crypt", aucBuffer);
	
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}

	//排序遍历测试
	//sortJson(cjRoot->child);
	//json_walk(cjRoot->child);

	//将json形式打印成普通字符串形式
	cjsonstr = cJSON_PrintUnformatted(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		strcpy(&oJson[0], cjsonstr);
		free(cjsonstr);
		cjsonstr = NULL;
	}
    Uart_Printf("oJson: [%s]\r\n", oJson);
	
    if(cjsonstr)
	{
		free(cjsonstr);
		cjsonstr = NULL;
		
	}
	if(cjRoot)
	{
		cJSON_Delete(cjRoot);
		cjRoot = NULL;
	}

    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_pubkey_login_unpack(uchar *Msg, STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:March 30, 2020  16:12:57
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  16:12:57
*/
static int dc_ploymerize_pay_pubkey_login_unpack(uchar *Msg, STRUCT_ONCE_SIMPLE *pterm_para)
{
    int iRet = 0;
	
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
	uchar au_rand[128] = {0};
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Object)
		{
			return 1;
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "key");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("key:%s\n", Jsonrst->valuestring);

			//mak解密
			memset(aucBuff, 0, sizeof(aucBuff));
			memset(aucCode, 0, sizeof(aucCode));
			
			MyAHex(Jsonrst->valuestring, aucBuff, strlen(Jsonrst->valuestring));
			DumpData("rand", pterm_para->rand_nb, 16);
			GMAlg_Sm4Decrypt(pterm_para->rand_nb, aucBuff, aucCode);
			MyHexA(aucCode, pterm_para->mak, 16);
			Uart_Printf("mak:%s\n", pterm_para->mak);

			//mak 存储
			cfg_set_para_key_value(TARM_MAK, pterm_para->mak);
	    }
    }
    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_pay_login(const STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	设备签到
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:March 30, 2020  15:03:11
 * @Last Modified by:hhl \@decard
 * @Last Modified time:March 30, 2020  15:03:11
*/
int dc_ploymerize_pay_login(const STRUCT_ONCE_SIMPLE *pterm_para)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//16字节随机数
	memset(aucBuffer, 0, sizeof(aucBuffer));
	dc_get_rand(pterm_para->rand_nb, 16);

	//数据组装
	dc_ploymerize_pay_login_pack(pterm_para, aucSendBuff);
	uiSendLen = strlen(aucSendBuff);
	
	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/user/key", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
       return iret;
    }

	//数据解析
	iret = dc_ploymerize_pay_pubkey_login_unpack(aucRecvBuff, pterm_para);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_pubkey_download_unpack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}



/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson);
 * @brief:	德卡聚合支付消费 组包接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  18:07:38
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  18:07:38
*/
static int dc_ploymerize_pay_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};
    uchar aucBuffer[32] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

    cJSON_AddStringToObject(cjRoot, "scene", tradeDate->scene);
    cJSON_AddStringToObject(cjRoot, "qrcode", tradeDate->qrcode);
    cJSON_AddStringToObject(cjRoot, "body", tradeDate->body);
    cJSON_AddNumberToObject(cjRoot, "totalFee", atoi(tradeDate->totalFee));
    cJSON_AddStringToObject(cjRoot, "terminalIp", tradeDate->terminalIp);
	cJSON_AddStringToObject(cjRoot, "outTradeNo", tradeDate->outTradeNo);
	cJSON_AddStringToObject(cjRoot, "terminalParams", tradeDate->terminalParams);
	cJSON_AddStringToObject(cjRoot, "model", tradeDate->model);
	cJSON_AddStringToObject(cjRoot, "sn", tradeDate->sn);
	cJSON_AddStringToObject(cjRoot, "shop_num", tradeDate->shop_num);

	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}

	//排序遍历测试
	//sortJson(cjRoot->child);
	//json_walk(cjRoot->child);

	//将json形式打印成普通字符串形式
	cjsonstr = cJSON_PrintUnformatted(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		strcpy(&oJson[0], cjsonstr);
		free(cjsonstr);
		cjsonstr = NULL;
	}
    Uart_Printf("oJson: [%s]\r\n", oJson);
	
    if(cjsonstr)
	{
		free(cjsonstr);
		cjsonstr = NULL;
		
	}
	if(cjRoot)
	{
		cJSON_Delete(cjRoot);
		cjRoot = NULL;
	}

    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付消费 解包接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  18:08:01
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  18:08:01
*/
static int dc_ploymerize_pay_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate)
{
    int iRet = 0;
	
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		memcpy(tradeDate->errMsg, JsonData->valuestring, strlen(JsonData->valuestring) >= 256 ? 256 : strlen(JsonData->valuestring));
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Object)
		{
			return 1;
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "openId");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("openId:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->openId, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "outTradeNo");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("outTradeNo:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "payType");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("payType:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->payType, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "pay_type");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("pay_type:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->pay_type, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "timeEnd");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("timeEnd:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->timeEnd, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "totalFee");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("totalFee:%d\n", Jsonrst->valueint);
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "transactionId");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("transactionId:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->transactionId, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
    }

    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_pay(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付消费接口
 * @param[in] :pterm_para：参数数据结构  tradeDate：交易数据结构
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  15:11:16
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:11:16
*/
int dc_ploymerize_pay(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//数据组装
	iret = dc_ploymerize_pay_pack(pterm_para, tradeDate, aucSendBuff);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_pack error : %d\r\n", iret);
		return iret;
	}
	uiSendLen = strlen(aucSendBuff);

	//MAC计算
	memset(aucBuffer, 0, sizeof(aucBuffer));
	dc_ploymerize_get_mac(pterm_para->mak, aucSendBuff, uiSendLen, aucBuffer);
	
	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/pay/pay", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
		
       	return iret;
    }

	//数据解析
	iret = dc_ploymerize_pay_unpack(aucRecvBuff, tradeDate);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_unpack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_chack_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson);
 * @brief:	德卡聚合支付消费查询接口 组包接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  18:07:38
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  18:07:38
*/
static int dc_ploymerize_pay_chack_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};
    uchar aucBuffer[32] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

    //cJSON_AddStringToObject(cjRoot, "transactionId", tradeDate->scene);
	cJSON_AddStringToObject(cjRoot, "outTradeNo", tradeDate->outTradeNo);
	cJSON_AddStringToObject(cjRoot, "shop_num", tradeDate->shop_num);
	cJSON_AddStringToObject(cjRoot, "model", tradeDate->model);
	cJSON_AddStringToObject(cjRoot, "sn", tradeDate->sn);

	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}

	//将json形式打印成普通字符串形式
	cjsonstr = cJSON_PrintUnformatted(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		strcpy(&oJson[0], cjsonstr);
		free(cjsonstr);
		cjsonstr = NULL;
	}
    Uart_Printf("oJson: [%s]\r\n", oJson);
	
    if(cjsonstr)
	{
		free(cjsonstr);
		cjsonstr = NULL;
		
	}
	if(cjRoot)
	{
		cJSON_Delete(cjRoot);
		cjRoot = NULL;
	}

    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_pay_chack_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付消费 解包接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  18:08:01
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  18:08:01
*/
static int dc_ploymerize_pay_chack_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate)
{
    int iRet = 0;
	
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		memcpy(tradeDate->errMsg, JsonData->valuestring, strlen(JsonData->valuestring) >= 256 ? 256 : strlen(JsonData->valuestring));
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Object)
		{
			return 1;
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "openId");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("openId:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->openId, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "outTradeNo");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("outTradeNo:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "payType");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("payType:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->payType, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "pay_type");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("pay_type:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->pay_type, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "timeEnd");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("timeEnd:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->timeEnd, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "totalFee");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("totalFee:%d\n", Jsonrst->valueint);
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "transactionId");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("transactionId:%s\n", Jsonrst->valuestring);
			memcpy(tradeDate->transactionId, Jsonrst->valuestring, strlen(Jsonrst->valuestring));
	    }
    }

    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_pay_chack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付消费查询接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  15:11:20
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:11:20
*/
int dc_ploymerize_pay_chack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//数据组装
	iret = dc_ploymerize_pay_chack_pack(pterm_para, tradeDate, aucSendBuff);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_pack error : %d\r\n", iret);
		return iret;
	}
	uiSendLen = strlen(aucSendBuff);

	//MAC计算
	memset(aucBuffer, 0, sizeof(aucBuffer));
	dc_ploymerize_get_mac(pterm_para->mak, aucSendBuff, uiSendLen, aucBuffer);
	
	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/pay/query", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
       return iret;
    }


	//数据解析
	iret = dc_ploymerize_pay_chack_unpack(aucRecvBuff, tradeDate);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_unpack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}


/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_return_goods_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 13, 2019  09:54:25
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  09:54:25
*/
static int dc_ploymerize_return_goods_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};
    uchar aucBuffer[32] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

	cJSON_AddStringToObject(cjRoot, "model", tradeDate->model);
    cJSON_AddStringToObject(cjRoot, "trade_no", tradeDate->outTradeNo);
	sprintf(aucBuffer, "%.2f", atoi(tradeDate->totalFee) * 0.01);
    cJSON_AddStringToObject(cjRoot, "refund_fee", aucBuffer);
    cJSON_AddStringToObject(cjRoot, "refund_reason", "123456");
	cJSON_AddStringToObject(cjRoot, "out_refund_no", tradeDate->out_refund_no);
	cJSON_AddStringToObject(cjRoot, "shop_num", tradeDate->shop_num);
	cJSON_AddStringToObject(cjRoot, "sn", tradeDate->sn);

	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}

	//将json形式打印成普通字符串形式
	cjsonstr = cJSON_PrintUnformatted(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		strcpy(&oJson[0], cjsonstr);
		free(cjsonstr);
		cjsonstr = NULL;
	}
    Uart_Printf("oJson: [%s]\r\n", oJson);
	
    if(cjsonstr)
	{
		free(cjsonstr);
		cjsonstr = NULL;
		
	}
	if(cjRoot)
	{
		cJSON_Delete(cjRoot);
		cjRoot = NULL;
	}

    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_return_goods_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 13, 2019  09:54:28
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  09:54:28
*/
static int dc_ploymerize_return_goods_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate)
{
    int iRet = 0;
	
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		memcpy(tradeDate->errMsg, JsonData->valuestring, strlen(JsonData->valuestring) >= 256 ? 256 : strlen(JsonData->valuestring));
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Object)
		{
			return 1;
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "fund_change");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("fund_change:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "gmt_refund_pay");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("gmt_refund_pay:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "payType");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("payType:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "outRefundNo");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("outRefundNo:%s\n", Jsonrst->valuestring);
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "outTradeNo");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("outTradeNo:%s\n", Jsonrst->valuestring);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "payType");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("payType:%s\n", Jsonrst->valuestring);
	    }

		Jsonrst = cJSON_GetObjectItem(JsonData, "refundFee");
	    if (Jsonrst != NULL)
	    {
			Uart_Printf("refundFee:%d\n", Jsonrst->valueint);
	    }
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "refundId");
		if (Jsonrst != NULL)
		{
			Uart_Printf("refundId:%s\n", Jsonrst->valuestring);
		}
		
		Jsonrst = cJSON_GetObjectItem(JsonData, "transactionId");
		if (Jsonrst != NULL)
		{
			Uart_Printf("transactionId:%s\n", Jsonrst->valuestring);
		}
    }

    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}



/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_return_goods(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付退货接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  15:11:29
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:11:29
*/
int dc_ploymerize_return_goods(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//数据组装
	iret = dc_ploymerize_return_goods_pack(pterm_para, tradeDate, aucSendBuff);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_return_goods_pack error : %d\r\n", iret);
		return iret;
	}
	uiSendLen = strlen(aucSendBuff);

	//MAC计算
	memset(aucBuffer, 0, sizeof(aucBuffer));
	dc_ploymerize_get_mac(pterm_para->mak, aucSendBuff, uiSendLen, aucBuffer);

	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/pay/refund", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
       return iret;
    }

	//数据解析
	iret = dc_ploymerize_return_goods_unpack(aucRecvBuff, tradeDate);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_return_goods_pack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}



/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_return_goods_chack_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 13, 2019  09:54:25
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  09:54:25
*/
static int dc_ploymerize_return_goods_chack_pack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

	//cJSON_AddStringToObject(cjRoot, "outTradeNo", tradeDate->outTradeNo);
	//cJSON_AddStringToObject(cjRoot, "outTradeNo", tradeDate->outTradeNo);
	cJSON_AddStringToObject(cjRoot, "model", tradeDate->model);
	cJSON_AddStringToObject(cjRoot, "sn", tradeDate->sn);
	cJSON_AddStringToObject(cjRoot, "out_refund_no", tradeDate->out_refund_no);
	cJSON_AddStringToObject(cjRoot, "shop_num", tradeDate->shop_num);

	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}

	//将json形式打印成普通字符串形式
	cjsonstr = cJSON_PrintUnformatted(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		strcpy(&oJson[0], cjsonstr);
		free(cjsonstr);
		cjsonstr = NULL;
	}
    Uart_Printf("oJson: [%s]\r\n", oJson);
	
    if(cjsonstr)
	{
		free(cjsonstr);
		cjsonstr = NULL;
		
	}
	if(cjRoot)
	{
		cJSON_Delete(cjRoot);
		cjRoot = NULL;
	}

    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	static int dc_ploymerize_return_goods_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 13, 2019  09:54:28
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 13, 2019  09:54:28
*/
static int dc_ploymerize_return_goods_chack_unpack(uchar *Msg, decard_ploymerize_pay *tradeDate)
{
    int iRet = 0;
	int iObjectNum = 0;
	int i = 0;
    uchar aucBuff[512] = {0};
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    uchar *cjsonstr = NULL;
	cJSON *cjRoot = NULL;
    cJSON *JsonData = NULL;
	cJSON *Jsonrst = NULL;
	cJSON *ObjectArray = NULL;
	
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!Msg)
        return 1;

    Uart_Printf("ResponseMsg:%s\r\n", Msg);
    
    cjRoot = cJSON_Parse(Msg);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
	//将json形式打印成正常字符串形式
	cjsonstr = cJSON_Print(cjRoot);
	Uart_Printf("%s\n",cjsonstr);
	if(cjsonstr != NULL)
	{
		free(cjsonstr);
	}
    
    //{"msg":"error","code":"999"} 
    

    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData != NULL)
    {
		Uart_Printf("code:%d\n", JsonData->valueint);
		if(JsonData->valueint != 1)
		{
			return 1;
		}
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData != NULL)
    {
		Uart_Printf("aucMsg:%s\n", JsonData->valuestring);
		memcpy(tradeDate->errMsg, JsonData->valuestring, strlen(JsonData->valuestring) >= 256 ? 256 : strlen(JsonData->valuestring));
		if(memcmp(JsonData->valuestring, "SUCCESS", 7) != 0)
		{
			//return 1;
		}
    }

    JsonData = cJSON_GetObjectItem(cjRoot, "result");
    if (JsonData != NULL)
    {
		Uart_Printf("JsonData->type:%d\n", JsonData->type);
		if(JsonData->type != cJSON_Array)
		{
			return 1;
		}

		//获取数组元素个数
		iObjectNum = cJSON_GetArraySize(JsonData);
		for(i = 0; i < iObjectNum; i ++)
		{
			ObjectArray = cJSON_GetArrayItem(JsonData, i);
			if(ObjectArray != NULL)
			{
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "out_refund_no");
				if (Jsonrst != NULL)
				{
					Uart_Printf("out_refund_no:%s\n", Jsonrst->valuestring);
				}

				Jsonrst = cJSON_GetObjectItem(ObjectArray, "refund_fee");
				if (Jsonrst != NULL)
				{
					Uart_Printf("refund_fee:%d\n", Jsonrst->valueint);
				}

				Jsonrst = cJSON_GetObjectItem(ObjectArray, "refund_result");
				if (Jsonrst != NULL)
				{
					Uart_Printf("refund_result:%s\n", Jsonrst->valuestring);
				}
				
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "refund_time");
				if (Jsonrst != NULL)
				{
					Uart_Printf("refund_time:%s\n", Jsonrst->valuestring);
				}
				
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "refund_type");
				if (Jsonrst != NULL)
				{
					Uart_Printf("refund_type:%s\n", Jsonrst->valuestring);
				}
				
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "set_time");
				if (Jsonrst != NULL)
				{
					Uart_Printf("set_time:%s\n", Jsonrst->valuestring);
				}
				
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "trade_no");
				if (Jsonrst != NULL)
				{
					Uart_Printf("trade_no:%s\n", Jsonrst->valuestring);
				}
				
				Jsonrst = cJSON_GetObjectItem(ObjectArray, "transaction_id");
				if (Jsonrst != NULL)
				{
					Uart_Printf("transaction_id:%s\n", Jsonrst->valuestring);
				}
				
			}
		}

    }

    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    return iRet;
}

/**
 * @Author: hhl \@decard
 * @fn     	int dc_ploymerize_return_goods_chack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate);
 * @brief:	德卡聚合支付退货查询接口
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 12, 2019  15:11:32
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 12, 2019  15:11:32
*/
int dc_ploymerize_return_goods_chack(const STRUCT_ONCE_SIMPLE *pterm_para, decard_ploymerize_pay *tradeDate)
{
	int iret = 0;
	cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;
	
	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
    uchar aucBuffer[128] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

	//清空缓存
	uiSendLen = 0;
	uiRecvLen = 0;
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));

	//数据组装
	iret = dc_ploymerize_return_goods_chack_pack(pterm_para, tradeDate, aucSendBuff);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_pack error : %d\r\n", iret);
		return iret;
	}
	uiSendLen = strlen(aucSendBuff);

	//MAC计算
	memset(aucBuffer, 0, sizeof(aucBuffer));
	dc_ploymerize_get_mac(pterm_para->mak, aucSendBuff, uiSendLen, aucBuffer);
	
	//数据发送接收
	memset(aucBuffer, 0, sizeof(aucBuffer));
	sprintf(aucBuffer, "http://%s:%d/pay/refund_query", pterm_para->serv_add, atoi(pterm_para->serv_port));
	iret = msg_http_send_rcve(aucBuffer, atoi(pterm_para->serv_port), aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
       return iret;
    }

	//数据解析
	iret = dc_ploymerize_return_goods_chack_unpack(aucRecvBuff, tradeDate);
	if(iret)
	{
		Uart_Printf("dc_ploymerize_pay_unpack error : %d\r\n", iret);
		return iret;
	}

	return iret;
}





