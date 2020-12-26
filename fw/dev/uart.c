#include "uart.h"
#include <stdbool.h>
#include <avr/io.h>
#include <util/setbaud.h>

static int uart_putc(char c, FILE *f);

static FILE _uart_out = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

void uart_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

	stdout = &_uart_out;
}

static bool is_tx_ready(void)
{
	return UCSR0A & (1 << UDRE0);
}

static bool is_tx_done(void)
{
	return UCSR0A & (1 << TXC0);
}

static int uart_putc(char c, FILE *f)
{
	(void)f;
	while (!is_tx_ready())
		;
	UDR0 = (uint8_t)c;
	while (!is_tx_done())
		;
	return 0;
}