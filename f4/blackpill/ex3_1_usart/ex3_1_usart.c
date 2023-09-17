#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/dwt.h>
#include "ex3_1_usart.h"

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_96MHZ]);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART2);
}

static uint32_t dwt_setup(void)
{
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

static void dwt_delay_ms(volatile uint32_t milliseconds)
{
        uint32_t initial_ticks = dwt_read_cycle_counter();
        uint32_t ms_count_tics = milliseconds * (rcc_ahb_frequency / 1000);
        while ((dwt_read_cycle_counter() - initial_ticks) < ms_count_tics);
}

static void usart_transmit(const char *str)
{
        while (*str != '\000') {
                usart_send_blocking(USART2, *str);
                str++;
        }
}

int main(void)
{
	clock_setup();
	dwt_setup();
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
 	gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

 	usart_set_baudrate(USART2, 9600);
 	usart_set_databits(USART2, 8);
 	usart_set_stopbits(USART2, USART_STOPBITS_1);
 	usart_set_parity(USART2, USART_PARITY_NONE);
 	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
 	usart_set_mode(USART2, USART_MODE_TX_RX);

 	usart_enable(USART2);
	while(1){
	  	dwt_delay_ms(1000);
		usart_transmit("Hello from LibOpenCM3!\r\n");
	}
}
