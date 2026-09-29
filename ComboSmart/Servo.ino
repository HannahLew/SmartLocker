const uint16_t LOCKED = 1500;
const uint16_t UNLOCKED = 4500;


void setupServo()
{
  pinMode(9, OUTPUT);

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

  // Start locked
  OCR1A = LOCKED;
}


void lockServo()
{
  OCR1A = LOCKED;
}


void unlockServo()
{
  OCR1A = UNLOCKED;
}