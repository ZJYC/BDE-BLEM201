/*
****************************************************
*  �ļ���             : 
*  ����               : -5A4A5943-
*  �汾               : 
*  ��д����           : 
*  ���               : 
*  �����б�           : 
*  ��ʷ�汾           : 
*****************************************************
*/


/*ͷ�ļ�  */





/*�궨��  */





/*��������*/


BT_BDE_DriverTypedef BT_BDE_Driver = {0x00};


/*��������*/





/*��������*/


/*
In:ָ������
Len:ָ���
Out:��ָ��ֽ��������ά�ַ�����

���룺��SPP: ok idle\r\n\0��
�����Out[0] = "SPP",Out[1] = "ok",Out[2] = "idle"
*/
static uint8_t prvGetInfFromFrame(uint8_t * In,uint16_t Len,uint8_t ** Out)
{
    //CharUseless������һ�������ַ�
    uint8_t i = 0,tIndex = 0,CharUseless = 0;
    
    BT_CHK_PARAM(In);
    BT_CHK_PARAM(Out);
    BT_CHK_PARAM(Len);  /* ������ô����0�أ��ǲ��� */
    //���ַ����ĳ�����һ���޶�
    while(In[i] && i < Len)
    {
        //������4�������ַ�
        if(In[i] == '\r' || In[i] == '\n' || In[i] == ':' || In[i] == " ")
        {
            In[i] = '\0';
            i ++;
            CharUseless = 0xff;
            continue;
        }
        //�洢���ֶεĵ�ַ
        if(CharUseless == 0xff){Out[tIndex++] = &In[i];CharUseless = 0x00;}
        if(tIndex > BT_MAX_PARAMS)return BT_Res_TooManyParams;
        i ++;
    }
    
    return BT_True;
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction1(uint8_t * Param1,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1�ĺϷ���Ӧ����һ�������м�� */
    
    /* ���һ�»��� */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* �ַ������� */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff,"/r/n");
    /* strcat ִ�к��*�Զ�*��dest�������'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen ������������������ָ����Ҫ'/r/n/0'��Ϊ������־���ʼ�һ */
    return Buff;
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction2(uint8_t * Param1,uint8_t * Param2,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1�ĺϷ���Ӧ����һ�������м�� */
    
    /* ���һ�»��� */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* �ַ������� */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff," ");
    strcat(Buff,Param2);
    strcat(Buff,"/r/n");
    /* strcat ִ�к��*�Զ�*��dest�������'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen ������������������ָ����Ҫ'/r/n/0'��Ϊ������־���ʼ�һ */
    return Buff;
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction3(uint8_t * Param1,uint8_t * Param2,uint8_t * Param3,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1�ĺϷ���Ӧ����һ�������м�� */
    
    /* ���һ�»��� */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* �ַ������� */
    strcpy(Buff,"SPP:");
    strcat(Buff,Param1);
    strcat(Buff," ");
    strcat(Buff,Param2);
    strcat(Buff," ");
    strcat(Buff,Param3);
    strcat(Buff,"/r/n");
    /* strcat ִ�к��*�Զ�*��dest�������'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen ������������������ָ����Ҫ'/r/n/0'��Ϊ������־���ʼ�һ */
    return Buff;
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint8_t * prvGenerateInstruction5(uint8_t * Param1,uint8_t * Param2,uint8_t * Param3,uint8_t * Param4,uint8_t * Param5,uint16_t * Len)
{
    uint8_t * Buff = BT_BDE_Driver.PriData.InstructionBuff;
    uint16_t i = 0;
    
    /* Param1�ĺϷ���Ӧ����һ�������м�� */
    
    /* ���һ�»��� */
    for(i = 0;i < 60;i ++)Buff[i] = 0x00;
    /* �ַ������� */
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
    /* strcat ִ�к��*�Զ�*��dest�������'\0' */
    *Len = strlen(Buff) + 1;
    /* strlen ������������������ָ����Ҫ'/r/n/0'��Ϊ������־���ʼ�һ */
    return Buff;
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
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
        /* �յ�һ֡��Ϣ ֱ���ж��Ƿ�Ϊ 'ok' */
        if(BT_BDE_Driver.Ops.Recv(Buf,&LenRecv) != 0)
        {
            prvGetInfFromFrame(Buf,LenRecv,ParamSplitTemp);
            
            if( (strcmp(ParamSplitTemp[0],"SPP:") == 0 ) && (strcmp(ParamSplitTemp[1],"ok") == 0 ))
            {
                return BT_True
            }
        }
        /* �ش� */
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
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint16_t prvSendData(uint8_t * Data,uint16_t Len)
{
    
}
/*
****************************************************
*  ������         : 
*  ��������       : 
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
*****************************************************
*/
static uint8_t BT_BDE_TimingProcess(uint16_t Period)
{
    BT_CHK_PARAM(Period);
    
    BT_BDE_Driver.PriData.Counter += Period;
    if(BT_BDE_Driver.PriData.Counter > 100000)BT_BDE_Driver.PriData.Counter = 0;
    /* �ޤ�10������ ��Ҫ���ʤ󣿣� */
    if(BT_BDE_Driver.PriData.Counter % (Period * 10) == 0)
    {
        /* �յ�һ֡���� */
        /******************************************************
            ��������£��˴�ȫΪ͸�����ݣ���һ�˴���ָ������
        ���ǿ�ѡ��Ҫ�������ų���
        ******************************************************/
        if(BT_BDE_Driver.Ops.Recv(Buf,&LenRecv) != 0)
        {
            /* ��������Ҫ�ж�����͸�����ݻ������� */
            if(Buf[0] == 'S' && Buf[0] == 'P' &&Buf[0] == 'P' &&Buf[0] == ':')
            {
                
            }
        }
    }
}
/*
****************************************************
*  ������         : 
*  ��������       : �����б�Ҫ֪�����ǵȴ��˶೤ʱ��
*  ����           : 
*  ����ֵ         : 
*  ����           : -5A4A5943-
*  ��ʷ�汾       : 
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
    /* ����������������BT_False */
    if(BT_CHK_BIT(BT_BDE_Driver.PriData.FlagGroup,BT_FLAG_RxLock))return BT_False;
    /* ���� �� �̰߳�ȫ */
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
    /* ������� */
    BT_CLR_BIT(BT_BDE_Driver.PriData.FlagGroup,BT_FLAG_RxLock);
}










