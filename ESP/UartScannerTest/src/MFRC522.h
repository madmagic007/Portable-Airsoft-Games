#include <Arduino.h>
#include "HardwareSerial.h"

// RX, TX (adjust for your wiring)
HardwareSerial rfidSerial(1);

class MFRC522 {
public:
  HardwareSerial &rfidSerial;

  // Command codes
  static const uint8_t PCD_RESETPHASE = 0x0F;
  static const uint8_t PCD_IDLE       = 0x00;
  static const uint8_t PCD_TRANSCEIVE = 0x0C;

  // PICC commands
  static const uint8_t PICC_REQIDL    = 0x26;
  static const uint8_t PICC_ANTICOLL  = 0x93;

  // Register addresses
  static const uint8_t CommandReg     = 0x01;
  static const uint8_t FIFODataReg    = 0x09;
  static const uint8_t FIFOLevelReg   = 0x0A;
  static const uint8_t BitFramingReg  = 0x0D;
  static const uint8_t TxControlReg   = 0x14;
  static const uint8_t VersionReg     = 0x37;
  static const uint8_t SerialSpeedReg = 0x1F;

  MFRC522() {}

  void begin(HardwareSerial &serial) {
    rfidSerial = serial;
    delay(100);
    reset();
    // Timer and modulation setup (based on Python init)
    writeRegister(0x2A, 0x80);
    writeRegister(0x2B, 0xA9);
    writeRegister(0x2C, 0x03);
    writeRegister(0x2D, 0xE8);
    writeRegister(0x15, 0x40);
    writeRegister(0x11, 0x3D);
    writeRegister(0x33, 0x00);
    antennaOn();
  }

  void reset() {
    if (!writeRegister(CommandReg, PCD_RESETPHASE)) {
      rfidSerial.updateBaudRate(115200);
      writeRegister(CommandReg, PCD_RESETPHASE);
      rfidSerial.updateBaudRate(9600);
      delay(50);
    }
    writeRegister(SerialSpeedReg, 0x7A);
    rfidSerial.updateBaudRate(115200);
  }

  bool writeRegister(uint8_t addr, uint8_t val) {
    for (int i = 0; i < 10; i++) {
      flushInput();
      rfidSerial.write(addr & 0x7F);
      rfidSerial.write(val);
      delayMicroseconds(500);
      if (rfidSerial.available()) {
        uint8_t echo = rfidSerial.read();
        if (echo == addr) return true;
      }
    }
    Serial.print("Write error at addr: 0x");
    Serial.println(addr, HEX);
    return false;
  }

  uint8_t readRegister(uint8_t addr) {
    flushInput();
    rfidSerial.write(addr | 0x80);
    delayMicroseconds(500);
    if (rfidSerial.available()) {
      return rfidSerial.read();
    }
    return 0xFF;
  }

  void flushInput() {
    while (rfidSerial.available()) rfidSerial.read();
  }

  void antennaOn() {
    uint8_t temp = readRegister(TxControlReg);
    if (!(temp & 0x03)) {
      writeRegister(TxControlReg, temp | 0x03);
    }
  }

  uint8_t getFirmwareVersion() {
    return readRegister(VersionReg);
  }

  void debugDump() {
    Serial.println("=== MFRC522 UART Debug ===");
    Serial.print("VersionReg: 0x");
    Serial.println(getFirmwareVersion(), HEX);
    Serial.println("==========================");
  }
};