// Timer2 setup
void setupTimer()
{
  // Clear Timer2 registers
  // TCCR2A = Timer/Counter2 Control Register A
  TCCR2A = 0;

  // TCCR2B = Timer/Counter2 Control Register B
  TCCR2B = 0;

  // TCNT2 = Timer2 Counter
  TCNT2 = 0;

  // CTC mode
  TCCR2A |= (1 << WGM21);

  // Prescaler = 64
  TCCR2B |= (1 << CS22);

  // Compare value
  OCR2A = 249;

  // Clear compare flag
  TIFR2 |= (1 << OCF2A);
}


// Update system timer
void updateTimer()
{
  if (TIFR2 & (1 << OCF2A))
  {
    TIFR2 |= (1 << OCF2A);

    timerCount++;
  }
}


// EEPROM write
void EEPROMWrite(int address, byte data)
{
  // Wait for previous EEPROM write to finish
  while (EECR & (1 << EEPE))
  {
  }

  // Set EEPROM address
  EEAR = address;

  // Set EEPROM data
  EEDR = data;

  // Start EEPROM write
  EECR |= (1 << EEMPE);
  EECR |= (1 << EEPE);
}


// EEPROM read
byte EEPROMRead(int address)
{
  // Wait for previous EEPROM write to finish
  while (EECR & (1 << EEPE))
  {
  }

  // Set EEPROM address
  EEAR = address;

  // Start EEPROM read
  EECR |= (1 << EERE);

  return EEDR;
}


// Save password to EEPROM
void savePassword()
{
  int length = enteredPass.length();

  // Make sure password is not too long
  if (length > MAX_PASSWORD_LENGTH)
  {
    length = MAX_PASSWORD_LENGTH;
  }

  // Save password length
  EEPROMWrite(EEPROM_PASSWORD_ADDRESS, length);

  // Save each password character
  for (int i = 0; i < length; i++)
  {
    EEPROMWrite(EEPROM_PASSWORD_ADDRESS + 1 + i, enteredPass[i]);
  }

  // Save marker after password is written
  EEPROMWrite(EEPROM_MARKER_ADDRESS, EEPROM_MARKER);

  savedPass = enteredPass;

  passwordInitialized = true;
}


// Load password from EEPROM
bool loadPassword()
{
  byte marker = EEPROMRead(EEPROM_MARKER_ADDRESS);

  // Check if a password has already been saved
  if (marker != EEPROM_MARKER)
  {
    return false;
  }

  // Read password length
  byte length = EEPROMRead(EEPROM_PASSWORD_ADDRESS);

  // Check password length
  if (length == 0 || length > MAX_PASSWORD_LENGTH)
  {
    return false;
  }

  savedPass = "";

  // Read each password character
  for (int i = 0; i < length; i++)
  {
    char passwordChar = EEPROMRead(EEPROM_PASSWORD_ADDRESS + 1 + i);

    savedPass += passwordChar;
  }

  passwordInitialized = true;

  return true;
}