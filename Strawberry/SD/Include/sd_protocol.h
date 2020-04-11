// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SD_PROTOCOL_H
#define SD_PROTOCOL_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "hsmci.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	SDSC,
	SDHC,
	SDXC
} sd_protocol_card_type_e;



typedef enum
{
	SD_PROTOCOL_OK,
	SD_PROTOCOL_ERROR
} sd_protocol_status_e;


//--------------------------------------------------------------------------------------------------//


// The CSD structure depends on the Physical Specification Version and on the SD card capacity
// The CSD register version 1.0 applies to all standard capacity memory cards

typedef struct  
{
	uint8_t		csd_structure;
	uint8_t		taac;
	uint8_t		nsac;
	uint8_t		transfer_speed;
	uint16_t	ccc;
	uint8_t		read_block_length;
	uint8_t		read_block_partial;
	uint8_t		write_block_misaligned;
	uint8_t		read_block_misaligned;
	uint8_t		dsr_implemented;
	uint16_t	c_size;
	uint8_t		vdd_read_current_min;
	uint8_t		vdd_read_current_max;
	uint8_t		vdd_write_current_min;
	uint8_t		vdd_write_current_max;
	uint8_t		c_size_multipliation;
	uint8_t		erase_block_enable;
	uint8_t		sector_size;
	uint8_t		write_protection_group_size;
	uint8_t		write_protection_group_enable;
	uint8_t		write_speed_factor;
	uint8_t		write_block_length;
	uint8_t		write_block_partial;
	uint8_t		file_format_group;
	uint8_t		copy_flag;
	uint8_t		permanent_write_protection;
	uint8_t		temporarily_write_protection;
	uint8_t		file_format;
	
} sd_protocol_csd_1_0;


//--------------------------------------------------------------------------------------------------//


// The CSD register version 2.0 applies to all high and extended capacity memory cards

typedef struct 
{
	uint8_t		csd_structure;
	uint8_t		taac;
	uint8_t		nsac;
	uint8_t		transfer_speed;
	uint16_t	ccc;
	uint8_t		read_block_length;
	uint8_t		read_block_partial;
	uint8_t		write_block_misaligned;
	uint8_t		read_block_misaligned;
	uint8_t		dsr_implemented;
	uint32_t	c_size;
	uint8_t		erase_block_enable;
	uint8_t		sector_size;
	uint8_t		write_protection_group_size;
	uint8_t		write_protection_group_enable;
	uint8_t		write_speed_factor;
	uint8_t		write_block_length;
	uint8_t		write_block_partial;
	uint8_t		file_format_group;
	uint8_t		copy_flag;
	uint8_t		permanent_write_protection;
	uint8_t		temporarily_write_protection;
	uint8_t		file_format;
	
} sd_protocol_csd_2_0;


//--------------------------------------------------------------------------------------------------//


// The byte field for application ID and name are one byte longer
// than the actual data, because of the \0 character
typedef struct  
{
	uint8_t		manifacturer_id;
	char		application_id[3];
	char		name[6];
	uint8_t		revision;
	uint32_t	serial_number;	
	uint16_t	manifacturer_date;
	
} sd_protocol_cid;


//--------------------------------------------------------------------------------------------------//


// This is the struct that contains all information of the SD card
// It uses union to save RAM space
typedef struct  
{
	uint8_t card_initialized;
	
	sd_protocol_cid card_identification;
	
	union
	{
		sd_protocol_csd_1_0 card_specific_data_1_0;
		sd_protocol_csd_2_0 card_specific_data_2_0;
	};
	
	
	sd_protocol_card_type_e	card_type;
	
	
	uint16_t				relative_card_address;
	uint32_t				card_size;
	
	
	uint8_t					four_bit_bus_width_support;
	uint8_t					version_1_10_and_later;
	uint8_t					high_speed_support;
	uint32_t				bus_speed;
	uint32_t				number_of_blocks;
	
	hsmci_sd_slot_select_e	slot;
	
} sd_card;


//--------------------------------------------------------------------------------------------------//


// R1
// 48-bit response
#define SD_PROTOCOL_RESPONSE_1		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_48_BIT

// R1b
// same as R1 but with busy on the data line
#define SD_PROTOCOL_RESPONSE_1b		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_R1B

// R2 CID and CSD registers
// 136-bit response
// [127:1] CID or CSD including CRC
#define SD_PROTOCOL_RESPONSE_2		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_136_BIT

// R3 OCR register
// 48-bit response
// [39:8] OCR register
// NO CRC
#define SD_PROTOCOL_RESPONSE_3		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_48_BIT

// R6 RCA response
// 48-bit
// [15:0] card status bits 23, 22, 19, 12:0
// [31:16] new published RCA
#define SD_PROTOCOL_RESPONSE_6		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_48_BIT

// R7 card interface condition
// 48-bit
// [19:16] supported voltage range typical 0b0001 2.7-3.6V
// [15:8] echo check pattern
#define SD_PROTOCOL_RESPONSE_7		HSMCI_CMDR_MAXLAT_64 | HSMCI_CMDR_RSPTYP_48_BIT


//--------------------------------------------------------------------------------------------------//


// These define the host supply voltage
#define SD_PROTOCOL_VOLTAGE_SUPPLIED		0b1
#define SD_PROTOCOL_VOLTAGE_CHECK_PATTERN	0b10101010
#define SD_PROTOCOL_VOLTAGE_ARGUMENT		((SD_PROTOCOL_VOLTAGE_SUPPLIED << 8) | SD_PROTOCOL_VOLTAGE_CHECK_PATTERN)


//--------------------------------------------------------------------------------------------------//


#define SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_WRITE	(HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_WRITE)
#define SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ	(HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ)


//--------------------------------------------------------------------------------------------------//


// These define the switch functions used in CMD6
#define SD_PROTOCOL_ACCESS_MODE_SDR_12		(0x0 << 0)
#define SD_PROTOCOL_ACCESS_MODE_SDR_25		(0x1 << 0)
#define SD_PROTOCOL_ACCESS_MODE_SDR_50		(0x2 << 0)
#define SD_PROTOCOL_ACCESS_MODE_SDR_104		(0x3 << 0)
#define SD_PROTOCOL_ACCESS_MODE_DDR_50		(0x4 << 0)

#define SD_PROTOCOL_COMMAND_SYSTEM_DEFAULT	(0xf << 4)
#define SD_PROTOCOL_COMMAND_SYSTEM_FOR_EC	(0x1 << 4)
#define SD_PROTOCOL_COMMAND_SYSTEM_OTP		(0x3 << 4)
#define SD_PROTOCOL_COMMAND_SYSTEM_ASSD		(0x4 << 4)

#define SD_PROTOCOL_DRIVER_STRENGTH_TYPE_B	(0xf << 8)
#define SD_PROTOCOL_DRIVER_STRENGTH_TYPE_A	(0x1 << 8)
#define SD_PROTOCOL_DRIVER_STRENGTH_TYPE_C	(0x2 << 8)
#define SD_PROTOCOL_DRIVER_STRENGTH_TYPE_D	(0x3 << 8)

#define SD_PROTOCOL_POWER_LIMIT_072_W		(0xf << 12)
#define SD_PROTOCOL_POWER_LIMIT_144_W		(0x1 << 12)
#define SD_PROTOCOL_POWER_LIMIT_216_W		(0x2 << 12)
#define SD_PROTOCOL_POWER_LIMIT_288_W		(0x3 << 12)
#define SD_PROTOCOL_POWER_LIMIT_180_W		(0x4 << 12)

#define SD_PROTOCOL_SWITCH					(0 << 31)
#define SD_PROTOCOL_CHECK					(0 << 31)

#define SD_PROTOCOL_RESPONSE_1_ERROR_MASK (0b1111111111111 << 19)


//--------------------------------------------------------------------------------------------------//


// CMD0 GO_IDLE_STATE
// This command brings the SD card to the IDLE state
//
// Argument:	[31:0] stuff bits
// Response:	none

uint8_t sd_protocol_send_cmd_0(void);


// CMD2 ALL_SEND_CID
// Ask the card to send their CID numbers on the CMD line
//
// Argument:	[31:0] stuff bits
// Response:	R2

uint8_t sd_protocol_send_cmd_2(sd_card* card);


// CMD3 SEND_RELATIVE_ADDRESS
// Asks the card to send new relative address (RCA)

// Argument:	[31:0] stuff bits
// Response:	R6
uint8_t sd_protocol_send_cmd_3(sd_card* card);


// CMD8 SEND_IF_COND
// Send interface condition, including host supply voltage, and check if the card supports it

// Argument:	[11:8] host voltage
//				[7:0] check pattern recommended 0b10101010
// Response:	R7
uint8_t sd_protocol_send_cmd_8(void);


// CMD9 SEND_CSD
// Addressed card sends card specific data (CSD)

// Argument:	[31:16] relative card address (RCA)
// Response:	R2
uint8_t sd_protocol_send_cmd_9(sd_card* card);


// CMD55 APP_CMD
// Indicated that the next command is an application command

// Argument:	[31:16] relative card address (RCA)
// Response:	R1
uint8_t sd_protocol_send_cmd_55(const sd_card* card);


// ACMD41 SD_SEND_OP_COND
// Send host capacity support to SD card, and asks for the operating condition (OCR)

// Argument:	[31] reserved set to 0
//				[30] host high capacity support
// Response:	R3
uint8_t sd_protocol_send_acmd_41(sd_card* card);


// CMD7 SELECT / DESELECT CARD
// Toggles the card between stand-by state and transfer state

// Argument:	[31:16] RCA WARNING: address zero de-select all
// Response:	R1b
uint8_t sd_protocol_send_cmd_7(const sd_card* card);


// ACMD6 SET_BUS_WIDTH
// Defines the bus width, 00 = 1bit and 10 = 4bit

// Argument:	[31:2] stuff bits
//				[1:0] bus width
// Response:	R1
uint8_t sd_protocol_send_acmd_6(const sd_card* card);


// CMD16 SET_BLOCKLEN
// Sets block length for all following read / write / lock commands
// WARNING: this is only allowed if partial block read is allowed in CSD
// WARNING: this command is only possible for LC SD cards

// Argument:	[31:0] block length
// Response:	R1
uint8_t sd_protocol_send_cmd_16(void);


// ACMD51 SEND_SCR
// Sends the SD configuration register

// Argument:	[31:0] stuffed bits
// Response:	R1
uint8_t sd_protocol_send_acmd_51(sd_card* card);


// CMD6 SWITCH_FUNC
// Switches the card functions

// Argument:	[31] 1
//				[30:16] zero
//				[15:12] power limit
//				[11:8] drive strength
//				[7:4] command system
//				[3:0] access mode
// Response:	R1
uint8_t sd_protocol_send_cmd_6(sd_card* card);


// CMD6 SWITCH_FUNC
// Checks switchable functions

// Argument:	[31] 0
//				[30:16] zero
//				[15:12] power limit
//				[11:8] drive strength
//				[7:4] command system
//				[3:0] access mode
// Response:	R1
uint8_t sd_protocol_send_cmd_6_check(sd_card* card);


// CMD13 SEND_STATUS
// Addressed card sends its status register

// Argument:	[31:16] RCA
//				[15] send task status register
// Result:		R1
uint8_t sd_protocol_send_cmd_13(sd_card* card);


// send 74 dummy cycles according to the SD Specification due to VDD ramp-up delay
uint8_t sd_protocol_boot(void);


//--------------------------------------------------------------------------------------------------//


sd_protocol_csd_1_0 sd_protocol_csd_decode_version_1_0(uint8_t* raw_data);

sd_protocol_csd_2_0 sd_protocol_csd_decode_version_2_0(uint8_t* raw_data);

sd_protocol_cid sd_protocol_cid_decode(uint8_t* raw_rata);


//--------------------------------------------------------------------------------------------------//


// just a temporarily function used for debug
void sd_protocol_print_reg(char* description, uint32_t data, uint8_t number_of_bits);

void sd_protocol_print_csd(const sd_card* card);

void sd_protocol_print_card_info(const sd_card* card);


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_initialize(sd_card* card);

uint8_t sd_protocol_read(sd_card* card, uint8_t *data, uint32_t sector, uint32_t count);

uint8_t sd_protocol_dma_read(sd_card* card, uint8_t *data, uint32_t sector, uint32_t count);

uint8_t sd_protocol_write(sd_card* card, const uint8_t *data, uint32_t sector, uint32_t count);


//--------------------------------------------------------------------------------------------------//


#endif