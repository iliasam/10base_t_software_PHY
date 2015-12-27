/******************************************************************************
* Copyright © 2004 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
* This file is used to set/read the target system clock.                      *
*                                                                             *
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
#include "alt_error_handler.h"
#include "alt_types.h"
#include "ntp_client.h"


#include "target_clock.h"


OS_EVENT *lcd_sem;

#ifdef LCD_DISPLAY_NAME
extern  FILE* lcdDevice;
#endif /* LCD_DISPLAY_NAME */

/*
 * This function sets the system clock time.  
 */
int setclock(alt_u32 seconds)
{
  struct timeval time = {0, 0};
  struct timezone zone = {0, 0};
  
  /* 
   * NTP Time is seconds since 1900 
   * Convert to Unix time which is seconds since 1970
   */
  seconds -= NTP_TO_UNIX_TIME;
  
  time.tv_sec = seconds;

  printf("Setting System Clock time to: %s\n",ctime(&time.tv_sec));
  //settimeofday() can not be called at the same time as gettimeofday()
  OSSchedLock();  
  //settimeofday(&time, &zone);
  if(settimeofday(&time, &zone) < 0)
  {
    printf("Error set the time of day %s\n", strerror(errno));
    exit(0);
  }
  OSSchedUnlock();

  return 0;
}

/*
 * This task is called once a second to update the LCD display with the 
 * current system time.  
 */
void lcddisplaytime_task(void *pdata)
{
  struct timeval time = {0, 0};
  struct timezone zone = {0, 0};
  struct tm time_struct;
  struct tm *pt_time_struct = &time_struct;
  alt_u8 char_array1[32];
  alt_u8 char_array2[32];
  INT8U  ucos_retcode = OS_NO_ERR;

  OSSemPend(lcd_sem, 0, &ucos_retcode);
    
  while(1)
  {

    OSSchedLock();
    if(gettimeofday(&time, &zone) < 0)
    {
      printf("Error get the time of day %s\n", strerror(errno));
      exit(0);
    }
    OSSchedUnlock();
  
    pt_time_struct = gmtime(&time.tv_sec);

    strftime(char_array1, 32, "%a %b %d,%Y", pt_time_struct);
    printf("DATE: %s\n", char_array1);
    
    strftime(char_array2, 32, "%I:%M:%S %p GMT", pt_time_struct);
    printf("TIME: %s\n", char_array2);

    OSSemPost(lcd_sem);

    OSTimeDlyHMSM(0,0,10,0);
  }
  
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
