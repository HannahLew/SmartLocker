int sensorPin = A0;
int ledPin = 13;

void initializeKeypad()
{
    pinMode(ledPin, OUTPUT);
}



// '0'-'9' for number keys
// '*' for star
// '#' for pound

char readKeypad()
{
    int sensorValue = analogRead(sensorPin);

    char currentKey = '\0';

    switch(sensorValue)
    {
        // # = 292
        case 280 ... 302:
            currentKey = '#';
            break;

        // 0 = 313
        case 303 ... 322:
            currentKey = '0';
            break;

        // * = 331
        case 323 ... 368:
            currentKey = '*';
            break;

        // 9 = 405
        case 369 ... 417:
            currentKey = '9';
            break;

        // 8 = 430
        case 418 ... 440:
            currentKey = '8';
            break;

        // 7 = 451
        case 441 ... 559:
            currentKey = '7';
            break;

        // 6 = 668
        case 560 ... 677:
            currentKey = '6';
            break;

        // 5 = 687
        case 678 ... 694:
            currentKey = '5';
            break;

        // 4 = 701
        case 695 ... 736:
            currentKey = '4';
            break;

        // 3 = 772
        case 737 ... 781:
            currentKey = '3';
            break;

        // 2 = 790
        case 782 ... 795:
            currentKey = '2';
            break;

        // 1 = 805
        case 796 ... 820:
            currentKey = '1';
            break;

        default:
            currentKey = '\0';
            break;
    }


    // one press only 
    static char previousKey = '\0';

    // A new key was just pressed
    if(currentKey != '\0' && previousKey == '\0')
    {
        previousKey = currentKey;

        digitalWrite(ledPin, HIGH);

        return currentKey;
    }

    // Key has been released
    if(currentKey == '\0')
    {
        previousKey = '\0';
        digitalWrite(ledPin, LOW);
    }

    return '\0';
}