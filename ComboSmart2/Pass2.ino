#include <stdint.h>

// Timer2 setup
void setupTimer()
{
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);
  OCR2A = 249;
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
  while (EECR & (1 << EEPE))
  {
  }

  EEAR = address;
  EEDR = data;
  EECR |= (1 << EEMPE);
  EECR |= (1 << EEPE);
}


// EEPROM read
byte EEPROMRead(int address)
{
  while (EECR & (1 << EEPE))
  {
  }

  EEAR = address;
  EECR |= (1 << EERE);
  return EEDR;
}


// SHA-256 constants
const uint32_t shaK[64] =
{
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

uint32_t rotateRight(uint32_t value, byte amount)
{
  return (value >> amount) | (value << (32 - amount));
}


// SHA-256 for inputs up to 55 bytes (one 512-bit block)
void sha256(const byte *data, byte length, byte output[32])
{
  uint32_t h[8] =
  {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
  };

  byte block[64] = {0};
  for (byte i = 0; i < length; i++)
  {
    block[i] = data[i];
  }

  block[length] = 0x80;
  uint64_t bitLength = (uint64_t)length * 8;

  for (byte i = 0; i < 8; i++)
  {
    block[63 - i] = (byte)(bitLength >> (i * 8));
  }

  uint32_t w[64];

  for (byte i = 0; i < 16; i++)
  {
    byte j = i * 4;
    w[i] = ((uint32_t)block[j] << 24) |
           ((uint32_t)block[j + 1] << 16) |
           ((uint32_t)block[j + 2] << 8) |
           block[j + 3];
  }

  for (byte i = 16; i < 64; i++)
  {
    uint32_t s0 = rotateRight(w[i - 15], 7) ^
                  rotateRight(w[i - 15], 18) ^ (w[i - 15] >> 3);
    uint32_t s1 = rotateRight(w[i - 2], 17) ^
                  rotateRight(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }

  uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
  uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

  for (byte i = 0; i < 64; i++)
  {
    uint32_t s1 = rotateRight(e, 6) ^ rotateRight(e, 11) ^ rotateRight(e, 25);
    uint32_t choice = (e & f) ^ (~e & g);
    uint32_t temp1 = hh + s1 + choice + shaK[i] + w[i];
    uint32_t s0 = rotateRight(a, 2) ^ rotateRight(a, 13) ^ rotateRight(a, 22);
    uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
    uint32_t temp2 = s0 + majority;

    hh = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }

  h[0] += a; h[1] += b; h[2] += c; h[3] += d;
  h[4] += e; h[5] += f; h[6] += g; h[7] += hh;

  for (byte i = 0; i < 8; i++)
  {
    output[i * 4] = (byte)(h[i] >> 24);
    output[i * 4 + 1] = (byte)(h[i] >> 16);
    output[i * 4 + 2] = (byte)(h[i] >> 8);
    output[i * 4 + 3] = (byte)h[i];
  }
}


// EEPROM layout: marker at 0, salt at 1-16, hash at 17-48
const int SALT_ADDRESS = 1;
const int HASH_ADDRESS = 17;
const byte SALT_LENGTH = 16;
const byte HASH_LENGTH = 32;
const uint16_t HASH_ROUNDS = 1000;

byte passwordSalt[SALT_LENGTH];
byte savedHash[HASH_LENGTH];


// Generate a salt for this password.
// The salt is not secret; it only needs to be different for each password.
void generateSalt()
{
  uint32_t seed = micros() ^ (uint32_t)timerCount ^
                  ((uint32_t)analogRead(A0) << 16);

  if (seed == 0)
  {
    seed = 0xA5367B19;
  }

  for (byte i = 0; i < SALT_LENGTH; i++)
  {
    // Xorshift pseudo-random sequence
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    passwordSalt[i] = (byte)seed;
  }
}


// Hash a password with its salt and repeat the hash to slow guessing.
void hashPassword(const String &password, byte output[HASH_LENGTH])
{
  byte input[48];
  byte digest[HASH_LENGTH];
  byte length = password.length();

  for (byte i = 0; i < length; i++)
  {
    input[i] = (byte)password[i];
  }

  for (byte i = 0; i < SALT_LENGTH; i++)
  {
    input[length + i] = passwordSalt[i];
  }

  sha256(input, length + SALT_LENGTH, digest);

  // Additional rounds make each password guess take longer.
  for (uint16_t round = 0; round < HASH_ROUNDS; round++)
  {
    for (byte i = 0; i < HASH_LENGTH; i++)
    {
      input[i] = digest[i];
    }

    for (byte i = 0; i < SALT_LENGTH; i++)
    {
      input[HASH_LENGTH + i] = passwordSalt[i];
    }

    sha256(input, HASH_LENGTH + SALT_LENGTH, digest);
  }

  for (byte i = 0; i < HASH_LENGTH; i++)
  {
    output[i] = digest[i];
  }
}


// Save the salted password hash to EEPROM
void savePassword()
{
  if (enteredPass.length() == 0 || enteredPass.length() > MAX_PASSWORD_LENGTH)
  {
    return;
  }

  // Invalidate the old record until the new one is completely written.
  EEPROMWrite(EEPROM_MARKER_ADDRESS, 0);

  generateSalt();
  hashPassword(enteredPass, savedHash);

  for (byte i = 0; i < SALT_LENGTH; i++)
  {
    EEPROMWrite(SALT_ADDRESS + i, passwordSalt[i]);
  }

  for (byte i = 0; i < HASH_LENGTH; i++)
  {
    EEPROMWrite(HASH_ADDRESS + i, savedHash[i]);
  }

  // Write marker last so incomplete data is not accepted.
  EEPROMWrite(EEPROM_MARKER_ADDRESS, EEPROM_MARKER);
  passwordInitialized = true;
}


// Load the salt and hash from EEPROM. The original password is never loaded.
bool loadPassword()
{
  if (EEPROMRead(EEPROM_MARKER_ADDRESS) != EEPROM_MARKER)
  {
    return false;
  }

  for (byte i = 0; i < SALT_LENGTH; i++)
  {
    passwordSalt[i] = EEPROMRead(SALT_ADDRESS + i);
  }

  for (byte i = 0; i < HASH_LENGTH; i++)
  {
    savedHash[i] = EEPROMRead(HASH_ADDRESS + i);
  }

  passwordInitialized = true;
  return true;
}


// Verify an entered password by hashing it and comparing the result.
bool verifyPassword(const String &password)
{
  byte enteredHash[HASH_LENGTH];
  byte difference = 0;

  hashPassword(password, enteredHash);

  // Compare every byte rather than stopping at the first mismatch.
  for (byte i = 0; i < HASH_LENGTH; i++)
  {
    difference |= enteredHash[i] ^ savedHash[i];
  }

  return difference == 0;
}
