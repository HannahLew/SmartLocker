int sensorPin = A0;
int ledPin = 13;
int lockPin = 2;

enum systemState
{
  sleep,
  enterPass,
  checkPass,
  unlocked,
  locked,
  accessDenied
};

systemState current = sleep;

String enteredPass = "";
String savedPass = "1234";

unsigned long lastActTime = 0;
unsigned long lastErrorTime = 0;

const unsigned long inactTimeout = 10000;

char lastKey = '\0';
bool isLocked = true;

int sensorValue = 0;
byte keyvalue = 13;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(lockPin, INPUT_PULLUP);

  Serial.begin(9600);

  isLocked = true;
  current = sleep;

  digitalWrite(ledPin, LOW);

  Serial.println("System started.");
  Serial.println("System LOCKED.");
  Serial.println("Press a keypad key to begin.");
}

void loop()
{
  //lock button
  if (digitalRead(lockPin) == LOW)
  {
    if (isLocked == false)
    {
      current = locked;
    }

    delay(50);
  }

  //sleep
  if (current == sleep)
  {
    sensorValue = analogRead(sensorPin);

    if (sensorValue >= 280 && sensorValue <= 820)
    {
      enteredPass = "";

      lastActTime = millis();

      current = enterPass;

      Serial.println("Entering password.");

      delay(300);
    }
  }

  //enter password
  else if (current == enterPass)
  {
    sensorValue = analogRead(sensorPin);
    keyvalue = 13;

    //keypad
    switch (sensorValue)
    {
      //#
      case 280 ... 302:
        keyvalue = 11;
        break;

      //0
      case 303 ... 322:
        keyvalue = 0;
        break;

      //*
      case 323 ... 368:
        keyvalue = 12;
        break;

      //9
      case 369 ... 417:
        keyvalue = 9;
        break;

      //8
      case 418 ... 440:
        keyvalue = 8;
        break;

      //7
      case 441 ... 559:
        keyvalue = 7;
        break;

      //6
      case 560 ... 677:
        keyvalue = 6;
        break;

      //5
      case 678 ... 694:
        keyvalue = 5;
        break;

      //4
      case 695 ... 736:
        keyvalue = 4;
        break;

      //3
      case 737 ... 781:
        keyvalue = 3;
        break;

      //2
      case 782 ... 795:
        keyvalue = 2;
        break;

      //1
      case 796 ... 820:
        keyvalue = 1;
        break;

      default:
        keyvalue = 13;
        break;
    }

    //key pressed
    if (keyvalue < 13)
    {
      lastActTime = millis();

      if (keyvalue == 11)
      {
        lastKey = '#';
      }
      else if (keyvalue == 12)
      {
        lastKey = '*';
      }
      else
      {
        lastKey = '0' + keyvalue;
      }

      Serial.print("Key pressed: ");
      Serial.println(lastKey);

      //enter
      if (lastKey == '#')
      {
        current = checkPass;
      }

      //clear
      else if (lastKey == '*')
      {
        enteredPass = "";

        Serial.println("Password cleared.");
      }

      //number
      else
      {
        enteredPass += lastKey;

        Serial.print("Password: ");

        for (int i = 0; i < enteredPass.length(); i++)
        {
          Serial.print("*");
        }

        Serial.println();
      }

      delay(300);
    }

    //timeout
    if (millis() - lastActTime >= inactTimeout)
    {
      Serial.println("Inactivity timeout.");

      enteredPass = "";

      current = sleep;
    }
  }

  //check password
  else if (current == checkPass)
  {
    Serial.println("Checking password.");

    if (enteredPass == savedPass)
    {
      current = unlocked;
    }
    else
    {
      Serial.println("ACCESS DENIED");

      lastErrorTime = millis();

      current = accessDenied;
    }
  }

  //unlocked
  else if (current == unlocked)
  {
    isLocked = false;

    digitalWrite(ledPin, HIGH);

    Serial.println("SYSTEM UNLOCKED");

    enteredPass = "";

    lastActTime = millis();

    current = enterPass;

    delay(500);
  }

  //locked
  else if (current == locked)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    enteredPass = "";

    Serial.println("SYSTEM LOCKED");

    current = sleep;

    delay(500);
  }

  //access denied
  else if (current == accessDenied)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    if (millis() - lastErrorTime >= 2000)
    {
      enteredPass = "";

      Serial.println("Try again.");

      lastActTime = millis();

      current = enterPass;
    }
  }
}