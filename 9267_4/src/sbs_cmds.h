// $Id: sbs_cmds.h,v 1.2 2012/12/05 22:28:24 gus Exp $
#ifndef SBSCMDS_INCLUDE
#define SBSCMDS_INCLUDE 1
//----------------------------------------------------------------------------
//
// SBS Module Command Definitions
//
//----------------------------------------------------------------------------
//$Log: sbs_cmds.h,v $
//Revision 1.2  2012/12/05 22:28:24  gus
//Change 2 GHz SBS frequency step change from 500 KHz to 10 KHz for RFoG module.
//
//Revision 1.1  2010/09/13 19:29:47  gus
//Initial project inherited from auroraLM2.
//
//

//#include "includes.h"

enum SBS_ID {
	SBS_2G,
	SBS_6G
};

void initSBSSPIport(unsigned int sbsFreqMhz);
void writeSBS(long cmd);
void setFreqSBS(enum SBS_ID sbs_id, float freq);


//extern float freq2G; //2G SBS VCO frequency (managed by accessor)


// ADF4360-2, 2 GHz
// Changing the SBS frequency only requires a write to the N Counter register
// Freq(vco) = (B * P + A) * 20 MHz / R
//           = (123 * 32 + 6) * 20E6 / 40 = 1971 MHz
//
// Hy --> 2405Mhz   = SBS2G_N_COUNT 0x00962A	B=150	A=10
//    --> 2500Mhz   = SBS2G_N_COUNT 0x009C22 	B=156	A=8
//    --> 2600Mhz   = SBS2G_N_COUNT 0x00A242  	B=162 	A=16
//    --> 2700Mhz   = SBS2G_N_COUNT 0x00A862	B=168	A=24
//
//
  #define SBS2G_N_COUNT 0x00A242 // N Counter: A=6, B=123
  #define SBS2G_R_COUNT 0x1000A1 // R Counter: R=40, step size (20 MHz / R) = 500 KHz, Clock Divider=2
  #define SBS2G_CONTROL 0x8FF124 // Control: P=32, core=10 mA :: 0x8FF10C --> 0x8FF12C enable Muxout for digital lock detect
  #define SBS2G_N_MASK  0xE00003 // Mask N Counter: A=0, B=0
  #define SBS2G_MAX_A   31       // A counter range 0 to 31
  #define SBS2G_MIN_B   3        // B counter range 3 8191
  #define SBS2G_MAX_B   8191






#endif
