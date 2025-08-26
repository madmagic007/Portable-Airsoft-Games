#include <Arduino.h>
#include "esp_now.h"
#include <esp_wifi.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {
  char msg[32];
} struct_message;

esp_now_peer_info_t peerInfo;
struct_message sendData;
struct_message recData;


void printMAC(const uint8_t *baseMac) {
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}

void getMac() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_AP, baseMac);
  if (ret == ESP_OK) {
    printMAC(baseMac);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recData, incomingData, sizeof(recData));
  Serial.println(recData.msg);
  
  Serial.println(recData.msg == "ping");
  if (strcmp(recData.msg, "ping") != 0) return;

  strcpy(sendData.msg, "ack");
  esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
}

// Sender callback function
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  delay(2000);

  WiFi.mode(WIFI_AP_STA);
  getMac();

  //WiFi.setTxPower(WIFI_POWER_20_5dBm);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_LR);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  Serial.println("Finished initializing ESP-NOW");

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("added peer");
}

void loop() {
  if (Serial.available()) {
    String read = Serial.readString();

    strcpy(sendData.msg, "ping");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
}