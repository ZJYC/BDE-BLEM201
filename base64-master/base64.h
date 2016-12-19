#ifndef __BASE64_H__
#define __BASE64_H__

typedef enum Base64ResultTypedef_
{
    BASE64_OK = 0, 
    BASE64_INVALID
}Base64ResultTypedef;

typedef struct Base64Typedef_
{
    Base64ResultTypedef (*Encode)(const unsigned char *, unsigned int, char *);
    Base64ResultTypedef (*Decode)(const unsigned char *, unsigned int, char *);
    
}Base64Typedef;
/* 输入待加密长度，输出加密之后的长度 */
#define BASE64_ENCODE_OUT_SIZE(s)	(((s) + 2) / 3 * 4)

#define BASE64_DECODE_OUT_SIZE(s)	(((s)) / 4 * 3)

extern Base64Typedef Base64;

#endif /* __BASE64_H__ */

