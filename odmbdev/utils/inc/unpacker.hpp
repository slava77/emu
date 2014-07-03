#ifndef H_UNPACKER
#define H_UNPACKER

#include <vector>
#include <string>
#include <utility>
#include "utils.hpp"
#include "TString.h"

namespace Packet{
  class Unpacker{
  public:
    Unpacker();
    Unpacker(const std::vector<dcfeb_data>& data, const uint_fast32_t l1a_in, const unsigned dcfeb_in);
    void SetData(const std::vector<dcfeb_data>& data, const uint_fast32_t l1a_in, const unsigned dcfeb_in);

    unsigned GetCutoff() const;
    float GetMuonPeak() const;
    float GetMuonAverage() const;
    float GetPedestalAverage() const;
    float GetAverageLayer() const;
    float GetAverageStrip() const;
    float GetAverageTimeBin() const;
    float GetMuonPedestalRatio() const;
    float GetMin() const;
    float GetMax() const;
    float GetDelta() const;
    float GetQuantileOf(const float value) const;
    float GetValueOf(const float quantile) const;
    std::vector<float> GetValues() const;
    std::vector<float> GetQuantiles() const;
    std::vector<std::pair<float, float> > GetValuesAndQuantiles() const;
    bool LooksLikeAMuon() const;

    const uint_fast32_t& l1a() const{return l1a_;}
    uint_fast32_t& l1a(){return l1a_;}

    const unsigned& dcfeb() const{return dcfeb_;}
    unsigned& dcfeb(){return dcfeb_;}

    TString RoundNumber(double num, int decimals, int length);
    void MakePlot(std::string file);
    void PrintData() const;

  private:
    mutable std::vector<dcfeb_data> data_;
    uint_fast32_t l1a_;
    unsigned dcfeb_;
    mutable unsigned cutoff_, min_, max_, delta_;
    mutable bool cutoff_cached_, min_cached_, max_cached_, delta_cached_;

    void CalcCutoff() const;
    bool IsNeighbor(const dcfeb_data& a, const dcfeb_data&b) const;
    float normalize(const uint_least16_t val) const;
  }; 
}
#endif
