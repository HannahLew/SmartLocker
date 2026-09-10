int sensorPin = A0;   // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
byte keyvalue;

void setup() 
{
 pinMode(ledPin, OUTPUT);   // declare the ledPin as an OUTPUT:
 Serial.begin(9600); 
 Serial.println("Started. Press a key");
}

void loop() 
{
 sensorValue = analogRead(sensorPin); // read the value from the sensor:
 digitalWrite(ledPin, HIGH);          // turn the ledPin on
 switch(sensorValue)
  {
    case   0 ... 29:  keyvalue = 13; break;   // noise
    case 30 ... 50:  keyvalue = 12; break;   // * 
    case 51 ... 70:  keyvalue =  7; break;   // 7 
    case 71 ... 90:  keyvalue = 0; break;   // 0 
    case 91 ... 110:  keyvalue =  11; break;   // # 
    case 111 ... 130:  keyvalue =  8; break;   // 8 
    case 131 ... 165:  keyvalue =  9; break;   // 4 
    case 166 ... 200:  keyvalue =  4; break;   // 9 
    case 201 ... 290:  keyvalue =  5; break;   // 5 
    case 291 ... 350:  keyvalue =  1; break;   // 1 
    case 351 ... 400:  keyvalue =  6; break;   // 6 
    case 401 ... 510:  keyvalue =  2; break;   // 2 
    case 511 ... 610: keyvalue =  3; break;   // 3
  }
 if(keyvalue<13) { Serial.println(keyvalue); delay(300); }
 digitalWrite(ledPin, LOW);   // turn the ledPin off:
}