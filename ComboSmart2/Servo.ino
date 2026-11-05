const int servoSensorPin = A1;

const uint16_t LOCKED = 1700;
const uint16_t UNLOCKED = 3500;

// Current detection settings
const int SPIKE_START = 180;
const int NORMAL_LEVEL = 10;
const unsigned long HOLD_TIME = 10;

// Servo position tracking
uint16_t currentPosition = LOCKED;
uint16_t previousPosition = LOCKED;

// Blockage detection variables
bool watchingSpike = false;
int spikePeak = 0;
unsigned long highStart = 0;
bool blockageDetected = false;


// Set up Timer1 for servo PWM
void setupServo()
{
  pinMode(9, OUTPUT);
  pinMode(servoSensorPin, INPUT);

  // Clear Timer1 registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set Timer1 maximum count
  ICR1 = 39999;

  // Fast PWM Mode 14
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Non-inverting PWM on pin 9
  TCCR1A |= (1 << COM1A1);

  // Prescaler = 8
  TCCR1B |= (1 << CS11);

// Keep the current servo position when restarting PWM
  OCR1A = currentPosition;
}


// Move servo to locked position
void lockServo()
{
  // Remember current position before moving
  previousPosition = currentPosition;

  OCR1A = LOCKED;
  currentPosition = LOCKED;

  // Reset blockage detection
  watchingSpike = false;
  blockageDetected = false;
  highStart = 0;
  spikePeak = 0;
}


// Move servo to unlocked position
void unlockServo()
{
  // Remember current position before moving
  previousPosition = currentPosition;

  OCR1A = UNLOCKED;
  currentPosition = UNLOCKED;

  // Reset blockage detection
  watchingSpike = false;
  blockageDetected = false;
  highStart = 0;
  spikePeak = 0;
}


// Check servo current for blockage
void checkServoBlockage()
{
  int sensorValue = analogRead(servoSensorPin);

  // Start watching when current rises
  if (!watchingSpike && sensorValue >= SPIKE_START)
  {
    watchingSpike = true;
    spikePeak = sensorValue;
    highStart = millis();
  }


  // While the current spike is happening
  if (watchingSpike)
  {
    // Keep track of the highest current
    if (sensorValue > spikePeak)
    {
      spikePeak = sensorValue;
    }

    // Current is still elevated
    if (sensorValue >= SPIKE_START)
    {
      // If current stays high long enough, blockage is detected
      if (millis() - highStart >= HOLD_TIME)
      {
        blockageDetected = true;
      }
    }


    // Current returned to normal
    if (sensorValue <= NORMAL_LEVEL)
    {
      watchingSpike = false;
      highStart = 0;
      spikePeak = 0;
    }
  }


  // Return servo if blockage was detected
  if (blockageDetected)
  {
    Serial.println("BLOCKAGE DETECTED");

    // Return to previous position
    OCR1A = previousPosition;
    currentPosition = previousPosition;

    // Reset detection
    blockageDetected = false;
    watchingSpike = false;
    highStart = 0;
    spikePeak = 0;
  }
}