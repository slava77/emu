%{
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "daq_module.h"
#include "csc_parameters.h"
#define YY_DECL int lex_cable_map()
#define YY_SKIP_YYWRAP

static int map_mod_TDCnn77();  /* for Lecroy TDCs 2277 and 3377 */
static int map_mod_DSC7106();  /* Phillips discriminators */
static int map_mod_SCALER();
static int map_mod_ADC2249();
static int map_CFEB();         /* Cathode front-end boards */
static int map_range_CFEB();   /* A range of strips for the Cathode FEBs */
static int map_AFEB();         /* Anode front-end boards   */

int yywrap();

int nlines = 0;

%}

INT        [0-9]+
INT2       [1-9]?[0-9]
INT1       [0-9]
FLOAT      (\+|-)?([0-9]+\.?[0-9]*|\.[0-9]+)([eE](\+|-)?[0-9]+)?

CRATE             [ ]+{INT}[ ]+
SLOT              [ ]+{INT}[ ]+
LAYER             [ ]+{INT}[ ]+
DATA_TYPE         [ ]+{INT}[ ]+
FEB_NUM           [ ]+{INT}[ ]+
CH                [ ]+{INT}[ ]*
FEB_CHAN          [ ]+{INT}[ ]+
FEB_CHAN_RANGE    [ ]+{INT}"-"{INT}[ ]+
CHIP_NUM          [ ]+{INT}[ ]+
CHIP_NUM_RANGE    [ ]+{INT}"-"{INT}[ ]+
MOD_CHAN          [ ]+{INT}[ ]+
MOD_CHAN_RANGE    [ ]+{INT}"-"{INT}[ ]+
CHAM_CHAN         [ ]+{INT}[ ]*
CHAM_CHAN_RANGE   [ ]+{INT}"-"{INT}[ ]*

%%       /****** The Rules Section ******/

\n     ++nlines;     /* Count the number of lines in the file */

#+ {                 /* Throw away the comment lines */
  while( input() != '\n' );
  unput('\n');
}

TDC3377_MAP{DATA_TYPE}{CRATE}{SLOT}{LAYER}{MOD_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_mod_TDCnn77() != 0) return 1;
} 

TDC2277_MAP{DATA_TYPE}{CRATE}{SLOT}{LAYER}{MOD_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_mod_TDCnn77() != 0) return 1;
} 

ADC2249_MAP{DATA_TYPE}{CRATE}{SLOT}{LAYER}{MOD_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_mod_ADC2249() != 0) return 1;
} 

DSC7106_MAP{DATA_TYPE}{CRATE}{SLOT}{LAYER}{MOD_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_mod_DSC7106() != 0) return 1;
} 

SCALER_MAP{DATA_TYPE}{CRATE}{SLOT}{CH} { 
  if (map_mod_SCALER() != 0) return 1;
} 

CFEB_MAP_RANGE{FEB_NUM}{LAYER}{FEB_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_range_CFEB() != 0) return 1;
} 

CFEB_MAP{FEB_NUM}{LAYER}{FEB_CHAN}{CHAM_CHAN} {
  if (map_CFEB() != 0) return 1;
}

AFEB_MAP{FEB_NUM}{CHIP_NUM}{LAYER}{FEB_CHAN_RANGE}{CHAM_CHAN_RANGE} { 
  if (map_AFEB() != 0) return 1;
} 

%%       /****** The User Code Section ******/


chamber_map_type map_;

int read_cable_map(int cable_map_file_id)
{
  char *path;
  char file_name[80];

/* Get the PATH to the data directory.
 * For ONLINE DAQ cable map files are in $DATADIR/cable_map
 */
  path = getenv("DATADIR");
  if(path == NULL) {
      printf("read_cable_map(): Can\'t find cable map. Environment variable DATADIR is not set. EE\n");
    exit(1);
  }
  sprintf(file_name,"%s/cable_map/cable_map_%04d", path, cable_map_file_id);

  printf("Read the cable map file %s\n", file_name);

  yyin = fopen(file_name, "r");
  if (yyin == NULL) {
    #ifdef OFFLINE_ONLY
    printf(
    #else
    syslog(LOG_LOCAL1|LOG_ERR, 
    #endif
      "read_cable_map(): Can\'t open cable map file %s EE\n", file_name);
    return 1;
  }
  if (lex_cable_map() !=0 ) {
    #ifdef OFFLINE_ONLY
    printf(
    #else
    syslog(LOG_LOCAL1|LOG_ERR, 
    #endif
      "Error reading cable map file EE\n");
    return 1;
  }
  fclose(yyin);

  return 0;

}

/* Dummy version of yywrap */
int yywrap()
{

  return 1;

}



static int map_mod_TDCnn77()
{
  static int first = 1;
  short data_type_layer_chan;
  char *mod_id, *mod_chan, *cham_chan;
  int data_type, crate, slot, layer;
  int mod_first, mod_last, cham_first, cham_last, tmp;
  int imod, icham;
  
/* Initialize the map arrays */
  if (first) {
    for (crate = 0; crate < MAX_CRATE; crate++)
      for (slot = 0; slot < MAX_SLOT; slot++)
        for (imod = 0; imod < 32; imod++) {
          map_.TDC3377[crate][slot][imod] = 0;
          map_.TDC2277[crate][slot][imod] = 0;
        }
    first = 0;
  }

  mod_id = strtok( (char *)yytext, " ");

  data_type  = atoi(strtok(NULL, " "));
  crate      = atoi(strtok(NULL, " "));
  slot       = atoi(strtok(NULL, " "));
  layer      = atoi(strtok(NULL, " "));

  mod_chan   = strtok(NULL, " ");
  cham_chan  = strtok(NULL, " ");

  mod_first  = atoi(strtok(mod_chan, "-"));
  mod_last   = atoi(strtok(NULL, "-"));
  
  cham_first = atoi(strtok(cham_chan, "-"));
  cham_last  = atoi(strtok(NULL, "-"));

/* In cable map file, slot starts counting from 1. 
 * As an index of an array we want it to start from 0.
 */
  slot--;

/*********************** Check if the data is valid ***********************/

  if ((data_type < 0) || (data_type > 9)) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Data type out of range: %d EE",  data_type);
    return 1;
  }

  if (crate >= MAX_CRATE) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Crate number > %d EE", MAX_CRATE - 1);
    return 1;
  }

  if (slot >= MAX_SLOT) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Slot number > %d EE", MAX_SLOT);
    return 1;
  }

  if (abs(mod_first - mod_last) != abs(cham_first - cham_last)) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "Number of module channels != number of chamber channels EE");
    return 1;
  }


  if ((mod_last > 31) || (mod_first < 0) ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Module channels out of range EE");
    return 1;
  }

  if ((cham_last  > NWIRE || cham_last  < 0) || 
      (cham_first > NWIRE || cham_first < 0)    ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Chamber channels out of range EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

/* Make sure AFEB channels are in increasing order */
  if (mod_first > mod_last) {
    tmp       = mod_last;
    mod_last  = mod_first;
    mod_first = tmp; 

    tmp        = cham_last;
    cham_last  = cham_first;
    cham_first = tmp; 
  }

  icham = cham_first;
  for ( imod = mod_first; imod < mod_last + 1 ; imod++ ) {

    /* Upper 4 bits data_type next 4 bits layer number and
     * lower 8 bits channel number 
     */
    data_type_layer_chan = (short )((data_type & 0x000F)<<12) +
                           (short )((layer & 0x000F)<<8) + 
                           (short )(icham & 0x00FF);

    if (strcmp(mod_id, "TDC3377_MAP") == 0)
      map_.TDC3377[crate][slot][imod] = data_type_layer_chan;
    else 
      map_.TDC2277[crate][slot][imod] = data_type_layer_chan;

    if (cham_first <= cham_last)
      icham++;
    else
      icham--;

  }

  return 0;

}


static int map_mod_ADC2249()
{
  static int first = 1;
  short data_type_layer_chan;
  char *mod_id, *mod_chan, *cham_chan;
  int data_type, crate, slot, layer;
  int mod_first, mod_last, cham_first, cham_last;
  int imod, icham;
  
/* Initialize the map arrays */
  if (first) {
    for (crate = 0; crate < MAX_CRATE; crate++)
      for (slot = 0; slot < MAX_SLOT; slot++)
        for (imod = 0; imod < 12; imod++) {
          map_.ADC2249[crate][slot][imod] = 0;
        }
    first = 0;
  }

  mod_id = strtok( (char *)yytext, " ");

  data_type  = atoi(strtok(NULL, " "));
  crate      = atoi(strtok(NULL, " "));
  slot       = atoi(strtok(NULL, " "));
  layer      = atoi(strtok(NULL, " "));

  mod_chan   = strtok(NULL, " ");
  cham_chan  = strtok(NULL, " ");

  mod_first  = atoi(strtok(mod_chan, "-"));
  mod_last   = atoi(strtok(NULL, "-"));
  
  cham_first = atoi(strtok(cham_chan, "-"));
  cham_last  = atoi(strtok(NULL, "-"));

/* In cable map file, slot starts counting from 1. 
 * As an index of an array we want it to start from 0.
 */
  slot--;

/*********************** Check if the data is valid ***********************/

  if (crate >= MAX_CRATE) {
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Crate number > %d EE\n", MAX_CRATE - 1);
    return 1;
  }


  if (slot >= MAX_SLOT) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Slot number > %d EE\n", MAX_SLOT);
    return 1;
  }


  if (abs(mod_first - mod_last) != abs(cham_first - cham_last)) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Number of module channels != number of chamber channels EE\n");
    return 1;
  }


  if (mod_first > mod_last) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Module channels must be in increasing order EE\n");
    return 1;
  }


  if ((mod_last > 11) || (mod_first < 0) ) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Module channels out of range EE\n");
    return 1;
  }

  if ((cham_last  > NSTRIP || cham_last  < 0) || 
      (cham_first > NSTRIP || cham_first < 0)    ) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Chamber channels out of range EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

  icham = cham_first;
  for ( imod = mod_first; imod < mod_last + 1 ; imod++ ) {

    /* Upper 4 bits data_type next 4 bits layer number and
     * lower 8 bits channel number 
     */
    data_type_layer_chan = (short )((data_type & 0x000F)<<12) +
                           (short )((layer & 0x000F)<<8) + 
                           (short )(icham & 0x00FF);

    map_.ADC2249[crate][slot][imod] = data_type_layer_chan;

    if (cham_first <= cham_last)
      icham++;
    else
      icham--;

  }

  return 0;

}

static int map_mod_DSC7106()
{
  static int first = 1;
  short data_type_layer_chan;
  char *mod_id, *mod_chan, *cham_chan;
  int data_type, crate, slot, layer;
  int mod_first, mod_last, cham_first, cham_last;
  int imod, icham;
  
/* Initialize the map arrays */
  if (first) {
    for (crate = 0; crate < MAX_CRATE; crate++)
      for (slot = 0; slot < MAX_SLOT; slot++)
        for (imod = 0; imod < 16; imod++) {
          map_.DSC7106[crate][slot][imod] = 0;
        }
    first = 0;
  }

  mod_id = strtok( (char *)yytext, " ");

  data_type  = atoi(strtok(NULL, " "));
  crate      = atoi(strtok(NULL, " "));
  slot       = atoi(strtok(NULL, " "));
  layer      = atoi(strtok(NULL, " "));

  mod_chan   = strtok(NULL, " ");
  cham_chan  = strtok(NULL, " ");

  mod_first  = atoi(strtok(mod_chan, "-"));
  mod_last   = atoi(strtok(NULL, "-"));
  
  cham_first = atoi(strtok(cham_chan, "-"));
  cham_last  = atoi(strtok(NULL, "-"));

/* In cable map file, slot starts counting from 1. 
 * As an index of an array we want it to start from 0.
 */
  slot--;

/*********************** Check if the data is valid ***********************/

  if (crate >= MAX_CRATE) {
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Crate number > %d EE\n", MAX_CRATE - 1);
    return 1;
  }

  if (slot >= MAX_SLOT) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Slot number > %d EE\n", MAX_SLOT);
    return 1;
  }

  if (abs(mod_first - mod_last) != abs(cham_first - cham_last)) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Number of module channels != number of chamber channels EE\n");
    return 1;
  }

  if (mod_first > mod_last) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Module channels must be in increasing order EE\n");
    return 1;
  }

  if ((mod_last > 15) || (mod_first < 0) ) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Module channels out of range EE\n");
    return 1;
  }

  if ((cham_last  > 12 || cham_last  < 1) || 
      (cham_first > 12 || cham_first < 1)    ) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Scintillator number out of range 1-12 EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

  icham = cham_first;
  for ( imod = mod_first; imod < mod_last + 1 ; imod++ ) {

    /* Upper 4 bits data_type next 4 bits layer number and
     * lower 8 bits channel number 
     */
    data_type_layer_chan = (short )((data_type & 0x000F)<<12) +
                           (short )((layer & 0x000F)<<8) + 
                           (short )(icham & 0x00FF);

    map_.DSC7106[crate][slot][imod] = data_type_layer_chan;

    if (cham_first <= cham_last) icham++;
    else icham--;
  }

  return 0;
}


static int map_mod_SCALER()
{
  static int first = 1;
  int data_type, crate, slot, ch;
  char *mod_id;
  
/* Initialize the map arrays */
  if (first) {
    for (crate = 0; crate < MAX_CRATE; crate++)
      for (slot = 0; slot < MAX_SLOT; slot++)
        for (ch = 0; ch < 32; ch++) {
          map_.SCALER[crate][slot][ch] = 0;
        }
    first = 0;
  }

  mod_id = strtok( (char *)yytext, " ");

  data_type  = atoi(strtok(NULL, " "));
  crate      = atoi(strtok(NULL, " "));
  slot       = atoi(strtok(NULL, " "));
  ch         = atoi(strtok(NULL, " "));

/* In cable map file, slot starts counting from 1. 
 * As an index of an array we want it to start from 0.
 */
  slot--;

/*********************** Check if the data is valid ***********************/

  if (crate >= MAX_CRATE) {
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR, 
      "Crate number > %d EE\n", MAX_CRATE - 1);
    return 1;
  }

  if (slot >= MAX_SLOT) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Slot number > %d EE\n", MAX_SLOT);
    return 1;
  }

  if ((ch > 31) || (ch < 0)) {
    syslog(LOG_LOCAL1+LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1+LOG_ERR,
      "Module channel out of range EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

  map_.SCALER[crate][slot][ch] = data_type;

  return 0;
}


static int map_AFEB()
{
  static int first = 1;
  short      board_chip_chan, layer_chan;
  char       *feb_id, *feb_chan, *cham_chan;
  int        feb_number, chip_number, layer;
  int        feb_first, feb_last, cham_first, cham_last, tmp;
  int        ifeb_chan, icham;
  
/* Initialize the map array */
  if (first) {
    for (layer = 0; layer < NLAYER; layer++)
      for (icham = 0; icham < NWIRE; icham++)
        map_.AFEB_CHIP[layer][icham] = 0;
    first = 0;
  }

  feb_id     = strtok( (char *)yytext, " ");

  feb_number = atoi(strtok(NULL, " "));
  chip_number= atoi(strtok(NULL, " "));
  layer      = atoi(strtok(NULL, " "));

  feb_chan   = strtok(NULL, " ");
  cham_chan  = strtok(NULL, " ");

  feb_first  = atoi(strtok(feb_chan, "-"));
  feb_last   = atoi(strtok(NULL, "-"));
  
  cham_first = atoi(strtok(cham_chan, "-"));
  cham_last  = atoi(strtok(NULL, "-"));

/*printf("%s %d %d %d %d %d %d %d\n", feb_id, feb_number, chip_number, layer, feb_first, feb_last, cham_first, cham_last);*/
/*********************** Check if the data is valid ***********************/

  if ((feb_number < 1) || (feb_number > NCFEB)) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "AFEB number out of range: %d EE",  feb_number);
    return 1;
  }

  if ((layer < 0) || (layer > NLAYER)) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Layer number out of range: %d EE",  layer);
    return 1;
  }

  if (abs(feb_first - feb_last) != abs(cham_first - cham_last)) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "Number of AFEB channels != number of chamber channels EE");
    return 1;
  }

  if ((feb_last > NAFEB_CHAN) || (feb_first < 0) ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "AFEB channels out of range: first %d last %d EE", feb_first, feb_last);
    return 1;
  }

  if ((cham_last  > NSTRIP || cham_last  <= 0) || 
      (cham_first > NSTRIP || cham_first <= 0)    ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Chamber channels out of range EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

/* Make sure AFEB channels are in increasing order */
  if (feb_first > feb_last) {
    tmp       = feb_last;
    feb_last  = feb_first;
    feb_first = tmp; 

    tmp        = cham_last;
    cham_last  = cham_first;
    cham_first = tmp; 
  }

  icham = cham_first;
  for ( ifeb_chan = feb_first; ifeb_chan <= feb_last; ifeb_chan++ ) {

    board_chip_chan = (short )((feb_number  & 0x000f)<<12) + 
                      (short )((chip_number & 0x000f)<<8 ) +
                      (short ) (ifeb_chan   & 0x00ff) ;

    map_.AFEB_CHIP[layer - 1][icham - 1] = board_chip_chan;

    /* Upper 8 bits layer number and lower 8 bits channel number */
    layer_chan = (short )((layer & 0x00FF)<<8) + 
                 (short ) (icham & 0x00FF);

    map_.AFEB[feb_number - 1][ifeb_chan - 1] = layer_chan;

    if (cham_first <= cham_last)
      icham++;
    else
      icham--;

  }

  return 0;

}

static int map_CFEB()
  {
  static int first = 1;
  short layer_chan;
  char *feb_id;
  int feb_number, layer;
  int cham_chan, feb_chan;
  int nfeb;
  
/* Initialize the map array */
  if (first) 
    {
    for (nfeb = 0; nfeb < NCFEB; nfeb++)
      for (feb_chan = 0; feb_chan < NCFEB_CHAN; feb_chan++)
        map_.CFEB[nfeb][feb_chan] = 0;
    first = 0;
    }

  feb_id     = strtok( (char *)yytext, " ");

  /* do data exchange validation on-the-fly */
  if (1 == sscanf(strtok(NULL," "),"%d",&feb_number))
    {
    if ((feb_number < 1 ) || (feb_number > NCFEB))
      {
      syslog(LOG_LOCAL1|LOG_ERR, 
        "Parse error line number %d of cable map file EE", ++nlines);
      syslog(LOG_LOCAL1|LOG_ERR, 
        "CFEB number out of range: %d EE",  feb_number);
      return 1;
      }
    }
  else
    {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "BUGCHECK: First identifier not a number, %s",feb_id);
    return 1;
    }

  if (1 == sscanf(strtok(NULL," "),"%d",&layer))
    {
    if ((layer < 1 ) || (layer > NLAYER))
      {
      syslog(LOG_LOCAL1|LOG_ERR, 
        "Parse error line number %d of cable map file EE", ++nlines);
      syslog(LOG_LOCAL1|LOG_ERR, 
        "Layer number out of range: %d EE",  layer);
      return 1;
      }
    }
  else
    {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "BUGCHECK: Second identifier not a number, %s",feb_id);
    return 1;
    }

  if (1 == sscanf(strtok(NULL," "),"%d",&feb_chan))
    {
    if ((feb_chan < 1 ) || (feb_chan > NCFEB_CHAN))
      {
      syslog(LOG_LOCAL1|LOG_ERR, 
        "Parse error line number %d of cable map file EE", ++nlines);
      syslog(LOG_LOCAL1|LOG_ERR, 
        "FEB channel number out of range: %d EE",  layer);
      return 1;
      }
    }
  else
    {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "BUGCHECK: Third identifier not a number, %s",feb_id);
    return 1;
    }

  if (1 == sscanf(strtok(NULL," "),"%d",&cham_chan))
    {
    if ((cham_chan < 1 ) || (cham_chan > NSTRIP))
      {
      syslog(LOG_LOCAL1|LOG_ERR, 
        "Parse error line number %d of cable map file EE", ++nlines);
      syslog(LOG_LOCAL1|LOG_ERR, 
        "FEB channel number out of range: %d EE",  layer);
      return 1;
      }
    }
  else
    {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "BUGCHECK: Fourth identifier not a number, %s",feb_id);
    return 1;
    }

  /* Data exchange & range check complete. */

  /* Upper 8 bits layer number and lower 8 bits channel number */
  layer_chan = (short )((layer & 0x00FF)<<8) + 
               (short ) (cham_chan & 0x00FF);

  map_.CFEB[feb_number - 1][feb_chan - 1] = layer_chan;

  return 0;
  }

static int map_range_CFEB()
{
  static int first = 1;
  short layer_chan;
  char *feb_id, *feb_chan, *cham_chan;
  int feb_number, layer;
  int feb_first, feb_last, cham_first, cham_last, tmp;
  int nfeb, ifeb_chan, icham;
  
/* Initialize the map array */
  if (first) {
    for (nfeb = 0; nfeb < NCFEB; nfeb++)
      for (ifeb_chan = 0; ifeb_chan < NCFEB_CHAN; ifeb_chan++)
        map_.CFEB[nfeb][ifeb_chan] = 0;
    first = 0;
  }

  feb_id     = strtok( (char *)yytext, " ");

  feb_number = atoi(strtok(NULL, " "));
  layer      = atoi(strtok(NULL, " "));

  feb_chan   = strtok(NULL, " ");
  cham_chan  = strtok(NULL, " ");

  feb_first  = atoi(strtok(feb_chan, "-"));
  feb_last   = atoi(strtok(NULL, "-"));
  
  cham_first = atoi(strtok(cham_chan, "-"));
  cham_last  = atoi(strtok(NULL, "-"));

/*printf("%s %d %d %d %d %d %d\n", feb_id, feb_number, layer, feb_first, feb_last, cham_first, cham_last);*/
/*********************** Check if the data is valid ***********************/

  if ((feb_number < 1) || (feb_number > NCFEB)) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "CFEB number out of range: %d EE",  feb_number);
    return 1;
  }

  if ((layer < 1) || (layer > NLAYER)) {
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Layer number out of range: %d EE",  layer);
    return 1;
  }

  if (abs(feb_first - feb_last) != abs(cham_first - cham_last)) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR,
      "Number of CFEB channels != number of chamber channels EE");
    return 1;
  }

  if ((feb_last > NCFEB_CHAN) || (feb_first < 0) ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "CFEB channels out of range: first %d last %d EE", feb_first, feb_last);
    return 1;
  }

  if ((cham_last  > NSTRIP || cham_last  <= 0) || 
      (cham_first > NSTRIP || cham_first <= 0)    ) {
    syslog(LOG_LOCAL1|LOG_ERR,
      "Parse error line number %d of cable map file EE\n", ++nlines);
    syslog(LOG_LOCAL1|LOG_ERR, 
      "Chamber channels out of range EE\n");
    return 1;
  }

/* Now that we know the data is O.K. let's continue */

/* Make sure AFEB channels are in increasing order */
  if (feb_first > feb_last) {
    tmp       = feb_last;
    feb_last  = feb_first;
    feb_first = tmp; 

    tmp        = cham_last;
    cham_last  = cham_first;
    cham_first = tmp; 
  }

  icham = cham_first;
  for ( ifeb_chan = feb_first; ifeb_chan <= feb_last; ifeb_chan++ ) {

    /* Upper 8 bits layer number and lower 8 bits channel number */
    layer_chan = (short )((layer & 0x00FF)<<8) + 
                 (short ) (icham & 0x00FF);

    map_.CFEB[feb_number - 1][ifeb_chan - 1] = layer_chan;

    if (cham_first <= cham_last)
      icham++;
    else
      icham--;

  }

  return 0;

}



