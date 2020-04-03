/*
 * trans_msg.c
 *	许昌公交二级平台通讯报文.c
 *  Created on: November 05, 2019  10:07:09
 *      Author: huhongliang@decard.com
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "trans_msg.h"
#include "data_type.h"
#include "cjson.h"
#include "sdkCurl.h"
#include "main_api.h"
#include "config.h"


#define  MAXLEN  100
#define  MAXSIZE 100

/**
 * @Author: hhl \@decard
 * @fn     	int msg_get_http_header(const STRUCT_ONCE_SIMPLE *pterm_para, uchar ucType, uchar *ParaUrl, uchar *pucURL);
 * @brief:	获取HTTP协议的头
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:01:08
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:01:08
*/
int msg_get_http_header(const STRUCT_ONCE_SIMPLE *pterm_para, uchar ucType, uchar *ParaUrl, uchar *pucURL)
{
    uchar aucHost[512] = {0};

	///<[yufupeng|2019年6月28日 15:51:45]|  永城的二级平台的地址
    if(memcmp(pterm_para->szCompanyNumb, "476600", 6) == 0)//永城县区
    {
        if(ucType == 1)//消费
        {
            strcpy(aucHost, "123.13.221.160:18085/produce");
        }
        else//参数
        {
            strcpy(aucHost, "123.13.221.160:18086/pos");
        }
    }
    else
    {    
        /* 
            平顶山，宝丰和鄢陵二级平台的地址
            接口测试环境  地址：
            消费数据上传       http://123.13.201.82:18085/produce/produce
            其他接口         http://123.13.201.82:18086/pos
        */
        
        if(ucType == 1)//消费
        {
            strcpy(aucHost, HOSTDEAL);
        }
        else//参数
        {
            strcpy(aucHost, HOSTPARA);
        }
    }
    sprintf(pucURL, "http://%s/%s", aucHost, ParaUrl);
    Uart_Printf("pucURL:[%s]\r\n", pucURL);

    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:04:43
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:04:43
*/
int msg_http_send_rcve(uchar *pURL, ushort port, uchar *pSend, uint Sendlen, uchar *pRev, uint *RcvLen)
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

  
void SqrtString(char title[][MAXLEN],int n) 
{//排序
    int i,j,k;
    char tstr[MAXLEN];

    for(i = 0; i < n - 1; ++i) 
    {
        k = i;

        for(j = i + 1; j < n; ++j) 
        {
            if(strcmp(title[k],title[j]) > 0)
                k = j;
        }
        
        if(k != i) 
        {
            strcpy(tstr,title[k]);
            strcpy(title[k],title[i]);
            strcpy(title[i],tstr);
        }
    }
}


void showString(char s[][MAXLEN],int n) 
{
    int i;
    for(i = 0; i < n; ++i)
        Uart_Printf("%s ",s[i]);
    
    Uart_Printf("\r\n");
}


cJSON* cJSON_ErgodicObjectItem ( cJSON* object, char chItem[][100])
{
	cJSON* c=object->child;
    int i = 0;
    
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
	while ( c)
	{
    	if ( NULL != c )
    	{
            switch ( ( c->type ) & 0XFF )
            {
                case cJSON_NULL:
                    Uart_Printf ( "VALUE-NULL:(%s)(%s)\r\n", c->string, "null" );
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_False:
                    Uart_Printf ( "VALUE-False:(%s)(%s)\r\n", c->string, "false" );
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_True:
                    Uart_Printf ( "VALUE-True:(%s)(%s)\r\n", c->string, "true" );
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_Number:
                    Uart_Printf ( "VALUE-Number:(%s)(%d)\r\n", c->string, c->valueint );
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_String:
                    //Uart_Printf ( "VALUE-String:(%s)(%s)(%d)\r\n", c->string, c->valuestring, c->strLen );
                    Uart_Printf ( "VALUE-String:(%s)(%s)\r\n", c->string, c->valuestring);
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_Array:
                    Uart_Printf ( "VALUE-Array:(%s)(%s)\r\n", c->string,  cJSON_PrintUnformatted(c) );
                    strcpy(chItem[i], c->string);
                    break;
                case cJSON_Object:
                    Uart_Printf ( "VALUE-Object:(%s)(%s)\r\n", c->string, cJSON_PrintUnformatted(c) );
                    strcpy(chItem[i], c->string);
                    break;
                default:
                    Uart_Printf ( "default c->type=%d\r\n", c->type );
                    break;
            }

            i++;
            
    	}
        else
        {
            Uart_Printf ( "NULL == c \r\n");
        }
        
		c=c->next;
	}
    
	return c;
}


/**********************************************************************************************//**
*  @fn       int Parse_SendData(uchar *JsonData, uchar *OutData, char dest[][MAXLEN]);
*  @brief    遍历JSON数据,且把key按字典序排序
*  
*  @author   yufupeng \@decard
*  @date     2019年4月23日 14:05:08
*  @param   [in] 
*  @param   [out] 
*  @param   [io] 
*  @return  -
*            -
*  @note    (描述需要注意的问题) 
*            - 1.
*            - 2.
*  @bug    (bug修复的描述问题) 
*            - 1.
*            - 2.
***************************************************************************************************/
int Parse_JsonData(unsigned char *JsonData, unsigned char *OutData, char dest[][MAXLEN])
{
    int iRet = -1;
    cJSON *cjRoot = NULL;
    uchar aucBuff[4*1024] = {0};
    uchar aucTempBuff[256] = {0};
    cJSON *CjData = NULL;
    int i = 0, j = 0;
    char srcItem[MAXSIZE][MAXLEN];
    char src[MAXSIZE][MAXLEN];
    char srcTemp[MAXSIZE][MAXLEN];
    char Value[MAXSIZE][1024];
    int index = 0, iOffset = 0;
    
    if (!JsonData)
        return 1;
    
    if (!OutData)
        return 1;
    
    for(i = 0; i < MAXSIZE; i++) 
    {
        memset(srcItem[i], 0, MAXLEN);
        memset(src[i], 0, MAXLEN);
        memset(srcTemp[i], 0, MAXLEN);
        memset(Value[i], 0, MAXLEN);
    }
        
    Uart_Printf("-----* %s *-----\r\n", __func__);

    Uart_Printf("JsonData:%s\r\n", JsonData);
    
    cjRoot = cJSON_Parse(JsonData);
    if(!cjRoot)
    {
        Uart_Printf("Error Parse: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }

    iRet = 0;
    i=0;

    //遍历JSON中的KEY
    CjData = cJSON_ErgodicObjectItem(cjRoot, srcItem);
    
    while (strlen(srcItem[i]) > 0) 
    {
        Uart_Printf("srcItem[%d]:%s\r\n", i, srcItem[i]);
    
        CjData = cJSON_GetObjectItem(cjRoot, srcItem[i]);
        if (CjData != NULL)
        {
            switch ( ( CjData->type ) & 0XFF )
            {
                case cJSON_NULL:
                    Uart_Printf ( "(%s)(%s)\r\n", CjData->string, "null" );
                    break;
                case cJSON_False:
                    Uart_Printf ( "(%s)(%s)\r\n", CjData->string, "false" );
                    break;
                case cJSON_True:
                    Uart_Printf ( "(%s)(%s)\r\n", CjData->string, "true" );
                    break;
                case cJSON_Number:                
                    {
                        Uart_Printf("%s : %d\r\n", srcItem[i], CjData->valueint);
                        sprintf(Value[index], "%d", CjData->valueint);
                        strcpy(src[index++], srcItem[i]);
                    }            
                    break;
                case cJSON_String:
                    if (CjData->valuestring)
                    {
                        Uart_Printf("%s : %s\r\n", srcItem[i], CjData->valuestring);
                        strcpy(Value[index], CjData->valuestring);
                        strcpy(src[index++], srcItem[i]);
                    }            
                    break;
                case cJSON_Array:
                    {
                        Uart_Printf("%s : %s\r\n", srcItem[i], cJSON_PrintUnformatted(CjData));
                        strcpy(Value[index], cJSON_PrintUnformatted(CjData));
                        strcpy(src[index++], srcItem[i]);
                    }
                    break;
                case cJSON_Object:
                    {
                        Uart_Printf("%s : %s\r\n", srcItem[i], cJSON_PrintUnformatted(CjData));
                        strcpy(Value[index], cJSON_PrintUnformatted(CjData));                        
                        strcpy(src[index++], srcItem[i]);
                    }            
                    break;
            }
        }
        else
        {
            Uart_Printf("Error #### %s not found\r\n", srcItem[i]);
            iRet = 1;
        }
        
        i++;   
    
    }
        
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }
    
    for (i = 0; i < index; i++) 
    {
        strcpy(srcTemp[i], src[i]);
    }

    Uart_Printf("排序前:\r\n");
    showString(src,index);
    SqrtString(src, index);
    Uart_Printf("排序后:\r\n");
    showString(src,index);
    
    for(i = 0; i < index; i++) 
    {
        strcpy(dest[i], src[i]);
    }

    iOffset = 0;
    for(i = 0; i < index; ++i) 
    {
        for (j=0; j<index; j++)
        {
            if (strcmp(src[i], srcTemp[j]) == 0)
            {
                strcpy(aucBuff+iOffset, src[i]);
                iOffset += strlen(src[i]);

                strcpy(aucBuff+iOffset, "=");
                iOffset += 1;
                
                strcpy(aucBuff+iOffset, Value[j]);
                iOffset += strlen(Value[j]);

                if (i < (index-1))

                {
                    strcat(aucBuff, "&");
                    iOffset += 1;
                }                
            }
        }
    }
    
    Uart_Printf("aucBuff:%s\n", aucBuff);

    strcpy(OutData, aucBuff);
    
    return iRet;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_up_status_pack(const STRUCT_ONCE_SIMPLE *pterm_para, st_TEMP_VERSION_CTRL *pterm_para_current_ver, uchar *oJson);
 * @brief:	状态上送报文组装
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  16:04:56
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  16:04:56
*/
static int msg_up_status_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_current_ver, uchar *oJson)
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

    cJSON_AddStringToObject(cjRoot, "posId", pterm_para->szTermID);
    cJSON_AddStringToObject(cjRoot, "black", pterm_para_current_ver->NewBlacklistVer);
    cJSON_AddStringToObject(cjRoot, "white", pterm_para_current_ver->NewWhiteListVer);
    cJSON_AddStringToObject(cjRoot, "program", "0001");
    cJSON_AddStringToObject(cjRoot, "rcaVersion", "00000000000000");
    
	    Data = cJSON_CreateObject();
	    if (NULL == Data)
	    {
	        Uart_Printf("create Data error\r\n");
	        if(cjRoot)
	        {
	            cJSON_Delete(cjRoot);
	        }
	        return 1;
	    }
	    cJSON_AddStringToObject(Data, "sim_imsi", pterm_para->szSIM_IMSI);
	    cJSON_AddStringToObject(Data, "busNo", pterm_para->aucPSAMID);//车辆自编号使用的是PSAM终端号
	    cJSON_AddStringToObject(Data, "x_version", " ");
	    cJSON_AddStringToObject(Data, "black", "00000000000000");
	    cJSON_AddStringToObject(Data, "devNumber", "11111111");
	    cJSON_AddStringToObject(Data, "driver", "00000000");
	    cJSON_AddStringToObject(Data, "white", "00000000000000");
	    cJSON_AddStringToObject(Data, "psamtricardon", "000000000000");
	    cJSON_AddStringToObject(Data, "lineCardNO", pterm_para->szCarLineID);
	    cJSON_AddStringToObject(Data, "binVersion", "00");
	    cJSON_AddStringToObject(Data, "route", pterm_para->szCarLineID);
	    cJSON_AddStringToObject(Data, "sim_serial", pterm_para->szSIM_CCID);
	    cJSON_AddStringToObject(Data, "posId", aucDevSN);
	    cJSON_AddStringToObject(Data, "pos_imei", pterm_para->szSIM_IMSI);
	    cJSON_AddStringToObject(Data, "dept", pterm_para->szCompanyNumb);
	    cJSON_AddStringToObject(Data, "program", "0001");
	    cJSON_AddStringToObject(Data, "busCardNo", "00010001");
	    cJSON_AddStringToObject(Data, "psamurdcardon", pterm_para->aucPSAMID);

    cJSON_AddItemToObject(cjRoot, "data", Data);


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
 * @fn     	int msg_up_status_unpack(uchar *Msg, st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	状态上送报文解包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:10:10
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:10:10
*/
static int msg_up_status_unpack(uchar *Msg, st_TEMP_VERSION_CTRL *pterm_para_ver)
{
    int iRet = -1;
	char *cjsonstr = NULL;
    cJSON *cjRoot = NULL;
    uchar aucBuff[512] = {0};
    cJSON *JsonData = NULL;
    uchar aucCode[128] = {0};
    uchar aucVersion[64] = {0};
    uchar aucProgversion[64] = {0};
    uchar aucWhite[64] = {0};
    uchar aucBinversion[64] = {0};
    uchar aucBin[64] = {0};
    uchar aucBlack[64] = {0};
    uchar aucProgram[64] = {0};    
    uchar aucServerTime[20] = {0};
    uchar aucRcaVersion[64] = {0};
    
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
    
    //{"progversion":"00","code":"01","white":"20171122","binversion":"0","bin":"0","black":"20190401","program":"0001"} 

    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("code                : %s\n", JsonData->valuestring);
            strcpy(aucCode, JsonData->valuestring);
			memcpy(pterm_para_ver->code, JsonData->valuestring, 2);
        }
    }
    else
    {
        Uart_Printf("Error #### code not found\r\n");
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "progversion");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("progversion: %s\n", JsonData->valuestring);
            strcpy(aucProgversion, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### progversion not found\r\n");
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "white");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("white: %s\n", JsonData->valuestring);
			//memset(aucWhite, 0, sizeof(aucWhite));
            //strcpy(aucWhite, JsonData->valuestring);
            memcpy(pterm_para_ver->NewWhiteListVer, JsonData->valuestring, strlen(JsonData->valuestring) >= 14 ? 14 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### white not found\r\n");
    }

    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "binversion");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("binversion: %s\n", JsonData->valuestring);
            strcpy(aucProgversion, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### binversion not found\r\n");
    }
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "bin");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("bin: %s\n", JsonData->valuestring);
            strcpy(aucBin, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### bin not found\r\n");
    }
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "black");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("black: %s\n", JsonData->valuestring);
			//memset(aucBlack, 0, sizeof(aucBlack));
            //strcpy(aucBlack, JsonData->valuestring);            
			memcpy(pterm_para_ver->NewBlacklistVer, JsonData->valuestring, strlen(JsonData->valuestring) >= 14 ? 14 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### black not found\r\n");
    }

        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "program");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("program: %s\n", JsonData->valuestring);
            strcpy(aucProgram, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### program not found\r\n");
    }
    
    JsonData = cJSON_GetObjectItem(cjRoot, "serverTime");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("serverTime: %s\n", JsonData->valuestring);
            memcpy(aucServerTime, JsonData->valuestring, 14);

            //同步时间
            //Sys_SetDateTime(aucServerTime);
        }
    }
    else
    {
        Uart_Printf("Error #### serverTime not found\r\n");
    }

	///<[yufupeng|2019年7月4日 13:54:03]|  交通部二维码根证书版本
    JsonData = cJSON_GetObjectItem(cjRoot, "rcaVersion");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("rcaVersion: %s\n", JsonData->valuestring);
            strcpy(aucRcaVersion, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### rcaVersion not found\r\n");
    }
    

    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

#if(0)
	//是否下载参数解析
    if (memcmp(aucCode, "01", 2) == 0)
    {
        if ((strlen(aucWhite) > 0) && (memcmp(aucWhite, "00", 2) != 0))
        {
            /*memset(aucVersion, 0, sizeof(aucVersion));
            sprintf(aucVersion, "24%s0001", aucWhite);
            aucVersion[14] = 0x00;*/
            Uart_Printf("Sever White Ver : %s\r\n", aucWhite);
            
            //白名单需要升级
            //RW_TermServerParaVer(RW_WRITE, 24, aucVersion);
            memcpy(pterm_para_ver->NewWhiteListVer, aucWhite, strlen(aucWhite) >= 14 ? 14 : strlen(aucWhite));
        }
        
        if ((strlen(aucBlack) > 0) && (memcmp(aucBlack, "00", 2) != 0))
        {
        	/*
            memset(aucVersion, 0, sizeof(aucVersion));
            sprintf(aucVersion, "02%s0001", aucBlack);
            aucVersion[14] = 0x00;*/
            Uart_Printf("Sever Black Ver : %s\r\n", aucBlack);
            
            //黑名单需要升级
            //RW_TermServerParaVer(RW_WRITE, 2, aucVersion);
			//memcpy(pterm_para_ver->NewBlacklistVer, aucVersion, 14);
			memcpy(pterm_para_ver->NewBlacklistVer, aucBlack, strlen(aucBlack) >= 14 ? 14 : strlen(aucBlack));
        }
        
        if ((strlen(aucRcaVersion) > 0) && (memcmp(aucRcaVersion, "00", 2) != 0))
        {
            memset(aucVersion, 0, sizeof(aucVersion));
            sprintf(aucVersion, "%s", aucRcaVersion);
            aucVersion[14] = 0x00;
            Uart_Printf("Sever MotQR Cert Ver : %s\r\n", aucVersion);
            
            //交通部二维码公钥证书需要升级
            //RW_TermServerParaVer(RW_WRITE, 34, aucVersion);
            //memcpy(pterm_para_ver->NewQrCertVer, aucVersion, 14);
			memcpy(pterm_para_ver->NewQrCertVer, aucRcaVersion, strlen(aucRcaVersion) >= 14 ? 14 : strlen(aucRcaVersion));
        }
    }
#endif

    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_up_status(const STRUCT_ONCE_SIMPLE *pterm_para, st_TEMP_VERSION_CTRL *pterm_para_current_ver, st_TEMP_VERSION_CTRL *pterm_para_serv_ver);
 * @brief:	状态上送
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 12, 2019  15:57:49
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 12, 2019  15:57:49
*/
int msg_up_status(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_current_ver, st_TEMP_VERSION_CTRL *pterm_para_serv_ver)
{
	int iret = 0;
	char cjsStr[1024] = {0};
	char aucURL[512] = {0};

	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
	
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
	
	//报文组装
	memset(cjsStr, 0, sizeof(cjsStr));
	iret = msg_up_status_pack(pterm_para, pterm_para_current_ver, cjsStr);
	if(iret)
	{
		Uart_Printf("msg_up_status_pack Err : %d\n", iret);
		return iret;
	}

	//地址获取
	memset(aucURL, 0, sizeof(aucURL));
    iret = msg_get_http_header(pterm_para, 0, "baseinfo", aucURL);
    if (0 != iret)
    {
		Uart_Printf("msg_get_http_header Err : %d\n", iret);
        return 1;
    }

	//报文发送
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	uiSendLen = 0;

	memcpy(aucSendBuff, cjsStr, strlen(cjsStr));
	uiSendLen = strlen(cjsStr);
	
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));
	uiRecvLen = 0;
    iret = msg_http_send_rcve(aucURL, PORTPARA, aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
    	

		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
        return iret;
    }

	//报文解析
	iret = msg_up_status_unpack(aucRecvBuff, pterm_para_serv_ver);
	if(iret)
	{
		Uart_Printf("msg_up_status_pack_unpack Err : %d\n", iret);
		return iret;
	}

	return iret;
}



/**
 * @Author: hhl \@decard
 * @fn     	int msg_Sftp_download_file(FTP_INFOT_T *pftpInfor);
 * @brief:	//sftp下载 文件
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 12, 2019  11:35:21
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 12, 2019  11:35:21
*/
int msg_Sftp_download_file(FTP_INFOT_T *pftpInfor)
{
	int iRet = 1; 
    int  SFTPServPort = 0;           //ftp服务端端口
    uchar SFTPServIp[512] = {0};    //ftp服务端IP
    uchar SFTPServPath[512] = {0};  //ftp服务端路径
    uchar SFTPFileName[512] = {0};  //ftp服务端文件名
    uchar SFTPUsrNname[512] = {0};  //ftp用户名
    uchar SFTPUsrPass[512] = {0};   //ftp密码
    uchar LocalPathFile[512] = {0}; //本地存放路径

	Uart_Printf("pftpInfor.FTPServIP:%s\r\n", pftpInfor->FTPServIP);
    Uart_Printf("pftpInfor.FTPServPort:%s\r\n", pftpInfor->FTPServPort);
    Uart_Printf("pftpInfor.FTPServPath:%s\r\n", pftpInfor->FTPServPath);
    Uart_Printf("pftpInfor.FTPServUser:%s\r\n", pftpInfor->FTPServUser);
	Uart_Printf("pftpInfor.FTPServPassword:%s\r\n", pftpInfor->FTPServPassword);
    Uart_Printf("pftpInfor.FTPFileName:%s\r\n", pftpInfor->FTPFileName);

    Uart_Printf("pftpInfor.FTPLocalPath:%s\r\n", pftpInfor->FTPLocalPath);
    Uart_Printf("pftpInfor.FTPLocalFileName:%s\r\n", pftpInfor->FTPLocalFileName);

	memset(SFTPServIp, 0, sizeof(SFTPServIp));
    strcpy(SFTPServIp, pftpInfor->FTPServIP);
    SFTPServPort = atoi(pftpInfor->FTPServPort);

	memset(SFTPUsrNname, 0, sizeof(SFTPUsrNname));
    strcpy(SFTPUsrNname, pftpInfor->FTPServUser);

	memset(SFTPUsrPass, 0, sizeof(SFTPUsrPass));
    strcpy(SFTPUsrPass, pftpInfor->FTPServPassword);
	
	memset(SFTPServPath, 0, sizeof(SFTPServPath));
    strcpy(SFTPServPath, pftpInfor->FTPServPath);
	
	memset(SFTPFileName, 0, sizeof(SFTPFileName));
    strcpy(SFTPFileName, pftpInfor->FTPFileName);

	memset(LocalPathFile, 0, sizeof(LocalPathFile));
    sprintf(LocalPathFile, "%s/%s", pftpInfor->FTPLocalPath, pftpInfor->FTPLocalFileName);

    iRet = sftp_client_libssh2(SFTPServIp, SFTPServPort, SFTPUsrNname, SFTPUsrPass, SFTPServPath, SFTPFileName, LocalPathFile);
    if(!iRet)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    
	return iRet;
}



/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_blacklist_download_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	黑名单下载组包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 08, 2019  10:10:32
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 08, 2019  10:10:32
*/
static int msg_mot_blacklist_download_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_blacklist_download_unpack(const STRUCT_ONCE_SIMPLE *pterm_para, const const FTP_INFOT_T *pFTPInfo);
 * @brief:	黑名单下载解包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 08, 2019  10:11:13
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 08, 2019  10:11:13
*/
static int msg_mot_blacklist_download_unpack(const STRUCT_ONCE_SIMPLE *pterm_para, const FTP_INFOT_T *pFTPInfo)
{
	int iret = 0;
	int i = 0;
	char fara_path[256] = {0};
	char linebuf[128 + 1] = "\0";
	char *pTmp = NULL;
	FILE *fp = NULL;

	sprintf(fara_path, "%s/%s", pFTPInfo->FTPLocalPath, pFTPInfo->FTPLocalFileName);
	
	//判断文件是否存在
	iret = file_exist_check(fara_path);
	if(iret == -1)
	{
		Uart_Printf("file_exist_check err %d (fp==NULL)", iret);
		return ERR_FTP_ANALYSIS;
	}




	fp = fopen(fara_path, "r");//以只读方式打开文件
	if (fp==NULL)
	{
		Uart_Printf("fopen file err (fp==NULL)");
		return ERR_FTP_ANALYSIS;
	}
	
	//事务开启
	db_para_black_list_table_opr(DB_DELETE, NULL);
	iret = db_para_black_list_table_opr(DB_TRANSACTION_BEGIN, NULL);
	if(iret)
	{
		Uart_Printf("db_para_black_list_table_opr  DB_TRANSACTION_BEGIN Err : %d\n", iret);
		return iret;
	}

	i = 0;
	while(!feof(fp))
	{
		//逐行读取
		memset(linebuf, 0, sizeof(linebuf));
		pTmp = fgets(linebuf, 128, fp);
		if (pTmp==NULL)
		{
			break;
		}

		i ++;
		//Uart_Printf("BLACK CARDNB:[%d][%s]\n", i ++, linebuf);
		
		//入库,第0 1行不是黑名单数据
		if(i > 2)
		{
			linebuf[20] = '\0';
			db_para_black_list_table_opr(DB_INSERT, linebuf);
		}
	}

	//事务结束
	db_para_black_list_table_opr(DB_TRANSACTION_COMIT, NULL);
	
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_blacklist_download(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	交通部黑名单下载
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  10:19:57
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  10:19:57
*/
int msg_mot_blacklist_download(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;
	char user[] = "posuser";
	char pwd[] = "Ftp2019";
	char url[128] = {0};
	char local_path[128] = "/home/root/bin/BKA8888888820191104000001";
	char tempBuff[256] = {0};
	FTP_INFOT_T stFTPInfo;

	
	//memset(url, 0, sizeof(url));
	//sprintf(url, "sftp://%s:%s/%s", "123.13.201.82", "6007", "BKA8888888820191104000001");
	//报文组装
	//报文发送
	//Uart_Printf("url : %s.\n", url);
	//download(user, pwd, url, local_path);
	
	memset(&stFTPInfo, 0, sizeof(FTP_INFOT_T));
	memcpy(&stFTPInfo, &pterm_para->stFTPInfo, sizeof(FTP_INFOT_T));
	sprintf(stFTPInfo.FTPFileName, "BKA%s%s000001", pterm_para->szMerchantId, pterm_para_ver->NewBlacklistVer);
	strcpy(stFTPInfo.FTPLocalPath, pterm_para->auccfgFilePath);
	strcpy(stFTPInfo.FTPLocalFileName, stFTPInfo.FTPFileName);

	//文件下载
	iret = msg_Sftp_download_file(&stFTPInfo);
	if(iret)
	{
		Uart_Printf("msg_Sftp_download_gile Err : %d\n", iret);
		return iret;
	}

	//报文解析
	iret =msg_mot_blacklist_download_unpack(pterm_para, &stFTPInfo);
	if(iret)
	{
		Uart_Printf("msg_mot_blacklist_download_unpack Err : %d\n", iret);
		return iret;
	}

	//版本号更新
	cfg_set_para_key_value(PARA_BLACKLIST_VER, pterm_para_ver->NewBlacklistVer);
	
	//文件清除
	memset(tempBuff, 0, sizeof(tempBuff));
	sprintf(tempBuff, "rm %s/%s", stFTPInfo.FTPLocalPath, stFTPInfo.FTPLocalFileName);
	system(tempBuff);
	
	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_whitelist_download_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	白名单下载组包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 08, 2019  10:12:39
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 08, 2019  10:12:39
*/
static int msg_mot_whitelist_download_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_whitelist_download_unpack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	白名单下载解包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 08, 2019  10:12:42
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 08, 2019  10:12:42
*/
static int msg_mot_whitelist_download_unpack(const STRUCT_ONCE_SIMPLE *pterm_para, const FTP_INFOT_T *pFTPInfo)
{
	int iret = 0;
	int i = 0;
	char fara_path[256] = {0};
	char linebuf[128 + 1] = "\0";
	char *pTmp = NULL;
	FILE *fp = NULL;

	sprintf(fara_path, "%s/%s", pFTPInfo->FTPLocalPath, pFTPInfo->FTPLocalFileName);
	
	//判断文件是否存在
	iret = file_exist_check(fara_path);
	if(iret == -1)
	{
		Uart_Printf("file_exist_check err %d (fp==NULL)", iret);
		return ERR_FTP_ANALYSIS;
	}

	fp = fopen(fara_path, "r");//以只读方式打开文件
	if (fp==NULL)
	{
		Uart_Printf("fopen file err (fp==NULL)");
		return ERR_FTP_ANALYSIS;
	}

	//事务开启
	db_para_white_list_table_opr(DB_DELETE, NULL);
	iret = db_para_white_list_table_opr(DB_TRANSACTION_BEGIN, NULL);
	if(iret)
	{
		Uart_Printf("db_para_black_list_table_opr  DB_TRANSACTION_BEGIN Err : %d\n", iret);
		return iret;
	}
	i = 0;
	while(!feof(fp))
	{
		//逐行读取
		memset(linebuf, 0, sizeof(linebuf));
		pTmp = fgets(linebuf, 128, fp);
		if (pTmp==NULL)
		{
			break;
		}
		
		i ++;
		//Uart_Printf("BLACK CARDNB:[%d][%s]\n", i, linebuf);
				
		//入库,第0 1行不是黑名单数据
		if(i > 2)
		{
			linebuf[16] = '\0';
			db_para_white_list_table_opr(DB_INSERT, linebuf);
		}
	}

	//事务结束
	db_para_white_list_table_opr(DB_TRANSACTION_COMIT, NULL);
	
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	return iret;
}



/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_whitelist_download(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	交通部白名单下载
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 05, 2019  10:21:14
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 05, 2019  10:21:14
*/
int msg_mot_whitelist_download(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;
	char user[] = "posuser";
	char pwd[] = "Ftp2019";
	char url[128] = {0};
	char local_path[128] = "/home/root/bin/BKA8888888820191104000001";
	char tempBuff[256] = {0};
	FTP_INFOT_T stFTPInfo;

	
	//memset(url, 0, sizeof(url));
	//sprintf(url, "sftp://%s:%s/%s", "123.13.201.82", "6007", "BKA8888888820191104000001");
	//报文组装
	//报文发送
	//Uart_Printf("url : %s.\n", url);
	//download(user, pwd, url, local_path);
	
	memset(&stFTPInfo, 0, sizeof(FTP_INFOT_T));
	memcpy(&stFTPInfo, &pterm_para->stFTPInfo, sizeof(FTP_INFOT_T));
	sprintf(stFTPInfo.FTPFileName, "CLA%s%s000001", pterm_para->szMerchantId, pterm_para_ver->NewWhiteListVer);
	strcpy(stFTPInfo.FTPLocalPath, pterm_para->auccfgFilePath);
	strcpy(stFTPInfo.FTPLocalFileName, stFTPInfo.FTPFileName);

	//文件下载
	iret = msg_Sftp_download_file(&stFTPInfo);
	if(iret)
	{
		Uart_Printf("msg_Sftp_download_gile Err : %d\n", iret);
		return iret;
	}

	//报文解析
	iret =msg_mot_whitelist_download_unpack(pterm_para, &stFTPInfo);
	if(iret)
	{
		Uart_Printf("msg_mot_blacklist_download_unpack Err : %d\n", iret);
		return iret;
	}

	//版本号更新
	cfg_set_para_key_value(PARA_WHITELIST_VER, pterm_para_ver->NewWhiteListVer);
	
	//文件清除
	memset(tempBuff, 0, sizeof(tempBuff));
	sprintf(tempBuff, "rm %s/%s", stFTPInfo.FTPLocalPath, stFTPInfo.FTPLocalFileName);
	system(tempBuff);

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_term_para_save(const STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	参数存储
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 18, 2019  18:01:03
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 18, 2019  18:01:03
*/
int msg_term_para_save(const STRUCT_ONCE_SIMPLE *pterm_para)
{
	//线路编号存储
	if(strlen(pterm_para->szCarLineID) != 0)
	{
		cfg_set_para_key_value(PARA_LINE_NMB, pterm_para->szCarLineID);
	}
	//县区号存储	
	if(strlen(pterm_para->szCompanyNumb) != 0)
	{
		cfg_set_para_key_value(PARA_AREA_NMB, pterm_para->szCompanyNumb);
	}

	//商户号存储	
	if(strlen(pterm_para->szMerchantId) != 0)
	{
		cfg_set_para_key_value(PARA_MERCH_NMB, pterm_para->szMerchantId);
	}

	//线路名称
	if(strlen(pterm_para->szCarLineName) != 0)
	{
		cfg_set_para_key_value(PARA_LINE_NAME, pterm_para->szCarLineName);
	}
	
	//票价
	if(strlen(pterm_para->szTicketsPrice) != 0)
	{
		cfg_set_para_key_value(PARA_LINE_PRICE, pterm_para->szTicketsPrice);
	}
	
	//票制类型
	if(strlen(pterm_para->szTicketsType) != 0)
	{
		cfg_set_para_key_value(PARA_PRICE_TYPE, pterm_para->szTicketsType);
	}
	
	//路队编号
	if(strlen(pterm_para->szCarTeamID) != 0)
	{
		cfg_set_para_key_value(PARA_LINE_TEAM_ID, pterm_para->szCarTeamID);
	}
	
	//设备类型
	if(strlen(pterm_para->szTermType) != 0)
	{
		cfg_set_para_key_value(PARA_TERM_TYPE, pterm_para->szTermType);
	}

	//ftp信息存储
	if(strlen(pterm_para->stFTPInfo.FTPServIP) != 0)
	{
		cfg_set_para_key_value(PARA_FTP_IP, pterm_para->stFTPInfo.FTPServIP);
	}
	
	if(strlen(pterm_para->stFTPInfo.FTPServPort) != 0)
	{
		cfg_set_para_key_value(PARA_FTP_PORT, pterm_para->stFTPInfo.FTPServPort);
	}
	
	if(strlen(pterm_para->stFTPInfo.FTPServPath) != 0)
	{
		cfg_set_para_key_value(PARA_FTP_PATH, pterm_para->stFTPInfo.FTPServPath);
	}
	
	if(strlen(pterm_para->stFTPInfo.FTPServUser) != 0)
	{
		cfg_set_para_key_value(PARA_FTP_USER, pterm_para->stFTPInfo.FTPServUser);
	}
	
	if(strlen(pterm_para->stFTPInfo.FTPServPassword) != 0)
	{
		cfg_set_para_key_value(PARA_FTP_PWD, pterm_para->stFTPInfo.FTPServPassword);
	}

	return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int msg_term_para_download_pack(STRUCT_ONCE_SIMPLE *pterm_para, uchar *oJson);
 * @brief:	线路参数下载报文组装
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:42:12
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:42:12
*/
static int msg_term_para_download_pack(const STRUCT_ONCE_SIMPLE *pterm_para, const st_TEMP_VERSION_CTRL *pterm_para_ver, uchar *oJson)
{
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
	char *cjsonstr = NULL;

    uchar aucDevSN[32] = {0};
    uchar aucLineParaVer[32] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

    cJSON_AddStringToObject(cjRoot, "posid", pterm_para->szTermID);
    cJSON_AddStringToObject(cjRoot, "version", pterm_para_ver->NewTicketsVer);

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
 * @fn     	int msg_term_para_download_unpack(STRUCT_ONCE_SIMPLE *pterm_para, st_TEMP_VERSION_CTRL *pterm_para_ver, uchar *oJson);
 * @brief:	参数下载报文解析
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:49:40
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:49:40
*/
static int msg_term_para_download_unpack(STRUCT_ONCE_SIMPLE *pterm_para, st_TEMP_VERSION_CTRL *pterm_para_ver, uchar *oJson)
{
    int iRet = -1;
    cJSON *cjRoot = NULL;
    uchar aucBuff[512] = {0};
    cJSON *JsonData = NULL;
    cJSON *Data = NULL;
    cJSON *ICObject = NULL;
    cJSON *ICObjectArray = NULL;
    int iICObjectNum = 0;
    int i = 0;
	int ifor = 0;
    uchar aucCode[20] = {0};
    uchar aucMsg[64] = {0};

	uchar *cjsonstr = NULL;
    uchar aucUnionCardEnabled[2+1];         //银联卡双免
    uchar aucUnionOdaEnabled[2+1];          //银联ODA
    uchar aucUnionStandardQrEnabled[2+1];   //银联付款码
    uchar aucUnionQrEnabled[2+1];           //银联乘车码
    uchar aucUnionBusinessEnabled[2+1];     //银联商务通道
    uchar aucAlipayEnabled[2+1];            //支付宝乘车码
    uchar aucWechatEnabled[2+1];            //微信乘车码
    uchar aucFacePayEnabled[2+1];           //人脸识别乘车
    uchar aucAliCardTypes[20];      //支付宝乘车码卡类型
    uchar aucAliCityCode[20];       //支付宝乘车码城市编码
    uchar aucAliCityName[64];       //支付宝乘车码城市名称
    uchar aucAliBusDeptName[64];    //支付宝乘车码公交公司名称
    uchar aucAliPartnerId[32];      //支付宝乘车码合作伙伴唯一标识PID
    uchar aucAliSaasIp[128];        //支付宝乘车码德卡SaaS平台ip
    uchar aucAliSaasPort[10];       //支付宝乘车码德卡SaaS平台终端    
    uchar aucPosGeneralEnabled[2+1];     //银联商务POS通开关
    uchar aucGoldSwallowEPayEnabled[2+1];     //金燕E支付开关

    uchar aucGsAppCode[64];               //金燕e支付 appCode
    uchar aucGsMerNbr[64];                //金燕e支付  一级商户号
    uchar aucGsSubMerchantId[64];         //金燕e支付  二级商户号
    uchar aucGsBranchId[16];              //金燕e支付  交易机构号
    uchar aucGsChannelId[32];             //金燕e支付  渠道ID
    uchar aucGsAreaInfo[16];              //金燕e支付  地区信息(地区编码)
    uchar aucGsServAddr[256];             //金燕e支付  平台地址
    uchar aucGsServPort[8];               //金燕e支付  平台端口

	XCXQGJ_CARDTYPERATE_T card_type_infor;
    
    Uart_Printf("-----* %s *-----\r\n", __func__);
    
    if (!oJson)
        return 1;

    //Uart_Printf("msg:%s\r\n", oJson);
    
    cjRoot = cJSON_Parse(oJson);
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

    iRet = 0;

    /*
	    "code": "",
	    "data": {
	    "merchCode": "",
	    "lineCode": "",
	    "lastVersion": "",
	    "lineName": "",
	    "price": "",
	    "isSectBill": "",
	    "cardCount": "",
	    "icObject": {
	    "cardType": "",
	    "walletDiscount": "",
	    "monthDiscount": "",
	    "contStatus": "",
	    "limitTime": ""
	    }
    */
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("msg                : %s\n", JsonData->valuestring);
            strcpy(aucMsg, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### msg not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("code                : %s\n", JsonData->valuestring);
            strcpy(aucCode, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### code not found\r\n");
        iRet = 1;
    }

    memset(aucBuff, 0, sizeof(aucBuff));
    Data = cJSON_GetObjectItem(cjRoot, "data");
    if (Data && Data->type == cJSON_Object)
    {
        
    }
    else
    {
        Uart_Printf("Error #### data not found\r\n");
        iRet = 1;
        if (memcmp(aucCode, "00", 2) != 0)
        {
            if(cjRoot)
            {
                cJSON_Delete(cjRoot);
            }
            
            return atoi(aucCode);
        }
    }

    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "merchCode");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("merchCode           : %s\n", JsonData->valuestring);
			memcpy(pterm_para->szMerchantId, JsonData->valuestring, strlen(JsonData->valuestring) >= 15 ? 15 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### merchCode not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "lineCode");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("lineCode            : %s\n", JsonData->valuestring);
			memcpy(pterm_para->szCarLineID, JsonData->valuestring, strlen(JsonData->valuestring) >= 6 ? 6 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### lineCode not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "version");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("version         : %s\n", JsonData->valuestring);
			memcpy(pterm_para_ver->NewTicketsVer, JsonData->valuestring, strlen(JsonData->valuestring) >= 14 ? 14 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### version not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "lineName");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("lineName            : %s\n", JsonData->valuestring);
			memcpy(pterm_para->szCarLineName, JsonData->valuestring, strlen(JsonData->valuestring) >= 40 ? 40 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### lineName not found\r\n");
        iRet = 1;
    }
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "price");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("price               : %s\n", JsonData->valuestring);
			memcpy(pterm_para->szTicketsPrice, JsonData->valuestring, strlen(JsonData->valuestring) >= 8 ? 8 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### price not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "isSectBill");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("isSectBill          : %s\n", JsonData->valuestring);
			memcpy(pterm_para->szTicketsType, JsonData->valuestring, strlen(JsonData->valuestring) >= 2 ? 2 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### isSectBill not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "cardCount");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("cardCount           : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### cardCount not found\r\n");
//        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "deptId");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("deptId              : %s\n", JsonData->valuestring);	
			memset(pterm_para->szCompanyNumb, 0, sizeof(pterm_para->szCompanyNumb));
			memcpy(pterm_para->szCompanyNumb, JsonData->valuestring, strlen(JsonData->valuestring) >= 8 ? 8 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### deptId not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "groupId");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("groupId             : %s\n", JsonData->valuestring);
			memset(pterm_para->szCarTeamID, 0, sizeof(pterm_para->szCarTeamID));
			memcpy(pterm_para->szCarTeamID, JsonData->valuestring, strlen(JsonData->valuestring) >= 10 ? 10 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### groupId not found\r\n");
//        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "unMerchCode");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unMerchCode         : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### unMerchCode not found\r\n");
        //iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "unTerminalCode");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unTerminalCode      : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### unTerminalCode not found\r\n");
        //iRet = 1;
    }
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "unTpdu");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unTpdu      : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### unTpdu not found\r\n");
        //iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "unServerIp");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unServerIp          : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### unServerIp not found\r\n");
        //iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "unServerPort");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unServerPort        : %s\n", JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### unServerPort not found\r\n");
        //iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "ftpHost");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("ftpHost             : %s\n", JsonData->valuestring);
			memset(pterm_para->stFTPInfo.FTPServIP, 0, sizeof(pterm_para->stFTPInfo.FTPServIP));
			strcpy(pterm_para->stFTPInfo.FTPServIP, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### ftpHost not found\r\n");
        iRet = 1;
    }
        
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "ftpPort");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("ftpPort             : %s\n", JsonData->valuestring);
			memset(pterm_para->stFTPInfo.FTPServPort, 0, sizeof(pterm_para->stFTPInfo.FTPServPort));
			strcpy(pterm_para->stFTPInfo.FTPServPort, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### ftpPort not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "ftpAccount");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("ftpAccount          : %s\n", JsonData->valuestring);
			memset(pterm_para->stFTPInfo.FTPServUser, 0, sizeof(pterm_para->stFTPInfo.FTPServUser));
			strcpy(pterm_para->stFTPInfo.FTPServUser, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### ftpAccount not found\r\n");
        iRet = 1;
    }
       
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "ftpPassword");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("ftpPassword         : %s\n", JsonData->valuestring);
			memset(pterm_para->stFTPInfo.FTPServPassword, 0, sizeof(pterm_para->stFTPInfo.FTPServPassword));
			strcpy(pterm_para->stFTPInfo.FTPServPassword, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### ftpPassword not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "ftpPath");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("ftpPath             : %s\n", JsonData->valuestring);
			memset(pterm_para->stFTPInfo.FTPServPath, 0, sizeof(pterm_para->stFTPInfo.FTPServPath));
			strcpy(pterm_para->stFTPInfo.FTPServPath, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### ftpPath not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "busNo");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("busNo             : %s\n", JsonData->valuestring);
			memset(pterm_para->szCarID, 0, sizeof(pterm_para->szCarID));
			memcpy(pterm_para->szCarID, JsonData->valuestring, strlen(JsonData->valuestring) >= 12 ? 12 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### busNo not found\r\n");
//        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(Data, "posType");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("posType             : %s\n", JsonData->valuestring);
			memset(pterm_para->szTermType, 0, sizeof(pterm_para->szTermType));
			memcpy(pterm_para->szTermType, JsonData->valuestring, strlen(JsonData->valuestring) >= 8 ? 8 : strlen(JsonData->valuestring));
        }
    }
    else
    {
        Uart_Printf("Error #### posType not found\r\n");
        iRet = 1;
    }

    ICObject = cJSON_GetObjectItem(Data, "icObject");
    if (ICObject && ICObject->type == cJSON_Array)
    {
        Uart_Printf("icObject is cJSON_Array\r\n");
    }
    else
    {
        Uart_Printf("Error #### icObject not found\r\n");
        iRet = 1;
    }

    if (ICObject)
    {
        iICObjectNum = cJSON_GetArraySize(ICObject);
        Uart_Printf("iICObjectNum = %d \r\n", iICObjectNum);

		if(iICObjectNum > 0)
		{
			db_para_card_type_table_opr(DB_DELETE, NULL);
		}
		
        ifor = 0;
		
        for (i = 0; i < iICObjectNum; i ++)
        {
            Uart_Printf("\r\n-----------------------------------------------------------------------------\r\n");
            ICObjectArray = cJSON_GetArrayItem(ICObject,i);

			memset(&card_type_infor, 0, sizeof(XCXQGJ_CARDTYPERATE_T));
			
            memset(aucBuff, 0, sizeof(aucBuff));
            JsonData = cJSON_GetObjectItem(ICObjectArray, "cardType");
            if (JsonData && JsonData->type == cJSON_String)
            {
                if (JsonData->valuestring)
                {
        			Uart_Printf("cardType        : %s\n", JsonData->valuestring);
					memcpy(card_type_infor.aucCardType, JsonData->valuestring, strlen(JsonData->valuestring) >= 4 ? 4 : strlen(JsonData->valuestring));
                }
            }
            else
            {
                Uart_Printf("Error #### cardType not found\r\n");
                iRet = 1;
            }
            
            memset(aucBuff, 0, sizeof(aucBuff));
            JsonData = cJSON_GetObjectItem(ICObjectArray, "walletDiscount");
            if (JsonData && JsonData->type == cJSON_String)
            {
                if (JsonData->valuestring)
                {
        			Uart_Printf("walletDiscount  : %s\n", JsonData->valuestring);
					memcpy(card_type_infor.aucWalletDiscount, JsonData->valuestring, strlen(JsonData->valuestring) >= 6 ? 6 : strlen(JsonData->valuestring));
                }
            }
            else
            {
                Uart_Printf("Error #### walletDiscount not found\r\n");
                iRet = 1;
            }
            
            memset(aucBuff, 0, sizeof(aucBuff));
            JsonData = cJSON_GetObjectItem(ICObjectArray, "monthDiscount");
            if (JsonData && JsonData->type == cJSON_String)
            {
                if (JsonData->valuestring)
                {
        			Uart_Printf("monthDiscount   : %s\n", JsonData->valuestring);
					memcpy(card_type_infor.aucMonthDiscount, JsonData->valuestring, strlen(JsonData->valuestring) >= 6 ? 6 : strlen(JsonData->valuestring));
                }
            }
            else
            {
                Uart_Printf("Error #### monthDiscount not found\r\n");
                iRet = 1;
            }
            
            memset(aucBuff, 0, sizeof(aucBuff));
            JsonData = cJSON_GetObjectItem(ICObjectArray, "contStatus");
            if (JsonData && JsonData->type == cJSON_String)
            {
                if (JsonData->valuestring)
                {
        			Uart_Printf("contStatus      : %s\n", JsonData->valuestring);
					memcpy(card_type_infor.aucContStatus, JsonData->valuestring, strlen(JsonData->valuestring) >= 4 ? 4 : strlen(JsonData->valuestring));
                }
            }
            else
            {
                Uart_Printf("Error #### contStatus not found\r\n");
                iRet = 1;
            }
            
            memset(aucBuff, 0, sizeof(aucBuff));
            JsonData = cJSON_GetObjectItem(ICObjectArray, "limitTime");
            if (JsonData && JsonData->type == cJSON_String)
            {
                if (JsonData->valuestring)
                {
        			Uart_Printf("limitTime       : %s\n", JsonData->valuestring);
					memcpy(card_type_infor.aucLimitTime, JsonData->valuestring, strlen(JsonData->valuestring) >= 10 ? 10 : strlen(JsonData->valuestring));
                }
            }
            else
            {
                Uart_Printf("Error #### limitTime not found\r\n");
                iRet = 1;
            }

			//存储
			db_para_card_type_table_opr(DB_INSERT, &card_type_infor);
        }
    }
    
	///<[yufupeng|2019年5月22日 15:16:34]|  增加支付方式的开关控制

    //银联卡云闪付开关
    memset(aucUnionCardEnabled, 0, sizeof(aucUnionCardEnabled));
    JsonData = cJSON_GetObjectItem(Data, "unionCardEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unionCardEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucUnionCardEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### unionCardEnabled not found\r\n");
        iRet = 1;
    }

    //银联银联ODA开关
    memset(aucUnionOdaEnabled, 0, sizeof(aucUnionOdaEnabled));
    JsonData = cJSON_GetObjectItem(Data, "unionOdaEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unionOdaEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucUnionOdaEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### unionOdaEnabled not found\r\n");
        iRet = 1;
    }

    //银联付款码付开关
    memset(aucUnionStandardQrEnabled, 0, sizeof(aucUnionStandardQrEnabled));
    JsonData = cJSON_GetObjectItem(Data, "unionStandardQrEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unionStandardQrEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucUnionStandardQrEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### unionStandardQrEnabled not found\r\n");
        iRet = 1;
    }

    //银联卡乘车码开关
    memset(aucUnionQrEnabled, 0, sizeof(aucUnionQrEnabled));
    JsonData = cJSON_GetObjectItem(Data, "unionQrEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unionQrEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucUnionQrEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### unionQrEnabled not found\r\n");
        iRet = 1;
    }

    //银联商务通道    
    memset(aucUnionBusinessEnabled, 0, sizeof(aucUnionBusinessEnabled));
    JsonData = cJSON_GetObjectItem(Data, "unionBusinessEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("unionBusinessEnabled         : %s\n", JsonData->valuestring);
            memcpy(aucUnionBusinessEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### unionBusinessEnabled not found\r\n");
        iRet = 1;
    }
    
    //支付宝乘车码开关
    memset(aucAlipayEnabled, 0, sizeof(aucAlipayEnabled));
    JsonData = cJSON_GetObjectItem(Data, "alipayEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("alipayEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucAlipayEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### alipayEnabled not found\r\n");
        iRet = 1;
    }

    //微信乘车码开关
    memset(aucWechatEnabled, 0, sizeof(aucWechatEnabled));
    JsonData = cJSON_GetObjectItem(Data, "wechatEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("wechatEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucWechatEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### wechatEnabled not found\r\n");
        iRet = 1;
    }

    //人脸识别乘车开关
    memset(aucFacePayEnabled, 0, sizeof(aucFacePayEnabled));
    JsonData = cJSON_GetObjectItem(Data, "facePayEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("facePayEnabled             : %s\n", JsonData->valuestring);
            memcpy(aucFacePayEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### facePayEnabled not found\r\n");
        iRet = 1;
    }
    
    //银联商务POS通开关    
    memset(aucPosGeneralEnabled, 0, sizeof(aucPosGeneralEnabled));
    JsonData = cJSON_GetObjectItem(Data, "posGeneralEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("posGeneralEnabled         : %s\n", JsonData->valuestring);
            memcpy(aucPosGeneralEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### posGeneralEnabled not found\r\n");
        iRet = 1;
    }
    
    //金燕E支付开关    
    memset(aucGoldSwallowEPayEnabled, 0, sizeof(aucGoldSwallowEPayEnabled));
    JsonData = cJSON_GetObjectItem(Data, "goldSwallowEPayEnabled");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("goldSwallowEPayEnabled         : %s\n", JsonData->valuestring);
            memcpy(aucGoldSwallowEPayEnabled, JsonData->valuestring, 1);
        }
    }
    else
    {
        Uart_Printf("Error #### goldSwallowEPayEnabled not found\r\n");
        iRet = 1;
    }
    
#ifdef ALIPAY_CCQR //支付宝乘车码
    if (aucAlipayEnabled[0] == '1')//打开支付宝乘车码时，再处理支付宝乘车码的参数
    {
    	///<[yufupeng|2019年8月7日 9:59:41]|  新增支付宝乘车码的参数
        memset(aucAliCardTypes, 0, sizeof(aucAliCardTypes));
        JsonData = cJSON_GetObjectItem(Data, "aliCardTypes");//支付宝卡类型
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliCardTypes             : %s\n", JsonData->valuestring);
                strcpy(aucAliCardTypes, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliCardTypes not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliCityCode, 0, sizeof(aucAliCityCode));
        JsonData = cJSON_GetObjectItem(Data, "aliCityCode");//支付宝城市编码
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliCityCode             : %s\n", JsonData->valuestring);
                strcpy(aucAliCityCode, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliCityCode not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliCityName, 0, sizeof(aucAliCityName));
        JsonData = cJSON_GetObjectItem(Data, "aliCity");//支付宝城市名称
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliCity             : %s\n", JsonData->valuestring);
                strcpy(aucAliCityName, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliCity not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliBusDeptName, 0, sizeof(aucAliBusDeptName));
        JsonData = cJSON_GetObjectItem(Data, "aliBusDeptName");//支付宝公交公司名称
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliBusDeptName             : %s\n", JsonData->valuestring);
                strcpy(aucAliBusDeptName, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliBusDeptName not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliPartnerId, 0, sizeof(aucAliPartnerId));
        JsonData = cJSON_GetObjectItem(Data, "aliPartnerId");//支付宝合作伙伴唯一标识
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliPartnerId             : %s\n", JsonData->valuestring);
                strcpy(aucAliPartnerId, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliPartnerId not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliSaasIp, 0, sizeof(aucAliSaasIp));
        JsonData = cJSON_GetObjectItem(Data, "aliSaasIp");//SAAS平台IP
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliSaasIp             : %s\n", JsonData->valuestring);
                strcpy(aucAliSaasIp, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliSaasIp not found\r\n");
            iRet = 1;
        }
        
        memset(aucAliSaasPort, 0, sizeof(aucAliSaasPort));
        JsonData = cJSON_GetObjectItem(Data, "aliSaasPort");//SAAS平台端口
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("aliSaasPort             : %s\n", JsonData->valuestring);
                strcpy(aucAliSaasPort, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### aliSaasPort not found\r\n");
            iRet = 1;
        }
    }
#endif /* #ifdef ALIPAY_CCQR //支付宝乘车码 */

#ifdef JINYANEPAY //金燕E支付
    if('1' == aucGoldSwallowEPayEnabled[0])//打开金燕E支付
    {
        memset(aucGsAppCode, 0, sizeof(aucGsAppCode));
        JsonData = cJSON_GetObjectItem(Data, "gsAppCode");//金燕e支付 appCode
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsAppCode             : %s\n", JsonData->valuestring);
                strcpy(aucGsAppCode, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsAppCode not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsMerNbr, 0, sizeof(aucGsMerNbr));
        JsonData = cJSON_GetObjectItem(Data, "gsMerchCode");//金燕e支付 一级商户号
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsMerchCode             : %s\n", JsonData->valuestring);
                strcpy(aucGsMerNbr, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsMerchCode not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsSubMerchantId, 0, sizeof(aucGsSubMerchantId));
        JsonData = cJSON_GetObjectItem(Data, "gsSubMerchCode");//金燕e支付   二级商户号
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsSubMerchCode             : %s\n", JsonData->valuestring);
                strcpy(aucGsSubMerchantId, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsSubMerchCode not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsBranchId, 0, sizeof(aucGsBranchId));
        JsonData = cJSON_GetObjectItem(Data, "gsTradeUnitCode");//金燕e支付    交易机构号
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsTradeUnitCode             : %s\n", JsonData->valuestring);
                strcpy(aucGsBranchId, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsTradeUnitCode not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsChannelId, 0, sizeof(aucGsChannelId));
        JsonData = cJSON_GetObjectItem(Data, "gsChannelId");//金燕e支付    渠道ID
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsChannelId             : %s\n", JsonData->valuestring);
                strcpy(aucGsChannelId, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsChannelId not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsAreaInfo, 0, sizeof(aucGsAreaInfo));
        JsonData = cJSON_GetObjectItem(Data, "gsAreaInfo");//金燕e支付    地区信息(地区编码)
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsAreaInfo             : %s\n", JsonData->valuestring);
                strcpy(aucGsAreaInfo, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsAreaInfo not found\r\n");
            iRet = 1;
        }
        
        memset(aucGsServAddr, 0, sizeof(aucGsServAddr));
        JsonData = cJSON_GetObjectItem(Data, "gsUrl");//金燕e支付    平台地址
        if (JsonData && JsonData->type == cJSON_String)
        {
            if (JsonData->valuestring)
            {
    			Uart_Printf("gsUrl             : %s\n", JsonData->valuestring);
                strcpy(aucGsServAddr, JsonData->valuestring);
            }
        }
        else
        {
            Uart_Printf("Error #### gsUrl not found\r\n");
            iRet = 1;
        }
    }
#endif /* JINYANEPAY 金燕E支付 */

    Uart_Printf("\r\n-----------------------------------------------------------------------------\r\n");
    
    if(cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    if(memcmp(aucCode, "00", 2) != 0)
    {
        return 2;
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int msg_term_para_download(const STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	终端参数下载
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 06, 2019  17:32:05
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 06, 2019  17:32:05
*/
int msg_term_para_download(STRUCT_ONCE_SIMPLE *pterm_para, st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;
	char cjsStr[1024] = {0};
	char aucURL[512] = {0};

	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;
	
	//报文组装
	memset(cjsStr, 0, sizeof(cjsStr));
	iret = msg_term_para_download_pack(pterm_para, pterm_para_ver, cjsStr);
	if(iret)
	{
		Uart_Printf("msg_term_para_download_pack Err : %d\n", iret);
		return iret;
	}

	//地址获取
	memset(aucURL, 0, sizeof(aucURL));
    iret = msg_get_http_header(pterm_para, 0, "parameters", aucURL);
    if (0 != iret)
    {
		Uart_Printf("msg_get_http_header Err : %d\n", iret);
        return 1;
    }

	//报文发送
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	uiSendLen = 0;

	memcpy(aucSendBuff, cjsStr, strlen(cjsStr));
	uiSendLen = strlen(cjsStr);
	
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));
	uiRecvLen = 0;
    iret = msg_http_send_rcve(aucURL, PORTPARA, aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
    if (0 != iret)
    {
    	

		Uart_Printf("msg_http_send_rcve Err : %d\n", iret);
		if(iret == SDK_TIME_OUT)
		{
			iret = ERR_TERMNOTRCVSERV;
		}
        
        return iret;
    }

	//报文解析
	iret = msg_term_para_download_unpack(pterm_para, pterm_para_ver, aucRecvBuff);
	if(iret)
	{
		Uart_Printf("msg_term_para_download_unpack Err : %d\n", iret);
		return iret;
	}

	//参数存储
	msg_term_para_save(pterm_para);

	return iret;
}



/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_data_pack(uchar *pucInData, uchar *pucOutData);
 * @brief:	交通部交易报文data域组装
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 11, 2019  11:36:48
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 11, 2019  11:36:48
*/
static int msg_mot_data_pack(StTradeInfor *pmot_trade_log, uchar *pucOutData)
{
    int iOffset = 0;
    unsigned char tempbuf[30]= {0};
    unsigned char tempbuf2[30]= {0};
    uchar aucBuff[1024] = {0};
    int iRet=0;
    
#if 0
    RHT_MSG_BODY_T stRhtSndMsg, stRhtRevMsg;
    MOT_RHT_EP_DEAL_REC_T stTradeRec;
    tarde_change((MOT_LS_EP_TRANS_RECORD_ASC_T *)ucpData, &stTradeRec);
    
    RHT_msg_init(&stRhtSndMsg);
    RHT_msg_init(&stRhtRevMsg);

    stRhtSndMsg.ucFTI = 0x42;

    memcpy(stRhtSndMsg.ucaMTI, "\xA0\x42", 2);

    memcpy(stRhtSndMsg.ucaDBL, "\x00\x00\x00\x00", 4);

    stRhtSndMsg.ucRTI = 0x52;

    stRhtSndMsg.ucSI = RHT_NORMAL_SI;

    stRhtSndMsg.stEXT.ucSW = 0;

    memcpy(stRhtSndMsg.stEXT.ucaCRC16, "\x00\x00\x00\x00", 4);

    stRhtSndMsg.stEXT.ucRESERVE = 0;

    iOffset = 0;
    
    //会话码
    print_hex("会话码:", "\x00\x00\x00\x00", 4);
    memcpy(stRhtSndMsg.stEXT.ucaDATA+iOffset, "\x00\x00\x00\x00", 4);
    iOffset += 4;

    //记录条数是一条
    stRhtSndMsg.stEXT.ucaDATA[iOffset++] = 1;

    //结算单元编号
    memcpy(stRhtSndMsg.stEXT.ucaDATA+iOffset, RHT_SETTLEMENTCODE, 4);
    iOffset += 4;

    memcpy(stRhtSndMsg.stEXT.ucaDATA+iOffset, (uchar *)&stTradeRec, sizeof(MOT_RHT_EP_DEAL_REC_T)-3);
    iOffset += sizeof(MOT_RHT_EP_DEAL_REC_T)-3;

    stRhtSndMsg.stEXT.ucaLEN[0] = iOffset/256;
    stRhtSndMsg.stEXT.ucaLEN[1] = iOffset%256;
#endif /* 0 */

    if (!pmot_trade_log)
        return 1;
    
    if (!pucOutData)
        return 1;

    iOffset = 0;
	memset(aucBuff, 0, sizeof(aucBuff));
	
    //报文标识
    memcpy(aucBuff+iOffset, "A042", 4);
    iOffset += 4;

    //会话码
    memcpy(aucBuff+iOffset, "00000000", 8);
    iOffset += 8;

    //记录条数是一条
    sprintf(aucBuff+iOffset, "%02d", 1);
    iOffset += 2;

    //结算单元编号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_merch_id, 8);
    iOffset += 8;

    //PSAM终端机编号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_psam_tern_id, 12);
    iOffset += 12;

    //线路编号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_line_id, 4);
    iOffset += 4;

    //司机编号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_driver_id, 6);
    iOffset += 6;

    //车辆编号.用PSAM终端号
    //memcpy(aucBuff+iOffset, pmot_trade_log->c_out_car_id, 12);
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_psam_tern_id, 12);
    iOffset += 12;

    //班次顺序号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_class_sq, 8);
    iOffset += 8;

    //发卡机构标识
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_ussuer_id, 16);
    iOffset += 16;

    //清分城市
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_card_citycode, 4);
    iOffset += 4;

    //卡应用序列号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_card_id, 20);
    iOffset += 20;

    //卡应用类型
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_card_child_type, 2);
    iOffset += 2;

    //卡交易序号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_card_cnt, 4);
    iOffset += 4;

    //交易金额
    sprintf(aucBuff+iOffset, "%08X", pmot_trade_log->i_out_real_trade_amt);
    iOffset += 8;

    //交易类型
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_trade_type, 2);
    iOffset += 2;

    //终端交易序号
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_psam_sq, 8);
    iOffset += 8;

    //交易日期时间
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_trade_datetime, 14);
    iOffset += 14;

    //交易验证码
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_tac, 8);
    iOffset += 8;

    //卡片当前余额
    sprintf(aucBuff+iOffset, "%08X", pmot_trade_log->i_out_aft_blance);
    iOffset += 8;

    //卡片交易前余额
    sprintf(aucBuff+iOffset, "%08X", pmot_trade_log->i_out_bef_blance);
    iOffset += 8;

    //随机数
    memcpy(aucBuff+iOffset, pmot_trade_log->c_out_trade_random, 8);
    iOffset += 8;

    //记录类型 00H上车记录 01H下车记录 02H补票记录
    memcpy(aucBuff+iOffset, "00", 2);
    iOffset += 2;

    //行车方向 00H上行 01H下行
    memcpy(aucBuff+iOffset, "00", 2);
    iOffset += 2;

    //站点序号
    memcpy(aucBuff+iOffset, "00", 2);
    iOffset += 2;

    //逃票受理机构标识 HEX
    memcpy(aucBuff+iOffset, "0000000000000000", 16);
    iOffset += 16;

    //逃票受理终端编码
    memcpy(aucBuff+iOffset, "0000000000000000", 16);
    iOffset += 16;

    Uart_Printf("aucBuff:[%d[]%s\r\n", iOffset, aucBuff);

    memcpy(pucOutData, aucBuff, iOffset);
    
    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_trade_update_pack(StTradeInfor *pmot_trade_log, uchar *oJson);
 * @brief:	交通部交易上送报文组装
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 11, 2019  11:24:56
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 11, 2019  11:24:56
*/
static int msg_mot_trade_update_pack(StTradeInfor *pmot_trade_log, uchar *oJson)
{
    int iRet = 1;
    cJSON *cjRoot = NULL;
    cJSON *Data = NULL;
    char  *cjsonstr = NULL;
    uchar aucDevSN[32] = {0};
    uchar aucBuffer[1024] = {0};

    Uart_Printf("-----* %s *-----\r\n", __func__);

    cjRoot = cJSON_CreateObject();
    if (NULL == cjRoot)
    {
        Uart_Printf("create cjRoot error\r\n");
        return 1;
    }

	//data 数据组装
    Data = cJSON_CreateObject();
    if (NULL == Data)
    {
        Uart_Printf("create Data error\r\n");
        if(cjRoot)
        {
            cJSON_Delete(cjRoot);
        }
        return 1;
    }
    
    cJSON_AddStringToObject(Data, "posId", pmot_trade_log->c_out_sn);
    cJSON_AddStringToObject(Data, "type", "2200C");
    cJSON_AddStringToObject(Data, "route", pmot_trade_log->c_out_line_id);
    cJSON_AddStringToObject(Data, "posType", "01");
	cJSON_AddStringToObject(Data, "driverCardNo", pmot_trade_log->c_out_driver_id);

	    memset(aucBuffer, 0, sizeof(aucBuffer));
	    iRet = msg_mot_data_pack(pmot_trade_log, aucBuffer);
	    if(0 != iRet)
	    {
	        if (Data)
	        {
	            cJSON_Delete(Data);
	        }
	        
	        if(cjRoot)
	        {
	            cJSON_Delete(cjRoot);
	        }
	        
	        return 1;
	    }
    
    	cJSON_AddStringToObject(Data, "data", aucBuffer);

	//data
    cJSON_AddItemToObject(cjRoot, "data", Data);

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
 * @fn     	int msg_mot_trade_update_unpack(uchar *oJson);
 * @brief:	交通部交易上送解包
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 11, 2019  11:56:29
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 11, 2019  11:56:29
*/
static int msg_mot_trade_update_unpack(uchar *Msg)
{
    int iRet = -1;
    cJSON *cjRoot = NULL;
    uchar aucBuff[512] = {0};
    cJSON *JsonData = NULL;
    uchar aucCode[128] = {0};
    uchar aucMsg[64] = {0};
    
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
    
    //{"msg":"error","code":"999"} 
    
    iRet = 1;
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "code");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("code                : %s\n", JsonData->valuestring);
            strcpy(aucCode, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### code not found\r\n");
        iRet = 1;
    }
    
    memset(aucBuff, 0, sizeof(aucBuff));
    JsonData = cJSON_GetObjectItem(cjRoot, "msg");
    if (JsonData && JsonData->type == cJSON_String)
    {
        if (JsonData->valuestring)
        {
			Uart_Printf("aucMsg              : %s\n", JsonData->valuestring);
            strcpy(aucMsg, JsonData->valuestring);
        }
    }
    else
    {
        Uart_Printf("Error #### msg not found\r\n");
        iRet = 1;
    }
    
    if (cjRoot)
    {
    	cJSON_Delete(cjRoot);
    }

    
    if (memcmp(aucCode, "100", 3) == 0)
    {
        iRet = 0;
    }
    else
    {
        iRet = 1;
    }

    return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int msg_mot_trade_update(StTradeInfor *pmot_trade_log);
 * @brief:	交通部交易上送
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 11, 2019  11:21:38
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 11, 2019  11:21:38
*/
int msg_mot_trade_update(const StTradeInfor *pmot_trade_log, const STRUCT_ONCE_SIMPLE *pterm_para)
{
	int iret = 0;
	char cjsStr[1024] = {0};
	char aucURL[512] = {0};

	uchar aucSendBuff[1024] = {0};
    uint uiSendLen = 0;
    uchar aucRecvBuff[1024] = {0};
    uint uiRecvLen = 0;

	//报文组装
	memset(cjsStr, 0, sizeof(cjsStr));
	iret = msg_mot_trade_update_pack(pmot_trade_log, cjsStr);
	if(iret)
	{
		Uart_Printf("msg_mot_trade_update_pack Err : %d\n", iret);
		return iret;
	}

	//地址获取
	memset(aucURL, 0, sizeof(aucURL));
    iret = msg_get_http_header(pterm_para, 1, "produce", aucURL);
    if (0 != iret)
    {
        return 1;
    }

	//报文发送
	memset(aucSendBuff, 0, sizeof(aucSendBuff));
	uiSendLen = 0;

	memcpy(aucSendBuff, cjsStr, strlen(cjsStr));
	uiSendLen = strlen(cjsStr);
	
	memset(aucRecvBuff, 0, sizeof(aucRecvBuff));
	uiRecvLen = 0;
    iret = msg_http_send_rcve(aucURL, PORTDEAL, aucSendBuff, uiSendLen, aucRecvBuff, &uiRecvLen);
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
	iret = msg_mot_trade_update_unpack(aucRecvBuff);
	if(iret)
	{
		Uart_Printf("msg_mot_trade_update_unpack Err : %d\n", iret);
		return iret;
	}

	return iret;

}



