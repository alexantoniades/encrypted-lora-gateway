# Encrypted Single-Channel LoRa Gateway

A single-channel gateway made with a Heltec-ESP32-WiFi-LoRa Dev Board.
The sketch was developed on the Arduino IDE, assigning the appropriate board in the tool bar and installing included libraries or similar.

The ESP32 specifically is programmed to communicate with the LoRa Radio by assigning the pins:
  - SS       18
  - RST      14
  - DIO(IRQ) 26

The ESP32 is set as a WiFi Client, staticly connecting to a WiFi Access Point assigned as SSID and Password.

Data is collected, wrapped as a LoRa packet and pushed to the LoRa Radio for trnasmission, encrypting the packets using AES128. The encrypted packet is then sent to the assigned destination, switching to Callback Mode, waiting for the receiver to respond.

The AES key can be chosen beforehand and assigned to both LoRa Bi-Directional Gateways in order to decrypt messages received.

Application for this includes: Private LoRa networks(Low-Level Encryption), Swarm Robotics, Security for IoT, Autonomous Drone composer

When uploading the code, make sure to click "File -> 'Additional Boards Manager URLs: '":
```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
copy and paste the above URLs and click ok.
In tools click "Tools -> Board: -> Board Manager..." and install the ESP32 board set.
In tools click "Tools -> Board: -> 'Heltec_WIfI_LoRa_32'"

Author: Alexandros Antoniades, 
Date: 14/03/2019
