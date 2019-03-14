#include "WiFi.h"     // Wi-Fi Library
#include "AESLib.h"   // AES Encryption Library
#include <U8x8lib.h>  // OLED display Library
#include <SPI.h>      // SPI Library
#include <LoRa.h>     // LoRa Lirbary

// SX1272 pins
#define SS      18
#define RST     14
#define DI0     26

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// Initialise and configure AES Encryption settings
AESLib aesLib;
char cleartext[256];
char ciphertext[512];

// AES Encryption key
byte aes_key[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


// LoRa duplex config
String outgoing;              // outgoing message

String plaintext = outgoing;

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

// Wi-Fi settings
char ssid[] = "WiFi_Guest";
const char* password = "";

const char* host = "";
const char* streamId   = "....................";
const char* privateKey = "....................";

void setup()
{
  // Initialise Serial monitor. At 115200 baud
  Serial.begin(230400);
  while (!Serial);
  // Initialise OLED Display settings 
  oledInit();
  // Initialize and configure WiFi Client
  configWifi();
  // Initialise and configure LoRa Radio (SX1272)
  configLora();
  // Connect to WiFi access point when near
  connectWifi();
  
}

void loop() {
  if (millis() - lastSendTime > interval) {
    sendMessage(outgoing);
    Serial.println("Sending " + outgoing);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;     // 2-3 seconds
    LoRa.receive();                     // go back into receive mode
  }
}
void oledInit() {
  // Initialize OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r); // Set font
}
void configLora() {
  // Configuring LoRa
  LoRa.setPins(SS, RST, DI0);
  Serial.println("Starting LoRa...");
  oledPrint(0, 4, "Starting LoRa");
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa... failed!");
    oledPrint(0, 4, "LoRa: Failed");
    while(true);
  }
  delay(1000);
  Serial.println("LoRa: ON");
  oledClearRow(4);
  oledPrint(0,4, "LoRa: ON");

  // Set LoRa to listen
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("Mode: Callback");
  oledPrint(0,5, "Mode: Callback");
}

void configWifi() {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  oledPrint(0,1,"Wi-Fi: ON");
  Serial.println("Wi-Fi: ON");
  delay(100);
  oledPrint(0,2,"SSID: N/A");
  Serial.println("SSID: N/A");
  delay(100);
  oledPrint(0,3,"NOT CONNECTED");
  Serial.println("NOT CONNECTED");
  delay(100);
}

void connectWifi(){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    oledClearRow(2);
    oledPrint(0, 2, "SSID: _");
    oledClearRow(3);
    oledPrint(0,3,"CONNECTED");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void oledPrint(int x, int y, char message[] ) {
  u8x8.drawString(x, y, message);
}

void oledClear() {
  u8x8.clear();
}

void oledClearRow(int row) {
  for (int i=0; i < 18; i++) {
    u8x8.drawString(i,row," ");
  }
}

void sendMessage(String outgoing) {
  // Encrypt
  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  //cleartext[] = outgoing;
  String encrypted_outgoing = encrypt(cleartext, enc_iv);   // send an encrypted message
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(encrypted_outgoing.length());        // add payload length
  LoRa.print(encrypted_outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  oledPrint(0,6,"SENDING");
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  oledPrint(0,6, "RECEIVED");
  String incoming = "";                 // payload of packet

  // Decrypt
  byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  String decr_incoming = decrypt(ciphertext, dec_iv);
  while (LoRa.available()) {            // can't use readString() in callback, so
    incoming += (char)LoRa.read();      // add bytes one by one
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

// Generate IV (once)
void aes_init() {
  Serial.println("gen_iv()");
  aesLib.gen_iv(aes_iv);
  // workaround for incorrect B64 functionality on first run...
  Serial.println("encrypt()");
  Serial.println(encrypt(strdup(plaintext.c_str()), aes_iv));
}

String encrypt(char * msg, byte iv[]) {  
  int msgLen = strlen(msg);
  Serial.print("msglen = "); Serial.println(msgLen);
  char encrypted[4 * msgLen];
  aesLib.encrypt64(msg, encrypted, aes_key, iv);
  Serial.print("encrypted = "); Serial.println(encrypted);
  oledPrint(0,7, "ENCRYPTED");
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, iv);
  oledPrint(0,7, "DECRYPTED");
  return String(decrypted);
}
