#include <Arduino.h>
#include <BLEDevice.h> // 1.0.0 RC4 https://github.com/espressif/arduino-esp32
#include <BLEUtils.h>  // 1.0.0 RC4 https://github.com/espressif/arduino-esp32
#include <BLEServer.h> // 1.0.0 RC4 https://github.com/espressif/arduino-esp32
#include <BLE2902.h>

// Give the ESP32 a device name. 
#define DEVICENAME  "PUMPKIN01"

// Give the service and characterists UUIDs generated with uuidgen
#define SERVICE_UUID "FD50791E-1967-479D-87FC-45DF1D7EF74D"
#define CHARACTERISTIC_UUID "A2082BD0-60BF-4D4B-BBC9-052FD7F78CA0"

// Use Digital Pin 18 (requires matching pad on Relay Board bridging)
#define RELAY_PIN   18

// Manage the switch on and off
enum State {
  TURN_ON,
  TURN_OFF,
  RESET
};

enum State state = RESET;
long timer = 0;
#define SWITCH_PERIOD_MILLIS 300

// Bluetooth Low Energy Callback
// http://www.neilkolban.com/esp32/docs/cpp_utils/html/class_b_l_e_characteristic_callbacks.html 
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    Serial.println("write Blue tooth characteristic called");
    const char *command = value.c_str();

    // Print the received command
    Serial.println(value.c_str());

    // If we receive the character '1' turn on the relay. Else turn it off.
    if (value.length() > 0) {
      if(command[0] == '1') {
        state = TURN_ON;
      }
    }
  }
};

/**
 *  Setup the hardware and Bluetooth comms
 */
void setup()
{
  // Output pin for controlling the relay
  pinMode(RELAY_PIN, OUTPUT);

  // Serial Port for monitoring and debug
  Serial.begin(115200);

  BLEDevice::init(DEVICENAME);
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  // Set the advertizing power
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  pAdvertising->start();
}

/**
 * 
 */
void turnOnRelay() {
  digitalWrite(RELAY_PIN,1);
}

/**
 * 
 */
void turnOffRelay() {
  digitalWrite(RELAY_PIN,0);
}

void loop()
{
  if (state == TURN_ON) {
    turnOnRelay();
    timer = millis();
    state = TURN_OFF;
  }

  if (state == TURN_OFF && (millis() - timer) > SWITCH_PERIOD_MILLIS) { 
    turnOffRelay();
    state = RESET;
  }
}