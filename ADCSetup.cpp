
#include <wiringPi.h>

#include "SPISetup.h"

#include "ADCSetup.h"

/*
 * adcRead:
 *	Return the analog value of the given pin
 *********************************************************************************
 */

static int adcRead (struct wiringPiNodeStruct *node, int pin)
{
  unsigned char spiData [3] ;
  unsigned char chanBits ;
  int chan = pin - node->pinBase ;

  chanBits = 0b10000000 | (chan << 4) ;

  spiData [0] = 1 ;		// Start bit
  spiData [1] = chanBits ;
  spiData [2] = 0 ;

  SPIDataRW (node->fd, spiData, 3) ;

  return ((spiData [1] << 8) | spiData [2]) & 0x3FF ;
}


/*
 * mSetup:
 *	Create a new wiringPi device node for an mcp3004 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int adcSetup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;

  if (SPISetup (spiChannel, 1000000) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, 8) ;

  node->fd         = spiChannel ;
  node->analogRead = adcRead ;

  return TRUE ;
}
