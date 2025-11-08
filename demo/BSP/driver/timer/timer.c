#include "driverlib.h"
#include "timer.h"

void init_timerB0_C0()
{
    Timer_B_initContinuousModeParam initContParam = {0};
    initContParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;  //6Mhz
    initContParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_6; //1Mhz
    initContParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initContParam.timerClear = TIMER_B_DO_CLEAR;
    initContParam.startTimer = false;
    Timer_B_initContinuousMode(TIMER_B0_BASE, &initContParam);

    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);

    //channel0 10ms
    Timer_B_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_B_OUTPUTMODE_OUTBITVALUE;
    initCompParam.compareValue = 10000;    //6Mhz SMCL div6, when reach CCIE is 1ms;
    Timer_B_initCompareMode(TIMER_B0_BASE, &initCompParam);

    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_CONTINUOUS_MODE);  //TIMER_B_UP_MODE  //TIMER_B_CONTINUOUS_MODE
}

void init_PWM_IO()
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PWM, GPIO_PIN_PWM, GPIO_FUNCTION_PWM);
}

void init_PWMB0_C1()
{
    //Start timer PWM
    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = 6;  //period =  SMCLK / div / timerperiod = 1Mhz
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);

    //Initialize compare mode to generate PWM
    Timer_B_initCompareModeParam param1 = {0};
    param1.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_1;
    param1.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
    param1.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    param1.compareValue = 1;  //Õ¼¿Õ±È 1/6
    Timer_B_initCompareMode(TIMER_B0_BASE, &param1);

    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_CONTINUOUS_MODE);
}

#ifndef USMART
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_B0_VECTOR)))
#endif
void TIMER0_B0_ISR (void)
{
    uint16_t compVal = Timer_B_getCaptureCompareCount(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0) + 10000;

    //10ms timer

    //Add Offset to CCR0
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, compVal);
}
#endif
