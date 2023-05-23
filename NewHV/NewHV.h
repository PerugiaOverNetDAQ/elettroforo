/*!
  @file NewHV.h
  @brief I2C-interface to the NewHV board
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#ifndef NHV_H_
#define NHV_H_

#include "LTC1669.h"
#include "ADC101CS021.h"

/*!
  @brief I2C-interface to the NewHV board
*/
class NewHVIntf {
  public:
    NewHVIntf(int i2cFile, uint32_t autoReadIn, uint8_t dacAddr, uint8_t adcAddr); //!< Constructor
    virtual ~NewHVIntf(); //!< Destructor
    
    /*!
      Set Vbias in volts and DAC units
      @param[in] vSet Update the NewHVIntf::voltageV with this value
    */
    void setBias(float vSet);

    /*!
      Apply Vbias in output
      @return False for error
    */
    bool applyBias();

    /*!
      Calls adc101::getConv() to set the pointer to the conversion register and
      reads the conversion result. Slower than NewHVIntf::readAdc() because of
      the adc101::setPointer() call.
      @param[out] value Reference to the conversion result buffer
      @param[out] alert Reference to the alert-flag buffer
      @return false for error
    */
    bool readAdcSingle(float &value, bool &alert);


    /*!
      Read the conversion register of the ADC. Does NOT call
      adc101::setPointer()
      @param[out] value Reference to the conversion result buffer
      @param[out] alert Reference to the alert-flag buffer
      @return false for error
    */
    bool readAdc(float &value, bool &alert);


    /*!
      Enables the adc101 auto-conversion mode and reads the ADC in loop.
    */
    void readAdcLoop();

  
  protected:
    int i2cFile; //!< I2C device
    uint32_t autoRead;  //!< Auto-read interval, in us; 0: off
    //DAC
    float voltageV; //!< Set voltage, in volts
    uint16_t voltageDac;  //!< Set voltage, in DAC units
    const float     biasMin = 0.0; //!< Minimum bias voltage the LT3482 can supply
    const float     biasMAX = 80.0; //!< Maximum bias voltage the LT3482 can supply
    const uint16_t  dacMin = 0; //!< DAC lowest code for meaningful output
    const uint16_t  dacMAX = 512; //!< DAC highest code for meaningful output
    const float     voltConvRatio = dacMax/biasMax; //!< Conversion factor from DAC codes to LT3482 out voltage
    
    //ADC

    uint16_t currentAdc; //<! Current monitor in ADC units
    float currentA; //!< Current monitor (in uA)
    bool alertFlag; //!< Alert flag (MSb of conversion register)
    const uint16_t Rgain = 40000; //!< Resistor to transform current in voltage
    const uint8_t  Vdd = 5; //!< Supply of the ADC
    const uint16_t resolution = 1024; //!< ADC resolution (\f$ 2^{bit} \f$)
    const uint8_t  ImonFactor = 5; //!< Factor between output and monitored current 
    const float    currConvRatio = (ImonFactor/Rgain)*(Vdd/resolution)*1000000; //!< Conversion factor from ADC codes to current in uA


    ltc1669 dac; //!< DAC interface instance
    adc101 adc;  //!< ADC interface instance

    /*!
      Translate the voltage from volts to DAC units.
      
      Generic version: line from two points equation
        \f$ y = {x - V_{bias,min} \over V_{bias,MAX}- V_{bias,min}} \cdot (V_{dac,MAX} - dacMin) + dacMin \f$
        \f$ y = 6.4 \cdot x \f$, or  \f$ 512x - 80y = 0 \f$
      For DAC output voltage: \f$ y = x/64 \f$, or  \f$ 1.25x - 80y = 0 \f$
      
      If dacMin and biasMin are both zero, it simplifies to the used re-scaling.
      @param[in] vIn Voltage to set in V
      @return Voltage to set in DAC codes (uint16_t)
    */
    uint16_t voltageV2D(float vIn);

    /*!
      Translate the voltage from DAC units to volts
        \f$ y = {x \over 6.4} \f$, or  \f$ 80x - 512y = 0 \f$
      for DAC output voltage: \f$ y = {x \over 64}\f$, or  \f$ 1.25x - 512y = 0 \f$
      Vbias resolution: 156 mV (for 1 step in DAC units)
      @return Voltage in V (float)
    */
    float voltageD2V();

    /*!
      Convert ADC read value to monitor current (in uA)
      \f$ y = {5 \over R_4} \cdot {V_{DD} \over 2^{10}} \cdot x \f$
      , where \f$ {LSb} = {V_{DD} \over 2^{10}} \f$
      @param[in] adcVal ADC conversion in ADC units
      @return Current monitor in uA (float)
    */
    float currentAdc2I(uint16_t adcVal);

}



#endif /*NHV_H_*/