/*
The MIT License (MIT)
Copyright © 2025 <Lyxt>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the “Software”), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lora_gpio.h"
#include "lora_diag.h"
#include "lora_spi.h"
#include "lora_opmode.h"

static const char *TAG = "ESP32-S3LORA-RX";

void app_main(void) {
    // One-line boot delay animation
    // Welcome screen
    printf("\n\n");
    printf("=========================================================================\n");
    printf("Modularized 433mhz Long Range (LoRa Ra-01) RX Communicator Firmware v1.02\n");
    printf("2025 © Lyxt under MIT License. Email : analyxt@proton.me\n");
    printf("=========================================================================\n");
    printf("ESP32-S3LORA-RX: Booting LoRa RX ");
    fflush(stdout);
    for (int i = 0; i < 5; i++) {
        printf(".");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("\n");

   /*Welcome screen, Uncomment this if to use ESP_LOGI for Welcome Screen*/
   // ESP_LOGI(TAG, "===============================");
   // ESP_LOGI(TAG, "  ESP32-S3 LoRa RX Firmware   ");
   // ESP_LOGI(TAG, "         by Lyxt ⚡️           ");
   // ESP_LOGI(TAG, "===============================");

    configure_gpio_defaults();
    reset_lora_module();
    diagnostic_gpio();
    spi_init();
    test_spi();

    if (!lora_begin(433000000)) {
        ESP_LOGE(TAG, "Failed to start LoRa module");
        return;
    }
    // lora_receive();  // tbc

    while (1) {
        int packetSize = lora_parse_packet();
        if (packetSize) {
            ESP_LOGI(TAG, "Received packet size: %d", packetSize);
            while (lora_available()) {
                putchar(lora_read());
            }
            int rssi = lora_packet_rssi();
            ESP_LOGI(TAG, "RSSI: %d", rssi);
            putchar('\n');
        }
        checkLoRaOpMode();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
