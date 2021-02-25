/* Includes ------------------------------------------------------------------*/
#include "ymodem.h"
#include "Tools.h"

#if (defined YMODEM_DEBUG_ENABLE) && (YMODEM_DEBUG_ENABLE == 1)
#include "serial_console_dbg.h"
#define YMODEM_DBG_PRINTF(f_, ...)           DBG_PRINTF(f_, ##__VA_ARGS__)
#define YMODEM_DBG_TAG_PRINTF(f_, ...)       do {\
                                               DBG_PRINTF("\r\n[YMODEM] ");\
                                               DBG_PRINTF((f_), ##__VA_ARGS__);\
                                             } while(0)
#else
#define YMODEM_DBG_PRINTF(f_, ...)
#define YMODEM_DBG_TAG_PRINTF(f_, ...)
#endif

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define CRC16_F       /* activate the CRC16 integrity */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* @note ATTENTION - please keep this variable 32bit alligned */
uint8_t aPacketData[PACKET_1K_SIZE + PACKET_DATA_INDEX + PACKET_TRAILER_SIZE];
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
static uint8_t HAL_UART_Receive(ymodem_cxt_t* ymodem, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static uint8_t HAL_UART_Transmit(ymodem_cxt_t* ymodem, uint8_t *pData, uint16_t Size, uint32_t Timeout);
static void PrepareIntialPacket(uint8_t *p_data, const uint8_t *p_file_name, uint32_t length);
static void PreparePacket(ymodem_cxt_t* ymodem, uint32_t index, uint8_t *p_packet, uint8_t pkt_nr, uint32_t size_blk);
static HAL_USER_StatusTypeDef ReceivePacket(ymodem_cxt_t* ymodem, uint8_t *p_data, uint32_t *p_length, uint32_t timeout);
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte);
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size);
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size);

/* Private functions ---------------------------------------------------------*/

static uint8_t HAL_UART_Receive(ymodem_cxt_t* ymodem, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t result;

  if (ymodem->input_cb)
  {
    result = ymodem->input_cb(pData, Size, Timeout);
  }
  else
  {
    result = HAL_USER_ERROR;
  }

  return result;
}

static uint8_t HAL_UART_Transmit(ymodem_cxt_t* ymodem, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t result;

  if (ymodem->output_cb)
  {
    result = ymodem->output_cb(pData, Size, Timeout);
  }
  else
  {
    result = HAL_USER_ERROR;
  }

  return result;
}

/**
  * @brief  Transmit a byte to the HyperTerminal
  * @param  param The byte to be sent
  * @retval HAL_USER_StatusTypeDef HAL_USER_OK if OK
  */
uint8_t Serial_PutByte(ymodem_cxt_t* ymodem, uint8_t param)
{
  return HAL_UART_Transmit(ymodem, &param, 1, TX_TIMEOUT);
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  *     0: end of transmission
  *     2: abort by sender
  *    >0: packet length
  * @param  timeout
  * @retval HAL_USER_OK: normally return
  *         HAL_USER_BUSY: abort by user
  */
static HAL_USER_StatusTypeDef ReceivePacket(ymodem_cxt_t* ymodem, uint8_t *p_data, uint32_t *p_length, uint32_t timeout)
{
  uint32_t crc, crc_true;
  uint32_t packet_size = 0;
  HAL_USER_StatusTypeDef status;
  uint8_t char1;

  *p_length = 0;
  status = HAL_UART_Receive(ymodem, &char1, 1, timeout);

  if (status == HAL_USER_OK)
  {
    switch (char1)
    {
      case SOH:
        packet_size = PACKET_SIZE;
        YMODEM_DBG_PRINTF("\r\n[SOH]");
        break;
      case STX:
        packet_size = PACKET_1K_SIZE;
        YMODEM_DBG_PRINTF("\r\n[STX]");
        break;
      case EOT:
        YMODEM_DBG_PRINTF("\r\n[EOT]");
        break;
      case CA:
        YMODEM_DBG_PRINTF("\r\n[CA]");
        if ((HAL_UART_Receive(ymodem, &char1, 1, timeout) == HAL_USER_OK) && (char1 == CA))
        {
          packet_size = 2;
        }
        else
        {
          status = HAL_USER_ERROR;
        }
        break;
      case ABORT1:
      case ABORT2:
        YMODEM_DBG_PRINTF("\r\n[ABORT]");
        status = HAL_USER_BUSY;
        break;
      default:
        YMODEM_DBG_PRINTF("\r\n[default] %02X", char1);
        status = HAL_USER_ERROR;
        break;
    }
    *p_data = char1;

    if (packet_size >= PACKET_SIZE )
    {
      YMODEM_DBG_PRINTF("\r\n[packet_size] = %lu", packet_size);
      status = HAL_UART_Receive(ymodem, &p_data[PACKET_NUMBER_INDEX], packet_size + PACKET_OVERHEAD_SIZE, timeout);

      /* Simple packet sanity check */
      if (status == HAL_USER_OK )
      {
        if (p_data[PACKET_NUMBER_INDEX] != ((p_data[PACKET_CNUMBER_INDEX]) ^ NEGATIVE_BYTE))
        {
          packet_size = 0;
          status = HAL_USER_ERROR;
          YMODEM_DBG_PRINTF("\r\n[PACKET_NUMBER_INDEX] = %u FAIL", p_data[PACKET_NUMBER_INDEX]);
        }
        else
        {
          /* Check packet CRC */
          crc = p_data[ packet_size + PACKET_DATA_INDEX ] << 8;
          crc += p_data[ packet_size + PACKET_DATA_INDEX + 1 ];
          crc_true = Cal_CRC16(&p_data[PACKET_DATA_INDEX], packet_size);
          if (crc_true != crc )
          {
            packet_size = 0;
            status = HAL_USER_ERROR;
            YMODEM_DBG_PRINTF("\r\n[CRC] = 0x%04lX FAIL, expect = 0x%04lX", crc, crc_true);
          }
          else
          {
            YMODEM_DBG_PRINTF("\r\n[CRC] rx 0x%04lX OK", crc);
          }
        }
      }
      else
      {
        packet_size = 0;
      }
    }
  }
  YMODEM_DBG_PRINTF("\r\n[status] = %u, %lu", status, packet_size);
  *p_length = packet_size;
  return status;
}

/**
  * @brief  Prepare the first block
  * @param  p_data:  output buffer
  * @param  p_file_name: name of the file to be sent
  * @param  length: length of the file to be sent in bytes
  * @retval None
  */
static void PrepareIntialPacket(uint8_t *p_data, const uint8_t *p_file_name, uint32_t length)
{
  uint32_t i, j = 0;
  uint8_t astring[10] = {0};

  /* first 3 bytes are constant */
  p_data[PACKET_START_INDEX] = SOH;
  p_data[PACKET_NUMBER_INDEX] = 0x00;
  p_data[PACKET_CNUMBER_INDEX] = 0xff;

  /* Filename written */
  for (i = 0; (p_file_name[i] != '\0') && (i < FILE_NAME_LENGTH); i++)
  {
    p_data[i + PACKET_DATA_INDEX] = p_file_name[i];
  }

  p_data[i + PACKET_DATA_INDEX] = 0x00;

  /* file size written */
  Int2Str (astring, length);
  i = i + PACKET_DATA_INDEX + 1;
  while (astring[j] != '\0')
  {
    p_data[i++] = astring[j++];
  }

  /* padding with zeros */
  for (j = i; j < PACKET_SIZE + PACKET_DATA_INDEX; j++)
  {
    p_data[j] = 0;
  }
}

/**
  * @brief  Prepare the data packet
  * @param  p_source: pointer to the data to be sent
  * @param  p_packet: pointer to the output buffer
  * @param  pkt_nr: number of the packet
  * @param  size_blk: length of the block to be sent in bytes
  * @retval None
  */
static void PreparePacket(ymodem_cxt_t* ymodem, uint32_t index, uint8_t *p_packet, uint8_t pkt_nr, uint32_t size_blk)
{
  uint32_t i, size, packet_size;

  /* Make first three packet */
  packet_size = size_blk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
  size = size_blk < packet_size ? size_blk : packet_size;
  if (packet_size == PACKET_1K_SIZE)
  {
    p_packet[PACKET_START_INDEX] = STX;
  }
  else
  {
    p_packet[PACKET_START_INDEX] = SOH;
  }
  p_packet[PACKET_NUMBER_INDEX] = pkt_nr;
  p_packet[PACKET_CNUMBER_INDEX] = (~pkt_nr);

  ymodem->flash_read_cb(index, (uint32_t*)&p_packet[PACKET_DATA_INDEX], size);
  if ( size  <= packet_size)
  {
    for (i = size + PACKET_DATA_INDEX; i < packet_size + PACKET_DATA_INDEX; i++)
    {
      p_packet[i] = 0x1A; /* EOF (0x1A) or 0x00 */
    }
  }
}

/**
  * @brief  Update CRC16 for input byte
  * @param  crc_in input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = UpdateCRC16(crc, *p_data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Calculate Check sum for YModem Packet
  * @param  p_data Pointer to input data
  * @param  size length of input data
  * @retval uint8_t checksum value
  */
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t *p_data_end = p_data + size;

  while (p_data < p_data_end )
  {
    sum += *p_data++;
  }

  return (sum & 0xffu);
}

/* Public functions ---------------------------------------------------------*/
/**
  * @brief  Receive a file using the ymodem protocol with CRC16.
  * @param  p_size The size of the file.
  * @retval COM_StatusTypeDef result of reception/programming
  */
COM_StatusTypeDef Ymodem_Receive (ymodem_cxt_t* ymodem, uint32_t *p_size )
{
  uint32_t i, packet_length, session_done = 0, file_done, errors = 0, session_begin = 0, packets_received = 0;
  uint32_t flashdestination, ramsource, filesize, rx_size = 0;
  uint8_t *file_ptr;
  uint8_t file_size[FILE_SIZE_LENGTH], tmp;
  COM_StatusTypeDef result = COM_OK;

  /* Initialize flashdestination variable */
  flashdestination = ymodem->flash.start_addr;

  while ((session_done == 0) && (result == COM_OK))
  {
    packets_received = 0;
    file_done = 0;
    while ((file_done == 0) && (result == COM_OK))
    {
      switch (ReceivePacket(ymodem, aPacketData, &packet_length, DOWNLOAD_TIMEOUT))
      {
        case HAL_USER_OK:
          errors = 0;
          switch (packet_length)
          {
            case 2:
              /* Abort by sender */
              Serial_PutByte(ymodem, ACK);
              YMODEM_DBG_PRINTF("\r\nResp [ACK]");
              result = COM_ABORT;
              YMODEM_DBG_PRINTF("\r\n[result = COM_ABORT]");
              break;
            case 0:
              /* End of transmission */
              Serial_PutByte(ymodem, ACK);
              YMODEM_DBG_PRINTF("\r\nResp [ACK]");
              file_done = 1;
              YMODEM_DBG_PRINTF("\r\n[file_done]");
              break;
            default:
              /* Normal packet */
              if (aPacketData[PACKET_NUMBER_INDEX] != (0xFFU & packets_received))
              {
                Serial_PutByte(ymodem, NAK);
                YMODEM_DBG_PRINTF("\r\nResp [NAK]");
              }
              else
              {
                if (packets_received == 0)
                {
                  YMODEM_DBG_PRINTF("\r\n[PACKET_DATA_INDEX] %u", aPacketData[PACKET_DATA_INDEX]);
                  /* File name packet */
                  if (aPacketData[PACKET_DATA_INDEX] != 0)
                  {
                    /* File name extraction */
                    i = 0;
                    file_ptr = aPacketData + PACKET_DATA_INDEX;
                    while ( (*file_ptr != 0) && (i < FILE_NAME_LENGTH))
                    {
                      aFileName[i++] = *file_ptr++;
                    }

                    /* File size extraction */
                    aFileName[i++] = '\0';
                    i = 0;
                    file_ptr ++;
                    while ( (*file_ptr != '\0') && (i < FILE_SIZE_LENGTH))
                    {
                      file_size[i++] = *file_ptr++;
                    }
                    file_size[i++] = '\0';
                    Str2Int(file_size, &filesize);

                    YMODEM_DBG_PRINTF("\r\nReceive filename: %s, size: %lu", aFileName, filesize);

                    /* Test the size of the image to be sent */
                    /* Image size is greater than Flash size */
                    if (*p_size > (ymodem->flash.max_size + 1))
                    {
                      /* End session */
                      tmp = CA;
                      HAL_UART_Transmit(ymodem, &tmp, 1, NAK_TIMEOUT);
                      HAL_UART_Transmit(ymodem, &tmp, 1, NAK_TIMEOUT);
                      result = COM_LIMIT;
                    }
                    if (ymodem->flash_erase_cb)
                    {
                      /* erase user application area */
                      ymodem->flash_erase_cb(ymodem->flash.start_addr, filesize);
                    }
                    *p_size = filesize;
                    
                    Serial_PutByte(ymodem, ACK);
                    YMODEM_DBG_PRINTF("\r\nResp [ACK]");
                    Serial_PutByte(ymodem, CRC16);
                    YMODEM_DBG_PRINTF("\r\nResp [CRC16]");
                  }
                  /* File header packet is empty, end session */
                  else
                  {
                    Serial_PutByte(ymodem, ACK);
                    YMODEM_DBG_PRINTF("\r\nResp [ACK]");
                    file_done = 1;
                    session_done = 1;
                    YMODEM_DBG_PRINTF("\r\n[session_done, file_done]");
                    break;
                  }
                }
                else /* Data packet */
                {
                  ramsource = (uint32_t) & aPacketData[PACKET_DATA_INDEX];

                  if (ymodem->flash_write_cb)
                  {
                    if(ymodem->flash_write_cb(flashdestination, (uint32_t*) ramsource, packet_length))
                    {
                      flashdestination += packet_length;
                      rx_size += packet_length;
                      Serial_PutByte(ymodem, ACK);
                      YMODEM_DBG_PRINTF("\r\nRx %lu/%lu, Resp [ACK]", rx_size, filesize);
                    }
                    else /* An error occurred while writing to Flash memory */
                    {
                      /* End session */
                      Serial_PutByte(ymodem, CA);
                      YMODEM_DBG_PRINTF("\r\nResp [CA]");
                      Serial_PutByte(ymodem, CA);
                      YMODEM_DBG_PRINTF("\r\nResp [CA]");
                      result = COM_DATA;
                      YMODEM_DBG_PRINTF("\r\n[result = COM_DATA]");
                    }
                  }
                }
                packets_received ++;
                session_begin = 1;
              }
              break;
          }
          break;
        case HAL_USER_BUSY: /* Abort actually */
          Serial_PutByte(ymodem, CA);
          YMODEM_DBG_PRINTF("\r\nResp [CA]");
          Serial_PutByte(ymodem, CA);
          YMODEM_DBG_PRINTF("\r\nResp [CA]");
          result = COM_ABORT;
          YMODEM_DBG_PRINTF("\r\n[HAL_USER_BUSY: result = COM_ABORT]");
          break;
        default:
          if (session_begin > 0)
          {
            errors ++;
          }
          if (errors > MAX_ERRORS)
          {
            /* Abort communication */
            Serial_PutByte(ymodem, CA);
            YMODEM_DBG_PRINTF("\r\nResp [CA]");
            Serial_PutByte(ymodem, CA);
            YMODEM_DBG_PRINTF("\r\nResp [CA]");
            YMODEM_DBG_PRINTF("\r\n[MAX_ERRORS]");
            result = COM_ABORT;
          }
          else
          {
            Serial_PutByte(ymodem, CRC16); /* Ask for a packet */
            YMODEM_DBG_PRINTF("\r\nResp [CRC16]");
            YMODEM_DBG_PRINTF("\r\n[Ask for a packet]");
          }
          break;
      }
    }
  }
  return result;
}

/**
  * @brief  Transmit a file using the ymodem protocol
  * @param  p_buf: Address of the first byte
  * @param  p_file_name: Name of the file sent
  * @param  file_size: Size of the transmission
  * @retval COM_StatusTypeDef result of the communication
  */
COM_StatusTypeDef Ymodem_Transmit (ymodem_cxt_t* ymodem, uint32_t index, const char *p_file_name, uint32_t file_size)
{
  uint32_t errors = 0, ack_recpt = 0, size = 0, pkt_size, tx_size = 0;
  uint32_t p_buf_int;
    COM_StatusTypeDef result = COM_OK;
    uint32_t blk_number = 1;
    uint8_t a_rx_ctrl[2];
    uint8_t i;
  #ifdef CRC16_F
    uint32_t temp_crc;
  #else /* CRC16_F */
    uint8_t temp_chksum;
  #endif /* CRC16_F */

    /* Prepare first block - header */
    PrepareIntialPacket(aPacketData, (const uint8_t*)p_file_name, file_size);

    YMODEM_DBG_PRINTF("\r\nFilename: %s, size = %lu", (const char*)p_file_name, file_size);

    while (( !ack_recpt ) && ( result == COM_OK ))
    {
      /* Send Packet */
      HAL_UART_Transmit(ymodem, &aPacketData[PACKET_START_INDEX], PACKET_SIZE + PACKET_HEADER_SIZE, NAK_TIMEOUT);

      /* Send CRC or Check Sum based on CRC16_F */
  #ifdef CRC16_F
      temp_crc = Cal_CRC16(&aPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
      Serial_PutByte(ymodem, temp_crc >> 8);
      Serial_PutByte(ymodem, temp_crc & 0xFF);
  #else /* CRC16_F */
      temp_chksum = CalcChecksum (&aPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
      Serial_PutByte(ymodem, temp_chksum);
  #endif /* CRC16_F */

      /* Wait for Ack and 'C' */
      if (HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK)
      {
        if (a_rx_ctrl[0] == ACK)
        {
          ack_recpt = 1;
          YMODEM_DBG_PRINTF("\r\nRx ACK");
        }
        else if (a_rx_ctrl[0] == CA)
        {
          YMODEM_DBG_PRINTF("\r\nRx CA");
          if ((HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK) && (a_rx_ctrl[0] == CA))
          {
            result = COM_ABORT;
          }
        }
      }
      else
      {
        errors++;
        YMODEM_DBG_PRINTF("\r\nError Cnt %lu, rx 0x%02X", errors, a_rx_ctrl[0]);
      }
      if (errors >= MAX_ERRORS)
      {
        result = COM_ERROR;
      }
    }

    if(result == COM_OK)
    {
      /* Wait for CRC16 */
      a_rx_ctrl[0] = 0;
      if ((HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK) && (a_rx_ctrl[0] == CRC16))
      {
        YMODEM_DBG_PRINTF("\r\nRx CRC16");
      }
      else
      {
        result = COM_ABORT;
        YMODEM_DBG_PRINTF("\r\nCOM_ABORT");
      }

    }

    p_buf_int = index;
    size = file_size;

    /* Here 1024 bytes length is used to send the packets */
    while ((size) && (result == COM_OK ))
    {
      /* Prepare next packet */
      PreparePacket(ymodem, p_buf_int, aPacketData, blk_number, size);
      ack_recpt = 0;
      a_rx_ctrl[0] = 0;
      errors = 0;

      /* Resend packet if NAK for few times else end of communication */
      while (( !ack_recpt ) && ( result == COM_OK ))
      {
        /* Send next packet */
        if (size >= PACKET_1K_SIZE)
        {
          pkt_size = PACKET_1K_SIZE;
        }
        else
        {
          pkt_size = PACKET_SIZE;
        }

        HAL_UART_Transmit(ymodem, &aPacketData[PACKET_START_INDEX], pkt_size + PACKET_HEADER_SIZE, NAK_TIMEOUT);

        /* Send CRC or Check Sum based on CRC16_F */
  #ifdef CRC16_F
        temp_crc = Cal_CRC16(&aPacketData[PACKET_DATA_INDEX], pkt_size);
        Serial_PutByte(ymodem, temp_crc >> 8);
        Serial_PutByte(ymodem, temp_crc & 0xFF);
  #else /* CRC16_F */
        temp_chksum = CalcChecksum (&aPacketData[PACKET_DATA_INDEX], pkt_size);
        Serial_PutByte(ymodem, temp_chksum);
  #endif /* CRC16_F */

        if (size < 128) pkt_size = size;
        YMODEM_DBG_PRINTF("\r\nSend pack [%lu] %lu/%lu, CRC = 0x%04lX", pkt_size, tx_size + pkt_size, file_size, temp_crc);

        /* Wait for Ack */
        a_rx_ctrl[0] = 0;
        if ((HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK) && (a_rx_ctrl[0] == ACK))
        {
          YMODEM_DBG_PRINTF("\r\nRx ACK");
          errors = 0;
          tx_size += pkt_size;
          ack_recpt = 1;
          if (size > pkt_size)
          {
            p_buf_int += pkt_size;
            size -= pkt_size;
            if (blk_number == (ymodem->flash.max_size / PACKET_1K_SIZE))
            {
              result = COM_LIMIT; /* boundary error */
              YMODEM_DBG_PRINTF("\r\nCOM_LIMIT");
            }
            else
            {
              blk_number++;
            }
          }
          else
          {
            p_buf_int += pkt_size;
            size = 0;
          }
        }
        else
        {
          errors++;
          YMODEM_DBG_PRINTF("\r\nError Cnt %lu, rx 0x%02X", errors, a_rx_ctrl[0]);
        }

        /* Resend packet if NAK  for a count of 10 else end of communication */
        if (errors >= MAX_ERRORS)
        {
          result = COM_ERROR;
        }
      }
    }

    /* Sending End Of Transmission char */
    YMODEM_DBG_PRINTF("\r\nSending End Of Transmission char");
    ack_recpt = 0;
    a_rx_ctrl[0] = 0x00;
    errors = 0;
    while (( !ack_recpt ) && ( result == COM_OK ))
    {
      Serial_PutByte(ymodem, EOT);
      YMODEM_DBG_PRINTF("\r\nSend EOT");

      /* Wait for Ack */
      if (HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK)
      {
        if (a_rx_ctrl[0] == ACK)
        {
          ack_recpt = 1;
          YMODEM_DBG_PRINTF("\r\nRx ACK");
        }
        else if (a_rx_ctrl[0] == CA)
        {
          if ((HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK) && (a_rx_ctrl[0] == CA))
          {
            result = COM_ABORT;
            YMODEM_DBG_PRINTF("\r\nRx COM_ABORT");
          }
        }
      }
      else
      {
        errors++;
      }

      if (errors >=  MAX_ERRORS)
      {
        result = COM_ERROR;
      }
    }

    /* Empty packet sent - some terminal emulators need this to close session */
    YMODEM_DBG_PRINTF("\r\nSending Empty packet");
    ack_recpt = 0;
    a_rx_ctrl[0] = 0x00;
    errors = 0;
    while (( !ack_recpt ) && ( result == COM_OK ))
    {
      /* Preparing an empty packet */
      aPacketData[PACKET_START_INDEX] = SOH;
      aPacketData[PACKET_NUMBER_INDEX] = 0;
      aPacketData[PACKET_CNUMBER_INDEX] = 0xFF;
      for (i = PACKET_DATA_INDEX; i < (PACKET_SIZE + PACKET_DATA_INDEX); i++)
      {
        aPacketData [i] = 0x00;
      }

      /* Send Packet */
      HAL_UART_Transmit(ymodem, &aPacketData[PACKET_START_INDEX], PACKET_SIZE + PACKET_HEADER_SIZE, NAK_TIMEOUT);

      /* Send CRC or Check Sum based on CRC16_F */
  #ifdef CRC16_F
      temp_crc = Cal_CRC16(&aPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
      Serial_PutByte(ymodem, temp_crc >> 8);
      Serial_PutByte(ymodem, temp_crc & 0xFF);
  #else /* CRC16_F */
      temp_chksum = CalcChecksum (&aPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
      Serial_PutByte(ymodem, temp_chksum);
  #endif /* CRC16_F */
      YMODEM_DBG_PRINTF("\r\nSend Empty pack [%lu], CRC = 0x%04lX", PACKET_SIZE, temp_crc);
      /* Wait for Ack and 'C' */
      if (HAL_UART_Receive(ymodem, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == HAL_USER_OK)
      {
        if (a_rx_ctrl[0] == ACK)
        {
          ack_recpt = 1;
          YMODEM_DBG_PRINTF("\r\nRx ACK");
        }
        if (a_rx_ctrl[0] == CA)
        {
          result = COM_ABORT;
          YMODEM_DBG_PRINTF("\r\nRx COM_ABORT");
        }
      }
      else
      {
        errors++;
      }

      if (errors >=  MAX_ERRORS)
      {
        result = COM_ERROR;
      }
    }

    return result; /* file transmitted successfully */
}
