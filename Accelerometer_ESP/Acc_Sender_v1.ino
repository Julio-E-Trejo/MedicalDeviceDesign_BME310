/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

#include <Wire.h>
#include <DFRobot_LIS2DH12.h>

DFRobot_LIS2DH12 LIS; //Accelerometer

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x9C, 0x9C, 0x1F, 0x47, 0x50, 0xA7};

//Accelerometer Initialization
int16_t x, y, z;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  uint16_t a;
  uint16_t b;
  uint16_t c;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup(){
  Wire.begin();
  Serial.begin(115200);
  while(!Serial);
  delay(100);

  // Set measurement range
  // Ga: LIS2DH12_RANGE_2GA
  // Ga: LIS2DH12_RANGE_4GA
  // Ga: LIS2DH12_RANGE_8GA
  // Ga: LIS2DH12_RANGE_16GA
  while(LIS.init(LIS2DH12_RANGE_16GA) == -1){  //Equipment connection exception or I2C address error
    Serial.println("No I2C devices found");
    delay(1000);
  // Init Serial Monitor
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    
    //delay(100);
    LIS.readXYZ(x, y, z);
    LIS.mgScale(x, y, z);

    Serial.print("Acceleration x: "); //print acceleration
    Serial.print(x);
    Serial.print(" mg \ty: ");
    Serial.print(y);
    Serial.print(" mg \tz: ");
    Serial.print(z);
    Serial.println(" mg");
    
    myData.a = x;
    myData.b = y;
    myData.c = z;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
  }
}
