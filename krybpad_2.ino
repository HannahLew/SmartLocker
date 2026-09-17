int sensorPin = A0;
int ledPin = 13;

int sensorValue = 0;
byte keyvalue = 13;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Started. Press a key");
}

void loop()
{
  sensorValue = analogRead(sensorPin);
  keyvalue = 13;

  digitalWrite(ledPin, HIGH);

  switch (sensorValue)
  {
    // # = 292
    case 280 ... 302:
      keyvalue = 11;
      break;

    // 0 = 313
    case 303 ... 322:
      keyvalue = 0;
      break;

    // * = 331
    case 323 ... 368:
      keyvalue = 12;
      break;

    // 9 = 405
    case 369 ... 417:
      keyvalue = 9;
      break;

    // 8 = 430
    case 418 ... 440:
      keyvalue = 8;
      break;

    // 7 = 451
    case 441 ... 559:
      keyvalue = 7;
      break;

    // 6 = 668
    case 560 ... 677:
      keyvalue = 6;
      break;

    // 5 = 687
    case 678 ... 694:
      keyvalue = 5;
      break;

    // 4 = 701
    case 695 ... 736:
      keyvalue = 4;
      break;

    // 3 = 772
    case 737 ... 781:
      keyvalue = 3;
      break;

    // 2 = 790
    case 782 ... 795:
      keyvalue = 2;
      break;

    // 1 = 805
    case 796 ... 820:
      keyvalue = 1;
      break;

    default:
      keyvalue = 13;
      break;
  }

  if (keyvalue < 13)
  {
    Serial.print("ADC = ");
    Serial.print(sensorValue);
    Serial.print("   Key = ");
    Serial.println(keyvalue);

    delay(300);
  }

  digitalWrite(ledPin, LOW);
}