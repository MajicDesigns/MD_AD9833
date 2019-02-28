/*
MD_AD9833 - Library for controlling an AD9833 Programmable Waveform Generator.

See the main header file for full information
*/
#pragma once

/**
* \file
* \brief Includes library-only definitions for AD_9833 library
*/

#define	AD_DEBUG	0		///< Enable or disable (default) debugging output from the MD_AD9833 library

#if AD_DEBUG
#define	PRINT(s, v)		{ Serial.print(F(s)); Serial.print(v); }		  ///< Print a string followed by a value (decimal)
#define	PRINTX(s, v)	{ Serial.print(F(s)); Serial.print(" 0x"); Serial.print(v, HEX); }	///< Print a string followed by a value (hex)
#define	PRINTB(s, v)	{ Serial.print(F(s)); Serial.print(v, BIN); }	///< Print a string followed by a value (binary)
#define	PRINTS(s)		  { Serial.print(F(s)); }							          ///< Print a string
#else
#define	PRINT(s, v)		///< Print a string followed by a value (decimal)
#define	PRINTX(s, v)	///< Print a string followed by a value (hex)
#define	PRINTB(s, v)	///< Print a string followed by a value (binary)
#define	PRINTS(s)		  ///< Print a string
#endif

/** \name Library defaults
 * @{
 */
#define AD_DEFAULT_FREQ   1000  ///< Default initialisation frequency (Hz)
#define AD_DEFAULT_PHASE  0     ///< Default initialisation phase angle (degrees)

/** @}*/ 

/** \name AD9833 Control Register bit definitions
 * @{
 */
const uint8_t AD_B28 = 13;      ///< B28 = 1 allows a complete word to be loaded into a frequency register in 
                                ///< two consecutive writes. When B28 = 0, the 28-bit frequency register 
                                ///< operates as two 14-bit registers.
const uint8_t AD_HLB = 12;      ///< Control bit allows the user to continuously load the MSBs or LSBs of a 
                                ///< frequency register while ignoring the remaining 14 bits. HLB is used 
                                ///< in conjunction with B28; when B28 = 1, this control bit is ignored.
const uint8_t AD_FSELECT = 11;  ///< Defines whether the FREQ0 register or the FREQ1 register is used in 
                                ///< the phase accumulator.
const uint8_t AD_PSELECT = 10;  ///< Defines whether the PHASE0 register or the PHASE1 register data is 
                                ///< added to the output of the phase accumulator.
const uint8_t AD_RESET = 8;     ///< Reset = 1 resets internal registers to 0, which corresponds to an 
                                ///< analog output of midscale. Reset = 0 disables reset.
const uint8_t AD_SLEEP1 = 7;    ///< When SLEEP1 = 1, the internal MCLK clock is disabled, and the DAC output 
                                ///< remains at its present value. When SLEEP1 = 0, MCLK is enabled.
const uint8_t AD_SLEEP12 = 6;   ///< SLEEP12 = 1 powers down the on-chip DAC. SLEEP12 = 0 implies that 
                                ///< the DAC is active.
const uint8_t AD_OPBITEN = 5;   ///< When OPBITEN = 1, the output of the DAC is no longer available at the 
                                ///< VOUT pin, replaced by MSB (or MSB/2) of the DAC. When OPBITEN = 0, the 
                                ///< DAC is connected to VOUT.
const uint8_t AD_DIV2 = 3;      ///< When DIV2 = 1, the MSB of the DAC data is passed to the VOUT pin. When 
                                ///< DIV2 = 0, the MSB/2 of the DAC data is output at the VOUT pin.
const uint8_t AD_MODE = 1;      ///< When MODE = 1, the SIN ROM is bypassed, resulting in a triangle output 
                                ///< from the DAC. When MODE = 0, the SIN ROM is used which results in a 
                                ///< sinusoidal signal at the output.

/** @}*/

/** \name AD9833 Frequency and Phase register bit definitions
* @{
*/
const uint8_t AD_FREQ1 = 15;    ///< Select frequency 1 register
const uint8_t AD_FREQ0 = 14;    ///< Select frequency 0 register
const uint8_t AD_PHASE = 13;    ///< Select the phase register

/** @}*/

/** \name AD9833 Freq and Phase register address identifiers
* @{
*/
#define SEL_FREQ0  (1<<AD_FREQ0)
#define SEL_FREQ1  (1<<AD_FREQ1)
#define SEL_PHASE0 (1<<AD_FREQ0 | 1<<AD_FREQ1 | 0<<AD_PHASE)
#define SEL_PHASE1 (1<<AD_FREQ0 | 1<<AD_FREQ1 | 1<<AD_PHASE)

/** @}*/

/** \name AD9833 frequency and phase calculation macros
* @{ */
#define AD_MCLK   25000000UL  ///< Clock speed of the AD9833 reference clock in Hz
#define AD_2POW28 (1UL << 28) ///< Used when calculating output frequency

/** Macro to calculate register value for AD9833 frequency register from a frequency */
#define FREQ_CALC(f) (float)(round(f*(double)AD_2POW28/(double)AD_MCLK))

/** Macro to calculate the value for AD9833 phase register from given phase in tenths of a degree */
#define CALC_PHASE(a) (uint16_t)(512.0*((float)a/10.0)/45.0)

/** @} */
