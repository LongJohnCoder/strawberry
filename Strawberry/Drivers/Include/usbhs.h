// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef USBHS_H
#define USBHS_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	USB_DRIVER_FULL_SPEED = 0,
	USB_DRIVER_HIGH_SPEED = 1,
	USB_DRIVER_LOW_SPEED = 2
} usb_driver_speed_status_e;



typedef enum
{
	USB_DRIVER_HOST_SPEED_NORMAL				= 0,
	USB_DRIVER_HOST_SPEED_LOW_POWER				= 1,
	USB_DRIVER_HOST_SPEED_HIGH_SPEED			= 2,
	USB_DRIVER_HOST_SPEED_FORCED_FULL_SPEED		= 3
} usb_driver_host_speed_e;



typedef enum
{
	PIPE_TYPE_CONTROL		= 0,
	PIPE_TYPE_ISOCHRONOUS	= 1,
	PIPE_TYPE_BULK			= 2,
	PIPE_TYPE_INTERRUPT		= 3
} usb_driver_pipe_type_e;



typedef enum
{
	PIPE_TOKEN_SETUP	= 0,
	PIPE_TOKEN_IN		= 1,
	PIPE_TOKEN_OUT		= 2
} usb_driver_pipe_token_e;



typedef enum
{
	PIPE_SIZE_8		= 0,
	PIPE_SIZE_16	= 1,
	PIPE_SIZE_32	= 2,
	PIPE_SIZE_64	= 3,
	PIPE_SIZE_128	= 4,
	PIPE_SIZE_256	= 5,
	PIPE_SIZE_512	= 6,
	PIPE_SIZE_1024	= 7,
} usb_driver_pipe_size_e;



typedef enum
{
	USB_DRIVER_HOST		= 0,
	USB_DRIVER_DEVICE	= 1
} usb_driver_mode_e;


//--------------------------------------------------------------------------------------------------//


#define USB_PIPE_0	0
#define USB_PIPE_1	1
#define USB_PIPE_2	2
#define USB_PIPE_3	3
#define USB_PIPE_4	4
#define USB_PIPE_5	5
#define USB_PIPE_6	6
#define USB_PIPE_7	7
#define USB_PIPE_8	8
#define USB_PIPE_9	9


//--------------------------------------------------------------------------------------------------//


void usbhs_global_set_mode(usb_driver_mode_e usb_mode);

void usbhs_global_freeze_clock(void);

void usbhs_global_unfreeze_clock(void);

void usbhs_global_enable(void);

void usbhs_global_disable(void);

void usbhs_global_send_vbus_request(void);

void usbhs_global_vbus_hardware_enable(void);

void usbhs_global_vbus_hardware_disable(void);

usb_driver_speed_status_e usbhs_global_get_speed_status(void);

uint32_t usbhs_global_get_status_register(void);

uint8_t usbhs_global_clock_usable(void);


//--------------------------------------------------------------------------------------------------//


void usbhs_host_set_speed_capability(usb_driver_host_speed_e speed);

// This function should only be called when the host generates start of frames
void usbhs_host_send_resume(void);

void usbhs_host_send_reset(void);

void usbhs_host_send_reset_clear(void);

void usbhs_host_SOF_enable(void);

void usbhs_host_SOF_disable(void);


//--------------------------------------------------------------------------------------------------//


// The host interrupts is as follows
//
// DMA[6:0]						: interrupt on one of the DMA channels
// PEP[9:0]						: interrupt on one of the pipes
// Wakeup						: only generated if SOF = 0 and upstream resume or disconnection
// Start of frame				: set when a SOF is issued by the host
// Upstream resume received		: set when a upstream resume is received
// Downstream resume sent		: set when a downstream resume has been sent
// Reset sent					: set when a reset sequence has been sent to the device
// Device disconnection			: set when a device has been removed from the USB bus
// Device connection			: set when a device has been connected to the USB bus

uint32_t usbhs_host_get_interrupt_status_register(void);

uint32_t usbhs_host_get_interrupt_mask(void);

void usbhs_host_interrupt_clear(uint32_t mask);

void usbhs_host_interrupt_force(uint32_t mask);

void usbhs_host_interrupt_disable(uint32_t mask);

void usbhs_host_interrupt_enable(uint32_t mask);


//--------------------------------------------------------------------------------------------------//


uint16_t usbhs_host_get_frame_number(void);

uint8_t usbhs_host_get_microframe_number(void);


//--------------------------------------------------------------------------------------------------//


void usbhs_host_set_pipe_address(uint8_t pipe_number, uint8_t addr);

void usbhs_host_pipe_enable(uint8_t pipe_number);

void usbhs_host_pipe_disable(uint8_t pipe_number);

void usbhs_host_pipe_reset_on(uint8_t pipe_number);

void usbhs_host_pipe_reset_off(uint8_t pipe_number);


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_interrupt_frequency(uint8_t pipe_number, uint8_t interrupt_frequency);

void usbhs_host_pipe_set_endpoint_number(uint8_t pipe_number, uint8_t endpoint_number);

void usbhs_host_pipe_set_type(uint8_t pipe_number, usb_driver_pipe_type_e pipe_type);

void usbhs_host_pipe_set_token(uint8_t pipe_number, usb_driver_pipe_token_e token);

void usbhs_host_pipe_set_size(uint8_t pipe_number, usb_driver_pipe_size_e pipe_size);

void usbhs_host_pipe_auto_bankswitch_enable(uint8_t pipe_number);

void usbhs_host_pipe_auto_bankswitch_disable(uint8_t pipe_number);

void usbhs_host_pipe_allocate(uint8_t pipe_number);

void usbhs_host_pipe_free(uint8_t pipe_number);


//--------------------------------------------------------------------------------------------------//


// USB pipe status info are as follows
//
// PBYCT[10:0]				: Contains the number of bytes in the FIFO
// CFGOK					: Pipe configuration is OK
// RWALL					: Set when OUT pipe is not full or when IN pipe is not empty
// Current bank [1:0]		: Indicates (non CONTROL pipe) the current bank
// Busy banks [1:0]			: Indicates the number of busy banks
// DTSEQ [1:0]				: Indicates the DATA PID of the current bank
//
// USB pipe interrupts are as follows
//
// Short packet				: Set when a short package is received
// STALL					: Set when a STALL handshake is received
// Overflow					: Set when the current pipe has received more the the pipe max size
// NAKED					: Set when NAK has been received on the current bank
// Pipe error				: Set when errors occur on the current pipe bank
// Transmitted setup		: Set (CONTROL) when the current SETUP bank is free and can be filled
// Transmitted out			: Set when the current out bank is free an can be filled
// Received in				: Set when a new USB message is stored in the current bank
//
// For the interrupt mask / disable / enable registers
//
// RSTDT					: Resets the data toggle for the current pipe
// PIPEFREEZE				: When cleared the pipe requests begins
// FIFOCONTROL				: Controls the FIFO buffer

uint32_t usbhs_host_pipe_get_interrupt_status_register(uint8_t pipe_number);

uint32_t usbhs_host_pipe_get_interrupt_mask(uint8_t pipe_number);

void usbhs_host_pipe_interrupt_clear(uint8_t pipe_number, uint32_t mask);

void usbhs_host_pipe_interrupt_force(uint8_t pipe_number, uint32_t mask);

void usbhs_host_pipe_interrupt_enable(uint8_t pipe_number, uint32_t mask);

void usbhs_host_pipe_interrupt_disable(uint8_t pipe_number, uint32_t mask);


//--------------------------------------------------------------------------------------------------//


void usbhs_host_in_request_disable(uint8_t pipe_number);

void usbhs_host_in_request_enable(uint8_t pipe_number, uint8_t number_of_in_requests);


//--------------------------------------------------------------------------------------------------//


#endif