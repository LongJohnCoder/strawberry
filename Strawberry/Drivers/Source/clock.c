// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "clock.h"
#include "critical_section.h"


//--------------------------------------------------------------------------------------------------//


void clock_sources_config(clock_source_e clock_oscillator, uint8_t startup_time)
{
	uint32_t data;
	
	if (clock_oscillator == CLOCK_SOURCE_CRYSTAL)
	{
		data = PMC->CKGR_MOR & ~(CKGR_MOR_MOSCXTBY_Msk | CKGR_MOR_MOSCXTST_Msk);
		
		data |= (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos) | (1 << CKGR_MOR_MOSCXTEN_Pos) | (CKGR_MOR_MOSCXTST_Msk & (startup_time << CKGR_MOR_MOSCXTST_Pos));
		
		CRITICAL_SECTION_ENTER()
		PMC->CKGR_MOR = data;
		CRITICAL_SECTION_LEAVE()
		
		// Wait for crystal to stabilize
		while (!(PMC->PMC_SR & (1 << PMC_SR_MOSCXTS_Pos)))
		{
			
		}
	}
	else if (clock_oscillator == CLOCK_SOURCE_RC)
	{
		data = PMC->CKGR_MOR;
				
		data |= (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos) | (1 << CKGR_MOR_MOSCRCEN_Pos);
		
		// Enable RC oscillator but do not switch yet
		CRITICAL_SECTION_ENTER()
		PMC->CKGR_MOR = data;
		CRITICAL_SECTION_LEAVE()
		
		// Wait for RC oscillator to stabilize
		while (!(PMC->PMC_SR & (1 << PMC_SR_MOSCRCS_Pos)))
		{
			
		}
		
		// Change RC oscillator frequency
		data = PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk;
		
		data |= (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos) | (2 << CKGR_MOR_MOSCRCF_Pos);
		
		CRITICAL_SECTION_ENTER()
		PMC->CKGR_MOR = data;
		CRITICAL_SECTION_LEAVE()
		
		// Wait for RC to stabilize
		while (!(PMC->PMC_SR & (1 << PMC_SR_MOSCRCS_Pos)))
		{
			
		}
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_main_clock_config(clock_source_e clock_oscillator)
{
	if (clock_oscillator == CLOCK_SOURCE_CRYSTAL)
	{
		// Use crystal as source for main clock
		CRITICAL_SECTION_ENTER()
		PMC->CKGR_MOR |= (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos) | (1 << CKGR_MOR_MOSCSEL_Pos);
		CRITICAL_SECTION_LEAVE()
	}
	else if (clock_oscillator == CLOCK_SOURCE_RC)
	{
		// Switch main clock to RC oscillator
		uint32_t data = (PMC->CKGR_MOR & ~(1 << CKGR_MOR_MOSCSEL_Pos));
		
		data |= (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos);
		
		CRITICAL_SECTION_ENTER()
		PMC->CKGR_MOR = data;
		CRITICAL_SECTION_LEAVE()
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_pll_config(uint16_t multiplication_factor, uint8_t division_factor, uint8_t startup_time)
{
	// Configuring PLL
	uint32_t data = (CKGR_PLLAR_MULA_Msk & ((multiplication_factor - 1) << CKGR_PLLAR_MULA_Pos)) |
	(CKGR_PLLAR_DIVA_Msk & (division_factor << CKGR_PLLAR_DIVA_Pos)) |
	(CKGR_PLLAR_PLLACOUNT_Msk & (startup_time << CKGR_PLLAR_PLLACOUNT_Pos));
	
	// Stop PLL first
	uint32_t tmp = PMC->CKGR_PLLAR;
	
	tmp &= ~CKGR_PLLAR_MULA_Msk;
	
	CRITICAL_SECTION_ENTER()
	PMC->CKGR_PLLAR = tmp;
	CRITICAL_SECTION_LEAVE()
	
	// Write register
	CRITICAL_SECTION_ENTER()
	PMC->CKGR_PLLAR = ((1 << CKGR_PLLAR_ONE_Pos) | data);
	CRITICAL_SECTION_LEAVE()
	
	// Wait for PLLA to lock loop
	while (!(PMC->PMC_SR & (1 << PMC_SR_LOCKA_Pos)))
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_master_clock_config(clock_master_clock_source_e source, clock_master_clock_prescaler_e prescaler, clock_master_clock_division_e division)
{
	// Set processor clock prescaler
	uint32_t tmp = PMC->PMC_MCKR;
	
	tmp &= ~(PMC_MCKR_PRES_Msk);
	tmp |= (prescaler << PMC_MCKR_PRES_Pos);
	
	CRITICAL_SECTION_ENTER()
	PMC->PMC_MCKR = tmp;
	CRITICAL_SECTION_LEAVE()
	
	// Wait until master clock is ready
	while (!(PMC->PMC_SR & (1 << PMC_SR_MCKRDY_Pos)))
	{
		
	}
	
	// Set master clock division
	tmp = PMC->PMC_MCKR;
	
	tmp &= ~(PMC_MCKR_MDIV_Msk);
	tmp |= (division << PMC_MCKR_MDIV_Pos);
	
	CRITICAL_SECTION_ENTER()
	PMC->PMC_MCKR = tmp;
	CRITICAL_SECTION_LEAVE()
	
	// Wait until master clock is ready
	while (!(PMC->PMC_SR & (1 << PMC_SR_MCKRDY_Pos)))
	{
		
	}

	// Selecting master clock source to PLLA
	tmp = PMC->PMC_MCKR;
	
	tmp &= ~(PMC_MCKR_CSS_Msk);
	tmp |= (source << PMC_MCKR_CSS_Pos);
	
	CRITICAL_SECTION_ENTER()
	PMC->PMC_MCKR = tmp;
	CRITICAL_SECTION_LEAVE()

	// Wait until master clock is ready
	while (!(PMC->PMC_SR & (1 << PMC_SR_MCKRDY_Pos)))
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_programmable_clocks_enable(uint8_t programmable_clock, clock_master_clock_source_e source, uint8_t prescaler)
{
	uint32_t tmp = (PMC_PCK_PRES_Msk & ((prescaler - 1) << PMC_PCK_PRES_Pos)) | (source << PMC_PCK_CSS_Pos);
	
	PMC->PMC_PCK[programmable_clock] = tmp;
	
	// Wait for clock to be ready
	while (!(PMC->PMC_SR & (1 << (programmable_clock + 8))))
	{
		
	}
	
	// Enable peripheral clock output
	CRITICAL_SECTION_ENTER()
	PMC->PMC_SCER = (1 << (programmable_clock + 8));
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void clock_programmable_clock_disable(uint8_t programmable_clock)
{
	CRITICAL_SECTION_ENTER()
	PMC->PMC_SCER = (1 << (programmable_clock + 8));
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t clock_get_cpu_frequency(void)
{

	CRITICAL_SECTION_ENTER()
	PMC->CKGR_MCFR |= (1 << CKGR_MCFR_CCSS_Pos) | (1 << CKGR_MCFR_RCMEAS_Pos);
	CRITICAL_SECTION_LEAVE()
	
	//Wait for measurement to complete
	while (!(PMC->CKGR_MCFR & (1 << CKGR_MCFR_MAINFRDY_Pos)))
	{
		
	}
	
	//Read register with bit mask on MAINF
	uint32_t freq = (PMC->CKGR_MCFR & CKGR_MCFR_MAINF_Msk);
	freq = (freq * 32000)/16;

	return freq;
}


//--------------------------------------------------------------------------------------------------//


void clock_usb_pll_config(clock_source_frequency_e source_frequency, uint8_t startup_time, uint8_t divider)
{
	// First set source frequency for the right PLL multiplication
	uint32_t tmp = UTMI->UTMI_CKTRIM;
	
	tmp &= ~UTMI_CKTRIM_FREQ_Msk;
	tmp |= (source_frequency << UTMI_CKTRIM_FREQ_Pos);
	
	// Write register
	CRITICAL_SECTION_ENTER()
	UTMI->UTMI_CKTRIM = tmp;
	CRITICAL_SECTION_LEAVE()
	
	
	// Configure the UPLL register
	CRITICAL_SECTION_ENTER()
	PMC->CKGR_UCKR = (1 << CKGR_UCKR_UPLLEN_Pos) | ((startup_time << CKGR_UCKR_UPLLCOUNT_Pos) & CKGR_UCKR_UPLLCOUNT_Msk);
	CRITICAL_SECTION_LEAVE()
	
	// Wait for USB PLL to stabilize
	while (!(PMC->PMC_SR & (1 << PMC_SR_LOCKU_Pos)))
	{
		// Do nothing
	}
	
	if (divider)
	{
		// divide UPLL frequency by 2
		CRITICAL_SECTION_ENTER()
		PMC->PMC_MCKR |= (1 << PMC_MCKR_UPLLDIV2_Pos);
		CRITICAL_SECTION_LEAVE()
	}
	else
	{
		CRITICAL_SECTION_ENTER()
		PMC->PMC_MCKR &= ~(1 << PMC_MCKR_UPLLDIV2_Pos);
		CRITICAL_SECTION_LEAVE()
	}
	
	// Wait for master clock to be ready
	while (!(PMC->PMC_SR & (1 << PMC_SR_MCKRDY_Pos)))
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_usb_config(clock_usb_source_e usb_source, uint8_t division)
{
	CRITICAL_SECTION_ENTER()
	PMC->PMC_USB = (usb_source << PMC_USB_USBS_Pos) | (((division - 1) << PMC_USB_USBDIV_Pos) & PMC_USB_USBDIV_Msk);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void clock_usb_full_speed_enable(void)
{
	CRITICAL_SECTION_ENTER()
	PMC->PMC_SCER = (1 << PMC_SCER_USBCLK_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void clock_usb_full_speed_disable(void)
{
	CRITICAL_SECTION_ENTER()
	PMC->PMC_SCDR = (1 << PMC_SCER_USBCLK_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void clock_peripheral_clock_enable(uint8_t id_peripheral)
{
	if (id_peripheral < 32)
	{
		if (!(PMC->PMC_PCSR0 & (1 << id_peripheral)))
		{
			PMC->PMC_PCER0 |= (1 << id_peripheral);
		}
	}
	else if (id_peripheral < 64)
	{
		id_peripheral -= 32;
		if (!(PMC->PMC_PCSR1 & (1 << id_peripheral)))
		{
			PMC->PMC_PCER1 |= (1 << id_peripheral);
		}
	}
	else
	{
		uint32_t tmp;
		//Write peripheral ID
		CRITICAL_SECTION_ENTER()
		PMC->PMC_PCR = (PMC_PCR_PID_Msk & (id_peripheral << PMC_PCR_PID_Pos));
		CRITICAL_SECTION_LEAVE()
		
		//Read register
		tmp = PMC->PMC_PCR;
		
		//Enable peripheral clock and command write
		tmp |= (1 << PMC_PCR_EN_Pos) | (1 << PMC_PCR_CMD_Pos);
		
		//Write register
		CRITICAL_SECTION_ENTER()
		PMC->PMC_PCR = tmp;
		CRITICAL_SECTION_LEAVE()
	}
}


//--------------------------------------------------------------------------------------------------//


void clock_peripheral_clock_disable(uint8_t id_peripheral)
{
	if (id_peripheral < 32) {
		if (PMC->PMC_PCSR0 & (1 << id_peripheral))
		{
			PMC->PMC_PCDR0 |= (1 << id_peripheral);
		}
	}
	else if (id_peripheral < 64)
	{
		id_peripheral -= 32;
		if (PMC->PMC_PCSR1 & (1 << id_peripheral))
		{
			PMC->PMC_PCDR1 |= (1 << id_peripheral);
		}
	}
	else
	{
		uint32_t tmp;
		//Write peripheral ID
		CRITICAL_SECTION_ENTER()
		PMC->PMC_PCR = (PMC_PCR_PID_Msk & (id_peripheral << PMC_PCR_PID_Pos));
		CRITICAL_SECTION_LEAVE()
		
		//Read register
		tmp = PMC->PMC_PCR;
		
		//Enable peripheral clock and command write
		tmp |= (1 << PMC_PCR_CMD_Pos);
		tmp &= ~(1 << PMC_PCR_EN_Pos);
		
		//Write register
		CRITICAL_SECTION_ENTER()
		PMC->PMC_PCR = tmp;
		CRITICAL_SECTION_LEAVE()
	}
}


//--------------------------------------------------------------------------------------------------//