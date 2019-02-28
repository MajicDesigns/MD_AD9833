/**
\mainpage Arduino AD9833 Library
The Analog Devices AD9833 Programmable Waveform Generator IC
------------------------------------------------------------
The AD9833 is a low power, programmable waveform generator capable 
of producing sine, triangular, and square wave outputs on 2 
separate channels, with one enabled for output at any time. The 
output frequency, phase and all other parameters are software 
programmable, allowing easy tuning with no additional external 
components.

The AD9833 is suitable for simple signal generator applications but it
can also be configured to perform simple modulation, such as FSK, or
more complex modulation schemes, such as GMSK and QPSK.

The AD9833 contains a 16-bit control register that allows the user to 
configure the operation of the hardware, via a 3-wire serial interface. 
This SPI connection is the microprocessor's only interface to control
the signal generator.

This library features access to all on-chip features though an abstracted 
class and methods to coordinate register changes to implement user-level
functionality.

Topics
------
- \subpage pageRevHistory
- \subpage pageCopyright
- \subpage pageDonation

\page pageDonation Support the Library
If you like and use this library please consider making a small donation using [PayPal](https://paypal.me/MajicDesigns/4USD)

\page pageRevHistory Revision History
Feb 2019 version 1.1.2
- Changed frequency to float type
- Some minor spelling errors

Jul 2018 version 1.1.1
- Minor Documentation updates, added LICENCE

Jan 2018 version 1.1.0
- Minor updates and changes

Feb 2016 version 1.0.0
- Initial version created

\page pageCopyright Copyright
Copyright (C) 2016 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
  
#pragma once
#include <Arduino.h>
#include <MD_AD9833.h>

/**
 * \file
 * \brief Main header file for the MD_AD9833 library
 */

/**
 * Core object for the MD_AD9833 library
 */
class MD_AD9833
{
  public:
    
 /**
  * Channel enumerated type.
  *
  * This enumerated type is used with the to specify which channel
  * is being invoked on operations that could be channel related.
  */
  enum channel_t
  {
    CHAN_0 = 0,		///< Channel 0 definition
    CHAN_1 = 1,  	///< Channel 1 definition
  };

 /**
  * Output mode request enumerated type.
  *
  * This enumerated type is used with the \ref setMode() methods to identify
  * the mode request.
  */
  enum mode_t
  {
    MODE_OFF,       ///< Set output all off
    MODE_SINE,      ///< Set output to a sine wave at selected frequency
    MODE_SQUARE1,   ///< Set output to a square wave at selected frequency
    MODE_SQUARE2,   ///< Set output to a square wave at half selected frequency
    MODE_TRIANGLE,  ///< Set output to a triangle wave at selected frequency
  };

 /**
  * Class Constructor - arbitrary digital interface.
  *
  * Instantiate a new instance of the class. The parameters passed are used to
  * connect the software to the hardware. Multiple instances may co-exist
  * but they should not share the same hardware Fsync pin (SPI interface).
  *
  * \param dataPin		output on the Arduino where data gets shifted out.
  * \param clkPin		  output for the clock signal.
  * \param fsyncPin		output for selecting the device.
  */
  MD_AD9833(uint8_t dataPin, uint8_t clkPin, uint8_t fsyncPin);

  /**
  * Class Constructor - Hardware SPI interface.
  *
  * Instantiate a new instance of the class. The SPI library is used to run
  * the native SPI hardware on the MCU to connect the software to the 
  * hardware. Multiple instances may co-exist but they should not share 
  * the same hardware Fsync pin (SPI interface).
  *
  * \param fsyncPin		output for selecting the device.
  */
  MD_AD9833(uint8_t fsyncPin);

 /**
  * Initialize the object.
  *
  * Initialise the object data. This needs to be called during setup() to initialise new
  * data for the class that cannot be done during the object creation.
  *
  * The AD9833 hardware is reset and set up to output a 1kHz Sine wave, 0 degrees
  * phase angle, CHAN_0 is selected as source for frequency and phase output.
  */
  void begin(void);

 /**
  * Class Destructor.
  *
  * Released allocated memory and does the necessary to clean up once the object is
  * no longer required.
  */
  ~MD_AD9833();
  
  /** @} */

  //--------------------------------------------------------------
  /** \name Methods for AD9833 frequency control
   * @{
   */

  /**
  * Get channel output waveform
  *
  * Get the last specified AD9833 channel output waveform.
  *
  * \return last wave_t setting for the waveform
  */
  inline mode_t getMode(void) { return _modeLast; };

  /**
   * Set channel output mode
   *
   * Set the specified AD9833 channel output waveform to one of the mode_t types.
   *
   * \param mode  wave defined by one of the mode_t enumerations
   * \return true if successful, false otherwise
   */
  boolean setMode(mode_t mode);

  /**
  * Get current frequency output channel
  *
  * Get the last specified AD9833 frequency output channel selection.
  *
  * \return last frequency setting for the waveform
  */
  channel_t getActiveFrequency(void);

  /**
  * Set the frequency channel for output
  *
  * Set the specified AD9833 frequency setting as the output frequency.
  *
  * \param chan output channel identifier (channel_t)
  * \return true if successful, false otherwise
  */
  boolean setActiveFrequency(channel_t chan);

  /**
  * Get channel frequency
  *
  * Get the last specified AD9833 channel output frequency.
  *
  * \param chan output channel identifier (channel_t)
  * \return the last frequency setting for the specified channel
  */
  inline float getFrequency(channel_t chan) const { return _freq[chan]; };

  /**
  * Set channel frequency
  *
  * Set the specified AD9833 channel output frequency.
  *
  * \param chan output channel identifier (channel_t)
  * \param freq frequency in Hz
  * \return true if successful, false otherwise
  */
  boolean setFrequency(channel_t chan, float freq);

  /** @} */


  //--------------------------------------------------------------
  /** \name Methods for AD98933 phase control
   * @{
   */
  /**
  * Get current phase output channel
  *
  * Get the last specified AD9833 frequency output channel selection.
  *
  * \return last phase setting for the waveform
  */
  channel_t getActivePhase(void);

  /**
  * Set the frequency channel for output
  *
  * Set the specified AD9833 frequency setting as the output frequency.
  *
  * \param chan output channel identifier (channel_t)
  * \return true if successful, false otherwise
  */
  boolean setActivePhase(channel_t chan);
  
  /**
  * Get channel phase
  *
  * Get the last specified AD9833 channel phase setting in tenths of a degree.
  *
  * \param chan output channel identifier (channel_t)
  * \return the last phase setting in degrees [0..3600] for the specified channel
  */
  inline uint16_t getPhase(channel_t chan) { return _phase[chan]; };

  /**
  * Set channel phase
  *
  * Set the specified AD9833 channel output phase in tenths of a degree.
  * 100.1 degrees is passed as 1001.
  *
  * \param chan output channel identifier (channel_t)
  * \param phase in tenths of a degree [0..3600]
  * \return true if successful, false otherwise
  */
  boolean setPhase(channel_t chan, uint16_t phase);

  /** @} */

private:
  // Hardware register images
  uint16_t  _regCtl;       // control register image
  uint32_t  _regFreq[2];   // frequency registers
  uint32_t  _regPhase[2];  // phase registers

  // Settings memory
  mode_t    _modeLast;    // last set mode
  float     _freq[2];     // last frequencies set
  uint16_t  _phase[2];    // last phase setting

  // SPI interface data
  uint8_t	_dataPin;	  	// DATA is shifted out of this pin ...
  uint8_t	_clkPin;	  	// ... signaled by a CLOCK on this pin ...
  uint8_t	_fsyncPin;		// ... and LOADed when the fsync pin is driven HIGH to LOW
  bool		_hardwareSPI;	// true if SPI interface is the hardware interface

  // SPI related 
  void dumpCmd(uint16_t reg);       // debug routine
  void spiSend(uint16_t data);			// do the actual physical communications task
};
