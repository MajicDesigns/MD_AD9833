#include <SPI.h>
#include <MD_cmdProcessor.h>
#include <MD_AD9833.h>

//
// Test sketch for the functions of the MD_AD9833 library.
//
// Commands are issued via the serial monitor to effect changes in the AD9833
// hardware using the library functions.
//
// Dependencies:
// MD_cmdProcessor is available at https://github.com/MajicDesigns/MD_cmdProcessor or Arduino Library Manager
//

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;	  ///< SPI Clock pin number
const uint8_t PIN_FSYNC = 10;	///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD(PIN_FSYNC); // Hardware SPI
//MD_AD9833	AD(PIN_DATA, PIN_CLK, PIN_FSYNC); // Arbitrary SPI pins

// handler functions
void handlerHelp(char* param);

void handlerF1(char* param) 
{
  float  f = strtod(param, nullptr);

  Serial.print(F("\nFreq 1: ")); Serial.print(f);
  AD.setFrequency(MD_AD9833::CHAN_0, f);
}

void handlerF2(char* param) 
{
  float  f = strtod(param, nullptr);

  Serial.print(F("\nFreq 2: ")); Serial.print(f);
  AD.setFrequency(MD_AD9833::CHAN_1, f);
}

void handlerP1(char* param) 
{
  uint32_t  ul = strtoul(param, nullptr, 10);

  Serial.print(F("\nPhase 1: ")); Serial.print(ul / 10); Serial.print(F(".")); Serial.print(ul % 10);
  AD.setPhase(MD_AD9833::CHAN_0, (uint16_t)ul);
}

void handlerP2(char* param) 
{
  uint32_t  ul = strtoul(param, nullptr, 10);

  Serial.print(F("\nPhase 2: ")); Serial.print(ul / 10); Serial.print(F(".")); Serial.print(ul % 10);
  AD.setPhase(MD_AD9833::CHAN_1, (uint16_t)ul);
}

void handlerOF(char* param) 
{
  MD_AD9833::channel_t chan = AD.getActiveFrequency();

  Serial.print(F("\nFreq source: ")); Serial.print(*param);
  switch (*param)
  {
  case '1': chan = MD_AD9833::CHAN_0; break;
  case '2': chan = MD_AD9833::CHAN_1; break;
  default: Serial.print(F(" error"));
  }
  AD.setActiveFrequency(chan);
}

void handlerOP(char* param) 
{
  MD_AD9833::channel_t chan = AD.getActivePhase();

  Serial.print(F("\nPhase source: ")); Serial.print(*param);
  switch (*param)
  {
  case '1': chan = MD_AD9833::CHAN_0; break;
  case '2': chan = MD_AD9833::CHAN_1; break;
  default: Serial.print(F(" error"));
  }
  AD.setActivePhase(chan);
}

void handlerOW(char* param) 
{
  MD_AD9833::mode_t mode = AD.getMode();

  Serial.print(F("\nOutput wave: ")); Serial.print(*param);
  switch (toupper(*param))
  {
  case 'O': mode = MD_AD9833::MODE_OFF;    break;
  case 'S': mode = MD_AD9833::MODE_SINE;   break;
  case 'T': mode = MD_AD9833::MODE_TRIANGLE;  break;
  case 'Q': mode = MD_AD9833::MODE_SQUARE1;  break;
  default: Serial.print(F(" error"));
  }
  AD.setMode(mode);
}

void handlerR(char* param) 
{ 
  Serial.print(F("\nReset"));
  if (*param == '1') Serial.print(F(" hold"));
  AD.reset(*param == '1');
}

const MD_cmdProcessor::cmdItem_t PROGMEM cmdTable[] =
{
  { "?",  handlerHelp,  "",    "Help", 0 },
  { "h",  handlerHelp,  "",    "Help", 0 },
  { "f1", handlerF1,    "f", "Frequency 1 to f Hz", 1 },
  { "f2", handlerF2,    "f", "Frequency 2 to f Hz", 1 },
  { "p1", handlerP1,    "p", "Phase 1 set p tenths degree (1201=120.1)", 2 },
  { "p2", handlerP2,    "p", "Phase 2 set p tenths degree (1201=120.1)", 2 },
  { "of", handlerOF,    "c", "Output Frequency source channel [c=1/2]", 3 },
  { "op", handlerOP,    "c", "Output Phase source channel [c=1/2]", 3 },
  { "ow", handlerOW,    "t", "Output Wave type [t=(o)ff/(s)ine/(t)ri/s(q)re]", 3 },
  { "or", handlerR,     "h", "Reset AD9833 registers (hold if h=1)", 3 }
};

MD_cmdProcessor CP(Serial, cmdTable, ARRAY_SIZE(cmdTable));

void handlerHelp(char* param)
{
  Serial.print(F("\n\n[MD_AD9833 Tester]"));
  Serial.print(F("\nEnsure line ending set to newline.\n"));
  CP.help();
  Serial.print(F("\n"));
}

void setup()
{
  Serial.begin(57600);
  AD.begin();
  CP.begin();
  CP.help();
}

void loop()
{
  CP.run();
}