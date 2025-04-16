// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Matter Manager
#include <Matter.h>
#include <WiFi.h>
#include <Preferences.h>
#include <MatterEndpoints/MatterOnOffLight.h>

// WiFi is manually set and started
const char *ssid = "";          // Change this to your WiFi SSID
const char *password = "";  // Change this to your WiFi password

// List of Matter Endpoints for this Node
// On/Off Light Endpoint
MatterOnOffLight OnOffLight;

// Matter Protocol Endpoint Callback
//bool setLightOnOff(bool state) {
//  Serial.printf("User Callback :: New Light State = %s\r\n", state ? "ON" : "OFF");
/*  if (state) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  // // store last OnOff state for when the Light is restarted / power goes off
  // matterPref.putBool(onOffPrefKey, state);
  // This callback must return the success state to Matter core
  return true;
}*/

void setup() {

  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Manually connect to WiFi
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\r\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  // Matter beginning - Last step, after all EndPoints are initialized
  Matter.begin();

  OnOffLight.begin(false);

  // Initialize the LED (light) GPIO and Matter End Point
  pinMode(LED_BUILTIN, OUTPUT);

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("");
    Serial.println("Matter Node is not commissioned yet.");
    Serial.println("Initiate the device discovery in your Matter environment.");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\r\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\r\n", Matter.getOnboardingQRCodeUrl().c_str());
  }

  uint32_t timeCount = 0;
  while (!Matter.isDeviceCommissioned()) {
      delay(100);
      if ((timeCount++ % 50) == 0) {  // 50*100ms = 5 sec
        Serial.println("Matter Node not commissioned yet. Waiting for commissioning.");
      }
  }

  // This may be a restart of a already commissioned Matter accessory
  // if (Matter.isDeviceCommissioned()) {
  //   Serial.println("Matter Node is commissioned and connected to Wi-Fi. Ready for use.");
  //   Serial.printf("Initial state: %s\r\n", OnOffLight.getOnOff() ? "ON" : "OFF");
  //   OnOffLight.updateAccessory();  // configure the Light based on initial state
  // }
}

void loop() {
  // Check Matter Light Commissioning state, which may change during execution of loop()
  //  Serial.printf("Initial state: %s\r\n", OnOffLight.getOnOff() ? "ON" : "OFF");
  //  OnOffLight.onChange(setLightOnOff);
  //  OnOffLight.updateAccessory();  // configure the Light based on initial state

  static bool matter_lightbulb_last_state = false;
  bool matter_lightbulb_current_state = OnOffLight.getOnOff();

  // If the current state is ON and the previous was OFF - turn on the LED
  if (matter_lightbulb_current_state && !matter_lightbulb_last_state) {
    matter_lightbulb_last_state = matter_lightbulb_current_state;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Bulb ON");
  }

  // If the current state is OFF and the previous was ON - turn off the LED
  if (!matter_lightbulb_current_state && matter_lightbulb_last_state) {
    matter_lightbulb_last_state = matter_lightbulb_current_state;
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Bulb OFF");
  }

}
