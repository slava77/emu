#ifndef EmuTFtiming_h
#define EmuTFtiming_h

#include <vector>
#include <map>
#include <set>

using namespace std;

class EmuTFtiming
{
private:
  vector< pair<unsigned int,CSCSP_MEblock> > hits[5][10];
public:
  void init(void)
  {
    for (int mpc=0; mpc<5; mpc++)
      for (int csc=0; csc<10; csc++)
        hits[mpc][csc].clear();
  }
  void fill(unsigned int bx, unsigned int mpc, unsigned int csc, CSCSP_MEblock correlatedLCT)
  {
    hits[mpc][csc].push_back( pair<unsigned int,CSCSP_MEblock>(bx,correlatedLCT) );
  }
  unsigned int nHits(void)
  {
    unsigned int retval=0;
    for (int mpc=0; mpc<5; mpc++)
      for (int csc=0; csc<10; csc++)
        retval += hits[mpc][csc].size();
    return retval;
  }
  map<pair<unsigned int,unsigned int>,int> diffBX(void)
  {
    map<pair<unsigned int,unsigned int>,vector< pair<unsigned int,CSCSP_MEblock> >::const_iterator> chamber_list;
    for (int mpc=0; mpc<5; mpc++)
      for (int csc=0; csc<10; csc++)
        {
          vector< pair<unsigned int,CSCSP_MEblock> >::const_iterator first_hit = hits[mpc][csc].begin();
          if (first_hit!=hits[mpc][csc].end()) chamber_list[pair<unsigned int,unsigned int>(mpc,csc)] = first_hit;
        }
    map<pair<unsigned int,unsigned int>,int> retval;

    typedef map<pair<unsigned int,unsigned int>,vector< pair<unsigned int,CSCSP_MEblock> >::const_iterator>::const_iterator ITER;
    for (ITER hit1=chamber_list.begin(); hit1!=chamber_list.end(); hit1++)
      for (ITER hit2=hit1; hit2!=chamber_list.end(); hit2++)
        {
          if (hit2==hit1) continue;
          unsigned int unique_id1 = hit1->first.first*10+hit1->first.second;
          unsigned int unique_id2 = hit2->first.first*10+hit2->first.second;
          retval[pair<unsigned int,unsigned int>(unique_id1,unique_id2)] = hit1->second->first - hit2->second->first;
        }
    return retval;
  }
};

#endif
