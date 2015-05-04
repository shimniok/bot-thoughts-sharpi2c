# Read The Sensor #

  * Send START
  * Send Read Address
  * READ high byte
  * READ low byte
  * Send STOP

By default, without specifying any other [register](Registers.md) to read, the Adapter will respond with the high, then low byte of the raw Sensor conversion. Repeat the above sequence over and over. In C to put the values back into a uint16\_t:
```
uint16_t value;
uint8_t hibyte, lowbyte;
value = hibyte << 8;
value |= lowbyte;
```

# Read The Sensor, 8-Bit #

  * Send START
  * Send Read Address
  * READ high byte
  * Send STOP

If you want a quick 8-bit value, just read once after sending the start and the Read Address. How easy is that? Repeat the above sequence over and over.

# Read A Register #

  * Send START
  * Send Write Address
  * Send Register Index
  * Send RESTART
  * Send Read Address
  * Read byte
  * Send STOP

If you want to read a particular [register](Registers.md), simply write the requested register index, restart, and read the byte.

# Write A Register #

  * Send START
  * Send Write Address
  * Send Register Index
  * Send new register value
  * Send STOP

Just write to the adapter with the [register](Registers.md) index and the new value.

# Read Multiple Registers #

  * Send START
  * Send Write Address
  * Send Register Index
  * Send RESTART
  * Send Read Address
  * Read byte
  * Repeat above n times
  * Send STOP

To read multiple registers, simply send a write with the starting register, then read as many times as you like to get as many registers as you like.