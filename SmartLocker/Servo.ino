const uint16_t LOCKED_PULSE   = 2000;
const uint16_t UNLOCKED_PULSE = 4000;


void initializeServo()
{
    pinMode(9, OUTPUT);

    // Clear Timer1 registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    // Timer1 counts from 0 to 39999
    // Creates approximately a 20 ms period
    ICR1 = 39999;

    // Fast PWM Mode 14
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // Non-inverting PWM on OC1A / Arduino Pin 9
    TCCR1A |= (1 << COM1A1);

    // Prescaler = 8
    TCCR1B |= (1 << CS11);

    // Start servo locked
    OCR1A = LOCKED_PULSE;
}


void lockServo()
{
    OCR1A = LOCKED_PULSE;
}


void unlockServo()
{
    OCR1A = UNLOCKED_PULSE;
}