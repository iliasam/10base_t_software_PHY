/******************************************************************************
* Copyright © 2009 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
* This is a basic NTP Client example.  The full NTP protocol is not           *
* implemented.  Just enough is done to get the time and set the system clock. *
* The goal of this is to show the basics of a sockets base client.            *
*                                                                             *
* This example uses the sockets interface. A good introduction to sockets     *
* programming is the Unix Network Programming book by Richard Stevens         *
******************************************************************************/


#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#include "includes.h"
#include "ipport.h"
#include "tcpport.h"
#include "osport.h"
#include "alt_types.h"

#include "ntp_client.h"
#include "alt_error_handler.h"
#include "network_utilities.h"

#include "target_clock.h"
#include "ntp_data_gram.h"


/* Define your NTP Server Address */
/* Example: IP Address for 95.213.132.254 (ru.pool.ntp.org) */

#define NTP_IP_ADDR0  95
#define NTP_IP_ADDR1  213
#define NTP_IP_ADDR2  132
#define NTP_IP_ADDR3  254

#define NTP_SERVER_ADDR   ((NTP_IP_ADDR0 << 24) | (NTP_IP_ADDR1 << 16) | (NTP_IP_ADDR2 << 8) | (NTP_IP_ADDR3))

#define NUM_TICKS 0x1000

OS_EVENT  *NTPCommandQ;
#define NTP_COMMAND_Q_SIZE  1024  /* Message capacity of NTPCommandQ */
void *NTPCommandQTbl[NTP_COMMAND_Q_SIZE]; /*Storage for NTPCommandQ*/
/*
 * Create our MicroC/OS-II resources. All of the resources beginning with 
 * "NTP" are declared in this file, and created in this function.
 */
void NTPCreateOSDataStructs(void)
{
/*
  * Create the resource for our MicroC/OS-II Queue for sending commands 
  * received on the TCP/IP socket from the SSSSimpleSocketServerTask()
  * to the LEDManagementTask().
  */
  NTPCommandQ = OSQCreate(&NTPCommandQTbl[0], NTP_COMMAND_Q_SIZE);
  if (!NTPCommandQ)
  {
     alt_uCOSIIErrorHandler(EXPANDED_DIAGNOSIS_CODE, 
     "Failed to create NTPCommandQ.\n");
  }
}

/* This function is a minimal SNTP Client implementation.  A NTP request
 * is sent and upon receiving a response, the system clock is set.
 */
//void ntp_func(void* pd)
void NTPTask(void* pd)
//void NTPTask(void)
{
  int        socket_fd;
  struct     sockaddr_in addr;
  ntp_struct ntp_send_data;
  ntp_struct ntp_recv_data;
  alt_u8     *send_buffer;
  alt_u8     *recv_buffer;
  alt_u32    sock_len;
  fd_set     socket_fds;
  int        req_delay = 1; //delay in minutes
  struct timeval timeout = {5, 0}; //timeout in {seconds, microseconds}

  /* The size of addr struct is used several times, so we just get the size once here.*/
  sock_len = sizeof(addr);

  /*Clear all of our data structures*/
  memset (&ntp_send_data, 0, sizeof(ntp_send_data));
  memset (&ntp_recv_data, 0, sizeof(ntp_recv_data));
  memset (&addr, 0, sock_len);

/* NTP has many different data fields, but for the most part a client that is
 * going to request the time from a sever only really needs set two values.  The
 * mode and version number.  We set those values here and then encode the entire
 * ntp_send_data struct to the NTP format.  Because the ntp_send_data_struct
 * has been zero initialized, all other fields will be sent as zeros. To be a
 * full implementation, other values may need to be set.  Functionally this gets
 * us what we want which is simply the time.
 */
  ntp_send_data.version_number = 3;
  ntp_send_data.mode = NTP_CLIENT;

  /* Here we declare our socket. For this example we declare a UDP socket as
   * NTP uses UDP.
   */
  if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
	  alt_NetworkErrorHandler(EXPANDED_DIAGNOSIS_CODE,"[ntp_task] Socket creation failed");
  }

 /* Now we fill information into our sockaddr_in struct about the server we are
   * going to communicate with.  In this case, we are using the AF_INET protocol
   * family.  Port number 123 is used for UDP, and finally we are broadcasting
   * this message to any NTP server.  So the IP address is specified as any.
   */
   addr.sin_family = AF_INET;
   addr.sin_port = htons(NTP_PORT_NUM);
   addr.sin_addr.s_addr = htonl(NTP_SERVER_ADDR);

   send_buffer = (alt_u8 *)malloc(NTP_BUFFER_SIZE);
   recv_buffer = (alt_u8 *)malloc(NTP_BUFFER_SIZE);

   memset(send_buffer, 0x0, NTP_BUFFER_SIZE);
   memset(recv_buffer, 0x0, NTP_BUFFER_SIZE);

 /* Here we do the communication with the NTP server.  This is a very simple 
   * client architecture.  A request is sent and then a NTP packet is received.
   * The NTP packet received is decoded to the ntp_recv_data structure for easy 
   * access.
   */
  while(1)
  {
    encode_ntp_data(send_buffer, &ntp_send_data);

    FD_ZERO(&socket_fds);

    if(!FD_ISSET(socket_fd, &socket_fds))
    {
      printf("Sending a NTP packet...\n");
      if(sendto(socket_fd, send_buffer, NTP_BUFFER_SIZE, 0, (struct sockaddr *)&addr, sock_len) < 0)
      {
          alt_NTPErrorHandler(EXPANDED_DIAGNOSIS_CODE,"[ntp_func] Error sending NTP packet:\n");
      }

      /* set a timeout on the recieve, so we don't lock up on a missed packet */
      FD_SET(socket_fd, &socket_fds);
      select(socket_fd+1, &socket_fds, NULL, NULL, &timeout);
    }

    if(FD_ISSET(socket_fd, &socket_fds))
    {
      if(recvfrom(socket_fd, (void *)recv_buffer, NTP_BUFFER_SIZE, 0, (struct sockaddr *)&addr, (int *) &sock_len) < 0)
      {
          alt_NTPErrorHandler(EXPANDED_DIAGNOSIS_CODE,"[ntp_func] Error receiving NTP packet:\n");
      }

      printf("Received a NTP response...\n");
      decode_ntp_data(recv_buffer, &ntp_recv_data);
      setclock(ntp_recv_data.recv_timestamp1);
    }

    /*
     * For a full implementation of an NTP server, the specification requires that
     * some additional things be done after the first NTP data gram is received.
     * e.g. bind to the server that the NTP data gram came from.  For the sake of
     * simplicity we will not implement those requirements, but set the system clock
     * and request the NTP data "req_delay" minutes from the time we get the first
     * NTP data back.
     */

    printf("Will send next NTP Request in %d minutes...\n",req_delay);
    OSTimeDlyHMSM(0,req_delay,0,0);
  }
  /* should never get here, but if we do free buffers */
  free(send_buffer);
  free(recv_buffer);

 return ;

}


/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2009 Altera Corporation, San Jose, California, USA.           *
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




