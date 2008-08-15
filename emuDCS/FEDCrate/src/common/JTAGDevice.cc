/*****************************************************************************\
* $Id: JTAGDevice.cc,v 3.2 2008/08/15 08:35:51 paste Exp $
*
* $Log: JTAGDevice.cc,v $
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "JTAGDevice.h"

#include "JTAG_constants.h"

emu::fed::JTAGDevice::JTAGDevice() {
	geo[0].nam = "NONE";
	geo[0].jchan = -1;
	geo[0].kbypass = 0;
	geo[0].kbit = 0;
	geo[0].kbybit = 0;
	geo[0].nser = 0;
	geo[0].seri[0] = 0; geo[0].seri[1] = 0; geo[0].seri[2] = 0; geo[0].seri[3] = 0; geo[0].seri[4] = 0;
	geo[0].sxtrbits = 0;
	geo[0].femask = 0xff;

	geo[1].nam = "OUTFIFO";
	geo[1].jchan = 1;
	geo[1].kbypass = PROM_BYPASS;
	geo[1].kbit = 8;
	geo[1].kbybit = 1;
	geo[1].nser = 0;
	geo[1].seri[0] = 0; geo[1].seri[1] = 0; geo[1].seri[2] = 0; geo[1].seri[3] = 0; geo[1].seri[4] = 0;
	geo[1].sxtrbits = 0;
	geo[1].femask = 0xff;

	geo[2].nam = "VMEPROM";
	geo[2].jchan = 2;
	geo[2].kbypass = PROM_BYPASS;
	geo[2].kbit = 8;
	geo[2].kbybit = 1;
	geo[2].nser = 0;
	geo[2].seri[0] = 0; geo[2].seri[1] = 0; geo[2].seri[2] = 0; geo[2].seri[3] = 0; geo[2].seri[4] = 0;
	geo[2].sxtrbits = 0;
	geo[2].femask = 0xff;

	geo[3].nam = "DDUPROM0";
	geo[3].jchan = 3;
	geo[3].kbypass = PROM_BYPASS;
	geo[3].kbit = 8;
	geo[3].kbybit = 1;
	geo[3].nser = 2;
	geo[3].seri[0] = 3; geo[3].seri[1] = -4; geo[3].seri[2] = 0; geo[3].seri[3] = 0; geo[3].seri[4] = 0;
	geo[3].sxtrbits = 1;
	geo[3].femask = 0xff;
	
	geo[4].nam = "DDUPROM1";
	geo[4].jchan = 3;
	geo[4].kbypass = PROM_BYPASS;
	geo[4].kbit = 8;
	geo[4].kbybit = 1;
	geo[4].nser = 2;
	geo[4].seri[0] = -3; geo[4].seri[1] = 4; geo[4].seri[2] = 0; geo[4].seri[3] = 0; geo[4].seri[4] = 0;
	geo[4].sxtrbits = 0;
	geo[4].femask = 0xff;

	geo[5].nam = "INPROM0";
	geo[5].jchan = 4;
	geo[5].kbypass = PROM_BYPASS;
	geo[5].kbit = 8;
	geo[5].kbybit = 1;
	geo[5].nser = 2;
	geo[5].seri[0] = 5; geo[5].seri[1] = -6; geo[5].seri[2] = 0; geo[5].seri[3] = 0; geo[5].seri[4] = 0;
	geo[5].sxtrbits = 1;
	geo[5].femask = 0xff;
	
	geo[6].nam = "INPROM1";
	geo[6].jchan = 4;
	geo[6].kbypass = PROM_BYPASS;
	geo[6].kbit = 8;
	geo[6].kbybit = 1;
	geo[6].nser = 2;
	geo[6].seri[0] = -5; geo[6].seri[1] = 6; geo[6].seri[2] = 0; geo[6].seri[3] = 0; geo[6].seri[4] = 0;
	geo[6].sxtrbits = 0;
	geo[6].femask = 0xff;
	
	geo[7].nam = "DDUFPGA";
	geo[7].jchan = 5;
	geo[7].kbypass = PROM_BYPASS;
	geo[7].kbit = 10;
	geo[7].kbybit = 1;
	geo[7].nser = 0;
	geo[7].seri[0] = 0; geo[7].seri[1] = 0; geo[7].seri[2] = 0; geo[7].seri[3] = 0; geo[7].seri[4] = 0;
	geo[7].sxtrbits = 0;
	geo[7].femask = 0xff;

	geo[8].nam = "INFPGA0";
	geo[8].jchan = 6;
	geo[8].kbypass = VTX2_BYPASS;
	geo[8].kbit = 14;
	geo[8].kbybit = 1;
	geo[8].nser = 0;
	geo[8].seri[0] = 0; geo[8].seri[1] = 0; geo[8].seri[2] = 0; geo[8].seri[3] = 0; geo[8].seri[4] = 0;
	geo[8].sxtrbits = 0;
	geo[8].femask = 0xff;

	geo[9].nam = "INFPGA1";
	geo[9].jchan = 7;
	geo[9].kbypass = VTX2_BYPASS;
	geo[9].kbit = 14;
	geo[9].kbybit = 1;
	geo[9].nser = 0;
	geo[9].seri[0] = 0; geo[9].seri[1] = 0; geo[9].seri[2] = 0; geo[9].seri[3] = 0; geo[9].seri[4] = 0;
	geo[9].sxtrbits = 0;
	geo[9].femask = 0xff;

	geo[10].nam = "SLINK";
	geo[10].jchan = 8;
	geo[10].kbypass = PROM_BYPASS;
	geo[10].kbit = 8;
	geo[10].kbybit = 1;
	geo[10].nser = 0;
	geo[10].seri[0] = 0; geo[10].seri[1] = 0; geo[10].seri[2] = 0; geo[10].seri[3] = 0; geo[10].seri[4] = 0;
	geo[10].sxtrbits = 0;
	geo[10].femask = 0xff;

	geo[11].nam = "VMEPARA";
	geo[11].jchan = 9;
	geo[11].kbypass = PROM_BYPASS;
	geo[11].kbit = 8;
	geo[11].kbybit = 1;
	geo[11].nser = 0;
	geo[11].seri[0] = 0; geo[11].seri[1] = 0; geo[11].seri[2] = 0; geo[11].seri[3] = 0; geo[11].seri[4] = 0;
	geo[11].sxtrbits = 0;
	geo[11].femask = 0xff;

	geo[12].nam = "VMESERI";
	geo[12].jchan = 10;
	geo[12].kbypass = PROM_BYPASS;
	geo[12].kbit = 8;
	geo[12].kbybit = 1;
	geo[12].nser = 0;
	geo[12].seri[0] = 0; geo[12].seri[1] = 0; geo[12].seri[2] = 0; geo[12].seri[3] = 0; geo[12].seri[4] = 0;
	geo[12].sxtrbits = 0;
	geo[12].femask = 0xff;

	geo[13].nam = "RESET";
	geo[13].jchan = 12;
	geo[13].kbypass = PROM_BYPASS;
	geo[13].kbit = 8;
	geo[13].kbybit = 1;
	geo[13].nser = 0;
	geo[13].seri[0] = 0; geo[13].seri[1] = 0; geo[13].seri[2] = 0; geo[13].seri[3] = 0; geo[13].seri[4] = 0;
	geo[13].sxtrbits = 0;
	geo[13].femask = 0xff;

	geo[14].nam = "SADC";
	geo[14].jchan = 13;
	geo[14].kbypass = PROM_BYPASS;
	geo[14].kbit = 8;
	geo[14].kbybit = 1;
	geo[14].nser = 0;
	geo[14].seri[0] = 0; geo[14].seri[1] = 0; geo[14].seri[2] = 0; geo[14].seri[3] = 0; geo[14].seri[4] = 0;
	geo[14].sxtrbits = 0;
	geo[14].femask = 0xff;

	geo[15].nam = "MPROM";
	geo[15].jchan = 2;
	geo[15].kbypass = PROM_BYPASS;
	geo[15].kbit = 16;
	geo[15].kbybit = 1;
	geo[15].nser = 0;
	geo[15].seri[0] = 0; geo[15].seri[1] = 0; geo[15].seri[2] = 0; geo[15].seri[3] = 0; geo[15].seri[4] = 0;
	geo[15].sxtrbits = 0;
	geo[15].femask = 0xff;

	geo[16].nam = "INPROM";
	geo[16].jchan = 3;
	geo[16].kbypass = PROM_BYPASS;
	geo[16].kbit = 8;
	geo[16].kbybit = 1;
	geo[16].nser = 0;
	geo[16].seri[0] = 0; geo[16].seri[1] = 0; geo[16].seri[2] = 0; geo[16].seri[3] = 0; geo[16].seri[4] = 0;
	geo[16].sxtrbits = 0;
	geo[16].femask = 0xff;
	
	geo[17].nam = "INCTRL1";
	geo[17].jchan = 4;
	geo[17].kbypass = PROM_BYPASS;
	geo[17].kbit = 8;
	geo[17].kbybit = 1;
	geo[17].nser = 5;
	geo[17].seri[0] = -18; geo[17].seri[1] = 19; geo[17].seri[2] = 20; geo[17].seri[3] = 21; geo[17].seri[4] = 22;
	geo[17].sxtrbits = 0;
	geo[17].femask = 0xff;
	
	geo[18].nam = "INCTRL2";
	geo[18].jchan = 4;
	geo[18].kbypass = PROM_BYPASS;
	geo[18].kbit = 8;
	geo[18].kbybit = 1;
	geo[18].nser = 5;
	geo[18].seri[0] = 18; geo[18].seri[1] = -19; geo[18].seri[2] = 20; geo[18].seri[3] = 21; geo[18].seri[4] = 22;
	geo[18].sxtrbits = 1;
	geo[18].femask = 0xff;

	geo[19].nam = "INCTRL3";
	geo[19].jchan = 4;
	geo[19].kbypass = PROM_BYPASS;
	geo[19].kbit = 8;
	geo[19].kbybit = 1;
	geo[19].nser = 5;
	geo[19].seri[0] = 18; geo[19].seri[1] = 19; geo[19].seri[2] = -20; geo[19].seri[3] = 21; geo[19].seri[4] = 22;
	geo[19].sxtrbits = 3;
	geo[19].femask = 0xff;

	geo[20].nam = "INCTRL4";
	geo[20].jchan = 4;
	geo[20].kbypass = PROM_BYPASS;
	geo[20].kbit = 8;
	geo[20].kbybit = 1;
	geo[20].nser = 5;
	geo[20].seri[0] = 18; geo[20].seri[1] = 19; geo[20].seri[2] = 20; geo[20].seri[3] = -21; geo[20].seri[4] = 22;
	geo[20].sxtrbits = 3;
	geo[20].femask = 0xff;

	geo[21].nam = "INCTRL5";
	geo[21].jchan = 4;
	geo[21].kbypass = PROM_BYPASS;
	geo[21].kbit = 8;
	geo[21].kbybit = 1;
	geo[21].nser = 5;
	geo[21].seri[0] = 18; geo[21].seri[1] = 19; geo[21].seri[2] = 20; geo[21].seri[3] = 21; geo[21].seri[4] = -22;
	geo[21].sxtrbits = 4;
	geo[21].femask = 0xff;

	geo[22].nam = "MCTRL";
	geo[22].jchan = 11;
	geo[22].kbypass = PROM_BYPASS;
	geo[22].kbit = 10;
	geo[22].kbybit = 1;
	geo[22].nser = 0;
	geo[22].seri[0] = 0; geo[22].seri[1] = 0; geo[22].seri[2] = 0; geo[22].seri[3] = 0; geo[22].seri[4] = 0;
	geo[22].sxtrbits = 0;
	geo[22].femask = 0xff;

	geo[23].nam = "RESET1";
	geo[23].jchan = 12;
	geo[23].kbypass = PROM_BYPASS;
	geo[23].kbit = 8;
	geo[23].kbybit = 1;
	geo[23].nser = 2;
	geo[23].seri[0] = -24; geo[23].seri[1] = 25; geo[23].seri[2] = 0; geo[23].seri[3] = 0; geo[23].seri[4] = 0;
	geo[23].sxtrbits = 0;
	geo[23].femask = 0xff;

	geo[24].nam = "RESET2";
	geo[24].jchan = 12;
	geo[24].kbypass = PROM_BYPASS;
	geo[24].kbit = 8;
	geo[24].kbybit = 1;
	geo[24].nser = 2;
	geo[24].seri[0] = 24; geo[24].seri[1] = -25; geo[24].seri[2] = 0; geo[24].seri[3] = 0; geo[24].seri[4] = 0;
	geo[24].sxtrbits = 1;
	geo[24].femask = 0xff;

}
