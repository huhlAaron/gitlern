/*
 * file.c
 *	文件操作模块
 *  Created on: November 07, 2019  15:17:19
 *      Author: huhongliang@decard.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

//文件操作互斥锁
pthread_mutex_t inifile_mutex = PTHREAD_MUTEX_INITIALIZER;

/*******************hhl gb*********************/
#define LineMaxLen 512
#define KeyMaxLen 128
#define MaxFileLength 1024*10

/* 该函数作用是判断linebuf中是否存在 pKey 这里使用的完全匹配
stu=123
student=124
这里都有stu，substr函数功能就是排除这种情况的
*/
static char *substr(char *linebuf,char *pKey)
{
	char *pTmp = NULL, *pRv = NULL;
	int lenKey = 0,len = 0;
	if (linebuf==NULL || pKey==NULL)
	{
		printf("func parameter err: (str==NULL || substr==NULL)");
		return pRv;
	}

	//在该行寻找key
	pRv = strstr(linebuf, pKey);
	if(pRv == NULL)
	{
		return pRv;
	}

	if(memcmp(linebuf, pKey, strlen(pKey)) != 0)
	{
		return NULL;
	}
	
	pTmp = pRv;
	lenKey = strlen(pKey);
	while(*pTmp != '\0' && *pTmp != ' '&& *pTmp != '=' && *pTmp != '\n')
	{
		len++;
		pTmp++;
		if(len>lenKey)
		{
			break;
		}
	}
	if (lenKey != len)
	{
		return NULL;
	}

	return pRv;
}
//实现流程
//打开文件
//按照行的方式 循环读文件
//解析每一行，若匹配key关键字，在进行value值的提取
//提取value值需要 去除前后空格
int GetCfItem(const char *pFileName, char *pKey, char * pValue, int * pValueLen )
{
	int rv = 0;
	FILE *fp = NULL;
	char linebuf[LineMaxLen];
	char *pTmp = NULL, *pBegin = NULL, *pEnd = NULL;

	if (pFileName==NULL||pKey==NULL||pValue==NULL||pValueLen==NULL)
	{
		rv = -1;
		printf("func GetCfItem paramter err!");
		goto End;
	}
	fp = fopen(pFileName,"r");//以只读方式打开文件
	if (fp==NULL)
	{
		rv = -2;
		printf("fopen file err %d (fp==NULL)",rv);
		goto End;
	}

	while(!feof(fp))
	{
		memset(linebuf,0,LineMaxLen);
		pTmp = fgets(linebuf, LineMaxLen, fp);// 按行读取
		if (pTmp==NULL)
		{
			break;
		}

		pTmp = substr(linebuf, pKey);//判断key是否在该行
		if (pTmp==NULL)
		{
			continue;
		}
		pTmp = strchr(linebuf, '=');// 判断该行可有'='
		if (pTmp==NULL)
		{
			continue;
		}
		pTmp=pTmp+1;// 从等号后面开始

		while (1)//获取value 起点
		{
			if(*pTmp==' ')
			{
				pTmp++;
			}
			else
			{
				pBegin = pTmp;
				if(*pBegin == '\n'||*pBegin=='\0')
				{
					rv = -3;
					printf("The key %s don't have value",pKey);
				}
				break;
			}
		}

		while (1)//获取valude结束点
		{
			if(*pTmp==' '||*pTmp=='\n'||*pTmp=='\0'||*pTmp=='\r')
			{
				break;
			}
			else
			{
				pTmp++;
			}
		}
		pEnd = pTmp;
		*pValueLen = pEnd-pBegin;
		memcpy(pValue,pBegin,*pValueLen);//拷贝
		*(pValue+*pValueLen)='\0';
		break;
	}
	if(pBegin==NULL) // 没有找到key
	{
		printf("can't find the key:%s \n",pKey);
		rv = -4;
	}

End:
	if (fp!=NULL)
	{
		fclose(fp);
	}
	return rv;
}
//实现流程
//循环读每一行，检查key配置项是否存在 若存在修改对应value值
//若不存在，在文件末尾 添加 "key = value"
//难点：如何修改文件流中的值
int SetCfItem(const char *pFileName, char *pKey, char *pValue)
{
	int rv = 0;
	int length = 0,iFlag = 0;
	FILE *fp = NULL;
	char filebuf[MaxFileLength+1];
	char linebuf[LineMaxLen+1];
	char *pTmp = NULL;

	memset(filebuf, 0, MaxFileLength+1);

	if (pFileName==NULL||pKey==NULL||pValue==NULL)
	{
		rv = -1;
		printf("func WriteCfItem paramter err!");
		goto End;
	}
	//fp = fopen(pFileName,"r+");
	fp = fopen(pFileName,"a+");
	if (fp==NULL)
	{
		rv = -2;
		printf("fopen file err:%d",rv);
		goto End;
	}
	fseek(fp, 0L, SEEK_END);

	//获取文件长度;
	length = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (length > MaxFileLength)
	{
		rv = -3;
		printf("file too long unspport:%d",rv);
		goto End;
	}
	while (!feof(fp))
	{
		//读每一行
		memset(linebuf, 0, sizeof(linebuf));
		pTmp = fgets(linebuf, LineMaxLen, fp);
		if(pTmp==NULL)
		{
			break;
		}

		pTmp = substr(linebuf, pKey);
		if(pTmp==NULL)
		{
			strcat(filebuf, linebuf);
			continue;
		}
		else
		{
			sprintf(linebuf,"%s=%s\n", pKey, pValue);
			strcat(filebuf, linebuf);
			iFlag = 1;//若存在key
		}
	}
	//若不存在 追加
	if (iFlag == 0)
	{
		fprintf(fp,"%s=%s\n", pKey, pValue);
		printf("write:%s=%s\n", pKey, pValue);
	}
	else//若存在
	{
		if (fp!=NULL)
		{
			fclose(fp);
		}
		fp = fopen(pFileName,"w+");
		if (fp==NULL)
		{
			rv = -4;
			printf("fopen file err:%d",rv);
			goto End;
		}
		fputs(filebuf, fp);//写回文件
	}

End:
	if (fp!=NULL)
	{
		fclose(fp);
	}
	return rv;
}

/*******************hhl end*********************/

/**
 * @Author: hhl \@decard
 * @fn     	int ConfigGetKey( void* pvCfgFile, void* pvSection, void* pvKey, void* pvBuf );
 * @brief:	获得key的值
 * @param[in] :void *CFG_file　文件；void *section　项值；void *key　键值
 * @param[out]:
 * @return:
 * @Date:November 07, 2019  15:24:45
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 07, 2019  15:24:45
*/
int ConfigGetKey( void* pvCfgFile, void* pvSection, void* pvKey, void* pvBuf )
{
	int iRet = 0;
	int VluLen = 0;

	pthread_mutex_lock(&inifile_mutex);
	GetCfItem(pvCfgFile, pvKey, pvBuf, &VluLen);
	pthread_mutex_unlock(&inifile_mutex);
	
	return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int ConfigSetKey( void* pvCfgFile, void* pvSection, void* pvKey, void* pvBuf );
 * @brief:	设置key的值
 * @param[in] :void *CFG_file　文件；void *section　项值；void *key　键值；void *buf　key的值
 * @param[out]:
 * @return:
 * @Date:November 07, 2019  15:27:02
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 07, 2019  15:27:02
*/
int ConfigSetKey( void* pvCfgFile, void* pvSection, void* pvKey, void* pvBuf )
{
	int iRet2 = 0;

	pthread_mutex_lock(&inifile_mutex); 
	SetCfItem(pvCfgFile, pvKey, pvBuf);
	pthread_mutex_unlock(&inifile_mutex);
	return iRet2;


}

/**
 * @Author: hhl \@decard
 * @fn     	int config_file_exist_check(void *FilePath);
 * @brief:	检测文件是否存在，
 * access说明:检查调用进程是否可以对指定的文件执行某种操作
 * pathname: 需要测试的文件路径名。   
 * mode: 需要测试的操作模式，可能值是一个或多个R_OK(可读?), W_OK(可写?), X_OK(可执行?) 或 F_OK(文件存在?)组合体。
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:September 30, 2019  09:01:23
 * @Last Modified by:hhl \@decard
 * @Last Modified time:September 30, 2019  09:01:23
*/
int file_exist_check(void *FilePath)
{
	if((access((char *)FilePath, F_OK)) != -1)   
    {   
        printf("文件[%s]存在.\n", FilePath);  
		return 0;
    }   
    else  
    {   
        printf("文件[%s] 不存在!\n", FilePath);   
		return -1;
    }   
}


