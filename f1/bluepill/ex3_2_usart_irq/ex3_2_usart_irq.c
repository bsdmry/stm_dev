#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/dwt.h>

#include "ex3_2_usart_irq.h"
#define BUF_SIZE 128

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_AFIO);
        rcc_periph_clock_enable(RCC_USART2);
}


uint8_t tx_buffer[BUF_SIZE] = {};
uint8_t rx_buffer[BUF_SIZE] = {};

typedef struct {
  uint8_t *data;
  uint32_t size;
  uint32_t head;
  uint32_t tail;
  uint32_t length;
} rb;

typedef struct {
        uint8_t cmd_code;
        uint8_t assembling;
} cmd;

volatile rb tx_rb = {
  .data = tx_buffer,
  .size = sizeof(tx_buffer),
  .head = 0,
  .tail = 0,
  .length = 0
};

volatile rb rx_rb = {
  .data = rx_buffer,
  .size = sizeof(rx_buffer),
  .head = 0,
  .tail = 0,
  .length = 0
};

cmd cli_cmd = {
  .cmd_code = 0,
  .assembling =0
};


static uint8_t rb_push(volatile rb *ring, const uint8_t data)
{
  if (((ring->tail + 1) % ring->size) != ring->head)
  {
    ring->data[ring->tail++] = data;
    ring->tail %= ring->size;
    ring->length++;
    return 1;
  }
  return 0;
}

static uint8_t rb_pop(volatile rb *ring, uint8_t *data)
{
  if (ring->head != ring->tail)
  {
    *data = ring->data[ring->head++];
    ring->head %= ring->size;
    ring->length--;
    return 1;
  }
  return 0;
}

static void usart_transmit(volatile rb *ring, const char *str){
        while (*str != '\000') {
                rb_push(ring, (uint8_t)*str);
                str++;
        }
        usart_enable_tx_interrupt(USART2);
}


static void parse_cmd(volatile rb *ring, cmd *cli){
        uint8_t data = 0;
        while(rb_pop(ring, &data)){
                if(data == 0x0D){ // \r char
                        cli->assembling = 0;
                        break;
                } else {
                                cli->assembling = 1;
                                cli->cmd_code ^= data;
                }
        }
        if (!cli->assembling){
                switch(cli->cmd_code){
                        case 0x20: // "toggle_led" cmd
                                gpio_toggle(GPIOC, GPIO13);
                                break;
                        case 0x56: // "say_hello" cmd
                                usart_transmit(&tx_rb, "Hello from LibOpenCm3!\n");
                                break;
                        default:
                                break;
                }
                cli->cmd_code = 0;
                cli->assembling = 1;
        }
}

static uint32_t  dwt_setup(void)
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

static void dwt_delay_us(uint32_t microseconds)
{
        uint32_t initial_ticks = dwt_read_cycle_counter();
        uint32_t us_count_tics = microseconds * (rcc_ahb_frequency / 1000000);
        while ((dwt_read_cycle_counter() - initial_ticks) < us_count_tics);
}


int main(void)
{
	clock_setup();
	dwt_setup();
	nvic_enable_irq(NVIC_USART2_IRQ);
	nvic_set_priority(NVIC_USART2_IRQ, 1); //Set USART IRQ priority.
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO3);

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
        gpio_set(GPIOC, GPIO13);
 
 
        usart_set_baudrate(USART2, 9600);
        usart_set_databits(USART2, 8);
        usart_set_stopbits(USART2, USART_STOPBITS_1);
        usart_set_parity(USART2, USART_PARITY_NONE);
        usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
        usart_set_mode(USART2, USART_MODE_TX_RX);
        usart_enable_rx_interrupt(USART2);
        usart_enable(USART2);

	usart_transmit(&tx_rb, "Startup\r\n");

	while(1){
		dwt_delay_us(1000);
                if(rx_rb.length > 0){
                        parse_cmd(&rx_rb, &cli_cmd);
                }
	}
}

void usart2_isr(void){
	        uint8_t data = 0;
        if (usart_get_flag(USART2, USART_SR_TXE)) {
                if(rb_pop(&tx_rb, &data)){
                        usart_send(USART2, data);
                } else {
                        usart_disable_tx_interrupt(USART2);
                }
        }
 
        if (usart_get_flag(USART2, USART_SR_RXNE)) {
                data = (uint8_t)usart_recv(USART2);
                rb_push(&rx_rb, data);
                if (data == 0x0D){
                        usart_send(USART2, 0x0A);
                } else {
                        usart_send(USART2, data);
                }
        }
}
