int sensorPin = A0;
int ledPin = 13;
int lockPin = 2;


// System states
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


// Password variables
String enteredPass = "";
String savedPass = "";

bool passwordInitialized = false;
bool isLocked = true;


// Timer variables
volatile unsigned long timerCount = 0;

unsigned long lastActTime = 0;
unsigned long lastErrorTime = 0;
unsigned long lastKeyTime = 0;

const unsigned long inactTimeout = 10000;
const unsigned long keyDelay = 300;
const unsigned long errorDelay = 2000;


// Keypad variables
char lastKey = '\0';

int sensorValue = 0;
byte keyvalue = 13;


// EEPROM settings
const byte EEPROM_MARKER = 0xA5;
const int EEPROM_MARKER_ADDRESS = 0;
const int EEPROM_PASSWORD_ADDRESS = 1;
const int MAX_PASSWORD_LENGTH = 5;


// Setup
void setup()
{
  // trick into think no password is saved
  //EEPROMWrite(EEPROM_MARKER_ADDRESS, 0);
  
  pinMode(ledPin, OUTPUT);
  pinMode(lockPin, INPUT_PULLUP);

  Serial.begin(9600);

  // Start Timer2
  setupTimer();

  // Start Timer1 servo
  setupServo();

  isLocked = true;
  current = sleep;

  digitalWrite(ledPin, LOW);

  // Start servo in locked position
  lockServo();


  // Load password from EEPROM
  if (loadPassword())
  {
    Serial.println("Password loaded from EEPROM.");
  }
  else
  {
    Serial.println("No password found.");
    Serial.println("Enter a new password.");
  }


  Serial.println("System started.");
  Serial.println("System LOCKED.");
  Serial.println("Press a keypad key to begin.");
}


// Main loop
void loop()
{
  // update timer
  updateTimer();


  // lock button
  if (digitalRead(lockPin) == LOW)
  {
    if (isLocked == false)
    {
      current = locked;
    }
  }


  // sleep
  if (current == sleep)
  {
    sensorValue = analogRead(sensorPin);

    if (sensorValue >= 280 && sensorValue <= 820)
    {
      enteredPass = "";

      lastActTime = timerCount;

      current = enterPass;

      if (passwordInitialized)
      {
        Serial.println("Entering password.");
      }
      else
      {
        Serial.println("Enter a new password.");
      }
    }
  }


  // enter password
  else if (current == enterPass)
  {
    sensorValue = analogRead(sensorPin);
    keyvalue = 13;


    // keypad
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


    // key pressed
    if (keyvalue < 13)
    {
      // debounce key
      if (timerCount - lastKeyTime >= keyDelay)
      {
        lastKeyTime = timerCount;
        lastActTime = timerCount;


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


        // enter
        if (lastKey == '#')
        {
          // First password setup
          if (passwordInitialized == false)
          {
            if (enteredPass.length() > 0)
            {
              savePassword();

              Serial.println("Password saved to EEPROM.");

              current = unlocked;
            }
            else
            {
              Serial.println("Password cannot be empty.");
            }
          }

          // Normal password check
          else
          {
            current = checkPass;
          }
        }


        // clear
        else if (lastKey == '*')
        {
          enteredPass = "";

          Serial.println("Password cleared.");
        }


        // number
        else
        {
          if (enteredPass.length() < MAX_PASSWORD_LENGTH)
          {
            enteredPass += lastKey;

            Serial.print("Password: ");

            for (int i = 0; i < enteredPass.length(); i++)
            {
              Serial.print("*");
            }

            Serial.println();
          }
          else
          {
            Serial.println("Password is too long.");
          }
        }
      }
    }


    // timeout
    if (timerCount - lastActTime >= inactTimeout)
    {
      Serial.println("Inactivity timeout.");

      enteredPass = "";

      current = sleep;
    }
  }


  // check password
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

      lastErrorTime = timerCount;

      current = accessDenied;
    }
  }


  // unlocked
  else if (current == unlocked)
  {
    isLocked = false;

    digitalWrite(ledPin, HIGH);

    // Move servo to unlocked position
    unlockServo();

    Serial.println("SYSTEM UNLOCKED");

    enteredPass = "";

    lastActTime = timerCount;

    current = enterPass;
  }


  // locked
  else if (current == locked)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    // Move servo to locked position
    lockServo();

    enteredPass = "";

    Serial.println("SYSTEM LOCKED");

    current = sleep;
  }


  // access denied
  else if (current == accessDenied)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    // Make sure servo stays locked
    lockServo();

    if (timerCount - lastErrorTime >= errorDelay)
    {
      enteredPass = "";

      Serial.println("Try again.");

      lastActTime = timerCount;

      current = enterPass;
    }
  }
}