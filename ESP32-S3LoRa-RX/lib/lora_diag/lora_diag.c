#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lora_diag.h"

#define TAG "GPIO_DIAG"

static void check_gpio(gpio_num_t pin, const char *name) {
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    vTaskDelay(pdMS_TO_TICKS(10));
    int level = gpio_get_level(pin);
    if (level == 0) {
        ESP_LOGI(TAG, "%s pin is LOW (expected).", name);
    } else {
        ESP_LOGW(TAG, "%s pin is HIGH (unexpected).", name);
    }
}

void diagnostic_gpio(void) {
    printf("\n");
    printf("ESP32-S3LORA-RX: Performing GPIO Diagnostic ");
    fflush(stdout);
    for (int i = 0; i < 7; i++) {
        printf(".");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("\n");

    check_gpio(GPIO_NUM_12, "SCK");
    vTaskDelay(pdMS_TO_TICKS(750));
    check_gpio(GPIO_NUM_11, "MISO");
    vTaskDelay(pdMS_TO_TICKS(750));
    check_gpio(GPIO_NUM_10, "MOSI");
    vTaskDelay(pdMS_TO_TICKS(750));
    check_gpio(GPIO_NUM_9, "NSS");
    vTaskDelay(pdMS_TO_TICKS(750));
    check_gpio(GPIO_NUM_46, "RST");
    vTaskDelay(pdMS_TO_TICKS(750));
    check_gpio(GPIO_NUM_3, "DIO0");
    vTaskDelay(pdMS_TO_TICKS(1000));
}
