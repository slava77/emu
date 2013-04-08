//
/* 
   crcdivide.c -- J. Gu, Ohio State University 6/1/99

   Produces Cyclic-Redundancy-Check (CRC) for the 96 words associated
   with a single CFEB.  This function returns the CRC when passed a pointer
   to the 96 data words.  This CRC should match the 97th word of that CFEB.

   Protocol:
      unsigned short int crcdivide(unsigned short int *datastream);

   Example:
     crc15c = crcdivide(buf);
     crc15r = buf[97];
     printf("%5x%5x%s\n", crc15r, crc15c, 
        (crc15r==crc15c) ? " Data correct":" Data corrupted");
      
*/   

unsigned short int crcdivide(unsigned short int *datastream);

/* Wrapper for calling from Fortran */
void crcdivide_(short int *datastream, unsigned short int *crc) {
  *crc = crcdivide((unsigned short int *)datastream);
  return;
}


unsigned short int crcdivide(unsigned short int *datastream)
{
  unsigned long int remainder=0;
  unsigned short int polynomial=32771;  //2**15+2+1
  unsigned short int blocksize=96, crcsize=15, crc,i,j;
  for (j=0;j<blocksize;j++)
  {
    remainder=(remainder<<13) + (datastream[j]&8191);
    for (i=13;i>0;i--)
        if (remainder & (1<<(i+crcsize-1)))  remainder ^= (polynomial<<(i-1));
  }
  // to append 15 zero at the end
  remainder=(remainder<<15);
  for (i=15;i>0;i--)
    if (remainder & (1<<(i+crcsize-1))) remainder ^= (polynomial<<(i-1));
  crc=remainder & 32767;
  return crc;
}

