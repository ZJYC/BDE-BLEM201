/*
****************************************************
*  文件名             : 
*  作者               : 
*  版本               : 1.0
*  编写日期           : 
*  简介               : 蓝牙驱动（底层协议）
*  函数列表           : 
*  历史版本           : 
*****************************************************
*/

#ifndef __BT_DRIVER_H__
#define __BT_DRIVER_H__
/*头文件  */

#include "stm32f10x.h"
#include <string.h>
/* 串口驱动需要这里面的变量 */
#include "siriallink.h"


/*宏定义  */

/*2016--11--23--11--27--07(ZJYC): 宏定义冲突，此属无奈之举，由此，此文件不能被大部分文件包含   */ 
#ifdef PIN 
    #undef PIN
#endif

#define BT_True         0xff
#define BT_False        0x00

#define BT_Inited       0
#define BT_MUTEX        1
#define BT_AT           2

#define BT_CHK_PARAM(x)   if((uint32_t)x == 0)while(1);

#define BT_CHK_BIT(val,bit) ((val) & (1 << (bit)))
#define BT_SET_BIT(val,bit) {(val) |= (1 << (bit));}
#define BT_CLR_BIT(val,bit) {(val) &= ~(1 << (bit));}
#define BT_ABS(x)           ((x) > 0 ? (x):(-(x)))

/*变量定义*/

typedef enum BT_ResTypedef_
{
    
    BT_Res_OK = 0,
    
    BT_Res_InvalidParam = 1,//非法参数
    BT_Res_TooManyParams = 2,
    BT_Res_NoResult = 3,
    BT_Res_UartFaild = 4,
    //不在AT模式，不能使用AT指令
    BT_Res_ATNotAllowed = 5,
    
    BT_Res_ERROR = 0xFFFF
    
}BT_ResTypedef;
//底层驱动
typedef struct BT_LLDriverTypedef_
{
    //初始化串口并指定波特率
    BT_ResTypedef (*InitUart)(uint32_t Baud);
    //通过串口发送一堆数据
    BT_ResTypedef (*SendByUart)(uint8_t * Data,uint8_t  Length);
    //通过串口接收一堆数据
    BT_ResTypedef (*RecvByUart)(uint8_t * Data,uint8_t *Length);
    //后期这个结构体是要拓展的
}BT_LLDriverTypedef,*p_BT_LLDriverTypedef;
//数据存储
typedef struct BT_InfTypedef_
{
    //版本号
    uint8_t GVER[20];
    //设备名
    uint8_t NAME[31];
    //PIN码
    uint8_t PIN[16];
    //COD
    uint8_t COD[6];
    //本地蓝牙地址
    uint8_t LBDADDR[12];
    //波特率
    uint8_t BAUD[6];
    //模块模式
    uint8_t BTMODE[1];
    //空闲时间间隔
    uint8_t IDLE[10];
    //SSP模式
    uint8_t SSP[1];
    //可发现模式
    uint8_t DISCOVERABLE[1];
    //广播状态
    uint8_t ADV[1];
    //发射频率
    uint8_t TXFREQ[4];
    //电池电量
    uint8_t BATTERY[6];
    //就绪状态
    uint8_t IM_READY;
    //连接状态
    uint8_t IM_CONN;
    //连接失败
    uint8_t IM_CONN_FAIL;
    //需要输入配对KEY
    uint8_t IM_PAIR_PASSKEY;
    //私有计数器
    uint32_t prvCounter;
    //私有，状态
    uint32_t prvState;
}BT_InfTypedef,*p_BT_InfTypedef;

typedef struct AT_GetTypedef_
{
    //获取版本号
    uint8_t * (*GVER)        (BT_ResTypedef *);
    //获取名字
    uint8_t * (*NAME)        (BT_ResTypedef *);
    //获取PIN码
    uint8_t * (*PIN)         (BT_ResTypedef *);
    //获取类型
    uint8_t * (*COD)         (BT_ResTypedef *);
    //获取本机地址
    uint8_t * (*LBDADDR)     (BT_ResTypedef *);
    //获取波特率
    uint8_t * (*BAUD)        (BT_ResTypedef *);
    //获取蓝牙模式
    uint8_t * (*BTMODE)      (BT_ResTypedef *);
    //获取空闲模式
    uint8_t * (*IDLE)        (BT_ResTypedef *);
    //获取SSP模式
    uint8_t * (*SSP)         (BT_ResTypedef *);
    //获取是否可发现
    uint8_t * (*DISCOVERABLE)(BT_ResTypedef *);
    //获取广播模式
    uint8_t * (*ADV)         (BT_ResTypedef *);
}AT_GetTypedef,p_AT_GetTypedef;

typedef struct AT_SetTypedef_
{
    BT_ResTypedef (*NAME)         (uint8_t * name);
    BT_ResTypedef (*PIN)          (uint8_t * PinCode);
    BT_ResTypedef (*COD)          (uint8_t * cod);
    BT_ResTypedef (*BAUD)         (uint8_t * baud);
    BT_ResTypedef (*BTMODE)       (uint8_t * mode);
    BT_ResTypedef (*IDLE)         (uint8_t * interval);
    BT_ResTypedef (*SSP)          (uint8_t * mode);
    BT_ResTypedef (*DISCOVERABLE) (uint8_t * mode);
    BT_ResTypedef (*ADV)          (uint8_t * mode);
    BT_ResTypedef (*ENTERTESTMODE)(uint8_t * mode);
    BT_ResTypedef (*TXFREQ)       (uint8_t * value);
    BT_ResTypedef (*LERXTEST)     (uint8_t * frequency);
    BT_ResTypedef (*LETXTEST)     (uint8_t * frequency,uint8_t * length,uint8_t * type);
}AT_SetTypedef,*p_AT_SetTypedef;

typedef struct AT_CtrTypedef_
{
    //通讯测试
    BT_ResTypedef (*Test)(void);
    //配对中用户确认
    BT_ResTypedef (*USERCONFIRM)(uint8_t * accept);
    //配对中验证码
    BT_ResTypedef (*PASSKEY)(uint8_t * accept,uint8_t * passkey);
    //主动连接
    BT_ResTypedef (*CONNECT)(uint8_t * conn_type,uint8_t * addr_type,uint8_t * addr);
    //恢复出厂设置
    BT_ResTypedef (*FACTORYRESET)(void);
    //结束BLE测试
    BT_ResTypedef (*LETESTEND)(void);
    //搜索设备
    BT_ResTypedef (*DISCOVERDEVICE)(uint8_t * Length,uint8_t * Num,uint8_t * Type);
    //停止搜索设备
    BT_ResTypedef (*STOPDISCOVER)(void);
}AT_CtrTypedef,*p_AT_CtrTypedef;

typedef struct BT_DriverTypedef_
{
    //蓝牙获取指令
    AT_GetTypedef Get;
    //蓝牙设置指令
    AT_SetTypedef Set;
    //蓝牙控制指令
    AT_CtrTypedef Ctr;
    //蓝牙信息
    BT_InfTypedef Inf;
    //底层驱动
    BT_LLDriverTypedef LLDriver;
    //初始化
    BT_ResTypedef (*Init)(void);
    //定时执行流程
    BT_ResTypedef (*Timing)(uint8_t Interval);
    //判断是否连接
    BT_ResTypedef (*IsConnect)(void);
    //复位蓝牙模块
    BT_ResTypedef (*Reset)(void);
}BT_DriverTypedef,*p_BT_DriverTypedef;

/*变量声明*/


extern BT_DriverTypedef BT_Driver;


/*函数声明*/




#endif

