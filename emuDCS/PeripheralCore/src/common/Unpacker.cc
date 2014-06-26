#include "emu/pc/Unpacker.h"
#include <vector>
#include <string>
#include <utility>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <cmath>

namespace emu{
  namespace pc{
    namespace Packet{
      Unpacker::Unpacker():
        data_(0),
        cutoff_(0),
        cutoff_cached_(false),
	data_sorted_(false),
        min_cached_(false),
        max_cached_(false),
        delta_cached_(false){
      }

      Unpacker::Unpacker(const std::vector<dcfeb_data>& data, const unsigned l1a_in, const unsigned dcfeb_in):
        data_(data),
        l1a_(l1a_in),
        dcfeb_(dcfeb_in),
        cutoff_(0),
        cutoff_cached_(false),
	data_sorted_(false),
        min_cached_(false),
        max_cached_(false),
        delta_cached_(false){
      }

      Unpacker::Unpacker(const std::vector<unsigned>& data, const unsigned l1a_in, const unsigned dcfeb_in):
        data_(TranslateRawWords(data)),
        l1a_(l1a_in),
        dcfeb_(dcfeb_in),
        cutoff_(0),
        cutoff_cached_(false),
	data_sorted_(false),
        min_cached_(false),
        max_cached_(false),
        delta_cached_(false){
      }

      void Unpacker::SetData(const std::vector<dcfeb_data>& data, const unsigned l1a_in, const unsigned dcfeb_in){
        data_=data;
        l1a_=l1a_in;
        dcfeb_=dcfeb_in;
        cutoff_cached_=false;
	data_sorted_=false;
        min_cached_=false;
        max_cached_=false;
        delta_cached_=false;
      }

      void Unpacker::SetData(const std::vector<unsigned>& data, const unsigned l1a_in, const unsigned dcfeb_in){
        data_=TranslateRawWords(data);
        l1a_=l1a_in;
        dcfeb_=dcfeb_in;
        cutoff_cached_=false;
	data_sorted_=false;
        min_cached_=false;
        max_cached_=false;
        delta_cached_=false;
      }

      unsigned Unpacker::GetCutoff() const{
        CalcCutoff();
        return cutoff_;
      }

      void Unpacker::SortData() const{
	if(!data_sorted_){
	  std::sort(data_.begin(), data_.end(), std::greater<dcfeb_data>());
	  data_sorted_=true;
	}
      }

      void Unpacker::CalcCutoff() const{
        if(!cutoff_cached_){
	  SortData();

          for(unsigned cut(1); cut<=64 && cut<=data_.size(); ++cut){
            std::vector<bool> part_of_set(cut, false);
            part_of_set.at(0)=true;
            bool last_changed(true);
            while(last_changed){
              last_changed=false;

              for(unsigned check(0); check<cut; ++check){
                if(!part_of_set.at(check)) continue;
                for(unsigned to_add(0); to_add<cut; ++to_add){
                  if(!part_of_set.at(to_add) && IsNeighbor(data_.at(to_add), data_.at(check))){
                    part_of_set.at(to_add)=true;
                    last_changed=true;
                  }
                }
              }
            }
            bool all_in(true);
            for(unsigned i(0); i<part_of_set.size(); ++i){
              if(!part_of_set.at(i)) all_in=false;
            }
            if(all_in) cutoff_=cut;
          }

          cutoff_cached_=true;
        }
      }

      float Unpacker::GetMuonPeak() const{
	return GetMax();
      }

      float Unpacker::GetMuonAverage() const{
	return GetAverageOf(4, 12);
      }

      float Unpacker::GetPedestalAverage() const{
	return GetAverageOf(512, 640);
      }

      float Unpacker::GetMuonPedestalRatio() const{
        return GetMuonPeak()/GetPedestalAverage();
      }

      bool Unpacker::IsNeighbor(const dcfeb_data& a, const dcfeb_data& b) const{
        const unsigned short dt((a.second.first>b.second.first)?a.second.first-b.second.first:b.second.first-a.second.first);
        const unsigned short ds((a.second.second.first>b.second.second.first)?a.second.second.first-b.second.second.first:b.second.second.first-a.second.second.first);
        const unsigned short dl((a.second.second.second>b.second.second.second)?a.second.second.second-b.second.second.second:b.second.second.second-a.second.second.second);
        return dt<=1 && ds<=1 && dl<=1;
      }

      float Unpacker::GetAverageLayer() const{
	SortData();
        const float pedestal_avg(GetPedestalAverage());

        float layer_total(0.0), weight_total(0.0);
        for(unsigned i(0); i<12; ++i){
          const float delta(data_.at(i).first-pedestal_avg);
          layer_total+=data_.at(i).second.second.second*delta;
          weight_total+=delta;
        }
        if(weight_total>0.0){
          return 1.0+layer_total/weight_total;
        }else{
          return 0.0;
        }
      }

      float Unpacker::GetAverageStrip() const{
	SortData();
        const float pedestal_avg(GetPedestalAverage());
    
        float strip_total(0.0), weight_total(0.0);
        for(unsigned i(0); i<12; ++i){
          const float delta(data_.at(i).first-pedestal_avg);
          strip_total+=data_.at(i).second.second.first*delta;
          weight_total+=delta;
        }
        if(weight_total>0.0){
          return 1.0+strip_total/weight_total;
        }else{
          return 0.0;
        }
      }

      float Unpacker::GetAverageTimeBin() const{
	SortData();
        const float pedestal_avg(GetPedestalAverage());

        float time_total(0.0), weight_total(0.0);
        for(unsigned i(0); i<4; ++i){
          const float delta(data_.at(i).first-pedestal_avg);
          time_total+=data_.at(i).second.first*delta;
          weight_total+=delta;
        }
        if(weight_total>0.0){
          return 1.0+time_total/weight_total;
        }else{
          return 0.0;
        }
      }

      void Unpacker::PrintData() const{
        CalcCutoff();
        for(unsigned i(0); i<data_.size() && i<cutoff_; ++i){
          std::cout << ' ' << std::setw(8) << i
                    << ' ' << std::setw(8) << std::hex << static_cast<unsigned>(data_.at(i).first) << std::dec
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).first)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.first)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.second.first)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.second.second)
                    << std::endl;
        }
        for(unsigned i(0); i<54; ++i) std::cout << '-';
        std::cout << std::endl;
        for(unsigned i(cutoff_); i<data_.size(); ++i){
          std::cout << ' ' << std::setw(8) << i
                    << ' ' << std::setw(8) << std::hex << static_cast<unsigned>(data_.at(i).first) << std::dec
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).first)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.first+1)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.second.first+1)
                    << ' ' << std::setw(8) << static_cast<unsigned>(data_.at(i).second.second.second+1)
                    << std::endl;
        }
      }

      float Unpacker::GetMin() const{
        if(!min_cached_){
	  SortData();
          min_=data_.size()?data_.at(data_.size()-1).first:0;
          min_cached_=true;
        }
        return min_;
      }

      float Unpacker::GetMax() const{
        if(!max_cached_){
	  SortData();
          max_=data_.size()?data_.at(0).first:0;
          max_cached_=true;
        }
        return max_;
      }

      float Unpacker::GetDelta() const{
        if(!delta_cached_){
          const float min(GetMin());
          const float max(GetMax());
          delta_=(max>min)?static_cast<unsigned>(max-min):0;
          delta_cached_=true;
        }
        return delta_;
      }

      float Unpacker::GetQuantileOf(const float value) const{
	SortData();
        switch(data_.size()){
        case 0: return 0.5;
        case 1:
          if(value<normalize(data_.at(0).first)){
            return 0.0;
          }else if(value>normalize(data_.at(0).first)){
            return 1.0;
          }else{
            return 0.5;
          }
        default:
          if(value<normalize(data_.at(data_.size()-1).first)){
            return 0.0;
          }else if(value>normalize(data_.at(0).first)){
            return 1.0;
          }else{
            unsigned i(0), j(data_.size()-1);
            while(i<data_.size() && normalize(data_.at(i).first)>value) ++i;
            if(i==data_.size() || (i>0 && normalize(data_.at(i).first)<value)) --i;
            while(j>0 && normalize(data_.at(j).first)<value) --j;
            if(normalize(data_.at(j).first)>value && j<data_.size()-1) ++j;
            if(normalize(data_.at(i).first)==value && normalize(data_.at(j).first)==value){
              return 1.0-0.5*(i+j)/(data_.size()-1);
            }else if(normalize(data_.at(i).first)>value && normalize(data_.at(j).first)<value){
              const float xi((value-normalize(data_.at(j).first))/(normalize(data_.at(i).first)-normalize(data_.at(j).first)));
              return 1.0-(xi*i+(1.0-xi)*j)/(data_.size()-1);
            }else{
              return -1.0;
            }
          }
        }
      }
  
      float Unpacker::GetValueOf(const float quantile) const{
	SortData();
        const float delta(GetDelta());
        if(data_.size()==0 || delta==0.0){
          return 0.5;
        }else{
          if(quantile<=0.0){
            return normalize(data_.at(data_.size()-1).first);
          }else if(quantile>=1.0){
            return normalize(data_.at(0).first);
          }else{
            const float index((1.0-quantile)*(data_.size()-1));
            const unsigned fl(static_cast<unsigned>(floor(index)));
            const unsigned ce(static_cast<unsigned>(ceil(index)));
            if(fl==ce || normalize(data_.at(fl).first)==normalize(data_.at(ce).first)){
              return normalize(data_.at(fl).first);
            }else{
              const float xce((index-fl)/(ce-fl));
              return xce*normalize(data_.at(ce).first)+(1.0-xce)*normalize(data_.at(fl).first);
            }
          }
        }
      }

      float Unpacker::GetAverageOf(unsigned low, unsigned high) const{
	SortData();
	if(high>data_.size()) high=data_.size();
	if(low>=high) return 0.0;
	float total(0.0);
	for(unsigned i(low); i<high; ++i){
	  total+=data_.at(i).first;
	}
	return total/(high-low);
      }

      float Unpacker::normalize(const unsigned val) const{
        return (GetDelta()>0.0)?(val-GetMin())/GetDelta():0.5;
      }

      std::vector<std::pair<float, float> > Unpacker::GetValuesAndQuantiles() const{
        std::vector<std::pair<float, float> > ret(data_.size(), std::pair<float, float>(0.0, 0.0));
        if(data_.size()==1){
          ret=std::vector<std::pair<float, float> >(1, std::pair<float, float>(normalize(data_.at(0).first), 0.5));
        }else{
          for(unsigned i(0); i<data_.size(); ++i){
            ret.at(i)=std::pair<float, float>(normalize(data_.at(i).first), 1.0-static_cast<float>(i)/(data_.size()-1));
          }
        }
        return ret;
      }

      std::vector<float> Unpacker::GetValues() const{
        std::vector<float> values(data_.size(),0.0);
        for(unsigned i(0); i<data_.size(); ++i){
          values.at(i)=normalize(data_.at(i).first);
        }
        return values;
      }

      std::vector<float> Unpacker::GetQuantiles() const{
        std::vector<float> quantiles(data_.size(),0.0);
        if(data_.size()==1){
          quantiles.at(0)=0.5;
        }else{
          for(unsigned i(0); i<data_.size(); ++i){
            quantiles.at(i)=static_cast<float>(i)/(data_.size()-1);
          }
        }
        return quantiles;
      }

      bool Unpacker::LooksLikeAMuon() const{
	SortData();
        return data_.size()==768 && normalize(data_.at(127).first)<0.5 && normalize(data_.at(4).first)>0.5;
      }

      std::vector<dcfeb_data> Unpacker::TranslateRawWords(const std::vector<unsigned>& data){
        if(data.size()>=32){
          std::vector<dcfeb_data> output(data.size()-32);
          unsigned data_index(0), output_index(0);
          unsigned time_sample(0), strip(0), layer(0);
          for(time_sample=0; time_sample<8 && data_index<data.size() && output_index<output.size(); ++time_sample){
            for(strip=0; strip<16 && data_index<data.size() && output_index<output.size(); ++strip){
              for(layer=0; layer<6 && data_index<data.size() && output_index<output.size(); ++layer){
                data_index=100*time_sample+6*strip+layer;
                const unsigned word(data.at(data_index) & 0xFFFu);
                output.at(output_index)=std::make_pair(word,
                                                       std::make_pair(time_sample,
                                                                      std::make_pair(strip, layer)));
                ++output_index;
              }
            }
          }
          return output;
        }else{
          return std::vector<dcfeb_data>(0);
        }
      }
    } // namespace emu::pc::packet
  } // namespace emu::pc
} // namespace emu

