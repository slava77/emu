
*******************************************************************************
* This is daq_module.inc. 
* It must be kept in agreement with the module identifiers in daq_module.h 
*******************************************************************************

      integer*4    HEAD_ID, SCL2551_ID, SCL4434_ID, TDC2228_ID
      integer*4    TDC2277_ID, TDC3377_ID, INREG_ID, OUTREG_ID 
      integer*4    IOREG_ID, ADC2232_ID, ADC2249_ID, TMB_ID, CCB_ID
      integer*4    DDU_ID, DDU2_ID, D360_ID
      INTEGER*4    PAT4448_ID, DSC7106_ID, DSCC808_ID, DAC_ID
      integer*4    PULSER_ID, ALCTCAL_ID, STRIPCAL_ID, CABLE_MAP_ID
      integer*4    MOD_CONF_ID, TRIG_SETUP_ID, AFEB_CONF_ID
      integer*4    ANODE_HOT_ID, ANODE_THRES_ID, ANODE_DELAY_ID
      integer*4    CATH_HOT_ID, CATH_THRES_ID, CCB2_ID, DMB2_ID
      INTEGER*4    ALCT2_ID, USER_NAME_ID, DAQ_REV_ID, SITE_ID
      INTEGER*4    ANODE_PATT_ID

      parameter(HEAD_ID      =x'48454144') ! "HEAD" event header
      parameter(SCL2551_ID   =x'53434C31') ! "SCL1" 2551 scaler
      parameter(SCL4434_ID   =x'53434C32') ! "SCL2" 4434 scaler
      parameter(TDC2228_ID   =x'54444331') ! "TDC1" TDC2228
      parameter(TDC2277_ID   =x'54444332') ! "TDC2" TDC2277
      parameter(TDC3377_ID   =x'54444333') ! "TDC3" TDC3377
      parameter(INREG_ID     =x'494E5231') ! "INR1" input register (SEN, Jorway)
      parameter(OUTREG_ID    =x'4F545231') ! "OTR1" output register (Jorway 41)
      parameter(IOREG_ID     =x'494F5231') ! "IOR1" input/output reg (CAEN C291)
      parameter(ADC2232_ID   =x'41444331') ! "ADC1" ADC2232
      parameter(ADC2249_ID   =x'41444332') ! "ADC2" ADC2249
      parameter(TMB_ID       =x'544D4232') ! "TMB2" trigger mother brd (VME 99)
      parameter(CCB_ID       =x'43434231') ! "CCB1" clock and control board 99
      parameter(CCB2_ID      =x'43434232') ! "CCB2" clock and control board 01
      parameter(DMB2_ID      =x'444D4232') ! "DMB2" daq mother board 01
      parameter(DDU_ID       =x'44445531') ! "DDU1" PCI DDU card for DAQMB 99
      parameter(DDU2_ID      =x'44445532') ! "DDU2" PCI DDU card for DAQMB 01
      parameter(D360_ID      =x'44333630') ! "D360" dynatem controller card
      parameter(PAT4448_ID   =x'50415431') ! "PAT1" 4448 pattern register
      parameter(DSC7106_ID   =x'44534331') ! "DSC1" discriminator(Phillips 7106)
      parameter(DSCC808_ID   =x'44534332') ! "DSC2" discriminator(CAEN C808)
      parameter(DAC_ID       =x'44414331') ! "DAC1" CAMAC DAC (not read out)
      parameter(PULSER_ID    =x'504C5352') ! "PLSR" CAMAC pulser (not read out)
      parameter(ALCTCAL_ID   =x'4143414C') ! "ACAL" wire JTAG & calib control
      parameter(ALCT2_ID     =x'414C4332') ! "ALC2" alct 2001 
      parameter(STRIPCAL_ID  =x'4343414C') ! "CCAL" cathode calibration control
      parameter(CABLE_MAP_ID =x'434D4346') ! "CMCF" the cable map
      parameter(MOD_CONF_ID  =x'4D434346') ! "MCCF" the module config file
      parameter(TRIG_SETUP_ID=x'54534346') ! "TSCF" the trigger setup file
      parameter(AFEB_CONF_ID =x'41464346') ! "AFCF" the AFEB config file

      parameter(ANODE_HOT_ID =x'41484346') ! "AHCF" anode hot mask file
      parameter(ANODE_THRES_ID=x'41544346') ! "ATCF" anode threshold file
      parameter(ANODE_DELAY_ID=x'41444346') ! "ADCF" anode delay file
      parameter(CATH_HOT_ID  =x'43484346') ! "CHCF" cathode hot mask file
      parameter(CATH_THRES_ID=x'43544346') ! "CTCF" cathode threshold file
      parameter(USER_NAME_ID =x'5553524E') ! "USRN"  user name identification
      parameter(DAQ_REV_ID   =x'44415152') ! "DAQR"  daq revision identification
      parameter(SITE_ID      =x'53495445') ! "SITE"  site identification
      parameter(ANODE_PATT_ID=x'41504154') ! "APAT"  pattern identification

