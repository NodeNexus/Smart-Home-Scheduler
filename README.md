# Smart Home Scheduler 🕰️

A project to automate home appliance control using a Real-Time Clock (RTC) and a Wi-Fi enabled microcontroller (ESP32) for easy web-based configuration.

## Overview

This system allows users to set two distinct daily ON/OFF schedules for a home appliance (e.g., a lamp, a water heater, or a fan) via a local web interface. The ESP32 acts as both the timekeeper (using NTP or an external RTC) and the web server, controlling a relay module connected to the appliance.

## Features

* **Accurate Timekeeping:** Utilizes Network Time Protocol (**NTP**) for accurate time synchronization (can be adapted for a hardware **DS3231 RTC** module).
* **Web-based Configuration:** Configure the two daily schedules (ON/OFF hours) using a responsive local web page.
* **Two Independent Schedules:** Supports two separate time blocks for operation (e.g., morning and evening).
* **Real-time Status:** Displays the current appliance status and the running schedule on the web page.
* **Safety:** Client-side validation for time inputs.

## Hardware Requirements

| Component | Quantity | Notes |
| :--- | :--- | :--- |
| **ESP32** or **ESP8266** | 1 | Microcontroller with built-in Wi-Fi. |
| **1-Channel Relay Module** | 1 | To safely switch the AC appliance power. |
| **DS3231 RTC Module** (Optional) | 1 | Recommended for reliable timekeeping without continuous internet. |
| **Jumper Wires** | Various | |
| **Power Supply** | 1 | Appropriate power supply for the microcontroller. |

## Wiring Diagram

1.  **Relay Module Connection:**
    * **VCC** $\rightarrow$ **ESP32 5V** (or V-IN, check module specs)
    * **GND** $\rightarrow$ **ESP32 GND**
    * **IN** $\rightarrow$ **ESP32 GPIO 27** (`RELAY_PIN` in code)
    * **Appliance:** Connect the appliance power line through the relay's **Common (COM)** and **Normally Open (NO)** terminals.

2.  **RTC Module Connection (If used):**
    * **VCC** $\rightarrow$ **ESP32 3.3V**
    * **GND** $\rightarrow$ **ESP32 GND**
    * **SDA** $\rightarrow$ **ESP32 SDA (GPIO 21)**
    * **SCL** $\rightarrow$ **ESP32 SCL (GPIO 22)**

## Software Setup and Installation

1.  **Arduino IDE:** Ensure you have the ESP32/ESP8266 boards installed.
2.  **Libraries:**
    * `WiFi.h` (Built-in)
    * `WebServer.h` (Built-in to ESP32/ESP8266 Core)
    * `TimeLib.h` (Install via Library Manager)
3.  **Configuration:**
    * Update the `ssid` and `password` variables in the code with your local Wi-Fi credentials.
    * Adjust the `RELAY_PIN` if you use a different GPIO pin.
4.  **Upload:** Connect the ESP32 and upload the code.
5.  **Access:** Open the Serial Monitor to find the assigned **IP Address** (e.g., `192.168.1.100`). Open this IP in any web browser on the same network to access the scheduler interface.

## Usage

1.  Access the web interface using the IP address.
2.  View the **Current Status** and **Simulated Time**.
3.  Enter your desired **ON/OFF hours** (in 24-hour format, e.g., 7 for 7 AM, 18 for 6 PM).
4.  Click **"Update Schedule"** to save the new times to the device.
5.  The ESP32 will continuously monitor the time and toggle the relay accordingly.
