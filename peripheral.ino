/*
  BLE_Peripheral.ino
*/

#include <ArduinoBLE.h>
#include <Wire.h>
#include <MPU6050_light.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int gesture = -1;

BLEService gestureService(deviceServiceUuid); 
BLEFloatCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);

MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);  
  
  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Arduino Nano 33 BLE (Peripheral)");
  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gestureCharacteristic);
  BLE.addService(gestureService);
  gestureCharacteristic.writeValue(-1);
  BLE.advertise();

  Wire.begin();
  mpu.begin();       
  mpu.calcGyroOffsets();                          // This does the calibration of the gyroscope  

  Serial.println("Nano 33 BLE (Peripheral Device)");
  Serial.println(" ");
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      mpu.update();  
      if((millis()-timer)>10)                         // print data every 10ms
      {                                                                 
        float test = mpu.getAngleX();
        
        timer = millis();

        gestureCharacteristic.writeValue(test);
        
        Serial.println(gestureCharacteristic.value());
      }
    }
    
    Serial.println("* Disconnected to central device!");
  }
}
