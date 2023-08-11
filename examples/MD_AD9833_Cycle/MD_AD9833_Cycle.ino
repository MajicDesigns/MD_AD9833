// Basic MD_AD9833 test file
//
// Initialises the device to default conditions
// 
// Connect a pot to A0 to change the frequency by turning the pot
//
#include <MD_AD9833.h>
#include <SPI.h>

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
// MD_AD9833	AD(PIN_DATA, PIN_CLK, PIN_FSYNC); // Arbitrary SPI pins

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

void setup(void)
{
  AD.begin();

/*
  AD.setMode(MD_AD9833::MODE_TRIANGLE);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low
  
  AD.setMode(MD_AD9833::MODE_SINE);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low

  AD.setMode(MD_AD9833::MODE_SQUARE1);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now high

  AD.setMode(MD_AD9833::MODE_SQUARE2);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low
*/
}

void loop(void)
{
  static bool bOff = false;
  static uint8_t m = 0;
  static MD_AD9833::mode_t modes[] =
  {
    MD_AD9833::MODE_TRIANGLE,
    MD_AD9833::MODE_SQUARE2,
    MD_AD9833::MODE_SINE,
    MD_AD9833::MODE_SQUARE1
  };

  if (bOff)
    AD.setMode(MD_AD9833::MODE_OFF);
  else
  {
    AD.setMode(modes[m]);
    m++;
    if (m == ARRAY_SIZE(modes)) m = 0;
  }
  bOff = !bOff;
  delay(2000);
}
