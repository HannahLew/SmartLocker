
const uint16_t LOCKED   = 1500; 
const uint16_t UNLOCKED = 4500; 

void setup()
{
    pinMode(9, OUTPUT);

    // Clear Timer1 registers
    //TCCR1A = timer/counter1 control registers A
    TCCR1A = 0;
    //TCCR1B= timer/counter1 control register B
    TCCR1B = 0;
    // TCNT1 = timer1 counter
    TCNT1  = 0;

    //ICR1= Input Capture Register 1
    //tells timer1 max count
    ICR1 = 39999;

    // Fast PWM Mode 14
    // TOP = ICR1
    //WGM = Waveform Gerneation Mode
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // Non-inverting PWM on Arduino Pin 9
// COM1A = compare output mode for timer1 channel A
    TCCR1A |= (1 << COM1A1);

    // Prescaler = 8
    // CS = clock select
    TCCR1B |= (1 << CS11);

    Serial.begin(9600);

    // Start locked
    // OCR1A = Output Compare Register 1A
    OCR1A = LOCKED;
}

void loop()
{
    if (Serial.available())
    {
        char command = Serial.read();

        if (command == 'L' || command == 'l')
        {
            OCR1A = LOCKED;
            Serial.println("LOCKED");
        }

        if (command == 'U' || command == 'u')
        {
            OCR1A = UNLOCKED;
            Serial.println("UNLOCKED");
        }
    }
}