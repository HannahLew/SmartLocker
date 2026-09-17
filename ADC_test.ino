int sensorPin = A0;        // select the input pin for the potentiometer
int sensorValue = 0;       // variable to store the value coming from the sensor

void setup()
{
  Serial.begin(9600);
  Serial.println("Ready - press a key");
}

void loop()
{
  sensorValue = analogRead(sensorPin);

  // Only print when the reading changes significantly
  static int lastValue = -1;

  if (abs(sensorValue - lastValue) > 5)
  {
    Serial.print("ADC = ");
    Serial.println(sensorValue);
    lastValue = sensorValue;
  }

  delay(50);
}