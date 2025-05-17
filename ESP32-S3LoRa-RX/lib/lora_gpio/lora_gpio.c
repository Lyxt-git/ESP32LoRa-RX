#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora_gpio.h"

#define LORA_RST GPIO_NUM_46
#define LORA_SCK GPIO_NUM_12
#define LORA_MISO GPIO_NUM_11
#define LORA_MOSI GPIO_NUM_10
#define LORA_SS GPIO_NUM_9
#define LORA_DIO0 GPIO_NUM_3

void configure_gpio_defaults(void) {
    gpio_set_direction(LORA_SCK, GPIO_MODE_INPUT);
    gpio_set_pull_mode(LORA_SCK, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(LORA_MISO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(LORA_MISO, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(LORA_MOSI, GPIO_MODE_INPUT);
    gpio_set_pull_mode(LORA_MOSI, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(LORA_SS, GPIO_MODE_OUTPUT);
    gpio_set_level(LORA_SS, 1);

    gpio_set_direction(LORA_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(LORA_RST, 1);

    gpio_set_direction(LORA_DIO0, GPIO_MODE_INPUT);
}

void reset_lora_module(void) {
    gpio_set_level(LORA_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LORA_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
}
