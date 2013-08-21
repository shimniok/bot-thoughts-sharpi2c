#include <Wire.h>

int addr = 0x26;

void setup() {
  Wire.begin(addr);
  Serial.begin(115200);
  Serial.println("Hello world!\n");
}

void loop() {
  unsigned char hi, lo;
  uint16_t value;

  while (Wire.available())
    Wire.read();
    
  Wire.requestFrom(addr, 2); // re-start, send addr, read two bytes
  if (Wire.available()) {
    hi = Wire.read(); // read first byte
    if (Wire.available()) {
      lo = Wire.read(); // read second byte
      value = hi<<8 | lo;
      Serial.print(millis());
      Serial.print(" ");
      Serial.print(hi, HEX);
      Serial.print(" ");
      Serial.print(lo, HEX);
      Serial.print(" ");
      Serial.print(value);
      Serial.println();
    }
  }

}

