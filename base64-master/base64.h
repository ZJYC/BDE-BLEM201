#ifndef __BASE64_H__
#define __BASE64_H__

typedef enum Base64ResultTypedef_
{
    BASE64_OK = 0, 
    BASE64_INVALID
}Base64ResultTypedef;

typedef struct Base64Typedef_
{
    Base64ResultTypedef (*Encode)(const unsigned char *, unsigned int, char *,uint16_t *);
    Base64ResultTypedef (*Decode)(const unsigned char *, unsigned int, char *,uint16_t *);
    
}Base64Typedef;

extern Base64Typedef Base64;

#endif /* __BASE64_H__ */

