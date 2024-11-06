#include "FreeRTOS.h"
#include "task.h"

#include "pico/flash.h"

void vFlash( void * pvArg )
{
}

uint32_t run_count;

void vTask( void * pvArg )
{
    for( ; ; )
    {
        const int xResult = flash_safe_execute( vFlash, NULL, 1000 );
        configASSERT( xResult == PICO_OK );
        run_count++;
        sleep_ms(1);
    }
}

int main( void )
{
    const BaseType_t xResult = xTaskCreate( vTask, "vTask",
                                            configMINIMAL_STACK_SIZE, NULL, 1,
                                            NULL );
    configASSERT( xResult == pdPASS );

    vTaskStartScheduler();
    hard_assert(0);

    return 0;
}
