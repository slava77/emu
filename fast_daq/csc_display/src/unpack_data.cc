//
#include<iostream>
#include<string.h>
#include<stdio.h>

#define MIN_TDC_TO_DISPLAY 0
#define MAX_TDC_TO_DISPLAY 3000
#define MIN_SCINT_TDC_TO_DISPLAY 0
#define MAX_SCINT_TDC_TO_DISPLAY 1600

#include "j_common_data.h"

extern "C"
{
#include "daq_conf.h"
#include "csc_event.h"
}

/// -- CMSSW Unpacker --
#include "csc_unpacker.h"

using namespace std;
int selectedChamberID = 0;
bool HACK_ME11_2DMB = 0;

void printBin(char c)
{
  for (int i = 7; i >= 0; --i)
    {
      putchar( (c & (1 << i)) ? '1' : '0' );
    }
  putchar('\n');
}

extern "C" void unpack_data_cc()
{

  /**** Semi-hack ****/
  //// ME1/1 chambers have opposite ordering for the A and B
  //// ends, thus the CLCT key halfstip needs to be inverted
  //// in one end or the other.
  bool invert_CLCT_for_ME11A = 1;
  bool invert_CLCT_for_ME11B = 0;
  
  int ibucket, ihalf_strip, half_strip_index, distrip, idistrip, itime;

  // What is this?
  bool alct_pattern[5][NLAYER] = { {1, 0, 0, 0, 0, 0},
				   {1, 1, 0, 0, 0, 0},
				   {1, 1, 1, 1, 1, 1},
				   {0, 0, 0, 1, 1, 1},
				   {0, 0, 0, 0, 1, 1} };
  
  int i, j, istrip, kstrip, ilayer, ihit, isample, ksample, ilct, iwire, imax, ifirst, ilast;
  float sca_sum, peak_strip_tmp;
  float sca_temp, sca_max;

  int sci_end[4]  = {2, 1, 2, 1};
  int sci_layer[4]  = {2, 2, 1, 1};
  j_data.nsamples = upevt_.nsca_sample;

  //zero arrays
  for (ilayer=0; ilayer<NLAYER; ilayer++)
    {
      j_data.peak_strip[ilayer] = 0;
      for (isample=0; isample<j_data.nsamples; isample++)
        {
          for (istrip=0; istrip<5; istrip++)
            {
              j_data.sample[istrip][ilayer][isample] = 0;
            }
        }
    }
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (istrip=1; istrip<=NSTRIP; istrip++)
        {
          j_data.strips[istrip-1][ilayer-1] = 0.0;
          j_data.strips_peaks[istrip-1][ilayer-1] = 0.0;
        }
    }
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (ihit=1; ihit<=NWIRE; ihit++)
        {
          j_data.wires[ihit-1][ilayer-1] = 0;
        }
    }
  for (ilayer=1; ilayer<=2; ilayer++)
    {
      for (j=1; j<=NSCINT; j++)
        {
          j_data.scint[0][j-1][ilayer-1] = 0;
          j_data.scint[1][j-1][ilayer-1] = 0;
        }
    }

  //fill strips array with new data
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      peak_strip_tmp = 0.0;
      for (istrip=1; istrip<=NSTRIP; istrip++)
        {
          imax = 1;

	  // Pedestal subtraction: subtract off samples (0+1)/2.0 as
	  // our pedestal before searching for the maximum sampling
	  
	  // compute the 'pedestal'
	  float temp_ped = (0.5*upevt_.sca[ilayer-1][istrip-1][0]+0.5*upevt_.sca[ilayer-1][istrip-1][1]);
        
          for (isample=1; isample<=j_data.nsamples; isample++)
            {

	      //  then do the pedestal subtraction
	      upevt_.sca[ilayer-1][istrip-1][isample-1] -= temp_ped;
                       
	      // zero out values > max
              if (upevt_.sca[ilayer-1][istrip-1][isample-1] > upevt_.nsca_sample*4096)  upevt_.sca[ilayer-1][istrip-1][isample-1] = 0;//-(upevt_.sca[ilayer-1][istrip-1][isample-1]-65536);

	      // find peak strip
              if (upevt_.sca[ilayer-1][istrip-1][isample-1]> upevt_.sca[ilayer-1][istrip-1][imax-1]) imax = isample;
	      
            }
          if ((imax-2)<1) ifirst = 1;
          else ifirst = imax - 2;

          if ((imax+2)>j_data.nsamples) ilast = j_data.nsamples;
	  else ilast = imax+2;
            
          sca_sum = 0.0;
          sca_max = upevt_.sca[0][0][0]; // firman
          for (ksample=ifirst; ksample<=ilast; ksample++)
            {
              sca_temp = float(upevt_.sca[ilayer-1][istrip-1][ksample-1]); // firman
              sca_sum += sca_temp; // firman
              if (sca_temp > sca_max) sca_max = sca_temp; // firman
            }
            
          j_data.strips[istrip-1][ilayer-1] = sca_sum;
          j_data.strips_peaks[istrip-1][ilayer-1] = sca_max; // firman

          if (j_data.strips[istrip-1][ilayer-1] > peak_strip_tmp)
            {
              peak_strip_tmp = j_data.strips[istrip-1][ilayer-1];
              j_data.peak_strip[ilayer-1] = istrip;
            }
	} // istrip
    } // ilayer
    
  //fill SCA sample array
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      if ((j_data.peak_strip[ilayer-1]-2) < 1)
        {
          ifirst = 1;
        }
      else
        {
          ifirst = j_data.peak_strip[ilayer-1] - 2;
        }
      if ((j_data.peak_strip[ilayer-1]+2) > NSTRIP)
        {
          ilast = NSTRIP;
        }
      else
        {
          ilast = j_data.peak_strip[ilayer-1] + 2;
        }

      for (istrip=ifirst; istrip<=ilast; istrip++)
        {
          kstrip = istrip - j_data.peak_strip[ilayer-1] + 3;
          // kstrip = istrip - 80 + 3; // firman
          for (isample=1; isample<=j_data.nsamples; isample++)
            {
              j_data.sample[kstrip-1][ilayer-1][isample-1] = upevt_.sca[ilayer-1][istrip-1][isample-1] + 50;
            }
        }
    }
  
  //fill wire array with new data
  for (ihit=1; ihit<=upevt_.num_wire_hits; ihit++)
    {
      if ((upevt_.wire_times_lead[ihit-1]>=MIN_TDC_TO_DISPLAY)&&(upevt_.wire_times_lead[ihit-1]<=MAX_TDC_TO_DISPLAY)&&(upevt_.wire_times_lead[ihit-1]>j_data.wires[upevt_.wire_groups[ihit-1]-1][upevt_.wire_layers[ihit-1]-1]))
        {
          j_data.wires[upevt_.wire_groups[ihit-1]-1][upevt_.wire_layers[ihit-1]-1] = upevt_.wire_times_lead[ihit-1];
        }
    }

  //fill scint array with new data
  for (ihit=1; ihit<=upevt_.num_scint_hits; ihit++)
    {
      i = (upevt_.scint_labels[ihit-1])&(0xff);
      j = upevt_.scint_labels[ihit-1]>>8;
      j_data.scint[sci_end[j-1]-1][i-1][sci_layer[j-1]-1] = upevt_.scint_times[ihit-1];
    }

  //get alct info
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (iwire=1; iwire<=NWIRE; iwire++)
        {
          j_data.wiregroup[iwire-1][ilayer-1] = false;
          j_data.alct[0][iwire-1][ilayer-1] = false;
          j_data.alct[1][iwire-1][ilayer-1] = false;
        }
    }

  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (iwire=1; iwire<=NWIRE; iwire++)
        {
          if (upevt_.alct_dump[ilayer-1][iwire-1] != 0)
            {
              j_data.wiregroup[iwire-1][ilayer-1] = true;
            }
        }
    }


  //key wire group layer 3
  for (ilct=1; ilct<=2; ilct++)
    {
      if (upevt_.alct_valid_patt[ilct-1]==1)
        {
          iwire = upevt_.alct_wire_group[ilct-1] + 1;
          j_data.alct_q[ilct-1] = upevt_.alct_patt_quality[ilct-1];
          if (upevt_.alct_accel_muon[ilct-1]==1)
            {
              for (ilayer=1; ilayer<=NLAYER; ilayer++)
                {
                  j_data.alct[ilct-1][iwire-1][ilayer-1] = true;
                }
            }
          else
            {
              for (ilayer=1; ilayer<=NLAYER; ilayer++)
                {
                  for (j=iwire-2; j<=iwire+2; j++)
                    {
                      if (alct_pattern[j+3-iwire-1][ilayer-1]&&j>=1&&j<=NWIRE)
                        {
                          j_data.alct[ilct-1][j-1][ilayer-1] = true;
                        }
                    }
                }
            }
        }
    }


  //get comparator halfstrip info
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (istrip=1; istrip<=NSTRIP; istrip++)
        {
          for (ihalf_strip=0; ihalf_strip<=1; ihalf_strip++)
            {
	      // zero arrays
              j_data.halfstrip[ihalf_strip][istrip-1][ilayer-1] = false;
              j_data.clct[ihalf_strip][istrip-1][ilayer-1] = false;
            }
        }
    }


  // CLCT key halfstrip layer 3
  for (ilct=1; ilct<=2; ilct++)
    {
      if (upevt_.clct_valid_patt[ilct-1]==1)
        {
          ihalf_strip = upevt_.clct_half_strip[ilct-1]%2;
	  istrip = upevt_.clct_half_strip[ilct-1]/2 + 1;

	  if(NCFEB==7) // Use NCFEB==7 to determine that this is an ME1/1 chamber
	    {
	      if(invert_CLCT_for_ME11A && istrip > 4*NCFEB_STRIP)
		{
		  istrip = 1 + 4*NCFEB_STRIP + 3*NCFEB_STRIP - (istrip - 4*NCFEB_STRIP);
		  ihalf_strip = !ihalf_strip;
		}
	      if(invert_CLCT_for_ME11B && istrip <= 4*NCFEB_STRIP)
		{
		  istrip = 1 + 4*NCFEB_STRIP - istrip;
		  ihalf_strip = !ihalf_strip;
		}
	    }
	  
	  j_data.clct[ihalf_strip][istrip-1][2] = true; //changed
	  //std::cout<<"Key Halfstrip: j_data.clct["<<ihalf_strip<<"]["<<istrip<<"-1][2] = true"<<std::endl;
        }
      else
	{
	  //std::cout<<"Key Halfstrip: No valid pattern"<<std::endl;
	}
    }

  //calculate halfstrip from CFEB data
  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (istrip=1; istrip<=NSTRIP; istrip++)
        {
	  if( upevt_.clct_dump_halfstrips[ilayer-1][istrip-1][0] ) j_data.halfstrip[0][istrip-1][ilayer-1] = true;
	  else if( upevt_.clct_dump_halfstrips[ilayer-1][istrip-1][1] ) j_data.halfstrip[1][istrip-1][ilayer-1] = true;
        }
    }
  
  return;
}

template<typename T> inline CSCCFEBDataWord const * const timeSample( T const & data, int nCFEB,int nSample,int nLayer, int nStrip)
{
  return data.cfebData(nCFEB)->timeSlice(nSample)->timeSample(nLayer,nStrip);
}

template<typename T> inline CSCCFEBTimeSlice const * const timeSlice( T const & data, int nCFEB, int nSample)
{
  return (CSCCFEBTimeSlice *)(data.cfebData(nCFEB)->timeSlice(nSample));
}

int check_file_binary(CSCDCCExaminer* bin_checker, const char *buf, int32_t evt_size)
{
  ///** Create and configure binary buffer checker object
  bin_checker->output1().hide();
  bin_checker->output2().hide();
  bin_checker->crcALCT(true);
  bin_checker->crcTMB (true);
  bin_checker->crcCFEB(true);
  bin_checker->modeDDU(true);

  uint32_t binCheckMask = 0x16EBF7F6;
  uint32_t dduBinCheckMask = 0x02080016;

  ///** Binary check of the buffer
  uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(buf);
  bin_checker->setMask(binCheckMask);
  if ( bin_checker->check(tmp,evt_size/sizeof(short)) < 0 )
    {
      ///**  No ddu trailer found - force checker to summarize errors by adding artificial trailer
      const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
      tmp = dduTrailer;
      bin_checker->check(tmp,uint32_t(4));
    }

  BinaryErrorStatus   = bin_checker->errors();
  BinaryWarningStatus = bin_checker->warnings();

  // This was commented in the past
  if (((BinaryErrorStatus & dduBinCheckMask)>0) || ((BinaryWarningStatus & 0x2) > 0) ){
    std::cerr << "Event is skipped because of format errors" << std::endl;
    return 4; /// Reject bad event data
  }
  
  return 0;
}

int getSelectedChamberID()
{
  return selectedChamberID;
}

void setSelectedChamberID(int id)
{
  selectedChamberID = id;
  printf("Set selected chamber ID: %d\n", selectedChamberID); // firman
}

int getHackMode()
{
  return HACK_ME11_2DMB;
}

void setHackMode(bool mode)
{
  HACK_ME11_2DMB = mode;
}

int get_chambers_data(std::vector<CSCEventData> *chambersData, const char *buf, int32_t evt_size)
{
  int rtval;
  
  // chambers data
  chambersData->clear();
  
  // check the binary of the file first
  CSCDCCExaminer bin_checker;
  rtval = check_file_binary(&bin_checker, buf, evt_size);
  
  // if the binary check is error, return its error code
  if (rtval > 0) return rtval;
  
  // unpack chambers data
  CSCDDUEventData dduData((uint16_t *) buf, &bin_checker);
  (*chambersData) = dduData.cscData();
  
  // set the selectedChambersID variable to the last chamber ID
  int lastIdx = chambersData->size() - 1;
  CSCEventData data = (*chambersData)[lastIdx];
  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  int crateID = dmbHeader->crateID();
  int dmbID = dmbHeader->dmbID();
  int chamberID = (((crateID) << 4) + dmbID) & 0xFFF;
  setSelectedChamberID(chamberID);
  return 0;
}

//*** Unpack data using CMSSW ***//
int get_next_event_cmssw(const char *buf, int32_t evt_size, int32_t first_time)
{
  int rtval;
  
  // check the binary of the file first
  CSCDCCExaminer bin_checker;
  rtval = check_file_binary(&bin_checker, buf, evt_size);
  
  // if binary checker is error, then return its error code
  if (rtval == 4) return 4;

  ///** Reset legacy event data structure
  memset(&(upevt_), 0x0, sizeof(csc_event_type));

  ///** Unpack top level
  CSCDDUEventData dduData((uint16_t *) buf, &bin_checker);

  ///** Unpack all founded CSC
  std::vector<CSCEventData> chamberDatas;
  chamberDatas.clear();
  chamberDatas = dduData.cscData();

  int nCSCs = chamberDatas.size();
  //nCSCs = 1;

  std::cout << "Found " << nCSCs << " chamber(s) in event" << std::endl;

  /***************************************************************

  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING

  I am adding a super hack to make the precision strip data for 
  strips 1-32 from the first CSC show up as strips 81-112 of the
  second CSC.  This is just a quick and dirty way to make the
  OTMB + 2DMB setup at B904 looks good.
  The modified code is indicated by the comment "2DMB HACK"
  ~Joe

   ***************************************************************/
  // const bool HACK_ME11_2DMB = 0; // flag to turn on/off B904 2DMB hack

  for (int ic=0; ic< nCSCs; ic++)
    {
      
      /// process_chamber_data(chamberDatas[i]);
      CSCEventData& data = chamberDatas[ic];
      
      //tmb_bxn=data.tmbHeader()->BXNCount();
      //tmb_l1a_num=data.tmbHeader()->L1ANumber();

      //    DMB Found
      //    Unpacking of DMB Header and trailer
      const CSCDMBHeader* dmbHeader = data.dmbHeader();
      const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();

      ///** Unpacking of Chamber Identification number
      int crateID   = 0xFF;
      int dmbID     = 0xF;
      int ChamberID = 0xFFF;


      crateID       = dmbHeader->crateID();
      dmbID         = dmbHeader->dmbID();
      ChamberID     = (((crateID) << 4) + dmbID) & 0xFFF;
      if ((ChamberID != selectedChamberID) && (selectedChamberID != 0) && ic != 0) continue;
      printf("-> Selected chamber ID: %d with ic=%d\n", selectedChamberID, ic); // firman

      std::cout<<"== Read data for: =="<<endl;
      cout<<"crateID:"<<crateID<<endl;
      cout<<"dmbID:"<<dmbID<<endl;
      cout<<"ChamberID:"<<ChamberID<<endl;

      upevt_.chamber_type_id = 0;
      if(crateID==10 && dmbID<=3){
	upevt_.chamber_type_id = 11; // ME11
      }



      int iendcap = -1;
      int istation = -1;
      // TODO: Add actual Map conversion

      CSCReadoutMappingFromFile cscMapping;

      //uint32_t id = cscMapping.chamber(iendcap, istation, crateID, dmbID, -1);
      uint32_t id=604021272; //for chamber1 types
      uint32_t binCheckMask = 0x16EBF7F6;
      //id=604029960;
      CSCDetId cid( id );

      unsigned long errors = bin_checker.errorsForChamber(ChamberID);
      if ((errors & binCheckMask) > 0 )
        {
          std::cerr << "Skipped CSC_" << crateID << "_" << dmbID << " because of format errors" << std::endl;
          continue;
        }

      ///** Get FEBs Data Available Info
      int alct_dav  = dmbHeader->nalct();
      int tmb_dav   = dmbHeader->nclct();
      int cfeb_dav2 = 0;
      for (int i=0; i<5; i++) cfeb_dav2 = cfeb_dav2 + (int)((dmbHeader->cfebAvailable()>>i) & 0x1);

      ///** ALCT Found
      if (data.nalct())
	{
	  upevt_.alct_active = 1;
	  const     CSCALCTHeader*  alctHeader      = data.alctHeader();
	  int                       fwVersion       = alctHeader->alctFirmwareVersion();
	  const     CSCALCTTrailer* alctTrailer     = data.alctTrailer();
	  const     CSCAnodeData*   alctData        = data.alctData();

	  if (alctHeader && alctTrailer)
	    {
	    
	      upevt_.alct_nbucket = alctHeader->NTBins();
	      upevt_.alct_l1a_num = alctHeader->L1Acc();
	      upevt_.alct_full_bxn = alctHeader->BXNCount();
	      //cout<<"ALCT nbins"<<alctHeader->NTBins()<<endl;
	      ///** Process ALCTs
	      vector<CSCALCTDigi> alctsDatasTmp = alctHeader->ALCTDigis();
	      vector<CSCALCTDigi> alctsDatas;

	      for (uint32_t lct=0; lct<alctsDatasTmp.size(); lct++)
		{
		  if (alctsDatasTmp[lct].isValid())
		    alctsDatas.push_back(alctsDatasTmp[lct]);

		}
	      
	      for (uint32_t lct=0; lct<alctsDatas.size(); lct++)
		{
		  // TODO: Add support for more than 2 ALCTs
		  if (lct>=2) continue;
		  upevt_.alct_valid_patt[lct]   = (alctsDatas[lct].isValid())?1:0;
		  upevt_.alct_patt_quality[lct] = alctsDatas[lct].getQuality();
		  upevt_.alct_accel_muon[lct]   = alctsDatas[lct].getAccelerator();
		  upevt_.alct_wire_group[lct]   = alctsDatas[lct].getKeyWG();
		  upevt_.alct_patb[lct]         = alctsDatas[lct].getCollisionB();

		  // if ME13, 31, 41, correct wire group
		  // if (correct_wg) upevt_.alct_wire_group[lct] -= 16;
		  // bxn for an lct is calculated as full_bxn + number of lct bin
		  // should take remainder from division of this number by number of LHC clocks per orbit
		  int alct_bxn = alctsDatas[lct].getBX();
		  if (fwVersion == 2007) alct_bxn = (alct_bxn + alctHeader->BXNCount())&0x1F;
		  upevt_.alct_bxn[lct] = alct_bxn;
		}


	      ///** Process anode raw data
	      for (int nLayer=1; nLayer<=6; nLayer++)
		{
		  vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
		  for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin();
		       wireDigisItr != wireDigis.end(); ++wireDigisItr)
		    {
		      int wg = wireDigisItr->getWireGroup();

		      vector<int> tbins = wireDigisItr->getTimeBinsOn();
		      int tbin = wireDigisItr->getTimeBin();
		      for (uint32_t n=0; n < tbins.size(); n++)
			{
			  tbin = tbins[n];
			  upevt_.alct_dump[nLayer-1][wg-1] |= 1<<tbin;
			  //std::cout<<"layer/wg : "<<nLayer-1<<"/"<<wg-1<<std::endl;
			}

		    }

		}
	    }
	  else
	    {
	      cout<<"ALCT trailer/header "<<alctTrailer <<"/"<<alctHeader <<endl;
	    }
	}
      else
	{
	  cout<<"Chamber "<<ChamberID<<": NO ALCT found"<<endl;
	}


      //cout<<" i get to clct stuff"<<endl;
      ///** CLCT Found
      if (data.nclct())
        {
          upevt_.clct_active = 1;
          const CSCTMBHeader* tmbHeader = data.tmbHeader();
          int                 tmbfwVersion = tmbHeader->FirmwareVersion();
          const CSCTMBTrailer* tmbTrailer=data.tmbData()->tmbTrailer();

          if (tmbHeader && tmbTrailer){
	    upevt_.clct_nbucket = tmbHeader->NTBins();
	    //cout<<"tmb header NTbins "<<tmbHeader->NTBins()<<endl;
	    
	    //** Process CLCTs
	    vector<CSCCLCTDigi> clctsDataTmp= data.tmbHeader()->CLCTDigis(cid.rawId());
	    vector<CSCCLCTDigi> clctsData;
	    
	    for (uint32_t i_clct=0; i_clct<clctsDataTmp.size();i_clct++){
	      if(clctsDataTmp[i_clct].isValid()){
		clctsData.push_back(clctsDataTmp[i_clct]);
	      }
	    }
	    
	    //std::cout<<"CLCT Data: Size: "<<clctsDataTmp.size()<<","<<clctsData.size()<<std::endl;

	    for (uint32_t clct=0; clct<clctsData.size(); clct++)
	      {
		// TODO: Add support for more than 2 CLCTs
		if (clct>=2) continue;
		upevt_.clct_valid_patt[clct]   = (clctsData[clct].isValid())?1:0;
		upevt_.clct_patt_number[clct] = clctsData[clct].getPattern();
		upevt_.clct_bend[clct]   = clctsData[clct].getBend();
		upevt_.clct_half_strip[clct]   = clctsData[clct].getKeyStrip();
		
		//std::cout<<"CLCT Data: Valid pattern: "<<clctsData[clct].isValid()<<std::endl;
		//std::cout<<"CLCT Data: Patt Number: "<< clctsData[clct].getPattern()<<std::endl;
		//std::cout<<"CLCT Data: Bend: "<<clctsData[clct].getBend()<<std::endl;
		//std::cout<<"CLCT Data: Halfstrip: "<<clctsData[clct].getKeyStrip()<<std::endl;
		//std::cout<<"CLCT Data: CFEB: "<<clctsData[clct].getCFEB()<<std::endl;

		// take rather clctsData[clct].getBX()?
		
		upevt_.clct_bxn[clct] = clctsData[clct].getFullBX();
	      }
	    
	  }else{
	    cout<<"tmb header and trailer not ok"<<endl;
	  }
	  
	  // nclct>0 checked before

	  ///** Process cathode raw data - layers count from 1
	  for (int nLayer=1; nLayer<=6; nLayer++)
	    {
	      vector<CSCComparatorDigi> comparatorDigis = data.clctData()->comparatorDigis(nLayer);
	      
	      for(vector<CSCComparatorDigi>::iterator compDigisItr= comparatorDigis.begin();compDigisItr != comparatorDigis.end();++compDigisItr)
		{
		  
		  int strip=compDigisItr->getStrip();
		  vector<int> tbins = compDigisItr->getTimeBinsOn();
		  int tbin=compDigisItr->getTimeBin();
		  for(uint32_t n=0; n < tbins.size(); n++)
		    {
		      tbin = tbins[n];
		      upevt_.clct_dump_strips[nLayer-1][strip-1] |= 1<<tbin;
		    }

		  upevt_.clct_dump_halfstrips[nLayer-1][strip-1][compDigisItr->getComparator()] = 1;

		  //cout<<"Layer "<<nLayer<<":"<<endl;
		  //cout<<"Comparator Data: Strip: " << compDigisItr->getStrip()<<endl;
		  //cout<<"Comparator Data: Comparator: " << compDigisItr->getComparator()<<endl;
		  //cout<<"Comparator Data: TimeBin: " << compDigisItr->getTimeBin()<<endl;
		  //cout<<"Comparator Data: TimeWord: "<< compDigisItr->getTimeBinWord()<<endl;
		  //cout<<"Comparator Data: TimeBinsOn: ";
		  //for(uint32_t n=0; n < tbins.size(); n++) cout << tbins[n] <<",";
		  //cout<<endl;

		}
	    }
	}//close check on number of clct's
      else
	{
	  cout<<"Chamber "<<ChamberID<<": NO CLCT found"<<endl;
	}

      
      ///** CFEBs Found
      //int N_CFEBs=5;
      int N_CFEBs=5;
      int N_Samples=16, N_Layers = 6, N_Strips = 16;
      int TrigTime, SCA_BLK, NmbTimeSamples, ADC, OutOfRange;
      
      upevt_.sca_clock_phase = 0;
      for (int nCFEB = 0; nCFEB < N_CFEBs; ++nCFEB)
        {
          if (data.cfebData(nCFEB) !=0)
            {
              NmbTimeSamples= (data.cfebData(nCFEB))->nTimeSamples();
              upevt_.nsca_sample = NmbTimeSamples;

              if (dmbHeader->cfebAvailable() & (0x1 << nCFEB))    /* Is this CFEB present? */
                {
                  upevt_.active_febs[nCFEB] = 1;
                }

              for (int nLayer = 1; nLayer <= N_Layers; ++nLayer)
                {
                  for (int nSample = 0; nSample < NmbTimeSamples; ++nSample)
                    {
                      if (timeSlice(data, nCFEB, nSample) == 0)
                        {
                          //std::cerr <<  "CFEB" << nCFEB << " nSample: " << nSample << " - B-Word" << std::endl;
                          continue;
                        }

                      SCA_BLK  = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).sca_blk);
                      TrigTime = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).trig_time);

                      for (int nStrip = 1; nStrip <= N_Strips; ++nStrip)
                        {
                          ADC = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcCounts) & 0xFFF);
                          OutOfRange = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcOverflow) & 0x1);

                          upevt_.adc_out_of_range[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = OutOfRange;
                          upevt_.sca[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = ADC;

                          upevt_.sca_block[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample]  = SCA_BLK;
                          upevt_.sca_trig_time[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = TrigTime;


			  // Start 2DMB HACK -->  (see comment about this hack above)
              // if the first chamber doesn't have any CLCT data, then the hack does not work
			  if(HACK_ME11_2DMB && ic==0 && nCFEB < 2){
			    upevt_.adc_out_of_range[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = OutOfRange;
			    upevt_.sca[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = ADC;
			    
			    upevt_.sca_block[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample]  = SCA_BLK;
			    upevt_.sca_trig_time[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = TrigTime;
			  }
			  // <-- End 2DMB HACK
                        }
                    }
                }
            }
          /// upevt_.sca_clock_phase |= ((temp[0][ifeb][0] >> 12) & 1) << nCFEB;
        }
      
    }
  

  return 0;
}

