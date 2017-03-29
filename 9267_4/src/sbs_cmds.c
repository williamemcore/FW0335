// $Id: sbs_cmds.c,v 1.4 2012/12/05 22:28:24 gus Exp $
/*******************************************************************************************
 *
 * SBS Module Commands
 *
 *******************************************************************************************/
//$Log: sbs_cmds.c,v $
//Revision 1.4  2012/12/05 22:28:24  gus
//Change 2 GHz SBS frequency step change from 500 KHz to 10 KHz for RFoG module.
//
//Revision 1.3  2011/03/02 15:34:03  gus
//Fix T_STR type paramters and define T_DAC for DAC table parameters.
//
//Revision 1.2  2010/09/13 21:16:49  gus
//Clean up I/O.
//
//Revision 1.1  2010/09/13 19:29:47  gus
//Initial project inherited from auroraLM2.
//
//


//#include "compiler_defs.h"             // Compiler declarations

#include "sbs_cmds.h"
#include "hal_data.h"
#include "dac.h"
#include "sbs_cmds.h"






// EXTERNS
//extern const float MIN2G; //2G SBS VCO frequency - defined in accessor
//extern const float MAX2G;
//extern uint8_t data_dac[8];


//Prototypes
//Global variables
//float freq2G; //2G SBS VCO frequency (managed by accessor)
//static float Ss2g = 0.5;  // fixed step size of 500 KHz
//extern static void wait(int value);



//***************************************************************************************
// initSBSSPIport()
// Initialize 2 GHz and 6 GHz SBS modules
//
//***************************************************************************************
void initSBSSPIport(unsigned int sbsFreqMhz) {
	//int i;

	//sbsTxCompleteMbox = OSMboxCreate((void *)NULL); //Message box to signal SPI transmit complete
	//assert(sbsTxCompleteMbox); //error

	// ADF4360-2, 2 GHz
	// first time initialization
	
	//writeSBS(SBS_2G, SBS2G_N_COUNT); // N Counter
	
	writeSBS(SBS2G_R_COUNT); // R Counter
	writeSBS(SBS2G_CONTROL); // Control
									 
	//OSTimeDly(TIME_5ms); 			 // Need a delay of >5mS due to Cn=10uF per datasheet.
	//for (i=0; i<1000; i++);
	wait(80);

	writeSBS(SBS2G_N_COUNT); // N Counter (again)
	//setFreqSBS(SBS_2G, (unsigned int)sbsFreqMhz);
	setFreqSBS(SBS_2G, (float)sbsFreqMhz);
}



//***************************************************************************************
// setFreqSBS()
// Set SBS VCO frequency
//   Arguments:
//   sbs_id: SBS_2G or SBS_6G
//   freq:   frequency in MHz
//
//***************************************************************************************
void setFreqSBS(enum SBS_ID sbs_id, float freq) {
	long countA, countB;
	long cmd;

	countB = (long)(freq / 16.);
	//countA = (long)((unsigned int)(freq * 2) % countB);
	countA = (long)((long)(freq * 2) % countB);
	if (sbs_id == SBS_2G) 
	{
		//validate counter A and B values
		countA = (countA > SBS2G_MAX_A)? SBS2G_MAX_A : countA;
		countB = (countB > SBS2G_MAX_B)? SBS2G_MAX_B : (countB < SBS2G_MIN_B)? SBS2G_MIN_B : countB;
		//build N counter register command
		cmd = (SBS2G_N_COUNT & SBS2G_N_MASK) | (countB << 8) | (countA << 2);
		writeSBS(cmd);
	}

}


