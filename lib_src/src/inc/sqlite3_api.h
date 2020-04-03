/*****************************************************************************//**
*  - Copyright (C)  1.0, XXX Co. Ltd.
*  - All rights reserved.
*
*  @file     sqlite3_interface.h
*  @brief    描述当前文档
*
*  @author   zhangxiangquan \@decard
*  @date     2018年12月20日 星期四 11:35:46
*  @note     GNU General Public License (GPL) *
*  @remark   Description *
* - ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐*
* - Change History *
* - <Date> | <Version> | <Author> | <Description> *
* - 2018年12月20日 星期四 |  1.0 | zhangxiangquan \@decard | Create file *
* - ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐*
********************************************************************************/
#ifndef _SQLITE3_INTERFACE_H_
#define _SQLITE3_INTERFACE_H_
/***********************[本地应用头文件]*********************************/

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>


/***********************[宏定义]*****************************************/

/**********************************************************************************************//**
* @name   系统函数重定向
* @author jixiaolong \@decard
* @date   2019年3月5日 16:00:36
* @{
*/
#ifndef IN
#define IN
#define OUT
#define IO
#endif

#ifndef SNPRINTF
#ifdef _WIN_DEV_
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif
#endif
/** @} */

///<[jxl|2019年5月14日14:35:53] | 最长字符串长度
#define MAX_STR_LEN            3096


/**********************[结构体/全局变量定义]****************************/
//   加载一条交易明细的回调函数(返回0代表交易加载成功，其他代表加载失败)
//*  @param	[OUT] pValue         一条明细缓存
//*  @param	[IN]  iColnums       当前记录中有多少列
//*  @param	[IN]  pccColumValues 每一列的值       
//*  @param	[IN]  pccColumName   每一列的名称
typedef int  (*SQL_callback)(OUT void*pValue, IN int iColnums, IN char**pccColumValues, IN char**pccColumName);  /* Callback function */

//   加载N条交易明细的回调函数(返回0代表交易加载成功，其他代表加载失败)
//*  @param	[OUT] pValue         N条明细缓存
//*  @param	[IN]  iMaxRec        当前缓存中只能存储的最大记录条数 iMaxRec
//*  @param	[IN]  iRows          从数据库加载到 iRows 条记录
//*  @param	[IN]  iColnums       每条记录中有 iColnums 列
//*  @param	[IN]  pccColumValues 每一列的值       
//*  @param	[IN]  pccColumName   每一列的名称
typedef int (*SQL_callbacks)(OUT void*pValue, IN int iMaxRec, IN int iRows, IN int iColnums,char**pccColumValues, char**pccColumName);  /* Callback function  */

//   检查数据库某个表结构是否正常
//*  @param	[IN]  pccPath        数据库文件路径
//*  @param	[IN]  pccTable       表名称
typedef int (*SQL_Check) (IN const char* pccPath, IN const char *pccTable);

//   创建数据库的函数
typedef int (*SQL_Creat) (void);
/** @} */

typedef struct SQL_INSERT_struct
{
	IN void *pvMutexDB;// 互斥锁
	IN char *pchDbPath;// 路径
	IN char *pchTable; // 表名
	IN char *pchExeSql;// SQL语句
	IN SQL_Creat fCreate;// 创建数据库结构
}SQL_INSERT_T;

typedef struct SQL_HISTORY_struct
{
	IN void *pvMutexDB; // 互斥锁
	IN char *pchDbPath; // 路径
	IN char *pchTable;  // 表名
	IN char *pchSqlWhere; // 条件
}SQL_HISTORY_T;

/**********************[结构体/全局变量引用声明]************************/

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/ 
	
	/**********************************************************************************************//**
	*  @fn		 void SQL_ModelInit(IN char chLogSwitch, IN char chTimeSwitch, char *pchDbDirPath, char *pchProjectId, char *pchTmpPath);
	*  @brief	 数据库模块初始化
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年11月7日 18:01:46
	*  @param	[in] chLogSwitch   日志开关
	*  @param	[in] chTimeSwitch  时间耗时开关
	*  @param	[in] pchDbDirPath  历史交易数据库路径（文件夹目录）末尾不包含‘/’，如/home/root
	*  @param	[in] pchProjectId  项目编号如：XA00-XA01-20170160
	*  @param	[in] pchTmpPath    临时文件如：/home/root/temp
	*  @return	 -	void
	*  @note	(描述需要注意的问题) 
	*			 - 1.开机必须设置一次
	***************************************************************************************************/
	void SQL_ModelInit(IN char chLogSwitch, IN char chTimeSwitch, IN char *pchDbDirPath, IN char *pchProjectId, IN char *pchTmpPath);
	
	/**********************************************************************************************//**
	*  @fn		 uint64_t SQL_ToolGetFileSize(IN const char * pucFilePath);
	*  @brief	 获取文件的大小
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月28日 星期五 15:16:52
	*  @param	[in] pccFilePath 文件路径
	*  @return	 - >=0 文件大小（byte）
	*			 - <0  文件处理失败
	***************************************************************************************************/
	uint64_t SQL_ToolGetFileSize(IN const char * pccFilePath);
	/**********************************************************************************************//**
	*  @fn		 long long int SQL_ToolGetDirSize(IN char *pchDir);
	*  @brief	 获取文件夹所占空间的大小(计算某目录所占空间大小（包含本身的4096Byte）)
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月16日 11:42:28
	*  @param	[in]  pchDir 文件夹路径
	*  @return	 -	  >= 0 字节大小
	*			 -	  <  0 文件夹路径有错误
	***************************************************************************************************/
	long long int SQL_ToolGetDirSize(IN char *pchDir);
	/**********************************************************************************************//**
	*  @fn		 int SQL_GetFileList(IN char *pchBasePath, IN int iMaxFiles, OUT char pFileName[][256], OUT int *piFileNum);
	*  @brief	 获取文件夹下文件列表
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月15日 19:00:28
	*  @param	[in] pchBasePath 基本路径
	*  @param	[in] iMaxFiles	  最大的文件数量
	*  @param	[out] pFileName 文件名称的集合输出
	*  @param	[out] piFileNum 遍历的数量
	*  @return	 - 0 成功
	*			 - 1 失败
	***************************************************************************************************/
	int SQL_GetFileList(IN char *pchBasePath, IN int iMaxFiles, OUT char pFileName[][256], OUT int *piFileNum);
		
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_ToolIsExists(IN const char *pccFilePath);
	*  @brief	 检查文件是否存在
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月28日 星期五 15:15:21
	*  @param	[in] pccFilePath 文件路径
	*  @return	 - 0 不存在
	*			 - 1 存在
	***************************************************************************************************/
	int SQL_ToolIsExists(IN const char *pccFilePath);
	
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SHELL(const char *command);
	*  @brief	 执行system命令
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 17:06:22
	*  @param	[in] pccCommand 命令
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SHELL(IN const char *pccCommand);
	
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_ExSql(IN void * pvDbPath,  IN void * pvSqlStr);
	*  @brief	 描述当前文档
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 16:55:18
	*  @param	[in] pvDbPath		   数据库完整路径
	*  @param	[in] pvSqlStr		   当前导出数据的sql语句
	*  @return	 - 非0执行失败
	***************************************************************************************************/
	int SQL_ExSql(IN void * pvDbPath,  IN void * pvSqlStr);
	
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_BakExSql(IN void * pvDbPath,  IN void * pvSqlStr);
	*  @brief	 描述当前文档
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 16:55:18
	*  @param	[in] pvDbPath		   数据库完整路径
	*  @param	[in] pvSqlStr		   当前导出数据的sql语句
	*  @return	 - 非0执行失败
	***************************************************************************************************/
	int SQL_BakExSql(IN void * pvDbPath,  IN void * pvSqlStr);
	
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_ExportTable(IN void * pvDbPath,  IN void * pvSqlStr, IN void * pvTempFilePath);
	*  @brief	 描述当前文档
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 16:55:18
	*  @param	[in] pvDbPath		   数据库完整路径
	*  @param	[in] pvSqlStr		   当前导出数据的sql语句
	*  @param	[in] pvTempFilePath    中间文件
	*  @return	 - 非0执行失败
	***************************************************************************************************/
	int SQL_ExportTable(IN void * pvDbPath,	IN void * pvSqlStr, IN void * pvTempFilePath);
	
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_ImportTable(IN void *pvDbPath, IN void * pvTempFilePath,  IN void * pvTableName);
	*  @brief	 描述当前文档
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 16:55:14
	*  @param	[in] pvDbPath		   数据库完整路径
	*  @param	[in] pvTableName	   表名
	*  @param	[in] pvTempFilePath    中间文件
	*  @return	 - 非0执行失败
	***************************************************************************************************/
	int SQL_ImportTable(IN void *pvDbPath, IN void * pvTempFilePath,  IN void * pvTableName);
	
	
	/**********************************************************************************************//**
	*  @fn		 void SQL_GetHistoryName(OUT char *pchHistory);
	*  @brief	 获取历史数据库的名称
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月16日 19:01:01
	*  @param	[out] pchHistory 备份数据库名称以 XXXX_201904.db命名
	*  @return	 -	  void
	***************************************************************************************************/
	void SQL_GetHistoryName(OUT char *pchHistory);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_Close(void);
	*  @brief	 数据库端口关闭
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 10:49:28
	*  @param	[io] void
	*  @return	 -	 非0失败
	*			 -
	***************************************************************************************************/
	int SQL_Close(void);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_Open(char *pchDbPathName);
	*  @brief	 数据库打开
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	[in]  pchDbPathName 数据库的完整路径
	*  @return	  -   非0失败
	***************************************************************************************************/
	int SQL_Open(char *pchDbPathName);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_ExecOneSql(IN void* pvDb, IN char * pchExeSql);
	*  @brief	 执行一条SQL语句后关闭数据库
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @return	 -失败，-1
	*			 -成功，返回数据处理所在行
	*
	*  @see   ?? Test();（本函数参考其它的相关的函数，这里作一个链接）
	*  @note	 (描述需要注意的问题);
	*			  - 1. SQL_Open 需要配套使用
	*			  - 2. SQL_Close 需要配套使用
	***************************************************************************************************/
	int SQL_ExecOneSql(IN char * pchExeSql);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_BeginTransaction(void);
	*  @brief	 开始事物处理
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 11:28:10
	*  @param	[io] void
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_BeginTransaction(void);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_EndCommit(void);
	*  @brief	 事物提交
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 11:28:12
	*  @param	[io] void
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_EndCommit(void);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_ExecOneSqlLoadData(IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
	*  @brief	 执行SQL语句查询记录
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 15:20:36
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @param	 [in] LoadData	处理加载数据的回调函数（必现有返回值，且返回为0时加载才成功）
	*  @param	 [io] pPara 	加载数据的回调函数的第一个入参，会被清0
	*  @param	 [io] pPara 	参数长度
	*  @return	 - 非0失败
	*			 -
	***************************************************************************************************/
	int SQL_ExecOneSqlLoadData(IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_OneCount(IN char *pchSqlStr);
	*  @brief	 执行sq语句,返回对应的语句数目
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	 [in] pchDbName  数据库的命子
	*  @param	 [in] pchSqlStr  SQL语句执行 select * from xxx;(select 后面不能跟统计类函数，负责无效);
	*  @return	 - 失败，-1
	*			 - 成功，SQL执行后对应的行数
	***************************************************************************************************/
	int SQL_OneCount(IN char *pchSqlStr);
		
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_Exec_Sql(char* pchDbPath, char * exeSql);
	*  @brief	 执行sql语句
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	 [in] pchDbPath 数据库的路径
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @param	 [in] iTrialNum 尝试次数
	*  @return	 -失败，-1
	*			 -成功，返回数据处理所在行
	***************************************************************************************************/
	int SQL_Exec_Sql(IN char* pchDbPath,IN char * pchExeSql);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_CheckHistory(IN const char* pccDbPath, IN const char *pccTable);
	*  @brief	 校验历史数据库表的结构完整性（若异常时则删除）
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月15日 15:20:23
	*  @param	[in] pccTable  表名
	*  @param	[in] fCreate   创建数据库表结构的执行函数
	*  @return	 -	 非0数据库异常
	***************************************************************************************************/
	int SQL_CheckHistory(IN const char *pccTable, IN SQL_Creat fCreate);
	
	
	/**********************************************************************************************//**
	*  @fn		 static int SQL_CheckAndRecovery(IN const char* pccDbPath, IN const char *pccTable);
	*  @brief	 校验主数据库表的结构完整性（若异常时用备份数据库恢复）
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月15日 15:20:23
	*  @param	[in] pccDbPath 主数据库文件路径
	*  @param	[in] pccTable  表名
	*  @param	[in] fCreate   创建数据库表结构的执行函数
	*  @return	 -	 非0数据库异常
	***************************************************************************************************/
	int SQL_CheckAndRecovery(IN const char* pccDbPath, IN const char *pccTable, IN SQL_Creat fCreate);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_TimerCRB(IN const char* pccDbPath, IN const char *pccTable);
	*  @brief	 定时校验主数据库表的结构完整性（若异常时用备份数据库恢复,若正常则备份）
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月15日 15:20:23
	*  @param	[in] pvMutexDB 互斥锁
	*  @param	[in] pccDbPath 主数据库文件路径
	*  @param	[in] pccTable  表名
	*  @param	[in] fCreate   创建数据库表结构的执行函数
	*  @return	 -	 非0数据库异常
	***************************************************************************************************/
	int SQL_TimerCRB(IN void* pvMutexDB, IN const char* pccDbPath, IN const char *pccTable, IN SQL_Creat fCreate);
		
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_MoveToHistory(IN SQL_HISTORY_T *pstHistory);
	*  @brief	 将已上送的明细上送到历史表中，将数据进行备份
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月27日 21:00:59
	*  @param	[in] stHistory 历史交易明细获取
	*  @param	[in] pccTable  表名
	*  @return	 -
	***************************************************************************************************/
	int SQL_MoveToHistory(IN SQL_HISTORY_T *pstHistory);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_Insert(IN SQL_INSERT_T *pstInsert);
	*  @brief	 交易记录存储
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年4月30日 11:00:21
	*  @param	[in] stInsert 
	*  @return	 - 非0失败
	***************************************************************************************************/
	int SQL_Insert(IN SQL_INSERT_T *pstInsert);
		
	/**********************************************************************************************//**
	*  @fn		 int SQL_Select(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
	*  @brief	 执行SQL语句查询记录
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 15:20:36
	*  @param	 [in] pvMutexDB 互斥锁
	*  @param	 [in] pchDbPath 数据库路径
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @param	 [in] LoadData	处理加载数据的回调函数（必现有返回值，且返回为0时加载才成功）(可传NULL)
	*  @param	 [io] pPara 	加载数据的回调函数的第一个入参(可传NULL)
	*  @param	 [io] iLen		参数pPara长度
	*  @return	 -	0 成功
	*			 - -1 无交易
	*			 - >0 SQL_API执行失败
	***************************************************************************************************/
	int SQL_Select(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_Select(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
	*  @brief	 执行SQL语句查询记录
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 15:20:36
	*  @param	 [in] pvMutexDB 互斥锁
	*  @param	 [in] pchDbPath 数据库路径
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @param	 [in] LoadData	处理加载数据的回调函数（必现有返回值，且返回为0时加载才成功）(可传NULL)
	*  @param	 [io] pPara 	加载数据的回调函数的第一个入参(可传NULL)加载前不会清零
	*  @param	 [io] iLen		参数pPara长度
	*  @return	 -	0 成功
	*			 - -1 无交易
	*			 - >0 SQL_API执行失败
	***************************************************************************************************/
	int SQL_Select1(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, OUT SQL_callback LoadData, IO void *pPara, IN int iLen);
			
	/**********************************************************************************************//**
	*  @fn		 int SQL_SelectS(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, OUT SQL_callbacks LoadDatas, IO void **pPara, IN int iMaxRec);
	*  @brief	 执行SQL语句查询记录(一次性可以加载多笔交易记录)
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年3月13日 15:20:36
	*  @param	 [in] pvMutexDB 互斥锁
	*  @param	 [in] pchDbPath 数据库路径
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @param	 [in] LoadData	处理加载数据的回调函数（必现有返回值，且返回为0时加载才成功）(可传NULL)
	*  @param	 [out] pPara	加载数据的回调函数的第一个入参(可传NULL)加载前不会清零
	*  @param	 [in] iMaxRec	加载的最大交易笔数
	*  @param	 [out] piRecNum 实际的交易笔数
	*  @return	 -	0 成功
	*			 - -1 无交易
	*			 - >0 SQL_API执行失败
	***************************************************************************************************/
	int SQL_SelectS(IN void *pvMutexDB, IN char* pchDbPath, IN char * pchExeSql, IN SQL_callbacks LoadDatas, OUT void *pPara, IN int iMaxRec, OUT int *piRecNum );
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_Update(IN void *pvMutexDB, IN char* pchDbPath,IN char * pchExeSql);
	*  @brief	 执行sql语句
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	 [in] pvMutexDB 互斥锁
	*  @param	 [in] pchDbPath 数据库的路径
	*  @param	 [in] pchExeSql 数据的SQL语句
	*  @return	 -失败，-1
	*			 -成功，返回数据处理所在行
	***************************************************************************************************/
	int SQL_Update(IN void *pvMutexDB, IN char* pchDbPath,IN char * pchExeSql);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_Count(IN void *pvMutexDB, IN char* pchDbPath,IN char * pchExeSql);
	*  @brief	 执行sq语句,返回对应的语句数目
	*
	*  @author	 zxq \@decard
	*  @date	 2018年12月18日 星期二 17:14:13
	*  @param	 [in] pvMutexDB  互斥锁
	*  @param	 [in] pchDbName  数据库的命子
	*  @param	 [in] pchSqlStr  SQL语句执行 select * from xxx;(select 后面不能跟统计类函数，负责无效)
	*  @return	 - 失败，-1
	*			 - 成功，SQL执行后对应的行数
	***************************************************************************************************/
	int SQL_Count(IN void *pvMutexDB, IN char* pchDbPath,IN char * pchExeSql);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SaveCupPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储银联参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SaveCupPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SaveOdaPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储银联参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SaveOdaPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SavePostPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储POS通参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SavePostPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SavePospPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储POSP参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SavePospPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SaveTransitPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储公共特有参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SaveTransitPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_SavePublicPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 存储公共参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:09:34
	*  @param	[in] pstBasePara 参数ASC码
	*  @param	[in] iParaLen	 参数长度
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_SavePublicPara(IN void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadCupPara(IN void *pstBasePara, IN int iParaLen);
	*  @brief	 加载银联参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadCupPara(OUT void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadOdaPara(OUT void *pstBasePara, IN int iParaLen);
	*  @brief	 加载ODA参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadOdaPara(OUT void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadCupPara(OUT void *pstBasePara, IN int iParaLen);
	*  @brief	 加载POST参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadPostPara(OUT void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadPospPara(OUT void *pstBasePara, IN int iParaLen);
	*  @brief	 加载POSP参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadPospPara(OUT void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadTransitPara(OUT void *pstBasePara, IN int iParaLen);
	*  @brief	 加载公共特有参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadTransitPara(OUT void *pstBasePara, IN int iParaLen);
	
	/**********************************************************************************************//**
	*  @fn		 int SQL_LoadPublicPara(OUT void *pstBasePara, IN int iParaLen);
	*  @brief	 加载公共参数
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 14:14:11
	*  @param	[OUT] pstBasePara 参数ASC码
	*  @param	[in]  iParaLen	  参数长度
	*  @return	 -	  非0失败
	***************************************************************************************************/
	int SQL_LoadPublicPara(OUT void *pstBasePara, IN int iParaLen);
	/**********************************************************************************************//**
	*  @fn		 int SQL_ParaCheck(void);
	*  @brief	 基础数据库的创建,检测和备份
	*  
	*  @author	 jixiaolong \@decard
	*  @date	 2019年5月14日 15:31:32
	*  @param	[io] void
	*  @return	 -	 非0失败
	***************************************************************************************************/
	int SQL_ParaCheck(void);
	
#ifdef __cplusplus
}

#endif /*__cplusplus*/ 


#endif /*sqlite3_interface.h*/
/**************************************************************************
**********************************【 sqlite3_interface.h 结束】*******************
*************************************************************************
*/


