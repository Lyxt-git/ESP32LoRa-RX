#include <SPI.h>
#include <LoRa.h>

// Define GPIO pins
#define LORA_SCK    12  // GPIO12
#define LORA_MISO   11  // GPIO11
#define LORA_MOSI   10  // GPIO10
#define LORA_SS     9   // GPIO9 (NSS)
#define LORA_RST    46  // GPIO46 (Reset)
#define LORA_DIO0   3   // GPIO3 (Interrupt)

// Test SPI transfer data
#define TEST_SPI_DATA 0xAA

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting GPIO diagnostic...");
  diagnostic_gpio();

  // Reset the LoRa module
  Serial.println("Resetting LoRa module...");
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(100);  // Keep low for 100 ms
  digitalWrite(LORA_RST, HIGH);
  delay(100);  // Wait after releasing reset
  Serial.println("LoRa module reset complete.");

  // Begin SPI communication
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

  // Set LoRa pins
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  // Start LoRa at 433 MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed. Check wiring.");
    while (true);  // Halt
  }

  Serial.println("LoRa Receiver Ready");

  // SPI communication test
  test_spi();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.print("  RSSI: ");
    Serial.println(LoRa.packetRssi());
  }

  delay(1000);  // Polling interval
}

// GPIO Diagnostic
void diagnostic_gpio() {
  check_gpio(LORA_SCK, "SCK");
  check_gpio(LORA_MISO, "MISO");
  check_gpio(LORA_MOSI, "MOSI");
  check_gpio(LORA_SS, "NSS");
  check_gpio(LORA_RST, "RST");
  check_gpio(LORA_DIO0, "DIO0");
  Serial.println("GPIO diagnostic completed.");
}

void check_gpio(int pin, String pinName) {
  pinMode(pin, INPUT);
  int state = digitalRead(pin);
  Serial.print(pinName);
  if (state == LOW) {
    Serial.println(" pin is LOW (correct).");
  } else {
    Serial.println(" pin is HIGH (incorrect).");
  }
}

// SPI Communication Test
void test_spi() {
  Serial.println("Testing SPI communication...");
  uint8_t received = 0;

  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));  // Slow down SPI for reliability
  digitalWrite(LORA_SS, LOW);
  received = SPI.transfer(TEST_SPI_DATA);
  digitalWrite(LORA_SS, HIGH);
  SPI.endTransaction();

  Serial.print("Sent: 0x");
  Serial.print(TEST_SPI_DATA, HEX);
  Serial.print(" | Received: 0x");
  Serial.println(received, HEX);

  if (received != 0x00 && received != TEST_SPI_DATA) {
    Serial.println("SPI communication looks active.");
  } else {
    Serial.println("SPI communication failed. Check wiring.");
  }
}
