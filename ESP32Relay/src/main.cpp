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

// Bluetooth Low Energy Callback
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
        digitalWrite(RELAY_PIN,1);
      } else {
        digitalWrite(RELAY_PIN,0);
      }
    } else {
      digitalWrite(RELAY_PIN,0);
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

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  pAdvertising->start();
}

void loop()
{
  // Nothing to see here
}