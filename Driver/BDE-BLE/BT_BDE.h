
#ifndef __BT_BDE_H__
#define __BT_BDE_H__

/*
****************************************************
*  文件名             : BT_BDE.h
*  作者               : --
*  版本               : V1.0
*  编写日期           : 2016--12--14--14--39--54
*  简介               : ！！C99标准！！
*  函数列表           : 
                        BT_BDE_OpsTypedef
                        BT_BDE_PriDataTypedef
                        BT_BDE_DriverTypedef
*  历史版本           : 
*****************************************************
*/

/* 常用宏定义 */
#if 1

#define BT_True                 (0xff)
#define BT_False                (0x00)

#define BT_CHK_PARAM(x)         {if((uint32_t)x == 0)return BT_False;}
#define BT_CHK_BIT(val,bit)     ((val) & (1 << (bit)))
#define BT_SET_BIT(val,bit)     {(val) |= (1 << (bit));}
#define BT_CLR_BIT(val,bit)     {(val) &= ~(1 << (bit));}
#define BT_ABS(x)               ((x) > 0 ? (x):(-(x)))
#define BT_CHK_LEN(pxStr,Len)   {if(strlen((const char *)(pxStr)) > (Len))return BT_False;}

#define BT_FLAG_RxLock          (0x01)

#endif
/* 蓝牙操作 */
typedef struct BT_BDE_OpsTypedef_
{
    /* 两者均有的操作 */
    (uint8_t)   (*setRole)              (uint8_t * Role);
    (uint8_t *) (*getRole)              (void);
    (uint8_t)   (*setName)              (uint8_t * Name);
    (uint8_t *) (*getName)              (void);
    (uint8_t)   (*setBR)                (uint8_t * BR);
    (uint8_t *) (*getBR)                (void);
    (uint8_t)   (*setTxDly)             (uint8_t * TxDly);
    (uint8_t *) (*getTxDly)             (void);
    (uint8_t)   (*setDBM)               (uint8_t * DBM);
    (uint8_t *) (*getDBM)               (void);
    (uint8_t)   (*setConnInt)           (uint8_t * min,uint8_t * max,uint8_t * lat,uint8_t * timeout);
    (uint8_t *) (*getConnInt)           (void);
    (uint8_t *) (*getAddr)              (void);
    (uint8_t)   (*disconnect)           (void);
    (uint8_t *) (*getStatus)            (void);
    (uint8_t)   (*saveConfigure)        (void);
    (uint8_t)   (*clearConfigure)       (void);
    (uint16_t)  (*sendData)             (uint16_t Len,uint8_t * Data);
    (uint8_t *) (*getVersion)           (void);
    /* 主模块特有的操作 */
    (uint8_t)   (*setScan)              (uint8_t * Scan);
    (uint8_t)   (*connect)              (uint8_t * Addr);
    (uint8_t)   (*setDirectConnAddr)    (uint8_t * DirectConnAddr);
    (uint8_t *) (*getDirectConnAddr)    (void);
    /* 从模块特有的操作 */
    (uint8_t)   (*setAdvInt)            (uint8_t * min,uint8_t * max);
    (uint8_t *) (*getAdvInt)            (void);
    (uint8_t)   (*setAdvData)           (uint8_t * data);
    (uint8_t *) (*getAdvData)           (void);
    (uint8_t)   (*setAdv)               (uint8_t * Adv);
    (uint8_t)   (*setConnectableAddr)   (uint8_t * ConnectableAddr);
    (uint8_t *) (*getConnectableAddr)   (void);
    
}BT_BDE_OpsTypedef;
/* 蓝牙数据 */
typedef struct BT_BDE_PriDataTypedef_
{
    /* 当前角色 */
    uint8_t CurRole;
    /* 字符分解输出 */
    uint8_t ParamSplit[6][16];
    /* 标志组 */
    uint16_t FlagGroup;
    /* 指令生成在此缓冲区中 */
    uint8_t InstructionBuff[60];
    uint8_t RxBuff[128];
    uint32_t Counter;
    
}BT_BDE_PriDataTypedef;
/* 串口驱动 */
typedef struct BT_UartTypedef_
{
    
    (uint8_t)(*Init)(uint16_t Baud);
    (uint8_t)(*Send)(uint8_t * Data,uint16_t Len);
    (uint8_t)(*Recv)(uint8_t * Data,uint16_t *Len);
    
}BT_UartTypedef;
/* 蓝牙驱动 */
typedef struct BT_BDE_DriverTypedef_
{
    BT_BDE_OpsTypedef       Ops;
    BT_UartTypedef          Uart;
    BT_BDE_PriDataTypedef   PriData;

    (uint8_t)(*TimingProcess)(uint16_t Period);
    
}BT_BDE_DriverTypedef;


#endif


