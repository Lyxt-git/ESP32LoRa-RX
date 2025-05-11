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

  Serial.println("=== Booting LoRa Receiver ===");
  configure_gpio_defaults();
  delay(300);

  diagnostic_gpio();

  // Reset LoRa
  Serial.println("Resetting LoRa module...");
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(150);
  digitalWrite(LORA_RST, HIGH);
  delay(200);
  Serial.println("LoRa module reset complete.");

  // Init SPI
  Serial.println("Initializing SPI...");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  delay(100);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  Serial.println("Starting LoRa...");
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (true);
  }

  Serial.println("LoRa Receiver Ready.");
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
        Serial.println("LoRa is in STANDBY (idle between receives).");
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
  check_gpio(LORA_SCK, "SCK");
  check_gpio(LORA_MISO, "MISO");
  check_gpio(LORA_MOSI, "MOSI");
  check_gpio(LORA_SS, "NSS");
  check_gpio(LORA_RST, "RST");
  check_gpio(LORA_DIO0, "DIO0");
  Serial.println("GPIO diagnostic completed.");
}

void check_gpio(int pin, String name) {
  pinMode(pin, INPUT);
  delay(10);
  int state = digitalRead(pin);
  Serial.print(name);
  if (state == LOW) {
    Serial.println(" pin is LOW (expected).");
  } else {
    Serial.println(" pin is HIGH (may be floating).");
  }
}

void test_spi() {
  Serial.println("Testing SPI...");
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

  if (received != 0x00 && received != TEST_SPI_DATA) {
    Serial.println("SPI communication looks active.");
  } else {
    Serial.println("SPI communication failed. Check NSS and MISO.");
  }
}
