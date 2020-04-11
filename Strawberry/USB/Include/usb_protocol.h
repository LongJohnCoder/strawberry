// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


// This file contain all USB 2.0 protocol descriptors and descriptor defines

// PIDS: OUT, IN, SOF, SETUP, DATA0, DATA1, DATA2, MDATA, ACK, NAK, STALL, NYET, PRE, ERR, SPLIT, PING, EXT

// USB descriptor type
// Identifies the descriptor type

typedef enum
{
	USB_DECRIPTOR_TYPE_DEVICE					= 0x01,
	USB_DECRIPTOR_TYPE_CONFIG					= 0x02,
	USB_DECRIPTOR_TYPE_STRING					= 0x03,
	USB_DECRIPTOR_TYPE_INTERFACE				= 0x04,
	USB_DECRIPTOR_TYPE_ENDPOINT					= 0x05,
	USB_DECRIPTOR_TYPE_DEVICE_QUALIFIER			= 0x06,
	USB_DECRIPTOR_TYPE_OTHER_SPEED_CONFIG		= 0x07,
	USB_DECRIPTOR_TYPE_INTERFACE_POWER			= 0x08,
	USB_DECRIPTOR_TYPE_OTG						= 0x09,
	USB_DECRIPTOR_TYPE_DEBUG					= 0x0a,
	USB_DECRIPTOR_TYPE_INTERFACE_ASSOCIATION	= 0x0b,
	USB_DECRIPTOR_TYPE_SECURITY					= 0x0c,
	USB_DECRIPTOR_TYPE_KEY						= 0x0d,
	USB_DECRIPTOR_TYPE_ENCRYPTION_TYPE			= 0x0e,
	USB_DECRIPTOR_TYPE_BOS						= 0x0f,
	USB_DECRIPTOR_TYPE_DEVICE_CAPABILITY		= 0x10
} usb_descriptor_type;



typedef enum
{
	USB_REQUEST_CODE_GET_STATUS = 0,
	USB_REQUEST_CODE_CLEAR_FEATURE = 1,
	USB_REQUEST_CODE_SET_FEATURE = 3,
	USB_REQUEST_CODE_SET_ADDRESS = 5,
	USB_REQUEST_CODE_GET_DESCRIPTOR = 6,
	USB_REQUEST_CODE_SET_DESCRIPTOR = 7,
	USB_REQUEST_CODE_GET_CONFIGURATION = 8,
	USB_REQUEST_CODE_SET_CONFIGURATION = 9,
	USB_REQUEST_CODE_GET_INTERFACE = 10,
	USB_REQUEST_CODE_SET_INTERFACE = 11,
	USB_REQUEST_CODE_SYNC_FRAME = 12	
} usb_request_code;



// USB device class type
typedef enum
{
	USB_DEV_CLASS_COMMUNICATION			= 0x02,
	USB_DEV_CLASS_HUB					= 0x09,
	USB_DEV_CLASS_PERSONAL_HEALTHCARE	= 0x0f,
	USB_DEV_CLASS_DIAGNOSTIC_DEVICE		= 0xdc,
	USB_DEV_CLASS_WIRELESS_CONTROLLER	= 0xe0,
	USB_DEV_CLASS_MISCELLANEOUS			= 0xef,
	USB_DEV_CLASS_VENDOR_SPECIFIC		= 0xff
} usb_device_class_type;


//--------------------------------------------------------------------------------------------------//


// USB device descriptor
// This identifies the product and its manufacturer and specifies size of endpoint zero
typedef struct usb_device_descriptor_s
{
	uint8_t		length;
	uint8_t		descriptor_type;
	uint16_t	usb_spesification_version;
	uint8_t		device_class;
	uint8_t		device_subclass;
	uint8_t		device_protocol;
	uint8_t		max_packet_size_endpoint_zero;
	uint16_t	vendor_id;
	uint16_t	product_id;
	uint16_t	device_release_number;
	uint8_t		manufacturer;
	uint8_t		product;
	uint8_t		serial_number;
	uint8_t		number_of_configurations;
} usb_device_descriptor;



// USB device qualifier descriptor
// Devices that supports both full and high speed have this.
// Depending on speed config some fields in device and device qualifier may swap
typedef struct usb_device_qualifier_descriptor_s
{
	uint8_t		length;
	uint8_t		descriptor_type;
	uint16_t	usb_spesification_version;
	uint8_t		device_class;
	uint8_t		device_subclass;
	uint8_t		device_protocol;
	uint8_t		max_packet_size_endpoint_zero;
	uint8_t		number_of_configurations;
	uint8_t		reserved;
} usb_device_qualifier_descriptor;



typedef struct
{
	uint8_t				request_type;
	usb_request_code	request;
	uint16_t			value;
	uint16_t			windex;
	uint16_t			length;
} usb_host_setup_packet;


//--------------------------------------------------------------------------------------------------//


#define USB_SETUP_PACKET_RQ_TYPE_HOST_TO_DEVICE		(0 << 7)
#define USB_SETUP_PACKET_RQ_TYPE_DEVCE_TO_HOST		(1 << 7)


#define USB_SETUP_PACKET_RQ_TYPE_STANDARD			(0 << 5)
#define USB_SETUP_PACKET_RQ_TYPE_CLASS				(1 << 5)
#define USB_SETUP_PACKET_RQ_TYPE_VENDOR				(2 << 5)


#define USB_SETUP_PACKET_RQ_TYPE_DEVICE				(0 << 0)
#define USB_SETUP_PACKET_RQ_TYPE_INTERFACE			(1 << 0)
#define USB_SETUP_PACKET_RQ_TYPE_ENDPOINT			(2 << 0)


//--------------------------------------------------------------------------------------------------//


#endif