#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED2 2
#define LED3 4

void led2_task(void *pvParameter)
{
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    uint32_t isOn = 0;
    while (true)
    {
        isOn = !isOn;
        gpio_set_level(LED2, isOn);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 2 second delay
    }
}

void led3_task(void *pvParameter)
{
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
    uint32_t isOn = 0;
    while (true)
    {
        isOn = !isOn;
        gpio_set_level(LED3, isOn);
        vTaskDelay(3000 / portTICK_PERIOD_MS); // 5 second delay
    }
}

void app_main(void)
{

    xTaskCreate(&led2_task, "LED2 Task", 2048, NULL, 5, NULL);
    xTaskCreate(&led3_task, "LED3 Task", 2048, NULL, 5, NULL);
}
