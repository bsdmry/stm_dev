#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include "ex5_3_timer.h"

uint32_t current_value = 0;

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_96MHZ]);
	rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_USART2);
        rcc_periph_clock_enable(RCC_TIM1);
}

static uint32_t rotary_encoder_tim1_get_value(void){
    return (timer_get_counter(TIM1) >> 2);
}

static void usart_transmit(uint32_t number){
        uint16_t div = 1000;
        while (div > 0) {
                usart_send_blocking(USART2, ((uint8_t)(number / div) + 0x30));
                number = number % div;
                div /= 10;
        }

        usart_send_blocking(USART2, 0x0D);
        usart_send_blocking(USART2, 0x0A);
}



int main(void)
{
	uint32_t max_value = 2000;

	clock_setup();
	
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO8 | GPIO9);
    	gpio_set_af(GPIOA, GPIO_AF1, GPIO8 | GPIO9);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
        gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

        timer_set_period(TIM1, max_value*4 );
        timer_slave_set_mode(TIM1, TIM_SMCR_SMS_EM3);
        timer_ic_set_filter(TIM1, TIM_IC1,  TIM_IC_DTF_DIV_32_N_8 );
        timer_ic_set_filter(TIM1, TIM_IC2,  TIM_IC_DTF_DIV_32_N_8 );
        timer_ic_set_input(TIM1, TIM_IC1, TIM_IC_IN_TI1);
        timer_ic_set_input(TIM1, TIM_IC2, TIM_IC_IN_TI2);
        timer_set_counter(TIM1, 0);
        timer_enable_update_event(TIM1);
        timer_enable_counter(TIM1);
 
        usart_set_baudrate(USART2, 9600);
        usart_set_databits(USART2, 8);
        usart_set_stopbits(USART2, USART_STOPBITS_1);
        usart_set_parity(USART2, USART_PARITY_NONE);
        usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
        usart_set_mode(USART2, USART_MODE_TX);
        usart_enable(USART2);
 
        while(1){
                if(rotary_encoder_tim1_get_value() != current_value){
                        current_value =  rotary_encoder_tim1_get_value();
                        usart_transmit(current_value);
                }
        }
}
