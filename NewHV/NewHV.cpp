/*!
  @file NewHV.cpp
  @brief I2C-interface to the NewHV board
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#include "NewHV.h"


NewHVIntf::NewHVIntf(int i2cFile, uint32_t autoReadIn, uint8_t dacAddr, uint8_t adcAddr) {
  i2cFile = i2cFile;
  autoRead = autoReadIn;
  voltageV = 0.0;
  voltageDac = 0;
  currentA = 0.0;
  currentAdc = 0;

  //Instantiate DAC and ADC
  dac = new ltc1669(i2cFile, dacAddr);
  adc = new adc101(i2cFile, adcAddr);
}


NewHVIntf::~NewHVIntf() {
  i2cFile = 0;
  voltage = 0;
  
  //Turn off output voltage
  setBias(0.0);
  applyBias();

  //Delete DAC and ADC istances
  if(dac!=nullptr)
    delete dac;
  if(adc!=nullptr)
    delete adc;
}


uint16_t NewHVIntf::voltageV2D(float vIn) {
  float temp = vIn*voltConvRatio;
  //Cast and limit output to 10-bit value
  if (temp < 1024.0)
    return static_cast<uint16_t>(temp) & 0x03FF;
  else
    return static_cast<uint16_t>(0x03FF);
}


float NewHVIntf::voltageD2V() {
  return voltageDac / voltConvRatio;
}


float NewHVIntf::currentAdc2I(uint16_t adcVal) {
  return adcVal * currConvRatio;
}


void NewHVIntf::setBias(float vSet) {
  voltageV = vSet;
  voltageDac = voltageV2D(voltageV);
}


bool NewHVIntf::applyBias() {
  if(dac.writeWord(0x04, voltageDac)) {
    printf("Failed to apply bias to DAC %02x", dac.getAddress());
    return false;
  }
  return true;
}


bool NewHVIntf::readAdcSingle(float &value, bool &alert) {
  bool bSuccess = false;

  //Read from ADC and convert in uA
  bSuccess = adc.getConv(currentAdc, alertFlag);
  currentA = currentAdc2I(currentAdc);

  //Output
  value = currentA;
  alert = alertFlag;
  return bSuccess;
}


bool NewHVIntf::readAdc(float &value, bool &alert) {
  bool bSuccess = false;

  //Read from ADC and convert in uA
  bSuccess = adc.read(currentAdc, alertFlag);
  currentA = currentAdc2I(currentAdc);

  //Output
  value = currentA;
  alert = alertFlag;
  return bSuccess;
}


void NewHVIntf::readAdcLoop() {
  //Map timer to adc101::cycleTimeT enum
  //autoRead; //Input timer
  //adc::cycleTimeT timer; //output timer

  
  //Start ADC auto-conversion
  adc.startAutoConv(adc::cycleTimeT::kspsP4)

  //!@todo Automatically read ADC

  return;
}
