#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include "test_blink.h"
 
static void clock_setup(void)
{
        rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
        rcc_periph_clock_enable(RCC_GPIOC);
        rcc_periph_clock_enable(RCC_TIM3);
}
 
int main(void)
{
        clock_setup();
 
        gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
 
        rcc_periph_reset_pulse(RST_TIM3);
        timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
        timer_continuous_mode(TIM3);
        timer_set_prescaler(TIM3, 7200-1); //72 Mhz / 7200 = 10 Khz
        timer_set_period(TIM3, 2500-1); // 10 khz / 2500 = 4 Hz
        timer_disable_preload(TIM3);
        timer_enable_irq(TIM3, TIM_DIER_UIE);
        timer_enable_counter(TIM3);
        nvic_enable_irq(NVIC_TIM3_IRQ);
 
        while(1){
        }
}
 
void tim3_isr(){
        if (timer_interrupt_source(TIM3, TIM_SR_UIF)) {
                timer_clear_flag(TIM3, TIM_SR_UIF);
                gpio_toggle(GPIOC, GPIO13);
        }
}


