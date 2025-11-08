#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdio.h>
#include <stdint.h>

//this is 32Pin VQFN MSP430FR2155

#if (defined(__MSP430FR2155__) || defined(__MSP430FR2355__))

/************  function ************/
//#define SPI_FUNCTION
#define REDEFINE_PRINTF
#define USMART

/************  Clock  ************/
#define CS_XT1_CRYSTAL_FREQUENCY      12000000
#define MCLK_FRQ                      12000000
#define CS_XT1_TIMEOUT                50000
#define delay_ms(ms)                  __delay_cycles(((long)MCLK_FRQ * (double)ms / 1000))
#define delay_us(us)                  __delay_cycles(((long)MCLK_FRQ * (double)us / 1000000))

#define GPIO_FUNCTION_XT1             GPIO_SECONDARY_MODULE_FUNCTION

#define GPIO_PORT_XIN                 GPIO_PORT_P2
#define GPIO_PIN_XIN                  GPIO_PIN6

#define GPIO_PORT_XOUT                GPIO_PORT_P2
#define GPIO_PIN_XOUT                 GPIO_PIN7


/************  I2C  ************/
#define GPIO_FUNCTION_I2C            GPIO_PRIMARY_MODULE_FUNCTION  //eUSCI_B0
#define GPIO_PORT_SDA                GPIO_PORT_P1
#define GPIO_PIN_SDA                 GPIO_PIN2

#define GPIO_PORT_SCL                GPIO_PORT_P1
#define GPIO_PIN_SCL                 GPIO_PIN3

#define GPIO_PORT_INT                GPIO_PORT_P3
#define GPIO_PIN_INT                 GPIO_PIN0


/************  Uart  ************/
#ifndef SPI_FUNCTION
#define GPIO_FUNCTION_UART           GPIO_PRIMARY_MODULE_FUNCTION //eUSCI_A1
#define GPIO_PORT_RXD                GPIO_PORT_P4
#define GPIO_PIN_RXD                 GPIO_PIN2

#define GPIO_PORT_TXD                GPIO_PORT_P4
#define GPIO_PIN_TXD                 GPIO_PIN3
#endif


/************  SPI  ************/
#ifdef SPI_FUNCTION
#define GPIO_PORT_UCA1STE            GPIO_PORT_P4  //eUSCI_A1
#define GPIO_PIN_UCA1STE             GPIO_PIN0
#define GPIO_FUNCTION_UCA1STE        GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCA1SCL            GPIO_PORT_P4
#define GPIO_PIN_UCA1SCL             GPIO_PIN1
#define GPIO_FUNCTION_UCA1SCL        GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCA1SOMI           GPIO_PORT_P4
#define GPIO_PIN_UCA1SOMI            GPIO_PIN2
#define GPIO_FUNCTION_UCA1SOMI       GPIO_PRIMARY_MODULE_FUNCTION

#define GPIO_PORT_UCA1SIMO           GPIO_PORT_P4
#define GPIO_PIN_UCA1SIMO            GPIO_PIN3
#define GPIO_FUNCTION_UCA1SIMO       GPIO_PRIMARY_MODULE_FUNCTION
#endif


/************  PWM  ************/
#define GPIO_FUNCTION_PWM             GPIO_SECONDARY_MODULE_FUNCTION  //TB0.1
#define GPIO_PORT_PWM                 GPIO_PORT_P1
#define GPIO_PIN_PWM                  GPIO_PIN6


/************  LED  ************/
#define GPIO_PORT_LED_RED             GPIO_PORT_P3
#define GPIO_PIN_LED_RED              GPIO_PIN6

#define GPIO_PORT_LED_YELLOW          GPIO_PORT_P3
#define GPIO_PIN_LED_YELLOW           GPIO_PIN7

#define GPIO_PORT_LED_GREEN           GPIO_PORT_P1
#define GPIO_PIN_LED_GREEN            GPIO_PIN4

#define GPIO_PORT_LED_BLUE            GPIO_PORT_P1
#define GPIO_PIN_LED_BLUE             GPIO_PIN5

typedef enum{
    LED_RED = (1<<6),
    LED_YELLOW = (1<<7),
    LED_GREEN = (1<<4),
    LED_BLUE = (1<<5),
}LED_TYPE;

typedef enum {
  LED_OFF = 0,
  LED_ON,
  LED_SWITCH,
}LED_STATE;




extern volatile uint32_t clockValue;

/************  function  ************/
extern void init_clock_IO();
extern void init_clock_12Mhz();
extern void init_clockDCO_16Mhz();

extern void init_LED_IO();
extern void ledControl(uint16_t led_type, uint8_t on);

#endif //__MSP430FR2155__ __MSP430FR2355__

#endif
