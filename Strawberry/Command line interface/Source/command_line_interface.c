// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "usart.h"
#include "board_serial.h"
#include "file_system_fat.h"
#include "dynamic_memory.h"
#include "kernel.h"
#include "critical_section.h"
#include "board_sd_card.h"
#include "interrupt.h"
#include "dynamic_loader.h"
#include "thread.h"


//--------------------------------------------------------------------------------------------------//


#include <string.h>


//--------------------------------------------------------------------------------------------------//


file_system_t cortex_file_system;


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_input_decode(char* data);

file_result_t file_system_command_line_ls(void);

file_result_t file_system_command_line_cd(char* arg);

file_result_t file_system_command_line_cat(char* arg);

file_result_t file_system_command_line_hex(char* arg);

file_result_t file_system_command_line_run(char* arg);

void file_system_command_line_print_hex(uint8_t tmp);

void file_system_command_line_print_directory(void);

void file_system_command_line_handler(void);

void file_system_command_line_thread(void* args);


//--------------------------------------------------------------------------------------------------//


#define FILE_SYSTEM_MAX_PATH_LENGTH		1000
#define FILE_SYSTEM_MAX_LIST_LENGTH		30
#define FILE_SYSTEM_BUFFER_SIZE			512
#define FILE_SYSTEM_COMMAND_BUFFER_SIZE	100
#define MAX_ARGUEMTNS 6
#define LENGTH_ARGUMENT 50


//--------------------------------------------------------------------------------------------------//


static char command_line_argument[MAX_ARGUEMTNS][LENGTH_ARGUMENT];
static char file_system_path[FILE_SYSTEM_MAX_PATH_LENGTH];
static char file_system_tmp_path[FILE_SYSTEM_MAX_PATH_LENGTH];
static char command_line_buffer[FILE_SYSTEM_COMMAND_BUFFER_SIZE];
static uint16_t command_line_buffer_index = 0;
static volatile uint8_t file_system_command_ready = 0;


directory_t directory;
file_info_t file_info;


struct thread_structure* file_thread;


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_config(void)
{
	strcpy(file_system_path, "/");
	
	file_thread = thread_new("file", file_system_command_line_thread, NULL, THREAD_PRIORITY_NORMAL, 500);
}


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_thread(void* args)
{	
	board_sd_card_config();
	
	while (1)
	{
		while (board_sd_card_get_status() == SD_DISCONNECTED)
		{
			syscall_sleep(100);
		}
		
		board_serial_print("Card connected\n");
		
		syscall_sleep(200);
		
		uint8_t retry_count = 0;
		while (retry_count++ < 5)
		{
			if (file_mount(&cortex_file_system, "", 1) == FR_OK)
			{
				file_system_command_line_print_directory();
				break;
			}
			syscall_sleep(200);
		}
		if (retry_count >= 5)
		{
			board_serial_print("Mount error\n");
		}
		
		while (1)
		{
			if (board_sd_card_get_status() == SD_DISCONNECTED)
			{
				board_serial_print("Card disconnected\n");
				break;
			}
			if (file_system_command_ready)
			{
				file_system_command_line_handler();
				file_system_command_line_print_directory();
				
			}
			syscall_sleep(100);
			
		}
	}
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_ls(void)
{
	file_result_t result;

	uint16_t file_count = 0;

	result = file_opendir(&directory, file_system_path);

	// Return if not successful
	if (result != FR_OK)
	{
		return result;
	}
	board_serial_print("\n");

	do
	{
		result = file_readdir(&directory, &file_info);

		if (result != FR_OK)
		{
			return result;
		}

		// Format the size so that we do not get ugly output 
		if (file_info.fsize > 1000000)
		{
			board_serial_print("%d\tMB\t", file_info.fsize / 1000000);
		}
		else if (file_info.fsize > 1000)
		{
			board_serial_print("%d\tkB\t", file_info.fsize / 1000);
		}
		else
		{
			board_serial_print("%d\tkB\t", 1);
		}
		
		board_serial_print("%d.%d.%d\t%d:%d\t", (file_info.fdate & 0b11111), (file_info.fdate >> 4) & 0b1111, 1980 + ((file_info.fdate >> 9) & 0b1111111), (file_info.ftime >> 11) & 0b11111, (file_info.ftime >> 5) & 0b111111);

		if (file_info.fattrib == AM_DIR)
		{
			board_serial_print("<DIR>");
		}
		else
		{

		}
		board_serial_print("\t\t");
		//board_serial_print(file_info.fname);


		board_serial_print(file_info.fname);
		board_serial_print("\n");

		if (file_count > FILE_SYSTEM_MAX_LIST_LENGTH)
		{
			return FR_OK;
		}

		file_count++;


	} while ((file_info.fname[0]) && (file_count < 100));

	board_serial_print("\n");

	return FR_OK;
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_touch(char* arg)
{
	file_result_t res;
	file_t file;

	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{

	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = file_open(&file, file_system_tmp_path, FA_WRITE | FA_OPEN_ALWAYS);
	
	if (res == FR_OK)
	{
		file_close(&file);
		return FR_OK;
	}
	return res;
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_rm(char* arg)
{
	file_result_t res;

	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{

	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = file_unlink(file_system_tmp_path);
	
	return res;
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_cd(char* arg)
{
	if (strcmp(arg, ""))
	{
		strcpy(file_system_tmp_path, file_system_path);

		// If the argument is a full path
		if (arg[0] == '/')
		{
			if (strlen(arg) + 1 > sizeof(file_system_path))
			{
				return FR_OK;
			}
			else
			{
				strncpy(file_system_tmp_path, arg, sizeof(file_system_tmp_path));
			}
		}
		else if (!strcmp(arg, ".."))
		{
			uint16_t back_counter = strlen(file_system_tmp_path) - 1;

			while ((file_system_tmp_path[back_counter] != '/') && (back_counter > 1))
			{
				back_counter--;
			}
			file_system_tmp_path[back_counter] = 0;
		}
		else
		{
			if (strlen(file_system_tmp_path) + strlen(arg) + 2 > sizeof(file_system_path))
			{
				return FR_OK;
			}
			else
			{
				if (strcmp(file_system_tmp_path, "/"))
				{
					strcat(file_system_tmp_path, "/");
				}

				strcat(file_system_tmp_path, arg);
			}
		}

		file_result_t res = file_opendir(&directory, file_system_tmp_path);

		if (res == FR_OK)
		{
			strncpy(file_system_path, file_system_tmp_path, sizeof(file_system_path));
		}

		return FR_OK;
	}
	return FR_OK;
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_cat(char* arg)
{
	file_result_t res;
	file_t file;
	uint32_t bytes_read = 0;

	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{

	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = file_open(&file, file_system_tmp_path, FA_READ);
	if (res != FR_OK)
	{
		return res;
	}

	char* file_system_buffer = (char *)dynamic_memory_new(DRAM_BANK_1, 1024);

	do
	{
		res = file_read(&file, file_system_buffer, 1024, &bytes_read);
		if (res != FR_OK)
		{
			dynamic_memory_free(file_system_buffer);
			return res;
		}
		
		/*
		for (uint16_t i = 0; i < bytes_read; i++)
		{
			board_serial_write(file_system_buffer[i]);
		}*/
		board_serial_dma_print_size(file_system_buffer, bytes_read);


	} while (bytes_read == 1024);

	dynamic_memory_free(file_system_buffer);
	res = file_close(&file);
	if (res != FR_OK)
	{
		return res;
	}

	return FR_OK;
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_hex(char* arg)
{
	file_result_t res;
	file_t file;
	uint32_t bytes_read = 0;

	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{

	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = file_open(&file, file_system_tmp_path, FA_READ);
	if (res != FR_OK)
	{
		return res;
	}
	
	char* file_system_buffer = (char *)dynamic_memory_new(DRAM_BANK_1, 1024);

	do
	{
		res = file_read(&file, file_system_buffer, 1024, &bytes_read);
		if (res != FR_OK)
		{
			dynamic_memory_free(file_system_buffer);
			return res;
		}

		for (uint16_t i = 0; i < bytes_read; i++)
		{
			if ((i % 4) == 0)
			{
				board_serial_print("\n");
			}
			file_system_command_line_print_hex(file_system_buffer[i]);
			board_serial_print("\t");
		}


	} while (bytes_read == 1024);

	dynamic_memory_free(file_system_buffer);
	res = file_close(&file);
	if (res != FR_OK)
	{
		return res;
	}

	return FR_OK;
}


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_print_hex(uint8_t tmp)
{
	uint8_t ch_1 = (tmp >> 4);
	uint8_t ch_2 = (0xf & tmp);
	
	if (ch_1 < 10)
	{
		board_serial_write('0' + ch_1);
	}
	else 
	{
		ch_1 -= 10;
		board_serial_write('A' + ch_1);
	}
	if (ch_2 < 10)
	{
		board_serial_write('0' + ch_2);
	}
	else
	{
		ch_2 -= 10;
		board_serial_write('A' + ch_2);
	}
}


//--------------------------------------------------------------------------------------------------//


file_result_t file_system_command_line_run(char* arg)
{
	file_result_t res;
	file_t file;
	uint32_t bytes_read = 0;
	
	//file_info_t file_info;


	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{

	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = file_open(&file, file_system_tmp_path, FA_READ);
	if (res != FR_OK)
	{
		return res;
	}

	// Before we get the data we have to allocate space for it
	uint8_t* application = (uint8_t *)dynamic_memory_new(SRAM, 2000);
	char* file_system_buffer = (char *)dynamic_memory_new(DRAM_BANK_1, 1024);
	uint8_t* application_iterator = application;
	do
	{
		res = file_read(&file, file_system_buffer, 1024, &bytes_read);
		if (res != FR_OK)
		{
			dynamic_memory_free(file_system_buffer);
			return res;
		}

		for (uint16_t i = 0; i < bytes_read; i++)
		{	
			*application_iterator++ = file_system_buffer[i];
		}


	} while (bytes_read == 1024);
	
	dynamic_memory_free(file_system_buffer);
	res = file_close(&file);
	if (res != FR_OK)
	{
		return res;
	}
	
	dynamic_loader_run((uint32_t *)application, bytes_read);

	return FR_OK;
}


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_print_directory(void)
{
	board_serial_print("Cortex:");
	board_serial_print(file_system_path);
	char* tmp = file_system_path;
	while (*tmp != '\0')
	{
		if (*tmp == '\n')
		{
			board_serial_print("ERROOROROROROROROR\n");
		}
		tmp++;
	}
	board_serial_print(" >");
}


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_input_decode(char* data)
{
	uint8_t argument_index = 0;
	uint8_t char_index = 0;

	while (*data != '\0')
	{
		// Check for overflow
		if ((char_index >= LENGTH_ARGUMENT)  || (argument_index >= MAX_ARGUEMTNS))
		{
			return;
		}

		// If we receive a space
		if (*data == ' ')
		{
			if (char_index < LENGTH_ARGUMENT - 1)
			{
				command_line_argument[argument_index][char_index] = '\0';
				argument_index++;
				char_index = 0;
				data++;
			}
			else
			{
				return;
			}
		}

		command_line_argument[argument_index][char_index++] = *data;

		data++;
	}
	if (char_index < LENGTH_ARGUMENT - 1)
	{
		command_line_argument[argument_index][char_index] = '\0';
		char_index = 0;
	}
	
	file_system_command_ready = 1;
}


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_handler(void)
{
	file_result_t result = FR_OK;
	
	if (!strncmp(command_line_argument[0], "ls", 2))
	{
		result = file_system_command_line_ls();
	}
	else if (!strncmp(command_line_argument[0], "touch", 5))
	{
		result = file_system_command_line_touch(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "rm", 2))
	{
		result = file_system_command_line_rm(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "cd", 2))
	{
		result = file_system_command_line_cd(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "cat", 3))
	{
		result = file_system_command_line_cat(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "run", 3))
	{
		result = file_system_command_line_run(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "hex", 3))
	{
		result = file_system_command_line_hex(command_line_argument[1]);
	}
	file_system_command_ready = 0;
	
	if (result != FR_OK)
	{
		board_serial_print("Error occured\n");
	}
}


//--------------------------------------------------------------------------------------------------//


void USART1_Handler()
{
	// The RXRDY flag is cleared upon read of RHR
	char data = (char)usart_read(USART1);

	// Local echo to the serial interface
	//board_serial_write(data);

	if (data == '\r')
	{
		command_line_buffer_index = 0;
	}

	if (data != '\n')
	{
		command_line_buffer[command_line_buffer_index++] = data;
	}
	else
	{
		command_line_buffer[command_line_buffer_index] = '\0';
		command_line_buffer_index = 0;

		// Set a flag here
		file_system_command_ready = 1;
		file_system_command_line_input_decode(command_line_buffer);
	}
}


//--------------------------------------------------------------------------------------------------//