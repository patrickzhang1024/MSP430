//#include <msp430.h>
#include "driverlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* Hardware includes. */
#include "Board.h"

/* Standard demo includes. */
//#include "ParTest.h"
//#include "dynamic.h"
//#include "comtest2.h"
//#include "GenQTest.h"
//#include "TimerDemo.h"
//#include "countsem.h"

#define mainCHECK_TIMER_PERIOD          ( 1000UL / ( unsigned long ) portTICK_PERIOD_MS )

//timer
static TimerHandle_t xCheckTimer = NULL;
static void vCheckTimerCallback( TimerHandle_t xTimer );

//start task
/*#define START_TASK_PRIO     1
#define START_STK_SIZE      128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);*/

//led task
#define LED_TASK_PRIO     2
#define LED_STK_SIZE      128
TaskHandle_t LEDTask_Handler;
void LED_task(void *pvParameters);

/**
 * main.c
 */
void main(void)
{
    taskDISABLE_INTERRUPTS();
    WDT_A_hold(WDT_A_BASE);

    //IO_init
    init_IO_MISC();

    PMM_unlockLPM5();

    //sys clock
    init_clock_12Mhz();

    //driver init
    //init_timerB();

    /*xTaskCreate((TaskFunction_t )start_task,
                (const char*    )"start_task",
                (uint16_t       )START_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )START_TASK_PRIO,
                (TaskHandle_t*  )&StartTask_Handler);*/

    xTaskCreate((TaskFunction_t )LED_task,
                 (const char*    )"LED_task",
                 (uint16_t       )LED_STK_SIZE,
                 (void*          )NULL,
                 (UBaseType_t    )LED_TASK_PRIO,
                 (TaskHandle_t*  )&LEDTask_Handler);

    xCheckTimer = xTimerCreate( "Check timer", mainCHECK_TIMER_PERIOD, pdTRUE, ( void * ) 0, vCheckTimerCallback);
    if(xCheckTimer != NULL)
       xTimerStart( xCheckTimer, portMAX_DELAY );

    vTaskStartScheduler();
    while(1)
    {

    }

}

/*void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t )LED_task,
                (const char*    )"LED_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);

    if(xCheckTimer != NULL)
        xTimerStart( xCheckTimer, portMAX_DELAY );

    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();
}*/

void LED_task(void *pvParameters)
{
    uint8_t timecount = 0;
    int32_t test_number = -1234;
    while(1)
    {
        ledControl(LED_RED, LED_SWITCH);
        // GPIO_setOutputHighOnPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
        //GPIO_setOutputHighOnPin(GPIO_PORT_SDA, GPIO_PIN_SDA);
        vTaskDelay(500);
        //GPIO_setOutputLowOnPin(GPIO_PORT_SCL, GPIO_PIN_SCL);
        //GPIO_setOutputLowOnPin(GPIO_PORT_SDA, GPIO_PIN_SDA);

        if(timecount < 2)
            timecount++;
        else
            GPIO_setOutputHighOnPin(GPIO_PORT_LED_BLUE, GPIO_PIN_LED_BLUE);
    }
}

//like while(1) in rtos
static void vCheckTimerCallback( TimerHandle_t xTimer )
{
    //ledControl(LED_BLUE, LED_SWITCH);
    ledControl(LED_GREEN, LED_SWITCH);
}



/*******************  rtos system   *************************/
void vApplicationSetupTimerInterrupt( void )
{
#ifdef DEFAULT_TICK
    const unsigned short usSMCLK_Frequency_Hz = 6000000;

    /* Ensure the timer is stopped. */
    TB0CTL = 0;

    /* Run the timer from the SMCLK. */
    TB0CTL = TBSSEL_2;

    /* Clear everything to start with. */
    TB0CTL |= TBCLR;

    /* Set the compare match value according to the tick rate we want. */
    TB0CCR0 = (usSMCLK_Frequency_Hz / configTICK_RATE_HZ);

    /* Enable the interrupts. */
    TB0CCTL0 = CCIE;

    /* Start up clean. */
    TB0CTL |= TBCLR;

    /* Up mode. */
    TB0CTL |= MC_1;
#elif defined NORMAL_IRQ
    uint16_t compVal = Timer_B_getCaptureCompareCount(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0) + 1000;

    //Add Offset to CCR0
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, compVal);
#endif

    init_timerB();
}

/*void vApplicationTickHook( void )
{

}

void vApplicationIdleHook( void )
{
    /* Called on each iteration of the idle task.  In this case the idle task
    just enters a low(ish) power mode. */
    //__bis_SR_register( LPM1_bits + GIE );

    /*__bis_SR_register( LPM4_bits + GIE );
    __no_operation();
}*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues or
    semaphores. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
