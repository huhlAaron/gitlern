/*
 * IF_UART.h
 *
 *  Created on: November 04, 2019  17:11:46
 *      Author: huhongliang@decard.com
 */

#ifndef _IF_UART_H
#define _IF_UART_H

#include "../../src_psam/inc/sam_card.h"

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
void Uart_SetDebug(char ucFlag);

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
void Uart_SetLogPath(char *path);

#endif 
