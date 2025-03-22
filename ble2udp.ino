#include <WiFi.h>
#include <WiFiUdp.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>

// Wi-Fi настройки
const char* ssid = "MyWiFi";
const char* password = "012345678";
const char* hostname = "BLE2UDPGW01";

// UDP настройки
// Сервер UDP и порт
const char* udpServer = "10.100.0.75";
const int udpPort = 12345;

WiFiUDP udp;

// BLE настройки
int scanTime = 5; // Время сканирования в секундах
BLEScan* pBLEScan;

void sendToUDPServer(String data);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    StaticJsonDocument<200> doc;
    // MAC-адрес
    doc["Mac"] = advertisedDevice.getAddress().toString().c_str();
    // Имя устройства
    String name = String(advertisedDevice.getName().c_str());
    if (name.length() > 0){
      doc["Name"] = name;
    }        
    // RSSI
    doc["Rssi"] = advertisedDevice.getRSSI();
    // Manufacturer Data
    String mfrData = advertisedDevice.getManufacturerData(); // String, а не std::string
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
      doc["Data"] = mfrDataHex;
    }    
    String jsonData;
    serializeJson(doc, jsonData);
    sendToUDPServer(jsonData);
  }
};

void setupWiFi() {
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Ограничение попыток
    delay(500);
    attempts++;
  }  
}
void checkAndReconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    setupWiFi(); // Повторная попытка подключения
  }
}
void setupBLE() {
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false);
}
void sendToUDPServer(String data) {
  if (WiFi.status() == WL_CONNECTED) { // Отправка только при активном соединении
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
  checkAndReconnectWiFi(); // Проверка и переподключение Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    pBLEScan->start(scanTime, false); // Запуск сканирования только при Wi-Fi
  }   
  delay(100); // Пауза перед следующим сканированием
  pBLEScan->clearResults(); // Очистка результатов
}
