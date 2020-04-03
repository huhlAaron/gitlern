/*
 * screen.c
 *	显示接口文件
 *  Created on: October 31, 2019  14:11:03
 *      Author: huhongliang@decard.com
 */
 
#include <stdio.h>
#include <stdarg.h>
#include "define.h"
#include "data_type.h"


/**
 * @Author: hhl \@decard
 * @fn     	void screen_display_err_infor();
 * @brief:	错误信息显示
 * @param[in] :
 * @param[out]:
 * @return:
 * @Date:October 31, 2019  14:13:18
 * @Last Modified by:hhl \@decard
 * @Last Modified time:October 31, 2019  14:13:18
*/
void screen_display_err_infor(unsigned long err_code)
{
	Uart_Printf("ErrCode:%X\r\n", err_code);
	/*
	switch(err_code)
	{
		case ERR_FUNCTIONCLOSE:
		{
			screen_display_infor("错误提示","不支持该业务", "请联系收单机构"," ");
			break;
		}
		case ERR_CARD_GETATS:
		{
			screen_display_infor("错误提示","读取ATS失败", "交易取消"," ");
			break;
		}
		case ERR_CARD_SELECT3F00:
		{
			screen_display_infor("错误提示","读取3F00失败", "交易取消"," ");
			break;
		}
		case ERR_CRD_ILLEGAL:
		{
			screen_display_infor("错误提示","读发卡方信息失败", "交易取消"," ");
			break;
		}
		case ERR_REFER_FMT:
		{
			screen_display_infor("错误提示","读文件失败", "交易取消"," ");
			break;
		}
		case ERR_READCOMMON_WALL:
		{
			screen_display_infor("错误提示","读钱包文件失败", "交易取消"," ");
			break;
		}
		case ERR_GATBLANCE805C:
		{
			screen_display_infor("错误提示","读余额失败", "交易取消"," ");
			break;
		}
		case ERR_INITCONSUMEWALLET:
		{
			screen_display_infor("错误提示","消费初始化失败", "交易取消"," ");
			break;
		}
		case ERR_SAMSELE1001:
		{
			screen_display_infor("错误提示","SAM文件选择失败", "请重试"," ");
			break;
		}
		case ERRR_SAMGETMAC1:
		{
			screen_display_infor("错误提示","SAM计算MAC1失败", "交易取消"," ");
			break;
		}
		case ERR_CPU8054CONSUME:
		{
			screen_display_infor("错误提示","扣款失败", "请重刷"," ");
			break;
		}
		case ERR_CPUCARDGREY_TRADE:
		{
			screen_display_infor("警告提示","交易未完成", "请重刷"," ");
			break;
		}
		case ERR_CPUCHECKMAC2:
		{
			screen_display_infor("错误提示","MAC2验证失败", "交易取消"," ");
			break;
		}
		case ERR_MIFARESELECT:
		{
			screen_display_infor("错误提示","选卡失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC0:
		{
			screen_display_infor("错误提示","读扇区0失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC1:
		{
			screen_display_infor("错误提示","读扇区1失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC2:
		{
			screen_display_infor("错误提示","读扇区2失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC3:
		{
			screen_display_infor("错误提示","读扇区3失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC4:
		{
			screen_display_infor("错误提示","读扇区4失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC5:
		{
			screen_display_infor("错误提示","读扇区5失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC6:
		{
			screen_display_infor("错误提示","读扇区6失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC7:
		{
			screen_display_infor("错误提示","读扇区7失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC8:
		{
			screen_display_infor("错误提示","读扇区8失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC9:
		{
			screen_display_infor("错误提示","读扇区9失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC10:
		{
			screen_display_infor("错误提示","读扇区10失败", "请重试"," ");
			break;
		}
		case ERR_MIFAREREADSEC11:
		{
			screen_display_infor("错误提示","读扇区11失败", "交易取消"," ");
			break;
		}
		case ERR_PROTOCOLLEN:
		{
			screen_display_infor("错误提示","数据长度错误", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC12:
		{
			screen_display_infor("错误提示","读扇区12失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC13:
		{
			screen_display_infor("错误提示","读扇区13失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC14:
		{
			screen_display_infor("错误提示","读扇区14失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREREADSEC15:
		{
			screen_display_infor("错误提示","读扇区15失败", "交易取消"," ");
			break;
		}
		case ERR_SAMGETOTHERKAY:
		{
			screen_display_infor("错误提示","卡片密钥计算失败", "交易取消"," ");
			break;
		}
		case ERR_CHECKBLOCK4:
		{
			screen_display_infor("错误提示","块4校验失败", "交易取消"," ");
			break;
		}
		case ERR_CHECKBLOCK5:
		{
			screen_display_infor("错误提示","块5校验失败", "交易取消"," ");
			break;
		}
		case ERR_CHECKBLOCK6:
		{
			screen_display_infor("错误提示","块6校验错误!!!", "交易取消"," ");
			break;
		}
		case ERR_CHECKBLOCK8:
		{
			screen_display_infor("错误提示","块8校验失败", "交易取消"," ");
			break;
		}
		case ERR_M1_BREAKPOINT:
		{
			screen_display_infor("错误提示","断点错误", "交易取消"," ");
			break;
		}
		case ERR_BALANCE:
		{
			screen_display_infor("错误提示","余额异常", "交易取消"," ");
			break;
		}
		case ERR_MIFAREMAKETAC:
		{
			screen_display_infor("错误提示","TAC计算失败!!!", "交易取消"," ");
			break;
		}
		case ERR_MIFAREFLOCK6WRITE:
		{
			screen_display_infor("错误提示","块6写卡失败", "交易取消"," ");
			break;
		}
		case ERR_MIFAREFLOCK7WRITE:
		{
			screen_display_infor("错误提示","块7写卡失败", "交易取消"," ");
			break;
		}
		case ERR_TRADCANCEL:
		{
			screen_display_infor("错误提示","交易被取消", "请重新交易"," ");
			break;
		}
		case ERR_GAETCARDTIMEOUT:
		{
			screen_display_infor("错误提示","刷卡超时", "交易取消"," ");
			break;
		}
		case ERR_TRANSLOG_SAVE:
		{
			screen_display_infor("错误提示","交易存储失败", "交易取消"," ");
			break;
		}
		case ERR_CARDTYPEERR:
		{
			screen_display_infor("错误提示","卡类型不支持", "交易取消"," ");
			break;
		}
		case ERR_CARDNOTENABLED:
		{
			screen_display_infor("错误提示","卡未启用", "交易取消"," ");
			break;
		}
		case ERR_CARDOVERDUE:
		{
			screen_display_infor("错误提示","此卡已过有效期", "交易取消"," ");
			break;
		}
		case ERR_CARDBLACKLIST:
		{
			screen_display_infor("错误提示","故障卡", "交易取消"," ");
			break;
		}
		case ERR_CARD_FAILURE:
		{
			screen_display_infor("错误提示","卡失效", "交易取消"," ");
			break;
		}
		case ERR_CNTOVER:
		{
			screen_display_infor("错误提示","金额过大", "交易取消"," ");
			break;
		}
		case ERR_CNTTOOSMALL:
		{
			screen_display_infor("错误提示","金额过小", "交易取消"," ");
			break;
		}
		case ERR_TRADEOVERLIMIT:
		{
			screen_display_infor("系统提示","笔数上限", "请先结算"," ");
			break;
		}
		case ERR_CARDOVERBLANCE:
		{
			screen_display_infor("系统提示","余额不足", "请投币"," ");
			break;
		}
		case ERR_CARDCHECKWRONG:
		{
			screen_display_infor("系统提示","卡校验不合法", "交易取消"," ");
			break;
		}
		case ERR_CARDNOTGETSTARTDATE:
		{
			screen_display_infor("系统提示","此卡未到启用日期!!!", "请投币"," ");
			break;
		}
		case ERR_TERMNOTLOGON:
		{
			screen_display_infor("系统提示","终端未签到", "请先签到"," ");
			break;
		}
		case ERR_PROTOCOLLRC:
		{
			screen_display_infor("系统提示","协议校验错误", "请检查"," ");
			break;
		}
		case ERR_TERMNOTSETTLE:
		{
			screen_display_infor("系统提示","终端未结算", "禁止升级"," ");
			break;
		}
		case ERR_CARDCDTIONERR:
		{
			screen_display_infor("系统提示","不支持该功能", "请检查"," ");
			break;
		}
		case ERR_TERMUPAPPNOTFINISH:
		{
			screen_display_infor("系统提示","终端程序需要更新", "请执行更新"," ");
			break;
		}
		case ERR_PROTOCOLPARA:
		{
			screen_display_infor("系统提示", "设置错误", "日期格式错误", " ");
			break;
		}
		case ERR_PSAMCAEDINITERR:
		{
			screen_display_infor("系统错误","SAM卡故障", "请检查SAM卡"," ");
			break;
		}
		case ERR_PKICAEDINITERR:
		{
			screen_display_infor("系统错误","PKI卡故障", "请检查PKI卡"," ");
			break;
		}
		case ERR_CNTNOTMULTIPLE:
			screen_display_infor("金额错误","金额应为整元", "交易取消"," ");
			break;
		case ERR_CHAEKKEYB:
			screen_display_infor("系统提示","KEY B出错", "交易取消"," ");
			break;
		case ERR_TERMNOTRCVSERV:
			screen_display_infor("网络错误","接收数据失败", "交易取消"," ");
			break;
		case ERR_TERMNETOVERTIME:
			screen_display_infor("网络错误","服务器连接失败", "交易取消"," ");
			break;
		case ERR_TERMSENDDATA:
			screen_display_infor("网络错误","发送数据失败", "交易取消"," ");
			break;

		case ERR_TERMNETRCVSERV:
			screen_display_infor("网络错误","接收数据非法", "交易取消"," ");
			break;

		case ERR_CHECKPINKEYCHECKVALUE:
			screen_display_infor("网络错误","PIN-KEY校验失败", "请检查主密钥"," ");

			break;

		case ERR_CHECKMACKEYCHECKVALUE:
			screen_display_infor("网络错误","MAC-KEY校验失败", "请检查主密钥"," ");
			break;

		case ERR_CHECKTDKKEYCHECKVALUE:
			screen_display_infor("网络错误","TDKEY校验失败", "请检查主密钥"," ");
			break;

		case ERR_M1NOTSUPPORTCARDTYPE:
			screen_display_infor("系统提示","卡类型不支持", "交易取消"," ");
			break;

		case ERR_RESERVETRADE:
			{
				screen_display_infor("系统提示","交易出错", "交易已冲正","请重新交易");
				break;
			}
		case ERR_IMAGINATIONCARD:
			{
				screen_display_infor("系统提示","卡物理类型不支持", "交易取消","畅享卡");
				break;
			}
		case ERR_ISNOTIMAGINATIONCARD:
			{
				screen_display_infor("系统提示","卡物理类型不支持", "交易取消","请更换畅享卡");
				break;
			}
		case ERR_TERMNOTFINDETRADE:
			{
				screen_display_infor("系统提示","未找到交易", "交易取消"," ");
				break;
			}
		case ERR_TRADEHAVEREVOKE:
			{
				screen_display_infor("系统提示","交易已经撤销", "交易取消"," ");
				break;
			}
		case ERR_NOTSAMECARD:
			{
				screen_display_infor("系统提示","非同一张卡", "交易取消", "请更换卡片");
				break;
			}
		case ERR_NEEDRESERVEDATA:
			{
				screen_display_infor("系统提示","有需要冲正的数据", "冲正失败", "请稍后再试");
				break;
			}
		case ERR_NEEDTICKETCARD:
			{
				screen_display_infor("系统提示","非学生月票卡", "交易取消", "请更换卡片");
				break;
			}

		case ERR_TICKETOVERTIMES:
			{
				screen_display_infor("系统提示","超过最大允许值", "交易取消", " ");
				break;
			}

		case ERR_INPUT:
			{
				screen_display_infor("系统提示","输入有误", "交易取消", " ");
				break;
			}
		case ERR_DONOTREVOKE:
			{
				screen_display_infor("系统提示","已经结帐", "禁止撤销", " ");
				break;
			}
		case ERR_UPDATEUPSUBSEC:
			{
				screen_display_infor("系统提示","上车更新分段信息失败", "交易取消", " ");
				break;
			}
		case ERR_UPDATEDOWNSUBSEC:
			{
				screen_display_infor("系统提示","下车更新分段信息失败", "交易取消", " ");
				break;
			}
		case ERR_READ0X1EFile:
			{
				screen_display_infor("系统提示","读取0x1E文件信息失败", "交易取消", " ");
				break;
			}
		case ERR_UPDATE0X1E:
			{
				screen_display_infor("系统提示","更新0x1E文件信息失败", "交易取消", " ");
				break;
			}
		case ERR_RCVDATAMAC:
			{
				screen_display_infor("系统提示","接收数据MAC错", "交易取消", " ");
				break;
			}
		case ERR_RCVDATALEN:
			{
				screen_display_infor("系统提示","接收数据长度错", "交易取消", " ");
				break;
			}
		case ERR_NOTINTERCARD:
			{
				screen_display_infor("系统提示","非互联互通卡", "交易取消", " ");
				break;
			}
		case ERR_RIDECARD:
			{
				screen_display_infor("系统提示","逃票卡", "禁止交易", " ");
				break;
			}
		case ERR_CONTINUITYSWIPCARD:
			{
				screen_display_infor("系统提示","连刷受限", "请投币", " ");
				break;
			}
		case ERR_OUT_LIMIT_TIMES:
			{
				screen_display_infor("系统提示","次数超过限制", "请投币", " ");
				break;
			}
		case ERR_TICKETTYPE:
			{
				screen_display_infor("系统提示","票制错误", "请检查票制错误", " ");
				break;
			}
		case ERR_READ0X1AFile:
			{
				screen_display_infor("系统提示","读取0x1A文件信息失败", "交易取消", " ");
				break;
			}
		case ERR_UPDATE0X1A:
			{
				screen_display_infor("系统提示","更新0x1A文件信息失败", "交易取消", " ");
				break;
			}
		case ERR_MIFAREREDBIT:
			{
				screen_display_infor("系统提示","M1卡2扇区扣款失败", "交易取消", " ");
				break;
			}
		case ERR_MIFARERERESTORE:
			{
				screen_display_infor("系统提示","M1卡2扇区备份失败", "交易取消", " ");
				break;
			}
		case ERR_EMVSELECTAPP:
			{
				screen_display_infor("系统提示","EMV应用选择失败", "交易取消", " ");
				break;
			}
		case ERR_EMVINITPARA:
			{
				screen_display_infor("系统提示","EMV参数初始化失败", "交易取消", "检查AID/CAPK");
				break;
			}
		case ERR_EMVGPO:
			{
				screen_display_infor("系统提示","EMVGPO失败", "交易取消", " ");
				break;
			}
		case ERR_EMVREADDATA:
			{
				screen_display_infor("系统提示","EMV读数据失败", "交易取消", " ");
				break;
			}
		case ERR_EMVAUTHEN:
			{
				screen_display_infor("系统提示","数据认证失败", "交易取消", " ");
				break;
			}
		case ERR_CARDDISABLE:
			{
				screen_display_infor("系统提示","卡已停用!!!", "请投币", " ");
				break;
			}
		case ERR_CARDDLOCKED:
			{
				screen_display_infor("系统提示","故障卡!!!", "请投币", " ");
				break;
			}
		case ERR_CHECKCITYCODE:
			{
				screen_display_infor("系统提示","城市代码错误!!!", "请投币", " ");
				break;
			}
		case ERR_CHECKAUTHCODE:
			{
				screen_display_infor("系统提示","卡认证码错误!!!", "请投币", " ");
				break;
			}
		case ERR_CHECKCARDNMB:
			{
				screen_display_infor("系统提示","卡号错误!!!", "请投币", " ");
				break;
			}
		case ERR_CARDDIBITCNT:
			{
				screen_display_infor("系统提示","卡计数器达到最大!!!", "请投币", " ");
				break;
			}
		case ERR_MIFARECHECKSEC2:
			{
				screen_display_infor("系统提示","钱包区错误!!!", "交易取消", " ");
				break;
			}
		case ERR_CARDSTRUCT:
			{
				screen_display_infor("系统提示","卡结构错误!!!", "交易取消", " ");
				break;
			}
		case ERR_QROVERDATE:
			{
				screen_display_infor("系统提示","二维码不在有效期内", "交易取消", " ");
				break;
			}
		case ERR_QROVERNOTSTART:
			{
				screen_display_infor("系统提示","二维码未到启用时间!!!", "交易取消", " ");
				break;
			}
		case ERR_QROVERAMT:
			{
				screen_display_infor("系统提示","二维码金额超限!!!", "交易取消", " ");
				break;
			}
		case ERR_ERRMAC2:
			{
				screen_display_infor("系统提示","MAC无效!!!", "交易取消", " ");
				break;
			}
		case ERR_ERRBLICKLOCKCARD:
			{
				screen_display_infor("系统提示","黑名单锁卡失败!!!", "交易取消", " ");
				break;
			}
		case ERR_ERRCARDISSUERCODE:
			{
				screen_display_infor("系统提示","发行机构不支持!!!", "交易取消", " ");
				break;
			}
		case ERR_QRCODEUSEMODE:
			{
				screen_display_infor("系统提示","不支持使用范围!!!", "交易取消", " ");
				break;
			}

		case ERR_QRCODELEN:
			{
				screen_display_infor("系统提示","二维码长度错误", "交易取消", " ");
				break;
			}
		case ERR_ERRTERID:
			{
				screen_display_infor("系统提示","终端号非法", "交易取消", " ");
				break;
			}
		case ERR_SERV:
			{
				screen_display_infor("系统提示","系统方故障", "交易取消", " ");
				break;
			}
		case ERR_TRADEFIAL:
			{
				screen_display_infor("系统提示","交易失败", "交易取消", " ");
				break;
			}

		case ERROR_UNION_QR_AUTHEN:
			{
				screen_display_infor("系统提示","二维码验签失败", "交易取消", " ");
				break;
			}
		case ERR_NO_UNION_QR_CERT:
			{
				screen_display_infor("系统提示","未找到二维码证书", "请检查证书更新", " ");
				break;
			}
		case ERR_MACKEYCHECKERR:
			{
				screen_display_infor("系统提示","MAC密钥解密失败", "请重新获取主密钥", " ");
				break;
			}
		case ERR_TDKEYCHECKERR:
			{
				screen_display_infor("系统提示","TDK密钥解密失败", "请重新获取主密钥", " ");
				break;
			}
		case ERR_DRIVER_NOT_LOGIN:
			{
				screen_display_infor("温馨提示","暂停刷卡", "请司机刷卡签到", " ");
				break;
			}
		case ERR_SLEEP_CARD:
			{
				screen_display_infor("系统提示","休眠卡", "拒绝交易", " ");
				break;
			}
		case ERR_MOT_QR_REPEAT:
			{
				screen_display_infor("系统提示","二维码重复", "请刷新二维码", " ");
				break;
			}
		case ERR_QR:
			{
				screen_display_infor("系统提示","无效二维码", "请检查二维码", " ");
				break;
			}

		default:
			{
				screen_display_infor("系统提示", " ", "未知错误", " ");
				break;
			}

	}
*/
	return;
}



