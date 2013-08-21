'' i2c_sharpir demo

CON
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000
  _stack        = 50
    
  sda           = 16
  scl           = 17

  addr = $26

OBJ
  Wire   : "I2Cmaster"
  Serial : "FullDuplexSerial"

pub Start | val, count, raddr, waddr, ack

  waddr := (addr<<1)
  raddr := waddr|1

  dira~
  Serial.start(31, 30, 0, 115200)
  dira[30]~~

  Serial.str(string("Hello world!"))
  Serial.tx(10)
  waitcnt(clkfreq/100 + cnt)

  Wire.init(sda, scl, Wire.FAST)

  count := 0

  repeat
    Wire.start
    ack := Wire.write(raddr)
    ifnot (ack)
      Wire.stop
      Serial.str(string("no response"))
      Serial.tx(13)
    else
      val := (Wire.read(1)<<8)
      val |= Wire.read(0)
      Wire.stop
      Serial.dec(count)
      count := count + 1
      serial.tx(32)
      Serial.dec(val)
      Serial.tx(13)
    waitcnt(clkfreq/10 + cnt)
