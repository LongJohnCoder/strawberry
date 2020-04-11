// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "usb_host_core.h"
#include "board_serial.h"
#include "interrupt.h"
#include "clock.h"
#include "config.h"
#include "usbhs.h"
#include "systick.h"
#include "usb_descriptors.h"


//--------------------------------------------------------------------------------------------------//


void usb_host_defaut_pipe_enable(void);


//--------------------------------------------------------------------------------------------------//


void usb_host_init(void)
{
	// Enable the USB peripheral clock
	clock_peripheral_clock_enable(ID_USBHS);
	
	// According to the data sheet, USB must be enabled and
	// freeze clock must be disabled before enabling the USB in PMC
	usbhs_global_set_mode(USB_DRIVER_HOST);
	usbhs_global_unfreeze_clock();
	usbhs_global_enable();
	
	// Enable the USB PLL and the USB full speed clock
	clock_usb_pll_config(CLOCK_SOURCE_FREQUENCY_12_MHZ, CLOCK_USB_PLL_STARTUP_TIME, 1);
	clock_usb_config(CLOCK_USB_SOURCE_USB_PLL, CLOCK_USB_FULL_SPEED_DIVIDER);
	clock_usb_full_speed_enable();
	
	// Enable USB interrupt with priority 4
	interrupt_enable_peripheral_interrupt(USBHS_IRQn, IRQ_LEVEL_4);
	
	// Use full speed anyway
	usbhs_host_set_speed_capability(USB_DRIVER_HOST_SPEED_NORMAL);
	
	// Force interrupts
	//USBHS->USBHS_HSTIFR = (1 << USBHS_HSTIER_DDISCIES_Pos) | (1 << USBHS_HSTIER_HWUPIES_Pos);

	
	// Check if clock is running
	usbhs_global_unfreeze_clock();
	
	while (!usbhs_global_clock_usable())
	{
		
	}
	board_serial_print("Clock usable\n");
	
	// Turn off hardware VBUS control
	usbhs_global_vbus_hardware_disable();

	// Clear all interrupts
	usbhs_host_interrupt_clear(USBHS_HSTIMR_DCONNIE_Msk | USBHS_HSTIMR_RSTIE_Msk | USBHS_HSTIMR_HSOFIE_Msk | USBHS_HSTIMR_HWUPIE_Msk);
	
	// Enable interrupts
	usbhs_host_interrupt_enable(USBHS_HSTIMR_DCONNIE_Msk | USBHS_HSTIMR_RSTIE_Msk | USBHS_HSTIMR_HSOFIE_Msk | USBHS_HSTIMR_HWUPIE_Msk);
	
}


//--------------------------------------------------------------------------------------------------//


void usb_host_root_hub_handler(uint32_t status)
{
	board_serial_print("Handler: Root hub\n\n");
	
	// Interrupt mask register
	uint32_t mask = usbhs_host_get_interrupt_mask();
	
	// Check if a bus reset has been sent
	if (status & USBHS_HSTISR_RSTI_Msk)
	{
		board_serial_print("Root hub: reset\n");
		
		// Clear the reset interrupt
		usbhs_host_interrupt_clear(USBHS_HSTISR_RSTI_Msk);
		
		// Reset all pipes...
		
		return;
	}
	
	// A disconnection has occurred
	else if (status & mask & USBHS_HSTISR_DDISCI_Msk)
	{
		board_serial_print("Root hub: disconnect\n");
		
		usbhs_host_interrupt_disable(USBHS_HSTISR_DDISCI_Msk);
		usbhs_host_interrupt_clear(USBHS_HSTISR_DDISCI_Msk | USBHS_HSTISR_DCONNI_Msk);
		usbhs_host_interrupt_enable(USBHS_HSTISR_DCONNI_Msk);
		
		return;
	}
	
	// A connection has occurred
	else if (status & mask & USBHS_HSTISR_DCONNI_Msk)
	{
		board_serial_print("Root hub: connection\n");
		
		usbhs_host_interrupt_clear(USBHS_HSTISR_DCONNI_Msk | USBHS_HSTISR_DDISCI_Msk);
		usbhs_host_interrupt_disable(USBHS_HSTISR_DCONNI_Msk);
		usbhs_host_interrupt_enable(USBHS_HSTISR_DDISCI_Msk);
		
		// According to specification wait 100ms before attempting reset
		// this is due to the mechanical insertion and power distribution
		systick_delay_milliseconds(100);
		
		// Issue a reset
		usbhs_host_send_reset();
		
		return;
	}
	
	if ((status & USBHS_HSTISR_HWUPI_Msk) && (mask & USBHS_HSTISR_DCONNI_Msk))
	{
		// This interrupt is triggered in one of two ways
		// When the host is in suspend mode SOF = 0 and
		//		- An upstream resume
		//		- A device disconnection
		
		board_serial_print("Root hub: wakeup to power\n");
		
		// Check if the host clock is running after the asynchronous interrupt
		usbhs_global_unfreeze_clock();
		while (!usbhs_global_clock_usable())
		{
			
		}
		
		// CLear the interrupt
		usbhs_host_interrupt_clear(USBHS_HSTISR_HWUPI_Msk);
		
		// VBUS
		usbhs_global_send_vbus_request();
	}
	
	// Resume
	if (status & mask & (USBHS_HSTISR_HWUPI_Msk | USBHS_HSTISR_RSMEDI_Msk | USBHS_HSTISR_RXRSMI_Msk))
	{
		board_serial_print("Root hub: resume\n");
		
		// Send SOF request
		//usbhs_host_SOF_enable();
	}

}


//--------------------------------------------------------------------------------------------------//


void usb_host_pipe_handler(uint32_t status)
{
	// USBdebug
	board_serial_print("Handler: Pipe\n");
}


//--------------------------------------------------------------------------------------------------//


void usb_host_dma_handler(uint32_t status)
{
	// USBdebug
	board_serial_print("Handler: DMA\n");
}


//--------------------------------------------------------------------------------------------------//


void usb_host_start_of_frame_handler(uint32_t status)
{
	// The start of frame is sent once every ms at low/full speed and once every 125 us on high-speed
	// For efficiency we don't handle micro frames in the interrupt
	
	// USBdebug
	board_serial_print("Handler: SOF\n");
	
	// Check if the micro frame number differ from zero
	if (USBHS->USBHS_HSTFNUM & USBHS_HSTFNUM_MFNUM_Msk)
	{
		return;
	}
}


//--------------------------------------------------------------------------------------------------//


// Allocation pipe
// Starts when ALLOC pipe config x is one
//

void usb_host_defaut_pipe_enable(void)
{
	// Reset pipe zero
	USBHS->USBHS_HSTPIP |= (1 << USBHS_HSTPIP_PRST0_Pos);
	USBHS->USBHS_HSTPIP &= ~(1 << USBHS_HSTPIP_PRST0_Pos);
	
	// Enable pipe zero
	USBHS->USBHS_HSTPIP |= (1 << USBHS_HSTPIP_PEN0_Pos);
	
	// Configure the pipe
	USBHS->USBHS_HSTPIPCFG[0] = (USBHS_HSTPIPCFG_PSIZE_64_BYTE) | (1 << USBHS_HSTPIPCFG_ALLOC_Pos);
	
	// Test if the pipe is configured correctly
	while (!(USBHS->USBHS_HSTPIPISR[0] & (1 << USBHS_HSTPIPISR_CFGOK_Pos)))
	{
		
	}
	board_serial_print("Pipe zero configure success\n");
}


//--------------------------------------------------------------------------------------------------//


void usb_host_get_device_descriptor(void)
{
	usb_host_setup_packet setup_packet;
	
	setup_packet.request_type = (USB_SETUP_PACKET_RQ_TYPE_DEVCE_TO_HOST | USB_SETUP_PACKET_RQ_TYPE_STANDARD | USB_SETUP_PACKET_RQ_TYPE_DEVICE);
	setup_packet.request = USB_REQUEST_CODE_GET_DESCRIPTOR;
	setup_packet.value = USB_DECRIPTOR_TYPE_DEVICE;
	setup_packet.windex = 0;									// Only for language ID
	setup_packet.length = 18;
	
	// The setup packet is ready to be sent
	uint8_t* source = (uint8_t*)(&setup_packet);
	uint8_t* dest = (uint8_t*)USB_FIFO_BUFFER_START_ADDRESS;
	
	// Configure token and interrupt for pipe 0
	
	
	// Copy the frame to the FIFO
	for (uint8_t i = 0; i < 8; i++)
	{
		*dest++ = *source++;
	}
	
	// Issue the transfer
	
}


//--------------------------------------------------------------------------------------------------//


void USBHS_Handler()
{
	board_serial_print("\n--------------------------- IRQ ---------------------------\n");
	
	// Read the interrupt status register
	// This will NOT clear interrupt bits, and this must be done manually
	uint32_t status = usbhs_host_get_interrupt_status_register();
	
	board_serial_print_register("Host interrupt register", status);
	
	// Clear all interrupt just for DEBUG purpose
	usbhs_host_interrupt_clear(0xffffffff);

	
	// Start of frame interrupt detected
	// Something
	if (status & USBHS_HSTISR_HSOFI_Msk)
	{
		usb_host_start_of_frame_handler(status);
	}
	
	// Pipe interrupt detected
	// Something
	if (status & (0b1111111111 << USBHS_HSTISR_PEP_0_Pos))
	{
		usb_host_pipe_handler(status);
	}
	
	// DMA interrupt
	// Currently not supported
	
	//usb_host_dma_handler(status);
	
	// General interrupts
	// Reset, wakeup, connection, disconnection
	// Something
	if (status & (USBHS_HSTISR_RSTI_Msk | USBHS_HSTISR_HWUPI_Msk | USBHS_HSTISR_DCONNI_Msk | USBHS_HSTISR_DDISCI_Msk | USBHS_HSTISR_RSMEDI_Msk | USBHS_HSTISR_RXRSMI_Msk))
	{
		usb_host_root_hub_handler(status);
	}

	board_serial_print("-----------------------------------------------------------\n\n\n");
	
	if (status & USBHS_HSTISR_HSOFI_Msk)
	{
		systick_delay_milliseconds(800);
	}
}


//--------------------------------------------------------------------------------------------------//