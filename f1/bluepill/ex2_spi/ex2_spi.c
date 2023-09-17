#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "ex2_spi.h"

#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_POWER        0x0C
#define MAX7219_MODE_TEST         0x0F
#define MAX7219_MODE_NOOP         0x00

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
   	rcc_periph_clock_enable(RCC_SPI2);
}

static void spi_setup(void){
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12); //CS
	gpio_set(GPIOB, GPIO12);
  	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO15 ); //SCK, MOSI
  	
	//gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12 | GPIO13 | GPIO15 ); //SCK, MOSI
	//gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14); //MISO

  	spi_disable(SPI2);
  	spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_256, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_16BIT, SPI_CR1_MSBFIRST);
	spi_enable_software_slave_management(SPI2);
	spi_set_nss_high(SPI2);
	spi_enable(SPI2);

	//spi_disable_software_slave_management(SPI2);
	//spi_enable_ss_output(SPI2);
  }

static void spi_transmit(uint16_t data){
//	spi_enable(SPI2);
	gpio_clear(GPIOB, GPIO12);
	spi_xfer(SPI2, data);
	gpio_set(GPIOB, GPIO12);
//	spi_disable(SPI2);	
}

static void max7219_send(uint8_t cmd, uint8_t data){
	spi_transmit(((uint16_t)cmd << 8) | data);
}

int main(void)
{
	clock_setup();
	spi_setup();
	max7219_send(MAX7219_MODE_DECODE, 0xFF);
	max7219_send(MAX7219_MODE_SCAN_LIMIT, 3);
	max7219_send(MAX7219_MODE_INTENSITY, 2);
	max7219_send(MAX7219_MODE_POWER, 1);
	max7219_send(0x1, 4);
	max7219_send(0x2, 3);
	max7219_send(0x3, 2);
	max7219_send(0x4, 1);

	while(1){
	}
}
