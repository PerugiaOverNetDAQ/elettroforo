/*!
  @file elettroforo.cpp
  @brief Main module for the NewHV interface
  @author Mattia Barbanera (mattia.barbanera@infn.it)
*/

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "hwlib.h"

#include "../NewHV/NewHV.h"

NewHVIntf* nhv = nullptr; //!< Pointer to the NewHVIntf instance

/*!
  Cleanly close the interface to the board.
  @param signum
*/
void closeIntf(int signum){
  printf("\nKilling NewHV interface...");
  
  if(nhv!=nullptr){
    delete nhv;
  }

  printf(" done\n");
  exit(signum);
}


int main(int argc, char *argv[]) {
  std::cout<<"hash="<<GIT_HASH<<", time="<<COMPILE_TIME<<", branch="<<GIT_BRANCH<<std::endl;
  
  //Args
  if (argc < 5) {
    printf("Usage:\n\tEFORO(arm) <Voltage> <Auto-read intervals> <DAC address> <ADC address>\n\n");
    printf("\tVoltage:\t\tFloat\tVoltage output in volts\n");
    printf("\tAuto-read intervals:\tuint32_t\tIntervals in us; 0: off\n");
    printf("\tDAC address:\t\tuint8\tI2c address of DAC\n");
    printf("\tADC address:\t\tuint8\tI2c address of ADC\n");
    return 0;
  }
  float voltageIn   = std::stof(argv[1]);
  int autoReadIn  = uint32_t(atoi(argv[2]));
  int dacAddr     = uint8_t(atoi(argv[3]));
  int adcAddr     = uint8_t(atoi(argv[4]));

  //Open I2C bus
  int i2cHandle = 0;
  const char *i2cDevice = "/dev/i2c-1";
	if ((i2cHandle = open(i2cDevice, O_RDWR)) < 0) {
  	  // ERROR HANDLING: you can check errno to see what went wrong
	    perror("Failed to open the i2c bus");
  	  exit(1);
	}
	
	//FIXME: i2c address: how do I switch between DAC and ADC?
	int addr = dacAddr; //Temporary, just trying out the DAC
	if (ioctl(i2cHandle, I2C_SLAVE, addr) < 0) {
  	  printf("Failed to acquire bus access and/or talk to slave.\n");
  	  exit(1);
	}

  signal(SIGINT, closeIntf);

  printf("Starting NewHV interface...\n");
  nhv = new NewHVIntf(i2cHandle, autoReadIn, dacAddr, adcAddr);
  
  //Apply DAC bias
  nhv->setBias(voltageIn);
  nhv->applyBias();

  //FIXME: Always running ADC?

  //Cleanly delete interface
  closeIntf(0);

  return 0;
}