#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>

#ifndef BAUD
#	define BAUD 19200
#endif

void uart_init(void);

#endif/*__UART_H__*/