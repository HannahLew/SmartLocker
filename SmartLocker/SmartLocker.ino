enum State
{
    LOCKED,
    ENTER_PASSWORD,
    CHECK_PASSWORD,
    UNLOCKED,
    ERROR_STATE
};

State currentState = LOCKED;


// PASSWORD 

char enteredPassword[5];


char savedPassword[5] = "1234";

int digitIndex = 0;


// LOCK BUTTON

const int lockButtonPin = 2;


void setup()
{
    Serial.begin(9600);

    initializeKeypad();
    initializeServo();

    // Button connects Pin 2 to GND when pressed
    pinMode(lockButtonPin, INPUT_PULLUP);

    // Locker starts locked
    lockServo();

    Serial.println("Smart Locker Started");
    Serial.println("Enter password and press #");
}


void loop()
{
    char key = readKeypad();

    switch(currentState)
    {

        // LOCKED

        case LOCKED:

            // Start password entry when a number is pressed

            if(key >= '0' && key <= '9')
            {
                digitIndex = 0;

                enteredPassword[digitIndex] = key;
                digitIndex++;

                Serial.print("*");

                currentState = ENTER_PASSWORD;
            }

            break;

        // ENTER PASSWORD

        case ENTER_PASSWORD:

            // Store number keys
            if(key >= '0' && key <= '9')
            {
                if(digitIndex < 4)
                {
                    enteredPassword[digitIndex] = key;
                    digitIndex++;

                    Serial.print("*");
                }
            }


            // # submits password
            if(key == '#')
            {
                enteredPassword[digitIndex] = '\0';

                currentState = CHECK_PASSWORD;
            }


            // * clears password
            if(key == '*')
            {
                digitIndex = 0;

                Serial.println();
                Serial.println("Password cleared");
            }

            break;


        // =====================================
        // CHECK PASSWORD
        // =====================================

        case CHECK_PASSWORD:

            if(passwordMatches())
            {
                Serial.println();
                Serial.println("Correct password");

                // Move servo to unlocked position
                unlockServo();

                Serial.println("UNLOCKED");

                currentState = UNLOCKED;
            }

            else
            {
                currentState = ERROR_STATE;
            }

            break;


        // UNLOCKED

        case UNLOCKED:

            if(digitalRead(lockButtonPin) == LOW)
            {
                lockServo();

                Serial.println("LOCKED");

                // Clear old password
                digitIndex = 0;

                currentState = LOCKED;
            }

            break;


        // INCORRECT PASSWORD

        case ERROR_STATE:

            Serial.println();
            Serial.println("Incorrect password");

            digitIndex = 0;

            currentState = LOCKED;

            break;
    }
}