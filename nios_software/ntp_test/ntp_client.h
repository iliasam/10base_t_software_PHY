/******************************************************************************
* Copyright © 2004 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
*                                                                             *
******************************************************************************/
#ifndef __NTP_CLIENT_H__
#define __NTP_CLIENT_H__

#if !defined (ALT_INICHE)
  #error The NTP example requires the 
  #error NicheStack TCP/IP Stack Software Component. Please see the Nichestack
  #error Tutorial for details on Nichestack TCP/IP Stack - Nios II Edition,
  #error including notes on migrating applications from lwIP to NicheStack.
#endif

#ifndef __ucosii__
  #error This NTP example requires 
  #error the MicroC/OS-II Intellectual Property Software Component.
#endif


#define NTP_PORT_NUM 123
#define NTP_CLIENT 3

/* Definition of Task Stacks for tasks not using networking. */
#define   TASK_STACKSIZE       2048



//void ntp_func(void* pd);
void NTPTask(void* pd);
void NTPCreateOSDataStructs();

//extern OS_EVENT *NTPCommandQ;
/*
 *  Task priorities
 *
 *
 *  uC OS II only allows one task (thread) per priority
 */

#define NTP_INITIAL_TASK_PRIORITY  	8
#define NTP_TASK_PRIORITY          	9
//#ifdef LCD_DISPLAY_NAME   
#define LCD_DISP_PRIO  				10 
//#endif /* LCD_DISPLAY_NAME */
/*
* Time we wait for DHCP to assign an IP address before settling on a static address
*/
#define DHCP_TIMEOUT ((120 * 1000)/100)

#define NTP_RX_BUF_SIZE  1500
#define NTP_TX_BUF_SIZE  1500
#define   TASK_STACKSIZE       2048

#define IPADDR0   0
#define IPADDR1   0
#define IPADDR2   0
#define IPADDR3   0

#define GWADDR0   0
#define GWADDR1   0
#define GWADDR2   0
#define GWADDR3   0

#define MSKADDR0  255
#define MSKADDR1  255
#define MSKADDR2  255
#define MSKADDR3  0

#endif /* __NTP_CLIENT_H__ */

/* 
 * Here we structure to manage ntp communication for a single connection
 */
//typedef struct NTP_SOCKET
//{
//  enum { READY, COMPLETE, CLOSE } state; 
//  int       fd;
//  int       close;
//  INT8U     rx_buffer[NTP_RX_BUF_SIZE];
//  INT8U    *rx_rd_pos; /* position we've read up to */
//  INT8U     *rx_wr_pos; /* position we've written up to */
//} NTPConn;


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
