# DSMR P1 Reader for Raspberry Pi and Windows

## Overview
This project is a **DSMR P1 smart meter reader** that works on **Raspberry Pi and Windows**. It reads P1 meter data via serial communication, processes it, and outputs key power metrics. It supports multiple hardware platforms through modular GPIO implementations.

## Features
- Reads **DSMR P1 telegrams** from smart meters.
- Uses **pigpio** for GPIO handling on Raspberry Pi.
- Supports **Raspberry Pi (pigpio)** and **Windows (MinGW/MSVC)**.
- Parses power consumption, voltage, and current values.
- Validates messages using **CRC16 checksum**.
- Modular **GPIO handling** for different platforms.
- Multi-platform support with **Makefile** and **cross-compilation options**.

## Supported Platforms
| Platform | Serial Support | GPIO Support |
|----------|---------------|--------------|
| **Raspberry Pi** | ✅ `/dev/ttyUSB0` | ✅ `pigpio` |
| **Windows (MSYS2/MinGW)** | ✅ `COMx` | ❌ (Disabled) |
| **Linux** | ✅ `/dev/ttyUSB0` | ✅ `/sys/class/gpio` |
| **ESP32 (Planned)** | ✅ UART | ✅ `driver/gpio.h` |

## Installation
### **For Raspberry Pi**
```sh
sudo apt update
sudo apt install -y pigpio
```

## Cross-Compilation for Raspberry Pi
To cross-compile on Ubuntu for Raspberry Pi, follow these steps:

1. Install the **Raspberry Pi cross-compiler**:
   ```sh
   sudo apt update
   sudo apt install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
   ```

2. Clone and cross-compile **pigpio**:
   ```sh
   git clone https://github.com/joan2937/pigpio.git
   cd pigpio
   make clean
   make CROSS_PREFIX=arm-linux-gnueabihf-
   ```

3. Copy the built libraries for cross-compilation:
   ```sh
   mkdir -p ~/pigpio-cross/lib ~/pigpio-cross/include
   cp libpigpio.so ~/pigpio-cross/lib/
   cp -r pigpio.h ~/pigpio-cross/include/
   ```

4. Modify the Makefile to use cross-compiled pigpio:
   ```make
   CFLAGS = -Wall -Wextra -O2 -DPLATFORM_RPI -I$(HOME)/pigpio-cross/include
   LDFLAGS = -L$(HOME)/pigpio-cross/lib -lpigpio
   ```

5. Compile the project:
   ```sh
   make clean
   make
   ```

6. Transfer the binary to Raspberry Pi:
   ```sh
   scp dsmr_reader pi@raspberrypi:/home/pi/
   ssh pi@raspberrypi
   chmod +x dsmr_reader
   sudo ./dsmr_reader
   ```

## Usage
After compilation, run:
```sh
sudo ./dsmr_reader
```
It will output parsed DSMR data:
```
Received data:
...raw DSMR telegram...
Parsed Data:
Timestamp: 2024-02-25 12:34:56
Power Delivered: 1.234 kW
Power Returned: 0.123 kW
Voltage L1: 230.1 V
Current L1: 2.34 A
```

## Contributions
- **Multi-platform GPIO support** (RPi, Windows, ESP32) is ongoing.
- **Feature requests & bug reports** are welcome!

## License
This project is licensed under **GNU General Public License v3 (GPLv3)**.
