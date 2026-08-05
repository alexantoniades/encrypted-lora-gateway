

# Gateway LoRa de Canal Único Cifrado

Un gateway de canal único fabricado con una Heltec-ESP32-WiFi-LoRa Dev Board.
El sketch se desarrolló en el Arduino IDE, asignando la placa correspondiente en la barra de herramientas e instalando las bibliotecas incluidas o similares.

El ESP32, específicamente, está programado para comunicarse con la LoRa Radio asignando los pines:
  - SS       18
  - RST      14
  - DIO(IRQ) 26

El ESP32 se configura como WiFi Client, conectándose estáticamente a un punto de acceso WiFi asignado como SSID y Password.

Los datos se recopilan, se envuelven como un paquete LoRa y se envían a la LoRa Radio para su transmisión, cifrando los paquetes usando AES128. El paquete cifrado se envía entonces al destino asignado, cambiando a Callback Mode, esperando que el receptor responda.

La clave AES puede elegirse con antelación y asignarse a ambos LoRa Bi-Directional Gateways para descifrar los mensajes recibidos.

Las aplicaciones para esto incluyen: Private LoRa networks(Low-Level Encryption), Swarm Robotics, Security for IoT, Autonomous Drone composer

Al cargar el código, asegúrese de hacer clic en "File -> 'Additional Boards Manager URLs: '":
```
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
copie y pegue las URLs anteriores y haga clic en ok.
En herramientas haga clic en "Tools -> Board: -> Board Manager..." e instale el conjunto de placas ESP32.
En herramientas haga clic en "Tools -> Board: -> 'Heltec_WIfI_LoRa_32'"

Autor: Alexandros Antoniades, 
Fecha: 14/03/2019
