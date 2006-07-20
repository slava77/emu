//-----------------------------------------------------------------------
// $Id: elong.cc,v 3.0 2006/07/20 21:15:48 geurts Exp $
// $Log: elong.cc,v $
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.1  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "elong.h"
#include <stdlib.h>

// clears extended long variable
void elong::clear ()
{
        int i;
        for (i = 0; i < ELLENGTH; i++) r[i] = 0;
        l = 0;      
}
//
// concatenates "bits" bits from "arg" ulong with extended long
// these bits are added into MSBs
void elong::cat (unsigned long arg, int bits)
{
        int wl = l/32;
        int bl = l%32;
        int bitsleft = bl + bits - 32;
        unsigned long mask = (bits == 32) ? (unsigned long)(-1) : (((unsigned long)1 << bits) - 1);
        arg &= mask;
        
        if (wl < ELLENGTH)
                r[wl] |= (arg << bl);
        
        if (bitsleft > 0 && wl+1 < ELLENGTH)
                r[wl+1] |= (arg >> (bits - bitsleft));
        
        l += bits;
        
}

// splits "bits" bits out of MSBs. Reduces the length of extended long by "bits"
// returns the bits as unsigned long. "bits" cannot be more than 32
unsigned long elong::split (int bits)
{
        int wb = (l - bits) / 32;
        int bb = (l - bits) % 32;
        unsigned long bmask = (bits == 32) ? (unsigned long)(-1) : (((unsigned long)1 << bits) - 1);
        unsigned long lmask = (((unsigned long)1 << (32-bb)) - 1);
        int bitsleft = bb + bits - 32;
        unsigned long ret;
        
        lmask &= bmask;
        if (wb < ELLENGTH)
                ret = (r[wb] >> bb) & lmask;
        
        if (bitsleft > 0 && wb+1 < ELLENGTH)
        {
                ret |= (r[wb+1] << (bits - bitsleft));
                ret &= bmask;           
        }
        
        l -= bits;
        return ret;
}
