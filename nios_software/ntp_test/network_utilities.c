/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
******************************************************************************
* Date - October 24, 2006                                                     *
* Module - network_utilities.c                                                *
*                                                                             *
******************************************************************************/

#include <alt_types.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <sys/alt_flash.h>
#include "includes.h"
#include "io.h"
#include "ntp_client.h"

#include <alt_iniche_dev.h>

#include "ipport.h"
#include "tcpport.h"
#include "network_utilities.h"

#define IP4_ADDR(ipaddr, a,b,c,d) ipaddr = \
    htonl((((alt_u32)(a & 0xff) << 24) | ((alt_u32)(b & 0xff) << 16) | \
          ((alt_u32)(c & 0xff) << 8) | (alt_u32)(d & 0xff)))

error_t generate_mac_addr(unsigned char mac_addr[6]);

/*
* get_mac_addr
*
* Read the MAC address in a board specific way. Prompt user to enter serial 
* number to generate MAC address if failed to read from flash.
*
*/
int get_mac_addr(NET net, unsigned char mac_addr[6])
{
    error_t error = 0;
    error = generate_mac_addr(mac_addr);
    return error;
}

/*
 * get_ip_addr()
 *
 * This routine is called by InterNiche to obtain an IP address for the
 * specified network adapter. Like the MAC address, obtaining an IP address is
 * very system-dependant and therefore this function is exported for the
 * developer to control.
 *
 * In our system, we are either attempting DHCP auto-negotiation of IP address,
 * or we are setting our own static IP, Gateway, and Subnet Mask addresses our
 * self. This routine is where that happens.
 */
int get_ip_addr(alt_iniche_dev *p_dev,
                ip_addr* ipaddr,
                ip_addr* netmask,
                ip_addr* gw,
                int* use_dhcp)
{

    IP4_ADDR(*ipaddr, IPADDR0, IPADDR1, IPADDR2, IPADDR3);
    IP4_ADDR(*gw, GWADDR0, GWADDR1, GWADDR2, GWADDR3);
    IP4_ADDR(*netmask, MSKADDR0, MSKADDR1, MSKADDR2, MSKADDR3);

#ifdef DHCP_CLIENT
    *use_dhcp = 1;
#else /* not DHCP_CLIENT */
    *use_dhcp = 0;

    printf("Static IP Address is %d.%d.%d.%d\n",
        ip4_addr1(*ipaddr),
        ip4_addr2(*ipaddr),
        ip4_addr3(*ipaddr),
        ip4_addr4(*ipaddr));
#endif /* not DHCP_CLIENT */

    /* Non-standard API: return 1 for success */
    return 1;
}



/*
 * generate_mac_addr()
 * 
 * This routine is called when failed to read MAC address from flash (i.e: no 
 * flash on the board). The user is prompted to enter the serial number at the 
 * console. A MAC address is then generated using 0xFF followed by the last 2 
 * bytes of the serial number appended to Altera's Vendor ID, an assigned MAC 
 * address range with the first 3 bytes of 00:07:ED.  For example, if the Nios 
 * Development Board serial number is 040800017, the corresponding ethernet 
 * number generated will be 00:07:ED:FF:8F:11.
 * 
 */
error_t generate_mac_addr(unsigned char mac_addr[6])
{
    error_t error = -1;
    
    if (1)
    {
        /* This is the Altera Vendor ID */
        mac_addr[0] = 0x0;
        mac_addr[1] = 0x1;
        mac_addr[2] = 0x2;
        
        mac_addr[3] = 0x3;
        mac_addr[4] = 0x4;
        mac_addr[5] = 0x5;
        
        printf("Your Ethernet MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n", 
            mac_addr[0],
            mac_addr[1],
            mac_addr[2],
            mac_addr[3],
            mac_addr[4],
            mac_addr[5]);
        
        error = 0;
    }
  
    return error;    
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
