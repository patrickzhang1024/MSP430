#include "driverlib.h"
#include "Board.h"

volatile uint32_t clockValue = 0;

void init_clock_IO()
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_XIN, GPIO_PIN_XIN + GPIO_PIN_XOUT, GPIO_FUNCTION_XT1);
}

void init_clock_12Mhz()
{
    CS_setExternalClockSource(CS_XT1_CRYSTAL_FREQUENCY);
    CS_initClockSignal(CS_MCLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_2);
    //CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_2);
    while(CS_turnOnXT1HFWithTimeout(CS_XT1_DRIVE_3, CS_XT1_HFFREQ_6MHZ_16MHZ, CS_XT1_TIMEOUT) != STATUS_SUCCESS) {}

    CS_clearAllOscFlagsWithTimeout(1000);

    clockValue = CS_getMCLK();
}

#define CS_MCLK_DESIRED_FREQUENCY_IN_KHZ   16000  //Target frequency for MCLK in kHz
#define CS_MCLK_FLLREF_RATIO   487                //MCLK/FLLRef Ratio
void init_clockDCO_16Mhz()
{
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);  //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);    //Set ACLK = REFO

    //Create struct variable to store proper software trim values
    CS_initFLLParam param = {0};

    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(CS_MCLK_DESIRED_FREQUENCY_IN_KHZ, CS_MCLK_FLLREF_RATIO, &param);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //For demonstration purpose, change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 487);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //Reload DCO trim values that were calculated earlier
    CS_initFLLLoadTrim(CS_MCLK_DESIRED_FREQUENCY_IN_KHZ, CS_MCLK_FLLREF_RATIO, &param);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);
}

void init_LED_IO()
{
    GPIO_setAsOutputPin(GPIO_PORT_LED_RED, GPIO_PIN_LED_RED);
    GPIO_setAsOutputPin(GPIO_PORT_LED_YELLOW, GPIO_PIN_LED_YELLOW);
    GPIO_setAsOutputPin(GPIO_PORT_LED_GREEN, GPIO_PIN_LED_GREEN);
    GPIO_setAsOutputPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);

    GPIO_setOutputHighOnPin(GPIO_PORT_LED_RED, GPIO_PIN_LED_RED);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_YELLOW, GPIO_PIN_LED_YELLOW);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_GREEN, GPIO_PIN_LED_GREEN);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);
}

//0:close 1:on 2:switch
void ledControl(uint16_t led_type, uint8_t on)
{
    uint8_t tmp_port = 0;
    uint16_t tmp_pin = 0;

    if(on > 1)
    {
        if(led_type == LED_RED || led_type == LED_YELLOW)
        {
            if(P3OUT&led_type)
                on = 1;
            else
                on = 0;
        }
        else {
            if(P1OUT&led_type)
                on = 1;
            else
                on = 0;
        }
    }

    switch(led_type)
    {
        case LED_RED:
            tmp_port = GPIO_PORT_LED_RED;
            tmp_pin = GPIO_PIN_LED_RED;
            break;

        case LED_GREEN:
            tmp_port = GPIO_PORT_LED_GREEN;
            tmp_pin = GPIO_PIN_LED_GREEN;
            break;

        case LED_YELLOW:
            tmp_port = GPIO_PORT_LED_YELLOW;
            tmp_pin = GPIO_PIN_LED_YELLOW;
            break;

        case LED_BLUE:
            tmp_port = GPIO_PORT_LED_BLUE;
            tmp_pin = GPIO_PIN_LED_BLUE;
            break;

        default:
            return;
    }

    if (on)
        GPIO_setOutputLowOnPin(tmp_port, tmp_pin);
    else
        GPIO_setOutputHighOnPin(tmp_port, tmp_pin);
}

