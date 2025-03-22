#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>

const char* ssid = "MyWiFi";
const char* password = "MyPassword";
String hostname = "";

const char* udpServer = "example.com";
const int udpPort = 12345;

WiFiUDP udp;

int scanTime = 5;
BLEScan* pBLEScan;

void sendToUDPServer(String data);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    StaticJsonDocument<256> doc;
    if (hostname.length() > 0){
        doc["gate"] = hostname;
    }    
    doc["mac"] = advertisedDevice.getAddress().toString().c_str();    
    String name = String(advertisedDevice.getName().c_str());
    if (name.length() > 0){
      doc["name"] = name;
    }        
    doc["rssi"] = advertisedDevice.getRSSI();
    String mfrData = advertisedDevice.getManufacturerData();
    String mfrDataHex = "";
    if (mfrData.length() > 0) {
      char hex[3];
      for (size_t i = 0; i < mfrData.length(); i++) {
        sprintf(hex, "%02X", (unsigned char)mfrData[i]);
        mfrDataHex += hex;
      }
    } else {
      mfrDataHex = "";
    }
    if (mfrDataHex.length() > 0){
      doc["data"] = mfrDataHex;
    }
    String jsonData;
    serializeJson(doc, jsonData);
    sendToUDPServer(jsonData);
  }
};

void setupWiFi() {
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    hostname = WiFi.getHostname();
  } else {
    WiFi.disconnect();
    delay(5000);
  }
}

void checkAndReconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    delay(5000);
    setupWiFi();
  }
}

void setupBLE() {
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false);
}

void sendToUDPServer(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    udp.beginPacket(udpServer, udpPort);
    udp.print(data);
    udp.endPacket();
  }  
}

void setup() {
  setupWiFi();
  setupBLE();
}

void loop() {
  checkAndReconnectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    pBLEScan->start(scanTime, false);
  }   
  delay(100);
  pBLEScan->clearResults();
}
