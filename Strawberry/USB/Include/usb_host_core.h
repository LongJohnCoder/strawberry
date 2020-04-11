// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef USB_HOST_CORE_H
#define USB_HOST_CORE_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define USB_FIFO_BUFFER_START_ADDRESS 0xA0100000


//--------------------------------------------------------------------------------------------------//


// Forward declaration
struct usb_host_pipe_s;
struct usb_host_control_trasfer_s;


//--------------------------------------------------------------------------------------------------//


typedef void (*usb_host_pipe_callback)(struct usb_host_pipe_s me);


//--------------------------------------------------------------------------------------------------//


typedef struct usb_host_pipe_s
{
	// Configure the pipe owner
	void* owner;
	
	// Callback when a transfer is done
	usb_host_pipe_callback trasfer_done;
	
	
	// Info of the pipe
	uint16_t		max_packet_size;
	uint8_t			device_address;
	uint8_t			endpoint_address;
	uint8_t			trasfer_type;
	uint8_t			data_toogle_state;
	
	union
	{
		
	} transfer;
} usb_host_pipe;


//--------------------------------------------------------------------------------------------------//


typedef struct usb_host_control_trasfer_s
{
	uint8_t*	data;
	uint8_t*	setup;
	uint16_t	size;
	uint16_t	count;
	
	int16_t		request_timeout;
	int16_t		packet_timeout;
	uint16_t	packet_size;
	
	uint8_t		state;
	int8_t		status;
	
} usb_host_control_trasfer;


//--------------------------------------------------------------------------------------------------//


// We allocate 4 pipes
// 3 transfer pipes + 1 control pipe (pipe zero)
usb_host_pipe pipe[4];


//--------------------------------------------------------------------------------------------------//


void usb_host_init(void);


//--------------------------------------------------------------------------------------------------//


#endif