/*
 * config.c
 *	设备参数、配置文件操作模块
 *  Created on: November 07, 2019  16:10:12
 *      Author: huhongliang@decard.com
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "data_type.h"
#include "define.h"


//交易配置文件路径
static char config_file_path[256] = {0};

//交易配置文件名称
static char xc_city_bus_config_file[64] = "xc_city_bus_trade.ini";


//交易流水配置文件路径
static char config_serail_file_path[256] = {0};

//交易流水配置文件名称
static char xc_city_bus_config_serail_file[64] = "xc_city_bus_serail.ini";

/**
 * @Author: hhl \@decard
 * @fn     	void cfg_set_path(char *path);
 * @brief:	配置文件路径设置
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 18, 2019  09:49:30
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 18, 2019  09:49:30
*/
void cfg_set_path(char *path)
{
	//交易配置文件
	memset(config_file_path, 0, sizeof(config_file_path));
	sprintf(config_file_path, "%s/%s", path, xc_city_bus_config_file);

	
	//交易流水配置文件
	memset(config_serail_file_path, 0, sizeof(config_serail_file_path));
	sprintf(config_serail_file_path, "%s/%s", path, xc_city_bus_config_serail_file);
}

/**
 * @Author: hhl \@decard
 * @fn     	int cfg_file_check(void);
 * @brief:	参数配置文件校验
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 18, 2019  09:50:49
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 18, 2019  09:50:49
*/
int cfg_file_check(void)
{
	int iret = 0;
	FILE *fp = NULL;

	iret = file_exist_check(config_file_path);
	if(iret != 0)
	{
		//重新创建
		fp = fopen(config_file_path, "a");//以“追加”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。
		if(fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
	}

	iret = file_exist_check(config_serail_file_path);
	if(iret != 0)
	{
		//重新创建
		fp = fopen(config_serail_file_path, "a");//以“追加”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。
		if(fp != NULL)
		{
			fclose(fp);
			fp = NULL;
		}
	}
	
	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	void TermWriteSerialNumb(void);
 * @brief:	流水号写入
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  19:19:44
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  19:19:44
*/
int cfg_get_serial_nmb(void)
{
	unsigned char TempBuff[7] = "\0";

	ConfigGetKey(config_serail_file_path, CFG_SERILAUMB, SER_SERILAUMB, TempBuff);
	TempBuff[6] = '\0';
	
	return atol(TempBuff);
}

/**
 * @Author: hhl \@decard
 * @fn     	void TermReadSerialNumb(void);
 * @brief:	流水号读取
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  19:19:56
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  19:19:56
*/
void cfg_set_serial_nmb(char *SerailNmb)
{
	unsigned char TempBuff[7] = "\0";

	SerailNmb[6] = '\0';
	sprintf(TempBuff, "%06d", atol(SerailNmb) % MAX_TRADE_SEQ + 1);
	Uart_Printf("WriteSerilaNumb:%s\n", TempBuff);

	ConfigSetKey(config_serail_file_path, CFG_SERILAUMB, SER_SERILAUMB, TempBuff);
	
	memset(SerailNmb, 0, sizeof(SerailNmb));
	memcpy(SerailNmb, TempBuff, 6);
	SerailNmb[6] = '\0';
}

/**
 * @Author: hhl \@decard
 * @fn     	int cfg_get_para_ver(st_TEMP_VERSION_CTRL *pterm_para_ver);
 * @brief:	设备参数版本获取
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 07, 2019  16:12:48
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 07, 2019  16:12:48
*/
int cfg_get_para_ver(st_TEMP_VERSION_CTRL *pterm_para_ver)
{
	int iret = 0;

	memcpy(pterm_para_ver->NewTicketsVer, "00000000000000", 14);
	memcpy(pterm_para_ver->NewBlacklistVer, "00000000", 8);
	memcpy(pterm_para_ver->NewWhiteListVer, "00000000", 8);
	
	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int cfg_get_para(STRUCT_ONCE_SIMPLE *pterm_para);
 * @brief:	参数获取
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 07, 2019  16:14:10
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 07, 2019  16:14:10
*/
int cfg_get_para(STRUCT_ONCE_SIMPLE *pterm_para)
{
	int iret = 0;
	char tempBuff[256] = {0};

	//流水号获取
	cfg_get_para_key_value(TARM_MAK, pterm_para->mak);

	return iret;
}

/**
 * @Author: hhl \@decard
 * @fn     	int cfg_set_para(void* pvKey, void* pvBuf);
 * @brief:	参数设置
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 18, 2019  18:08:18
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 18, 2019  18:08:18
*/
int cfg_set_para_key_value(void* pvKey, void* pvBuf)
{
	ConfigSetKey(config_file_path, CFG_PARA, pvKey, pvBuf);
}

/**
 * @Author: hhl \@decard
 * @fn     	int cfg_get_para(void* pvKey, void* pvBuf);
 * @brief:	参数获取
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 18, 2019  18:08:18
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 18, 2019  18:08:18
*/
int cfg_get_para_key_value (void* pvKey, void* pvBuf)
{
	ConfigGetKey(config_file_path, CFG_PARA, pvKey, pvBuf);
}


