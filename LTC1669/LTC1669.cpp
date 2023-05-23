/*!
  @file LTC1669.cpp
  @brief LTC1669 I2C interface
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#include "LTC1669.h"

ltc1669::ltc1669(int i2cFile, uint8_t addrIn) {
  i2cFile = i2cFile;
  addr = addrIn;
}

ltc1669::~ltc1669() {
  i2cFile = 0;
}

bool ltc1669::writeWord(uint8_t command, uint16_t value) {
    uint8_t buffer[3];
    
    buffer[0] = command               // Command
    buffer[1] = value & 0xFF;         // LSB
    buffer[2] = (value >> 8) & 0xFF;  // MSB
    
    if (write(i2cFile, buffer, sizeof(buffer)) != sizeof(buffer)) {
        printf("Failed to write command 0x%02x and value %u to I2C device\n", command, value);
        //perror("Failed to write command 0x%02x and value %u to I2C device", command, value);
        //exit(1);
        return false;
    }
    return true;
}


bool ltc1669::writeCommand(uint8_t command) {
    if (write(i2cFile, &command, sizeof(command)) != sizeof(command)) {
        printf("Failed to write command 0x%02x to I2C device\n", command);
        //perror("Failed to write command 0x%02x to I2C device", command);
        //exit(1);
        return false;
    }
    return true;
}


void ltc1669::setAddress(uint8_t address) {
    addr = address;
};


uint8_t ltc1669::getAddress() {
    return addr;
};
