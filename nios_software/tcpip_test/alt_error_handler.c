/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************                                                                             *
* Date - October 24, 2006                                                     *
* Module - alt_error_handler.c                                                *
*                                                                             *
******************************************************************************/

/*
 * The following functions are MicroC/OS-II system call, network stack system
 * call, and Simple Socket Server Application call error handlers that perform 
 * error handling appropriate for development.  If the error_code is not 0,
 * an error has occurred, and task scheduling is turned off while the error is 
 * processed.  Depending on the severity  level, as determined by the value 
 * set for FAULT_LEVEL (NONE, TASK, or SYSTEM), either control is returned to 
 * the task, the task is deleted, or the system scheduler is suspended,
 * respectively.
 * 
 * In a deployed embedded system, errors must be 
 * handled according to the data storage and communications capabilities of an
 * embedded hardware platform.  For example, a JTAG UART requires a host PC to
 * receive the JTAG UART printf command, and a serial UART requires a device 
 * at the end of the cable to display the output.  Development PCs and 
 * terminals normally do not exist in a deployed embedded system, so other 
 * means of logging the error must be used.  
 * 
 * The manner in which errors are resolved also changes in deployed embedded
 * systems.  While developing, it is desireable to have the system halt task 
 * activity and report an error message so that the developer can study the 
 * context in which the error occurred.  The scheduler is locked so that the 
 * developer can read the error message written to standard out (or stderr), 
 * without having the message garbled by having messages from other tasks 
 * interleaved on the output device, or scrolled out of view by other task 
 * console output.
 * 
 * In a live deployed system, it is desireable not to disable the scheduler.  
 * To minimize task latency, the scheduler should not be locked while the error 
 * is processed. Worst case, the embedded system hardware should be power-
 * cycled, so that it can recover on its own.  Some intelligent checking in the 
 * error handler can take a less severe error recovery method, such as deleting 
 * and restarting only the errant task,  or by freeing up resources required by 
 * the errant task so that it may continue.  In addition, errors should be 
 * logged to some form of persistent data storage in a deployed embedded system 
 * so that a log of activity is available 
 * to the engineer who may have to resolve a critical failure in the field.
 * 
 * The purpose of this development error handler is to simply alert a developer
 * that a task has encountered a problem by writing error details to standard 
 * output, so that a developer can study the cause of the error.
 * 
 * A MicroC/OS-II system call passes an output value or sets a return value 
 * with some status other than "OK".  The network stack will set errno in some
 * cases.  The implementation of MicroC/OS-II, network stack, and SSS
 * application error handlers take the following actions based on the fault
 * status:
 * 
 * Fault Status           Action
 * ------------           ------
 * TASK                   Terminates the task
 * SYSTEM                 Halts the scheduler, enters an endless loop
 * NONE                   Returns control to the application task
 *  
 * In a deployed embedded systems application, there should be some attempt made 
 * to recover from errors where possible, by recreating task which caused the 
 * error in the case of a TASK fault level, or resetting the entire application, 
 * if necessary due to a SYSTEM fault level.  In addition, if any means exist,
 * the error should be logged, such as to a local persistent memory store, or
 * to an external data collection device.  Some embedded applications even send
 * errors out over the internet for handling by a central facility.  This error
 * handling function to invoke should be chosen based on whether errors are the 
 * result of the application only, or invocation of the network stack or
 * MicroC/OS-II system call. 
 * 
 *    Additional extensions can be made to the use of EXPANDED_DIAGNOSIS
 * value for error_code.  This feature could also be used to control the flow 
 * that is taken from different errors occurring at different locations in the 
 * application code.  For example, instead of using the expanded_diagnosis_ptr 
 * as a string to be printed, as it is used below, the expanded_diagnosis_ptr 
 * could point to a different function to be invoked, or it could point to a
 * structure which contains several items for analysis, including multiple
 * function pointers to be invoked based on other structure elements which 
 * indicate the state of the application at the time of the failure.  This 
 * extensibility capability for a variety of overloaded uses is the reason 
 * for declaration of the expanded_diagnosis_ptr as void *.
 */
 
 /*
 * Include files:
 * 
 * <stdio.h>: Contains C "{fp,p}rintf()" functions.
 * includes.h: This is the default MicroC/OS-II include file.
 * alt_error_handler.h: Altera Error Handler suite of development 
 * error handling functions.
 */
#include <stdio.h>
#include <errno.h>
#include "includes.h"
#include "alt_error_handler.h"
 
void alt_uCOSIIErrorHandler(INT8U error_code, void *expanded_diagnosis_ptr)
{
   FAULT_LEVEL fault_level;
   
   if(error_code == OS_NO_ERR)
   {
      return;
   }
   
   fault_level = SYSTEM;  
   OSSchedLock();  /* Disable Task Switching but still service other IRQs */
      
   switch (error_code)
   {  
      case OS_PRIO_EXIST:
         fprintf(stderr, "Attempted to assign task priority aready in use.\n");
         break;
      case OS_PRIO_INVALID:
         fprintf(stderr, "Specified task priority higher than allowed max.\n");
         fprintf(stderr, "Task can't be assigned a priority higher than %d\n",
            OS_LOWEST_PRIO);
         break;
      case OS_NO_MORE_TCB:
         fprintf(stderr, "Task Control Blocks have been exhausted\n");
         fprintf(stderr, "Current max number of tasks is %d\n",OS_MAX_TASKS);
         break;
      case OS_MBOX_FULL:
         fault_level = NONE;
         fprintf(stderr, "Attempted Post to Mailbox already holding message\n");
         break;
      case OS_ERR_EVENT_TYPE:
         fault_level = TASK;
         fprintf(stderr, 
"Attempted to access a resource with no match for the required data type.\n");
         break;
      case OS_ERR_PEVENT_NULL:
         fprintf(stderr, "Attempting to access a resource pointing to NULL\n");
         break;
      case OS_ERR_POST_NULL_PTR:
         fault_level = TASK;
         fprintf(stderr, "Attempted to Post a NULL to a resource. \n");
         break;
      case OS_TIMEOUT:
         fault_level = NONE;
         fprintf(stderr, "Resource not received in specified time\n");
         break;
      case OS_ERR_PEND_ISR:
         fprintf(stderr, "Attempting to pend for a resource in an ISR\n");
         break;
      case OS_TASK_DEL_IDLE:
         fprintf(stderr, "Attempted to delete the IDLE task\n");
         break;
      case OS_TASK_DEL_ERR:
         fault_level = NONE;
         fprintf(stderr, "Attempted to delete a task that does not exist\n");
         break;
      case OS_TASK_DEL_ISR:
         fprintf(stderr, "Attempted to delete a task from an ISR\n");
         break;
      case OS_Q_FULL:
         fault_level = NONE;
         fprintf(stderr, "Attempted to post to a full message queue\n");
         break;
      case OS_ERR_NOT_MUTEX_OWNER:
         fault_level = TASK;
         fprintf(stderr, "Attempted to post a mutex not owned by the task\n");
         break;
      case EXPANDED_DIAGNOSIS_CODE:      
         fault_level = SYSTEM;
         printf(
"\n[MicroC/OS-II]: See STDERR for expanded diagnosis translation.");    
         fprintf(stderr, "\n[MicroC/OS-II]: Expanded Diagnosis: %s.", 
                 (char *)expanded_diagnosis_ptr);
         break;           
      default:
         printf("\n[MicroC/OS-II]: (Not a MicroC/OS-II error) See STDERR.\n");    
         fprintf(stderr, "\n[MicroC/OS-II]:");
         fprintf(stderr, "\nError_code %d.\n", error_code);
         perror("\n[MicroC/OS-II]: (Not a MicroC/OS-II error), ERRNO: ");
         break;

   }

   /* Process the error based on the fault level, 
    * reenable scheduler if appropriate. */  
   switch (fault_level) {
      case TASK:
         /* Error can be isolated by killing the task */
         printf("\n[MicroC/OS-II]: See STDERR (FAULT_LEVEL is TASK).");
         fprintf(stderr, "\n[MicroC/OS-II]: FAULT_LEVEL is TASK");
         fprintf(stderr, "\n[MicroC/OS-II]: Task is being deleted.\n");
         OSSchedUnlock(); /* Reenable Task Switching */
         OSTaskDel(OS_PRIO_SELF);
         /* Reinvoke uCOSII error handler in case task deletion fails, in 
          * which case fault_level for this secondary error will be SYSTEM. */
         alt_uCOSIIErrorHandler(error_code, 0);         
         break;
      case SYSTEM:
         /* Total System Failure, Restart Required */
         printf("\n[MicroC/OS-II]: See STDERR (FAULT_LEVEL is SYSTEM).");    
         fprintf(stderr, "\n[MicroC/OS-II]: FAULT_LEVEL is SYSTEM");
         fprintf(stderr, "\n[MicroC/OS-II]: FATAL Error, Restart required.");
         fprintf(stderr, "\n[MicroC/OS-II]: Locking scheduler - endless loop.\n");
         while(1); /* Since scheduler is locked,loop halts all task activity.*/
         break;
      case NONE:
         fprintf(stderr, "\n[MicroC/OS-II]: FAULT_LEVEL is NONE");
         fprintf(stderr, "\n[MicroC/OS-II]: Informational error only, control"); 
         fprintf(stderr, 
            "returned to task to complete processing at application level.\n");
         OSSchedUnlock(); /* Reenable Task Switching */
         return;   
         break;      
      default:
         printf("\n[MicroC/OS-II]: See STDERR (FAULT_LEVEL is Unknown).\n");
         fprintf(stderr, "\n[MicroC/OS-II]: FAULT_LEVEL is unknown!?!\n");
   }
   while(1); /* Correct Program Flow never gets here. */
}

void alt_NetworkErrorHandler(INT8U error_code, void *expanded_diagnosis_ptr)
{
   FAULT_LEVEL fault_level;

   if(error_code == OS_NO_ERR)
   {
      return;
   }

   fault_level = SYSTEM;   
   OSSchedLock();  /* Disable Task Switching but still service other IRQs */  

   if (error_code == EXPANDED_DIAGNOSIS_CODE) 
   {
      fault_level = SYSTEM;
      printf("\n[Network]: See STDERR for expanded diagnosis translation.");    
      fprintf(stderr, "\n[Network]: %s", (char *)expanded_diagnosis_ptr);
      /* Check errno also in case it has been set. */
      perror("\n[Network]:  ERRNO: ");
   }
   else 
   {
      fault_level = TASK;
      printf("\n[Network]: See STDERR.\n");    
      fprintf(stderr, "\n[Network]: Error_code %d!\n", error_code);        
      perror("\n[Network]:  ERRNO: ");
   }

   /* Process error based on fault level, reenable scheduler if appropriate. */     
   switch (fault_level) 
   {
      case TASK:
         /* Error can be isolated by killing the task */
         printf("\n[Network]: See STDERR (FAULT_LEVEL is TASK).");
         fprintf(stderr, "\n[Network]: FAULT_LEVEL is TASK");
         fprintf(stderr, "\n[Network]: Task is being deleted.\n");
         OSSchedUnlock(); /* Reenable Task Switching */
         OSTaskDel(OS_PRIO_SELF);
         /* Reinvoke uCOSII error handler in case task deletion fails, in 
          * which case fault_level for this secondary error will be SYSTEM. */
         alt_uCOSIIErrorHandler(error_code, 0);         
         break;
      case SYSTEM:
         /* Total System Failure, Restart Required */
         printf("\n[Network]: See STDERR (FAULT_LEVEL is SYSTEM).");    
         fprintf(stderr, "\n[Network]: FAULT_LEVEL is SYSTEM.");
         fprintf(stderr, "\n[Network]: FATAL Error, Restart required.");
         fprintf(stderr, "\n[Network]: Locking scheduler - endless loop.\n");
         while(1); /* Since scheduler is locked, loop halts all task activity.*/
         break;
      case NONE:
         fprintf(stderr, "\n[Network]: FAULT_LEVEL is NONE.");
         fprintf(stderr, "\n[Network]: Informational "
                         "error only, control returned");
         fprintf(stderr, 
            "to task to complete processing at the application level.\n");
         OSSchedUnlock(); /* Reenable Task Switching */ 
         return;
         break;         
      default:
         printf("\n[Network]: See STDERR (FAULT_LEVEL is unknown).\n");    
         fprintf(stderr, "\n[Network] FAULT_LEVEL is unknown !?!\n");
   }
   while(1); /* Correct Program Flow never gets here. */
}
   
   
void alt_SSSErrorHandler(INT8U error_code, 
                         void *expanded_diagnosis_ptr)
{
   FAULT_LEVEL fault_level;
   
   if   (error_code == OS_NO_ERR)
   {
      return;
   }

   fault_level = (error_code == OS_NO_ERR) ? NONE : SYSTEM;
   
   OSSchedLock();  /* Disable Task Switching but still service other IRQs */
   switch (error_code)
   {
      case EXPANDED_DIAGNOSIS_CODE:      
         fault_level = SYSTEM;
         printf("\n[SSS]: See STDERR for expanded diagnosis translation.");    
         fprintf(stderr, "\n[SSS]: %s", (char *)expanded_diagnosis_ptr);
         break;
         
      case OS_Q_FULL:
         fault_level = NONE;
         fprintf(stderr,"\n[SSS]: Attempted to post to a full message queue.");
         break;
      
      default:
         fault_level = SYSTEM;
         printf("\n[SSS]: See STDERR.\n");    
         fprintf(stderr, "\n[SSS]: Error_code %d!", error_code);        
         perror("\n[SSS]:  ERRNO: ");
   }

   /* Process the error based on the fault level, 
    * reenable scheduler if appropriate. */     
   switch (fault_level) 
   {
      case TASK:
         /* Error can be isolated by killing the task */
         printf("\n[SSS]: See STDERR (FAULT_LEVEL is TASK).");
         fprintf(stderr, "\n[SSS]: FAULT_LEVEL is TASK");
         fprintf(stderr, "\n[SSS]: Task is being deleted.\n");
         OSSchedUnlock(); /* Reenable Task Switching */
         OSTaskDel(OS_PRIO_SELF);
         /* Invoke uCOSII error handler in case task deletion fails, in 
          * which case fault_level for this secondary error will be SYSTEM. */
         alt_uCOSIIErrorHandler(error_code, 0);         
         break;
      case SYSTEM:
          /* Total System Failure, Restart Required */
         printf("\n[SSS]: See STDERR (FAULT_LEVEL is SYSTEM).");    
         fprintf(stderr, "\n[SSS]: FAULT_LEVEL is SYSTEM.");
         fprintf(stderr, "\n[SSS]: FATAL Error, Restart required.");
         fprintf(stderr, "\n[SSS]: Locking scheduler - endless loop.\n");
         while(1); /* Since scheduler is locked, loop halts all task activity.*/
         break;
      case NONE:
         fprintf(stderr, "\n[SSS] FAULT_LEVEL is NONE.");
         fprintf(stderr, 
            "\n[SSS] Informational error only, control returned to task to ");
         fprintf(stderr,
            "complete processing at the application level.\n");
         OSSchedUnlock(); /* Reenable Task Switching */ 
         return;         
         break;
      default:
         printf("\n[SSS]: See STDERR (FAULT_LEVEL is Unknown).\n");
         fprintf(stderr, "\n[SSS] FAULT_LEVEL is unknown!?!\n");
   }
   while(1); /* Correct Program Flow never gets here. */
}
   
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
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
