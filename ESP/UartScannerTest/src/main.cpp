#include "Arduino.h"

class MFRC522_UART {
public:
  HardwareSerial &ser;
  static const uint8_t MAX_LEN = 16;

  // --- Commands ---
  static const uint8_t PCD_IDLE       = 0x00;
  static const uint8_t PCD_MEM        = 0x01;
  static const uint8_t PCD_RNDID      = 0x02;
  static const uint8_t PCD_CALCCRC    = 0x03;
  static const uint8_t PCD_TRANSMIT   = 0x04;
  static const uint8_t PCD_AUTHENT    = 0x0E;
  static const uint8_t PCD_RECEIVE    = 0x08;
  static const uint8_t PCD_TRANSCEIVE = 0x0C;
  static const uint8_t PCD_RESETPHASE = 0x0F;

  // --- PICC Commands ---
  static const uint8_t PICC_REQA      = 0x26;
  static const uint8_t PICC_WUPA      = 0x52;
  static const uint8_t PICC_ANTICOLL  = 0x93;
  static const uint8_t PICC_SELECTTAG = 0x93;
  static const uint8_t PICC_AUTHENT1A = 0x60;
  static const uint8_t PICC_AUTHENT1B = 0x61;
  static const uint8_t PICC_READ      = 0x30;
  static const uint8_t PICC_WRITE     = 0xA0;
  static const uint8_t PICC_HALT      = 0x50;

  // --- Status codes ---
  static const uint8_t MI_OK       = 0;
  static const uint8_t MI_NOTAGERR = 1;
  static const uint8_t MI_ERR      = 2;

  // --- Registers ---
  static const uint8_t CommandReg     = 0x01;
  static const uint8_t CommIEnReg     = 0x02;
  static const uint8_t DivIrqReg      = 0x05;
  static const uint8_t ErrorReg       = 0x06;
  static const uint8_t FIFODataReg    = 0x09;
  static const uint8_t FIFOLevelReg   = 0x0A;
  static const uint8_t ControlReg     = 0x0C;
  static const uint8_t BitFramingReg  = 0x0D;
  static const uint8_t ModeReg        = 0x11;
  static const uint8_t TxControlReg   = 0x14;
  static const uint8_t TxASKReg       = 0x15;
  static const uint8_t TModeReg       = 0x2A;
  static const uint8_t TPrescalerReg  = 0x2B;
  static const uint8_t TReloadRegH    = 0x2C;
  static const uint8_t TReloadRegL    = 0x2D;
  static const uint8_t VersionReg     = 0x37;
  static const uint8_t SerialSpeedReg = 0x1F;
  static const uint8_t CRCResultRegL  = 0x22;
  static const uint8_t CRCResultRegM  = 0x21;

  MFRC522_UART(HardwareSerial &serial) : ser(serial) {}

  void begin() {
    ser.begin(9600, SERIAL_8N1, 3, 6);
    delay(100);
    reset();

    writeRegister(TModeReg, 0x80);
    writeRegister(TPrescalerReg, 0xA9);
    writeRegister(TReloadRegH, 0x03);
    writeRegister(TReloadRegL, 0xE8);
    writeRegister(TxASKReg, 0x40);
    writeRegister(ModeReg, 0x3D);
    antennaOn();
  }

  void reset() {
    if (!writeRegister(CommandReg, PCD_RESETPHASE)) {
      ser.updateBaudRate(115200);
      writeRegister(CommandReg, PCD_RESETPHASE);
      ser.updateBaudRate(9600);
      delay(50);
    }
    writeRegister(SerialSpeedReg, 0x7A);
    ser.updateBaudRate(115200);
  }

  // --- Register operations ---
  bool writeRegister(uint8_t addr, uint8_t val) {
    for (int i = 0; i < 10; i++) {
      flushInput();
      ser.write(addr & 0x7F);
      ser.write(val);
      delayMicroseconds(500);
      if (ser.available()) {
        uint8_t echo = ser.read();
        if (echo == addr)
          return true;
      }
    }
    Serial.print("Write error at ");
    Serial.println(addr, HEX);
    return false;
  }

  uint8_t readRegister(uint8_t addr) {
    flushInput();
    ser.write(addr | 0x80);
    delayMicroseconds(500);
    if (ser.available()) {
      return ser.read();
    }
    return 0xFF;
  }

  void flushInput() {
    while (ser.available())
      ser.read();
  }

  void setBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp | mask);
  }

  void clearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp & (~mask));
  }

  void antennaOn() {
    uint8_t temp = readRegister(TxControlReg);
    if (!(temp & 0x03)) {
      setBitMask(TxControlReg, 0x03);
    }
  }

  void antennaOff() {
    clearBitMask(TxControlReg, 0x03);
  }

  // --- Core card communication ---
  uint8_t toCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
                 uint8_t *backData, uint8_t *backLen) {
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    if (command == PCD_AUTHENT) {
      irqEn = 0x12;
      waitIRq = 0x10;
    }
    if (command == PCD_TRANSCEIVE) {
      irqEn = 0x77;
      waitIRq = 0x30;
    }

    writeRegister(CommIEnReg, irqEn | 0x80);
    clearBitMask(DivIrqReg, 0x80);
    setBitMask(FIFOLevelReg, 0x80);

    writeRegister(CommandReg, PCD_IDLE);

    for (i = 0; i < sendLen; i++)
      writeRegister(FIFODataReg, sendData[i]);

    writeRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
      setBitMask(BitFramingReg, 0x80);

    i = 1000;
    do {
      n = readRegister(DivIrqReg);
      i--;
    } while (i != 0 && !(n & 0x01) && !(n & waitIRq));

    clearBitMask(BitFramingReg, 0x80);

    if (i != 0) {
      if (!(readRegister(ErrorReg) & 0x1B)) {
        status = MI_OK;
        if (n & irqEn & 0x01)
          status = MI_NOTAGERR;
        if (command == PCD_TRANSCEIVE) {
          n = readRegister(FIFOLevelReg);
          lastBits = readRegister(ControlReg) & 0x07;
          if (lastBits)
            *backLen = (n - 1) * 8 + lastBits;
          else
            *backLen = n * 8;
          if (n == 0)
            n = 1;
          if (n > MAX_LEN)
            n = MAX_LEN;
          for (i = 0; i < n; i++)
            backData[i] = readRegister(FIFODataReg);
        }
      } else
        status = MI_ERR;
    }
    return status;
  }

  // --- High-level functions ---
  uint8_t request(uint8_t reqMode, uint8_t *backBits) {
    uint8_t status;
    uint8_t tagType[1] = {reqMode};
    uint8_t backData[16];
    uint8_t backLen = 0;

    writeRegister(BitFramingReg, 0x07);
    status = toCard(PCD_TRANSCEIVE, tagType, 1, backData, &backLen);
    if (status != MI_OK || backLen != 0x10)
      status = MI_ERR;
    *backBits = backLen;
    return status;
  }

  uint8_t anticoll(uint8_t *serNum) {
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint8_t unLen;
    uint8_t backData[16];
    uint8_t buf[2] = {PICC_ANTICOLL, 0x20};

    writeRegister(BitFramingReg, 0x00);
    status = toCard(PCD_TRANSCEIVE, buf, 2, backData, &unLen);
    if (status == MI_OK) {
      for (i = 0; i < 4; i++) {
        serNum[i] = backData[i];
        serNumCheck ^= backData[i];
      }
      if (serNumCheck != backData[4])
        status = MI_ERR;
    }
    return status;
  }

  // --- Utility: CRC calculation ---
  void calculateCRC(uint8_t *data, uint8_t len, uint8_t *out) {
    clearBitMask(DivIrqReg, 0x04);
    setBitMask(FIFOLevelReg, 0x80);
    for (uint8_t i = 0; i < len; i++)
      writeRegister(FIFODataReg, data[i]);
    writeRegister(CommandReg, PCD_CALCCRC);
    uint16_t i = 0xFF;
    uint8_t n;
    do {
      n = readRegister(DivIrqReg);
      i--;
    } while (i != 0 && !(n & 0x04));
    writeRegister(CommandReg, PCD_IDLE);
    out[0] = readRegister(CRCResultRegL);
    out[1] = readRegister(CRCResultRegM);
  }

  uint8_t getVersion() {
    return readRegister(VersionReg);
  }
};

HardwareSerial mySerial(1);
MFRC522_UART rfid(mySerial);

void setup() {
  Serial.begin(115200);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  delay(2000);
  Serial.println("Starting MFRC522 UART...");
  rfid.begin();
  Serial.print("Version: 0x");
  Serial.println(rfid.getVersion(), HEX);
}

void loop() {
  uint8_t status;
  uint8_t backBits;
  uint8_t serNum[5];

  status = rfid.request(MFRC522_UART::PICC_REQA, &backBits);
  Serial.println(status);
  if (status == MFRC522_UART::MI_OK) {
    Serial.println("Card detected");
    status = rfid.anticoll(serNum);
    if (status == MFRC522_UART::MI_OK) {
      Serial.print("Card UID: ");
      for (uint8_t i = 0; i < 4; i++) {
        Serial.print(serNum[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
  delay(1000);
}