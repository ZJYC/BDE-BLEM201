/*
****************************************************
*  文件名             : 
*  作者               : -5A4A5943-
*  版本               : 
*  编写日期           : 
*  简介               : 
*  函数列表           : 
*  历史版本           : 
*****************************************************
*/


/*头文件  */





/*宏定义  */





/*变量定义*/


BT_BDE_DriverTypedef BT_BDE_Driver = {0x00};


/*变量声明*/





/*函数声明*/


/*
In:指令输入
Len:指令长度
Out:把指令分解输出到二维字符数组

输入：“SPP: ok idle\r\n\0”
输出：Out[0] = "SPP",Out[1] = "ok",Out[2] = "idle"
*/
static uint8_t prvGetInfFromFrame(uint8_t * In,uint16_t Len,uint8_t ** Out)
{
    //CharUseless：标明一个无用字符
    uint8_t i = 0,tIndex = 0,CharUseless = 0;
    
    BT_CHK_PARAM(In);
    BT_CHK_PARAM(Out);
    BT_CHK_PARAM(Len);  /* 长度怎么能是0呢？是不是 */
    //对字符串的长度做一下限定
    while(In[i] && i < Len)
    {
        //跳过这4个无用字符
        if(In[i] == '\r' || In[i] == '\n' || In[i] == ':' || In[i] == " ")
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
    
    return BT_True;
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction1(uint8_t * Param1,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1的合法性应在上一级函数中检查 */
    
    /* 清除一下缓存 */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* 字符串操作 */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff,"/r/n");
    /* strcat 执行后会*自动*在dest后面添加'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen 不包括结束符，但是指令需要'/r/n/0'作为结束标志，故加一 */
    return Buff;
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction2(uint8_t * Param1,uint8_t * Param2,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1的合法性应在上一级函数中检查 */
    
    /* 清除一下缓存 */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* 字符串操作 */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff," ");
    strcat(Buff,Param2);
    strcat(Buff,"/r/n");
    /* strcat 执行后会*自动*在dest后面添加'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen 不包括结束符，但是指令需要'/r/n/0'作为结束标志，故加一 */
    return Buff;
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction3(uint8_t * Param1,uint8_t * Param2,uint8_t * Param3,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1的合法性应在上一级函数中检查 */
    
    /* 清除一下缓存 */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* 字符串操作 */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff," ");
    strcat(Buff,Param2);
    strcat(Buff," ");
    strcat(Buff,Param3);
    strcat(Buff,"/r/n");
    /* strcat 执行后会*自动*在dest后面添加'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen 不包括结束符，但是指令需要'/r/n/0'作为结束标志，故加一 */
    return Buff;
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction5(uint8_t * Param1,uint8_t * Param2,uint8_t * Param3,uint8_t * Param4,uint8_t * Param5,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1的合法性应在上一级函数中检查 */
    
    /* 清除一下缓存 */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* 字符串操作 */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff," ");
    strcat(Buff,Param2);
    strcat(Buff," ");
    strcat(Buff,Param3);
    strcat(Buff," ");
    strcat(Buff,Param4);
    strcat(Buff," ");
    strcat(Buff,Param5);
    strcat(Buff,"/r/n");
    /* strcat 执行后会*自动*在dest后面添加'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen 不包括结束符，但是指令需要'/r/n/0'作为结束标志，故加一 */
    return Buff;
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t prvSendInstruction(uint8_t * Instruction,uint16_t Len)
{
    uint32_t CurCounter = BT_BDE_Driver.PriData.Counter;
    uint8_t Retry = 0,Buf[100] = {0x00},**ParamSplitTemp = BT_BDE_Driver.PriData.ParamSplit;;
    uint16_t LenRecv = 0;
    
    BT_BDE_Driver.Ops.Send(Instruction,Len);
    
    for(;;)
    {
        /* 收到一帧信息 直接判断是否为 'ok' */
        if(BT_BDE_Driver.Ops.Recv(Buf,&LenRecv) != 0)
        {
            prvGetInfFromFrame(Buf,LenRecv,ParamSplitTemp);
            
            if( (strcmp(ParamSplitTemp[0],"SPP:") == 0 ) && (strcmp(ParamSplitTemp[1],"ok") == 0 ))
            {
                return BT_True
            }
        }
        /* 重传 */
        if((GetDelayed(CurCounter) > 200) && (++Retry < 6))
        {
            CurCounter = BT_BDE_Driver.PriData.Counter;
            BT_BDE_Driver.Ops.Send(Instruction,Len);
        }
        if(Retry >= 6)return BT_False;
    }
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint16_t prvSendData(uint8_t * Data,uint16_t Len)
{
    
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint8_t BT_BDE_TimingProcess(uint16_t Period)
{
    BT_CHK_PARAM(Period);
    
    BT_BDE_Driver.PriData.Counter += Period;
    if(BT_BDE_Driver.PriData.Counter > 100000)BT_BDE_Driver.PriData.Counter = 0;
    /* まい10个周期 你要做なん？？ */
    if(BT_BDE_Driver.PriData.Counter % (Period * 10) == 0)
    {
        /* 收到一帧数据 */
        /******************************************************
            理想情况下，此处全为透传数据，万一此处有指令数据
        我们可选的要在这里排除掉
        ******************************************************/
        if(BT_BDE_Driver.Ops.Recv(Buf,&LenRecv) != 0)
        {
            /* 我们首先要判断这是透传数据还是命令 */
            if(Buf[0] == 'S' && Buf[0] == 'P' &&Buf[0] == 'P' &&Buf[0] == ':')
            {
                
            }
        }
    }
}
/*
****************************************************
*  函数名         : 
*  函数描述       : 我们有必要知道我们等待了多长时间
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
static uint32_t GetDelayed(uint32_t ConstCounter)
{
    if(ConstCounter <= BT_BDE_Driver.PriData.Counter)return (BT_BDE_Driver.PriData.Counter - ConstCounter)
    return 100000 - ConstCounter + BT_BDE_Driver.PriData.Counter;
}


static uint8_t BT_UartInit(uint16_t Baud)
{
    
}
static uint8_t BT_UartSend(uint8_t * Data,uint16_t Len)
{
    
}
static uint8_t BT_UartRecv(uint8_t * Data,uint16_t *Len)
{
    /* 被别人锁定，返回BT_False */
    if(BT_CHK_BIT(BT_BDE_Driver.PriData.FlagGroup,BT_FLAG_RxLock))return BT_False;
    /* 锁定 以 线程安全 */
    BT_SET_BIT(BT_BDE_Driver.PriData.FlagGroup,BT_FLAG_RxLock);
    /* 

    if(RxFinished != 0)
    {
        for(i = 0;i < RxBufLen;i ++)
        {
            Data[i] = ComBuff[i];
        }
        *Len = RxBufLen;
        return BT_True;
    }

    */
    /* 解除锁定 */
    BT_CLR_BIT(BT_BDE_Driver.PriData.FlagGroup,BT_FLAG_RxLock);
}










