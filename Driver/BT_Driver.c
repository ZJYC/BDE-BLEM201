/*
****************************************************
*  文件名             : 
*  作者               : 
*  版本               : 
                        1.0     没有失败重传机制
*  编写日期           : 
*  简介               : 
                        直接看本文件末尾：结构体定义 BT_DriverTypedef BT_Driver
*  函数列表           : 
*  历史版本           : 
                            2016--11--23--09--03--48:加入工程进行编译
                            2016--11--23--11--25--30:终于将所有的error去掉了，接下来进行测试
                            2016--11--24--15--12--46:测试通过，可以在MCU控制下连接BM78蓝牙，进入透传模式
                            2016--11--29--09--36--41:考虑着适当的加一些说明和注释
*****************************************************
*/

/*头文件  */

#include "BT_Driver.h"
#include "ComAppAPI.h"
#include "BT_Task.h"

/*宏定义  */

//此项指定了蓝牙协议中参数最多有多少个
#define BT_MAX_PARAMS       8
//参数最长是多少
#define BT_MAX_PARAM_LEN    24



/*变量定义*/

static uint8_t TxBuf[100] = {0x00};
static uint8_t RxBuf[100] = {0x00};
static uint8_t TxLen = 0x00;
static uint8_t RxLen = 0x00;
static uint8_t *TempArray[BT_MAX_PARAMS] = {0x00};


/*变量声明*/


extern int comlen;
extern volatile uint8_t ScalRxTrig;
extern uint8_t Aprxbuff[MaxPcApRx];




/*函数声明*/

static BT_ResTypedef BT_EnsureInited(void)
{
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_Inited) == 0 && BT_Driver.Init() == BT_Res_OK)return BT_Res_OK;
    return BT_Res_ERROR;
}

/*
****************************************************
*  函数名         : 获取互斥体
*  函数描述       : 表示获取了对相关资源的占有权，类似于操作系统的互斥体，线程安全？
*  参数           : 
*  返回值         : 
*  作者           : 
*  历史版本       : 
*****************************************************
*/
static BT_ResTypedef BT_GetMutex(uint16_t HowLong)
{
    uint32_t CurCnt = 0;
    CurCnt = BT_Driver.Inf.prvCounter;
    //等待0.5S
    if(HowLong != 0)while(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_MUTEX) && BT_ABS(BT_Driver.Inf.prvCounter - CurCnt) < HowLong);
    //如果当前已空闲，则占有此标志
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_MUTEX) == 0)
    {
        BT_SET_BIT(BT_Driver.Inf.prvState,BT_MUTEX);
        return BT_Res_OK;
    }
    return BT_Res_ERROR;
}
/*
****************************************************
*  函数名         : 释放互斥体
*  函数描述       : 放手对相关资源的占有权，其他函数得以使用其资源
*  参数           : 
*  返回值         : 
*  作者           : 
*  历史版本       : 
*****************************************************
*/
static BT_ResTypedef BT_FreeMutex(void)
{
    BT_CLR_BIT(BT_Driver.Inf.prvState,BT_MUTEX);
    return BT_Res_OK;
}

/*
输入："\r\n+NAME:name\r\n\r\nOK\r\n"
输出：Out[0] = "NAME",Out[1] = "name",Out[2] = "OK"
*/
static BT_ResTypedef GetInfFromFrame(uint8_t * In,uint8_t ** Out,uint16_t Len)
{
    //CharUseless：标明一个无用字符
    uint8_t i = 0,tIndex = 0,CharUseless = 0;
    
    BT_CHK_PARAM(In);
    BT_CHK_PARAM(Out);
    BT_CHK_PARAM(Len);
    //对字符串的长度做一下限定
    while(In[i] && i < Len)
    {
        //跳过这6个无用字符
        if(In[i] == '\r' || In[i] == '\n' || In[i] == '+' || In[i] == ':' || In[i] == ',')
        {
            In[i] = '\0';
            i ++;
            CharUseless = 0xff;
            continue;
        }
        //存储此字段的地址
        if(CharUseless == 0xff){Out[tIndex++] = &In[i];CharUseless = 0x00;}
        if(tIndex > BT_MAX_PARAMS)return BT_Res_TooManyParams;
        i ++;
    }
    
    return BT_Res_OK;
}

static uint8_t * GetGVER(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+GVER\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    if(GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen) != BT_Res_OK)return 0x00;
    if(strcmp((const char *)TempArray[1],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.GVER,(char *)TempArray[0]);
        BT_FreeMutex();
        return BT_Driver.Inf.GVER;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetNAME(BT_ResTypedef * Res)
{
    BT_CHK_PARAM((int)Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+NAME?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"NAME") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.NAME,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.NAME;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetPIN(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+PIN?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"PIN") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.PIN,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.PIN;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetCOD(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+CLASS?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"CLASS") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.COD,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.COD;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetLBDADDR(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+LBDADDR?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"LBDADDR") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.LBDADDR,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.LBDADDR;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetBAUD(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+BAUD?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"BAUD") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.BAUD,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.BAUD;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetBTMODE(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+BTMODE?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"BTMODE") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.BTMODE,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.BTMODE;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetIDLE(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+IDLE?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"IDLE") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.IDLE,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.IDLE;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetSSP(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+SSP?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"SSP") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.SSP,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.SSP;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetDISCOVERABLE(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+DISCOVERABLE?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"DISCOVERABLE") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.DISCOVERABLE,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.DISCOVERABLE;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}

static uint8_t * GetADV(BT_ResTypedef * Res)
{
    BT_CHK_PARAM(Res);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){*Res = BT_Res_ATNotAllowed;return 0x00;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+ADV?\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"ADV") == 0 && strcmp((const char *)TempArray[2],"OK") == 0)
    {
        strcpy((char *)BT_Driver.Inf.ADV,(char *)TempArray[1]);
        BT_FreeMutex();
        return BT_Driver.Inf.ADV;
    }
    else 
    {
        BT_FreeMutex();
        *Res = BT_Res_ERROR;
        return 0x00;
    }
}
/*
static const AT_GetTypedef Get = 
{
    .GVER        = GetGVER,
    .NAME        = GetNAME        ,
    .PIN         = GetPIN         ,
    .COD         = GetCOD         ,
    .LBDADDR     = GetLBDADDR     ,
    .BAUD        = GetBAUD        ,
    .BTMODE      = GetBTMODE      ,
    .IDLE        = GetIDLE        ,
    .SSP         = GetSSP         ,
    .DISCOVERABLE= GetDISCOVERABLE,
    .ADV         = GetADV         
};
*/
static BT_ResTypedef SetNAME(uint8_t * name)
{
    //检查参数是否合法
    BT_CHK_PARAM(name);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(name) > 31)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.NAME,(char *)name);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+NAME=");
    strcat((char *)TxBuf,(char *)name);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetPIN(uint8_t * PIN)
{
    //检查参数是否合法
    BT_CHK_PARAM(PIN);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(PIN) > 16)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.PIN,(char *)PIN);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+PIN=");
    strcat((char *)TxBuf,(char *)PIN);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetCOD(uint8_t * COD)
{
    //检查参数是否合法
    BT_CHK_PARAM(COD);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(COD) > 16)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.COD,(char *)COD);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+CLASS=");
    strcat((char *)TxBuf,(char *)COD);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetBAUD(uint8_t * BAUD)
{
    //检查参数是否合法
    BT_CHK_PARAM(BAUD);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(BAUD) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.BAUD,(char *)BAUD);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+BAUD=");
    strcat((char *)TxBuf,(char *)BAUD);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetBTMODE(uint8_t * BTMODE)
{
    //检查参数是否合法
    BT_CHK_PARAM(BTMODE);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(BTMODE) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.BTMODE,(char *)BTMODE);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+BTMODE=");
    strcat((char *)TxBuf,(char *)BTMODE);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetIDLE(uint8_t * IDLE)
{
    //检查参数是否合法
    BT_CHK_PARAM(IDLE);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(IDLE) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.IDLE,(char *)IDLE);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+IDLE=");
    strcat((char *)TxBuf,(char *)IDLE);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetSSP(uint8_t * SSP)
{
    //检查参数是否合法
    BT_CHK_PARAM(SSP);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen((const char *)SSP) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.SSP,(char *)SSP);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+SSP=");
    strcat((char *)TxBuf,(char *)SSP);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetDISCOVERABLE(uint8_t * SSP)
{
    //检查参数是否合法
    BT_CHK_PARAM(SSP);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(SSP) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.DISCOVERABLE,(char *)SSP);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+DISCOVERABLE=");
    strcat((char *)TxBuf,(char *)SSP);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetADV(uint8_t * ADV)
{
    //检查参数是否合法
    BT_CHK_PARAM(ADV);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(ADV) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.ADV,(char *)ADV);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+ADV=");
    strcat((char *)TxBuf,(char *)ADV);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetENTERTESTMODE(uint8_t * ENTERTESTMODE)
{
    //检查参数是否合法
    BT_CHK_PARAM(ENTERTESTMODE);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(ENTERTESTMODE) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.ENTERTESTMODE,ENTERTESTMODE);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+ENTERTESTMODE=");
    strcat((char *)TxBuf,(char *)ENTERTESTMODE);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetTXFREQ(uint8_t * TXFREQ)
{
    //检查参数是否合法
    BT_CHK_PARAM(TXFREQ);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(TXFREQ) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    BT_GetMutex(500);
    strcpy((char *)BT_Driver.Inf.TXFREQ,(char *)TXFREQ);
    //合成待发送数据
    strcpy((char *)TxBuf,"AT+TXFREQ=");
    strcat((char *)TxBuf,(char *)TXFREQ);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetLERXTEST(uint8_t * LERXTEST)
{
    //检查参数是否合法
    BT_CHK_PARAM(LERXTEST);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+LERXTEST=");
    strcat((char *)TxBuf,(char *)LERXTEST);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef SetLETXTEST(uint8_t * frequency,uint8_t * length,uint8_t * type)
{
    //检查参数是否合法
    BT_CHK_PARAM(frequency);
    BT_CHK_PARAM(length);
    BT_CHK_PARAM(type);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+LETXTEST=");
    strcat((char *)TxBuf,(char *)frequency);
    strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)length);
    strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)type);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}
/*
static const AT_SetTypedef Set = 
{
    .NAME         = SetNAME         ,
    .PIN          = SetPIN          ,
    .COD          = SetCOD          ,
    .BAUD         = SetBAUD         ,
    .BTMODE       = SetBTMODE       ,
    .IDLE         = SetIDLE         ,
    .SSP          = SetSSP          ,
    .DISCOVERABLE = SetDISCOVERABLE ,
    .ADV          = SetADV          ,
    .ENTERTESTMODE= SetENTERTESTMODE,
    .TXFREQ       = SetTXFREQ       ,
    .LERXTEST     = SetLERXTEST     ,
    .LETXTEST     = SetLETXTEST     
};
*/
static BT_ResTypedef CtrTest(void)
{
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT\r");
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef CtrUSERCONFIRM(uint8_t * USERCONFIRM)
{
    //检查参数是否合法
    BT_CHK_PARAM(USERCONFIRM);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(USERCONFIRM) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.USERCONFIRM,USERCONFIRM);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+USERCONFIRM=");
    strcat((char *)TxBuf,(char *)USERCONFIRM);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef CtrPASSKEY(uint8_t * accept,uint8_t * passkey)
{
    //AT+PASSKEY=<accept>,<passkey>\r
    //检查参数是否合法
    BT_CHK_PARAM(accept);
    BT_CHK_PARAM(passkey);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+PASSKEY=");
    strcat((char *)TxBuf,(char *)accept);
    strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)passkey);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef CtrCONNECT(uint8_t * conn_type,uint8_t * addr_type,uint8_t * addr)
{
    //检查参数是否合法
    BT_CHK_PARAM(conn_type);
    BT_CHK_PARAM(addr_type);
    BT_CHK_PARAM(addr);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+CONNECT=");
    strcat((char *)TxBuf,(char *)conn_type);strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)addr_type);strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)addr);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
        GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
        //已连接
        if(strcmp((const char *)TempArray[0],"IM_CONN") == 0)
        {
            BT_Driver.Inf.IM_CONN = BT_True;
            BT_Driver.Inf.IM_CONN_FAIL = BT_False;
            //表明进入透传模式
            BT_CLR_BIT(BT_Driver.Inf.prvState,BT_AT);
        }
        //断开连接
        if(strcmp((const char *)TempArray[0],"IM_CONN_FAIL") == 0)
        {
            BT_Driver.Inf.IM_CONN_FAIL = BT_True;
            BT_Driver.Inf.IM_CONN = BT_False;
            //依旧在AT模式
            BT_SET_BIT(BT_Driver.Inf.prvState,BT_AT);
        }
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef CtrFACTORYRESET(void)
{
    //检查参数是否合法
    //BT_CHK_PARAM(LERXTEST);
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+ FACTORYRESET\r");
    //strcat(TxBuf,LERXTEST);
    //strcat(TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;

}

static BT_ResTypedef CtrLETESTEND(void)
{
    //检查参数是否合法
    //BT_CHK_PARAM(LERXTEST);
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+LETESTEND\r");
    //strcat(TxBuf,LERXTEST);
    //strcat(TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

extern BT_SearchResItemTypedef BT_SearchRes[MaxSearchResNum];

static BT_ResTypedef CtrDISCOVERDEVICE(uint8_t * Length,uint8_t * Num,uint8_t * Type)
{
    uint8_t SearchResIndex = 0;
    //检查参数是否合法
    BT_CHK_PARAM(Length);
    BT_CHK_PARAM(Num);
    BT_CHK_PARAM(Type);
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+DISCOVERDEVICE=");
    strcat((char *)TxBuf,(char *)Length);strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)Num);strcat((char *)TxBuf,",");
    strcat((char *)TxBuf,(char *)Type);
    strcat((char *)TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        uint32_t cnt = 0;
        cnt = BT_Driver.Inf.prvCounter;
        //蓝牙模块已经收到命令，并开始搜索,这里等待蓝牙回复(10秒)
        while(BT_ABS(BT_Driver.Inf.prvCounter - cnt) < 10*1000)
        {
            //程序会死在这里
            BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
            cnt = BT_Driver.Inf.prvCounter;
            GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
            if(strcmp((const char *)TempArray[0],"INQUIRY COMPLETE!") == 0)
            {
                //搜索完成
                if(SearchResIndex == 0)
                {
                    BT_FreeMutex();
                    return BT_Res_NoResult;
                }
                break;
            }
            if(strcmp((const char *)TempArray[0],"INQUIRY") == 0)
            {
                strcpy((char *)BT_SearchRes[SearchResIndex].bdaddr,(char *)TempArray[1]);
                strcpy((char *)BT_SearchRes[SearchResIndex].class,(char *)TempArray[2]);
                strcpy((char *)BT_SearchRes[SearchResIndex].addr_type,(char *)TempArray[3]);
                strcpy((char *)BT_SearchRes[SearchResIndex].device_type,(char *)TempArray[4]);
                strcpy((char *)BT_SearchRes[SearchResIndex].name,(char *)TempArray[5]);
                strcpy((char *)BT_SearchRes[SearchResIndex].rssi,(char *)TempArray[6]);
                
                BT_SearchRes[SearchResIndex].Used = 0xff;
                SearchResIndex++;
            }
        }
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}

static BT_ResTypedef CtrSTOPDISCOVER(void)
{
    //AT+ STOPDISCOVER\r
    //检查参数是否合法
    //BT_CHK_PARAM(LERXTEST);
    //判断字符长度是否超限
    //if(strlen(LERXTEST) > 6)return BT_Res_InvalidParam;
    //保存数据到本地
    //strcpy(BT_Driver.Inf.LERXTEST,LERXTEST);
    //合成待发送数据
    if(BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT) == 0){return BT_Res_ATNotAllowed;}
    BT_GetMutex(500);
    strcpy((char *)TxBuf,"AT+STOPDISCOVER\r");
    //strcat(TxBuf,LERXTEST);
    //strcat(TxBuf,"\r");
    //发送并检查应答
    BT_Driver.LLDriver.SendByUart(TxBuf,strlen((char *)TxBuf));
    BT_Driver.LLDriver.RecvByUart(RxBuf,&RxLen);
    GetInfFromFrame(RxBuf,(uint8_t**)TempArray,RxLen);
    if(strcmp((const char *)TempArray[0],"OK") == 0 )
    {
        BT_FreeMutex();
        return BT_Res_OK;
    }
    BT_FreeMutex();
    return BT_Res_ERROR;
}
/*
static const AT_CtrTypedef Ctr = 
{
    .Test            = CtrTest          ,
    .USERCONFIRM     = CtrUSERCONFIRM   ,
    .PASSKEY         = CtrPASSKEY       ,
    .CONNECT         = CtrCONNECT       ,
    .FACTORYRESET    = CtrFACTORYRESET  ,
    .LETESTEND       = CtrLETESTEND     ,
    .DISCOVERDEVICE  = CtrDISCOVERDEVICE,
    .STOPDISCOVER    = CtrSTOPDISCOVER  
};
*/

static BT_ResTypedef InitUart (uint32_t Baud)
{
    //已在主体实现
    return BT_Res_OK;
}

static BT_ResTypedef SendByUart(uint8_t * Data,uint8_t  Length)
{
    ComSend(COM2,Data,Length);
    //此部分需用串口实现
    return BT_Res_OK;
}

static BT_ResTypedef RecvByUart(uint8_t * Data,uint8_t *Length)
{
    uint16_t i = 0;
    while(!ScalRxTrig);
    ScalRxTrig = 0;
    //Data = Aprxbuff;
    *Length = comlen;
    for(i = 0;i < comlen;i ++)
    {
        Data[i] = Aprxbuff[i];
    }
    return BT_Res_OK;
}
/*
static BT_LLDriverTypedef LLDriver = 
{
    .SendByUart = SendByUart,
    .RecvByUart = RecvByUart
};
*/
static BT_ResTypedef BT_Init(void)
{
    BT_ResTypedef Res = (BT_ResTypedef)0xFFFF;
    
    if(BT_Driver.LLDriver.InitUart(115200) != BT_Res_OK)return BT_Res_UartFaild;
    
    BT_Driver.Reset();
    //上电即为AT模式
    BT_SET_BIT(BT_Driver.Inf.prvState,BT_AT);
    //等待设备就绪
    while(BT_Driver.Inf.IM_READY == BT_True);
    
    BT_Driver.Get.NAME(&Res);
    BT_Driver.Get.PIN(&Res);
    BT_Driver.Get.GVER(&Res);
    BT_Driver.Get.BAUD(&Res);
    BT_Driver.Get.LBDADDR(&Res);
    BT_Driver.Get.ADV(&Res);
    BT_Driver.Get.BTMODE(&Res);
    BT_Driver.Get.COD(&Res);
    BT_Driver.Get.DISCOVERABLE(&Res);
    BT_Driver.Get.IDLE(&Res);
    BT_Driver.Get.SSP(&Res);
    
    BT_Driver.Set.PIN((uint8_t *)"1234");
    BT_Driver.Set.NAME((uint8_t *)"BT-ZJYC-Test");
    
    BT_Driver.Ctr.Test();
    //BT_Driver.Ctr.DISCOVERDEVICE((uint8_t *)"10",(uint8_t *)"10",(uint8_t *)"3");
    BT_Driver.Ctr.CONNECT((uint8_t *)"0",(uint8_t *)"0",(uint8_t *)"8CDE52C15C56");
    
    BT_SET_BIT(BT_Driver.Inf.prvState,BT_Inited);
    return BT_Res_OK;
}

static BT_ResTypedef BT_Timing(uint8_t Interval)
{
    uint16_t Length = 0,i = 0;
    
    BT_Driver.Inf.prvCounter += Interval;
    
    if(BT_Driver.Inf.prvCounter >= 100000)BT_Driver.Inf.prvCounter = 0;
    
    if(BT_Driver.Inf.prvCounter % 1000)
    {
        //每1S你想做什么？？？以此类推吧
    }
    //被动接收到的信息我们在这里处理
    if(ScalRxTrig && (BT_CHK_BIT(BT_Driver.Inf.prvState,BT_AT)))
    {
        //获取互斥体失败则退出，BT_Timing很有可能在定时中断中调用，所以BT_GetMutex的参数必须为0
        //别人已经在使用了，本函数不予理解，因为此处只关心那些被动的AT信息
        if(BT_GetMutex(0) != BT_Res_OK)return BT_Res_ERROR;
        ScalRxTrig = 0;
        //获取长度并复制信息
        Length = comlen;
        for(i = 0;i < comlen;i ++)RxBuf[i] = Aprxbuff[i];
        //解析信息
        GetInfFromFrame(RxBuf,(uint8_t**)TempArray,Length);
        //设备就绪
        if(strcmp((const char *)TempArray[0],"IM_READY") == 0)
        {
            BT_Driver.Inf.IM_READY = BT_True;
            BT_SET_BIT(BT_Driver.Inf.prvState,BT_AT);
        }
        //已连接
        if(strcmp((const char *)TempArray[0],"IM_CONN") == 0)
        {
            BT_Driver.Inf.IM_CONN = BT_True;
            BT_Driver.Inf.IM_CONN_FAIL = BT_False;
            //表明进入透传模式
            BT_CLR_BIT(BT_Driver.Inf.prvState,BT_AT);
        }
        //断开连接
        if(strcmp((const char *)TempArray[0],"IM_CONN_FAIL") == 0)
        {
            BT_Driver.Inf.IM_CONN_FAIL = BT_True;
            BT_Driver.Inf.IM_CONN = BT_False;
            //依旧在AT模式
            BT_SET_BIT(BT_Driver.Inf.prvState,BT_AT);
        }
        //配对中验证码
        if(strcmp((const char *)TempArray[0],"IM_PAIR_PASSKEY") == 0)
        {
            BT_Driver.Inf.IM_PAIR_PASSKEY = BT_True;
        }
        BT_FreeMutex();
    }
    return BT_Res_OK;
}

static BT_ResTypedef BT_IsConnect(void)
{
    if(BT_Driver.Inf.IM_CONN == BT_True )
    {
        return BT_Res_OK;
    }
    if(BT_Driver.Inf.IM_CONN_FAIL == BT_True)
    {
        return BT_Res_ERROR;
    }
}

static BT_ResTypedef BT_Reset(void)
{
    //此处复位蓝牙模块，具体的引脚需要以后实现
    
    //复位相关标志位
    BT_Driver.Inf.IM_CONN = BT_False;
    BT_Driver.Inf.IM_CONN_FAIL = BT_True;
    BT_Driver.Inf.IM_READY = BT_False;
    BT_Driver.Inf.IM_PAIR_PASSKEY = BT_False;
}
/*
****************************************************
    为了支持如下的".xx = xxx,"的成员初始化，
    需在编译器中加入C99功能。
*****************************************************
*/
BT_DriverTypedef BT_Driver = 
{
    /* 
        通过 BT_Driver.Get.Name()的方式来获取蓝牙名称
        具体参数寻找相关函数实体
    */
    .Get = 
    {
        .GVER        = GetGVER,
        .NAME        = GetNAME        ,
        .PIN         = GetPIN         ,
        .COD         = GetCOD         ,
        .LBDADDR     = GetLBDADDR     ,
        .BAUD        = GetBAUD        ,
        .BTMODE      = GetBTMODE      ,
        .IDLE        = GetIDLE        ,
        .SSP         = GetSSP         ,
        .DISCOVERABLE= GetDISCOVERABLE,
        .ADV         = GetADV         
    },
    /* 
        通过 BT_Driver.Set.Name("BlueToothName")的方式来设置蓝牙的名称
        具体参数寻找相关函数实体
    */
    .Set = 
    {
        .NAME         = SetNAME         ,
        .PIN          = SetPIN          ,
        .COD          = SetCOD          ,
        .BAUD         = SetBAUD         ,
        .BTMODE       = SetBTMODE       ,
        .IDLE         = SetIDLE         ,
        .SSP          = SetSSP          ,
        .DISCOVERABLE = SetDISCOVERABLE ,
        .ADV          = SetADV          ,
        .ENTERTESTMODE= SetENTERTESTMODE,
        .TXFREQ       = SetTXFREQ       ,
        .LERXTEST     = SetLERXTEST     ,
        .LETXTEST     = SetLETXTEST     
    },
    /*
        通过 BT_Driver.Ctr.DISCOVERDEVICE("10","10","3") 的方式来搜索附近蓝牙
        具体参数寻找相关函数实体
    */
    .Ctr = 
    {
        .Test            = CtrTest          ,
        .USERCONFIRM     = CtrUSERCONFIRM   ,
        .PASSKEY         = CtrPASSKEY       ,
        .CONNECT         = CtrCONNECT       ,
        .FACTORYRESET    = CtrFACTORYRESET  ,
        .LETESTEND       = CtrLETESTEND     ,
        .DISCOVERDEVICE  = CtrDISCOVERDEVICE,
        .STOPDISCOVER    = CtrSTOPDISCOVER  
    },
    /*
    
        存储了蓝牙的一些attribute，至于有没有用，后期再说
    
    */
    .Inf = {0x00},
    /*
    
        MCU是通过串口与蓝牙模块通信的，如下提供了串口的接口，须由用户实现
    
    */
    .LLDriver = 
    {
        .InitUart = InitUart,
        .SendByUart = SendByUart,
        .RecvByUart = RecvByUart
    },
    /*
        
    */
    .Init = BT_Init,
    /*
        MCU定期调用BT_Driver.Timing(xx),所以，蓝牙的某些需要定时执行的process可以写在这里
    */
    .Timing = BT_Timing,
    .IsConnect = BT_IsConnect,
    .Reset = BT_Reset
};




















