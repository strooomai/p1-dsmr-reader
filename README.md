# DSMR P1 Reader for Raspberry Pi and Windows

## Overview
This project is a **DSMR P1 smart meter reader** that works on **Raspberry Pi and Windows**. It reads P1 meter data via serial communication, processes it, and outputs key power metrics. It supports multiple hardware platforms through modular GPIO implementations.

## Features
- Reads **DSMR P1 telegrams** from smart meters.
- Supports **Raspberry Pi (wiringPi)** and **Windows (MinGW/MSVC)**.
- Parses power consumption, voltage, and current values.
- Validates messages using **CRC16 checksum**.
- Modular **GPIO handling** for different platforms.
- Multi-platform support with **Makefile** and **cross-compilation options**.

## Supported Platforms
| Platform | Serial Support | GPIO Support |
|----------|---------------|--------------|
| **Raspberry Pi** | ✅ `/dev/ttyUSB0` | ✅ `wiringPi` |
| **Windows (MSYS2/MinGW)** | ✅ `COMx` | ❌ (Disabled) |
| **Linux** | ✅ `/dev/ttyUSB0` | ✅ `/sys/class/gpio` |
| **ESP32 (Planned)** | ✅ UART | ✅ `driver/gpio.h` |

## Installation
### **For Raspberry Pi**
```sh
sudo apt update
sudo apt install -y build-essential wiringpi
```

### **For Windows (MSYS2/MinGW)**
1. Download and install **MSYS2** from [https://www.msys2.org/](https://www.msys2.org/).
2. Open **MSYS2 MinGW 64-bit** terminal.
3. Install dependencies:
   ```sh
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
   ```

## Compilation
### **For Raspberry Pi**
```sh
make
sudo ./dsmr_reader
```

### **For Windows (MinGW)**
```sh
mingw32-make
./dsmr_reader.exe
```

### **For Cross-Compiling (Raspberry Pi from Windows)**
1. Install the **Raspberry Pi Toolchain**.
2. Modify **Makefile**:
   ```make
   CC = arm-linux-gnueabihf-gcc
   ```
3. Compile:
   ```sh
   make
   scp dsmr_reader pi@raspberrypi:/home/pi/
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