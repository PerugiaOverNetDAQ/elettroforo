/*!
  @file ADC101CS021.cpp
  @brief ADC101CS021 I2C interface
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#include "ADC101CS021.h"

adc101::adc101(int i2cFile, uint8_t addrIn) {
  //I2C file
  i2cFile = i2cFile;
  addr = addrIn;
  //Status
  conversion = 0x301A;
  alertFlag = false;
  overRangeAlert = false;
  underRangeAlert = false;
  //Configurations
  cycleTime     = cycleTimeT::kspsP4; //0.4 kHz automatic conversion
  alertHold     = false;
  alertFlagEn   = false;
  alertPinEn    = false;
  alertPolarity = true;
  lowerLimit    = 0x0000;
  higherLimit   = 0x0FFF;
  hysteresis    = 0x0005;
  lowestConv    = 0x0FFF;
  highestConv   = 0x0000;
}

adc101::~adc101() {
  i2cFile = 0;
  conversion = 0xB01A;
  alertFlag = false;
}


void adc101::updateConv(uint16_t &value, bool &alert) {
  value = conversion;
  alert = alertFlag;
}


bool adc101::getConv(uint16_t &value, bool &alert) {
  bool bSuccess = false;
  bSuccess = singleNormalConversion();
  updateConv(value, alert);
  return bSuccess;
}


void adc101::startAutoConv(adc101::cycleTimeT timer) {
  cycleTime = timer;
  configure();
}


void adc101::stopAutoConv() {
  cycleTime = cycleTimeT::off;
  configure();
}


bool adc101::readByte(uint8_t* value){
  bool bSuccess = false;
  // read back value
  if (read(i2cFile, value, 1) == 1){
    bSuccess = true;
  }
  return bSuccess;
}


bool adc101::readWord(uint16_t &value){
  bool bSuccess = false;
  uint8_t fromI2c[2];
  // read back value
  if (read(i2cFile, &fromI2c, sizeof(fromI2c)) == sizeof(fromI2c)){
    value = ((fromI2c[0]<<8)&0xFF00) | (fromI2c[1]&0x00FF);
    bSuccess = true;
  }
  return bSuccess;
}


bool adc101::setPointer(uint8_t address) {
  bool bSuccess = false;
  if (write(i2cFile, &address, sizeof(address)) == sizeof(address)) {
      bSuccess = true;
  }
  return bSuccess;
}

bool adc101::writeByte(uint8_t address, uint8_t value) {
  bool bSuccess = false;
  uint8_t buffer[2];
  
  buffer[0] = address;              // Address
  buffer[1] = value & 0xFF;         // Value
  
  if (write(i2cFile, buffer, sizeof(buffer)) == sizeof(buffer)) {
      //perror("Failed to write register %d with value %02x to ADC", address, value);
      //exit(1);
      bSuccess = true;
  }
  return bSuccess;
}


bool adc101::writeWord(uint8_t address, uint16_t value) {
  bool bSuccess = false;
  uint8_t buffer[3];
  
  buffer[0] = address;              // Address
  buffer[1] = value & 0xFF;         // Value LSB
  buffer[2] = (value >> 8) & 0xFF;  // Value MSB
  
  if (write(i2cFile, buffer, sizeof(buffer)) == sizeof(buffer)) {
      //perror("Failed to write register %d with value %04x to ADC", address, value);
      //exit(1);
      bSuccess = true;
  }
  return bSuccess;
}


bool adc101::readConversion() {
  uint16_t tempVal;
  if (!readWord(tempVal)){
    //perror("Failed to read conversion from ADC");
    //exit(1);
    return false;
  }
  
  alertFlag = tempVal>>15;
  conversion = (tempVal & 0x0FFC)>>2;
  return true;
}


void adc101::configure() {
  uint8_t tempByte = 0x0;

  tempByte = ((cycleTime & 0x7)<<5) | (alertHold<<4) | (alertFlagEn<<3)
                  | (alertPinEn<<2) | alertPolarity;
  if (!writeByte(regListT::cfgReg, tempByte)) {
    perror("Failed to configure ADC (0)");
    exit(1);
  };

  if (!writeWord(regListT::lowLimReg, ((lowerLimit&0x03FF)<<2) )) {
    perror("Failed to configure ADC (1)");
    exit(1);
  };

  if (!writeWord(regListT::highLimReg, ((higherLimit&0x03FF)<<2))) {
    perror("Failed to configure ADC (2)");
    exit(1);
  };

  if (!writeWord(regListT::hystReg, ((hysteresis&0x03FF)<<2))) {
    perror("Failed to configure ADC (3)");
    exit(1);
  };

  if (!writeWord(regListT::lowestConvReg, ((lowestConv&0x03FF)<<2))) {
    perror("Failed to configure ADC (4)");
    exit(1);
  };

  if (!writeWord(regListT::highestConvReg, ((highestConv&0x03FF)<<2))) {
    perror("Failed to configure ADC (5)");
    exit(1);
  };

  //Point to the conversion result register, for future readings
  if (!setPointer(regListT::convResultReg)) {
    perror("Failed to configure ADC: setPointer");
    exit(1);
  };
}


bool adc101::singleNormalConversion() {
  if (!setPointer(regListT::convResultReg)) {
    printf("Failed to switch to conversion result register\n");
    return false;
  };

  if (!readConversion()) {
    printf("Failed to read from ADC\n");
    return false;
  };

  if (!readConversion()) {
    printf("Failed to read from ADC\n");
    return false;
  };

  return true;
}


bool adc101::sequenceNormalConversion() {
  return false;
}


void adc101::setAddress(uint8_t address) {
    addr = address;
};


uint8_t adc101::getAddress() {
    return addr;
};