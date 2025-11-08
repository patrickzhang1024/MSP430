#include "driverlib.h"
#include "Board.h"
#include "spi.h"

#ifdef SPI_FUNCTION
void init_SPI_IO()
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCA1STE, GPIO_PIN_UCA1STE, GPIO_FUNCTION_UCA1STE);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCA1SCL, GPIO_PIN_UCA1SCL, GPIO_FUNCTION_UCA1SCL);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_UCA1SOMI, GPIO_PIN_UCA1SOMI, GPIO_FUNCTION_UCA1SOMI);    //SOMI slave out
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCA1SIMO, GPIO_PIN_UCA1SIMO, GPIO_FUNCTION_UCA1SIMO);   //SIMO slave in
}

void init_SPIA1_Slave()
{
    EUSCI_A_SPI_initSlaveParam param = {0};
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH;
    EUSCI_A_SPI_initSlave(EUSCI_A1_BASE, &param);
    EUSCI_A_SPI_select4PinFunctionality(EUSCI_A1_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);

    EUSCI_A_SPI_enable(EUSCI_A1_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
}

void init_SPIA1_Master()
{
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = CS_getSMCLK();
    param.desiredSpiClock = 500000;   //500Khz
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    param.spiMode = EUSCI_A_SPI_3PIN;  //EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW
    EUSCI_A_SPI_initMaster(EUSCI_A1_BASE, &param);

    EUSCI_A_SPI_enable(EUSCI_A1_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
}

uint8_t SPI_ReceiveRegAddr = 0;
uint8_t uca1_rx_val = 0;

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void USCI_A1_ISR (void)
{
    switch(__even_in_range(UCA1IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            //Receive data from master
            uca1_rx_val = EUSCI_A_SPI_receiveData(EUSCI_A1_BASE);
            switch(SPI_ReceiveRegAddr)
            {
                case (0xB1):
                    //get_data = uca1_rx_val;
                    SPI_ReceiveRegAddr = 0;
                    break;
                default:
                    break;
            }

            switch(uca1_rx_val)
            {
                case (0xA1):
                   //EUSCI_A_SPI_transmitData(EUSCI_A1_BASE, send_data);
                   return;
               case (0xA2):
                   //EUSCI_A_SPI_transmitData(EUSCI_A1_BASE, send_data);
                   return;
               default:
                   EUSCI_A_SPI_transmitData(EUSCI_A1_BASE, 0);
                   SPI_ReceiveRegAddr = uca1_rx_val;
                   break;
            }
            break;

        case USCI_SPI_UCTXIFG:      // UCTXIFG
            break;

        default:
            __no_operation();
            break;
    }
}
#endif
