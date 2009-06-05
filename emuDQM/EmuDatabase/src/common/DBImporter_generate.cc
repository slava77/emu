#include "DBImporter.h"

void DBImporter::generate() {

  cscdqm::MonitorObject* me = 0;

  worker.setSQL("begin DQM_UPDATE.RUN_REMOVE(:1); end;");
  worker.setInt(1, runNumber);
  worker.execSQL();

  worker.setSQL("insert into DQM_RUN (RUN_NUMBER) values (:1)");
  worker.setInt(1, runNumber);
  worker.execSQL();

  /** Get DDU Readout */

  if (cache.getEMU(cscdqm::h::EMU_ALL_DDUS_IN_READOUT, me)) {
  
    const TH1* h = me->getTH1();
    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      const uint32_t cnt = uint32_t(h->GetBinContent(i));
      if (cnt > 0) {
        worker.setSQL("insert into DQM_RUN_DDU (RDU_RUN_NUMBER, RDU_DDU_NUMBER, RDU_EVENTS) values (:1, :2, :3)");
        worker.setInt(1, runNumber);
        worker.setInt(2, i);
        worker.setInt(3, cnt);
        worker.execSQL();
      }
    }

  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_ALL_DDUS_IN_READOUT];
  }

  /** Get DDU trailer errors */

  if (cache.getEMU(cscdqm::h::EMU_ALL_DDUS_TRAILER_ERRORS, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = 3; j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          worker.setSQL("insert into DQM_RUN_DDU_TRAILER (RDT_RUN_NUMBER, RDT_DDU_NUMBER, RDT_DUT_BIT, RDT_VALUE) values (:1, :2, :3, :4)");
          worker.setInt(1, runNumber);
          worker.setInt(2, i);
          worker.setInt(3, j - 2);
          worker.setInt(4, cnt);
          worker.execSQL();
        }
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_ALL_DDUS_TRAILER_ERRORS];
  }

  /** Get DDU Format errors */

  if (cache.getEMU(cscdqm::h::EMU_ALL_DDUS_FORMAT_ERRORS, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = 3; j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, 2));
        if (cnt > 0) {
          worker.setSQL("insert into DQM_RUN_DDU_FORMAT (RDF_RUN_NUMBER, RDF_DDU_NUMBER, RDF_DUF_BIT, RDF_VALUE) values (:1, :2, :3, :4)");
          worker.setInt(1, runNumber);
          worker.setInt(2, i);
          worker.setInt(3, j - 2);
          worker.setInt(4, cnt);
          worker.execSQL();
        }
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_ALL_DDUS_FORMAT_ERRORS];
  }

  /** Get DDU Live Inputs */
  UIntUIntUIntUIntMap ddu_inputs;
  if (cache.getEMU(cscdqm::h::EMU_ALL_DDUS_LIVE_INPUTS, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
        uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          ddu_inputs[i][j][0] = cnt;
        }
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_ALL_DDUS_LIVE_INPUTS];
  }

  /** Get DDU With Data */
  getDDUInputData(cscdqm::h::EMU_ALL_DDUS_INPUTS_WITH_DATA, 1, ddu_inputs);

  /** Get DDU INPUT Errors */
  getDDUInputData(cscdqm::h::EMU_ALL_DDUS_INPUTS_ERRORS, 2, ddu_inputs);

  /** Get DDU Input Warnings */
  getDDUInputData(cscdqm::h::EMU_ALL_DDUS_INPUTS_WARNINGS, 3, ddu_inputs);

  /** Write DDU Input Data to DB */

  for (UIntUIntUIntUIntMap::const_iterator it0 = ddu_inputs.begin(); it0 != ddu_inputs.end(); it0++) {
    const unsigned int ddu = it0->first;
    for (UIntUIntUIntMap::const_iterator it1 = it0->second.begin(); it1 != it0->second.end(); it1++) {
      const unsigned int input = it1->first;
      unsigned int events = 0, data = 0, errors = 0, warnings = 0;
      for (UIntUIntMap::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
        switch (it2->first) {
          case 0:
            events = it2->second;
            break;
          case 1:
            data = it2->second;
            break;
          case 2:
            errors = it2->second;
            break;
          case 3:
            warnings = it2->second;
        }
      }
      worker.setSQL("insert into DQM_RUN_DDU_INPUT (RDI_RUN_NUMBER, RDI_DDU_NUMBER, RDI_DDU_INPUT, RDI_EVENTS, RDI_WITH_DATA, RDI_ERRORS, RDI_WARNINGS) values (:1, :2, :3, :4, :5, :6, :7)");
      worker.setInt(1, runNumber);
      worker.setInt(2, ddu);
      worker.setInt(3, input);
      worker.setInt(4, events);
      worker.setInt(5, data);
      worker.setInt(6, errors);
      worker.setInt(7, warnings);
      worker.execSQL();
    }
  }
    
  /** Get CSC Events */

  if (cache.getEMU(cscdqm::h::EMU_DMB_REPORTING, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          const unsigned int id = i * 100 + j;
          worker.setSQL("insert into DQM_RUN_CHAMBER (RCH_RUN_NUMBER, RCH_CHA_ID, RCH_EVENTS) values (:1, :2, :3)");
          worker.setInt(1, runNumber);
          worker.setInt(2, id);
          worker.setInt(3, cnt);
          worker.execSQL();
        }  
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_DMB_REPORTING];
  }

  /** Get CSC wo ALCT Errors */

  if (cache.getEMU(cscdqm::h::EMU_CSC_WO_ALCT, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          const unsigned int id = i * 100 + j;
          worker.setSQL("update DQM_RUN_CHAMBER set RCH_WO_ALCT = :1 where RCH_RUN_NUMBER = :2 and RCH_CHA_ID = :3");
          worker.setInt(1, cnt);
          worker.setInt(2, runNumber);
          worker.setInt(3, id);
          worker.execSQL();
        }  
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_DMB_REPORTING];
  }

  /** Get CSC wo CLCT Errors */

  if (cache.getEMU(cscdqm::h::EMU_CSC_WO_CLCT, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          const unsigned int id = i * 100 + j;
          worker.setSQL("update DQM_RUN_CHAMBER set RCH_WO_CLCT = :1 where RCH_RUN_NUMBER = :2 and RCH_CHA_ID = :3");
          worker.setInt(1, cnt);
          worker.setInt(2, runNumber);
          worker.setInt(3, id);
          worker.execSQL();
        }  
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_DMB_REPORTING];
  }

  /** Get CSC wo CFEB Errors */

  if (cache.getEMU(cscdqm::h::EMU_CSC_WO_CFEB, me)) {
    const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

    for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
      for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(i, j));
        if (cnt > 0) {
          const unsigned int id = i * 100 + j;
          worker.setSQL("update DQM_RUN_CHAMBER set RCH_WO_CFEB = :1 where RCH_RUN_NUMBER = :2 and RCH_CHA_ID = :3");
          worker.setInt(1, cnt);
          worker.setInt(2, runNumber);
          worker.setInt(3, id);
          worker.execSQL();
        }  
      }
    }
  } else {
    LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::EMU_DMB_REPORTING];
  }

  unsigned int i = 0, crateId, dmbId;
  while (cache.nextBookedCSC(i, crateId, dmbId)) {
    const unsigned int id = crateId * 100 + dmbId;

    /** Get main chamber data */

    worker.setSQL("select CHA_CFEBS, CHA_WIREGROUPS from DQM_CHAMBER where CHA_ID = :1");
    worker.setInt(1, id);
    worker.execSQLResults(true);
    unsigned int nCFEB = worker.getInt(1);
    unsigned int nWiregroups = worker.getInt(2);

    /** Get HV segments */

    std::vector<unsigned int> hvSegs;
    worker.setSQL("select CHS_CHANNELS from DQM_CHAMBER_HV_SEGMENT where CHS_CHA_ID = :1 order by CHS_NUMBER asc");
    worker.setInt(1, id);
    worker.execSQLResultsToVector(1, hvSegs);

    /** Get Out of Sync data */

    const unsigned int alct_oos = getL1OutOffSynData(cscdqm::h::CSC_ALCT_DMB_L1A_DIFF, crateId, dmbId, 0);
    const unsigned int clct_oos = getL1OutOffSynData(cscdqm::h::CSC_CLCT_DMB_L1A_DIFF, crateId, dmbId, 0);
    const unsigned int ddu_oos  = getL1OutOffSynData(cscdqm::h::CSC_DMB_DDU_L1A_DIFF, crateId, dmbId, 0);

    if (alct_oos > 0 || clct_oos > 0 || ddu_oos > 0) {
      worker.setSQL("update DQM_RUN_CHAMBER set RCH_ALCT_L1A_OUT_OFF_SYN = :1, RCH_CLCT_L1A_OUT_OFF_SYN = :2, RCH_DDU_L1A_OUT_OFF_SYN = :3 where RCH_RUN_NUMBER = :4 and RCH_CHA_ID = :5");
      worker.setInt(1, alct_oos);
      worker.setInt(2, clct_oos);
      worker.setInt(3, ddu_oos);
      worker.setInt(4, runNumber);
      worker.setInt(5, id);
      worker.execSQL();
    }

    for (unsigned int cfeb = 0; cfeb < nCFEB; cfeb++) {
      const unsigned int cfeb_oos  = getL1OutOffSynData(cscdqm::h::CSC_CFEBXX_DMB_L1A_DIFF, crateId, dmbId, cfeb);
      if (cfeb_oos > 0) {
        worker.setSQL("begin DQM_UPDATE.RUN_CFEB(:1, :2, :3, p_out_off_syn => :4); end;");
        worker.setInt(1, runNumber);
        worker.setInt(2, id);
        worker.setInt(3, cfeb + 1);
        worker.setInt(4, cfeb_oos);
        worker.execSQL();
      }
    }

    /** Get CSC Format Errors */

    if (cache.getCSC(cscdqm::h::CSC_BINCHECK_ERRORSTAT_TABLE, crateId, dmbId, 0, me)) {
      const TH2* h = dynamic_cast<const TH2*>(me->getTH1());
      for (int i = int(h->GetYaxis()->GetXmin()); i <= int(h->GetYaxis()->GetXmax()); i++) {
        const uint32_t cnt = uint32_t(h->GetBinContent(1, i));
        if (cnt > 0) {
          worker.setSQL("insert into DQM_RUN_CHAMBER_FORMAT values (:1, :2, :3, :4)");
          worker.setInt(1, runNumber);
          worker.setInt(2, id);
          worker.setInt(3, i);
          worker.setInt(4, cnt);
          worker.execSQL();
        }
      }
    } else {
      LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_BINCHECK_ERRORSTAT_TABLE];
    }

    /** Get CSC Dataflow Errors */

    if (cache.getCSC(cscdqm::h::CSC_BINCHECK_DATAFLOW_PROBLEMS_TABLE, crateId, dmbId, 0, me)) {
      const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

      unsigned int bits[24] = { 0 };
      for (int i = int(h->GetYaxis()->GetXmin()); i <= int(h->GetYaxis()->GetXmax()); i++) {
        bits[i] = uint32_t(h->GetBinContent(1, i));
      }
      
      /** Get Chamber Dataflow Errors */

      if (bits[1] > 0 || bits[2] > 0 || bits[8] > 0 || bits[9] > 0 || bits[15] > 0 || bits[16] > 0 || bits[22] > 0 || bits[23] > 0) {
        worker.setSQL("update DQM_RUN_CHAMBER set RCH_ALCT_FIFO_FULL = :1, RCH_ALCT_START_TIMEOUT = :2, RCH_ALCT_END_TIMEOUT = :3, RCH_TMB_FIFO_FULL = :4, RCH_TMB_START_TIMEOUT = :5, RCH_TMB_END_TIMEOUT = :6, RCH_CFEB_ACTIVE_DAV_MISMATCH = :7, RCH_BWORDS_FOUND = :8 where RCH_RUN_NUMBER = :9 and RCH_CHA_ID = :10");
        worker.setInt(1, bits[1]);
        worker.setInt(2, bits[8]);
        worker.setInt(3, bits[15]);
        worker.setInt(4, bits[2]);
        worker.setInt(5, bits[9]);
        worker.setInt(6, bits[16]);
        worker.setInt(7, bits[22]);
        worker.setInt(8, bits[23]);
        worker.setInt(9, runNumber);
        worker.setInt(10, id);
        worker.execSQL();
      }

      /** Get CFEB Dataflow Errors */

      for (unsigned int cfeb = 0; cfeb < nCFEB; cfeb++) {
        if (bits[cfeb + 3] > 0 || bits[cfeb + 10] > 0 || bits[cfeb + 17] > 0) {
          worker.setSQL("begin DQM_UPDATE.RUN_CFEB(:1, :2, :3, :4, :5, :6); end;");
          worker.setInt(1, runNumber);
          worker.setInt(2, id);
          worker.setInt(3, cfeb + 1);
          worker.setInt(4, bits[cfeb + 3]);
          worker.setInt(5, bits[cfeb + 10]);
          worker.setInt(6, bits[cfeb + 17]);
          worker.execSQL();
        }
      }

    } else {
      LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_BINCHECK_DATAFLOW_PROBLEMS_TABLE];
    }

    /** Get CFEB Events */

    if (cache.getCSC(cscdqm::h::CSC_ACTUAL_DMB_CFEB_DAV_RATE, crateId, dmbId, 0, me)) {
      const TH1* h = me->getTH1();
	for (unsigned int icfeb = 0; icfeb < nCFEB; icfeb++) {
	  unsigned int cnt = int(h->GetBinContent(icfeb + 1));
        if (cnt > 0) {
          worker.setSQL("begin DQM_UPDATE.RUN_CFEB(:1, :2, :3, p_dav_events => :4); end;");
          worker.setInt(1, runNumber);
          worker.setInt(2, id);
          worker.setInt(3, icfeb + 1);
          worker.setInt(4, cnt);
          worker.execSQL();
        }
      }
    } else {
      LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_ACTUAL_DMB_CFEB_DAV_RATE];
    }

    /** Loop over chamber layers */
    
    for (unsigned int ilayer = 1; ilayer <= 6; ilayer++) {

      worker.setSQL("insert into DQM_RUN_CHAMBER_LAYER (LAY_RUN_NUMBER,LAY_CHA_ID,LAY_LAYER) values (:1, :2, :3)");
      worker.setInt(1, runNumber);
      worker.setInt(2, id);
      worker.setInt(3, ilayer);
      worker.execSQL();

      /** Get Layer CFEB SCA data */

      if (cache.getCSC(cscdqm::h::CSC_CFEB_ACTIVESTRIPS_LYXX, crateId, dmbId, ilayer, me)) {
        const TH1* h = me->getTH1();
        for (unsigned int icfeb = 0; icfeb < nCFEB; icfeb++) {
          std::vector<unsigned int> ch_bits;
          for (unsigned int ch = 1; ch <= 16; ch++) {
            ch_bits.push_back(int(h->GetBinContent(ch + icfeb * 16)));
          }
          worker.setSQL("insert into DQM_RUN_CFEB_LAYER (RCL_RUN_NUMBER,RCL_CHA_ID,RCL_LAYER,RCL_CFEB,RCL_SCA_CHANNEL_DATA) values (:1, :2, :3, :4, :5)");
          worker.setInt(1, runNumber);
          worker.setInt(2, id);
          worker.setInt(3, ilayer);
          worker.setInt(4, icfeb + 1);
          worker.setVector(5, ch_bits, "DQM_UINT_ARRAY");
          worker.execSQL();
        }
      } else {
        LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_CFEB_ACTIVESTRIPS_LYXX];
      }

      /** Get Layer CFEB CMP data */

      if (cache.getCSC(cscdqm::h::CSC_CLCT_LYXX_RATE, crateId, dmbId, ilayer, me)) {
        const TH1* h = me->getTH1();
        for (unsigned int icfeb = 0; icfeb < nCFEB; icfeb++) {
          std::vector<unsigned int> ch_bits;
          for (unsigned int ch = 1; ch <= 32; ch++) {
            ch_bits.push_back(int(h->GetBinContent(ch + icfeb * 32)));
          }
          worker.setSQL("update DQM_RUN_CFEB_LAYER set RCL_CMP_CHANNEL_DATA = :1 where RCL_RUN_NUMBER = :2 and RCL_CHA_ID = :3 and RCL_LAYER = :4 and RCL_CFEB = :5");
          worker.setVector(1, ch_bits, "DQM_UINT_ARRAY");
          worker.setInt(2, runNumber);
          worker.setInt(3, id);
          worker.setInt(4, ilayer);
          worker.setInt(5, icfeb + 1);
          worker.execSQL();
        }
      } else {
        LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_CLCT_LYXX_RATE];
      }

      /** Get Layer AFEB and HV seg data */

      if (cache.getCSC(cscdqm::h::CSC_ALCT_LYXX_RATE, crateId, dmbId, ilayer, me)) {
        const TH1* h = me->getTH1();
        
        std::vector<unsigned int> afebs;
        for (unsigned int iseg = 0; iseg < (nWiregroups / 8); iseg++) {
          // const unsigned int afeb = iseg * 3 + (ilayer + 1) / 2;
          const unsigned int cnt = int(h->Integral(iseg * 8 + 1 + 1, (iseg + 1) * 8 + 1));
          afebs.push_back(cnt);
        }

        std::vector<unsigned int> hvdata;
        unsigned int bin_from = 0, bin_to = 0;
        for (std::vector<unsigned int>::const_iterator it = hvSegs.begin(); it != hvSegs.end(); it++) {
          bin_from = bin_to + 1;
          bin_to   = bin_from + *it - 1;
          const unsigned int cnt = int(h->Integral(bin_from, bin_to));
          hvdata.push_back(cnt);
        }

        worker.setSQL("update DQM_RUN_CHAMBER_LAYER set LAY_AFEB_DATA = :1, LAY_HV_SEG_DATA = :2 where LAY_RUN_NUMBER = :3 and LAY_CHA_ID = :4 and LAY_LAYER = :5");
        worker.setVector(1, afebs, "DQM_UINT_ARRAY");
        worker.setVector(2, hvdata, "DQM_UINT_ARRAY");
        worker.setInt(3, runNumber);
        worker.setInt(4, id);
        worker.setInt(5, ilayer);
        worker.execSQL();

      } else {
        LOG_WARN << "Can not find " << cscdqm::h::names[cscdqm::h::CSC_ALCT_LYXX_RATE];
      }

    }

  }

}


