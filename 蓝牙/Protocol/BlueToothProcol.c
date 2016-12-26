
#include "BT_Config.h"

static uint8_t BT_Buff[512] = {0x00};
static DataCostTypedef DataCost = {0};
static DataAddupTypedef DataAddup = {0};
static DataAddupCostTypedef DataAddupCost = {0};
static DataAttriTypedef DataAttri = {0};
static DataFileOpsTypedef DataFileOps = {0};
static BT_HandleTypedef BT_Handle = {&DataCost,&DataAddup,&DataAddupCost,&DataAttri,&DataFileOps};

/*
****************************************************
*  函数名         : SubString
*  函数描述       : 根据指定字符截取出子字符串 
                    ！待截取字符串被改变！
*  参数           : 
                        String:待截取字符串
                        Char：指定字符
                        Len：待截取字符串长度
                        WhichOne：截取第几个子字符串
*  返回值         : 子字符串的首地址，返回0异常
*  作者           : 
*  历史版本       : 
*****************************************************
*/
static uint8_t * SubString(uint8_t * String,uint16_t Len,uint8_t WhichOne)
{
    uint16_t i = 0,MatchStringCnt = 0,MatchCharCnt = 0;
    uint8_t * SubStart = 0,SubEnd = 0;
    //参数检测
    if(String == 0x00 || WhichOne == 0 || Len == 0)return 0;
    /*
        循环截取字符串    <！待截取字符串被改变！>
        
        根据蓝牙版小额支付中断协议，指定字符为'$'，成对出现，由此我们检测
        '$'的个数，当为偶数则判定一条子字符串结束
        
    */
    while(String[i] && i < Len)
    {
        if(String[i] == '$')
        {
            MatchCharCnt++;
            if(MatchCharCnt % 2 == 0)
            {
                MatchStringCnt++;
                //截断字符串
                String[i] = 0x00;
                if(MatchStringCnt >= WhichOne)return SubStart;
            }
            if(MatchCharCnt % 2 == 1)
            {
                //获取首地址
                SubStart = &String[i + 1];
            }
        }
        i++;
    }
}
/*
****************************************************
*  函数名         : ReturnComAndParam
*  函数描述       : 返回命令和参数
*  参数           : 
*  返回值         : 
*  作者           : --
*  历史版本       : 
*****************************************************
*/
static ComdTypedef ReturnComAndParam(uint8_t * String,uint8_t ** Com,uint8_t ** Param)
{
    uint8_t * ComTemp = 0,* ParamTem = 0,i = 0;
    
    if(Com == 0 || Param == 0)return Result_InvaildParam;
    
    ComTemp = String;
    
    while(String[i] != '=' && i < 10)i ++;
    
    if(i < 10)
    {
        String[i] = 0x00;
        ParamTem = String[i + 1];
        *Com = ComTemp;
        *Param = ParamTem;
        return Result_True;
    }
    
    return Result_False;
}
/*
****************************************************
*  函数名         : Check
*  函数描述       : 检查数据是否有效
*  参数           : 
*  返回值         : 
*  作者           : 
*  历史版本       : 
*****************************************************
*/
static ComdTypedef Check(uint8_t * Data)
{
    p_ReqTypedef ReqTemp = (p_ReqTypedef)Data;
    //检查帧头
    if(strcmp(ReqTemp->HEAD.Name,"SDsEs") != 0)return Result_HeaderError;
    
    {
        //获取并比较CRC
        uint16_t CrcTemp = Data[ReqTemp->LEN] | (Data[ReqTemp->LEN - 1] << 8);
        if(CrcTemp != CRC(ReqTemp->COMR,ReqTemp->LEN))return Result_CrcError;
    }
    
    return Result_True;
}

/*
****************************************************
*  函数名         : BT_Parse
*  函数描述       : 解析蓝牙数据
*  参数           : 
*  返回值         : 
*  作者           : -5A4A5943-
*  历史版本       : 
*****************************************************
*/
ComdTypedef BT_Parse(uint8_t * Data,uint16_t Len)
{
    ComdTypedef Comd = Result_False;
    p_ReqTypedef ReqTemp = 0x00;
    uint8_t LenTemp = 0;
    
    if(Data == 0 || Len == 0)return Result_ValidParam;
    
#if Enable_Base64//使用Base64加密 
   
    if(Data[Len - 1] == '=')
    {
        LenTemp = 1;//被加密信息长度余2
        if(Data[Len - 2] == '=')
        {
            LenTemp = 2;//被加密信息长度余1
        }
    }
    //解密出的信息的长度
    LenTemp = (Len / 4) * 3 - LenTemp;
    
    Base64.Decode(Data,Len,BT_Buff);
    
#else   
    
    memcpy(BT_Buff,Data,Len);
#endif
    
    ReqTemp = (p_ReqTypedef)BT_Buff;
    
    Comd = Check(BT_Buff);
    if(Comd != Result_True)return Comd;
    
    switch(ReqTemp->CMDR)
    {
        /* 减去CMDR和CRC长度 */
        uint16_t ComdLen = ReqTemp->LEN - 4;
        
        case COMD_ManualCost:
        case COMD_Cost:
        {
            uint16_t i = 0;
            uint8_t * Comd = ReqTemp->DATA;
            uint8_t Len = 0;
            
            while(i < ComdLen)
            {
                switch(Comd[i])
                {
                    /*2016--12--26--16--40--43(ZJYC): Comd[i] = 0x00;必要性   */ 
                    case TAG_ConfirmWeight:
                    {
                        Comd[i] = 0x00;
                        Len = Comd[i + 1];
                        DataCost.Weight.Buf = Comd[i + 2];
                        i += Len + 2;
                        break;
                    }
                    case TAG_ConfirmUintPrice:
                    {
                        Comd[i] = 0x00;
                        DataCost.UnitPrice.Buf = Comd[i + 1];
                        i += 6;break;}
                    case TAG_ConfirmTotalPrice:{Comd[i] = 0x00;DataCost.Total.Buf = Comd[i + 1];i += 11;break;}
                    case TAG_ConfirmPLU:{Comd[i] = 0x00;DataCost.PLU.Buf = Comd[i + 1];break;}
                    case TAG_ConfirmTrace:{Comd[i] = 0x00;DataCost.Trace.Buf = Comd[i + 1];break;}
                    default:break;
                }
                if(i == ComdLen)Comd[ComdLen] = 0x00;
            }
            return COMD_Cost;
        }

        case COMD_Return:return COMD_Return;
        
        case COMD_Menu:return COMD_Menu;
        
        case COMD_Addup :
        {
            uint16_t i = 0;
            uint8_t * Comd = ReqTemp->DATA;
            while(i < ComdLen)
            {
                switch(Comd[i])
                {
                    case TAG_AddupWeight:{DataAddup.Weight.Buf = Comd[i + 1];break;}
                    case TAG_AddupUintPrice:{Comd[i] = 0x00;DataAddup.UnitPrice.Buf = Comd[i + 1];break;}
                    case TAG_AddupTotalPrice:{Comd[i] = 0x00;DataAddup.Total.Buf = Comd[i + 1];break;}
                    case TAG_AddupPLU:{Comd[i] = 0x00;DataAddup.PLU.Buf = Comd[i + 1];break;}
                    case TAG_AddupTrace:{Comd[i] = 0x00;DataAddup.Trace.Buf = Comd[i + 1];break;}
                    case TAG_AddupCount:{Comd[i] = 0x00;DataAddup.Count.Buf = Comd[i + 1];break;}
                    default:break;
                }
                i++;if(i == ComdLen)Comd[ComdLen] = 0x00;
            }
            return COMD_Addup;

        }
        
        case COMD_AddupClear:return COMD_AddupClear;
        case COMD_AddupCost:
        {
            uint16_t i = 0;
            uint8_t * Comd = ReqTemp->DATA;
            while(i < ComdLen)
            {
                switch(Comd[i])
                {
                    case TAG_AddupCostTotalPrice:{DataAddupCost.TotalPrice.Buf = Comd[i + 1];break;}
                    case TAG_AddupCostCount:{Comd[i] = 0x00;DataAddupCost.Count.Buf = Comd[i + 1];break;}
                    default:break;
                }
                i++;if(i == ComdLen)Comd[ComdLen] = 0x00;
            }
            return COMD_AddupCost;
        }
        
        case COMD_NumberKey:{DataAttri.NumKey = ReqTemp->DATA;return COMD_NumberKey;}
        
        case COMD_GetVersion:return COMD_GetVersion;
        
        case COMD_FileOps:
        {
            uint8_t * Data = ReqTemp->DATA,*Temp = 0,i = 0,Buf[20] = 0;
            uint8_t * Commd = 0,*Param = 0;
            
            while(1)
            {
                Temp = SubString(Data,ComdLen,i++);
                if(Temp == 0)break;
                if(ReturnComAndParam(Temp,&Commd,&Param) != Result_True)continue;
                if(strcmp(Temp,"CMD") == 0){strcpy(DataFileOps.Str_CMD,Param);}
                if(strcmp(Temp,"FILENAME") == 0){strcpy(DataFileOps.Str_FILENAME,Param);}
                if(strcmp(Temp,"FILETYPE") == 0){strcpy(DataFileOps.Str_FILETYPE,Param);}
                if(strcmp(Temp,"FILELEN") == 0){strcpy(DataFileOps.Str_FILELEN,Param);}
                if(strcmp(Temp,"SUMPACK") == 0){strcpy(DataFileOps.Str_SUMPACK,Param);}
                if(strcmp(Temp,"NOWPACK") == 0){strcpy(DataFileOps.Str_NOWPACK,Param);}
                if(strcmp(Temp,"PACKLEN") == 0){strcpy(DataFileOps.Str_PACKLEN,Param);}
                if(strcmp(Temp,"PACKBUF") == 0){DataFileOps.Str_PACKBUF = Param;}
            }
            return COMD_FileOps;
        }
        
        default:break;
    }
    
    return Result_False;
    
}

p_BT_HandleTypedef BT_GetHandle(void)
{
    return &BT_Handle;
}





