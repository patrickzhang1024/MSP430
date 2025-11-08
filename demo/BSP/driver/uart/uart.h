#ifndef __UART_H__
#define __UART_H__
#include "Board.h"

#ifdef USMART
#define USART_REC_LEN          200

extern uint16_t USART_RX_STA;
extern uint8_t USART_RX_BUF[USART_REC_LEN];
#endif

extern void init_uart_IO();
extern void init_uartA1();

#endif
