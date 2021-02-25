#ifndef _TOOLS_H
#define _TOOLS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdint.h>

#define LOWER_CASE 0
#define UPPER_CASE 1

#define IS_CAP_LETTER(c)    (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            (((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))

int vspfunc(size_t (*write_cb)(uint8_t*, size_t), const char *format, ...);
void hex2str(char *str, uint8_t *hex, uint8_t len, uint8_t type);
char str2hex(const char *str, uint8_t *hex, uint8_t len);
uint8_t StrncpyUntilChar(char *Dest, char *Scr, char Chr, uint16_t Leng);
int find_char_rev(char *buff, char dat, int start_id);
char check_strncmp(char *str, char dat, unsigned char *index, unsigned char len);
void convert_num2str_fpu(int32_t num, char *buff, uint8_t cmd);
uint8_t crc_xor(const uint8_t *buff, uint32_t lenght);
void Int2Str(uint8_t *p_str, uint32_t intnum);
uint32_t Str2Int(uint8_t *inputstr, uint32_t *intnum);

#define CV_NUM2VOLUME 0
#define CV_NUM2CASH 1

#ifdef __cplusplus
}
#endif
#endif
