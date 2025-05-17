#include "esp_log.h"
#include "lora_opmode.h"
#include "lora_spi.h"

static const char *TAG = "LORA_OPMODE";

static uint8_t lastOpMode = 0xFF;

void checkLoRaOpMode(void) {
    uint8_t mode = 0; // readRegister(0x01) & 0b111; // Implement readRegister in lora_spi.c
    mode = 0; // stub, replace with actual SPI readRegister call if implemented

    if (mode != lastOpMode) {
        lastOpMode = mode;
        switch (mode) {
            case 0b000:
                ESP_LOGI(TAG, "LoRa is in SLEEP mode.");
                break;
            case 0b001:
                ESP_LOGI(TAG, "LoRa is in STANDBY (IDLE) mode.");
                break;
            case 0b010:
                ESP_LOGI(TAG, "LoRa is in FREQUENCY SYNTHESIZER mode.");
                break;
            case 0b011:
                ESP_LOGI(TAG, "LoRa is in TRANSMIT mode.");
                break;
            case 0b100:
                ESP_LOGI(TAG, "LoRa is in RECEIVE mode.");
                break;
            default:
                ESP_LOGI(TAG, "Unknown LoRa mode: %d", mode);
                break;
        }
    }
}
