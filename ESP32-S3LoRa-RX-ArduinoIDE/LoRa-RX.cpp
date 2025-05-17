/*MIT License

Copyright (c) 2025 Lyxt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include <SPI.h>
#include <LoRa.h>

// GPIO definitions
#define LORA_SCK    12
#define LORA_MISO   11
#define LORA_MOSI   10
#define LORA_SS     9
#define LORA_RST    46
#define LORA_DIO0   3

#define TEST_SPI_DATA 0xAA

uint8_t lastOpMode = 0xFF;  // Track last mode for change detection

void setup() {
  Serial.begin(115200);
  while (!Serial);

  delay(1000);
  Serial.println(".");
  delay(1000);
  Serial.println("..");
  delay(1000);
  Serial.println("...");
  delay(2000);
  Serial.println("=============================");
  Serial.println("=== Booting LoRa Receiver ===");
  Serial.println("=============================");
  delay(2000);
  Serial.println("Loading ....");
  configure_gpio_defaults();
  delay(10000);
  diagnostic_gpio();      // GPIO state check

  // Reset LoRa
  Serial.println("Resetting LoRa module for the unexpected issue ...");
  delay(5000);
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(200);
  digitalWrite(LORA_RST, HIGH);
  delay(200);
  Serial.println("LoRa module reset complete.");
  Serial.println("Diagnose completed, Press Reset Button to Re-Diagnose.");
  Serial.println("======================================================");
  delay(2000);

  // Init SPI
  Serial.println("Initializing SPI...");
  delay(5000);
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("SPI communication failed. Check 'Unexpected' wiring Issue.");
    delay(5000);
    while (true);
  }
  test_spi();
}

void loop() {
  checkLoRaOpMode();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.print("  RSSI: ");
    Serial.println(LoRa.packetRssi());
  }

  delay(500);
}

void checkLoRaOpMode() {
  uint8_t mode = readRegister(0x01) & 0b111;
  if (mode != lastOpMode) {
    lastOpMode = mode;
    switch (mode) {
      case 0b000:
        Serial.println("LoRa is in SLEEP mode.");
        break;
      case 0b001:
        Serial.println("LoRa is in STANDBY (idle between receives):");
        delay(1000);
        Serial.println("Waiting incoming packet from TX ...");
        break;
      case 0b011:
        Serial.println("LoRa is TRANSMITTING.");
        break;
      case 0b100:
        Serial.println("LoRa is actively listening (RX_CONTINUOUS).");
        break;
      default:
        Serial.print("LoRa OpMode: 0b");
        Serial.println(mode, BIN);
        break;
    }
  }
}

// Read register via SPI
uint8_t readRegister(uint8_t reg) {
  digitalWrite(LORA_SS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg & 0x7F);
  uint8_t val = SPI.transfer(0x00);
  SPI.endTransaction();
  digitalWrite(LORA_SS, HIGH);
  return val;
}

void configure_gpio_defaults() {
  pinMode(LORA_SCK, INPUT_PULLDOWN);
  pinMode(LORA_MISO, INPUT_PULLDOWN);
  pinMode(LORA_MOSI, INPUT_PULLDOWN);
  pinMode(LORA_SS, OUTPUT); digitalWrite(LORA_SS, HIGH);
  pinMode(LORA_RST, OUTPUT); digitalWrite(LORA_RST, HIGH);
  pinMode(LORA_DIO0, INPUT);
}

void diagnostic_gpio() {
  Serial.println("");
  Serial.println("Performing GPIO diagnostic...");
  delay(3000);
  check_gpio(LORA_SCK, "SCK");
  delay(750);
  check_gpio(LORA_MISO, "MISO");
  delay(750);
  check_gpio(LORA_MOSI, "MOSI");
  delay(750);
  check_gpio(LORA_SS, "NSS");
  delay(750);
  check_gpio(LORA_RST, "RST");
  delay(750);
  check_gpio(LORA_DIO0, "DIO0");
  delay(1000);
}

void check_gpio(int pin, String name) {
  pinMode(pin, INPUT);
  delay(10);
  int state = digitalRead(pin);
  Serial.print(name);
  if (state == LOW) {
    Serial.println(" pin is LOW (expected).");
  } else {
    Serial.println(" pin is HIGH (unexpected).");
  }
}

void test_spi() {
  Serial.println("Testing SPI communication...");
  delay(5000);
  uint8_t received = 0;

  digitalWrite(LORA_SS, HIGH);
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  digitalWrite(LORA_SS, LOW);
  delayMicroseconds(10);
  received = SPI.transfer(TEST_SPI_DATA);
  delayMicroseconds(10);
  digitalWrite(LORA_SS, HIGH);
  SPI.endTransaction();

  Serial.print("Sent: 0x");
  Serial.print(TEST_SPI_DATA, HEX);
  Serial.print(" | Received: 0x");
  Serial.println(received, HEX);
  delay(3000);

  if (received != 0x00 && received != TEST_SPI_DATA) {
    Serial.println("SPI communication looks active.");
    delay(500);
    Serial.println("LoRa Receiver Ready.");
    delay(1000);
  } else {
    Serial.println("SPI communication failed. Check NSS and MISO.");
  }
}