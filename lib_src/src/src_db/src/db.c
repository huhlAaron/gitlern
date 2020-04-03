/*
 * xc_mot_db.c
 *	许昌城际公交数据库操作模块
 *  Created on: November 07, 2019  18:37:03
 *      Author: huhongliang@decard.com
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "data_type.h"
#include "main_api.h"

pthread_mutex_t db_trade_mutex = PTHREAD_MUTEX_INITIALIZER;	//交易数据库互斥锁
pthread_mutex_t db_para_mutex = PTHREAD_MUTEX_INITIALIZER;	//参数数据库互斥锁

//交易数据库路径
static char db_trade_path[256] = {0};

//交易数据库名称
static char xc_city_bus_trade_db[64] = "dc_ploymerize_pay_trade.db";

//交易表
static char Table_Mot_Trade[64] = "TRADE_TABLE";


//参数数据库路径
static char db_para_path[256] = {0};

//参数数据库名称
static char xc_city_bus_para_db[64] = "xc_city_bus_para.db";

//交通部黑名单表
static char Table_Mot_Black[64] = "MOT_BLACK_TABLE";

//交通部白名单表
static char Table_Mot_White[64] = "MOT_WHITE_TABLE";

//卡类型参数
static char Table_Mot_CardType[64] = "MOT_CARD_TYPE_TABLE";

/**
 * @Author: hhl \@decard
 * @fn     	void db_set_path(char *db_path);
 * @brief:	数据库路径设置
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 12, 2019  17:38:24
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 12, 2019  17:38:24
*/
void db_set_path(char *pdb_path)
{
	//交易数据库
	memset(db_trade_path, 0, sizeof(db_trade_path));
	sprintf(db_trade_path, "%s/%s", pdb_path, xc_city_bus_trade_db);

	//参数数据库
	memset(db_para_path, 0, sizeof(db_para_path));
	sprintf(db_para_path, "%s/%s", pdb_path, xc_city_bus_para_db);
}

/**
 * @Author: hhl \@decard
 * @fn     	sqlite3 *db_open(char *db_name);
 * @brief:	打开数据库
 * @param[in] :
 * @param[out]:
 * @return:	返回sqlite3 指针 数据库句柄
 * @Date:May 24, 2019  11:17:07
 * @Last Modified by:hhl \@decard
 * @Last Modified time:May 24, 2019  11:17:07
*/
sqlite3 *db_open(char *db_name)
{
	char sqlite3_errmsg_buf[1024] = "\0";
	int res;
	sqlite3 *db = NULL;
	res = sqlite3_open(db_name, &db);
	if (res)
	{
		memset(sqlite3_errmsg_buf, 0x00, sizeof(sqlite3_errmsg_buf));
		strcpy(sqlite3_errmsg_buf, sqlite3_errmsg(db));
		if (NULL != db)
			sqlite3_close(db);
		return NULL ;
	}

	return db;
}

/**
 * @Author: hhl \@decard
 * @fn     	int db_exec_sql(char* pchDbPath, char * exeSql);
 * @brief:	执行sq语句.含事物
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:May 24, 2019  11:14:59
 * @Last Modified by:hhl \@decard
 * @Last Modified time:May 24, 2019  11:14:59
*/
int db_exec_sql(char* pchDbPath, char * exeSql)
{
	sqlite3 *P16Db = NULL;
	char *zErrMsg = 0;
	int rc = 0;
	int ret = 0;
	int n;
	rc = sqlite3_open(pchDbPath, &P16Db);
	if (rc)
	{
		Uart_Printf("Can't open database: %s\n", sqlite3_errmsg(P16Db));
		sqlite3_close(P16Db);
		return -1;
	}

	Uart_Printf("sq : %s\n", exeSql);
	ret = sqlite3_exec(P16Db, "begin transaction", 0, 0, 0);
	Uart_Printf("begin transaction : %d\n", ret);

	rc = sqlite3_exec(P16Db, exeSql, 0, 0, &zErrMsg);
	Uart_Printf("sqlite3_exec : %d\n", rc);

	ret = sqlite3_exec(P16Db, "commit transaction", 0, 0, 0);
	Uart_Printf("commit transaction : %d\n", ret);

	if (rc != SQLITE_OK)
	{
		Uart_Printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(P16Db);
		return -1;
	}

	n = sqlite3_last_insert_rowid(P16Db);

END:
	sqlite3_free(zErrMsg);
	sqlite3_close(P16Db);

	return n;
}

/**
 * @Author: hhl \@decard
 * @fn     	int db_exec_sql_without_transaction(char* pchDbPath, char * exeSql);
 * @brief:	sq执行不含事务
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 12, 2019  17:35:12
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 12, 2019  17:35:12
*/
int db_exec_sql_without_transaction(char* pchDbPath, char * exeSql)
{
	sqlite3 *P16Db = NULL;
	char *zErrMsg = 0;
	int rc = 0;
	int ret = 0;
	int n;
	rc = sqlite3_open(pchDbPath, &P16Db);
	if (rc)
	{
		Uart_Printf("Can't open database: %s\n", sqlite3_errmsg(P16Db));
		sqlite3_close(P16Db);
		return -1;
	}
	rc = sqlite3_exec(P16Db, exeSql, 0, 0, &zErrMsg);
	Uart_Printf("sqlite3_exec : %d\n", rc);

	if (rc != SQLITE_OK)
	{
		Uart_Printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(P16Db);
		return -1;
	}

	n = sqlite3_last_insert_rowid(P16Db);

END:
	sqlite3_free(zErrMsg);
	sqlite3_close(P16Db);

	return n;
}

/**
 * @Author: hhl \@decard
 * @fn     	int db_trade_decard_ploymerize_pay(DB_OPR_TYPE OprType, decard_ploymerize_pay *pDecardPloymerizePay_st);
 * @brief:	德卡聚合支付交易表操作
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:December 18, 2019  11:48:09
 * @Last Modified by:hhl \@decard
 * @Last Modified time:December 18, 2019  11:48:09
*/
int db_trade_decard_ploymerize_pay(DB_OPR_TYPE OprType, decard_ploymerize_pay *pDecardPloymerizePay_st)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	static sqlite3 *Transedb = NULL;

	int nRow = 0, nColumn = 0;
	char **dbResult = NULL;

	char Sq[1024+1] = {0};

	char tableName[256] = {0};
	char DataBaseNume[512] = {0};

	char ClrDateTime[15] = {0};
	char NeedClrDateTime[15] = {0};

	memset(DataBaseNume, 0, sizeof(DataBaseNume));
	strcpy(DataBaseNume, db_trade_path);

	memset(tableName, 0, sizeof(tableName));
	strcpy(tableName, Table_Mot_Trade);

	//互斥锁上锁
	pthread_mutex_lock(&db_trade_mutex);

	/*
		scene	VARCHAR(32),\	//支付场景
		qrcode	VARCHAR(64),\	//二维码数据
		body  VARCHAR(64),\		//商品名称
		totalFee  VARCHAR(12),\	//支付金额
		terminalIp	VARCHAR(20),\	//终端ip
		outTradeNo	VARCHAR(32),\	//商户订单号（唯一的订单流水号）
		terminalParams	VARCHAR(64),\	//终端其他信息
		model	VARCHAR(64),\		//终端型号
		sn	VARCHAR(32),\			//终端sn
		shop_num VARCHAR(64),\		//商户编号(商户项目编号)
		openId	VARCHAR(64),\		//用户标识
		payType VARCHAR(64),\		//支付分类
		pay_type	VARCHAR(64),\	//支付类型
		timeEnd VARCHAR(32),\		//支付时间
		transactionId	VARCHAR(64),\	//交易流水号
		out_refund_no	NVARCHAR(32),\	//退款流水号（唯一）
		state	NVARCHAR(6),\			//交易状态:成功/退货
		upLoadFlag VARCHAR(6),\			//上送状态

		filed0	 VARCHAR(20),\
		filed1	 VARCHAR(20)
	*/

	//增
	if(OprType == DB_INSERT)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq,"INSERT INTO %s VALUES(NULL,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s', NULL ,NULL)",
				tableName,
				pDecardPloymerizePay_st->scene,
				pDecardPloymerizePay_st->qrcode,
				pDecardPloymerizePay_st->body,
				pDecardPloymerizePay_st->totalFee,
				pDecardPloymerizePay_st->terminalIp,
				pDecardPloymerizePay_st->outTradeNo,
				pDecardPloymerizePay_st->terminalParams,
				pDecardPloymerizePay_st->model,
				pDecardPloymerizePay_st->sn,
				pDecardPloymerizePay_st->shop_num,
				pDecardPloymerizePay_st->openId,
				pDecardPloymerizePay_st->payType,
				pDecardPloymerizePay_st->pay_type,
				pDecardPloymerizePay_st->timeEnd,
				pDecardPloymerizePay_st->transactionId,
				pDecardPloymerizePay_st->out_refund_no,
				pDecardPloymerizePay_st->state,
				pDecardPloymerizePay_st->upLoadFlag);
		Uart_Printf( "chaSqlStr is %s\n", Sq);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if (iRet == -1)
		{
			goto END;
		}
	}
	//删
	else if(OprType == DB_DELETE)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "delete from %s", tableName);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}

	}
	//查(未上送)
	else if(OprType == DB_SELECT)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			goto END;
		}

		//查询
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "select * from %s where state='%s' and outTradeNo='%s' limit 1", tableName, pDecardPloymerizePay_st->state, pDecardPloymerizePay_st->outTradeNo);
		iRet = sqlite3_get_table(Transedb, Sq, &dbResult, &nRow, &nColumn, &zErrMsg);
		Uart_Printf("sqlite3_get_table[%d]:%s\n", iRet, Sq);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			Uart_Printf("共 %d 记录\n", nRow);
			Uart_Printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
					}
					Uart_Printf("\n");
				}

				memset(pDecardPloymerizePay_st, 0, sizeof(decard_ploymerize_pay));
				index ++;
				strcpy(pDecardPloymerizePay_st->scene, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->qrcode, 		dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->body, 		dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->totalFee, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->terminalIp, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->outTradeNo, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->terminalParams, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->model, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->sn, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->shop_num, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->openId, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->payType, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->pay_type, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->timeEnd, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->transactionId, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->out_refund_no, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->state, dbResult[index++]);
				strcpy(pDecardPloymerizePay_st->upLoadFlag, dbResult[index++]);
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = nRow;
			goto END;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", Sq, zErrMsg);

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = -1;
			goto END;
		}
	}
	//改
	else if(OprType == DB_UPDATE)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "update %s set state='%s', out_refund_no='%s' where qrcode='%s' and outTradeNo='%s' and transactionId='%s'", tableName, pDecardPloymerizePay_st->state, pDecardPloymerizePay_st->out_refund_no, pDecardPloymerizePay_st->qrcode, pDecardPloymerizePay_st->outTradeNo, pDecardPloymerizePay_st->transactionId);
		Uart_Printf("Sq : %s\n", Sq);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}
	}
	else if(OprType == DB_REUPLOAD_CLR)
	{
		memset(ClrDateTime, 0, sizeof(ClrDateTime));
		memset(NeedClrDateTime, 0, sizeof(NeedClrDateTime));
		
		//计算需要清理的时间
		GetLocDate(ClrDateTime);
		Api_ToolGetDays(ClrDateTime, 30 * 3, NeedClrDateTime);
		
		//清理交易表
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "delete from %s where timeEnd < '%s' ", tableName, NeedClrDateTime);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}
	
		//数据库瘦身
		memset(Sq, 0, sizeof(Sq));
		strcpy(Sq, "VACUUM");
		Uart_Printf("chaSqlStr : %s\n", Sq);
		db_exec_sql_without_transaction(DataBaseNume, Sq);
	}
	
	else
	{
		Uart_Printf("database_cpu_trade_table_opr's OprType Err\n");
	}
	
END:
	//互斥锁释放
	pthread_mutex_unlock(&db_trade_mutex);
	
	return iRet;

}


/**
 * @Author: hhl \@decard
 * @fn     	int db_trade_mot_table_opr(DB_OPR_TYPE OprType, StTradeInfor *ST_TrdeLog);
 * @brief:	互联互通交易数据表操作
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  10:14:25
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  10:14:25
*/
int db_trade_mot_table_opr(DB_OPR_TYPE OprType, StTradeInfor *ST_TrdeLog)
{
	
	int iRet = 0;
	char *zErrMsg = NULL;
	static sqlite3 *Transedb = NULL;

	int nRow = 0, nColumn = 0;
	char **dbResult = NULL;

	char Sq[1024+1] = {0};

	char tableName[256] = {0};
	char DataBaseNume[512] = {0};

	char ClrDateTime[15] = {0};
	char NeedClrDateTime[15] = {0};

	memset(DataBaseNume, 0, sizeof(DataBaseNume));
	strcpy(DataBaseNume, db_trade_path);

	memset(tableName, 0, sizeof(tableName));
	strcpy(tableName, Table_Mot_Trade);

	//互斥锁上锁
	pthread_mutex_lock(&db_trade_mutex);

	/*
		ID INTEGER PRIMARY KEY,		
		c_out_work_type	VARCHAR(5),\ 	//业务类型
		i_in_trade_amt	VARCHAR(12),\	//应收金额
		c_in_card_uid  VARCHAR(12),\	//uid
		c_out_card_citycode  VARCHAR(8),\	//城市代码
		c_out_card_id 	VARCHAR(26),\		//交易卡号
		c_out_ussuer_id 	VARCHAR(18),\	//发卡方标识
		c_out_card_index_type	VARCHAR(6),\//卡主类型
		c_out_card_child_type 	VARCHAR(6),\//卡子类型
		c_out_card_cnt 	VARCHAR(8),\		//卡计数器
		i_out_bef_blance VARCHAR(12),\		//交易前余额	
		i_out_real_trade_amt	VARCHAR(12),\//实际交易金额
		i_out_aft_blance VARCHAR(12),\		//交易后余额	
		c_out_trade_type	VARCHAR(8),\	//卡交易类型
		c_out_trade_random	VARCHAR(12),\	//卡交易随机数
		c_out_old_trade_type	VARCHAR(8),\//上次交易类型
		c_out_old_trade_termid	NVARCHAR(14),\//上次交易psam终端号
		c_out_old_trade_dt	NVARCHAR(16),\	//上次交易时间
		c_out_old_trade_amt VARCHAR(12),\	//上次交易金额
		c_out_old_trade_cnt VARCHAR(6),\	//上次交易计数器
		c_out_trade_datetime VARCHAR(16),\	//交易时间
		c_out_wallet_type VARCHAR(4),\		//钱包类型00金额 01扣次
		c_out_aq VARCHAR(8),\				//交易流水
		c_out_tac	NVARCHAR(12),\			//交易tac
		c_out_psam_tern_id 	VARCHAR(14),\	//psam终端号
		c_out_psam_id VARCHAR(24),\			//psam卡号
		c_out_psam_sq VARCHAR(12),\			//交易psam卡交易流水
		c_out_trade_result VARCHAR(4),\		//交易标识00 正常 01灰交易
		c_out_district_id VARCHAR(20),\		//县区编号
		c_out_merch_id VARCHAR(20),\		//商户号
		c_out_driver_id VARCHAR(20),\		//司机编号
		c_out_line_id VARCHAR(8),\			//线路编号
		c_out_dev_no VARCHAR(10),\			//设备编号
		c_out_sn VARCHAR(32),\				//设备SN
		c_out_car_id VARCHAR(12),\			//车辆编号
		c_out_class_sq VARCHAR(12),\		//班次顺序号
		c_out_update_flag VARCHAR(4),\		//上送状态
		filed0	 VARCHAR(20),\
		filed1	 VARCHAR(20)
	*/

	//增
	if(OprType == DB_INSERT)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq,"INSERT INTO %s VALUES(NULL,'%s','%d','%s','%s','%s','%s','%s','%s','%s','%d','%d','%d','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s', NULL ,NULL)",
				tableName,
				ST_TrdeLog->c_out_work_type,
				ST_TrdeLog->i_in_trade_amt,
				ST_TrdeLog->c_in_card_uid,
				ST_TrdeLog->c_out_card_citycode,
				ST_TrdeLog->c_out_card_id,
				ST_TrdeLog->c_out_ussuer_id,
				ST_TrdeLog->c_out_card_index_type,
				ST_TrdeLog->c_out_card_child_type,
				ST_TrdeLog->c_out_card_cnt,
				ST_TrdeLog->i_out_bef_blance,
				ST_TrdeLog->i_out_real_trade_amt,
				ST_TrdeLog->i_out_aft_blance,
				ST_TrdeLog->c_out_trade_type,
				ST_TrdeLog->c_out_trade_random,
				ST_TrdeLog->c_out_old_trade_type,
				ST_TrdeLog->c_out_old_trade_termid,
				ST_TrdeLog->c_out_old_trade_dt,
				ST_TrdeLog->c_out_old_trade_amt,
				ST_TrdeLog->c_out_old_trade_cnt,
				ST_TrdeLog->c_out_trade_datetime,
				ST_TrdeLog->c_out_wallet_type,
				ST_TrdeLog->c_out_aq,
				ST_TrdeLog->c_out_tac,
				ST_TrdeLog->c_out_psam_tern_id,
				ST_TrdeLog->c_out_psam_id,
				ST_TrdeLog->c_out_psam_sq,
				ST_TrdeLog->c_out_trade_result,
				ST_TrdeLog->c_out_district_id,
				ST_TrdeLog->c_out_merch_id,
				ST_TrdeLog->c_out_driver_id,
				ST_TrdeLog->c_out_line_id,
				ST_TrdeLog->c_out_dev_no,
				ST_TrdeLog->c_out_sn,
				ST_TrdeLog->c_out_car_id,
				ST_TrdeLog->c_out_class_sq,
				ST_TrdeLog->c_out_update_flag);
		Uart_Printf( "chaSqlStr is %s\n", Sq);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if (iRet == -1)
		{
			goto END;
		}
	}
	//删
	else if(OprType == DB_DELETE)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "delete from %s", tableName);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}

	}
	//查(未上送)
	else if(OprType == DB_SELECT)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			goto END;
		}

		//查询
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "select * from %s where c_out_trade_result='%s' and c_out_update_flag='%s' limit 1", tableName, ST_TrdeLog->c_out_trade_result, ST_TrdeLog->c_out_update_flag);
		iRet = sqlite3_get_table(Transedb, Sq, &dbResult, &nRow, &nColumn, &zErrMsg);
		Uart_Printf("sqlite3_get_table[%d]:%s\n", iRet, Sq);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			Uart_Printf("共 %d 记录\n", nRow);
			Uart_Printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
					}
					Uart_Printf("\n");
				}

				memset(ST_TrdeLog, 0, sizeof(StTradeInfor));
				index ++;
				strcpy(ST_TrdeLog->c_out_work_type, dbResult[index++]);
				ST_TrdeLog->i_in_trade_amt = atol(dbResult[index++]);
				strcpy(ST_TrdeLog->c_in_card_uid, 		dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_card_citycode, 		dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_card_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_ussuer_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_card_index_type, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_card_child_type, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_card_cnt, dbResult[index++]);
				ST_TrdeLog->i_out_bef_blance = atol(dbResult[index++]);
				ST_TrdeLog->i_out_real_trade_amt = atol(dbResult[index++]);
				ST_TrdeLog->i_out_aft_blance = atol(dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_trade_type, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_trade_random, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_old_trade_type, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_old_trade_termid, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_old_trade_dt, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_old_trade_amt, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_old_trade_cnt, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_trade_datetime, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_wallet_type, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_aq, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_tac, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_psam_tern_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_psam_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_psam_sq, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_trade_result, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_district_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_merch_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_driver_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_line_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_dev_no, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_sn, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_car_id, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_class_sq, dbResult[index++]);
				strcpy(ST_TrdeLog->c_out_update_flag, dbResult[index++]);
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = nRow;
			goto END;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", Sq, zErrMsg);

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = -1;
			goto END;
		}
	}
	//改
	else if(OprType == DB_UPDATE)
	{
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "update %s set c_out_update_flag='%s' where c_out_card_id='%s' and c_out_trade_datetime='%s' and c_out_tac='%s'", tableName, ST_TrdeLog->c_out_update_flag, ST_TrdeLog->c_out_card_id, ST_TrdeLog->c_out_trade_datetime, ST_TrdeLog->c_out_tac);
		Uart_Printf("Sq : %s\n", Sq);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}
	}
	//未上送交易统计
	else if(OprType == DB_STATISTICAL_RECORD)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			goto END;
		}
		
		//查询
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "select count(*), sum(%s) from %s where c_out_update_flag='%s' ", "i_out_real_trade_amt", tableName, "01");
		iRet = sqlite3_get_table(Transedb, Sq, &dbResult, &nRow, &nColumn, &zErrMsg);
		Uart_Printf("sqlite3_get_table[%d]:%s\n", iRet, Sq);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;
		
			Uart_Printf("共 %d 记录\n", nRow);
			Uart_Printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");
		
			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
					}
					Uart_Printf("\n");
				}
		
				//统计的笔数
				iRet = atol(dbResult[index + 0]);
				if(iRet  != 0)
				{
					Uart_Printf("%d\n", atol(dbResult[index   + 0]));
					ST_TrdeLog->motDataStatic.ulNotUpMot_mnt = atol(dbResult[index + 0]);
		
					Uart_Printf("%d\n", atol(dbResult[index + 1]));
					ST_TrdeLog->motDataStatic.ulNotUpMot_mny = atol(dbResult[index + 1]);
				}
			}
		
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
		
			goto END;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", Sq, zErrMsg);
		
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
		
			iRet = -1;
			goto END;
		}
	}
	//当天交易统计
	else if(OprType == DB_STATISTICAL_MOT_CRT_DAY)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			goto END;
		}

		//查询
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "select count(*), sum(%s) from %s where c_out_trade_datetime like '%s%%' and c_out_update_flag='%s' or c_out_update_flag='%s'", "i_out_real_trade_amt", tableName, ST_TrdeLog->c_out_old_trade_dt, "00", "01");
		iRet = sqlite3_get_table(Transedb, Sq, &dbResult, &nRow, &nColumn, &zErrMsg);
		Uart_Printf("sqlite3_get_table[%d]:%s\n", iRet, Sq);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			Uart_Printf("共 %d 记录\n", nRow);
			Uart_Printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
					}
					Uart_Printf("\n");
				}

				//统计的笔数
				iRet = atol(dbResult[index + 0]);
				if(iRet  != 0)
				{
					Uart_Printf("%d\n", atol(dbResult[index   + 0]));
					ST_TrdeLog->motDataStatic.ulCrtDayMot_mnt = atol(dbResult[index + 0]);

					Uart_Printf("%d\n", atol(dbResult[index + 1]));
					ST_TrdeLog->motDataStatic.ulCrtDayMot_mny = atol(dbResult[index + 1]);
				}
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			goto END;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", Sq, zErrMsg);

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = -1;
			goto END;
		}
	}

	//全部交易统计
	else if(OprType == DB_STATISTICAL_MOT_TATOL)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			goto END;
		}

		//查询
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "select count(*), sum(%s) from %s where c_out_update_flag='%s' or c_out_update_flag='%s'", "i_out_real_trade_amt", tableName, "00", "01");
		iRet = sqlite3_get_table(Transedb, Sq, &dbResult, &nRow, &nColumn, &zErrMsg);
		Uart_Printf("sqlite3_get_table[%d]:%s\n", iRet, Sq);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			Uart_Printf("共 %d 记录\n", nRow);
			Uart_Printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
					}
					Uart_Printf("\n");
				}

				//统计的笔数
				iRet = atol(dbResult[index + 0]);
				if(iRet  != 0)
				{
					Uart_Printf("%d\n", atol(dbResult[index   + 0]));
					ST_TrdeLog->motDataStatic.ulTotalMot_mnt = atol(dbResult[index + 0]);

					Uart_Printf("%d\n", atol(dbResult[index + 1]));
					ST_TrdeLog->motDataStatic.ulTotalMot_mny = atol(dbResult[index + 1]);
				}
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			goto END;
		}
		else 
		{
			printf("SQLITE[%s] ERR : %s\n", Sq, zErrMsg);

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			iRet = -1;
			goto END;
		}
	}
	else if(OprType == DB_REUPLOAD_CLR)
	{
		memset(ClrDateTime, 0, sizeof(ClrDateTime));
		memset(NeedClrDateTime, 0, sizeof(NeedClrDateTime));
		
		//计算需要清理的时间
		GetLocDate(ClrDateTime);
		Api_ToolGetDays(ClrDateTime, 30 * 3, NeedClrDateTime);
		
		//清理交易表
		memset(Sq, 0, sizeof(Sq));
		sprintf(Sq, "delete from %s where c_out_trade_datetime < '%s' ", tableName, NeedClrDateTime);
		iRet = db_exec_sql(DataBaseNume, Sq);
		if( iRet == -1 )
		{
			goto END;
		}
	
		//数据库瘦身
		memset(Sq, 0, sizeof(Sq));
		strcpy(Sq, "VACUUM");
		Uart_Printf("chaSqlStr : %s\n", Sq);
		db_exec_sql_without_transaction(DataBaseNume, Sq);
	}
	
	else
	{
		Uart_Printf("database_cpu_trade_table_opr's OprType Err\n");
	}
	
END:
	//互斥锁释放
	pthread_mutex_unlock(&db_trade_mutex);
	
	return iRet;

}

/**
 * @Author: hhl \@decard
 * @fn     	int db_para_black_list_table_opr(DB_OPR_TYPE OprType, char *pcard_id)
 * @brief:	参数数据库，黑名单表操作
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  10:15:46
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  10:15:46
*/
int db_para_black_list_table_opr(DB_OPR_TYPE OprType, char *pcard_id)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	static sqlite3 *Transedb = NULL;

	int nRow = 0, nColumn = 0;
	char **dbResult = NULL;

	char chaSqlStr[1024+1] = {0};

	char tableName[256] = {0};
	char DataBaseNume[512] = {0};

	memset(DataBaseNume, 0, sizeof(DataBaseNume));
	strcpy(DataBaseNume, db_para_path);

	memset(tableName, 0, sizeof(tableName));
	strcpy(tableName, Table_Mot_Black);

	/*
		BlackCardNumb 卡号
	*/
	//事务开启
	if(OprType == DB_TRANSACTION_BEGIN)
	{
		iRet = sqlite3_open(DataBaseNume, &Transedb);
		if(iRet)
		{
			Uart_Printf("sqlite3_open Err : %s\n", sqlite3_errmsg(Transedb));
			if (NULL != Transedb)
				sqlite3_close(Transedb);
			return -1;
		}

		iRet = sqlite3_exec(Transedb, "begin transaction", 0, 0, 0);
		Uart_Printf("begin transaction : %d\n", iRet);
		if(iRet != SQLITE_OK)
		{
			sqlite3_close(Transedb);
			return iRet;
		}

		return 0;
	}
	//事务结束
	else if(OprType == DB_TRANSACTION_COMIT)
	{
		//事务提交
		iRet = sqlite3_exec(Transedb, "commit transaction", 0, 0, 0);
		Uart_Printf("commit transaction : %d\n", iRet);
		if (iRet != SQLITE_OK)
		{
			Uart_Printf("SQL error: %s\n", zErrMsg);
			
			//关闭数据库
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return iRet;
		}

		//关闭数据库
		sqlite3_free(zErrMsg);
		sqlite3_close(Transedb);
		return 0;
	}
	//增
	else if(OprType == DB_INSERT)
	{
		/*
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "INSERT INTO %s VALUES('%s');",tableName, pcard_id);
		Uart_Printf( "chaSqlStr is %s\n", chaSqlStr);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if (iRet != 0)
		{
			return iRet;
		}
		*/
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "INSERT INTO %s VALUES('%s');",tableName, pcard_id);
		iRet = sqlite3_exec(Transedb, chaSqlStr, 0, 0, &zErrMsg);
		if(iRet != SQLITE_OK)
		{	
			Uart_Printf("sqlite3_exec[%s] err : %d [%s]\n", chaSqlStr, iRet, zErrMsg);
		}
		sqlite3_free(zErrMsg);
		
		return iRet;
	}
	//删
	else if(OprType == DB_DELETE)
	{
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "delete from %s", tableName);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if( iRet == -1 )
		{
			return (iRet);
		}
	}
	//查
	else if(OprType == DB_SELECT)
	{
 		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s where BlackCardNumb='%s'", tableName, pcard_id);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index]);
						index ++;
					}
					Uart_Printf("\n");
				}
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}

	}
	//统计查询
	else if(OprType == DB_STATISTICAL_RECORD)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s", tableName);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
/*				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						printf("%-5s ", dbResult[index++]);
					}
					printf("\n");
				}*/
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}
	}
	else
	{
		Uart_Printf("database_cpu_black_list_table_opr's OprType [%d] Err\n", OprType);
	}

	return (0);
}


/**
 * @Author: hhl \@decard
 * @fn     	int db_para_white_list_table_opr(DB_OPR_TYPE OprType, char *pcard_id);
 * @brief:	参数数据库，白名单表操作
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  10:15:50
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  10:15:50
*/
int db_para_white_list_table_opr(DB_OPR_TYPE OprType, char *pcard_id)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	static sqlite3 *Transedb = NULL;

	int nRow = 0, nColumn = 0;
	char **dbResult = NULL;

	char chaSqlStr[1024+1] = {0};

	char tableName[256] = {0};
	char DataBaseNume[512] = {0};

	memset(DataBaseNume, 0, sizeof(DataBaseNume));
	strcpy(DataBaseNume, db_para_path);

	memset(tableName, 0, sizeof(tableName));
	strcpy(tableName, Table_Mot_White);

	/*
		BlackCardNumb 卡号
	*/
	//事务开启
	if(OprType == DB_TRANSACTION_BEGIN)
	{
		iRet = sqlite3_open(DataBaseNume, &Transedb);
		if(iRet)
		{
			Uart_Printf("sqlite3_open Err : %s\n", sqlite3_errmsg(Transedb));
			if (NULL != Transedb)
				sqlite3_close(Transedb);
			return -1;
		}

		
		iRet = sqlite3_exec(Transedb, "begin transaction", 0, 0, 0);
		Uart_Printf("begin transaction : %d\n", iRet);
		if(iRet != SQLITE_OK)
		{
			sqlite3_close(Transedb);
			return iRet;
		}

		return 0;
	}
	//事务结束
	else if(OprType == DB_TRANSACTION_COMIT)
	{
		//事务提交
		iRet = sqlite3_exec(Transedb, "commit transaction", 0, 0, 0);
		Uart_Printf("commit transaction : %d\n", iRet);
		if (iRet != SQLITE_OK)
		{
			Uart_Printf("SQL error: %s\n", zErrMsg);
			
			//关闭数据库
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return iRet;
		}

		//关闭数据库
		sqlite3_free(zErrMsg);
		sqlite3_close(Transedb);
		return 0;
	}
	//增
	else if(OprType == DB_INSERT)
	{
		/*
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "INSERT INTO %s VALUES('%s');",tableName, pcard_id);
		Uart_Printf( "chaSqlStr is %s\n", chaSqlStr);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if (iRet != 0)
		{
			return iRet;
		}
		*/
		
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "INSERT INTO %s VALUES('%s');",tableName, pcard_id);
		iRet = sqlite3_exec(Transedb, chaSqlStr, 0, 0, &zErrMsg);
		if(iRet != SQLITE_OK)
		{	
			Uart_Printf("sqlite3_exec[%s] err : %d [%s]\n", chaSqlStr, iRet, zErrMsg);
		}
		sqlite3_free(zErrMsg);
		
		
		return iRet;
	}
	//删
	else if(OprType == DB_DELETE)
	{
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "delete from %s", tableName);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if( iRet == -1 )
		{
			return (iRet);
		}
	}
	//查
	else if(OprType == DB_SELECT)
	{
 		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s where WhiteCardNumb='%s'", tableName, pcard_id);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index]);
						index ++;
					}
					Uart_Printf("\n");
				}
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}

	}
	//统计查询
	else if(OprType == DB_STATISTICAL_RECORD)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s", tableName);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
/*				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						printf("%-5s ", dbResult[index++]);
					}
					printf("\n");
				}*/
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}
	}
	else
	{
		Uart_Printf("database_cpu_black_list_table_opr's OprType [%d] Err\n", OprType);
		return -1;
	}

	return (0);
}

/**
 * @Author: hhl \@decard
 * @fn     	int db_para_card_type_table_opr(DB_OPR_TYPE OprType, XCXQGJ_CARDTYPERATE_T *pcard_type_infor);
 * @brief:	参数数据库，卡类型表操作
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 19, 2019  09:52:36
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 19, 2019  09:52:36
*/
int db_para_card_type_table_opr(DB_OPR_TYPE OprType, XCXQGJ_CARDTYPERATE_T *pcard_type_infor)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	static sqlite3 *Transedb = NULL;

	int nRow = 0, nColumn = 0;
	char **dbResult = NULL;

	char chaSqlStr[1024+1] = {0};

	char tableName[256] = {0};
	char DataBaseNume[512] = {0};

	memset(DataBaseNume, 0, sizeof(DataBaseNume));
	strcpy(DataBaseNume, db_para_path);

	memset(tableName, 0, sizeof(tableName));
	strcpy(tableName, Table_Mot_CardType);

	/*
		aucCardType 卡类型白名单
		aucWalletDiscount 钱包折扣
		aucMonthDiscount 月票折扣
		aucContStatus 连刷状态 1可以连刷，0不可以连刷
		aucLimitTime 限制时间 秒
	*/
	if(OprType == DB_INSERT)
	{
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "INSERT INTO %s VALUES('%s', '%s', '%s', '%s', '%s');",tableName, 
			pcard_type_infor->aucCardType,
			pcard_type_infor->aucWalletDiscount,
			pcard_type_infor->aucMonthDiscount,
			pcard_type_infor->aucContStatus,
			pcard_type_infor->aucLimitTime);
		Uart_Printf( "chaSqlStr is %s\n", chaSqlStr);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if (iRet != 0)
		{
			return iRet;
		}

		return iRet;
	}
	//删
	else if(OprType == DB_DELETE)
	{
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "delete from %s", tableName);
		iRet = db_exec_sql(DataBaseNume, chaSqlStr);
		if( iRet == -1 )
		{
			return (iRet);
		}
	}
	//查
	else if(OprType == DB_SELECT)
	{
 		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s where aucCardType='%s'", tableName, pcard_type_infor->aucCardType);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			Uart_Printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				Uart_Printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						Uart_Printf("%-5s ", dbResult[index + j]);
						//index ++;
					}
					Uart_Printf("\n");
				}
				
				memset(pcard_type_infor, 0, sizeof(XCXQGJ_CARDTYPERATE_T));
				
				strcpy(pcard_type_infor->aucCardType, dbResult[index++]);
				strcpy(pcard_type_infor->aucWalletDiscount,	dbResult[index++]);
				strcpy(pcard_type_infor->aucMonthDiscount, 	dbResult[index++]);
				strcpy(pcard_type_infor->aucContStatus, dbResult[index++]);
				strcpy(pcard_type_infor->aucLimitTime, dbResult[index++]);
			}
			
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}

	}
	//统计查询
	else if(OprType == DB_STATISTICAL_RECORD)
	{
		//打开
		Transedb = db_open(DataBaseNume);
		if(Transedb == NULL)
		{
			printf("Open db Err !!!\n");
			return -1;
		}

		//查询
		memset(chaSqlStr, 0, sizeof(chaSqlStr));
		sprintf(chaSqlStr, "SELECT * FROM %s", tableName);
		iRet = sqlite3_get_table(Transedb, chaSqlStr, &dbResult, &nRow, &nColumn, &zErrMsg);
		printf("sqlite3_get_table[%d]:%s\n", iRet,	chaSqlStr);
		if(iRet == SQLITE_OK)
		{
			int i, j, index;

			printf("共 %d 记录\n", nRow);
			printf("共 %d 列\n", nColumn);
			printf("字段名			|	 字段值\n");
			for(i = 0; i < nColumn; i ++)
			{
				printf("|%s   ", dbResult[i]);
			}
			Uart_Printf("\n");
			Uart_Printf("--------------------------------------------------\n");

			index = nColumn;
			if(nRow != 0)
			{
/*				for(i = 0; i < nRow; i ++)
				{
					for(j = 0; j < nColumn; j ++)
					{
						printf("%-5s ", dbResult[index++]);
					}
					printf("\n");
				}*/
			}

			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);

			return nRow;
		}
		else
		{
			printf("SQLITE[%s] ERR : %s\n", chaSqlStr, zErrMsg);
			sqlite3_free_table(dbResult);
			sqlite3_free(zErrMsg);
			sqlite3_close(Transedb);
			return -1;
		}
	}
	else
	{
		Uart_Printf("database_cpu_black_list_table_opr's OprType [%d] Err\n", OprType);
		return -1;
	}
	

	return (0);
}


/**
 * @Author: hhl \@decard
 * @fn     	int db_trade_creat(void);
 * @brief:	交易数据库创建
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  08:58:10
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  08:58:10
*/
int db_trade_creat(char *db_path)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	unsigned char createCsmDetailtbSql[2048] = { 0 };
	sqlite3 *Transedb = NULL;


	//数据库文件打开
	Transedb = db_open(db_path);
	if(Transedb == NULL)
	{
		File_Printf("Open db Err[%s] !!!\n", db_path);
		return 1;
	}

	Uart_Printf("db_trade_creat:%s\r\n", db_path);

	//银联交易表
	Uart_Printf("Creat %s table !!!\n", Table_Mot_Trade);

	/*
		scene	VARCHAR(32),\	//支付场景
		qrcode	VARCHAR(64),\	//二维码数据
		body  VARCHAR(64),\		//商品名称
		totalFee  VARCHAR(12),\	//支付金额
		terminalIp	VARCHAR(20),\	//终端ip
		outTradeNo	VARCHAR(32),\	//商户订单号（唯一的订单流水号）
		terminalParams	VARCHAR(64),\	//终端其他信息
		model	VARCHAR(64),\		//终端型号
		sn	VARCHAR(32),\			//终端sn
		shop_num VARCHAR(64),\		//商户编号(商户项目编号)
		openId	VARCHAR(64),\		//用户标识
		payType VARCHAR(64),\		//支付分类
		pay_type	VARCHAR(64),\	//支付类型
		timeEnd VARCHAR(32),\		//支付时间
		transactionId	VARCHAR(64),\	//交易流水号
		out_refund_no	NVARCHAR(32),\	//退款流水号（唯一）
		state	NVARCHAR(6),\			//交易状态:成功/退货
		upLoadFlag VARCHAR(6),\			//上送状态

		filed0	 VARCHAR(20),\
		filed1	 VARCHAR(20)
	*/
	memset(createCsmDetailtbSql, 0, sizeof(createCsmDetailtbSql));
	sprintf(createCsmDetailtbSql, "CREATE TABLE IF NOT EXISTS %s(\ID INTEGER PRIMARY KEY,\
								scene	VARCHAR(32),\
								qrcode	VARCHAR(64),\
								body  VARCHAR(64),\
								totalFee  VARCHAR(12),\
								terminalIp 	VARCHAR(20),\
								outTradeNo 	VARCHAR(32),\
								terminalParams	VARCHAR(64),\
								model 	VARCHAR(64),\
								sn 	VARCHAR(32),\
								shop_num VARCHAR(64),\
								openId	VARCHAR(64),\
								payType VARCHAR(64),\
								pay_type	VARCHAR(64),\
								timeEnd	VARCHAR(32),\
								transactionId	VARCHAR(64),\
								out_refund_no	NVARCHAR(32),\
								state	NVARCHAR(6),\
								upLoadFlag VARCHAR(6),\
								filed0	 VARCHAR(20),\
								filed1	 VARCHAR(20))", Table_Mot_Trade);
	iRet = sqlite3_exec(Transedb, createCsmDetailtbSql, 0, 0, &zErrMsg);
	if( iRet != SQLITE_OK )
	{
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		File_Printf("Err >>SQ:[%s][%s]\n", zErrMsg, createCsmDetailtbSql);
		iRet =  -1;
		goto END;
	}

END:
	
	sqlite3_free(zErrMsg);
	sqlite3_close(Transedb);

	return iRet;
}

/**
 * @Author: hhl \@decard
 * @fn     	int db_para_creat(void);
 * @brief:	参数数据库创建
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  08:59:43
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  08:59:43
*/
int db_para_creat(char *db_path)
{
	int iRet = 0;
	char *zErrMsg = NULL;
	unsigned char createCsmDetailtbSql[2048] = { 0 };
	sqlite3 *Transedb = NULL;

	//数据库打开
	Transedb = db_open(db_path);
	if(Transedb == NULL)
	{
		File_Printf("Open db Err[%s] !!!\n", db_path);
		return -1;
	}

	Uart_Printf("db_para_creat:%s\r\n", db_path);

	//CPU黑名单表
	/*
		BlackCardNumb 卡号
	*/
	memset(createCsmDetailtbSql, 0, sizeof(createCsmDetailtbSql));
	sprintf(createCsmDetailtbSql, "CREATE TABLE IF NOT EXISTS %s(\
								BlackCardNumb	VARCHAR(32)PRIMARY KEY)", Table_Mot_Black);
	iRet = sqlite3_exec(Transedb, createCsmDetailtbSql, 0, 0, &zErrMsg);
	if( iRet != SQLITE_OK )
	{
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		File_Printf("Err >>SQ:[%s][%s]\n", zErrMsg, createCsmDetailtbSql);
		iRet =  -1;
		goto END;
	}

	//CPU白名单表.发卡行机构代码
	/*
		WhiteCardNumb 机构编码
	*/
	memset(createCsmDetailtbSql, 0, sizeof(createCsmDetailtbSql));
	sprintf(createCsmDetailtbSql, "CREATE TABLE IF NOT EXISTS %s(\
								WhiteCardNumb	VARCHAR(32) PRIMARY KEY)", Table_Mot_White);
	iRet = sqlite3_exec(Transedb, createCsmDetailtbSql, 0, 0, &zErrMsg);
	if( iRet != SQLITE_OK )
	{
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		File_Printf("Err >>SQ:[%s][%s]\n", zErrMsg, createCsmDetailtbSql);
		iRet =  -1;
		goto END;
	}

	//卡类型参数
	/*
		aucCardType 卡类型白名单
		aucWalletDiscount 钱包折扣
		aucMonthDiscount 月票折扣
		aucContStatus 连刷状态 1可以连刷，0不可以连刷
		aucLimitTime 限制时间 秒
	*/
	memset(createCsmDetailtbSql, 0, sizeof(createCsmDetailtbSql));
	sprintf(createCsmDetailtbSql, "CREATE TABLE IF NOT EXISTS %s(\
								aucCardType	VARCHAR(8) PRIMARY KEY, \
								aucWalletDiscount VARCHAR(8),\
								aucMonthDiscount VARCHAR(8),\
								aucContStatus VARCHAR(8),\
								aucLimitTime VARCHAR(10))", Table_Mot_CardType);
	iRet = sqlite3_exec(Transedb, createCsmDetailtbSql, 0, 0, &zErrMsg);
	if( iRet != SQLITE_OK )
	{
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);
		File_Printf("Err >>SQ:[%s][%s]\n", zErrMsg, createCsmDetailtbSql);
		iRet =	-1;
		goto END;
	}

END:
	sqlite3_free(zErrMsg);
	sqlite3_close(Transedb);

	return iRet;
}


/**
 * @Author: hhl \@decard
 * @fn     	int db_init(void);
 * @brief:	数据库初始化
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:November 13, 2019  09:16:57
 * @Last Modified by:hhl \@decard
 * @Last Modified time:November 13, 2019  09:16:57
*/
int db_init(void)
{
	int iret = 0;

	//交易数据库创建
	iret = db_trade_creat(db_trade_path);
	if(iret)
	{
		File_Printf("db_trade_creat Err : %d\n", iret);
	}

	//参数数据库创建
	/*
	iret = db_para_creat(db_para_path);
	if(iret)
	{
		File_Printf("db_trade_creat Err : %d\n", iret);
	}
	*/

	//交易数据库清除机制
	db_trade_decard_ploymerize_pay(DB_REUPLOAD_CLR, NULL);

	return iret;
}



