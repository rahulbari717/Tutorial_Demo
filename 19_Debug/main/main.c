#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

void random_num()
{
    printf("\n Random num Genrator function");
    for (int i = 0; i < 30; i++)
    {
        int num1 = rand() % 5;
        int known = 100;
        printf("Result is %d\n", known / num1);
    }
}
void callingFunk()
{
    // printf("in func 1 \n");
    char buffer[30];
    // printf("in func 2 \n");
    memset(buffer, 'x', sizeof(buffer));
    // printf("in func 3 \n");
    printf("buffer is %s\n", buffer);

    random_num();
    // printf("in func 4 \n");
}

void task1(void *params)
{
    // printf("before func \n");
    callingFunk();
    // printf("after func \n");
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(&task1, "task1", 1024 * 2, NULL, 5, NULL);
}