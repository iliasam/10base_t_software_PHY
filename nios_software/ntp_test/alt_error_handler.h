/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************                                                                             *
* Date - October 24, 2006                                                     *
* Module - alt_error_handler.h                                                *
*                                                                             *                                                                             *
******************************************************************************/

/*The following functions are MicroC/OS-II system call, network stack system
 * call, and Simple Socket Server Application call error handlers that perform 
 * error handling appropriate for development.  If the error_code is not 0,
 * an error has occurred, and the scheduling is turned off while the error is 
 * processed.  Depending on the severity  level, as determined by the value 
 * set for FAULT_LEVEL, either control is returned to the task, the task is 
 * deleted, or the system scheduler is suspended.
 * 
 * In a deployed embedded system, errors must be 
 * handled according to the data storage and communications capabilities of
 * embedded hardware platform.  For example, a JTAG UART requires a host PC to
 * receive the JTAG commnand, and a serial UART requires a terminal at the end
 * of the terminal to receive the commands.  Development PCs and terminals 
 * normally do not exist in an embedded system.  
 * 
 * The manner in which the errors are resolved also changes in deployed embedded
 * systems.  While developing, it is desireable to have the system halt task 
 * activity and report an error message, so that the developer can study the 
 * context in which the error occurred.  The scheduler is locked so that the 
 * developer can read the error message written to standard out (or standard
 * error), without having the message garbled by having messages from other 
 * tasks interleaved on the output device.
 * 
 *    In a live deployed system, it is desireable not to disable the scheduler.  
 * To minimize task latency, th scheduler should not be locked while an error is 
 * processed. Worst case, the embedded system hardware should be power cycled, 
 * so that it can recover on its own.  Some intelligent checking in the error 
 * handler can take a less severe error recovery method, such as deleting and 
 * restarting only the errant task,  or by freeing up resources required by 
 * the errant task so that it may continue.  In addition, errors should be 
 * logged to some form of persistent data storage in a deployed embedded system 
 * so that a log of activity is available to the engineer who may have to 
 * resolve a critical failure in the field.
 * 
 * The purpose of this development error handler is to simply alert the 
 * developer that a task has encountered a problem by writing error details 
 * to standard output, and then hold the task in a busy loop so that a 
 * developer can study the cause of the error.
 * 
 * A MicroC/OS-II system call passes an output value or sets a return value 
 * with some status other than "OK".  The implementation of MicroC/OS-II 
 * terminates the task or halts the system.  In a deployed embedded 
 * application, there should be some attempt made to recover from errors 
 * from system calls where possible, by recreating the task which caused the 
 * error, or resetting the entire application if necessary.  If any means exist,
 * the error should be logged, such as to a local persistent memory store, or
 * to an external data collection device.  Some embedded applications even send
 * errors out over the internet for handling by a central facility.  This 
 * function has an error class input to classify the error by application 
 * or system module.  This feature can also be used to controls the flow 
 * that is taken from different errors occurring at different locations in 
 * the application code.
 */
 

#ifndef __ALT_ERROR_HANDLER_H__
#define __ALT_ERROR_HANDLER_H__

/* MicroC/OS-II definitions */
#include "includes.h"

/* EXPANDED_DIAGNOSIS_CODE value of 255 is equivalent to -1 
 * (after casting to INT8S) for functions that return -1 as an error_code 
 * to indicate errno has been set. 
 */
 
#define EXPANDED_DIAGNOSIS_CODE 255

enum FAULT_LEVEL_ENUM {NONE, TASK, SYSTEM};
typedef enum FAULT_LEVEL_ENUM FAULT_LEVEL;

extern void alt_uCOSIIErrorHandler(INT8U error_code, 
                                   void *expanded_diagnosis_ptr);
extern void alt_NetworkErrorHandler(INT8U error_code, 
                                    void *expanded_diagnosis_ptr);
extern void alt_SSSErrorHandler(INT8U error_code, 
                                void *expanded_diagnosis_ptr);


#endif /*__ALT_ERROR_HANDLER_H__ */

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
