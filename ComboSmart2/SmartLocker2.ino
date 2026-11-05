int sensorPin = A0;
int ledPin = 13;
int lockPin = 2;


// System states
enum systemState
{
  lockerIdle,
  enterPass,
  checkPass,
  unlocked,
  locked,
  accessDenied
};

systemState current = lockerIdle;


// Password variables
String enteredPass = "";

bool passwordInitialized = false;
bool isLocked = true;


// Timer variables
volatile unsigned long timerCount = 0;

unsigned long lastActTime = 0;
unsigned long lastErrorTime = 0;
unsigned long lastKeyTime = 0;

const unsigned long inactTimeout = 10000; // 10 sec
const unsigned long keyDelay = 300;
const unsigned long errorDelay = 2000;


// Keypad variables
char lastKey = '\0';

int sensorValue = 0;
byte keyvalue = 13;


// EEPROM settings
const byte EEPROM_MARKER = 0xB7;
const int EEPROM_MARKER_ADDRESS = 0;
const int EEPROM_PASSWORD_ADDRESS = 1;
const int MAX_PASSWORD_LENGTH = 20;


// Sleep mode
volatile bool wakeFlag = false;


// Wake when A0 changes
ISR(PCINT1_vect)
{
  wakeFlag = true;
}


// Wake when the lock button is pressed
ISR(INT0_vect)
{
  wakeFlag = true;
}


// Set up wake-up interrupts
void setupWakeInterrupts()
{
  // Enable pin-change interrupt for A0 (PCINT8)
  PCMSK1 |= (1 << PCINT8);
  PCIFR |= (1 << PCIF1);
  PCICR |= (1 << PCIE1);

  // Configure D2 (INT0) to trigger on falling edge
  EICRA = (EICRA & ~((1 << ISC01) | (1 << ISC00))) |
          (1 << ISC01);

  EIFR |= (1 << INTF0);
  EIMSK |= (1 << INT0);
}


// Enter Power-down sleep
void enterPowerDown()
{
  // Only sleep while locked and idle
  if (!isLocked || current != lockerIdle)
  {
    return;
  }

  // Check whether a keypad key is already pressed
  sensorValue = analogRead(sensorPin);

  if (sensorValue >= 280 && sensorValue <= 820)
  {
    return;
  }

  // Disable ADC to reduce power
  ADCSRA &= ~(1 << ADEN);

  // Clear pending interrupt flags
  PCIFR |= (1 << PCIF1);
  EIFR |= (1 << INTF0);

  // Select Power-down mode and enable sleep
  SMCR = (1 << SM1) | (1 << SE);

  // Enable interrupts and enter sleep
  __asm__ __volatile__(
    "sei" "\n\t"
    "sleep" "\n\t"
    "cli" "\n\t"
    ::: "memory"
  );

  // Disable sleep after waking
  SMCR &= ~(1 << SE);

  // Re-enable ADC
  ADCSRA |= (1 << ADEN);

  // Enable interrupts again
  sei();

  // Check the keypad after waking
  sensorValue = analogRead(sensorPin);

  if (sensorValue >= 280 && sensorValue <= 820)
  {
    wakeFlag = true;
  }
}


// Setup
void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(lockPin, INPUT_PULLUP);

  Serial.begin(9600);

  // Start Timer2
  setupTimer();

  // Start servo PWM
  setupServo();

  isLocked = true;
  current = lockerIdle;

  digitalWrite(ledPin, LOW);

  // Start servo in locked position
  lockServo();

  // Set up sleep wake-up interrupts
  setupWakeInterrupts();

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
  
  // Check servo current for blockage
  checkServoBlockage();

  // Update timer
  updateTimer();


  // Lock button
  if (digitalRead(lockPin) == LOW)
  {
    if (isLocked == false)
    {
      current = locked;
    }
  }


  // Idle / sleep
  if (current == lockerIdle)
  {
    if (isLocked)
    {
      enterPowerDown();
    }

    // Check keypad after waking
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


  // Enter password
  else if (current == enterPass)
  {
    sensorValue = analogRead(sensorPin);
    keyvalue = 13;

    // Convert ADC reading to a key
    switch (sensorValue)
    {
      case 280 ... 302:
        keyvalue = 11;  // #
        break;

      case 303 ... 322:
        keyvalue = 0;   // 0
        break;

      case 323 ... 368:
        keyvalue = 12;  // *
        break;

      case 369 ... 417:
        keyvalue = 9;   // 9
        break;

      case 418 ... 440:
        keyvalue = 8;   // 8
        break;

      case 441 ... 559:
        keyvalue = 7;   // 7
        break;

      case 560 ... 677:
        keyvalue = 6;   // 6
        break;

      case 678 ... 694:
        keyvalue = 5;   // 5
        break;

      case 695 ... 736:
        keyvalue = 4;   // 4
        break;

      case 737 ... 781:
        keyvalue = 3;   // 3
        break;

      case 782 ... 795:
        keyvalue = 2;   // 2
        break;

      case 796 ... 820:
        keyvalue = 1;   // 1
        break;

      default:
        keyvalue = 13;  // No key
        break;
    }


    // Key pressed
    if (keyvalue < 13)
    {
      // Debounce key
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


        // Enter key
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


        // Clear password
        else if (lastKey == '*')
        {
          enteredPass = "";

          Serial.println("Password cleared.");
        }


        // Add a number
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


    // Inactivity timeout
    if (timerCount - lastActTime >= inactTimeout)
    {
      Serial.println("Inactivity timeout.");

      enteredPass = "";

      current = lockerIdle;
    }
  }


  // Check password
  else if (current == checkPass)
  {
    Serial.println("Checking password.");

    if (verifyPassword(enteredPass))
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


  // Unlocked
  else if (current == unlocked)
  {
    isLocked = false;

    digitalWrite(ledPin, HIGH);

    // Restart Timer1 PWM and move servo to unlocked position
    setupServo();
    unlockServo();

    Serial.println("SYSTEM UNLOCKED");

    enteredPass = "";

    lastActTime = timerCount;

    current = enterPass;
  }


  // Locked
  else if (current == locked)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    // Restart Timer1 PWM and move servo to locked position
    setupServo();
    lockServo();

    enteredPass = "";

    Serial.println("SYSTEM LOCKED");

    current = lockerIdle;
  }


  // Access denied
  else if (current == accessDenied)
  {
    isLocked = true;

    digitalWrite(ledPin, LOW);

    // Restart Timer1 PWM and keep servo locked
    setupServo();
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
