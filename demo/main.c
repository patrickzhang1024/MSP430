//#include <msp430.h>
#include "driverlib.h"

#include "Board.h"

#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"

/**
 * main.c
 */
int main(void)
{
    WDT_A_hold(WDT_A_BASE);

    //IO init
    init_clock_IO();

    PMM_unlockLPM5();

    //driver
    init_clock_12Mhz();
    init_uart_IO();
    init_uartA1();

    __bis_SR_register(GIE);

    while(1)
    {

    }
	
	return 0;
}
