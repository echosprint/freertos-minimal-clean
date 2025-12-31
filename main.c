/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * This is a simple main that will start the FreeRTOS-Kernel and run a periodic task
 * that only delays if compiled with the template port, this project will do nothing.
 * For more information on getting started please look here:
 * https://freertos.org/FreeRTOS-quick-start-guide.html
 */

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

/* Standard includes. */
#include <stdio.h>

/*-----------------------------------------------------------*/

/* Timer period in ms */
#define TIMER_PERIOD_MS    1000

/* Function prototypes */
static void exampleTask( void * parameters );
static void vTimerCallback( TimerHandle_t xTimer );

/* Timer handle */
static TimerHandle_t xTimer = NULL;

/*-----------------------------------------------------------*/

static void exampleTask( void * parameters )
{
    /* Unused parameters. */
    ( void ) parameters;

    printf( "Task: Started - will print every 100 ticks\n" );

    for( ; ; )
    {
        /* Example Task Code */
        printf( "Task: Running...\n" );
        vTaskDelay( 100 ); /* delay 100 ticks */
    }
}

/*-----------------------------------------------------------*/

static void vTimerCallback( TimerHandle_t xTimer )
{
    static uint32_t ulCount = 0;

    /* Unused parameter */
    ( void ) xTimer;

    /* This callback is called when the timer expires */
    ulCount++;
    printf( "Timer: Callback executed (count = %lu)\n", ( unsigned long ) ulCount );
}

int main( void )
{
    static StaticTask_t exampleTaskTCB;
    static StackType_t exampleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Initialize semihosting */
    extern void initialise_monitor_handles(void);
    initialise_monitor_handles();

    printf("===========================================\n");
    printf("Minimal FreeRTOS Example with Timer\n");
    printf("Target: ARM Cortex-M3\n");
    printf("===========================================\n\n");

    /* Create the example task */
    ( void ) xTaskCreateStatic( exampleTask,
                                "Task",
                                configMINIMAL_STACK_SIZE,
                                NULL,
                                configMAX_PRIORITIES - 1U,
                                &( exampleTaskStack[ 0 ] ),
                                &( exampleTaskTCB ) );

    /* Create a software timer that calls vTimerCallback every 1000ms */
    xTimer = xTimerCreate( "Timer",
                          pdMS_TO_TICKS( TIMER_PERIOD_MS ),
                          pdTRUE,  /* Auto-reload */
                          0,       /* Timer ID */
                          vTimerCallback );

    if( xTimer != NULL )
    {
        /* Start the timer */
        if( xTimerStart( xTimer, 0 ) == pdPASS )
        {
            printf( "Timer: Created and started (1000 ms period)\n\n" );
        }
    }

    printf( "Starting FreeRTOS scheduler...\n\n" );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Should never reach here */
    for( ; ; )
    {
    }

    return 0;
}
/*-----------------------------------------------------------*/

/* Hook functions are provided by the kernel when
 * configKERNEL_PROVIDED_STATIC_MEMORY = 1 */
