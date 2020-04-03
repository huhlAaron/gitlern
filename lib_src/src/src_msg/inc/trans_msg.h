#ifndef _TRANS_MSG_H
#define _TRANS_MSG_H

/***********************[宏定义]*****************************************/
/*
    ------------测试环境接入参数
    接口测试环境  地址：
    消费数据上传     http://123.13.201.82:18085/produce/produce
    其他接口         http://123.13.201.82:18086/pos
*/
#define TEST_SERPORT_PARA       18086
#define TEST_HOST_PARA          "123.13.201.82:18086/pos"
#define TEST_SERPORT_DEAL       18085
#define TEST_HOST_DEAL          "123.13.201.82:18085/produce"

/*
    ------------正式环境接入参数
*/
#define REAL_SERPORT_PARA       18086
#define REAL_HOST_PAR           " "
#define REAL_SERPORT_DEAL       18086
#define REAL_HOST_DEAL          " "

#define PORTPARA            TEST_SERPORT_PARA       //    REAL_SERPORT             //其他参数端口
#define HOSTPARA            TEST_HOST_PARA          //    REAL_HOST_PARA           //其他参数地址
#define PORTDEAL            TEST_SERPORT_DEAL       //    REAL_SERPORT             //消费数据上传端口
#define HOSTDEAL            TEST_HOST_DEAL          //    REAL_HOST_DEA            //消费数据上传地址


#endif /* _TRANS_MSG_H */
