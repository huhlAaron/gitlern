/*
 * IF_UART.c
 *	底层打印接口封装
 *  Created on: November 01, 2019  16:39:54
 *      Author: huhongliang@decard.com
 */

#include <stdio.h>
#include <stdarg.h>

#include "../../src_inc/data_type.h"

//日志路径
static char LOG_PATH[512] = {0};

//日志开关标识
static char bUartDebug = TRUE;

/**
 * @Author: hhl \@decard
 * @fn     	void Uart_SetDebug(char ucFlag);
 * @brief:	调试日志开关设置接口
 * @param[in] :ucFlag:1开启 0关闭
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  14:38:28
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  14:38:28
*/
void Uart_SetDebug(char ucFlag)
{
    bUartDebug = (ucFlag) ? TRUE : FALSE;
}

/**
 * @Author: hhl \@decard
 * @fn     	void Uart_SetLogPath(char *path);
 * @brief:	设置调试日志路径
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 04, 2019  17:02:56
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 04, 2019  17:02:56
*/
void Uart_SetLogPath(char *path)
{
	memset(LOG_PATH, 0, sizeof(LOG_PATH));
	strcpy(LOG_PATH, path);
}

/**
 * @Author: hhl \@decard
 * @fn     	void Uart_Printf(const char *cmd, ...);
 * @brief:	
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  14:36:40
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  14:36:40
*/
void Uart_Printf(const char *cmd, ...)
{
	char DateTime[15] = {0};
	char ucaLogfilePath[1024]={0};
	FILE *fd=NULL;
	
    if(!bUartDebug)
        return;

	if(strlen(LOG_PATH) != 0)
	{
		memset(DateTime, 0, sizeof(DateTime));
		GetLocDate(DateTime);

		memset(ucaLogfilePath, 0, sizeof(ucaLogfilePath));
		sprintf(ucaLogfilePath,"%s/%.8s.log", LOG_PATH, DateTime);

		fd = fopen(ucaLogfilePath, "a+");
		if(fd == NULL)
		{
			printf("fopen(%s) Error\n", ucaLogfilePath);
			return ;
		}
	}
	
    va_list args;       	//定义一个va_list类型的变量，用来储存单个参数
    va_start(args,cmd); 	//使args指向可变参数的第一个参数
    if(strlen(LOG_PATH) != 0)
    {
	    vfprintf(fd, cmd, args);  	//必须用vprintf等带V的
    }
    vprintf(cmd,args);  	//必须用vprintf等带V的
    va_end(args);       	//结束可变参数的获取

	fclose(fd);
}


/**
 * @Author: hhl \@decard
 * @fn     	void File_Printf(const char *cmd, ...);
 * @brief:	文件打印,不受开关限制，主要用于错误信息记录
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  09:30:40
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  09:30:40
*/
void File_Printf(const char *cmd, ...)
{
	char DateTime[15] = {0};
	char ucaLogfilePath[1024]={0};
	FILE *fd=NULL;
	
    //if(!bUartDebug)
    //   return;

	if(strlen(LOG_PATH) != 0)
	{
		memset(DateTime, 0, sizeof(DateTime));
		GetLocDate(DateTime);

		memset(ucaLogfilePath, 0, sizeof(ucaLogfilePath));
		sprintf(ucaLogfilePath,"%s/%.8s.log", LOG_PATH, DateTime);

		fd = fopen(ucaLogfilePath, "a+");
		if(fd == NULL)
		{
			printf("fopen(%s) Error\n", ucaLogfilePath);
			return ;
		}
	}
	
    va_list args;       	//定义一个va_list类型的变量，用来储存单个参数
    va_start(args,cmd); 	//使args指向可变参数的第一个参数
    if(strlen(LOG_PATH) != 0)
    {
	    vfprintf(fd, cmd, args);  	//必须用vprintf等带V的
    }
    vprintf(cmd,args);  	//必须用vprintf等带V的
    va_end(args);       	//结束可变参数的获取

	fclose(fd);
}


/**
 * @Author: hhl \@decard
 * @fn     	void DumpData(const char * str, unsigned char * pBuf, unsigned int len);
 * @brief:	格式化输出
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  16:02:02
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  16:02:02
*/
void DumpData(const char * str, unsigned char * pBuf, unsigned int len)
{
    unsigned int i;
    char tmp[17];
    char *p;
    char *add = (char *)pBuf;

    if(!bUartDebug)
    {
		return;
    }
	
    if(str)
    {
        Uart_Printf("\r\n%s: length = %d [0x%X]\r\n", str, len, len);
    }
    if(len == 0)
    {
        return;
    }
    p = tmp;
    Uart_Printf("%p  ", add);
    Uart_Printf("\t");
    for(i=0;i<len;i++)
    {
        Uart_Printf("%02X ",pBuf[i]);
        if((pBuf[i]>=0x20) && (pBuf[i]<0x7F))
        {
            *p++ = pBuf[i];
        }
        else
        {
            *p++ = '.';
        }
        if((i+1)%16==0)
        {
            *p++ = 0;//string end
            Uart_Printf("        | %s", tmp);
            p = tmp;

            Uart_Printf("\r\n");

            if((i+1) < len)
            {
                add += 16;
                Uart_Printf("%p  ", add);
                Uart_Printf("\t");
            }
        }
        else if((i+1)%8==0)
        {
            Uart_Printf("- ");
        }
    }
    if(len%16!=0)
    {
        for(i=len%16;i<16;i++)
        {
            Uart_Printf("   ");
            if(((i+1)%8==0) && ((i+1)%16!=0))
            {
                Uart_Printf("- ");
            }
        }
        *p++ = 0;//string end
        Uart_Printf("        | %s", tmp);
        Uart_Printf("\r\n");
    }

    return;
}


void print_hex(const char *pTitle, const char *pSendBuff, int SendLen)
{
    int i;
    Uart_Printf("%s ---> (%d)(%d): [", pTitle, SendLen, SendLen);
    for(i = 0; i < SendLen; i ++)
    {
        Uart_Printf("%02X", pSendBuff[i]);
    }

    Uart_Printf("]\r\n");
}



