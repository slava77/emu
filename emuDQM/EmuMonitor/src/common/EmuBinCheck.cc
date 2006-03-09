#include "EmuLocalPlotter.h"

unsigned int EmuLocalPlotter::bin_check(unsigned char * data, int dataSize)
{
        bool save_debug=debug_printout;
        map<string, TH1*> h = histos[0];

	long fControlWord=0;

	bool fDDU_Header=false, fDDU_Trailer=false;
	bool fDMB_Header=false, fALCT_Header=false, fTMB_Header=false;

	long cntDDU_Headers=0, cntDDU_Trailers=0;

	long DDU_WordsSinceLastHeader=0;
	long DDU_WordCount=0;
	long DDU_WordMismatch_Occurrences=0;
	long DDU_WordsSinceLastTrailer=0;

	long ALCT_WordsSinceLastHeader=0;
	long ALCT_WordCount=0;
	long ALCT_WordsExpected=0;

	long TMB_WordsSinceLastHeader=0;
	long TMB_WordCount=0;
	long TMB_WordsExpected=0;
	long TMB_Tbins=0;
	long TMB_WordsExpectedCorrection=0;

	long CFEB_SampleWordCount=0;
	long CFEB_SampleCount=0, CFEB_BSampleCount=0;

	bool fERROR[20], fWARNING[5], fBadEvent;
	long cntERROR[20], cntWARNING[5], cntBadEvent=0;
	string sERROR[20], sWARNING[5];

	// == Define 16bit words buffer size of 4
	unsigned short buf_2[4], buf_1[4], buf0[4], buf1[4], buf2[4];
        bool crcALCT=true, crcTMB=true, crcFFEB=true;

// Lager buffer for CRC checks
	unsigned short largeBuf[1048576/2], *largeBuf_pointer=largeBuf, *alct_data=0, *tmb_data=0;

	// == Set buffer to 0's
	bzero(buf_2, sizeof(buf_2));
	bzero(buf_1, sizeof(buf_1));
	bzero(buf0, sizeof(buf0));
	bzero(buf1, sizeof(buf1));
	bzero(buf2, sizeof(buf2));

	// set ERROR and WARNING counters and flags to zero
	bzero(cntERROR, sizeof(cntERROR));
	bzero(cntWARNING, sizeof(cntWARNING));
	bzero(fERROR, sizeof(fERROR));
	bzero(fWARNING, sizeof(fWARNING));

	// == Define strings describing ERRORs
	sERROR[0] = " DDU Trailer Missing                              ";
	sERROR[1] = " DDU Header Missing                               ";
	sERROR[3] = " DDU Word Count Error                             ";
	sERROR[2] = " DDU CRC Error (not yet implemented)              ";

	sERROR[4] = " DMB Trailer Missing                              ";
	sERROR[5] = " DMB Header Missing                               ";

	sERROR[6] = " ALCT Trailer Missing                             ";
	sERROR[7] = " ALCT Header Missing                              ";
	sERROR[8] = " ALCT Word Count Error                            ";
	sERROR[9] = " ALCT CRC Error                                   ";

	sERROR[10] = "TMB Trailer Missing                              ";
	sERROR[11] = "TMB Header Missing                               ";
	sERROR[12] = "TMB Word Count Error                             ";
	sERROR[13] = "TMB CRC Error                                    ";

	sERROR[14] = "CFEB Word Count Per Sample Error                 ";
	sERROR[15] = "CFEB Sample Count Error                          ";
	sERROR[16] = "CFEB CRC Error                                   ";

	sERROR[17] = "ALCT Trailer Bit Error                           ";

	sERROR[18] = "DDU Event Size Limit Error                       ";
	// This is due to seeing many events in ddu293 (also, some in ddu294)
	// with some bits in the 1st ALCT D-Header being lost. This causes a chain of errors:
	// - TMB Trailer is not identified and TMB word count mismatch occur when Trailer is found
	// - CFEB sample count is not reset on ALCT Trailer.
	// To merge all these errors in one,
	// the D-signature in the 1st ALCT Trailer will not be required for identifying the ALCT Trailer;
	// However, if these bits are found to be missing, ERROR[17] will be flagged.
	// This is just a temporary patch to make the output look less clattered.


	// == Define strings defining WARNINGs
	sWARNING[0] = " Extra words between DDU Trailer and DDU Header ";
	sWARNING[1] = " CFEB B-Words                                   ";


        int ptr=0;
        // == Read from datafile 4 16bit words into buf till end-of-file found
        if (dataSize%8) {
            if (debug_printout) cout << "+++ Buffer 8-bytes boundary failed" << endl;
            h["hBinCheck_Errors"]->Fill(0.,18);
            return 1<<18;
        }

        while((ptr+8)<=dataSize)
	{

		// == Store last 4 read buffers in pipeline-like memory (note that memcpy works quite slower!)
		for (int i=0; i<4; i++) {
//		buf_2[i]=buf_1[i];         //  This bufer was not needed so far
		buf_1[i]=buf0[i];
		buf0[i]=buf1[i];
		buf1[i]=buf2[i];
		}

		memcpy((char *)buf2, data+ptr, 8);

                /*
		// == Read 8 bytes (4 16-bit words, or 1 64-bit word) into buffer
		if( startOfEvent ) memcpy((char *)buf2, data+ptr, sizeof(buf2));
                // input.read((char *)buf2, sizeof(buf2));
		else {
			int readed=0;
			for(int pos=0; pos<4 && (ptr<=datasize); readed++){

                                memcpy((char *)buf2+pos, data+ptr, sizeof(buf2[pos]));
                                ptr++;
				// input.read((char*)&buf2[pos],sizeof(buf2[pos]));
				switch( pos ){
					case 0: if( buf2[0]==0x8000 ) pos++; break;
					case 1: if( buf2[1]==0x8000 ) pos++; break;
					case 2: if( buf2[2]==0x0001 ) pos++; break;
					case 3: if( buf2[3]==0x8000 ) pos++; break;
					default : break;
				}
			}
			if( readed != 4 ) cout<<"Additional extra words for "<<cntDDU_Headers<<" DDU Header"<<endl;
			startOfEvent = true;
		}

//KKotov
		if( !fERROR[18] && DDU_WordsSinceLastHeader>50000 ){
			fERROR[18]=true;
			cntERROR[18]++;
			cntBadEvent++;
			cerr << endl << endl << "DDU Header Occurrence = " << cntDDU_Headers;
			cerr << "  ERROR 18    " << sERROR[18] << endl;
		}
                */
//KK
//KKotov
// For CRC checks you need more than just last four worlds
		if( buf2[0]==0x8000 && buf2[1]==0x8000 && buf2[2]==0x0001 && buf2[3]==0x8000 ) {
			largeBuf_pointer = largeBuf; alct_data=0; tmb_data=0;
			memcpy((void*)largeBuf_pointer,(void*)buf1,8); largeBuf_pointer += 4;
			memcpy((void*)largeBuf_pointer,(void*)buf2,8); largeBuf_pointer += 4;
		} else {
			if( (largeBuf_pointer-largeBuf)*2 < sizeof(largeBuf) - 8 ){
				memcpy((void*)largeBuf_pointer,(void*)buf2,8);
				largeBuf_pointer += 4;
			} /*else { cout<<"!!!!!!! Big event !!!!!!!"<<endl; }*/
		}
//KK

		// increment counter of 64-bit words since last DDU Header
		// this counter is reset if DDU Header is found
		if ( fDDU_Header ) { DDU_WordsSinceLastHeader++; }

		// increment counter of 64-bit words since last DDU Trailer
		// this counter is reset if DDU Trailer is found
		if ( fDDU_Trailer ) { DDU_WordsSinceLastTrailer++; }

		// increment counter of 16-bit words since last DMB*ALCT Header match
		// this counter is reset if ALCT Header is found right after DMB Header
		if ( fALCT_Header) { ALCT_WordsSinceLastHeader = ALCT_WordsSinceLastHeader + 4; }

		// increment counter of 16-bit words since last DMB*TMB Header match
		// this counter is reset if TMB Header is found right after DMB Header or ALCT Trailer
		if ( fTMB_Header ) { TMB_WordsSinceLastHeader = TMB_WordsSinceLastHeader + 4; }

		// increment counter of 16-bit words since last of DMB Header, ALCT Trailer, TMB Trailer,
		// CFEB Sample Trailer, CFEB B-word; this counter is reset by all these conditions
		if ( fDMB_Header ) { CFEB_SampleWordCount = CFEB_SampleWordCount + 4; }




		// == Check for Format Control Words, set proper flags, perform self-consistency checks

		// == DDU Header found
		if ( (buf0[0]==0x8000) &&
		     (buf0[1]==0x8000) &&
		     (buf0[2]==0x0001) &&
		     (buf0[3]==0x8000) ) {

			// Reset all Error and Warning flags to be false
			bzero(fERROR, sizeof(fERROR));
			bzero(fWARNING, sizeof(fWARNING));
			fBadEvent=false;

			if (fDDU_Header) {
 			// == Another DDU Header before encountering DDU Trailer!
			fERROR[0]=true;
			cntERROR[0]++;
			cntBadEvent++;
//			h["hDDUActualWordCount_NoTrailerEvents"]->Fill(DDU_WordsSinceLastHeader);
			cerr << endl << endl << "DDU Header Occurrence = " << cntDDU_Headers;
			cerr << "  ERROR 0    " << sERROR[0] << endl;
			}

			if (fDDU_Trailer && DDU_WordsSinceLastTrailer != 4) {
			// == Counted extraneous words between last DDU Trailer and this DDU Header
                            cntWARNING[0]++;
                            fWARNING[0]=true;
                            if (debug_printout) {
                                cerr << endl << "DDU Header Occurrence = " << cntDDU_Headers;
                                cerr << "  WARNING 0 " << sWARNING[0] << " " << DDU_WordsSinceLastTrailer
                                << " extra 64-bit words" << endl;
                            }
			}

                    fControlWord=1;
                    fDDU_Header=true;
                    fDDU_Trailer=false;
                    DDU_WordCount=0;

                    fDMB_Header=false;
                    fALCT_Header=false;
                    fTMB_Header=false;
                    ALCT_WordsSinceLastHeader=0;
                    ALCT_WordCount=0;
                    ALCT_WordsExpected=0;
                    TMB_WordsSinceLastHeader=0;
                    TMB_WordCount=0;
                    TMB_WordsExpected=0;
                    TMB_Tbins=0;
                    CFEB_SampleWordCount=0;
                    CFEB_SampleCount=0;
                    CFEB_BSampleCount=0;

                    cntDDU_Headers++;
                    DDU_WordsSinceLastHeader=0; // Reset counter of DDU Words since last DDU Header
                    if (debug_printout) {
                            cout << endl << "----------------------------------------------------------" << endl;
                            cout << "DDU  Header Occurrence " << cntDDU_Headers << endl;
                    }
		}


		// == DMB Header found
		if ( ((buf0[0]&0xF000)==0xA000) &&
		     ((buf0[1]&0xF000)==0xA000) &&
		     ((buf0[2]&0xF000)==0xA000) &&
		     ((buf0[3]&0xF000)==0xA000) )   {
                    fControlWord=2;
                    if(fDMB_Header) { fERROR[4]=true; }  // DMB Trailer is missing
                    fDMB_Header=true;

                    fALCT_Header=false;
                    fTMB_Header=false;
                    ALCT_WordsSinceLastHeader=0;
                    ALCT_WordCount=0;
                    ALCT_WordsExpected=0;
                    TMB_WordsSinceLastHeader=0;
                    TMB_WordCount=0;
                    TMB_WordsExpected=0;
                    TMB_Tbins=0;
                    CFEB_SampleWordCount=0;
                    CFEB_SampleCount=0;
                    CFEB_BSampleCount=0;

                    if (debug_printout) {
                        // Print DMB_ID from DMB Header
                        cout << "DMB=" << setw(2) << setfill('0') << (buf0[1]&0x000F) << " ";
                        // Print ALCT_DAV and TMB_DAV from DMB Header
                        cout << setw(1) << ((buf0[0]&0x0020) >> 5) << " "<< ((buf0[0]&0x0040) >> 6) << " ";
                        // Print CFEB_DAV from DMB Header
                        cout << setw(1) << ((buf0[0]&0x0010) >> 4) << ((buf0[0]&0x0008) >> 3)
                        << ((buf0[0]&0x0004) >> 2) << ((buf0[0]&0x0002) >> 1) << (buf0[0]&0x0001);
                        // Print DMB Header Tag
                        cout << " {";
                    }
		}


		// == ALCT Header found right after DMB Header
                //   (check for all currently reserved/fixed bits in ALCT first 4 words)
		if( ( ((buf0[0]&0xF800)==0x6000) &&
			((buf0[1]&0xFF80)==0x0080) &&
			((buf0[2]&0xF000)==0x0000) &&
			((buf0[3]&0xc000)==0x0000) )
                            &&
		    ( ((buf_1[0]&0xF000)==0xA000) &&
		      ((buf_1[1]&0xF000)==0xA000) &&
		      ((buf_1[2]&0xF000)==0xA000) &&
		      ((buf_1[3]&0xF000)==0xA000) )  )  {
		fControlWord=3;
		fALCT_Header=true;
		ALCT_WordsSinceLastHeader = 4;

		// Calculate expected number of ALCT words
			if ( (buf0[3]&0x0003) == 0) { ALCT_WordsExpected = 12; }  	// Short Readout

			if ( (buf0[1]&0x0003) == 1) { 					// Full Readout
			ALCT_WordsExpected = ((buf0[1]&0x007c) >> 2) *
			( (buf0[3]&0x0001)+((buf0[3]&0x0002)>>1)+
			  ((buf0[3]&0x0004)>>2)+((buf0[3]&0x0008)>>3)+
			  ((buf0[3]&0x0010)>>4)+((buf0[3]&0x0020)>>5)+
			  ((buf0[3]&0x0040)>>6) ) * 12 + 12;
			}

		if (debug_printout) cout << " <A";
//KKotov
// For ALCT CRC check:
		alct_data = largeBuf_pointer-12;
//KK
		}


		// == TMB Header found right after DMB Header or right after ALCT Trailer
		if( ( ((buf0[0]&0xFFFF) ==0x6B0C) &&
		      ((buf_1[0]&0xF000)==0xA000) &&
		      ((buf_1[1]&0xF000)==0xA000) &&
		      ((buf_1[2]&0xF000)==0xA000) &&
		      ((buf_1[3]&0xF000)==0xA000) )
                              ||
		    ( ((buf0[0]&0xFFFF) ==0x6B0C) &&
		      ((buf_1[0]&0x0800)==0x0000) &&  	// should've been ((buf_1[0]&0xF800)==0xD000)
		      					// see comments for sERROR[14]
		      ((buf_1[1]&0xF800)==0xD000) &&
		      ((buf_1[2]&0xFFFF)==0xDE0D) && (
				( !ddu2004 && (buf_1[3]&0xFC00)==0xD000) ||
				(ddu2004 && (buf_1[3]&0xF000)==0xD000)
				  ) )   )   {
                    fControlWord=5;
                    fTMB_Header=true;
                    TMB_WordsSinceLastHeader = 4;

                    // Calculate expected number of TMB words (whether RPC included will be known later)
                            if ( (buf0[1]&0x3000) == 0x3000) { TMB_WordsExpected = 8; }   // Short Header Only
                            if ( (buf0[1]&0x3000) == 0x0000) { TMB_WordsExpected = 32; }  // Long Header Only

                            if ( (buf0[1]&0x3000) == 0x1000) {
                            // Full Readout   = 28 + (#Tbins * #CFEBs * 6)
                            TMB_Tbins=(buf0[1]&0x001F);
                            TMB_WordsExpected = 28 + TMB_Tbins * ((buf1[0]&0x00E0)>>5) * 6;
                            }

                    if (debug_printout) cout << " <T";
    //KKotov
    // For TMB CRC check:
                    tmb_data = largeBuf_pointer-12;
    //KK
		}

		// == ALCT Trailer found
		if ( ((buf0[0]&0x0800)==0x0000) &&	// should've been ((buf0[0]&0xF800)==0xD000)
		      					// see comments for sERROR[14]
		     ((buf0[1]&0xF800)==0xD000) &&
		     ((buf0[2]&0xFFFF)==0xDE0D) && (
			 ( !ddu2004 && (buf0[3]&0xFC00)==0xD000) ||
			 (ddu2004 && (buf0[3]&0xF000)==0xD000)
				 ) )   {
                    fControlWord = 4;
                    if(!fALCT_Header) { fERROR[7]=true; }  // ALCT Header is missing
                    if( (buf0[0]&0xF800) != 0xD000 ) { fERROR[17]=true; }
                                                        // some bits in 1st D-Trailer are lost
    //KKotov
    // ALCT CRC check:
                    if( crcALCT ){
                            int i, j, t;
                            unsigned long lcrc = 0, ncrc;
                            unsigned short w;
                            int length = ( alct_data ? largeBuf_pointer - alct_data - 12 : 0 );
                            for (j = 0; j < length; j++){
                                    w = alct_data[j] & 0x7fff;
                                    for (i = 15; i >= 0; i--) {
                                            t = ((w >> i) & 1) ^ ((lcrc >> 21) & 1);
                                            ncrc = (lcrc << 1) & 0x3ffffc;
                                            ncrc |= (t ^ (lcrc & 1)) << 1;
                                            ncrc |= t;
                                            lcrc = ncrc;
                                    }
                            }
                            unsigned long crc = buf0[0] & 0x7ff;
                            crc |= ((unsigned long)(buf0[1] & 0x7ff)) << 11;
                            if( lcrc != crc ) fERROR[9]=true;
                    }
    //KK
                    fALCT_Header = false;
                    ALCT_WordCount = (buf0[3]&0x03FF);
                    CFEB_SampleWordCount = 0;
                    if (debug_printout) cout << "A> ";
		}



		// == Find Correction for TMB_WordsExpected,
		//    should it turn out to be the new RPC-aware format
		if( fTMB_Header && ((buf0[2]&0xFFFF)==0x6E0B) )  {
			TMB_WordsExpectedCorrection =  2 +   // header/trailer for block of RPC raw hits
		           ((buf_1[2]&0x0800)>>11) * ((buf_1[2]&0x0700)>>8) * TMB_Tbins * 2;  // RPC raw hits
		}




		// == TMB Trailer found
		if ( ((buf0[0]&0xF000)==0xD000) &&
		     ((buf0[1]&0xF000)==0xD000) &&
		     ((buf0[2]&0xFFFF)==0xDE0F) &&
		     ((buf0[3]&0xF000)==0xD000) )   {
		fControlWord = 6;
		if(!fTMB_Header) { fERROR[11]=true; }  // TMB Header is missing

//KKotov
// TMB CRC check:
		if( crcTMB ){
			int i, j, t;
			unsigned long lcrc = 0, ncrc;
			unsigned short w;
			int length = ( tmb_data ? largeBuf_pointer - tmb_data - 12 : 0 );
			for (j = 0; j < length; j++){
				w = tmb_data[j] & 0x7fff;
				for (i = 15; i >= 0; i--) {
					t = ((w >> i) & 1) ^ ((lcrc >> 21) & 1);
					ncrc = (lcrc << 1) & 0x3ffffc;
					ncrc |= (t ^ (lcrc & 1)) << 1;
					ncrc |= t;
					lcrc = ncrc;
				}
			}
			unsigned long crc = buf0[0] & 0x7ff;
			crc |= ((unsigned long)(buf0[1] & 0x7ff)) << 11;
			if( lcrc != crc ) fERROR[13]=true;
		}
//KK

		fTMB_Header = false;
		TMB_WordCount = (buf0[3]&0x07FF);

			// == Correct TMB_WordsExpected
			//	1) for 2 optional 0x2AAA and 0x5555 Words in the Trailer
			//    	2) for extra 4 frames in the new RPC-aware format and
			//         for RPC raw hit data, if present
			if( buf_1[1]==0x6E0C ) {
				TMB_WordsExpected = TMB_WordsExpected + 2;	//
		    		if ( buf_1[0]==0x6E04 )   {
		    		TMB_WordsExpected = TMB_WordsExpected + 4 + TMB_WordsExpectedCorrection;
				}
			}

			if( (buf_1[3]==0x6E0C) && (buf_1[2]==0x6E04)  ) {
		    		TMB_WordsExpected = TMB_WordsExpected + 4 + TMB_WordsExpectedCorrection;
			}

		CFEB_SampleWordCount = 0;
		if (debug_printout) cout << "T> ";
		}


		// == CFEB Sample Trailer found
		if ( ((buf0[1]&0xF000)==0x7000) &&
		     ((buf0[2]&0xF000)==0x7000) &&
		     ((buf0[3]&0xFFFF)==0x7FFF) )   {
			fControlWord = 7;

			if ( (CFEB_SampleCount%8) == 0 )   { if (debug_printout) cout << " <";}
			if ( CFEB_SampleWordCount == 100 ) { if (debug_printout) cout <<  "+";}
			if ( CFEB_SampleWordCount != 100 ) { if (debug_printout) cout <<  "-";  fERROR[14]=true; }

			CFEB_SampleCount++;

			if ( (CFEB_SampleCount%8) == 0 ) {
			if (debug_printout) cout << ">";
			CFEB_BSampleCount=0;
			}

//KKotov
// CFEB CRC check:
			if( crcFFEB ){
				if( CFEB_SampleWordCount == 100 ){
					unsigned short int crc15r=buf0[0], crc15c=0x0000;
					unsigned short *cfeb_data = largeBuf_pointer-100-8;
					for(int index=0; index<96; index++)
						crc15c=(cfeb_data[index]&0x1fff)^((cfeb_data[index]&0x1fff)<<1)^(((crc15c&0x7ffc)>>2)|((0x0003&crc15c)<<13))^((crc15c&0x7ffc)>>1);
					if( crc15r!=crc15c ) fERROR[16]=true;
				}
			}
//KK
			CFEB_SampleWordCount=0;
		}


		// == CFEB B-word found
		if ( ((buf0[0]&0xF000)==0xB000) &&
		     ((buf0[1]&0xF000)==0xB000) &&
		     ((buf0[2]&0xF000)==0xB000) &&
		     ((buf0[3]&0xF000)==0xB000) )   {
			fControlWord=8;
			fWARNING[1]=true;

			if ( (CFEB_SampleCount%8) == 0 )   { if (debug_printout) cout << " <";}
			if (debug_printout) cout << "B";

			CFEB_SampleCount++;
			CFEB_BSampleCount++;

			if ( (CFEB_SampleCount%8) == 0 ) {
			if (debug_printout) cout << ">";
			CFEB_BSampleCount=0;
			}

			CFEB_SampleWordCount=0;
		}


		// == DMB F-Trailer found
		if ( ((buf0[0]&0xF000)==0xF000) &&
		     ((buf0[1]&0xF000)==0xF000) &&
		     ((buf0[2]&0xF000)==0xF000) &&
		     ((buf0[3]&0xF000)==0xF000) )   {
		fControlWord=9;
		if(!fDMB_Header) { fERROR[5]=true; }	// DMB Header is missing
		fDMB_Header=false;

		// Print DMB F-Trailer marker
		if (debug_printout) cout << " }";
		}


		// == DMB F-Trailer found
		if ( ((buf0[0]&0xF000)==0xE000) &&
		     ((buf0[1]&0xF000)==0xE000) &&
		     ((buf0[2]&0xF000)==0xE000) &&
		     ((buf0[3]&0xF000)==0xE000) )   {
		fControlWord=10;
		fDMB_Header=false;

			if( (!fALCT_Header) &&
			    ( (ALCT_WordsSinceLastHeader != ALCT_WordCount) ||
                              (ALCT_WordsSinceLastHeader != ALCT_WordsExpected) ) ) {
			    fERROR[8]=true;	// ALCT Word Count Error
			}

			if( (!fTMB_Header) &&
			    ( (TMB_WordsSinceLastHeader != TMB_WordCount) ||
                              (TMB_WordsSinceLastHeader != TMB_WordsExpected) ) ) {
			    fERROR[12]=true;	// TMB Word Count Error
			}

			if ( (CFEB_SampleCount%8) != 0 ) { fERROR[15]=true; }  // Number of CFEB samples != 8*n


			if(fALCT_Header) {
			fERROR[6]=true;		// ALCT Trailer is missing
			ALCT_WordsSinceLastHeader=0;
			ALCT_WordsExpected = 0;
			}

			if(fTMB_Header) {
			fERROR[10]=true;	// TMB Trailer is missing
			TMB_WordsSinceLastHeader=0;
			TMB_WordsExpected = 0;
			}


                if (debug_printout) {
		// Print DMB E-Trailer marker
                    cout << "; "
                        << ALCT_WordsSinceLastHeader << "-"
                        << ALCT_WordCount << "-"
                        << ALCT_WordsExpected
                        << "      "
                        << TMB_WordsSinceLastHeader << "-"
                        << TMB_WordCount << "-"
                        << TMB_WordsExpected
                        << endl;
                    }
                }

		// == DDU Trailer found
		if ( (buf0[0]==0x8000) &&
		     (buf0[1]==0x8000) &&
		     (buf0[2]==0xFFFF) &&
		     (buf0[3]==0x8000) ) {
			fControlWord=11;
			if(fDDU_Trailer) { fERROR[1]=true; } // DDU Header is missing
			fDDU_Trailer=true;
			fDDU_Header=false;
			cntDDU_Trailers++; // Increment DDUTrailer counter

			// == Combining 2 words into 24bit value
			DDU_WordCount = buf2[2] | ((buf2[3] & 0xFF) <<16) ;
			// == Filling Histo
			// h["hDDUWordCount"]->Fill(DDU_WordCount);

			if ( (DDU_WordsSinceLastHeader+4) != DDU_WordCount ) { fERROR[3]=true; }

                        if (debug_printout) {
                            cout << "DDU Trailer Occurrence " << cntDDU_Trailers << endl;
                            cout << "----------------------------------------------------------" << endl;
                            cout << "DDU 64-bit words = Actual - DDUcounted =" << DDU_WordsSinceLastHeader+4
			     << "-" << DDU_WordCount << endl;
                        }


                        // increment statistics Errors and Warnings (i=0 case is handled in DDU Header)
                        for (int i=0; i<sizeof(fERROR); i++) {
                            if (fERROR[i]) {
                                h["hBinCheck_Errors"]->Fill(0.,i);
                                cntERROR[i]++;
                                fBadEvent=true;
                                if (debug_printout) {
                                    cerr << endl << "DDU Header Occurrence = " << cntDDU_Headers;
                                    cerr << "  ERROR " << i << "  " << sERROR[i] << endl;
                                }
                            }
                        }

                        for (int i=0; i<sizeof(fWARNING); i++) {
                            if (fWARNING[i]) {
                                h["hBinCheck_Warnings"]->Fill(0.,i);
                                cntWARNING[i]++;
                                if (debug_printout) {
                                    cout << endl << "DDU Header Occurrence = " << cntDDU_Headers;
                                    cout << "  WARNING " << i << "  " << sWARNING[i] << endl;
                                }
                            }
                        }


			 if(fBadEvent) { cntBadEvent++; }


 			DDU_WordsSinceLastHeader=0;
			DDU_WordsSinceLastTrailer=0;
		}
                ptr+=8;
	}
        unsigned int res=0;
        for (int i=0; i<sizeof(fERROR); i++) {
           if (fERROR[i]) {
                res|=1<<i;
           }
        }
        debug_printout=save_debug;
        return res;

}

