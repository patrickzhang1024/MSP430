#include "driverlib.h"
#include "uart.h"

#ifdef REDEFINE_PRINTF
    #pragma import(__use_no_semihosting)
    struct __FILE
    {
        int handle;
    };

    FILE __stdout;
    void _sys_exit(int x)
    {
        x = x;
    }

    /*int fputc(int ch, FILE *f)
    {
        //while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)) {}
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, (uint8_t)ch);
        return ch;
    }*/

    int fputc(int ch,FILE *f)
    {
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE,(uint8_t)ch);
        while(EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == UCTXIFG);
        return ch;
    }

    int fputs(const char *_ptr,register FILE *_fp)
    {
        uint16_t i,len;
        len = strlen(_ptr);
        for(i=0;i<len;i++)
        {
            EUSCI_A_UART_transmitData(EUSCI_A1_BASE, (unsigned char)_ptr[i]);
            while(EUSCI_A_UART_getInterruptStatus(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == UCTXIFG);
        }
        return len;
    }
#endif

#ifdef UART_LOOPBUF
    #include <string.h>
    #include "crc.h"
    #include "trans_loopbuffer.h"

    typedef struct {
        uint8_t head;
        uint8_t serial;
        uint16_t cmd;
        uint8_t* data;
        uint16_t crc;
    }packetStruct;
    packetStruct read_packet = {0};

    loopStructure uart_struct = {0};

    static void loop_init()
    {
        uart_struct.buffersize = PACKET_MAX_SIZE;
        uart_struct.buffer = uart_rx_buf;
        uart_struct.write_index = 0;
        uart_struct.read_index = 0;
        uart_struct.overflow_flag = 0;
        uart_struct.handled_flag = 1;
    }
#endif

#ifdef USMART
    uint8_t USART_RX_BUF[USART_REC_LEN];

    uint16_t USART_RX_STA = 0;
    uint8_t aRxBuffer;

    void usmart_read_byte()
    {
        if((USART_RX_STA & 0x8000) == 0)//接收未完成
        {
            if(USART_RX_STA & 0x4000)//接收到了0xd
            {
                if(aRxBuffer != 0x0a) USART_RX_STA = 0;//接受错误，重新开始
                else USART_RX_STA |= 0x8000;  //接受完成了
            }
            else //还未接受到0xd
            {
                if(aRxBuffer == 0x0d) USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer;
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;//接受数据错误，重新接受
                }
            }
        }
    }
#endif


void init_uart_IO()
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_TXD, GPIO_PIN_TXD, GPIO_FUNCTION_UART);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_RXD, GPIO_PIN_RXD, GPIO_FUNCTION_UART);
}

void init_uartA1()
{
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK; //6Mhz
    param.clockPrescalar = 3; //UCBRx  INT(N) = CLK/115200
    param.firstModReg = 4;    //UCBRFx  OS16开启时使用 INT([(N/16) – INT(N/16)] × 16)
    param.secondModReg = 0x04;  //UCBRSx  6Mhz/115200 = 52.0833 对照表格小数位0x04
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;  //OS16
    while (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param))
    {
        //err
    }

    EUSCI_A_UART_enable(EUSCI_A1_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

#ifndef SPI_FUNCTION
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void EUSCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
#ifdef UART_LOOPBUF
            writeBuffer(&uart_struct, EUSCI_A_UART_receiveData(EUSCI_A1_BASE));
#elif defined USMART
            aRxBuffer = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
            usmart_read_byte();
#endif
            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
    }
}
#endif

