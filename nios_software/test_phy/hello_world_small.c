#include "system.h"
#include <stdio.h>
#include "sys/alt_stdio.h"
#include <altera_avalon_sgdma.h>
#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>
#include <altera_avalon_timer_regs.h>
#include "altera_avalon_pio_regs.h"
#include "triple_speed_ethernet_regs.h"

// Function Prototypes
void rx_ethernet_isr (void *context);
void send_packet(void);

alt_sgdma_dev * sgdma_tx_dev;
alt_sgdma_dev * sgdma_rx_dev;

alt_sgdma_descriptor tx_descriptor;
alt_sgdma_descriptor tx_descriptor_end;

alt_sgdma_descriptor rx_descriptor;
alt_sgdma_descriptor rx_descriptor_end;

unsigned int text_length;
int i;

void delay_1ms(void);
void delay_ms(unsigned int delay);

// Create a receive frame
unsigned char rx_frame[1024] = { 0 };

//simple UDP packet
unsigned char tx_frame[1024] = {
		0x00,0x00, 						// for 32-bit alignment
		0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x08,
		0x00, 0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11, 0x36, 0xbb,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1A, 0xd2, 0x7c, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02,
		0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x11,
		'\0'
};

int main()
{ 
  // Triple-speed Ethernet MegaCore base address
  volatile int * tse = (int *) ETHERNET_1_BASE;

  delay_ms(1000);
  alt_putstr("Hello from Nios II!\n");

  delay_ms(10);
  *(tse + 2) = ALTERA_TSEMAC_CMD_SW_RESET_MSK;
  delay_ms(5);

  // Open the sgdma transmit device
  sgdma_tx_dev = alt_avalon_sgdma_open ("/dev/sgdma_tx");
  if (sgdma_tx_dev == NULL) {
	  alt_printf ("Error: could not open scatter-gather dma transmit device\n");
	  return -1;
  } else alt_printf ("Opened scatter-gather dma transmit device\n");

  // Open the sgdma receive device
  sgdma_rx_dev = alt_avalon_sgdma_open ("/dev/sgdma_rx");
  if (sgdma_rx_dev == NULL) {
	  alt_printf ("Error: could not open scatter-gather dma receive device\n");
	  return -1;
  } else alt_printf ("Opened scatter-gather dma receive device\n");

 // Set interrupts for the sgdma receive device
  alt_avalon_sgdma_register_callback( sgdma_rx_dev, (alt_avalon_sgdma_callback) rx_ethernet_isr, 0x00000014, NULL );

  // Create sgdma receive descriptor
  alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor, &rx_descriptor_end, (alt_u32*)rx_frame, 0, 0 );

  // Set up non-blocking transfer of sgdma receive descriptor
  alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev, &rx_descriptor);


  // Initialize the MAC address
  //6-byte MAC primary address
  *(tse + 3) = 0x04030200;//00-02-03-04-05-06
  *(tse + 4) = 0x00000605;

  // Enable read and write transfers, gigabit Ethernet operation, and CRC forwarding
  *(tse + 2) = ALTERA_TSEMAC_CMD_TX_ENA_MSK|\
    		ALTERA_TSEMAC_CMD_RX_ENA_MSK|\
    		ALTERA_TSEMAC_CMD_RX_ERR_DISC_MSK|\
    		ALTERA_TSEMAC_CMD_NO_LENGTH_CHECK_MSK|\
    		ALTERA_TSEMAC_CMD_ENA_10_MSK;

  /* Event loop never exits. */
  int i = 0;
  while (1)
  {
	  alt_u8 status = (alt_u8)(alt_avalon_sgdma_check_descriptor_status(&rx_descriptor)) & 0xFF;
	  //printf("1_RX desc status: 0x%X\n", status);

	  status = (alt_u8)IORD(SGDMA_RX_BASE,0);
	  //printf("2_RX sgdma status: 0x%X\n", status);

	  /*
	  //manual receive
	  if (((status & (1+2+4)) != 0) || (status == 1))
	  {
		  //if ((rx_descriptor.status & 127) > 0) rx_descriptor.status&= 128 ;
		  alt_u16 length = 0;
		  length = rx_descriptor.actual_bytes_transferred;

		  if ((status & 1) == 1) printf("3_ERROR\n");
		  else printf("3_received> %d\n", length);

		  IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 4);
		  delay_ms(5);
		  IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);

		  // Create new receive sgdma descriptor
		  alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor, &rx_descriptor_end, (alt_u32*)rx_frame, 0, 0 );

		  // Set up non-blocking transfer of sgdma receive descriptor
		  alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev, &rx_descriptor );
	  }
	  */

	  if ((i%4) == 0) send_packet();
	  i++;
	  delay_ms(300);
  }

  return 0;
}

/****************************************************************************************
 * Subroutine to read incoming Ethernet frames
****************************************************************************************/
void rx_ethernet_isr (void *context)
{
	alt_u16 length = 0;

	// Wait until receive descriptor transfer is complete
	while (alt_avalon_sgdma_check_descriptor_status(&rx_descriptor) != 0)
	{}

	length = rx_descriptor.actual_bytes_transferred;
	printf("received length> %d\n", length);

	IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 4);
	delay_ms(5);
	IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);

	// Output received text
	//alt_printf( "receive> %s\n", rx_frame + 16 );

	// Create new receive sgdma descriptor
	alt_avalon_sgdma_construct_stream_to_mem_desc( &rx_descriptor, &rx_descriptor_end, (alt_u32*)rx_frame, 0, 0 );

	// Set up non-blocking transfer of sgdma receive descriptor
	alt_avalon_sgdma_do_async_transfer( sgdma_rx_dev, &rx_descriptor );
}

void send_packet(void)
{
	IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 1);
	delay_ms(5);
	IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, 0);
	delay_ms(5);
	alt_avalon_sgdma_construct_mem_to_stream_desc( &tx_descriptor, &tx_descriptor_end, (alt_u32*)tx_frame, 62, 0, 1, 1, 0 );
	// Set up non-blocking transfer of sgdma transmit descriptor
	alt_avalon_sgdma_do_async_transfer( sgdma_tx_dev, &tx_descriptor );
	// Wait until transmit descriptor transfer is complete
	while (alt_avalon_sgdma_check_descriptor_status(&tx_descriptor) != 0) {};
}


void delay_1ms(void)
{
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER1_BASE, (1<<3));//stop
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER1_BASE,0);// Clear TO Bit(Reaching 0)
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER1_BASE,(alt_u16)(100000));//50000*(1/50mhz)=1ms
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER1_BASE,(alt_u16)( (100000) >> 16 ));
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER1_BASE,(1<<2));//Start Timer

	while( (IORD_ALTERA_AVALON_TIMER_STATUS(TIMER1_BASE) & ALTERA_AVALON_TIMER_STATUS_TO_MSK) == 0 ) {}
}

void delay_ms(unsigned int delay)
{
	unsigned int i;
	for (i=0;i<delay;i++) {delay_1ms();}
}

