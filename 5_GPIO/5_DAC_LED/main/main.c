#include <stdio.h>
#include "driver/gpio.h"
#include "driver/dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DAC_CHANNEL DAC_CHANNEL_1
#define DELAY_MS 100

void app_main()
{
    dac_output_enable(DAC_CHANNEL); // Enable DAC channel
    int brightness = 0;
    int step = 10; // Step size for changing brightness

    while (1)
    {
        // Increase brightness from 0 to 255
        for (brightness = 0; brightness <= 255; brightness += step)
        {
            dac_output_voltage(DAC_CHANNEL, brightness);
            vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
        }

        // Decrease brightness from 255 to 0
        for (brightness = 255; brightness >= 0; brightness -= step)
        {
            dac_output_voltage(DAC_CHANNEL, brightness);
            vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
        }
    }
}
