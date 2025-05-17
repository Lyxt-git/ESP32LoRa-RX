#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lora_spi.h"
#include "lora_gpio.h"

// Define LoRa SPI and GPIO pins
#define LORA_SCK   GPIO_NUM_12
#define LORA_MISO  GPIO_NUM_11
#define LORA_MOSI  GPIO_NUM_10
#define LORA_SS    GPIO_NUM_9
#define LORA_RST   GPIO_NUM_46
#define LORA_DIO0  GPIO_NUM_3

#define TAG "LORA_SPI"

static spi_device_handle_t spi = NULL;

#define TEST_SPI_DATA 0xAA

static uint8_t readRegister(uint8_t reg) {
    uint8_t val = 0;
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 16, // bits
        .tx_data = {reg & 0x7F, 0x00},
    };
    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret == ESP_OK) {
        val = t.rx_data[1];
    } else {
        ESP_LOGE(TAG, "SPI transmit failed");
    }
    return val;
}

void spi_init(void) {
    spi_bus_config_t buscfg = {
        .mosi_io_num = LORA_MOSI,
        .miso_io_num = LORA_MISO,
        .sclk_io_num = LORA_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .mode = 0,
        .spics_io_num = LORA_SS,
        .queue_size = 1,
    };

    // ESP32-S3: Use SPI3_HOST and auto DMA allocation
    esp_err_t ret = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }

    ret = spi_bus_add_device(SPI3_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return;
    }
}

void test_spi(void) {
    printf("\n");
    printf("ESP32-S3LORA-RX: Testing SPI Communication ");
    fflush(stdout);
    for (int i = 0; i < 7; i++) {
        printf(".");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("\n");

    uint8_t received = 0;
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 8, // bits
        .tx_data = {TEST_SPI_DATA},
    };

    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret == ESP_OK) {
        received = t.rx_data[0];
    } else {
        ESP_LOGE(TAG, "SPI transmit failed");
        return;
    }

    ESP_LOGI(TAG, "Sent: 0x%02X | Received: 0x%02X", TEST_SPI_DATA, received);

    if (received != 0x00 && received != TEST_SPI_DATA) {
        ESP_LOGI(TAG, "SPI communication looks active.");
        vTaskDelay(pdMS_TO_TICKS(500));
        ESP_LOGI(TAG, "LoRa Receiver Ready.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    } else {
        ESP_LOGE(TAG, "SPI communication failed. Check NSS and MISO wiring.");
    }
}

// Stubbed LoRa functions for demonstration purposes
bool lora_begin(uint32_t frequency) {

    printf("\n");
    printf("ESP32-S3LORA-RX: Begin LoRa RX ");
    fflush(stdout);
    for (int i = 0; i < 5; i++) {
        printf(".");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    printf("\n");

    ESP_LOGI(TAG, "LoRa.begin at %" PRIu32 " Hz", frequency);
    return true;
}

int lora_parse_packet(void) {
    return 0;
}

int lora_available(void) {
    return 0;
}

int lora_read(void) {
    return 0;
}

int lora_packet_rssi(void) {
    return -50;
}
