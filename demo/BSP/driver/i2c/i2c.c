#include "driverlib.h"
#include "Board.h"
#include "i2c.h"

uint32_t busy_timeout = 0;
uint32_t I2C_BUS = EUSCI_B0_BASE;

void init_I2C_IO()
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_SCL, GPIO_PIN_SCL, GPIO_FUNCTION_I2C);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_SDA, GPIO_PIN_SDA, GPIO_FUNCTION_I2C);
}

void init_I2CB0_Master()
{
   //I2C Master
   EUSCI_B_I2C_disable(EUSCI_B0_BASE);
   EUSCI_B_I2C_initMasterParam param = {0};
   param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
   param.i2cClk = CS_getSMCLK();
   param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
   param.byteCounterThreshold = 1;
   param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
   EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

   //Specify slave address
   EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

   EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
   EUSCI_B_I2C_enable(EUSCI_B0_BASE);

   EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0 + EUSCI_B_I2C_STOP_INTERRUPT);
}

int32_t I2C_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
    while(HWREG16(I2C_BUS + OFS_UCBxSTATW) & UCBBUSY)
    {
        busy_timeout++;
        if(busy_timeout >= 12000)
        {
            //Send stop condition.
            HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTXSTP;
            while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTP) ; //等STP发送
            break;
        }
    }

    busy_timeout = 0;

    //Send start condition.
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTR + UCTXSTT;

    while (!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCTXIFG)) ; //等Txbuf空
    while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTT) ; //等STT发送

    HWREG16(I2C_BUS + OFS_UCBxTXBUF) = reg;
    while (!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCTXIFG)) ;

    uint32_t i = 0;
    while(len)
    {
        HWREG16(I2C_BUS + OFS_UCBxTXBUF) = bufp[i];
        while (!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCTXIFG)) ;
        i++;
        len--;
    }

    //Send stop condition.
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTXSTP;
    while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTP) ; //等STP发送

  return 0;
}

int32_t I2C_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    while(HWREG16(I2C_BUS + OFS_UCBxSTATW) & UCBBUSY)
    {
        busy_timeout++;
        if(busy_timeout >= 12000)
        {
            //Send stop condition.
            HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTXSTP;
            while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTP) ; //等STP发送
            break;
        }
    }

    busy_timeout = 0;

    //Send start condition.
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTR + UCTXSTT;

    while (!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCTXIFG)) ; //等Txbuf空
    while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTT) ; //等STT发送

    HWREG16(I2C_BUS + OFS_UCBxTXBUF) = reg;
    while (!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCTXIFG)) ;

    //Send start condition rx
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) &= ~UCTR;
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTXSTT;
    while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTT) ; //等STT发送

    uint32_t i = 0;
    while(len)
    {
        while(!(HWREG16(I2C_BUS + OFS_UCBxIFG) & UCRXIFG0));
        bufp[i] = (HWREG16(I2C_BUS + OFS_UCBxRXBUF));
        i++;
        len--;
    }

    //Send stop condition.
    HWREG16(I2C_BUS + OFS_UCBxCTLW0) |= UCTXSTP;
    while (HWREG16(I2C_BUS + OFS_UCBxCTLW0) & UCTXSTP) ; //等STP发送

    return 0;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
    {
        case USCI_NONE:             // No interrupts break;
            break;
        case USCI_I2C_UCALIFG:      // Arbitration lost
            break;
        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
            break;
        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
            break;
        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
            break;
        case USCI_I2C_UCRXIFG3:     // RXIFG3
            break;
        case USCI_I2C_UCTXIFG3:     // TXIFG3
            break;
        case USCI_I2C_UCRXIFG2:     // RXIFG2
            break;
        case USCI_I2C_UCTXIFG2:     // TXIFG2
            break;
        case USCI_I2C_UCRXIFG1:     // RXIFG1
            break;
        case USCI_I2C_UCTXIFG1:     // TXIFG1
            break;

        case USCI_I2C_UCRXIFG0:     // RXIFG0
            break;
        case USCI_I2C_UCTXIFG0:     // TXIFG0
            break;

        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
            break;
        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
            break;
        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
            break;
        default:
            break;
    }
}
