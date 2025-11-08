#include "driverlib.h"
#include "Board.h"

volatile uint32_t clockValue = 0;

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

void init_IO_MISC()
{
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_XIN, GPIO_PIN_XIN + GPIO_PIN_XOUT, GPIO_FUNCTION_XT1);

    GPIO_setAsOutputPin(GPIO_PORT_LED_RED, GPIO_PIN_LED_RED);
    GPIO_setAsOutputPin(GPIO_PORT_LED_YELLOW, GPIO_PIN_LED_YELLOW);
    GPIO_setAsOutputPin(GPIO_PORT_LED_GREEN, GPIO_PIN_LED_GREEN);
    GPIO_setAsOutputPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);

    GPIO_setOutputHighOnPin(GPIO_PORT_LED_RED, GPIO_PIN_LED_RED);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_YELLOW, GPIO_PIN_LED_YELLOW);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_GREEN, GPIO_PIN_LED_GREEN);
    GPIO_setOutputHighOnPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_BEEP, GPIO_PIN_BEEP, GPIO_FUNCTION_BEEP);

    GPIO_setAsInputPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
    GPIO_setAsInputPin(GPIO_PORT_SDA, GPIO_PIN_SDA);
    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_SCL, GPIO_PIN_SCL);
    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_SDA, GPIO_PIN_SDA);
    GPIO_setOutputLowOnPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);
    /*GPIO_setAsOutputPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
    GPIO_setAsOutputPin(GPIO_PORT_SDA, GPIO_PIN_SDA);

    GPIO_setOutputHighOnPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
    GPIO_setOutputHighOnPin(GPIO_PORT_SDA, GPIO_PIN_SDA);

    delay_ms(1);

    GPIO_setOutputLowOnPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
    GPIO_setOutputLowOnPin(GPIO_PORT_SDA, GPIO_PIN_SDA);

    GPIO_setAsInputPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
    GPIO_setAsInputPin(GPIO_PORT_SDA, GPIO_PIN_SDA);

    /*GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_TXD, GPIO_PIN_TXD, GPIO_FUNCTION_UART);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_RXD, GPIO_PIN_RXD, GPIO_FUNCTION_UART);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SCL, GPIO_PIN_SCL, GPIO_FUNCTION_I2C);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_SDA, GPIO_PIN_SDA, GPIO_FUNCTION_I2C);
    GPIO_setAsOutputPin(GPIO_PORT_INT, GPIO_PIN_INT);
    GPIO_setOutputLowOnPin(GPIO_PORT_INT, GPIO_PIN_INT);*/
}

void init_timerB()
{
    Timer_B_initContinuousModeParam initContParam = {0};
    initContParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;  //6Mhz
    initContParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_6; //1Mhz
    initContParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initContParam.timerClear = TIMER_B_DO_CLEAR;
    initContParam.startTimer = false;
    Timer_B_initContinuousMode(TIMER_B0_BASE, &initContParam);

    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);

    //channel0 10ms for keys
    Timer_B_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_B_OUTPUTMODE_OUTBITVALUE;
    initCompParam.compareValue = 1000;    //6Mhz SMCL div 6, when reach CCIE is 1ms;  //不知道为什么莫名其妙时钟快2倍
    Timer_B_initCompareMode(TIMER_B0_BASE, &initCompParam);

    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE);  //TIMER_B_UP_MODE  //TIMER_B_CONTINUOUS_MODE
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

