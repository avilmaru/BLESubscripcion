// ---------------------------------------------------------------------------
// Ejemplo subscripcion servicio para control gestual (periférico)  - v1.0 - 26/03/2020
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
#include <Arduino_APDS9960.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

const int R_LED_PIN = 22;
const int G_LED_PIN = 23;
const int B_LED_PIN = 24;

int gesture = 4; 
int oldGestureValue = 4;   

// BLE gesture Service
BLEService gestureService(deviceServiceUuid); 

// BLE gesture Switch Characteristic 
BLEByteCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLENotify);


void setup() {
  
  //Serial.begin(9600);
  //while (!Serial);

  setColor("RED");

  if (!APDS.begin()) {
    //Serial.println("Error initializing APDS9960 sensor!");
  }

   APDS.setGestureSensitivity(80); // [1..100]
   
  
  // begin ble initialization
  if (!BLE.begin()) {
    //Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Gesture peripheral");
  BLE.setAdvertisedService(gestureService);

  // add the characteristic to the service
  gestureService.addCharacteristic(gestureCharacteristic);

  // add service
  BLE.addService(gestureService);

  // set the initial value for the characeristic:
  gestureCharacteristic.writeValue(4);

  // start advertising
  BLE.advertise();

  //Serial.println("BLE gesture Peripheral");
}

void loop() {
  
  // listen for BLE centrals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {

    setColor("BLUE");
      
    //Serial.print("Connected to central: ");
    // print the central's MAC address:
    //Serial.println(central.address());

      while (central.connected()) { 
  
        gesture = gestureDetectection();

        if (gesture == 4)
          continue;
          
        if (oldGestureValue != gesture) {
  
          // gesture value changed
          oldGestureValue = gesture;
              
          //Serial.print("writing value: ");
          //Serial.println(gesture);
          if (gestureCharacteristic.subscribed())
            gestureCharacteristic.writeValue((byte)gesture);
          //Serial.println("done!");
    
        }
    
    }


    // when the central disconnects, print it out:
    //Serial.print(F("Disconnected from central: "));
    //Serial.println(central.address());
    
  }
  
}



int gestureDetectection(){

  int gesture = 4;
  
  if (APDS.gestureAvailable()) {
    
    // a gesture was detected
    gesture = APDS.readGesture();

    switch (gesture) {
      case GESTURE_UP:
        //Serial.println("Detected UP gesture");
        break;
      case GESTURE_DOWN:
        //Serial.println("Detected DOWN gesture");
        break;
      case GESTURE_LEFT:
        //Serial.println("Detected LEFT gesture");
        break;
      case GESTURE_RIGHT:
        //Serial.println("Detected RIGHT gesture");
        break;
      default:
        //Serial.println("NO gesture detected!");
        break;
      }
    
    }

    return gesture;
    
}


void setColor(String color)
{

  if (color == "RED")
  {
    digitalWrite(R_LED_PIN, LOW);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, HIGH);
    
  }else if (color == "GREEN")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, LOW);
    digitalWrite(B_LED_PIN, HIGH);
    
  }else if (color == "BLUE")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, LOW);
    
  }else if (color == "BLACK")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, HIGH);
  }        

}
