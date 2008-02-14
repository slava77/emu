var TREE_ITEMS = [
	['EMU', ' ',
           ['EMU Test00 - Readout Buffer Errors','EMU_Test00_Readout_Buffer_Errors.png'],
           ['EMU Test01 - DDUs in Readout','EMU_Test01_DDUs_in_Readout.png'],
           ['EMU Test02 - DDU Event Size','EMU_Test02_DDU_Event_Size.png'],
           ['EMU Test03 - DDU Reported Errors','EMU_Test03_DDU_Reported_Errors.png'],
           ['EMU Test04 - DDU Format Errors','EMU_Test04_DDU_Format_Errors.png'],
           ['EMU Test05 - DDU Live Inputs','EMU_Test05_DDU_Live_Inputs.png'],
           ['EMU Test06 - DDU Inputs in ERROR/WARNING State','EMU_Test06_DDU_Inpits_in_Error_State.png'],
           ['EMU Test07 - DDU Inputs with Data','EMU_Test07_DDU_Inputs_with_Data.png'],
           ['EMU Test08a - Unpacked DMBs','EMU_Test08a_Unpacked_DMBs.png'],
           ['EMU Test08b - Unpacked CSCs','EMU_Test08b_Unpacked_CSCs.png'],
           ['EMU Test09a - DMBs with Format Errors','EMU_Test09a_DMB_Format_Errors.png'],
           ['EMU Test09b - CSCs with Format Errors','EMU_Test09b_CSC_Format_Errors.png'],
           ['EMU Test09c - DMBs with Format Errors (Fractions)','EMU_Test09c_DMB_Format_Errors_Fract.png'],
           ['EMU Test09d - CSCs with Format Errors (Fractions)','EMU_Test09d_CSC_Format_Errors_Fract.png'],
           ['EMU Test10a - DMBs with CFEB B-Words','EMU_Test10a_DMB_Format_Warnings.png'],
           ['EMU Test10b - CSCs with CFEB B-Words (Fraction)','EMU_Test10b_CSC_Format_Warnings_Fract.png'],
	],
	['DDU', ' ',
           ['Connected and Active Inputs','DDU_Connected_and_Active_Inputs.png'],
           ['DMBs DAV and Unpacked vs DMBs Active','DDU_DMBs_DAV_and_Unpacked_vs_DMBs_Active.png'],
           ['Error Status from DDU Trailer','DDU_Error_Status_from_DDU_Trailer.png'],
           ['Event Buffer Size and DDU Word Count','DDU_Event_Buffer_Size_and_DDU_Word_Count.png'],
           ['L1A and BXN Counters','DDU_L1A_and_BXN_Counters.png'],
           ['State of CSCs','DDU_State_of_CSCs.png'],
	],
	['CSC', ' ',
           ['ALCT: ALCT0_BXN and ALCT_L1A_BXN Synchronization','ALCT/ALCT_ALCT0_BXN_and_ALCT_L1A_BXN_Synchronization.png'],
           ['ALCT: ALCT0 Key Wiregroups, Patterns and Quality','ALCT/ALCT_ALCT0_Key_Wiregroups__Patterns_and_Quality.png'],
           ['ALCT: ALCT1_BXN and ALCT_L1A_BXN Synchronization','ALCT/ALCT_ALCT1_BXN_and_ALCT_L1A_BXN_Synchronization.png'],
           ['ALCT: ALCT1 Key Wiregroups, Patterns and Quality','ALCT/ALCT_ALCT1_Key_Wiregroups__Patterns_and_Quality.png'],
           ['ALCT: ALCTs Found','ALCT/ALCT_ALCTs_Found.png'],
           ['ALCT: Anode Hit Occupancy per Chamber','ALCT/ALCT_Anode_Hit_Occupancy_per_Chamber.png'],
           ['ALCT: Anode Hit Occupancy per Wire Group','ALCT/ALCT_Anode_Hit_Occupancy_per_Wire_Group.png'],
           ['ALCT: Number of Words in ALCT','ALCT/ALCT_Number_of_Words_in_ALCT.png'],
           ['ALCT: Raw Hit Time Bin Average Occupancy','ALCT/ALCT_Raw_Hit_Time_Bin_Average_Occupancy.png'],
           ['ALCT: Raw Hit Time Bin Occupancy','ALCT/ALCT_Raw_Hit_Time_Bin_Occupancy.png'],
           ['CFEB: Cluster Duration','CFEB/CFEB_Cluster_Duration.png'],
           ['CFEB: Clusters Charge','CFEB/CFEB_Clusters_Charge.png'],
           ['CFEB: Clusters Width','CFEB/CFEB_Clusters_Width.png'],
           ['CFEB: Free SCA Cells','CFEB/CFEB_Free_SCA_Cells.png'],
           ['CFEB: Number of Clusters','CFEB/CFEB_Number_of_Clusters.png'],
           ['CFEB: Number of SCA blocks locked by LCTs','CFEB/CFEB_Number_of_SCA_blocks_locked_by_LCTs.png'],
           ['CFEB: Number of SCA blocks locked by LCTxL1','CFEB/CFEB_Number_of_SCA_blocks_locked_by_LCTxL1.png'],
           ['CFEB: Out of ADC Range Strips','CFEB/CFEB_Out_of_ADC_Range_Strips.png'],
           ['CFEB: Pedestals (First Sample)','CFEB/CFEB_Pedestals_First_Sample.png'],
           ['CFEB: Pedestals RMS','CFEB/CFEB_Pedestals_RMS.png'],
           ['CFEB: SCA Active Strips Occupancy','CFEB/CFEB_SCA_Active_Strips_Occupancy.png'],
           ['CFEB: SCA Active Time Samples vs Strip Numbers','CFEB/CFEB_SCA_Active_Time_Samples_vs_Strip_Numbers.png'],
           ['CFEB: SCA Active Time Samples vs Strip Numbers Profile','CFEB/CFEB_SCA_Active_Time_Samples_vs_Strip_Numbers_Profile.png'],
           ['CFEB: SCA Block Occupancy','CFEB/CFEB_SCA_Block_Occupancy.png'],
           ['CFEB: SCA Cell Peak','CFEB/CFEB_SCA_Cell_Peak.png'],
           ['CSC: Data Block Finding Efficiency','CSC/CSC_Data_Block_Finding_Efficiency.png'],
           ['CSC: Data Format Errors and Warnings','CSC/CSC_Data_Format_Errors_and_Warnings.png'],
           ['DMB: CFEB Multiple Overlaps','DMB/DMB_CFEB_Multiple_Overlaps.png'],
           ['DMB: CFEBs DAV and Active','DMB/DMB_CFEBs_DAV_and_Active.png'],
           ['DMB: DMB-CFEB-SYNC BXN Counter','DMB/DMB_DMB-CFEB-SYNC_BXN_Counter.png'],
           ['DMB: FEB Status (Timeouts, FIFO, L1 pipe)','DMB/DMB_FEB_Status_Timeouts__FIFO__L1_pipe.png'],
           ['DMB: FEBs DAV and Unpacked','DMB/DMB_FEBs_DAV_and_Unpacked.png'],
           ['SYNC: ALCT - DMB Synchronization','SYNC/SYNC_ALCT_-_DMB_Synchronization.png'],
           ['SYNC: CFEB - DMB Synchronization','SYNC/SYNC_CFEB_-_DMB_Synchronization.png'],
           ['SYNC: DMB - DDU Synchronization','SYNC/SYNC_DMB_-_DDU_Synchronization.png'],
           ['SYNC: TMB - ALCT Syncronization','SYNC/SYNC_TMB_-_ALCT_Syncronization.png'],
           ['SYNC: TMB - DMB Synchronization','SYNC/SYNC_TMB_-_DMB_Synchronization.png'],
           ['TMB-CLCT: CLCT0_BXN and TMB_L1A_BXN Synchronization','TMB/TMB-CLCT_CLCT0_BXN_and_TMB_L1A_BXN_Synchronization.png'],
           ['TMB-CLCT: CLCT0 Key HalfStrips, Patterns and Quality','TMB/TMB-CLCT_CLCT0_Key_HalfStrips__Patterns_and_Quality.png'],
           ['TMB-CLCT: CLCT1_BXN and TMB_L1A_BXN Synchronization','TMB/TMB-CLCT_CLCT1_BXN_and_TMB_L1A_BXN_Synchronization.png'],
           ['TMB-CLCT: CLCT1 Key HalfStrips, Patterns and Quality','TMB/TMB-CLCT_CLCT1_Key_HalfStrips__Patterns_and_Quality.png'],
           ['TMB-CLCT: CLCTs Found','TMB/TMB-CLCT_CLCTs_Found.png'],
           ['TMB-CLCT: Cathode Comparator Hit Occupancy per Chamber','TMB/TMB-CLCT_Cathode_Comparator_Hit_Occupancy_per_Chamber.png'],
           ['TMB-CLCT: Cathode Comparator Hit Occupancy per Half Strip','TMB/TMB-CLCT_Cathode_Comparator_Hit_Occupancy_per_Half_Strip.png'],
           ['TMB-CLCT: Comparator Raw Hit Time Bin Average Occupancy','TMB/TMB-CLCT_Comparator_Raw_Hit_Time_Bin_Average_Occupancy.png'],
           ['TMB-CLCT: Comparator Raw Hit Time Bin Occupancy','TMB/TMB-CLCT_Comparator_Raw_Hit_Time_Bin_Occupancy.png'],
           ['TMB: ALCT0 KeyWiregroup vs CLCT0 Key DiStrip','TMB/TMB_ALCT0_KeyWiregroup_vs_CLCT0_Key_DiStrip.png'],
           ['TMB: ALCT - CLCT Time MatchingSynchronization','TMB/TMB_ALCT_-_CLCT_Time_MatchingSynchronization.png'],
           ['TMB: Number of Words in TMB','TMB/TMB_Number_of_Words_in_TMB.png'],
	],
];
