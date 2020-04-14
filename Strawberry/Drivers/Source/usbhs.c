// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "usbhs.h"
#include "critical_section.h"


//--------------------------------------------------------------------------------------------------//


void usbhs_global_set_mode(usb_driver_mode_e usb_mode)
{
	uint32_t tmp = USBHS->USBHS_CTRL;
	
	if (usb_mode == USB_DRIVER_DEVICE)
	{
		tmp |= (1 << USBHS_CTRL_UIMOD_Pos);
	}
	else
	{
		tmp &= ~(1 << USBHS_CTRL_UIMOD_Pos);
	}
	
	tmp &= ~(1 << 24);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_freeze_clock(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL |= (1 << USBHS_CTRL_FRZCLK_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_unfreeze_clock(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL &= ~(1 << USBHS_CTRL_FRZCLK_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_enable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL |= (1 << USBHS_CTRL_USBE_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_disable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL &= ~(1 << USBHS_CTRL_USBE_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_send_vbus_request(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_SFR = (1 << USBHS_SFR_VBUSRQS_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


usb_driver_speed_status_e usbhs_global_get_speed_status(void)
{
	uint32_t tmp = USBHS->USBHS_SR;
	
	tmp &= USBHS_SR_SPEED_Msk;
	tmp >>= USBHS_SR_SPEED_Pos;
	
	return (usb_driver_speed_status_e)tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t usbhs_global_get_status_register(void)
{
	uint32_t tmp = USBHS->USBHS_SR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint8_t usbhs_global_clock_usable(void)
{
	if (USBHS->USBHS_SR & (1 << USBHS_SR_CLKUSABLE_Pos))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_vbus_hardware_enable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL &= ~(1 << USBHS_CTRL_VBUSHWC_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_global_vbus_hardware_disable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_CTRL |= (1 << USBHS_CTRL_VBUSHWC_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_set_speed_capability(usb_driver_host_speed_e speed)
{
	uint32_t tmp = USBHS->USBHS_HSTCTRL;
	
	tmp &= ~USBHS_HSTCTRL_SPDCONF_Msk;
	tmp |= (speed << USBHS_HSTCTRL_SPDCONF_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_send_resume(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL |= (1 << USBHS_HSTCTRL_RESUME_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_send_reset(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL |= (1 << USBHS_HSTCTRL_RESET_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_send_reset_clear(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL &= ~(1 << USBHS_HSTCTRL_RESET_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_SOF_enable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL |= (1 << USBHS_HSTCTRL_SOFE_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_SOF_disable(void)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTCTRL &= ~(1 << USBHS_HSTCTRL_SOFE_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t usbhs_host_get_interrupt_status_register(void)
{
	uint32_t tmp = USBHS->USBHS_HSTISR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t usbhs_host_get_interrupt_mask(void)
{
	uint32_t tmp = USBHS->USBHS_HSTIMR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_interrupt_clear(uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTICR = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_interrupt_force(uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTIFR = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_interrupt_disable(uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTIDR = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_interrupt_enable(uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTIER = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint16_t usbhs_host_get_frame_number(void)
{
	uint32_t tmp = USBHS->USBHS_HSTFNUM;
	
	tmp &= USBHS_HSTFNUM_FNUM_Msk;
	tmp >>= USBHS_HSTFNUM_FNUM_Pos;
	
	return (uint16_t)tmp;
}


//--------------------------------------------------------------------------------------------------//


uint8_t usbhs_host_get_microframe_number(void)
{
	uint32_t tmp = USBHS->USBHS_HSTFNUM;
	
	tmp &= USBHS_HSTFNUM_MFNUM_Msk;
	
	return (uint8_t)tmp;
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_set_pipe_address(uint8_t pipe_number, uint8_t addr)
{
	uint32_t tmp;
	
	if (pipe_number < 4)
	{
		tmp = USBHS->USBHS_HSTADDR1;
		
		tmp &= ~(0xff << pipe_number);
		tmp |= ((addr & 0b1111111) << pipe_number);
		
		CRITICAL_SECTION_ENTER()
		USBHS->USBHS_HSTADDR1 = tmp;
		CRITICAL_SECTION_LEAVE()
	}
	else if (pipe_number < 8)
	{
		pipe_number -= 4;
		
		tmp = USBHS->USBHS_HSTADDR2;
		
		tmp &= ~(0xff << pipe_number);
		tmp |= ((addr & 0b1111111) << pipe_number);
		
		CRITICAL_SECTION_ENTER()
		USBHS->USBHS_HSTADDR2 = tmp;
		CRITICAL_SECTION_LEAVE()
	}
	else if (pipe_number < 10)
	{
		pipe_number -= 8;
		
		tmp = USBHS->USBHS_HSTADDR3;
		
		tmp &= ~(0xff << pipe_number);
		tmp |= ((addr & 0b1111111) << pipe_number);
		
		CRITICAL_SECTION_ENTER()
		USBHS->USBHS_HSTADDR3 = tmp;
		CRITICAL_SECTION_LEAVE()
	}
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_enable(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIP |= (1 << pipe_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_disable(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIP &= ~(1 << pipe_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_reset_on(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIP |= (1 << (pipe_number + 16));
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_reset_off(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIP &= ~(1 << (pipe_number + 16));
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_interrupt_frequency(uint8_t pipe_number, uint8_t interrupt_frequency)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPCFG[pipe_number];
	
	tmp &= ~USBHS_HSTPIPCFG_INTFRQ_Msk;
	tmp |= (interrupt_frequency << USBHS_HSTPIPCFG_INTFRQ_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_endpoint_number(uint8_t pipe_number, uint8_t endpoint_number)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPCFG[pipe_number];
	
	tmp &= ~USBHS_HSTPIPCFG_PEPNUM_Msk;
	tmp |= (endpoint_number << USBHS_HSTPIPCFG_PEPNUM_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_type(uint8_t pipe_number, usb_driver_pipe_type_e pipe_type)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPCFG[pipe_number];
	
	tmp &= ~USBHS_HSTPIPCFG_PTYPE_Msk;
	tmp |= (pipe_type << USBHS_HSTPIPCFG_PTYPE_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_token(uint8_t pipe_number, usb_driver_pipe_token_e token)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPCFG[pipe_number];
	
	tmp &= ~USBHS_HSTPIPCFG_PTOKEN_Msk;
	tmp |= (token << USBHS_HSTPIPCFG_PTOKEN_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_set_size(uint8_t pipe_number, usb_driver_pipe_size_e pipe_size)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPCFG[pipe_number];
	
	tmp &= ~USBHS_HSTPIPCFG_PSIZE_Msk;
	tmp |= (pipe_size << USBHS_HSTPIPCFG_PSIZE_Pos);
	
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_auto_bankswitch_enable(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] |= (1 << USBHS_HSTPIPCFG_AUTOSW_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_auto_bankswitch_disable(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] &= ~(1 << USBHS_HSTPIPCFG_AUTOSW_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_allocate(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] |= (1 << USBHS_HSTPIPCFG_ALLOC_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_free(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPCFG[pipe_number] &= ~(1 << USBHS_HSTPIPCFG_ALLOC_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t usbhs_host_pipe_get_interrupt_status_register(uint8_t pipe_number)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPISR[pipe_number];
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t usbhs_host_pipe_get_interrupt_mask(uint8_t pipe_number)
{
	uint32_t tmp = USBHS->USBHS_HSTPIPIMR[pipe_number];

	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_interrupt_clear(uint8_t pipe_number, uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPICR[pipe_number] = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_interrupt_force(uint8_t pipe_number, uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPIFR[pipe_number] = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_interrupt_enable(uint8_t pipe_number, uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPIER[pipe_number] = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_pipe_interrupt_disable(uint8_t pipe_number, uint32_t mask)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPIDR[pipe_number] = mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_in_request_disable(uint8_t pipe_number)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPINRQ[pipe_number] = (1 << USBHS_HSTPIPINRQ_INRQ_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usbhs_host_in_request_enable(uint8_t pipe_number, uint8_t number_of_in_requests)
{
	CRITICAL_SECTION_ENTER()
	USBHS->USBHS_HSTPIPINRQ[pipe_number] = number_of_in_requests;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//