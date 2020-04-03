/*****************************************************************************//**
*  - Copyright (C) 1.0.0.1, XXX Co. Ltd.
*  - All rights reserved.
*                  
*  @file     gmalg_inc.h 
*  @brief    描述当前文档
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 14:41:27
*  @note     GNU General Public License (GPL) *
*  @remark   Description *
* - ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐*
* - Change History *
* - <Date> | <Version> | <Author> | <Description> *
* - 2020年1月17日 | 1.0.0.1 | yufupeng \@decard | Create file *
* - ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐*
********************************************************************************/
#ifndef _GMALG_INC_H_
#define _GMALG_INC_H_
/***********************[本地应用头文件]*********************************/


/***********************[宏定义]*****************************************/
#define IN
#define OUT

#ifndef U8
#define	 U8 unsigned char
#endif /* U8 */

#ifndef U16
#define  U16 unsigned short
#endif /* U16 */

#ifndef U32
#define  U32 unsigned int
#endif /* U32 */

#ifndef S8
#define	 S8 signed char
#endif /* S8 */

#ifndef S16
#define  S16 signed short
#endif /* S16 */

#ifndef S32
#define  S32 signed int
#endif /* S32 */

#ifndef uint
#define uint unsigned int
#endif /* uint */

#ifndef uchar
#define  uchar unsigned char
#endif/* uchar */

#ifndef byte
//#define byte (unsigned char)
typedef unsigned char byte;
#endif/* byte */

#ifndef ushort
#define ushort unsigned short
#endif/* ushort */

#ifndef ulong
#define ulong unsigned long 
#endif/* ulong */

#ifndef uint64
#define uint64 unsigned long long int
#endif/* uint64 */


/**********************[结构体/全局变量定义]****************************/

/**********************************************************************************************//**
* @name 分组说明文字
* @author yufupeng \@decard
* @date   2020年1月17日 14:51:07
* @{
*/
typedef enum SM2_MODE_enum
{
    SM2_Mode_C1C2C3 = 1,
    SM2_Mode_C1C3C2 = 2
}SM2_MODE_E;
/** @} */
/**********************[结构体/全局变量引用声明]************************/


/**********************[接口定义]****************************************/


/**********************************************************************************************//**
*  @fn       int GMAlg_Sm2Encrypt(IN SM2_MODE_E iMode,IN U8 *pbPlainText,IN int iPlainTextLen,IN U8 *pbPubKey,IN int iPubKeyLen,OUT U8 *pbCipherText,OUT int *piCipherTextLen);
*  @brief    SM2加密
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 14:53:32
*  @param	[in]  iMode 		  即密文输出模式C1C2C3/C1C3C2 \sa SM2_MODE_E
*  @param	[in]  pbPlainText	  明文输入
*  @param	[in]  iPlainTextLen   明文长度
*  @param	[in]  pbPubKey		  公钥
*  @param	[in]  iPubKeyLen	  公钥长度
*  @param	[out] pbCipherText	  密文输出
*  @param	[out] piCipherTextLen 密文长度
*  @return	 -	0 成功   其他失败
***************************************************************************************************/
int GMAlg_Sm2Encrypt(IN SM2_MODE_E iMode,IN U8 *pbPlainText,IN int iPlainTextLen,IN U8 *pbPubKey,IN int iPubKeyLen,OUT U8 *pbCipherText,OUT int *piCipherTextLen);



/**********************************************************************************************//**
*  @fn       int GMAlg_Sm2Decrypt(IN SM2_MODE_E iMode, IN U8 *pbCipherText, IN int iCipherTextLen,IN U8 *pbPriKey,IN int iPriKeyLen,OUT U8 *pbPlainText,OUT int *piPlainTextLen);
*  @brief    SM2解密
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 14:54:36
*  @param	[in]  iMode 		   即密文输入模式C1C2C3/C1C3C2 \sa SM2_C1C2C3
*  @param	[in]  pbCipherText	   密文输入
*  @param	[in]  iCipherTextLen   密文长度
*  @param	[in]  pbPriKey		   私钥
*  @param	[in]  iPriKeyLen	   私钥长度
*  @param	[out] pbPlainText	   明文输出
*  @param	[out] piPlainTextLen   明文长度
*  @return	 -	0成功 其他失败
***************************************************************************************************/
int GMAlg_Sm2Decrypt(IN SM2_MODE_E iMode, IN U8 *pbCipherText, IN int iCipherTextLen,IN U8 *pbPriKey,IN int iPriKeyLen,OUT U8 *pbPlainText,OUT int *piPlainTextLen);



/**********************************************************************************************//**
*  @fn       int GMAlg_Sm4Decrypt(IN U8 *pKey, IN U8 *pInData, OUT U8*pOutData);
*  @brief    SM4解密
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 13:44:13
*  @param   [in]    pKey sm4密钥   
*  @param   [in]    pInData 输入数据
*  @param   [out]   pOutData 输出数据
*  @return  - 0成功 其他失败
***************************************************************************************************/
int GMAlg_Sm4Decrypt(IN U8 *pKey, IN U8 *pInData, OUT U8*pOutData);


/**********************************************************************************************//**
*  @fn       int GMAlg_Sm4Encrypt(IN U8 *pKey, IN U8 *pInData, OUT U8*pOutData);
*  @brief    SM4加密
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 13:44:13
*  @param   [in]    pKey sm4密钥   
*  @param   [in]    pInData 输入数据
*  @param   [out]   pOutData 输出数据
*  @return  - 0成功 其他失败
***************************************************************************************************/
int GMAlg_Sm4Encrypt(IN U8 *pKey, IN U8 *pInData, OUT U8*pOutData);


/**********************************************************************************************//**
*  @fn       int DC_CreateSM4CBCMAC(IN U8 *MacKey, IN U8 *InData, IN U32 InLen, OUT U8 *OutMac);
*  @brief    德卡平台MAC， CBC模式计算SM4 MAC
*  
*  @author   yufupeng \@decard
*  @date     2020年1月17日 13:45:51
*  @param   [in]    MacKey 计算MAC密钥 HEX格式
*  @param   [in]    InData 计算MAC的数据
*  @param   [in]    InLen  计算MAC的数据长度
*  @param   [out]   OutMac 计算的MAC
*  @return  - 0成功 其他失败
***************************************************************************************************/
int DC_CreateSM4CBCMAC(IN U8 *MacKey, IN U8 *InData, IN U32 InLen, OUT U8 *OutMac);


#endif /*gmalg_inc.h*/
/**************************************************************************
**********************************【 gmalg_inc.h 结束】*******************
*************************************************************************
*/

