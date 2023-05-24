/*!
  @file ADC101CS021.h
  @brief I2C-interface ADC101CS021 Class
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#ifndef ADC101CS021_H_
#define ADC101CS021_H_

#include <errno.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <linux/i2c-dev.h>
//#include <sys/ioctl.h>

#include <unistd.h>
#include <iostream>
#include <stdint.h>

/*!
  @brief I2C-interface ADC101CS021 Class
  @details Modeled on [TI datasheet](https://www.ti.com/lit/ds/symlink/adc101c027.pdf) (version SNAS446D, Feb. 2008 – Feb. 2013)
  @todo Add ioctl to set the address of the ADC?
*/
class adc101 {
  public:
    adc101(int i2cFile, uint8_t addrIn);  //!< Constructor
    virtual ~adc101();    //!< Destructor

    /*!
      Automatic-Conversion mode sampling frequency
      | b | Automatic Conversion | Sample rate (ksps) |
      |:-:|:--------------------:|:------------------:|
      | 0 | OFF                  | -                  |
      | 1 | ON                   | 27                 |
      | 2 | ON                   | 13.5               |
      | 3 | ON                   | 6.7                |
      | 4 | ON                   | 3.4                |
      | 5 | ON                   | 1.7                |
      | 6 | ON                   | 0.9                |
      | 7 | ON                   | 0.4                |
    */
    enum cycleTimeT : uint8_t { 
      off     = 0,
      ksps27  = 1,
      ksps13  = 2,
      ksps6   = 3,
      ksps3   = 4,
      ksps2   = 5,
      ksps1   = 6,
      kspsP4  = 7
    };

    /*!
      Address of registers
      | b | Register | Permissions |
      |:-:|:--------:|:-----------:|
      | 0 | Conversion Result | R |
      | 1 | Alert Status | R/W |
      | 2 | Configuration | R/W |
      | 3 | Low Limit | R/W |
      | 4 | High Limit | R/W |
      | 5 | Hysteresis | R/W |
      | 6 | Lowest Conversion | R/W |
      | 7 | Highest Conversion | R/W |
    */
    enum regListT : uint8_t {
      convResultReg  = 0,
      alrtStsReg     = 1,
      cfgReg         = 2,
      lowLimReg      = 3,
      highLimReg     = 4,
      hystReg        = 5,
      lowestConvReg  = 6,
      highestConvReg = 7 
    };

    /*!
      Get conversion result and alert flag
      @param[out] value Reference to the conversion result buffer
      @param[out] alert Reference to the alert-flag buffer
    */
    void updateConv(uint16_t &value, bool &alert);

    /*!
      Start conversion and get the result (non-automatic conversion).
      @todo singleNormalConversion() uses setPointer(); add a function with only read?
      @param[out] value Reference to the conversion result buffer
      @param[out] alert Reference to the alert-flag buffer
      @return false for error
    */
    bool getConv(uint16_t &value, bool &alert);

    /*!
      Start automatic conversion
      @param[in] timer Timer for the automatic conversion; use the cycleTimeT enum
    */
    void startAutoConv(adc101::cycleTimeT timer);

    /*!
      Stop automatic conversion
    */
    void stopAutoConv();

    /*!
      Set I2C address;
      @param[in] address
    */
    void setAddress(uint8_t address);

    /*!
      Get I2C address
      @return I2C address
    */
    uint8_t getAddress();


  protected:
    int i2cFile; //!< i2cFile
    uint8_t addr; //!< I2C address 
    
    //Status
    uint16_t conversion; //!< Conversion register, 11:2; ADC conversion
    bool alertFlag; //!< Conversion register, 16; Alert Flag
    bool overRangeAlert; //!< Alert status register, over Range Alert, active high
    bool underRangeAlert;//!< Alert status register, under Range Alert, active high

    //ADC configurations (see file's end or datasheet for meaning)
    cycleTimeT cycleTime; //!< Configuration register, 7:5; Automatic conversion period
    bool alertHold; //!< Configuration register, Alert Hold (0: self clear, 1: hold)
    bool alertFlagEn; //!< Configuration register, Alert flag enable (0: disabled, 1: en)
    bool alertPinEn; //!< Configuration register, Alert pin enable (0: disabled, 1: en)
    bool alertPolarity; //!< Configuration register, Alert polarity (0: active low, 1: active high)
    uint16_t lowerLimit; //!< Low-Limit register, 11:2; for alert condition
    uint16_t higherLimit; //!< High-Limit register, 11:2; for alert condition
    uint16_t hysteresis; //!< Hysteresis register, 11:2; for lowerLimit and higherLimit
    uint16_t lowestConv; //!< Lowest Conversion register, 11:2; 0x0FFF to clear
    uint16_t highestConv; //!< Highest Conversion register, 11:2; 0x0000 to clear

    /*!
      Read 1 byte from the ADC
      @param[out] value Reference to the conversion result buffer
      @return false for error
    */
    bool readByte(uint8_t* value);

    /*!
      Read 2 bytes from the ADC
      @param[out] value Reference to the conversion result buffer
      @return false for error
    */
    bool readWord(uint16_t &value);

    /*!
      Read 2 bytes from the ADC and extrapolate value and alert flag
      @return false for error
    */
    bool readConversion();

    /*!
      Set pointer to read address.
      
      The register-address pointer is set at the beginning of a writing operation!
      To read a register, write to that register and then read it.
      @param[in] address Address of the register to read; use the regListT enum
      @return false for error
    */
    bool setPointer(uint8_t address);

    /*!
      Write 1 byte to the ADC
      @param[in] address Register to write; use the regListT enum
      @param[in] value Value to write on the register
      @return false for error
    */
    bool writeByte(uint8_t address, uint8_t value);

    /*!
      Write 2 bytes to the ADC
      @param[in] address Register to write; use the regListT enum
      @param[in] value Value to write on the register
      @return false for error
    */
    bool writeWord(uint8_t address, uint16_t value);


    /*!
      Single conversion with ADC in Normal Conversion mode (non-Automatic).
      In this mode, a new conversion starts after the previous result is read.
      @return false for error
    */
    bool singleNormalConversion();
    
    /*!
      Repeated conversion with ADC in Normal Conversion mode (non-Automatic)
      @todo Implement it
      @return false for error
    */
    bool sequenceNormalConversion();

    /*!
      Configure the ADC registers
      @return false for error
    */
    void configure();

};

#endif /*ADC101CS021_H_*/

/* Registers content

Conversion Register (16 bit):
 15: Alert flag
 11-2: Conversion result 11:0
 others: reserved ('b0)

Alert status register (8 bit) (active high):
 1: Over-range alert flag
 0: Under-range alert flag

Configuration register (8 bit):
 7:5: cycle time
  0: Automatic conversion OFF
  1: Automatic conversion ON: 27 ksps
  2: Automatic conversion ON: 13.5 ksps
  3: Automatic conversion ON: 6.7 ksps
  4: Automatic conversion ON: 3.4 ksps
  5: Automatic conversion ON: 1.7 ksps
  6: Automatic conversion ON: 0.9 ksps
  7: Automatic conversion ON: 0.4 ksps
 4: Alert Hold (0: self clear, 1: hold)
 3: Alert flag enable (0: disabled, 1: enabled)
 2: Alert pin enable (0: disabled, 1: enabled)
 1: Reserved
 0: Alert polarity (0: active low, 1: active high)

Low-Limit register (16 bit):
 11:2: Lower limit for alert condition

High-Limit register (16 bit):
 11:2: Higher limit for alert condition

Hysteresis register (16 bit):
 11:2: hysteresis for low- and high-limits crossings

Lowest Conversion register (16 bit):
 11:2: Lowest conversion encountered after a reset. Write 0x0FFF to clear

Highest Conversion register (16 bit):
 11:2: Highest conversion encountered after a reset. Write 0x0000 to clear

*/