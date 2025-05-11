# LoRa Receiver with ESP32-S3

This project implements a simple LoRa receiver using the ESP32-S3 microcontroller and a LoRa module (typically an SX127x based module). The receiver listens for incoming LoRa packets and displays the received data along with the signal strength (RSSI) in the Serial Monitor.

## Features

### Old Features
- Custom pin mapping for the ESP32-S3 GPIO pins.
- Receives LoRa messages from the transmitter and prints them to the Serial Monitor.
- Displays received signal strength (RSSI) for each packet.
- GPIO diagnostic function to check the status of the LoRa pins.
- SPI communication test to ensure the LoRa module is working properly.

### New Features
- **Smart LoRa OpMode Tracking**: Detects and reports changes in SX127x state (SLEEP, STANDBY, RX, TX).
- **Improved SPI Health Check**: Actively tests SPI by sending a known byte (0xAA) and reporting feedback.
- **Cleaner Serial Output**: Status messages are printed only on change, eliminating repeated logs.
- **GPIO Preconditioning**: Uses `INPUT_PULLDOWN` to stabilize lines and prevent false readings.
- **Code Refactor**: Organized functions for GPIO configuration, diagnostics, and SPI tests.
- **Faster Packet Loop**: Reduced delay between reads for quicker reaction to incoming LoRa messages.

## Hardware Requirements

- **ESP32-S3** (e.g., ESP32-S3-N16R8).
- **LoRa Module** (e.g., SX1278 or SX1276) connected via SPI.
- **LED** connected to **GPIO2** (or another available GPIO) for visual indication (optional). If your ESP32-S3 has a built-in LED, it is typically connected to GPIO2.

### LoRa Pin Mapping

This project uses the following custom pin mappings for the LoRa module:

| LoRa Pin    | ESP32 Pin  |
|-------------|------------|
| **SCK**     | GPIO12     |
| **MISO**    | GPIO11     |
| **MOSI**    | GPIO10     |
| **SS**      | GPIO9      |
| **RST**     | GPIO46     |
| **DIO0**    | GPIO3      |

## Software Requirements

- **Arduino IDE** with support for ESP32 boards.
- **LoRa Library**: Install the [LoRa library](https://github.com/sandeepmistry/arduino-LoRa) via the Arduino Library Manager.
- **SPI Library**: This library is part of the standard Arduino setup.

## Setup Instructions

1. **Install the ESP32 Board in Arduino IDE**:
   - Go to `File > Preferences` and add the following URL to the "Additional Boards Manager URLs":
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager` and install the **esp32** platform.

2. **Install the LoRa Library**:
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for `LoRa` and install the library by **Sandeep Mistry**.

3. **Upload the Code**:
   - Connect your ESP32-S3 to your computer.
   - Open the `LoRa Receiver` sketch in Arduino IDE.
   - Select the correct board and port under `Tools > Board` and `Tools > Port`.
   - Upload the sketch to your ESP32-S3.

4. **Testing**:
   - Once the code is uploaded, open the Serial Monitor at a baud rate of 115200 to see the received data.
   - Ensure that the transmitter is also powered on and transmitting messages.

## How it Works

- The program initializes the LoRa module with custom pin mapping and sets up SPI communication.
- It listens for incoming LoRa packets using `LoRa.parsePacket()`.
- When a packet is received, it prints the packet data along with the RSSI (signal strength) value to the Serial Monitor.
- A GPIO diagnostic function checks the status of each pin to ensure the connections are correct.
- An SPI communication test is run to verify the LoRa module is functioning correctly.

## Troubleshooting

- If LoRa initialization fails, ensure that your LoRa module is connected properly, and check that the pins are correctly mapped in the code.
- If the receiver isn't picking up packets, ensure that the transmitter is sending on the same frequency (433 MHz in this case).
- If the RSSI value is low, ensure that the antenna is properly connected and the devices are within range of each other.

#Output sample based on some message and debugging.

=== Booting LoRa Receiver ===
SCK pin is LOW (expected).
MISO pin is HIGH (may be floating).     << This Could be Floating or Wiring Issue (My breadboard old)
MOSI pin is LOW (expected).
NSS pin is HIGH (may be floating).      << This Could be Floating or Wiring Issue (My breadboard old)
RST pin is HIGH (may be floating).      << This Could be Floating or Wiring Issue (My breadboard old)
DIO0 pin is LOW (expected).
GPIO diagnostic completed.
Resetting LoRa module...
LoRa module reset complete.
Initializing SPI...
Starting LoRa...
LoRa Receiver Ready.
Testing SPI...
Sent: 0xAA | Received: 0x49
SPI communication looks active.
LoRa is in STANDBY (idle between receives).
Received packet: Hello  ESP32-S3Central  RSSI: -50
Received packet: I am your servant now  RSSI: -51
Received packet: This message come from us, the ESP32-S3Rogue  RSSI: -61
Received packet: Assalamualaikum  RSSI: -52
Received packet: Keep this message for your Infiltration Task  RSSI: -51
Received packet: Hello  ESP32-S3Central  RSSI: -53
Received packet: I am your servant now  RSSI: -57
Received packet: This message come from us, the ESP32-S3Rogue  RSSI: -55
Received packet: Assalamualaikum  RSSI: -55
Received packet: Keep this message for your Infiltration Task  RSSI: -66
Received packet: Hello  ESP32-S3Central  RSSI: -62
Received packet: I am your servant now  RSSI: -55
Received packet: Assalamualaikum  RSSI: -50

P/S : Tested on Breadboard, the issue on debugging health mostly come from Wiring Issue

## License

This project is open-source and released under the [MIT License](LICENSE).
