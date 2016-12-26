
#ifndef _BLUETOOTHPROCOL_H__
#define _BLUETOOTHPROCOL_H__

#define STATE_Success   (0x90)
#define STATE_Failed    (0x41)
#define STATE_Unsupport (0x6D)

#define COMD_Cost       (0x3000)
#define COMD_Return     (0x3001)
#define COMD_ManualCost (0x3002)
#define COMD_Menu       (0x3003)
#define COMD_Addup      (0x3004)
#define COMD_AddupClear (0x3005)
#define COMD_AddupCost  (0x3006)
#define COMD_NumberKey  (0x3007)
#define COMD_GetVersion (0x7F01)
#define COMD_FileOps    (0x3008)

#define TAG_ConfirmWeight       (0xA1)
#define TAG_ConfirmUintPrice    (0xA2)
#define TAG_ConfirmTotalPrice   (0xA3)
#define TAG_ConfirmPLU          (0xA4)
#define TAG_ConfirmTrace        (0xA5)

#define TAG_AddupWeight         (0xA1)
#define TAG_AddupUintPrice      (0xA2)
#define TAG_AddupTotalPrice     (0xA3)
#define TAG_AddupPLU            (0xA4)
#define TAG_AddupTrace          (0xA5)
#define TAG_AddupCount          (0xB0)

#define TAG_AddupCostTotalPrice (0xA6)
#define TAG_AddupCostCount      (0xB1)

typedef enum ComdTypedef_
{
    Result_False = 0,
    Result_True = 1,
    Result_HeaderError = 2,
    Result_CrcError = 3,
    Result_InvaildParam = 4,
    
    
    COMD_Cost = 0x3000,
    COMD_Return = 0x3001,
    COMD_ManualCost = 0x3002,
    COMD_Menu = 0x3003,
    COMD_Addup = 0x3004,
    COMD_AddupClear = 0x3005,
    COMD_AddupCost = 0x3006,
    COMD_NumberKey = 0x3007,
    COMD_FileOps = 0x3008,
    
    COMD_GetVersion = 0x7F01
    
}ComdTypedef;

typedef struct HeaderTypedef_
{
    
    uint8_t Name[5];
    uint8_t Addr[1];
    
}HeaderTypedef,*p_HeaderTypedef;

typedef struct ReqTypedef_
{
    
    HeaderTypedef HEAD;
    uint32_t LEN;
    uint16_t CMDR;
    uint8_t *DATA;
    uint16_t CRC;
    
}ReqTypedef,*p_ReqTypedef;

typedef struct AckTypedef_
{
    HeaderTypedef HEAD;
    uint32_t LEN;
    uint16_t CMDA;
    uint8_t STATE;
    uint8_t *DATA;
    uint16_t CRC;    
    
}AckTypedef,*p_AckTypedef;

typedef struct DataItemTypedef_
{
    
    uint8_t Tag;
    uint8_t Len;
    uint8_t *Buf;
    
}DataItemTypedef,*p_DataItemTypedef;

typedef struct DataCostTypedef_
{
    
    DataItemTypedef Weight;
    DataItemTypedef UnitPrice;
    DataItemTypedef Total;
    DataItemTypedef PLU;
    DataItemTypedef Trace;
    
}DataCostTypedef,*p_DataCostTypedef;

typedef struct DataAddupTypedef_
{
    
    DataItemTypedef Weight;
    DataItemTypedef UnitPrice;
    DataItemTypedef Total;
    DataItemTypedef PLU;
    DataItemTypedef Trace;
    DataItemTypedef Count;
    
}DataAddupTypedef,*p_DataAddupTypedef;

typedef struct DataAddupCostTypedef_
{
    
    DataItemTypedef TotalPrice;
    DataItemTypedef Count;
    
}DataAddupCostTypedef,*p_DataAddupCostTypedef;

typedef struct DataAttriTypedef_
{
    
    uint8_t NumKey;
    
    
}DataAttriTypedef,*p_DataAttriTypedef;

typedef struct DataFileOpsTypedef_
{
    
    uint8_t Str_CMD[1 ];
    uint8_t Str_FLENAME[20];
    uint8_t Str_FILETYPE[1];
    uint8_t Str_FILELEN[10];
    uint8_t Str_SUMPACK[4];
    uint8_t Str_NOWPACK[4];
    uint8_t Str_PACKLEN[8];
    uint8_t *Str_PACKBUF;
    
}DataFileOpsTypedef,*p_DataFileOpsTypedef;

typedef struct BT_HandleTypedef_
{
    p_DataCostTypedef DataCost;
    p_DataAddupTypedef DataAddup;
    p_DataAddupCostTypedef DataAddupCost;
    p_DataAttriTypedef DataAttri;
    p_DataFileOpsTypedef DataFileOps;
    
}BT_HandleTypedef,*p_BT_HandleTypedef;

#endif






