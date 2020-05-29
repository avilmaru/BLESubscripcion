// ---------------------------------------------------------------------------
// Ejemplo subscripcion servicio para control gestual (central) - v1.0 - 26/03/2020
//
// AUTOR:
// Creado por Angel Villanueva - @avilmaru
//
// LINKS:
// Blog: http://www.mecatronicalab.es
//
//
// HISTORICO:
// 26/03/2020 v1.0 - Release inicial.
//
// ---------------------------------------------------------------------------

#include <ArduinoBLE.h>
#include <MKRMotorCarrier.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

enum {
  GESTURE_NONE = 4,
  GESTURE_UP = 0,
  GESTURE_DOWN = 1,
  GESTURE_LEFT = 2,
  GESTURE_RIGHT = 3
};

byte oldCommand = GESTURE_NONE;

void setup() {
  
  //Serial.begin(9600);
  //while (!Serial);

  // begin ble initialization
  if (!BLE.begin()) {
    //Serial.println("starting BLE failed!");
    while (1);
  }

  //Serial.println("BLE Central - gesture control");

  //Establishing the communication with the motor shield
  if (controller.begin()) 
    {
      //Serial.print("MKR Motor Shield connected, firmware version ");
      //Serial.println(controller.getFWVersion());
    } 
  else 
    {
      //Serial.println("MKR Motor Shield couldn't connect! Is the red led blinking? You may need to update the firmware with FWUpdater sketch");
      while (1);
    }

  // Reboot the motor controller; brings every value back to default
  //Serial.println("reboot");
  controller.reboot();
  delay(500);

  servo1.setAngle(95); 
  delay(500);
  

}

void loop() {
  
     connectToPeripheral();
}


void connectToPeripheral(){

  BLEDevice peripheral;

  do
  {
    //Serial.println("escaneando servicio");
     // start scanning for peripherals
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
    delay(100);
    
  } while (!peripheral);

  
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    //Serial.print("Found  ");
    //Serial.print(peripheral.address());
    //Serial.print(" '");
    //Serial.print(peripheral.localName());
    //Serial.print("' ");
    //Serial.print(peripheral.advertisedServiceUuid());
    //Serial.println();
  
    // stop scanning
    BLE.stopScan();
  
    getDataPeripheral(peripheral);
   
  }
  
}

void getDataPeripheral(BLEDevice peripheral) {

  
  // connect to the peripheral
  //Serial.println("Connecting ...");

  if (peripheral.connect()) {
    //Serial.println("Connected");
   } else {
    //Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  //Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    //Serial.println("Attributes discovered");
  } else {
    //Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
  // subscribe to the simple key characteristic
  //Serial.println("Subscribing to simple key characteristic ...");
  if (!gestureCharacteristic) {
    //Serial.println("no gesture characteristic found!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canSubscribe()) {
    //Serial.println("gesture characteristic is not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.subscribe()) {
    //Serial.println("subscription failed!");
    peripheral.disconnect();
    return;
  } else {
    //Serial.println("Subscribed");
  }
  
  while (peripheral.connected()) {

    // while the peripheral is connected

    // check if the value of the gesture characteristic has been updated
    if (gestureCharacteristic.valueUpdated()) {

      byte command;
      gestureCharacteristic.readValue(command);

      if (oldCommand == command)
        continue;

      oldCommand = command;
       
      //Serial.print("command:");
      //Serial.println(command);
      doMovement(command);
  

    }

    //Keep active the communication MKR1000 & MKRMotorCarrier
    //Ping the samd11
    controller.ping();
    //wait
    delay(50);

   
  }

  //Serial.println("Peripheral disconnected!");

}


void doMovement(byte command)
{
 
 if (command == GESTURE_UP) {
   //Serial.println("UP");
   servo1.setAngle(95);  
 }else if (command == GESTURE_LEFT) {
   //Serial.println("LEFT");
   servo1.setAngle(180);
 }else if (command == GESTURE_RIGHT) {
   //Serial.println("RIGHT");
   servo1.setAngle(0); 
 }
        
}
