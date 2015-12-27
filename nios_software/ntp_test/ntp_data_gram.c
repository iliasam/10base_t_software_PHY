/******************************************************************************
* Copyright © 2004 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
* Author - HRB                                                                *
*                                                                             *
* The purpose of this file is to abstract the details of NTP data grams from  *
* the user. It is important to note that the details here are specific to NTP *
* and are independent of what this example design is trying to present which  *
* is a simple sockets based client.                                           *
*                                                                             *
* This code allows the user to deal with a ntp_data_gram_struct as definded   *
* in ntp_data_gram.h rather than NTP data directly.  The following functions  *
* are provided as an interface:                                               *
*                                                                             *
* alt_u8* init_ntp_data_gram(void);                                           *
* int decode_ntp_data(alt_u8 * ntp_buffer, ntp_data_gram_struct *ntp_data);   *
* int encode_ntp_data(alt_u8 * ntp_buffer, ntp_data_gram_struct *ntp_data);   *
*                                                                             *
* See comments below for additional details on the above functions            * 
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "ntp_client.h"
#include "alt_types.h"
#include "ntp_data_gram.h"

#include "includes.h"

#include "ipport.h"
#include "libport.h"
#include "osport.h"

static int convert_ntp_data_to_net(alt_u8 * ntp_buffer);
static int convert_ntp_data_to_host(alt_u8 * ntp_buffer);

//#ifdef LCD_DISPLAY_NAME     
//extern  FILE* lcdDevice;
//#endif /* LCD_DISPLAY_NAME */     

/* 
 * The purpose of this function is to initialize a area of memory big enough to
 * hold a NTP data packet and clear the contents.  Care has to be taken when 
 * calling this function as malloc is not tread safe.
 */
alt_u8* init_ntp_data_gram(void)
{
  alt_u8 *ntp_buffer;

  ntp_buffer = (alt_u8 *)malloc(NTP_BUFFER_SIZE);

  
  if(ntp_buffer == NULL)
  {
    printf("Fatal Error: Program Terminating.  See STDERR\n");
//    fprintf(lcdDevice, "Memory allocation for NTP buffer failed.\n");
    exit(0);
  }
  else
  {
    memset(ntp_buffer, 0x0, NTP_BUFFER_SIZE);
  }
 return ntp_buffer;
}

/*
 * The purpose of this function is to decode NTP data and place it in a
 * ntp_data_gram_struc.  NTP comes a network byte order in a format described
 * in RFC 2030(SNTP) and RFC 1305(NTP).  Once the ntp_data_gram_struc is loaded
 * the user can access the data in the host byte order.
 */
int decode_ntp_data(alt_u8 * ntp_buffer, ntp_struct *ntp_data)
{
//Convert the NTP data received to host byte order
 convert_ntp_data_to_host(ntp_buffer);

//NTP Control Header Decode
 ntp_data->leap_indicator = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >>
                               NTP_CONT_HEAD_LI_OFST) & 
                               NTP_CONT_HEAD_LI_MSK);
                              
 ntp_data->version_number = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >>
                               NTP_CONT_HEAD_VN_OFST) & 
                               NTP_CONT_HEAD_VN_MSK);

 ntp_data->mode           = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >>
                               NTP_CONT_HEAD_MODE_OFST) & 
                               NTP_CONT_HEAD_MODE_MSK); 
                    
 ntp_data->stratum        = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >>
                               NTP_CONT_HEAD_STRATUM_OFST)& 
                               NTP_CONT_HEAD_STRATUM_MSK);
                       
 ntp_data->poll_interval  = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >> 
                               NTP_CONT_HEAD_POLL_OFST) & 
                               NTP_CONT_HEAD_POLL_MSK); 
                             
 ntp_data->precision      = ((*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) >>
                               NTP_CONT_HEAD_PRECISION_OFST) & 
                               NTP_CONT_HEAD_PRECISION_MSK); 

//NTP Root Delay Decode
 ntp_data->root_delay       = *NTP_DATA_GRAM_ROOT_DELAY(ntp_buffer);
//NTP Root Dispersion Decode
 ntp_data->root_dispersion  = *NTP_DATA_GRAM_ROOT_DISP(ntp_buffer);
//NTP Reference ID Decode
 ntp_data->ref_id           = *NTP_DATA_GRAM_REF_ID(ntp_buffer);
//NTP Reference Timestamp Decode
 ntp_data->ref_timestamp1   = *NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer);
 ntp_data->ref_timestamp2   = *NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer);
//NTP Originate Timestamp Decode 
 ntp_data->orig_timestamp1  = *NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer);
 ntp_data->orig_timestamp2  = *NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer);
//NTP Receive Timestamp Decode 
 ntp_data->recv_timestamp1  = *NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer);
 ntp_data->recv_timestamp2  = *NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer);
//NTP Transmit Timestamp Decode 
 ntp_data->trans_timestamp1 = *NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer);
 ntp_data->trans_timestamp2 = *NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer);
//NTP Key ID Decode 
 ntp_data->key_id           = *NTP_DATA_GRAM_KEY_ID(ntp_buffer);
//NTP Message Digest Decode 
 ntp_data->msg_digest1      = *NTP_DATA_GRAM_MSG_DIG1(ntp_buffer);
 ntp_data->msg_digest2      = *NTP_DATA_GRAM_MSG_DIG2(ntp_buffer);
 ntp_data->msg_digest3      = *NTP_DATA_GRAM_MSG_DIG3(ntp_buffer);
 ntp_data->msg_digest4      = *NTP_DATA_GRAM_MSG_DIG4(ntp_buffer);
 
 return 0;
}

/*
 * The purpose of this function is to encode NTP data from a 
 * ntp_data_gram_struc and place it in a NTP buffer in a order ready for 
 * transmission.  
 */
 
 int encode_ntp_data(alt_u8 * ntp_buffer, ntp_struct *ntp_data)
{
//NTP Control Header Encode 
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->leap_indicator << 
                                               NTP_CONT_HEAD_LI_OFST);
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->version_number << 
                                               NTP_CONT_HEAD_VN_OFST);
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->mode << 
                                               NTP_CONT_HEAD_MODE_OFST);
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->stratum << 
                                               NTP_CONT_HEAD_STRATUM_OFST);
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->poll_interval << 
                                               NTP_CONT_HEAD_POLL_OFST);
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) |= (ntp_data->precision << 
                                               NTP_CONT_HEAD_PRECISION_OFST);
//NTP Root Delay Encode
 *NTP_DATA_GRAM_ROOT_DELAY(ntp_buffer)       = ntp_data->root_delay;
//NTP Root Dispersion Encode
 *NTP_DATA_GRAM_ROOT_DISP(ntp_buffer)        = ntp_data->root_dispersion;
//NTP Reference ID Encode
 *NTP_DATA_GRAM_REF_ID(ntp_buffer)           = ntp_data->ref_id;
//NTP Reference Timestamp Encode 
 *NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer)    = ntp_data->ref_timestamp1;
 *NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer)   = ntp_data->ref_timestamp2;
//NTP Originate Timestamp Encode  
 *NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer)   = ntp_data->orig_timestamp1;
 *NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer)  = ntp_data->orig_timestamp2;
//NTP Receive Timestamp Encode 
 *NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer)   = ntp_data->recv_timestamp1;
 *NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer)  = ntp_data->recv_timestamp2;
//NTP Transmit Timestamp Encode 
 *NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer)  = ntp_data->trans_timestamp1;
 *NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer) = ntp_data->trans_timestamp2;
//NTP Key ID Encode  
 *NTP_DATA_GRAM_KEY_ID(ntp_buffer)           = ntp_data->key_id;
//NTP Message Digest Encode  
 *NTP_DATA_GRAM_MSG_DIG1(ntp_buffer)         = ntp_data->msg_digest1;
 *NTP_DATA_GRAM_MSG_DIG2(ntp_buffer)         = ntp_data->msg_digest2;
 *NTP_DATA_GRAM_MSG_DIG3(ntp_buffer)         = ntp_data->msg_digest3;
 *NTP_DATA_GRAM_MSG_DIG4(ntp_buffer)         = ntp_data->msg_digest4;
 
//Convert the NTP host data to network byte order 
 convert_ntp_data_to_net(ntp_buffer);
 
 return 0;
}

/*
 * The purpose of this function is to convert an NTP buffer area from network
 * byte order to host byte order.
 */
static int convert_ntp_data_to_host(alt_u8 * ntp_buffer)
{
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer));
  
 *NTP_DATA_GRAM_ROOT_DISP(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_ROOT_DISP(ntp_buffer));
 
 *NTP_DATA_GRAM_REF_ID(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_REF_ID(ntp_buffer));
 
 *NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer));
 
 *NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer));
 
 *NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer));
 
 *NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer));

 *NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_KEY_ID(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_KEY_ID(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG1(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_MSG_DIG1(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG2(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_MSG_DIG2(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG3(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_MSG_DIG3(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG4(ntp_buffer) = 
  ntohl(*NTP_DATA_GRAM_MSG_DIG4(ntp_buffer));
     
 return 0;
}

/*
 * The purpose of this function is to convert an NTP buffer area from host
 * byte order to network byte order.
 */

static int convert_ntp_data_to_net(alt_u8 * ntp_buffer)
{
 *NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_CONTROL_HEADER(ntp_buffer));
  
 *NTP_DATA_GRAM_ROOT_DISP(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_ROOT_DISP(ntp_buffer));
  
 *NTP_DATA_GRAM_REF_ID(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_REF_ID(ntp_buffer));
  
 *NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_REF_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_REF_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_ORIG_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_RECV_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_RECV_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_TRANS_TSTMP_INT(ntp_buffer));
  
 *NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(ntp_buffer));
  
 *NTP_DATA_GRAM_KEY_ID(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_KEY_ID(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG1(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_MSG_DIG1(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG2(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_MSG_DIG2(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG3(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_MSG_DIG3(ntp_buffer));
  
 *NTP_DATA_GRAM_MSG_DIG4(ntp_buffer) = 
  htonl(*NTP_DATA_GRAM_MSG_DIG4(ntp_buffer));
     
 return 0;
}


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
