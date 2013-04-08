C csc_parameters.inc
C
C Author: David Chrisman
C Date: May 28, 1999
C
C Note: csc_parameters.h (C) and csc_parameters.inc (FORTRAN)       
C       should be identical!
C
C

      integer NLAYER, NSTRIP, NWIRE, NWIRES_MAX,MAX_WIRE_HITS
      integer NMOD_HEAD, NLCT_HEAD
      integer NDDU_HEAD, NDDU_TRAIL
      integer NCFEB, NCFEB_CHAN
      integer NAFEB, NAFEB_CHIP, NAFEB_CHAN
      integer MAX_SCA_SAMPLE
      integer NALCT_BUCKET, NCLCT_BUCKET
      integer FEB_SIZE
      integer ALCT_NO_DUMP_SIZE, ALCT_FULL_DUMP_SIZE
      integer CLCT_NO_DUMP_SIZE, CLCT_FULL_DUMP_SIZE
      integer NRPC_SECTOR, NRPC_STRIP, NRPC_HIT
      integer NBEAM_CHAM_LAYER
      integer NSCINT, NSCINT_LAYERS, NSCINT_PER_LAYER, MAX_SCINT_HITS
      integer NSCALER_CHAN
      integer USER_LENGTH, SITE_LENGTH
      
      parameter(NLAYER         = 6  ) ! Number of layers per chamber
      parameter(NSTRIP         = 80 ) ! Number of strips per layer
      parameter(NWIRE          = 112) ! Number of wire groups per layer
      parameter(NWIRES_MAX     = 112) ! MAX Number of wire groups per layer
      parameter(MAX_WIRE_HITS  =6144) ! Max number of TDC hits allow per event

      parameter(USER_LENGTH    = 20)  ! Name of the tester, half of daq_conf.h
      parameter(SITE_LENGTH    = 10 ) ! Name of the site, half of daq_conf.h

      parameter(NMOD_HEAD      = 4  ) ! Number of generic module header words

      parameter(NLCT_HEAD      = 8  ) ! Number of LCT99 header words
      parameter(NDDU_HEAD      = 6  ) ! Number of DDU header words
      parameter(NDDU_TRAIL     = 2  ) ! Number of DDU trailer words

      parameter(NCFEB          = 5  ) ! Number of cathode front-end boards
      parameter(NCFEB_CHAN     = 96 ) ! Number of channels per CFEB

      parameter(NAFEB          = 4  ) ! Number of anode front-end boards
      parameter(NAFEB_CHIP     = 6  ) ! Number of chips per AFEB
      parameter(NAFEB_CHAN     = 96 ) ! Number of channels per AFEB

      parameter(MAX_SCA_SAMPLE = 16 ) ! Max number of SCA time samples     

      parameter(NALCT_BUCKET   = 31 ) ! Max number of ALCT time buckets
      parameter(NCLCT_BUCKET   = 16 ) ! Max number of CLCT time buckets

      parameter(NRPC_SECTOR    =  6 ) ! Number sectors per RPC
      parameter(NRPC_STRIP     = 32 ) ! Number of channels per sector
      parameter(NRPC_HIT       = 16 ) ! Number of hits allowed per channel

      parameter(NBEAM_CHAM_LAYER = 6 ) ! Number of Beam Chamber Layers

      parameter(NSCINT         = 48 ) ! Number of trigger scintillators
      parameter(NSCINT_LAYERS  = 4  ) ! Number of trigger scintillators
      parameter(NSCINT_PER_LAYER = 12)! Number of trigger scintillators
      parameter(MAX_SCINT_HITS = 100) ! Max Number of trigger scintillators hits

      parameter(NSCALER_CHAN   = 12 ) ! Number of Lecroy 2551 scaler channels
C 
C FEB data size in 16-bit words:
C FEB_SIZE = (NFEB_CHAN)*(NLAYER) + 1 checksum + 1 trailer
C
      parameter(FEB_SIZE      = NCFEB_CHAN + 1 + 1)

      parameter(ALCT_NO_DUMP_SIZE   =  10)
      parameter(CLCT_NO_DUMP_SIZE   =  10)
      parameter(ALCT_FULL_DUMP_SIZE = 730) ! 10 header + 12 words * 4 LCTs * 15 time bins
      parameter(CLCT_FULL_DUMP_SIZE = 220)
