/******************************************************************************
* Copyright © 2004 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
*                                                                             *
******************************************************************************/
#ifndef __NTP_DATA_GRAM_H__
#define __NTP_DATA_GRAM_H__

/* NTP Data Gram Definition
 *                        1                   2                   3
 *      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                          Root Delay                           |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                       Root Dispersion                         |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                     Reference Identifier                      |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                   Reference Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                   Originate Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                    Receive Timestamp (64)                     |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                    Transmit Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                 Key Identifier (optional) (32)                |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                                                               |
 *     |                 Message Digest (optional) (128)               |
 *     |                                                               |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */ 

#include "alt_types.h"

#define NTP_BUFFER_SIZE 48

#define NTP_DATA_GRAM_CONTROL_HEADER(base_addr) ((alt_u32*) (base_addr  + 0x00 ))
#define NTP_CONT_HEAD_PRECISION_MSK (0xFF)
#define NTP_CONT_HEAD_PRECISION_OFST (0)
#define NTP_CONT_HEAD_POLL_MSK  (0xFF)
#define NTP_CONT_HEAD_POLL_OFST (8)
#define NTP_CONT_HEAD_STRATUM_MSK (0xFF)
#define NTP_CONT_HEAD_STRATUM_OFST (16)
#define NTP_CONT_HEAD_MODE_MSK  (0x7)
#define NTP_CONT_HEAD_MODE_OFST (24)
#define NTP_CONT_HEAD_VN_MSK (0x7)
#define NTP_CONT_HEAD_VN_OFST (27)
#define NTP_CONT_HEAD_LI_MSK (0x3)
#define NTP_CONT_HEAD_LI_OFST (30)

#define NTP_DATA_GRAM_ROOT_DELAY(base_addr)       ((alt_u32*) (base_addr  + 0x04 ))
#define NTP_DATA_GRAM_ROOT_DISP(base_addr)        ((alt_u32*) (base_addr  + 0x08 ))
#define NTP_DATA_GRAM_REF_ID(base_addr)           ((alt_u32*) (base_addr  + 0x0C ))
#define NTP_DATA_GRAM_REF_TSTMP_INT(base_addr)    ((alt_u32*) (base_addr  + 0x10 ))
#define NTP_DATA_GRAM_REF_TSTMP_FRAC(base_addr)   ((alt_u32*) (base_addr  + 0x14 ))
#define NTP_DATA_GRAM_ORIG_TSTMP_INT(base_addr)   ((alt_u32*) (base_addr  + 0x18 ))
#define NTP_DATA_GRAM_ORIG_TSTMP_FRAC(base_addr)  ((alt_u32*) (base_addr  + 0x1C ))
#define NTP_DATA_GRAM_RECV_TSTMP_INT(base_addr)   ((alt_u32*) (base_addr  + 0x20 ))
#define NTP_DATA_GRAM_RECV_TSTMP_FRAC(base_addr)  ((alt_u32*) (base_addr  + 0x24 ))
#define NTP_DATA_GRAM_TRANS_TSTMP_INT(base_addr)  ((alt_u32*) (base_addr  + 0x28 ))
#define NTP_DATA_GRAM_TRANS_TSTMP_FRAC(base_addr) ((alt_u32*) (base_addr  + 0x2C ))
#define NTP_DATA_GRAM_KEY_ID(base_addr)           ((alt_u32*) (base_addr  + 0x30 ))
#define NTP_DATA_GRAM_MSG_DIG1(base_addr)         ((alt_u32*) (base_addr  + 0x34 ))
#define NTP_DATA_GRAM_MSG_DIG2(base_addr)         ((alt_u32*) (base_addr  + 0x38 ))
#define NTP_DATA_GRAM_MSG_DIG3(base_addr)         ((alt_u32*) (base_addr  + 0x3C ))
#define NTP_DATA_GRAM_MSG_DIG4(base_addr)         ((alt_u32*) (base_addr  + 0x40 ))

//check on the types
typedef struct
{
  alt_u8  leap_indicator;
  alt_u8  version_number;
  alt_u8  mode;
  alt_u8  stratum;
  alt_8   poll_interval;
  alt_8   precision;
  alt_32  root_delay;
  alt_u32 root_dispersion;
  alt_u32 ref_id;
  alt_u32 ref_timestamp1;
  alt_u32 ref_timestamp2;
  alt_u32 orig_timestamp1;
  alt_u32 orig_timestamp2;
  alt_u32 recv_timestamp1;
  alt_u32 recv_timestamp2;
  alt_u32 trans_timestamp1;
  alt_u32 trans_timestamp2; 
  alt_u32 key_id;
  alt_u32 msg_digest1;
  alt_u32 msg_digest2;
  alt_u32 msg_digest3;
  alt_u32 msg_digest4;
}ntp_struct;



alt_u8* init_ntp_data_gram(void);
int decode_ntp_data(alt_u8 * ntp_buffer, ntp_struct *ntp_data);
int encode_ntp_data(alt_u8 * ntp_buffer, ntp_struct *ntp_data);

#endif /* __NTP_DATA_GRAM_H__ */


/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
