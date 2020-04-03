#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//#include "curl.h"
#include "cjson.h"
#include "sdkCurl.h"

#define  MAXLEN  100
#define  MAXSIZE 100

//extern int Parse_JsonData(unsigned char *JsonData, unsigned char *OutData, char dest[][MAXLEN]);

/*********************************************************************************************************
**Function:     static size_t RecvDataCb(const void *pBuffer, size_t size, size_t nmemb, void *userdata)
** Descriotions:   数据接收回调函数
** parameters:
** Returned value:
** Created By:
** Remarks:
*********************************************************************************************************/
static size_t RecvDataCb(const void *pBuffer, size_t size, size_t nmemb, void *userdata)
{
    SDK_CURL_PARAM *pstCurlParam = (SDK_CURL_PARAM*)userdata;
    unsigned int SegSize = size * nmemb;
    unsigned int MaxSize = pstCurlParam->uiRecvMaxBufSize;
    unsigned int cursor = pstCurlParam->uiRecvLen;
    bool bIsEnough = false;

    Uart_Printf("RecvDataCb: begin, segsize=%d, maxsize=%d, cursor=%d\r\n", SegSize, MaxSize, cursor);

    if (MaxSize < cursor + SegSize)     //buffer is NOT large enough
    {
        memcpy(pstCurlParam->pheRecvBuf + cursor, pBuffer, (MaxSize - cursor));
        pstCurlParam->uiRecvLen = MaxSize;
        *(pstCurlParam->pheRecvBuf + (MaxSize - 1)) = '\0';

        bIsEnough = true;
    }
    else  //buffer is large enough
    {
        memcpy(pstCurlParam->pheRecvBuf + cursor, pBuffer, SegSize);
        pstCurlParam->uiRecvLen += SegSize;
    }

    if (bIsEnough)
    {
        Uart_Printf("Receiving buffer is NOT large enough!!!\r\n");
        return MaxSize - cursor;    /*返回能保存的最大值*/
    }
    else
    {
        return SegSize;
    }
}


/***************************************************************************************************
**Function:     static void *do_curl(void *arg)
** Descriotions: CURL 参数配置执行
** parameters:
** Returned value:
** Created By:
** Remarks:
****************************************************************************************************/
static void *do_curl(void *arg)
{
    CURL *pCurl;
    CURLcode ret = CURLE_OK;
    OBJ_CURL*pStruct = (OBJ_CURL*)arg;
    SDK_CURL_PARAM *pstCurl = pStruct->pstCurlParam;
    unsigned int    uiUrlLen = (sizeof(pstCurl->asUrl) + pstCurl->uiSendMaxBufSize + 5) * sizeof(unsigned char);
    char *pasUrl = NULL;
    
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;

	char s_tag = 0;

	struct curl_slist *header = NULL;
    
//    Uart_Printf("-----* %s *-----\r\n", __func__);

    pasUrl = calloc(1, uiUrlLen);
    if (pasUrl == NULL)
    {
        Uart_Printf("pasUrl calloc return null!\r\n");
        return NULL;
    }

	memset(pasUrl, 0, uiUrlLen);
//    memset(pstCurl->pheRecvBuf, 0, pstCurl->uiRecvMaxBufSize);
    strcpy(pasUrl,  (const char *)pstCurl->asUrl);

    Uart_Printf("URL = %s\r\n", pstCurl->asUrl);
    Uart_Printf("port = %d\r\n", pstCurl->usPort);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    pCurl = curl_easy_init();

    if (pCurl == NULL)
    {
        Uart_Printf("curl_easy_init() return NULL!\r\n");
        pStruct->curl_flag = SDK_ERR;
    }
    else
    {
        //debug
        curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //打印请求连接过程和返回http数据

        //设置超时
        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT_MS, pstCurl->uiTimeout);

        //接收数据回调
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, RecvDataCb);
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)pstCurl);

        //设置http/https 请求模式
        if (SDK_CURL_REQ_POST == pstCurl->eReqMode)
        {
            curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
            if (pstCurl->pheSendBuf != NULL && pstCurl->uiSendLen > 0)
            {
                curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pstCurl->pheSendBuf);
                curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, pstCurl->uiSendLen);
                Uart_Printf(">>>>Senddata len=%d Send Data=%s\r\n", (int)pstCurl->uiSendLen, pstCurl->pheSendBuf);
            }
        }
        else if (SDK_CURL_REQ_GET == pstCurl->eReqMode)
        {
            curl_easy_setopt(pCurl, CURLOPT_HTTPGET, 1L);

            //当GET方式时，需要判断发送缓存是否为空，若是为空时，不附加内容到url中
            if ( pstCurl->pheSendBuf != NULL && pstCurl->uiSendLen != 0)
            {
                unsigned int  cursor = strlen((char *)pstCurl->asUrl);
                pasUrl[cursor++] = '?';

                if (pstCurl->uiSendLen > uiUrlLen - cursor)
                {
                    Uart_Printf("sdkcurl: size of data to send is TOO large!!!\r\n");
                    pStruct->curl_flag = SDK_ERR;

                    curl_easy_cleanup(pCurl);
                    curl_global_cleanup();
                    free(pasUrl);
                    pthread_detach(pthread_self());
                    Uart_Printf("curl perform failed!\r\n");

                    return NULL;
                }
                else
                {
                    memcpy(&pasUrl[cursor], pstCurl->pheSendBuf, pstCurl->uiSendLen);
					cursor += pstCurl->uiSendLen;
					pasUrl[cursor] = '\0';
                    Uart_Printf(">>>>Senddata len=%d Send Data=%s\r\n", (int)cursor, pasUrl);
                }
            }
        }

        if (pstCurl->eCurlProtocol == SDK_CURL_PROTO_HTTPS)
        {
            //服务器 CA验证
            if (pstCurl->bIsVerifyServer)
            {
				curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);

                if (curl_easy_setopt(pCurl, CURLOPT_CAINFO, pstCurl->asCAcertPath) != CURLE_OK)
                    Uart_Printf("!!!set capath failed!\r\n");
            }
            else
            {
                curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            }

            if (strlen((char *)pstCurl->asCliCertPath) > 0)  //客户端
            {
                curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 1L);

                if ( curl_easy_setopt(pCurl, CURLOPT_SSLCERT, pstCurl->asCliCertPath) != CURLE_OK )
                    Uart_Printf("!!!set client cert failed!\r\n");
                if ( curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD, pstCurl->asCliCertPwd) != CURLE_OK )
                    Uart_Printf("!!!set client cert password failed!\r\n");

                //curl默认为 pem 格式, 不用设置
                if(pstCurl->uiCertType == SDK_CURL_DER_TYPE)
                    ret = curl_easy_setopt(pCurl, CURLOPT_SSLCERTTYPE, "DER");
                else if (pstCurl->uiCertType == SDK_CURL_P12_TYPE)
                    ret = curl_easy_setopt(pCurl, CURLOPT_SSLCERTTYPE, "P12");

                if (ret != CURLE_OK)
                    Uart_Printf("!!!set client cert type failed!\r\n");
            }
            else
            {
                curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
            }
        }

		if(pstCurl->bIsGetProtelHead)
		{
			curl_easy_setopt(pCurl,CURLOPT_HEADER,1); //将响应头信息和相应体一起传给write_data
		}

		//修改协议头
        {
        	char TempBuf[64] = {0};
			char cMacData[64] = {0};
        	//char _pasUrl[]="http://58.213.149.86:6080/platform/ogsAccess.do?reqDataType=XML&resDataType=XML&force=Y&reqData=PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPFNlcnZpY2U%2BPFNlcnZpY2VfSGVhZGVyPjxUcmFuc05hbWU%2BU3dlcHRXeFBhaWVkPC9UcmFuc05hbWU%2BCjx0cmFuc2NvZGU%2BMTAwMjwvdHJhbnNjb2RlPgo8VHJhbnNhY3Rpb25UaW1lc3RhbXA%2BPC9UcmFuc2FjdGlvblRpbWVzdGFtcD4KPGNoYW5uZWxfaWQ%2BMDk8L2NoYW5uZWxfaWQ%2BCjxCcmFuY2hJZD48L0JyYW5jaElkPgo8cmVnaXN0ZXJfbm8%2BUGF5WVhfMjAxNjEyMDQwOTU4Mzg8L3JlZ2lzdGVyX25vPgo8dmVyc2lvbl9pZD4wMTwvdmVyc2lvbl9pZD4KPGFwcF9uYW1lPlBheVlYPC9hcHBfbmFtZT4KPHNlcnZpY2VfaWQ%2BMDUzNjAwMDAwMDAwMDE8L3NlcnZpY2VfaWQ%2BCjxzZXJ2aWNlX3NuPjA1MzYxNTIzMTcxMjAxNzA5MTQ8L3NlcnZpY2Vfc24%2BCjwvU2VydmljZV9IZWFkZXI%2BPFNlcnZpY2VfQm9keT48ZXh0X2F0dHJpYnV0ZXM%2BPENoYW5uZWxJZD4wMjwvQ2hhbm5lbElkPgo8Y2hhbm5lbGRhdGU%2BMjAxNzA5MTQ8L2NoYW5uZWxkYXRlPgo8Y2hhbm5lbHNlcT4yMDE3MDkxNDE1MjMxNzwvY2hhbm5lbHNlcT4KPEJyYW5jaElkPjMyMDEwMzAwMTwvQnJhbmNoSWQ%2BCjwvZXh0X2F0dHJpYnV0ZXM%2BPHJlcXVlc3Q%2BPFRyYW5zX0RhdGE%2BPE1lcmNoYW50SWQ%2BMzIwMjIzMDAxMUIwMDAxMjY3PC9NZXJjaGFudElkPgo8VHJhbnNDaGFubmVsSWQ%2BMDI8L1RyYW5zQ2hhbm5lbElkPgo8TWVyY2hhbnREYXRlVGltZT4yMDE4MDkxOTEwMTEyNDwvTWVyY2hhbnREYXRlVGltZT4KPE1lcmNoYW50U2VxTm8%2BMDAwMTgwPC9NZXJjaGFudFNlcU5vPgo8VGVybWluYWxJZD4wMDIxODA4MDI0NjY8L1Rlcm1pbmFsSWQ%2BCjxBdHRhY2g%2BPC9BdHRhY2g%2BCjxDdXJyZW5jeT5DTlk8L0N1cnJlbmN5Pgo8VHJhbnNBbW91bnQ%2BMTwvVHJhbnNBbW91bnQ%2BCjxCaWxsQ3JlYXRlaXA%2BNjYuOTcuOTMuMTwvQmlsbENyZWF0ZWlwPgo8R29vZHNUYWc%2BPC9Hb29kc1RhZz4KPE5vdGlmeVVybD48L05vdGlmeVVybD4KPFRyYWRlVHlwZT48L1RyYWRlVHlwZT4KPExpbWl0UGF5PjwvTGltaXRQYXk%2BCjxBdXRoQ29kZT4xMzQ2Mzg0ODUyMDgxMTM1NTQ8L0F1dGhDb2RlPgo8SU1HX0RhdGE%2BPC9JTUdfRGF0YT4KPFJlbWFyaz48L1JlbWFyaz4KPFJlbWFyazE%2BPC9SZW1hcmsxPgo8UmVtYXJrMj48L1JlbWFyazI%2BCjxSZW1hcmszPjwvUmVtYXJrMz4KPFJlbWFyazQ%2BPC9SZW1hcms0Pgo8UmVtYXJrNT48L1JlbWFyazU%2BCjxHb29kc0JvZHk%2BPC9Hb29kc0JvZHk%2BCjxHb29kc0RldGFpbD48L0dvb2RzRGV0YWlsPgo8L1RyYW5zX0RhdGE%2BPC9yZXF1ZXN0PjwvU2VydmljZV9Cb2R5PjwvU2VydmljZT4=";
/*			char _pasUrl[]="http://58.213.149.86:6080/platform/ogsAccess.do?reqDataType=XML&resDataType=XML&force=Y&reqData=PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPFNlcnZpY2U%2BPFNlcnZpY2VfSGVhZGVyPjxUcmFuc05hbWU%2BU3dlcHRXeFBhaWVkPC9UcmFuc05hbWU%2BCjx0cmFuc2NvZGU%2BMTAwMjwvdHJhbnNjb2RlPgo8VHJhbnNhY3Rpb25UaW1lc3RhbXA%2BPC9UcmFuc2FjdGlvblRpbWVzdGFtcD4KPGNoYW5uZWxfaWQ%2BMDk8L2NoYW5uZWxfaWQ%2BCjxCcmFuY2hJZD48L0JyYW5jaElkPgo8cmVnaXN0ZXJfbm8%2BUGF5WVhfMjAxNjEyMDQwOTU4Mzg8L3JlZ2lzdGVyX25vPgo8dmVyc2lvbl9pZD4wMTwvdmVyc2lvbl9pZD4KPGFwcF9uYW1lPlBheVlYPC9hcHBfbmFtZT4KPHNlcnZpY2VfaWQ%2BMDUzNjAwMDAwMDAwMDE8L3NlcnZpY2VfaWQ%2BCjxzZXJ2aWNlX3NuPjAxODA5MjAxNjMzMzAwMDAzMjQ8L3NlcnZpY2Vfc24%2BCjwvU2VydmljZV9IZWFkZXI%2BPFNlcnZpY2VfQm9keT48ZXh0X2F0dHJpYnV0ZXM%2BPENoYW5uZWxJZD4wMjwvQ2hhbm5lbElkPgo8Y2hhbm5lbGRhdGU%2BMjAxODA5MjA8L2NoYW5uZWxkYXRlPgo8Y2hhbm5lbHNlcT4yMDE4MDkyMDE2MzMzMDwvY2hhbm5lbHNlcT4KPEJyYW5jaElkPjMyMDEwMzAwMTwvQnJhbmNoSWQ%2BCjwvZXh0X2F0dHJpYnV0ZXM%2BPHJlcXVlc3Q%2BPFRyYW5zX0RhdGE%2BPE1lcmNoYW50SWQ%2BMzIwMjIzMDAxMUIwMDAxMjY3PC9NZXJjaGFudElkPgo8VHJhbnNDaGFubmVsSWQ%2BMDI8L1RyYW5zQ2hhbm5lbElkPgo8TWVyY2hhbnREYXRlVGltZT4yMDE4MDkyMDE2MzMzMDwvTWVyY2hhbnREYXRlVGltZT4KPE1lcmNoYW50U2VxTm8%2BMDAwMzI0PC9NZXJjaGFudFNlcU5vPgo8VGVybWluYWxJZD4wMDIxODA4MDI0NjY8L1Rlcm1pbmFsSWQ%2BCjxBdHRhY2g%2BPC9BdHRhY2g%2BCjxDdXJyZW5jeT5DTlk8L0N1cnJlbmN5Pgo8VHJhbnNBbW91bnQ%2BMTwvVHJhbnNBbW91bnQ%2BCjxCaWxsQ3JlYXRlaXA%2BNjYuOTcuOTMuMTwvQmlsbENyZWF0ZWlwPgo8R29vZHNUYWc%2BPC9Hb29kc1RhZz4KPE5vdGlmeVVybD48L05vdGlmeVVybD4KPFRyYWRlVHlwZT48L1RyYWRlVHlwZT4KPExpbWl0UGF5PjwvTGltaXRQYXk%2BCjxBdXRoQ29kZT4xMzQ2MTE2NzgzMjY3NDExNjY8L0F1dGhDb2RlPgo8SU1HX0RhdGE%2BPC9JTUdfRGF0YT4KPFJlbWFyaz48L1JlbWFyaz4KPFJlbWFyazE%2BPC9SZW1hcmsxPgo8UmVtYXJrMj48L1JlbWFyazI%2BCjxSZW1hcmszPjwvUmVtYXJrMz4KPFJlbWFyazQ%2BPC9SZW1hcms0Pgo8UmVtYXJrNT48L1JlbWFyazU%2BCjxHb29kc0JvZHk%2BPC9Hb29kc0JvZHk%2BCjxHb29kc0RldGFpbD48L0dvb2RzRGV0YWlsPgo8L1RyYW5zX0RhdGE%2BPC9yZXF1ZXN0PjwvU2VydmljZV9Cb2R5PjwvU2VydmljZT4=";
			int _port = 6080;

			curl_easy_setopt(pCurl, CURLOPT_URL, _pasUrl);
       	 	curl_easy_setopt(pCurl, CURLOPT_PORT,_port);
*/

//*			header = curl_slist_append(header, "batchNo:");

			memset(TempBuf, 0, sizeof(TempBuf));
			///sprintf(TempBuf, "signature: %s", pstCurl->pheSendMac); 
//          strcpy(TempBuf, "Content-Type: application/json");
            strcpy(TempBuf, "Content-Type: application/json");
  			header = curl_slist_append(header, TempBuf);
	        curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, header);

			
			memset(TempBuf, 0, sizeof(TempBuf));
			memset(cMacData, 0, sizeof(cMacData));
			sprintf(TempBuf, "mac: %s", dc_ploymerize_return_msg_mac(cMacData));
			header = curl_slist_append(header, TempBuf);
			curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, header);
//*/
		}

		{
            //表单请求
            unsigned char aucTempBuff[1024] = {0};
            unsigned char aucSendData[4*1024] = {0};
            char dest[MAXSIZE][MAXLEN];

			
            memcpy(aucSendData, pstCurl->pheSendBuf, pstCurl->uiSendLen);
            
            //Parse_JsonData(aucSendData, aucTempBuff, dest);
            
//        	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "posid", CURLFORM_COPYCONTENTS, /*"002180500020"*/" ", CURLFORM_END);
//        	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "version", CURLFORM_COPYCONTENTS, /*"04000000000000"*/" ", CURLFORM_END);

            curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, formpost);
            
        	// 设置参数，比如"ParamName1=ParamName1Content&ParamName2=ParamName2Content&..."
//        	memset(aucTempBuff, 0, sizeof(aucTempBuff));
//        	strcpy(aucTempBuff, "posid=002180500020&version=04000000000000");
            Uart_Printf("aucTempBuff: %s\n", aucSendData);
        	curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, aucSendData);

        }
        
        curl_easy_setopt(pCurl, CURLOPT_URL, pasUrl);
       	curl_easy_setopt(pCurl, CURLOPT_PORT, pstCurl->usPort);
SED_AG:
        ret = curl_easy_perform(pCurl);
        if (ret == CURLE_OK)
            pStruct->curl_flag = SDK_OK;
        else if (ret == CURLE_OPERATION_TIMEDOUT)
            pStruct->curl_flag = SDK_TIME_OUT;
        else
            pStruct->curl_flag = SDK_ERR;

        if (ret != CURLE_OK)
        {
            Uart_Printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
			if(!s_tag)
			{
				s_tag = 1;
				//goto SED_AG;
			}
        }

        pStruct->eRetCode = ret;
        curl_easy_cleanup(pCurl);
		if(header)
		{
			curl_slist_free_all(header); /* free the list again */
		}
    }

    Uart_Printf("curl cleanup!\r\n");

    curl_global_cleanup();

    free(pasUrl);

    Uart_Printf("curl perform exit!\r\n");
    
    return NULL;
    
}

/*****************************************************************************
**Function:     static int sdk_ext_curl(SDK_CURL_PARAM *pstCurlParam)
** Descriptions:
** Parameters:          SDK_CURL_PARAM *pstCurlParam
** Returned value:
** Created By:
** Remarks:
*****************************************************************************/
static int sdk_ext_curl(SDK_CURL_PARAM *pstCurlParam)
{
    OBJ_CURL stCurl = {pstCurlParam, CURLE_OK, SDK_TIME_OUT};

//    Uart_Printf("-----* %s *-----\r\n", __func__);

    if (pstCurlParam == NULL ||
        pstCurlParam->eCurlProtocol > SDK_CURL_PROTO_HTTPS || pstCurlParam->eReqMode > SDK_CURL_REQ_GET ||
        pstCurlParam->pheRecvBuf == NULL ||
        (pstCurlParam->eCurlProtocol == SDK_CURL_PROTO_HTTPS &&
         pstCurlParam->bIsVerifyServer == true && strlen((char *)pstCurlParam->asCAcertPath) == 0))
    {
        Uart_Printf("sdk_ext_curl param error\n");
        Uart_Printf("eCurlProtocol=%d, bIsVerifyServer=%d, asCAcertPath=%s\n",
            pstCurlParam->eCurlProtocol, pstCurlParam->bIsVerifyServer, pstCurlParam->asCAcertPath);
        return SDK_PARA_ERR;
    }

    do_curl((void *)&stCurl);
    return stCurl.curl_flag;
}

/*****************************************************************************
**Function:     int sdkExtCurl(SDK_EXT_CURL_DATA *pstData, bool bForceSSL)
** Descriptions:	通过http/https协议获取数据
** Parameters:  SDK_EXT_CURL_DATA *pstData      详见结构体说明
                        bool bForceSSL:   1.无CA证书的https连接   0.其他
** Returned value:	SDK_OK-成功；SDK_ERR-失败；SDK_TIME_OUT-超时；SDK_PARA_ERR-参数错误
** Created By:
** Remarks:
*****************************************************************************/
int sdkExtCurl(SDK_EXT_CURL_DATA *pstData, bool bForceSSL)
{
	int iret = 0;
    SDK_CURL_PARAM param;
    
//    Uart_Printf("-----* %s *-----\r\n", __func__);

    if(pstData == NULL ||
       pstData->psURL == NULL || strlen((char *)pstData->psURL) > URL_MAX_LEN ||
       pstData->psRecv == NULL ||
       pstData->nSendLen > SDK_EXT_CURL_BUF_SIZE)
    {
        Uart_Printf("sdkExtCurl param error\n");
        return SDK_PARA_ERR;
    }

    memset(&param, 0, sizeof(SDK_CURL_PARAM));
       
    if (pstData->bMethod)
    {
        param.eReqMode = SDK_CURL_REQ_GET;
    }
    else
    {
        param.eReqMode = SDK_CURL_REQ_POST;
    }

    if (bForceSSL )
    {
        param.eCurlProtocol = SDK_CURL_PROTO_HTTPS;
        if (pstData->psCaFile && strlen((char *)pstData->psCaFile) > 0)
        {
            param.bIsVerifyServer = 1;
            if (strlen((char *)pstData->psCaFile) >= CA_PATH_MAX_LEN)
            {
                Uart_Printf("sdkExtCurl Cafile error\n");
                return SDK_PARA_ERR;
            }
            strcpy((char *)param.asCAcertPath, (const char *)pstData->psCaFile);

            if (pstData->psCliCert)
            {
                strcpy((char *)param.asCliCertPath, (const char *)pstData->psCliCert);
                if (pstData->psCliPwd)
                    strcpy((char *)param.asCliCertPwd, (const char *)pstData->psCliPwd);
                param.uiCertType = (unsigned short)pstData->nCertType;
            }
        }
    }
    else
    {
        if (pstData->psCaFile)
        {
            param.eCurlProtocol = SDK_CURL_PROTO_HTTPS;
            param.bIsVerifyServer = 1;
            if (strlen((char *)pstData->psCaFile) >= CA_PATH_MAX_LEN)
            {
                Uart_Printf("sdkExtCurl Cafile error\n");
                return SDK_PARA_ERR;
            }
            strcpy((char *)param.asCAcertPath, (const char *)pstData->psCaFile);

            if (pstData->psCliCert)
            {
                strcpy((char *)param.asCliCertPath, (const char *)pstData->psCliCert);
                if (pstData->psCliPwd)
                    strcpy((char *)param.asCliCertPwd, (const char *)pstData->psCliPwd);
                param.uiCertType = (unsigned short)pstData->nCertType;
            }
        }
        else
        {
            param.eCurlProtocol = SDK_CURL_PROTO_HTTP;
            param.bIsVerifyServer = 0;
        }
    }

    strcpy((char *)param.asUrl, (const char *)pstData->psURL);  //url

    param.usPort = pstData->nPort;       //端口号
    param.pheSendBuf = pstData->psSend;                //发送数据
    param.uiSendLen = pstData->nSendLen;               //发送数据长度
    param.uiSendMaxBufSize = pstData->nSendLen;
    param.pheSendMac = pstData->psSendMac;			     //发送数据MAC

    param.pheRecvBuf = pstData->psRecv;                //接收数据缓存
    param.uiRecvLen = 0;
    param.uiRecvMaxBufSize = pstData->nRecvMaxLen;

    param.uiTimeout = pstData->nTimeout;               //超时时间
	param.bIsGetProtelHead = pstData->bIsGetProtelHead;

	iret = sdk_ext_curl(&param);
	if(SDK_OK == iret)
	{
		pstData->nRvcLen = param.uiRecvLen;
	}
    return iret;

}




