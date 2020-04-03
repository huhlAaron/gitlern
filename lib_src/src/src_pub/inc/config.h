/*
 * config.h
 *	配置文件key定义模块
 *  Created on: November 07, 2019  15:29:50
 *      Author: huhongliang@decard.com
 */
#ifndef _CONFIG_H
#define _CONFIG_H

//流水配置文件key定义
#define CFG_SERILAUMB	"serail_cfg"          //流水号文件
#define SER_SERILAUMB 	"serialNumb"          //流水号

//参数配置文件key定义
#define CFG_PARA      	"para_cfg"          //参数文件
#define PARA_LINE_NMB  	"para_line_nmb"     //线路编号
#define PARA_LINE_NAME  	"line_name"     //线路名称
#define PARA_PRICE_TYPE  	"pri_type"   	//票制类型
#define PARA_LINE_PRICE  	"price"    		//票价
#define PARA_LINE_TEAM_ID  	"team_id"    	//路队编号
#define PARA_TERM_TYPE  	"term_type"    	//设备类型

#define PARA_MERCH_NMB  	"para_merchant_nmb"     //商户号
#define PARA_AREA_NMB  	"para_area_nmb"     //区域编号

//FTP相关参数
#define PARA_FTP_IP  	"para_ftp_ip"     //ftp地址
#define PARA_FTP_PORT  	"para_ftp_port"     //ftp端口
#define PARA_FTP_PATH  	"para_ftp_path"     //ftp路径
#define PARA_FTP_USER  	"para_ftp_user"     //ftp用户名
#define PARA_FTP_PWD  	"para_ftp_pwd"     //ftp密码

//版本控制相关参数
#define PARA_BLACKLIST_VER  	"blackListVer"     //黑名单版本
#define PARA_WHITELIST_VER  	"whiteListVer"     //白名单版本


//聚合支付平台参数存储
#define SERV_PUB_KEY  "ser_pub_key"	//服务端公钥
#define TARM_MAK		"term_mak"		//终端工作密钥

#endif /* _CONFIG_H */

