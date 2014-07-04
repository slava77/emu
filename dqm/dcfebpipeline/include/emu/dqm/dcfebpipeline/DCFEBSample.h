#ifndef __DCFEBSample_h__
#define __DCFEBSample_h__

#include <vector>

using namespace std;

    struct DCFEBSampleId{
      // DCFEBSampleId( unsigned int crate, unsigned int slot, unsigned int cfeb_number, unsigned int depth );
      DCFEBSampleId( unsigned int cfeb_number, unsigned int depth );
      bool operator<( const DCFEBSampleId& other ) const;
      // unsigned int crate;
      // unsigned int slot;
      unsigned int cfeb;
      unsigned int pipelineDepth;
    };

    class DCFEBSample{
    public:
      DCFEBSample();
      DCFEBSample( const vector<int>& sample );
      DCFEBSample& add( const vector<int>& sample );
      size_t getNSamples() const { return nSamples_; }
      vector<int> getSample() const { return sample_; }
      vector<double> getAverageSample() const;
    private:
      vector<int> sample_;
      size_t      nSamples_;
    };

#endif
