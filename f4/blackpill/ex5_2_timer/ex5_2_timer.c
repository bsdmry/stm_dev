#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "ex5_2_timer.h"

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_96MHZ]);
	rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_TIM3);
}

int main(void)
{
	clock_setup();

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO6);
        gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO6);
        gpio_set_af(GPIOA, GPIO_AF2, GPIO6);

	rcc_periph_reset_pulse(RST_TIM3);
        timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
        timer_continuous_mode(TIM3);
        timer_set_prescaler(TIM3, 48-1); //96 Mhz / 48 = 2 Mhz
        timer_set_period(TIM3, 20000-1); // 2 Mhz / 20000 = 100 Hz

        timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);
        timer_set_oc_value(TIM3, TIM_OC1, 5000-1);
        timer_enable_oc_output(TIM3, TIM_OC1);

        timer_set_counter(TIM3, 0);
        timer_enable_preload(TIM3);
        timer_enable_counter(TIM3);
	while(1);
}
