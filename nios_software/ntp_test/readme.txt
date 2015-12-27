Readme - Simple Socket Server Software Example

DESCRIPTION:	
A Simple Socket Server that controls development board LEDs.

Requirements:
  -RTOS Type - MicroC/OS-II
  -Software Component - NicheStack TCP/IP Stack - Nios II Edition
  -Period System Timer - SYS_CLK_TIMER

PERIPHERALS USED:
This example exercises the following peripherals:
- Ethernet MAC (named "lan91c111" in SOPC Builder)
- PIO, 8-bit output (named "led_pio" in SOPC Builder)
- PIO, 16-bit output (named "seven_seg_pio" in SOPC Builder)
- STDOUT device (UART or JTAG UART)

SOFTWARE SOURCE FILES:
This example includes the following software source files:

- iniche_init.c: Contains main() and SSSInitialTask() to initialize NicheStack 
and then create the other tasks once network has been properly initialized.
Tasks which will use sockets, such as the SSSSimpleSocketServerTask() in this 
example, must be created with TK_NEWTASK.  All other tasks can be created by directly
calling the MicroC/OS-II API to create a task, i.e. OSTaskCreateExt().

- simple_socket_server.c: Implementation of a simple_socket_server including all necessary sockets
calls to handle a single socket connection & process received commands.

- network_utilities.c: Contains MAC address and IP address routines to
manage addressing. These are used by NicheStack during initialization, but are
implementation-specific (you set your MAC address to whatever you want.. or read
it from your own special non-volatile memory.

- network_utilities.h: Contains prototype for function get_board_mac_addr().

- led.c: Contains tasks to manage board LED commands and update LED displays.
LEDManagementTask interprets commands, and toggles the row of 8 LEDS or signals the 
LED7SegLightshowTask in response to commands received from the host running telnet.
The LEDManagementTask reads data from a MicroC/OS-II SSSLEDCommandQ Queue which
receives its data from the SSSSimpleSocketServerTask.  LED7SegLightshowTask controls the 
7-segment display.    

- simple_socket_server.h: Definitions for the entire example application.

- alt_error_handler.h: Definitions for 3 error handlers, one each for MicroC-OS/II, Network, 
and Simple Socket Server Application.

- alt_error_handler.c: Implementation for 3 error handlers, one each for MicroC-OS/II, 
Network, and Simple Socket Server Application.

BOARD/HOST REQUIREMENTS:

This example requires an Ethernet cable connected to the development board's 
RJ-45 jack, and a JTAG connection with the development board. If the host 
communication settings are changed from JTAG UART (default) to use a
conventional UART, a serial cable between board DB-9 connector  and the host is
required. 

If DHCP is available (and enabled in the Software component configuration page, from
the BSP properties configuration), NicheStack TCP/IP Stack will attempt 
to obtain an IP address from a DHCP server. Otherwise, a static IP address (defined in 
Simple_Socket_Server.h) will be assigned after a DHCP timeout.  

ADDITIONAL INFORMATION:

This is an example socket server using NicheStack TCP/IP Stack on MicroC/OS-II. The server
implements simple commands to control board LEDs through a separate MicroC/OS-II
task. It is in no way a complete implementation of a telnet server.

A good introduction to sockets programming is the book "Unix Network Programming" by 
Richard Stevens. Additionally, the text "Sockets in C", by Donahoo & Calvert, is a concise 
& inexpensive text for getting started with sockets programming.

This example will not run on the Instruction Set Simulator (ISS).

Once the simple socket server example is running and has obtained an IP address (shown 
in the terminal window of Nios II Software Build Tools for Eclipse), 
you can connect to it over a network by typing the 
following command in a command shell on a development host:

  telnet <ip address> 30

This command will try to connect to the Simple Socket Server using port 30.
