/*
MD_AD9833 - Library for controlling an AD9833 Programmable Waveform Generator.

See the main header file for full information
*/
#include <SPI.h>
#include "MD_AD9833.h"
#include "MD_AD9833_lib.h"

/**
* \file
* \brief Main class definitions for AD_9833 library
*/

// SPI interface functions
#if AD_DEBUG
void MD_AD9833::dumpCmd(uint16_t reg)
{
  if (!bitRead(reg, AD_FREQ1) && !bitRead(reg, AD_FREQ0))  // control register
  {
    // print the bits that are on
    PRINTS(" CTL ");
    if bitRead(reg, AD_B28) PRINTS(" B28") else PRINTS(" ---");
    if bitRead(reg, AD_HLB) PRINTS(" HLB") else PRINTS(" ---");
    if bitRead(reg, AD_FSELECT) PRINTS(" FSL") else PRINTS(" ---");
    if bitRead(reg, AD_PSELECT) PRINTS(" PSL") else PRINTS(" ---");
    if bitRead(reg, AD_RESET) PRINTS(" RST") else PRINTS(" ---");
    if bitRead(reg, AD_SLEEP1) PRINTS(" SL1") else PRINTS(" ---");
    if bitRead(reg, AD_SLEEP12) PRINTS(" SL2") else PRINTS(" ---");
    if bitRead(reg, AD_OPBITEN) PRINTS(" OPB") else PRINTS(" ---");
    if bitRead(reg, AD_DIV2) PRINTS(" DIV") else PRINTS(" ---");
    if bitRead(reg, AD_MODE) PRINTS(" MOD") else PRINTS(" ---");
  }
  else if (bitRead(reg, AD_FREQ1) && bitRead(reg, AD_FREQ0))  // Phase register
  {
    PRINT(" PH", bitRead(reg, AD_PHASE));
    PRINTX("", reg & 0x7ff);
  }
  else if (bitRead(reg, AD_FREQ1) || bitRead(reg, AD_FREQ0))  // Freq register
  {
    PRINT(" FQ", bitRead(reg, AD_FREQ1) ? 1 : 0);
    PRINTX("", reg & 0x3fff);
  }

  return;
}
#endif

void MD_AD9833::spiSend(uint16_t data)
// Either use SPI.h or a dedicated shifting function.
// Sometimes the hardware shift does not appear to work reliably with the hardware - 
// similar problems also reported on the internet.
// The dedicated routine below is modelled on the flow and timing on the datasheet
// and seems to works reliably, but is much slower than the hardware interface.
{
#if AD_DEBUG
  PRINTX("\nspiSend", data);
  dumpCmd(data);
#endif // AD_DEBUG

  if (_hardwareSPI)
  {
    SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE2));
    digitalWrite(_fsyncPin, LOW);
    SPI.transfer16(data);
    digitalWrite(_fsyncPin, HIGH);
    SPI.endTransaction();
  }
  else
  {
    digitalWrite(_fsyncPin, LOW);
    for (uint8_t i = 0; i < 16; i++)
    {
      digitalWrite(_dataPin, (data & 0x8000) ? HIGH : LOW);
      digitalWrite(_clkPin, LOW); //data is valid on falling edge
      digitalWrite(_clkPin, HIGH);
      data <<= 1; // one less bit to do
    }
    digitalWrite(_dataPin, LOW); //idle low
    digitalWrite(_fsyncPin, HIGH);
  }
}

// Class functions
MD_AD9833::MD_AD9833(uint8_t fsyncPin) :
_dataPin(0), _clkPin(0), _fsyncPin(fsyncPin), _hardwareSPI(true)
{
}

MD_AD9833::MD_AD9833(uint8_t dataPin, uint8_t clkPin, uint8_t fsyncPin) :
_dataPin(dataPin), _clkPin(clkPin), _fsyncPin(fsyncPin), _hardwareSPI(false)
{
}

MD_AD9833::~MD_AD9833(void)
{
  if (_hardwareSPI) 
    SPI.end(); 
};

void MD_AD9833::reset(bool hold)
// Reset is done on a 1 to 0 transition
{
  bitSet(_regCtl, AD_RESET);
  spiSend(_regCtl);
  if (!hold)
  {
    bitClear(_regCtl, AD_RESET);
    spiSend(_regCtl);
  }
}

void MD_AD9833::begin(void)
// Initialize the AD9833 and then set up safe values for the AD9833 device
// Procedure from Figure 27 of in the AD9833 Data Sheet
{
  // initialize the SPI interface
  if (_hardwareSPI)
  {
    PRINTS("\nHardware SPI");
    SPI.begin();
  }
  else
  {
    PRINTS("\nSoftware SPI");
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_clkPin, LOW);
    pinMode(_clkPin, OUTPUT);
  }

  // initialize our preferred CS pin (could be same as SS)
  pinMode(_fsyncPin, OUTPUT);
  digitalWrite(_fsyncPin, HIGH);

  _regCtl = 0;

  bitSet(_regCtl, AD_B28);  // always write 2 words consecutively for frequency
  spiSend(_regCtl);

  reset(true);              // Reset and hold
  setClk(AD_MCLK);
  setFrequency(CHAN_0, AD_DEFAULT_FREQ);
  setFrequency(CHAN_1, AD_DEFAULT_FREQ);
  setPhase(CHAN_0, AD_DEFAULT_PHASE);
  setPhase(CHAN_1, AD_DEFAULT_PHASE);
  reset();                  // full transition

  setMode(MODE_SINE);
  setActiveFrequency(CHAN_0);
  setActivePhase(CHAN_0);
}

boolean MD_AD9833::setActiveFrequency(channel_t chan)
{
  PRINT("\nsetActiveFreq CHAN_", chan);

  switch (chan)
  {
  case CHAN_0: bitClear(_regCtl, AD_FSELECT); break;
  case CHAN_1: bitSet(_regCtl, AD_FSELECT);   break;
  }

  spiSend(_regCtl);

  return(true);
}

MD_AD9833::channel_t MD_AD9833::getActiveFrequency(void)
{ 
  return bitRead(_regCtl, AD_FSELECT) ? CHAN_1 : CHAN_0; 
};

boolean MD_AD9833::setActivePhase(channel_t chan)
{
  PRINT("\nsetActivePhase CHAN_", chan);

  switch (chan)
  {
  case CHAN_0: bitClear(_regCtl, AD_PSELECT); break;
  case CHAN_1: bitSet(_regCtl, AD_PSELECT);   break;
  }

  spiSend(_regCtl);

  return(true);
}

MD_AD9833::channel_t MD_AD9833::getActivePhase(void)
{ 
  return bitRead(_regCtl, AD_PSELECT) ? CHAN_1 : CHAN_0; 
};

boolean MD_AD9833::setMode(mode_t mode)
{
  PRINTS("\nsetWave ");
  _modeLast = mode;

  switch (mode)
  {
  case MODE_OFF:
    PRINTS("OFF");
    bitClear(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitSet(_regCtl, AD_SLEEP1);
    bitSet(_regCtl, AD_SLEEP12);
    break;
  case MODE_SINE:
    PRINTS("SINE");
    bitClear(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case MODE_SQUARE1:
    PRINTS("SQ1");
    bitSet(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitSet(_regCtl, AD_DIV2);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case MODE_SQUARE2:
    PRINTS("SQ2");
    bitSet(_regCtl, AD_OPBITEN);
    bitClear(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_DIV2);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  case MODE_TRIANGLE:
    PRINTS("TRNG");
    bitClear(_regCtl, AD_OPBITEN);
    bitSet(_regCtl, AD_MODE);
    bitClear(_regCtl, AD_SLEEP1);
    bitClear(_regCtl, AD_SLEEP12);
    break;
  }

  spiSend(_regCtl);

  return(true);
}

uint32_t MD_AD9833::calcFreq(float f) 
// Calculate register value for AD9833 frequency register 
// from the specified frequency
{ 
  return (uint32_t)((f * AD_2POW28/_mClk) + 0.5);
}

uint16_t MD_AD9833::calcPhase(float a) 
// Calculate the value for AD9833 phase register from given 
// phase in tenths of a degree
{
  return (uint16_t)((512.0 * (a/10) / 45) + 0.5);
}

boolean MD_AD9833::setFrequency(channel_t chan, float freq)
{
  uint16_t  freq_select = SEL_FREQ0;   // stop ESP32 compiler warnings

  PRINT("\nsetFreq CHAN_", chan);

  _freq[chan] = freq;

  _regFreq[chan] = calcFreq(freq);

  PRINT(" - freq ", _freq[chan]);
  PRINTX(" =", _regFreq[chan]);

  // select the address mask
  switch (chan)
  {
  case CHAN_0:  freq_select = SEL_FREQ0; break;
  case CHAN_1:  freq_select = SEL_FREQ1; break;
  }

  // Assumes B28 is on so we can send consecutive words
  // B28 is set by default for the library, so just send it here
  // Now send the two parts of the frequency 14 bits at a time,
  // LSBs first

  spiSend(_regCtl);   // set B28
  spiSend(freq_select | (uint16_t)(_regFreq[chan] & 0x3fff));
  spiSend(freq_select | (uint16_t)((_regFreq[chan] >> 14) & 0x3fff));

  return(true);
}

boolean MD_AD9833::setPhase(channel_t chan, uint16_t phase)
{
  uint16_t  phase_select = SEL_PHASE0;     // stop ESP32 compiler warnings

  PRINT("\nsetPhase CHAN_", chan);

  _phase[chan] = phase;
  _regPhase[chan] = calcPhase(phase);

  PRINT(" - phase ", _phase[chan]);
  PRINTX(" =", _regPhase[chan]);

  // select the address mask
  switch (chan)
  {
  case CHAN_0:  phase_select = SEL_PHASE0; break;
  case CHAN_1:  phase_select = SEL_PHASE1; break;
  }

  // Now send the phase as 12 bits with appropriate address bits
  spiSend(phase_select | (0xfff & _regPhase[chan]));
  
  return(true);

}

