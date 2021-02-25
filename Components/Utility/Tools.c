#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "Tools.h"

#define V_BUFFER    256
int vspfunc(size_t (*write_cb)(uint8_t*, size_t), const char *format, ...)
{
  va_list args;
  char v_buff[V_BUFFER];
  size_t lenght;

  va_start(args, format);
  lenght = vsnprintf(v_buff, V_BUFFER, format, args);
  va_end(args);

  if (lenght > 0)
  {
    write_cb((uint8_t*)v_buff, lenght);
  }

  return lenght;
}

/*
- In chuoi Buff
- str: chuoi sau khi chuyen
- hex: la chuoi Hex
- len: chieu dai mang hex can chuyen sang string, nen mang string co chieu dai la 2*len + 1
- type: 1. chuyen dang chu Hoa, 0. chuyen dang chu thuong
#define LOWER_CASE	0
#define UPPER_CASE	1
- Vi du: hex[]={0x12,0xAB,0xD3}, len=3, hoathuong=1 -> String="12ABD3"
hex2str(String,hex,3,UPPER_CASE);
*/
void hex2str(char *str, uint8_t *hex, uint8_t len, uint8_t type)
{
  uint8_t i, j = 0;
  for (i = 0; i < len; i++)
  {
    if (type == UPPER_CASE)
      sprintf(str + j, "%02X", hex[i]);
    else
      sprintf(str + j, "%02x", hex[i]);
    j += 2;
  }
  str[len * 2] = '\0';
}

/*
- Chuyen string thanh Hex
- Len: Chieu dai chuoi String muon chuyen thanh Hex
- Vi du: String="12ABD3", len=6 -> hex[]={0x12,0xAB,0xD3}
Ghi chu: String phai la UPPER_CASE
*/
char str2hex(const char *str, uint8_t *hex, uint8_t len)
{
  uint8_t i, j;
  uint8_t high, low;
  j = 0;
  i = 0;
  while (1)
  {
    if (str[j] >= '0' && str[j] <= '9')
    {
      high = str[j] - '0';
    }
    else if (str[j] >= 'A' && str[j] <= 'F')
    {
      high = str[j] - 'A' + 10;
    }
    else
      return 0;
    hex[i] = (high << 4);
    if (++j >= len)
      break;

    if (str[j] >= '0' && str[j] <= '9')
    {
      low = str[j] - '0';
    }
    else if (str[j] >= 'A' && str[j] <= 'F')
    {
      low = str[j] - 'A' + 10;
    }
    else
      return 0;
    hex[i] |= low;
    if (++j >= len)
      break;
    i++;
  }
  return 1;
}

/*Copy 1 chuoi ky tu den ky tu cuoi cung 'Chr'*/
uint8_t StrncpyUntilChar(char *Dest, char *Scr, char Chr, uint16_t Leng)
{
  uint16_t i;
  for (i = 0; i <= Leng && Scr[i] != 0; i++)
  {
    if (Scr[i] == Chr)
    {
      strncpy(Dest, Scr, i);
      Dest[i] = 0;
      return 1;
    }
  }
  return 0;
}

int find_char_rev(char *buff, char dat, int start_id)
{
  int i;
  for(i = start_id - 1; i >= 0; --i)
  {
    if (buff[i] == dat)
    {
      break;
    }
  }
  return i;
}

char check_strncmp(char *str, char dat, uint8_t *index, uint8_t len)
{
  if (str[*index] == dat)
  {
    (*index)++;
    if ((*index) == len)
    {
      *index = 0;
      return 1;
    }
  }
  else
  {
    *index = 0;
    if (str[*index] == dat)
      (*index)++;
  }
  return 0;
}

void convert_num2str_fpu(int32_t num, char *buff, uint8_t cmd)
{
  int32_t num_abs;
  num_abs = abs(num);

  if (cmd == CV_NUM2VOLUME)
  {
    sprintf(buff, "%ld,%03lu", num / 1000, num_abs % 1000);
  }
  else
  {
    if (num_abs < 1000)
    {
      sprintf(buff, "%ld", num);
    }
    else if (num_abs < 1000000)
    {
      sprintf(buff, "%ld.%03lu", num / 1000, num_abs % 1000);
    }
    else if (num_abs < 1000000000)
    {
      sprintf(buff, "%ld.%03lu.%03lu", num / 1000000, (num_abs % 1000000) / 1000, num_abs % 1000);
    }
    else
    {
      sprintf(buff, "%ld.%03lu.%03lu.%03lu", num / 1000000000, (num_abs % 1000000000) / 1000000, (num_abs % 1000000) / 1000, num_abs % 1000);
    }
  }
}

uint8_t crc_xor(const uint8_t *buff, uint32_t lenght)
{
  uint8_t xor = 0;
  for (uint32_t i = 0; i < lenght; ++i)
  {
    xor ^= buff[i];
  }
  return xor;
}


/**
  * @brief  Convert an Integer to a string
  * @param  p_str: The string output pointer
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t *p_str, uint32_t intnum)
{
  uint32_t i, divider = 1000000000, pos = 0, status = 0;

  for (i = 0; i < 10; i++)
  {
    p_str[pos++] = (intnum / divider) + 48;

    intnum = intnum % divider;
    divider /= 10;
    if ((p_str[pos-1] == '0') & (status == 0))
    {
      pos = 0;
    }
    else
    {
      status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  p_inputstr: The string to be converted
  * @param  p_intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2;
    while ( ( i < 11 ) && ( p_inputstr[i] != '\0' ) )
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }

    /* valid result */
    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ( ( i < 11 ) && ( res != 1 ) )
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        /* return 1 */
        res = 1;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0))
      {
        val = val << 10;
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }
  }

  return res;
}

