#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include "ex4_i2c.h"

#define LCD1602_ADDR 0x3F
#define LCD1602_BACKLIGHT 0x08
#define LCD1602_ENABLE 0x04
#define LCD1602_WIDTH 20   // 20 символов на линию
#define LCD1602_CHR  1 // Режим отправки данных
#define LCD1602_CMD  0 // Режим отправки команд
#define LCD1602_LINE_1  0x80 // LCD RAM адрес первой строки
#define LCD1602_LINE_2  0xC0 // LCD RAM адрес второй строки

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
        rcc_periph_clock_enable(RCC_AFIO);
        rcc_periph_clock_enable(RCC_I2C1);
}

static void lcd1602_write_byte(uint32_t i2c, uint8_t byte, uint8_t flag){
        uint8_t transaction[6] = {0};
        transaction[0] = (flag | (byte & 0xF0) | LCD1602_BACKLIGHT);
        transaction[1] = ((flag | (byte & 0xF0) | LCD1602_BACKLIGHT) | LCD1602_ENABLE);
        transaction[2] = ((flag | (byte & 0xF0) | LCD1602_BACKLIGHT) & ~LCD1602_ENABLE);

        transaction[3] = (flag | ((byte<<4) & 0xF0) | LCD1602_BACKLIGHT);
        transaction[4] = ((flag | ((byte<<4) & 0xF0) | LCD1602_BACKLIGHT) | LCD1602_ENABLE );
        transaction[5] = ((flag | ((byte<<4) & 0xF0) | LCD1602_BACKLIGHT) & ~LCD1602_ENABLE);

        for(uint8_t i = 0; i < 6; i++){
                i2c_transfer7(i2c, LCD1602_ADDR, &transaction[i], 1, NULL, 0);
        }
}

static void lcd1602_init(uint32_t i2c){
        lcd1602_write_byte(i2c, 0x33, LCD1602_CMD); // 110011 Initialise
        lcd1602_write_byte(i2c, 0x32, LCD1602_CMD); // 110010 Initialise
        lcd1602_write_byte(i2c, 0x06, LCD1602_CMD); // 000110 Cursor move direction
        lcd1602_write_byte(i2c, 0x0C, LCD1602_CMD); // 001100 Display On,Cursor Off, Blink Off
        lcd1602_write_byte(i2c, 0x28, LCD1602_CMD); // 101000 Data length, number of lines, font size
        lcd1602_write_byte(i2c, 0x01, LCD1602_CMD); // 000001 Clear display
}

int main(void)
{
	clock_setup();
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ , GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN , GPIO8 | GPIO9);
        gpio_primary_remap(AFIO_MAPR_SWJ_CFG_FULL_SWJ, AFIO_MAPR_I2C1_REMAP);

        i2c_peripheral_disable(I2C1);
        i2c_set_own_7bit_slave_address(I2C1, 0x00);
        i2c_set_speed(I2C1, i2c_speed_sm_100k, rcc_apb1_frequency/1000000);
        i2c_peripheral_enable(I2C1);

	lcd1602_init(I2C1);
        lcd1602_write_byte(I2C1, ((LCD1602_LINE_1) | ((0x00) & 0x0f)), LCD1602_CMD);

	const char msg[15] = "Hi!";
        lcd1602_write_byte(I2C1, (uint8_t)msg[0], LCD1602_CHR);
        lcd1602_write_byte(I2C1, (uint8_t)msg[1], LCD1602_CHR);
        lcd1602_write_byte(I2C1, (uint8_t)msg[2], LCD1602_CHR);
	while(1){
	
	}
}
