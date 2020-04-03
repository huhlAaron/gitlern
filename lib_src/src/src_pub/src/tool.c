#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <memory.h>
#include <string.h>

/**
 * @Author: hhl \@decard
 * @fn     	void MyHexA(unsigned char* hex, unsigned char* a, int length);
 * @brief:	hex->asc
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  15:34:52
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  15:34:52
*/
void MyHexA(unsigned char* hex, unsigned char* a, int length)
{
    unsigned char hLowbit, hHighbit;
    int i;
    for (i = 0; i < length *2; i += 2)
    {
        hLowbit = hex[i / 2] & 0x0f;
        hHighbit = hex[i / 2] / 16;
        if (hHighbit >= 10)
            a[i] = hHighbit + '7';
        else
            a[i] = hHighbit + '0';
        if (hLowbit >= 10)
            a[i + 1] = hLowbit + '7';
        else
            a[i + 1] = hLowbit + '0';
    }
    a[length * 2] = '\0';
}

/**
 * @Author: hhl \@decard
 * @fn     	int MyAHex(unsigned char* a, unsigned char* hex, int len);
 * @brief:	asc->hex
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  15:35:07
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  15:35:07
*/
int MyAHex(unsigned char* a, unsigned char* hex, int len)
{
    int i;
    unsigned char aLowbit, aHighbit;
    unsigned char hconval, lconval;
    for (i = 0; i < len; i += 2)
    {
        aHighbit = toupper(a[i]);
        if ((aHighbit >= 'A') && (aHighbit <= 'F'))
            hconval = '7';
        else if ((aHighbit >= '0') && (aHighbit <= '9'))
            hconval = '0';
        else
            return  -1;
        aLowbit = toupper(a[i + 1]);
        if ((aLowbit >= 'A') && (aLowbit <= 'F'))
            lconval = '7';
        else if ((aLowbit >= '0') && (aLowbit <= '9'))
            lconval = '0';
        else
            return  -1;
        hex[(i / 2)] = ((aHighbit - hconval) * 16 + (aLowbit - lconval));
    }
    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	int TimestampToTime(time_t timeStamp, char *pTime);
 * @brief:	//时间戳 转 北京时间 1513076245 -> 2017年12月12日18时57分25秒
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  19:07:29
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  19:07:29
*/
int TimestampToTime(time_t timeStamp, char *pTime)
{
	time_t t;
    struct tm *p;

    t = timeStamp/* + 28800*/;//UTC时间转CST时间
    //p = gmtime(&t);//gmtime只是获取的时间未经过时区转换
    p = localtime(&t);
	
    char s[100] = {0};
    //strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);
    strftime(s, sizeof(s), "%Y%m%d%H%M%S", p);
    Uart_Printf("%d: %s\n", (int)t, s);

	memcpy(pTime, s, 14);
	pTime[14] = '\0';

    return 0;
}


/**
 * @Author: hhl \@decard
 * @fn     	int TimeToTimestamp(char *pTime, time_t *pTimestamp);
 * @brief:	//时间 转 时间戳 2017年12月12日10时57分25秒 -> 1513076245
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  19:07:40
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  19:07:40
*/
int TimeToTimestamp(char *pTime, time_t *pTimestamp)
{
    struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));
    //strptime("20131120000000","%Y%m%d%H%M%S",tmp_time);
    strptime(pTime,"%Y%m%d%H%M%S",tmp_time);
    time_t t = mktime(tmp_time);//注意，mktime带时区
    Uart_Printf("%s : %ld\n", pTime, t);
    free(tmp_time);

    *pTimestamp = t;
    return 0;
}

/**
 * @Author: hhl \@decard
 * @fn     	long time_lag(unsigned char *pucSecond, unsigned char *pucFirst);
 * @brief:	计算时间差
 * @param[in] :
 * @param[out]:
 * @return:返回时间差(s)
 * @Date:May 07, 2019  15:26:33
 * @Last Modified by:hhl \@decard
 * @Last Modified time:May 07, 2019  15:26:33
*/
long time_lag( unsigned char *pucFirst, unsigned char *pucSecond)
{
    long ulTimeLagSum = 0;
    time_t Time1;
    time_t Time2;

    //string_to_time_t(pucFirst,  &Time1);
    //string_to_time_t(pucSecond, &Time2);
	TimeToTimestamp(pucFirst,  &Time1);
    TimeToTimestamp(pucSecond, &Time2);

    ulTimeLagSum = difftime(Time2, Time1);
    Uart_Printf("pucSecond (%s)\n", pucSecond);
    Uart_Printf("pucFirst  (%s)\n", pucFirst);
    Uart_Printf("ulTimeLagSum (%ld)\n", ulTimeLagSum);

    return (ulTimeLagSum);
}



/**********************************************************************************************//**
*  @fn       void Api_ToolGetDays(IN char *pchToday, IN int iDays, OUT char *pchOutDate);
*  @brief    根据传入的日期获取几天之前的日期是多少号
*  
*  @author   jixiaolong \@decard
*  @date     2018年12月29日 12:00:23
*  @param   [in]  pchToday  当前日期
*  @param   [in]  iDays     天数
*  @param   [out] pchOutDate计算获取到的日期
*  @return   - void
***************************************************************************************************/
void Api_ToolGetDays(char *pchToday, int iDays, char *pchOutDate)
{
	time_t tTimes = 0;
	
	TimeToTimestamp(pchToday, &tTimes);

	tTimes -= iDays * 24 * 60 * 60;
	
	TimestampToTime((unsigned long)tTimes, pchOutDate);

	Uart_Printf("距离今天【%s】的【%d】天前是【%s】\n", pchToday, iDays, pchOutDate);
}



