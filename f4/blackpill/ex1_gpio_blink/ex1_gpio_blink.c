#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/dwt.h>
#include "ex1_gpio_blink.h"

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_96MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
}

static uint32_t dwt_setup(void) {
        dwt_enable_cycle_counter();
        __asm volatile ("nop");
        __asm volatile ("nop");
        __asm volatile ("nop");

     if(dwt_read_cycle_counter())
     {
       return 0;
     }
     else
  {
    return 1;
  }
}

static void dwt_delay_us(uint32_t microseconds){
        uint32_t initial_ticks = dwt_read_cycle_counter();
        uint32_t us_count_tics = microseconds * (rcc_ahb_frequency / 1000000);
        while ((dwt_read_cycle_counter() - initial_ticks) < us_count_tics);
}

static void dwt_delay_ms(uint32_t milliseconds){
        uint32_t initial_ticks = dwt_read_cycle_counter();
        uint32_t ms_count_tics = milliseconds * (rcc_ahb_frequency / 1000);
        while ((dwt_read_cycle_counter() - initial_ticks) < ms_count_tics);
}


int main(void)
{
	clock_setup();
	dwt_setup();
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	while(1){
		dwt_delay_ms(1000);
                gpio_toggle(GPIOC, GPIO13);
	}
}
