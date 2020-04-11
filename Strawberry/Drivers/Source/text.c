// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "text.h"
#include "board_serial.h"


//--------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdarg.h>


//--------------------------------------------------------------------------------------------------//


static const unsigned s_value[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};


//--------------------------------------------------------------------------------------------------//


uint32_t number_to_buffer(char* buffer, uint32_t number)
{
	char tmp_buffer[12];
	uint32_t number_of_chars = 0;
	
	if (number == 0)
	{
		*buffer = '0';
		
		return 1;
	}
	
	uint32_t i;
	uint32_t j;
	
	for (i = 0; i < 10; i++)
	{
		for (j = 0; i < 10; j++)
		{
			if (number >= s_value[i])
			{
				number -= s_value[i];
			}
			else
			{
				break;
			}
		}
		tmp_buffer[i] = '0' + j;
	}
	
	for (i = 0; i < 10; i++)
	{
		if (tmp_buffer[i] != '0')
		{
			break;
		}
	}
	
	for (; i < 10; i++)
	{
		*buffer++ = tmp_buffer[i];
		number_of_chars++;
	}
	
	return number_of_chars;
}


//--------------------------------------------------------------------------------------------------//


void text_to_buffer(char* output, uint32_t* output_size, const char* input, ...)
{
	va_list argument_list;

	uint32_t count = 0;

	va_start(argument_list, input);
	
	while (*input)
	{
		if (*input != '%')
		{
			*output = *input;
			
			output++;
			input++;
			
			count++;
		}
		else
		{
			input++;
			
			// Check which formatting we should use
			switch (*input)
			{
				case 'c':
				{
					char format_char = va_arg(argument_list, int);
					
					*output++ = format_char;
					
					input++;
					
					count++;
					break;
				}
				case 'd':
				{
					int format_integer = va_arg(argument_list, int);
					
					int number_of_chars;
					
					if (format_integer < 0)
					{
						number_of_chars = number_to_buffer(output, (uint32_t)(-format_integer));
					}
					else
					{
						number_of_chars = number_to_buffer(output, (uint32_t)format_integer);
					}
					
					count += number_of_chars;
					output += number_of_chars;
					input++;
					
					break;
				}
				case 'u':
				{
					unsigned format_unsigned_integer = va_arg(argument_list, unsigned);
					
					int number_of_chars = number_to_buffer(output, (uint32_t)format_unsigned_integer);
					
					count += number_of_chars;
					output += number_of_chars;
					input++;
					
					break;
				}
				case 's':
				{
					char* format_string = va_arg(argument_list, char *);
					
					while (*format_string)
					{
						*output++ = *format_string++;
						
						count++;
					}
					input++;
					break;
				}
				default:
				{
					*output++ = *input++;
					count++;
					break;
				}
			}
		}
	}
	
	*output = '\0';
	
	va_end(argument_list);
	
	*output_size = count;
}


//--------------------------------------------------------------------------------------------------//