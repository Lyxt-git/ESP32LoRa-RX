#ifndef LORA_SPI_H
#define LORA_SPI_H

#include <stdbool.h>
#include <stdint.h>

void spi_init(void);
void test_spi(void);
bool lora_begin(uint32_t frequency);
int lora_parse_packet(void);
int lora_available(void);
int lora_read(void);
int lora_packet_rssi(void);

#endif
