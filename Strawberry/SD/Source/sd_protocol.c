// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "sd_protocol.h"
#include "board_sd_card.h"
#include "board_serial.h"
#include "dma.h"


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_boot(void)
{
	if (hsmci_send_command(HSMCI, HSMCI_CMDR_RSPTYP_NORESP | HSMCI_CMDR_SPCMD_INIT | HSMCI_CMDR_OPDCMD_OPENDRAIN, 0, CHECK_CRC) == HSMCI_OK)
	{
		return 1;
	}
	else
	{
		board_serial_print("[  FAIL ]\tBoot trigger failed\n");
		return 0;
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_0(void)
{
	if (hsmci_send_command(HSMCI, HSMCI_CMDR_OPDCMD_OPENDRAIN, 0, CHECK_CRC) == HSMCI_OK)
	{
		return 1;
	}
	else
	{
		board_serial_print("[  FAIL ]\tSending go to IDLE command CMD0\n");
		return 0;
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_8(void)
{	
	if (hsmci_send_command(HSMCI, 8 | SD_PROTOCOL_RESPONSE_7 | HSMCI_CMDR_OPDCMD_OPENDRAIN, SD_PROTOCOL_VOLTAGE_ARGUMENT, CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD8\n");
		return 0;
	}

	uint32_t tmp = hsmci_read_48_bit_response_register(HSMCI);
	
	if ((tmp & SD_PROTOCOL_VOLTAGE_ARGUMENT) == SD_PROTOCOL_VOLTAGE_ARGUMENT)
	{
		return 1;
	}
	else
	{
		board_serial_print("[  FAIL ]\tCMD8 with ");
		board_serial_print_register("response: ", tmp);
		return 0;
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_55(const sd_card* card)
{
	if(hsmci_send_command(HSMCI, 55 | SD_PROTOCOL_RESPONSE_1, (uint32_t)card->relative_card_address << 16, CHECK_CRC) == HSMCI_ERROR)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_2(sd_card* card)
{
	if (hsmci_send_command(HSMCI, 2 | SD_PROTOCOL_RESPONSE_2 | HSMCI_CMDR_OPDCMD_OPENDRAIN, 0, CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD2\n");
		return 0;
	}
	
	uint8_t cid_raw[16];
	
	hsmci_read_136_bit_response_register_extended(HSMCI, cid_raw);
	
	card->card_identification = sd_protocol_cid_decode(cid_raw);
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_acmd_41(sd_card* card)
{	
	uint32_t retry_count = 0;
	uint32_t tmp;
	
	while (1)
	{		
		sd_protocol_send_cmd_55(card);
		
		// the voltage argument is mandatory in the SD 2.0 specification
		hsmci_send_command(HSMCI, 41 | SD_PROTOCOL_RESPONSE_3 | HSMCI_CMDR_OPDCMD_OPENDRAIN, (1 << 30) | (0b111111 << 15), DONT_CHECK_CRC);
			
		tmp = hsmci_read_48_bit_response_register(HSMCI);
			
		// checks if power up is done
		if (tmp & (1 << 31))
		{
			if (tmp & (1 << 30))
			{
				card->card_type = SDHC;
				return 1;
			}
			card->card_type = SDSC;
			return 1;
		}
			
		if (retry_count++ >= 1000)
		{
			board_serial_print_register("[  FAIL ]\tACMD41 response: ", tmp);
			return 0;
		}
	}	
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_3(sd_card* card)
{
	if (hsmci_send_command(HSMCI, 3 | SD_PROTOCOL_RESPONSE_6 | HSMCI_CMDR_OPDCMD_OPENDRAIN, 0, CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD3\n");
		return 0;
	}
	
	card->relative_card_address = ((hsmci_read_48_bit_response_register(HSMCI) >> 16) & 0xffff);
		
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_9(sd_card* card)
{
	if (hsmci_send_command(HSMCI, 9 | SD_PROTOCOL_RESPONSE_2, (uint32_t)(card->relative_card_address << 16), CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD9\n");
		return 0;
	}
	
	uint8_t csd_raw[16];
	hsmci_read_136_bit_response_register_extended(HSMCI, csd_raw);
	
	if (card->card_type == SDHC)
	{
		card->card_specific_data_2_0 = sd_protocol_csd_decode_version_2_0(csd_raw);
		card->card_size = (uint32_t)(card->card_specific_data_2_0.c_size*512);
		
		// Update the number_of_blocks field too
		card->number_of_blocks = card->card_specific_data_2_0.c_size * 1000;
	}
	else if (card->card_type == SDSC)
	{
		card->card_specific_data_1_0 = sd_protocol_csd_decode_version_1_0(csd_raw);
		card->card_size =	((card->card_specific_data_1_0.c_size + 1) *
								(1 << (card->card_specific_data_1_0.c_size_multipliation + 2)) *
								(1 << card->card_specific_data_1_0.read_block_length)) / 1000;
								
		// Update the number_of_blocks field too
		card->number_of_blocks = (card->card_specific_data_1_0.c_size + 1) *
									(1 << (card->card_specific_data_1_0.c_size_multipliation + 2));
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_7(const sd_card* card)
{
	if (hsmci_send_command(HSMCI, 7 | SD_PROTOCOL_RESPONSE_1b, (uint32_t)card->relative_card_address << 16, CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD7\n");
		return 0;
	}
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_acmd_6(const sd_card* card)
{
	if (sd_protocol_send_cmd_55(card) == 0)
	{
		return 0;
	}
	
	if (hsmci_send_command(HSMCI, 6 | SD_PROTOCOL_RESPONSE_1, 0b10, CHECK_CRC) == HSMCI_ERROR)
	{
		return 0;
	}
	uint32_t status = hsmci_read_48_bit_response_register(HSMCI);
	if (status & (0b1111111111111 << 19))
	{
		sd_protocol_print_reg("ACMD6 status : ", status, 32);
		// the response contains an error
		return 0;
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_acmd_16(void)
{
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_acmd_51(sd_card* card)
{
	uint8_t scr[8];
	
	if (sd_protocol_send_cmd_55(card) == 0)
	{
		return 0;
	}
	if (hsmci_send_addressed_data_transfer_command(HSMCI, 51 | SD_PROTOCOL_RESPONSE_1 | 
													SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ | HSMCI_CMDR_TRTYP_BYTE,
													0, 8, 1, 0, CHECK_CRC) == HSMCI_ERROR)
	{
		board_serial_print("[  FAIL ]\tCMD51 error\n");
		return 0;
	}
	
	hsmci_read_data_register_reverse(HSMCI, scr, 2);
	
	if (((scr[6] >> 2) & 0b1) == 0b1)
	{
		card->four_bit_bus_width_support = 1;
	}
	else
	{
		card->four_bit_bus_width_support = 0;
	}
	
	uint8_t version = (scr[7] & 0b1111);
	if (version > 0)
	{
		
		card->version_1_10_and_later = 1;
	}
	else
	{
		card->version_1_10_and_later = 0;
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_6(sd_card* card)
{	
	card->high_speed_support = 0;
	card->bus_speed = 25000000;
	
	// the command receive 64 bytes of data
	uint8_t data_buffer[64];
	
	for (uint8_t i = 0; i < 64; i++)
	{
		data_buffer[i] = 0;
	}
	
	if (hsmci_send_addressed_data_transfer_command(	HSMCI, 6 | SD_PROTOCOL_RESPONSE_1 | HSMCI_CMDR_TRTYP_SINGLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ,
													SD_PROTOCOL_ACCESS_MODE_SDR_12 | SD_PROTOCOL_COMMAND_SYSTEM_DEFAULT | SD_PROTOCOL_DRIVER_STRENGTH_TYPE_B | SD_PROTOCOL_POWER_LIMIT_072_W | SD_PROTOCOL_SWITCH, 
													64, 1, 0, CHECK_CRC) == HSMCI_ERROR)
	{
		return 0;
	}
	
	hsmci_read_data_register_reverse(HSMCI, data_buffer, 16);

	uint32_t resp = hsmci_read_48_bit_response_register(HSMCI);
	
	// function group 1 responses
	// uint16_t function_group_1_support = data_buffer[50] | (data_buffer[51] << 8);
	uint8_t function_group_1_function_status = (data_buffer[47] & 0b1111);	
	uint16_t function_group_1_busy_status = (data_buffer[35] << 8) | data_buffer[34];
	
	if (function_group_1_function_status == 0xf)
	{
		board_serial_print("[WARNING]\tHigh speed error\n");
		return 1;
	}
	if (function_group_1_busy_status)
	{
		board_serial_print("[  FAIL ]\tCard is busy during CMD6\n");
		return 0;
	}	
	
	if ((resp & SD_PROTOCOL_RESPONSE_1_ERROR_MASK))
	{
		// error has occurred
		sd_protocol_print_reg("Response R1: ", resp, 32);
		return 0;
	}
	
	card->high_speed_support = 1;
	card->bus_speed = 50000000;

	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_6_check(sd_card* card)
{
	// the command receive 64 bytes of data
	uint8_t data_buffer[64];
	
	for (uint8_t i = 0; i < 64; i++)
	{
		data_buffer[i] = 0;
	}
	
	if (hsmci_send_addressed_data_transfer_command(	HSMCI, 6 | SD_PROTOCOL_RESPONSE_1 | HSMCI_CMDR_TRTYP_SINGLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ,
													SD_PROTOCOL_ACCESS_MODE_SDR_12 | SD_PROTOCOL_COMMAND_SYSTEM_DEFAULT | SD_PROTOCOL_DRIVER_STRENGTH_TYPE_B | SD_PROTOCOL_POWER_LIMIT_072_W | SD_PROTOCOL_CHECK,
													64, 1, 0, CHECK_CRC) == HSMCI_ERROR)
	{
		return 0;
	}
	
	hsmci_read_data_register_reverse(HSMCI, data_buffer, 16);

	uint32_t resp = hsmci_read_48_bit_response_register(HSMCI);
	
	// function group 1 responses
	uint16_t function_group_1_support = data_buffer[50] | (data_buffer[51] << 8);
	uint8_t function_group_1_function_status = (data_buffer[47] & 0b1111);
	uint16_t function_group_1_busy_status = (data_buffer[35] << 8) | data_buffer[34];
	
	sd_protocol_print_reg("Support: ", function_group_1_support, 16);
	sd_protocol_print_reg("Status: ", function_group_1_function_status, 8);
	sd_protocol_print_reg("Busy: ", function_group_1_busy_status, 16);
	
	if ((resp & SD_PROTOCOL_RESPONSE_1_ERROR_MASK))
	{
		// error has occurred
		sd_protocol_print_reg("Response R1: ", resp, 32);
		return 0;
	}
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_send_cmd_13(sd_card* card)
{
	uint32_t timeout_count = 200000;
	uint32_t status;
	
	do 
	{
		if (hsmci_send_command(HSMCI, 13 | SD_PROTOCOL_RESPONSE_1, (uint32_t)(card->relative_card_address << 16), CHECK_CRC) == HSMCI_ERROR)
		{
			return 0;
		}
		status = hsmci_read_48_bit_response_register(HSMCI);
		
		if (timeout_count -- <= 1)
		{
			return 0;
		}
	} while (!(status & (1 << 8)));
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


void sd_protocol_print_reg(char* description, uint32_t data, uint8_t number_of_bits)
{
	board_serial_print(description);
	board_serial_print(" : 0b");
	
	// cant print more than 32-bits
	if (number_of_bits > 32)
	{
		return;
	}
	
	for (uint8_t i = 0; i < number_of_bits; i++)
	{
		if ((i % 8 == 0) && (i != 0))
		{
			board_serial_print(" ");
		}
		if ((data & (1 << (number_of_bits - 1 - i))) == 0)
		{
			board_serial_print("0");
		}
		else
		{
			board_serial_print("1");
		}
	}
	
	board_serial_print("\n");
}


//--------------------------------------------------------------------------------------------------//


sd_protocol_csd_1_0 sd_protocol_csd_decode_version_1_0(uint8_t* raw_data)
{
	sd_protocol_csd_1_0 csd;
	
	csd.csd_structure					= ((raw_data[15] >> 6) & 0b11);
	csd.taac							= raw_data[14];
	csd.nsac							= raw_data[13];
	csd.transfer_speed					= raw_data[12];
	csd.ccc								= (((raw_data[10] >> 4) & 0b1111) | (raw_data[11] << 4));
	csd.read_block_length				= (raw_data[10] & 0b1111);
	csd.read_block_partial				= ((raw_data[9] >> 7) & 0b1);
	csd.write_block_misaligned			= ((raw_data[9] >> 6) & 0b1);
	csd.read_block_misaligned			= ((raw_data[9] >> 5) & 0b1);
	csd.dsr_implemented					= ((raw_data[9] >> 4) & 0b1);
	csd.c_size							= ((raw_data[7] >> 6) & 0b11) | (raw_data[8] << 2) | ((raw_data[9] & 0b11) << 10);
	csd.vdd_read_current_min			= ((raw_data[7] >> 3) & 0b111);
	csd.vdd_read_current_max			= (raw_data[7] & 0b111);
	csd.vdd_write_current_min			= ((raw_data[6] >> 5) & 0b111);
	csd.vdd_write_current_max			= ((raw_data[6] >> 2) & 0b111);
	csd.c_size_multipliation			= (((raw_data[6] & 0b11) << 1) | ((raw_data[5] >> 7) & 0b1));
	csd.erase_block_enable				= ((raw_data[5] >> 6) & 0b1);
	csd.sector_size						= (((raw_data[4] >> 7) & 0b1) | ((raw_data[5] & 0b111111) << 1));
	csd.write_protection_group_size		= (raw_data[4] & 0b1111111);
	csd.write_protection_group_enable	= ((raw_data[3] >> 7) & 0b1);
	csd.write_speed_factor				= ((raw_data[3] >> 2) & 0b111);
	csd.write_block_length				= (((raw_data[2] >> 6) & 0b11) | ((raw_data[3] & 0b11) << 2));
	csd.write_block_partial				= ((raw_data[2] >> 5) & 0b1);
	csd.file_format_group				= ((raw_data[1] >> 7) & 0b1);
	csd.copy_flag						= ((raw_data[1] >> 6) & 0b1);
	csd.permanent_write_protection		= ((raw_data[1] >> 5) & 0b1);
	csd.temporarily_write_protection	= ((raw_data[1] >> 4) & 0b1);
	csd.file_format						= (raw_data[1] & 0b11);
	
	return csd;
}


//--------------------------------------------------------------------------------------------------//


sd_protocol_csd_2_0 sd_protocol_csd_decode_version_2_0(uint8_t* raw_data)
{
	sd_protocol_csd_2_0 csd;
	
	csd.csd_structure					= ((raw_data[15] >> 6) & 0b11);
	csd.taac							= raw_data[14];
	csd.nsac							= raw_data[13];
	csd.transfer_speed					= raw_data[12];
	csd.ccc								= (((raw_data[10] >> 4) & 0b1111) | (raw_data[11] << 4));
	csd.read_block_length				= (raw_data[10] & 0b1111);
	csd.read_block_partial				= ((raw_data[9] >> 7) & 0b1);
	csd.write_block_misaligned			= ((raw_data[9] >> 6) & 0b1);
	csd.read_block_misaligned			= ((raw_data[9] >> 5) & 0b1);
	csd.dsr_implemented					= ((raw_data[9] >> 4) & 0b1);
	csd.c_size							= (((uint32_t)raw_data[8] & 0x3F) << 16) | ((uint32_t)raw_data[7] << 8) | raw_data[6];
	
	csd.erase_block_enable				= ((raw_data[5] >> 6) & 0b1);
	csd.sector_size						= (((raw_data[4] >> 7) & 0b1) | ((raw_data[5] & 0b111111) << 1));
	csd.write_protection_group_size		= (raw_data[4] & 0b1111111);
	csd.write_protection_group_enable	= ((raw_data[3] >> 7) & 0b1);
	csd.write_speed_factor				= ((raw_data[3] >> 2) & 0b111);
	csd.write_block_length				= (((raw_data[2] >> 6) & 0b11) | ((raw_data[3] & 0b11) << 2));
	csd.write_block_partial				= ((raw_data[2] >> 5) & 0b1);
	csd.file_format_group				= ((raw_data[1] >> 7) & 0b1);
	csd.copy_flag						= ((raw_data[1] >> 6) & 0b1);
	csd.permanent_write_protection		= ((raw_data[1] >> 5) & 0b1);
	csd.temporarily_write_protection	= ((raw_data[1] >> 4) & 0b1);
	csd.file_format						= (raw_data[1] & 0b11);
		
	return csd;
}


//--------------------------------------------------------------------------------------------------//


void sd_protocol_print_csd(const sd_card* card)
{
	if (card->card_type == SDSC)
	{
		sd_protocol_csd_1_0 csd = card->card_specific_data_1_0;
		
		sd_protocol_print_reg("CSD structure", (uint32_t)csd.csd_structure, 2);
		sd_protocol_print_reg("TAAC", (uint32_t)csd.taac, 8);
		sd_protocol_print_reg("NSAC", (uint32_t)csd.nsac, 8);
		sd_protocol_print_reg("Transfer speed", (uint32_t)csd.transfer_speed, 8);
		sd_protocol_print_reg("CCC", (uint32_t)csd.ccc, 12);
		sd_protocol_print_reg("Read block length", (uint32_t)csd.read_block_length, 4);
		sd_protocol_print_reg("Read block partial", (uint32_t)csd.read_block_partial, 1);
		sd_protocol_print_reg("Write block misaligned", (uint32_t)csd.write_block_misaligned, 1);
		sd_protocol_print_reg("Read block misaligned", (uint32_t)csd.read_block_misaligned, 1);
		sd_protocol_print_reg("DSR implemented", (uint32_t)csd.dsr_implemented, 1);
		sd_protocol_print_reg("Card size", (uint32_t)csd.c_size, 12);
		
		sd_protocol_print_reg("VDD read current min", (uint32_t)csd.vdd_read_current_min, 3);
		sd_protocol_print_reg("VDD read current max", (uint32_t)csd.vdd_read_current_max, 3);
		sd_protocol_print_reg("VDD write current min", (uint32_t)csd.vdd_write_current_min, 3);
		sd_protocol_print_reg("VDD write current max", (uint32_t)csd.vdd_write_current_max, 3);
		
		sd_protocol_print_reg("Card size multiplication", (uint32_t)csd.c_size_multipliation, 3);
		
		sd_protocol_print_reg("Erase block enable", (uint32_t)csd.erase_block_enable, 1);
		sd_protocol_print_reg("Sector size", (uint32_t)csd.sector_size, 7);
		sd_protocol_print_reg("Write protection group size", (uint32_t)csd.write_protection_group_size, 7);
		sd_protocol_print_reg("Write protection group enable", (uint32_t)csd.write_protection_group_enable, 1);
		sd_protocol_print_reg("Write speed factor", (uint32_t)csd.write_speed_factor, 3);
		sd_protocol_print_reg("Write block length", (uint32_t)csd.write_block_length, 4);
		sd_protocol_print_reg("Write block partial", (uint32_t)csd.write_block_partial, 1);
		sd_protocol_print_reg("File format group", (uint32_t)csd.file_format_group, 1);
		sd_protocol_print_reg("Copy flag", (uint32_t)csd.copy_flag, 1);
		sd_protocol_print_reg("Permanent write protection", (uint32_t)csd.permanent_write_protection, 1);
		sd_protocol_print_reg("Temporarily write protection", (uint32_t)csd.temporarily_write_protection, 1);
		sd_protocol_print_reg("File format", (uint32_t)csd.file_format, 2);
	}
	else
	{
		sd_protocol_csd_2_0 csd = card->card_specific_data_2_0;
		
		sd_protocol_print_reg("CSD structure", (uint32_t)csd.csd_structure, 2);
		sd_protocol_print_reg("TAAC", (uint32_t)csd.taac, 8);
		sd_protocol_print_reg("NSAC", (uint32_t)csd.nsac, 8);
		sd_protocol_print_reg("Transfer speed", (uint32_t)csd.transfer_speed, 8);
		sd_protocol_print_reg("CCC", (uint32_t)csd.ccc, 12);
		sd_protocol_print_reg("Read block length", (uint32_t)csd.read_block_length, 4);
		sd_protocol_print_reg("Read block partial", (uint32_t)csd.read_block_partial, 1);
		sd_protocol_print_reg("Write block misaligned", (uint32_t)csd.write_block_misaligned, 1);
		sd_protocol_print_reg("Read block misaligned", (uint32_t)csd.read_block_misaligned, 1);
		sd_protocol_print_reg("DSR implemented", (uint32_t)csd.dsr_implemented, 1);
		sd_protocol_print_reg("Card size", (uint32_t)csd.c_size, 12);
		sd_protocol_print_reg("Erase block enable", (uint32_t)csd.erase_block_enable, 1);
		sd_protocol_print_reg("Sector size", (uint32_t)csd.sector_size, 7);
		sd_protocol_print_reg("Write protection group size", (uint32_t)csd.write_protection_group_size, 7);
		sd_protocol_print_reg("Write protection group enable", (uint32_t)csd.write_protection_group_enable, 1);
		sd_protocol_print_reg("Write speed factor", (uint32_t)csd.write_speed_factor, 3);
		sd_protocol_print_reg("Write block length", (uint32_t)csd.write_block_length, 4);
		sd_protocol_print_reg("Write block partial", (uint32_t)csd.write_block_partial, 1);
		sd_protocol_print_reg("File format group", (uint32_t)csd.file_format_group, 1);
		sd_protocol_print_reg("Copy flag", (uint32_t)csd.copy_flag, 1);
		sd_protocol_print_reg("Permanent write protection", (uint32_t)csd.permanent_write_protection, 1);
		sd_protocol_print_reg("Temporarily write protection", (uint32_t)csd.temporarily_write_protection, 1);
		sd_protocol_print_reg("File format", (uint32_t)csd.file_format, 2);
	}
}


//--------------------------------------------------------------------------------------------------//


void sd_protocol_print_card_info(const sd_card* card)
{
	if (card->card_initialized)
	{
		// Print card type
		board_serial_print("Card type: ");
		if (card->card_type == SDHC)
		{
			board_serial_print("SDHC\n");
		}
		else if (card->card_type == SDSC)
		{
			board_serial_print("SDSC\n");	
		}
		
		// Print product name
		board_serial_print("Card name: %s\n", card->card_identification.name);
		
		// Print card size
		board_serial_print("Card size: %dMbyte\n", card->card_size / 1000);
		
		// Print bus width
		if (card->four_bit_bus_width_support)
		{
			board_serial_print("Using 4-bit bus\n");
		}
		else
		{
			board_serial_print("Using 1-bit bus\n");
		}
		
		// Print high speed
		if (card->high_speed_support)
		{
			board_serial_print("Using high speed ");
		}
		else
		{
			board_serial_print("Using default speed ");
		}
		board_serial_print("@ %dMHz\n", card->bus_speed / 1000000);
	}
}


//--------------------------------------------------------------------------------------------------//


sd_protocol_cid sd_protocol_cid_decode(uint8_t* raw_rata)
{
	sd_protocol_cid cid;
	
	// store the card name
	uint8_t count = 0;
	for (uint8_t i = 12; i > 7; i--)
	{
		cid.name[count++] = (char)raw_rata[i];
	}
	cid.name[count] = '\0';
	
	// store the application id
	count = 0;
	for (uint8_t i = 14; i > 12; i--)
	{
		cid.application_id[count++] = (char)raw_rata[i];
	}
	cid.application_id[count] = '\0';
	
	cid.manifacturer_id = raw_rata[15];
	cid.revision = raw_rata[7];
	cid.serial_number = (raw_rata[3]) | (raw_rata[4] << 8) | (raw_rata[5] << 16) | (raw_rata[6] << 24);
	cid.manifacturer_date = (raw_rata[1]) | ((raw_rata[2] & 0b1111) << 8);
	
	return cid;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_initialize(sd_card* card)
{
	HSMCI->HSMCI_CR = (1 << HSMCI_CR_SWRST_Pos);
	
	for (volatile uint32_t i = 0; i < 5000000; i++)
	{
		asm volatile ("nop");	
	}
		
	// Since the local variable does not get initialized we have
	// to manually set the relative_card_address to zero.
	// Otherwise we will get an error on CMD55.
	card->relative_card_address = 0;
	card->slot = HSMCI_SLOT_A;
	card->card_initialized = 0;
	
	// disable write protection before writing to the HSMCI registers
	hsmci_write_protection_disable(HSMCI);
	
	// write control register
	hsmci_set_bus_width(HSMCI, HSMCI_SD_BUS_WIDTH_1_BIT, card->slot);
	
	// set the data and completion timeout to 2.000.000 cycles
	hsmci_set_data_timeout(HSMCI, HSMCI_TIMEOUT_MULTIPLIER_1048576, 3);
	hsmci_set_completion_timeout(HSMCI, HSMCI_TIMEOUT_MULTIPLIER_1048576, 3);
	
	// wire configuration register
	hsmci_write_configuration_register(HSMCI, 0, 0, 1, 1);
	
	//  write mode register
	hsmci_write_mode_register(HSMCI, 0, HSMCI_MR_PADDING_00, 0, 0, 0, 0xff, 0xff);
	
	// select bus speed
	hsmci_set_bus_speed(HSMCI, 400000, 150000000);
	
	// enable the interface
	hsmci_enable(HSMCI);
	
	// send 74 clock cycles
	if (sd_protocol_boot() == 0)
	{
		return 0;
	}
	
	// get the card into idle state
	if (sd_protocol_send_cmd_0() == 0)
	{
		return 0;
	}
	
	// send command 8 to verify operating conditions
	if (sd_protocol_send_cmd_8() == 0)
	{
		return 0;
	}
	
	// try to read operating condition and return when the card do not return busy
	if (sd_protocol_send_acmd_41(card) == 0)
	{
		return 0;
	}
	
	// maybe add CMD11 in the future	
	
	// retrieve CID info
	if (sd_protocol_send_cmd_2(card) == 0)
	{
		return 0;
	}
	
	//get the relative address	
	if (sd_protocol_send_cmd_3(card) == 0)
	{
		return 0;
	}
	
	// now the SD card is in standby state
	
	// get card specific data
	if (sd_protocol_send_cmd_9(card) == 0)
	{
		return 0;
	}
	
	// get the card into transfer mode
	if (sd_protocol_send_cmd_7(card) == 0)
	{
		return 0;
	}
	
	// retrieve SCR register
	if (sd_protocol_send_acmd_51(card) == 0)
	{
		return 0;
	}
	
	// set bus width
	if (card->four_bit_bus_width_support)
	{
		if (sd_protocol_send_acmd_6(card) == 0)
		{
			return 0;
		}
		hsmci_set_bus_width(HSMCI, HSMCI_SD_BUS_WIDTH_4_BIT, card->slot);
	}
	
	if (card->version_1_10_and_later)
	{
		if (sd_protocol_send_cmd_6(card) == 0)
		{
			return 0;
		}
		if (card->high_speed_support)
		{
			hsmci_high_speed_enable(HSMCI);
		}
	}
	hsmci_set_bus_speed(HSMCI, card->bus_speed, 150000000);
	
	card->card_initialized = 1;
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_read(sd_card* card, uint8_t *data, uint32_t sector, uint32_t count)
{
	// First check if the section is supported on the card
	if (sector + count >= card->number_of_blocks)
	{
		return 0;
	}
	
	uint32_t command = 0;
	uint32_t argument = 0;
	
	for (uint8_t i = 0; i < count; i++)
	{
		// For each sector
		// Check if card is ready
		if (sd_protocol_send_cmd_13(card) == 0)
		{
			return 0;	
		}
		
		command |= (17 | HSMCI_CMDR_TRTYP_SINGLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ | SD_PROTOCOL_RESPONSE_1);
		
		if (card->card_type == SDSC)
		{
			// Standard capacity
			argument = (sector + i) * 512;
		}
		else if (card->card_type == SDHC)
		{
			// High capacity
			argument = sector + i;
		}
		
		hsmci_send_addressed_data_transfer_command(HSMCI, command, argument, 512, 1, 0, CHECK_CRC);
		
		// Read the data in response
		uint32_t data_reg;
		
		for (uint8_t i = 0; i < 128; i++)
		{
			data_reg = hsmci_read_data_register(HSMCI);
			
			*((uint32_t *)(data)) = data_reg;
			data += 4;
			
			// Read complete
			//sd_protocol_print_reg("Data: ", data_reg, 32);
		}
		
		// Check for error
		uint32_t status = hsmci_read_48_bit_response_register(HSMCI);
		
		if (status & (0b1111111111111 << 19))
		{
			sd_protocol_print_reg("Status reg: ", status, 32);
			board_serial_print("ERROR\n");
		}
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_write(sd_card* card, const uint8_t *data, uint32_t sector, uint32_t count)
{
	// First check if the section is supported on the card
	if (sector + count >= card->number_of_blocks)
	{
		return 0;
	}
	
	uint32_t command = 0;
	uint32_t argument = 0;
	
	for (uint8_t i = 0; i < count; i++)
	{
		// For each sector
		// Check if card is ready
		if (sd_protocol_send_cmd_13(card) == 0)
		{
			return 0;
		}
		
		command |= (24 | HSMCI_CMDR_TRTYP_SINGLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_WRITE | SD_PROTOCOL_RESPONSE_1);
		
		if (card->card_type == SDSC)
		{
			// Standard capacity
			argument = (sector + i) * 512;
		} 
		else if (card->card_type == SDHC)
		{
			// High capacity
			argument = sector + i;
		}
		
		if (hsmci_send_addressed_data_transfer_command(HSMCI, command, argument, 512, 1, 0, CHECK_CRC) == HSMCI_ERROR)
		{
			board_serial_print("WARNING\n");
		}
		
		// Check for error
		uint32_t status = 0;
		status = hsmci_read_48_bit_response_register(HSMCI);
		//sd_protocol_print_reg("STATUS: ", status, 32);
		if (status & (0b1111111111111 << 19))
		{
			
			board_serial_print("ERROR\n");
			while (1);
		}
		
		for (uint8_t i = 0; i < 128; i++)
		{
			hsmci_write_data_register(HSMCI, *((uint32_t *)data));
			data += 4;
			
			// Read complete
			//board_serial_print("Byte transfered\n"); 
		}
		while (!(hsmci_read_status_register(HSMCI) & (1 << HSMCI_SR_NOTBUSY_Pos)))
		{
			
		}
		
		
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t sd_protocol_dma_read(sd_card* card, uint8_t *data, uint32_t sector, uint32_t count)
{
	board_serial_print("x\n");
	// First check if the section is supported on the card
	if (sector + count >= card->number_of_blocks)
	{
		return 0;
	}
	board_serial_print_address("data: ", (uint32_t)data);
	
	uint32_t command = 0;
	uint32_t argument = 0;
	
	// Check if the card is ready for new data
	if (sd_protocol_send_cmd_13(card) == 0)
	{
		return 0;
	}
	
	if (count > 1)
	{
		command |= (18 | HSMCI_CMDR_TRTYP_MULTIPLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ | SD_PROTOCOL_RESPONSE_1);
	}
	else
	{
		command |= (17 | HSMCI_CMDR_TRTYP_SINGLE | SD_PROTOCOL_ADDRESSED_DATA_TRANSFER_READ | SD_PROTOCOL_RESPONSE_1);
	}
	
	if (card->card_type == SDSC)
	{
		// Standard capacity
		argument = sector * 512;
	}
	else if (card->card_type == SDHC)
	{
		// High capacity
		argument = sector;
	}
	
	hsmci_send_addressed_data_transfer_command(HSMCI, command, argument, 512, count, 1, CHECK_CRC);

	hsmci_start_read_blocks(data, count);
	
	hsmci_wait_end_of_read();
	
	hsmci_send_command(HSMCI, 12 | SD_PROTOCOL_RESPONSE_1b, 0, CHECK_CRC);
	
	SCB_InvalidateDCache_by_Addr((uint32_t *)((uint32_t)data & ~32), count * 512 + 32);
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//