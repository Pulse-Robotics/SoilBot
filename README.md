# SoilBot: Wireless Moisture Monitor

SoilBot is a wireless moisture monitoring system designed for high-accuracy soil data collection. Using the **ESP32-C3** RISC-V microcontroller, it gathers sensor readings and transmits data wirelessly for environmental analysis.


| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

## Project Overview

The current firmware initializes the ESP-IDF environment, configures the hardware abstraction layer for the C3 chipset, and prepares the system for wireless data transmission.

### Folder Contents

```text
├── .gitignore                 # Tells Git which files to ignore
├── CMakeLists.txt             # Project-level build script
├── partitions.csv             # Memory partitions file
├── data/                      
│   └── config.json            # WiFi, sleep timer, and plant profiles
├── main/
│   ├── CMakeLists.txt         # Component-level build script
│   └── main.c                 # Core SoilBot logic
└── README.md                  # Project documentation
```


## Getting Started

### Prerequisites

*   **ESP-IDF Toolchain**: Ensure you have [ESP-IDF installed](https://docs.espressif.com) (v5.0 or newer recommended).
*   **Hardware**: An ESP32-C3 development board and a compatible moisture sensor.

### Build and Flash

Follow these steps to compile the code and upload it to your chipset:

1.  **Set the Target**:
    Standardize the build for the ESP32-C3:
    ```bash
    idf.py set-target esp32c3
    ```

2.  **Build the Project**:
    ```bash
    idf.py build
    ```

3.  **Flash and Monitor**:
    Replace `PORT` with your specific COM port (e.g., `COM3` on Windows or `/dev/ttyUSB0` on Linux):
    ```bash
    idf.py -p PORT flash monitor
    ```
    *Note: Use `Ctrl + ]` to exit the monitor.*

## Data Collection
The system is designed to wake up from deep sleep, sample the ADC connected to the moisture probe, and send the data via Wi-Fi or ESP-NOW before returning to a low-power state.

## Troubleshooting

*   **Chipset Mismatch**: If you see errors regarding chip revision, ensure you ran `set-target esp32c3`.
*   **Permissions**: On Linux, ensure your user is part of the `dialout` group to access the USB port.
*   **Flash Failures**: If the board won't enter bootloader mode, hold the **BOOT** button while plugging in the USB cable.

## Technical Support and Feedback

*   For ESP32-C3 specific hardware questions, refer to the [Espressif Documentation](https://docs.espressif.com).