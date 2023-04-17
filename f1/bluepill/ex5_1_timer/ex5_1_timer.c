#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include "ex5_1_timer.h"
 
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
        timer_set_prescaler(TIM3, 36-1); //72 Mhz / 36 = 2 Mhz
        timer_set_period(TIM3, 10000-1); // 2 Mhz / 10000 = 200 Hz
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
