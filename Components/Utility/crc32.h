/** @file crc32.h
 *  @brief To porting some of type, value to mismatch with platform
 *
 *  @author Luong Thanh (ThanhLV13)
 *  @note No Note at the moment
 *  @bug No known bugs.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver    Who        Date             Changes
 * -----  --------   ----------       -----------------------------------------------
 * 1.0    ThanhLV13  Oct 30, 2020    First release
 *
 *
 *</pre>
 */
/* MODULE BSP */
#ifndef __CRC32_H_
#define __CRC32_H_
/******************************************************************************/
//  INCLUDE HEADER
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
//  MACRO. DEFINE
/******************************************************************************/
/** Enable the refercen table / Or RAW value */
#define CRC32_ENABLE_REFLECT_BIT_ORDER      (1)
/* CRC32_POLY */
#define CRC32_POLY                          (0x04C11DB7)

/******************************************************************************/
//  TYPEDEF
/******************************************************************************/

/******************************************************************************/
//  FUNCTIONS
/******************************************************************************/
/**
 * Start to calculator CRC
 */
void CRC32_Start(uint32_t seek);

/**
 * Calculator CRC
 */
uint32_t CRC32_Accumulate(const uint8_t * buffer, uint32_t length);

/**
 * Get current CRC
 */
uint32_t CRC32_Get(void);

/**
 * Calculator CRC by Buffer
 */
uint32_t Crc32_CalculateBuffer(const uint8_t * buffer, uint32_t length);

#endif /* __CRC32_H_ */
