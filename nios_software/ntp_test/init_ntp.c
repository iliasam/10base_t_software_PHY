/******************************************************************************
* Copyright © 2004 Altera Corporation, San Jose, California, USA.             *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
*                                                                             *
* LWIP has two API's, a callback interface and "standard" sockets. This       *
* example uses the sockets interface. A good introduction to sockets          *
* programming is the Unix Network Programming book by Richard Stevens.        *
*                                                                             *
******************************************************************************/
#include <stdio.h>
#include "ntp_client.h"
#include "network_utilities.h"
#include "target_clock.h"
#include "alt_types.h"

/* MicroC/OS-II definitions */
#include "includes.h"
#include "alt_error_handler.h"

/* Nichestack definitions */
#include "ipport.h"
#include "libport.h"
#include "osport.h"

#ifdef LCD_DISPLAY_NAME 
FILE* lcdDevice;
#endif /* LCD_DISPLAY_NAME */

/* Definition of task stack for the initial task which will initialize the NicheStack
 * TCP/IP Stack and then initialize the rest of the Simple Socket Server example tasks. 
 */
OS_STK    NTPInitialTaskStk[TASK_STACKSIZE];
//#ifdef LCD_DISPLAY_NAME 
OS_STK    lcddisplaytime_task_stack[TASK_STACKSIZE];
//#endif /* LCD_DISPLAY_NAME */

/* Declarations for creating a task with TK_NEWTASK.  
 * All tasks which use NicheStack (those that use sockets) must be created this way.
 * TK_OBJECT macro creates the static task object used by NicheStack during operation.
 * TK_ENTRY macro corresponds to the entry point, or defined function name, of the task.
 * inet_taskinfo is the structure used by TK_NEWTASK to create the task.
 */
TK_OBJECT(to_ntp_task);
TK_ENTRY(NTPTask);

struct inet_taskinfo ntp_task = {
      &to_ntp_task,
      "ntp",
      NTPTask,
      NTP_TASK_PRIORITY,
     APP_STACK_SIZE,
};

/* NTPInitialTask will initialize the NicheStack
 * TCP/IP Stack and then initialize the rest of the Simple Socket Server example 
 * RTOS structures and tasks. 
 */
void NTPInitialTask(void *task_data)
{
  INT8U error_code;
  
  /*
   * Initialize Altera NicheStack TCP/IP Stack - Nios II Edition specific code.
   * NicheStack is initialized from a task, so that RTOS will have started, and 
   * I/O drivers are available.  Two tasks are created:
   *    "Inet main"  task with priority 2
   *    "clock tick" task with priority 3
   */   
  alt_iniche_init();
  netmain(); 

  /* Wait for the network stack to be ready before proceeding. 
   * iniche_net_ready indicates that TCP/IP stack is ready, and IP address is obtained.
   */
  while (!iniche_net_ready)
    TK_SLEEP(1);

  /* Now that the stack is running, perform the application initialization steps */
  
  /* Application Specific Task Launching Code Block Begin */

  printf("\nNTP starting up\n");

  /* Create the main ntp task. - create main simple socket server task*/
  TK_NEWTASK(&ntp_task);
  
  /*create os data structures */
  NTPCreateOSDataStructs(); 

  /* create the other tasks */
  error_code = OSTaskCreateExt(lcddisplaytime_task,
								NULL,
								(void *)&lcddisplaytime_task_stack[TASK_STACKSIZE-1],
								LCD_DISP_PRIO,
								LCD_DISP_PRIO,
								lcddisplaytime_task_stack,
                             TASK_STACKSIZE,
                             NULL,
                             0);
   
   alt_uCOSIIErrorHandler(error_code, 0);
   printf("\n\nNTP LCD Time Display Task Created\n\n");
  

  /* Application Specific Task Launching Code Block End */
  
  /*This task is deleted because there is no need for it to run again */
  error_code = OSTaskDel(OS_PRIO_SELF);
  alt_uCOSIIErrorHandler(error_code, 0);
  while (1); /* Correct Program Flow should never get here */
}

int main ()
{
  INT8U error_code;

  setup_phy();

  /* Clear the RTOS timer */
  OSTimeSet(0);

  printf("\n\nNTP Client starting up\n\n");
  
  /* NTPInitialTask will initialize the NicheStack
   * TCP/IP Stack and then initialize the rest of the 
   * RTOS structures and tasks. 
   */  
  error_code = OSTaskCreateExt(NTPInitialTask,
                             NULL,
                             (void *)&NTPInitialTaskStk[TASK_STACKSIZE],
                             NTP_INITIAL_TASK_PRIORITY,
                             NTP_INITIAL_TASK_PRIORITY,
                             NTPInitialTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);



/*
   * As with all MicroC/OS-II designs, once the initial thread(s) and 
   * associated RTOS resources are declared, we start the RTOS. That's it!
   */  
  printf("\n\nRTOS starting up\n\n");
  OSStart();

  
  while(1); /* Correct Program Flow never gets here. */

  return -1;
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
