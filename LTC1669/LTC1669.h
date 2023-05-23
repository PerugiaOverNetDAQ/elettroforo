/*!
  @file LTC1669.h
  @brief I2C-interface LTC1669 Class
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#ifndef LTC1669_H_
#define LTC1669_H_

//headers
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <linux/i2c-dev.h>
//#include <sys/ioctl.h>

/*!
  @brief I2C-interface LTC1669 Class
  @details  Modeled on [analog.com datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/1669fa.pdf) (v.1669fa).
            Implemented only a subset of functions.
            No _SYNC Address_ / _Quick Command_ implemented.
  @todo Add ioctl to set the address of the DAC?
*/
class ltc1669 {
  public:
    ltc1669(int i2cFile); //!< Constructor
    virtual ~ltc1669();   //!< Destructor

    /*!
      Write a 2 bytes to the DAC (only the 10-LSb are valid)
      @param[in] command Command byte, as per datasheet
      @param[in] value Voltage value (2 bytes, unsigned)
      @return False for error
    */
    bool writeWord(uint8_t command, uint16_t value);
    
    /*!
      Write a command (1 byte) to the DAC
      @param[in] command Command byte, as per datasheet
      @return False for error
    */
    bool writeCommand(uint8_t command);

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
    const uint8_t syncAddr = 0xFC; //!< I2C address to sync all connected DACs

}

#endif /*LTC1669_H_*/

/*
Command byte: 7-3: X - 2: BG - 1: SD - 0: SY
  BG: 1: Internal band-gap reference (range: 0-2.5V) (independent from Vcc)
      0: Vcc reference(range: 0-Vcc)
  SD: 1: power-down mode
      0: operating mode
  SY: 1: Update on sync
      0: Update on stop
*/