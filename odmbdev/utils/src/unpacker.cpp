#include "unpacker.hpp"
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <stdexcept>
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TLegend.h"

namespace Packet{
  Unpacker::Unpacker():
    data_(0),
    cutoff_(0),
    cutoff_cached_(false),
    min_cached_(false),
    max_cached_(false),
    delta_cached_(false){
  }

  Unpacker::Unpacker(const std::vector<dcfeb_data>& data, const uint_fast32_t l1a_in, const unsigned dcfeb_in):
    data_(data),
    l1a_(l1a_in),
    dcfeb_(dcfeb_in),
    cutoff_(0),
    cutoff_cached_(false),
    min_cached_(false),
    max_cached_(false),
    delta_cached_(false){
  }

  void Unpacker::SetData(const std::vector<dcfeb_data>& data, const uint_fast32_t l1a_in, const unsigned dcfeb_in){
    data_=data;
    l1a_=l1a_in;
    dcfeb_=dcfeb_in;
    cutoff_cached_=false;
    min_cached_=false;
    max_cached_=false;
    delta_cached_=false;
  }

  unsigned Unpacker::GetCutoff() const{
    CalcCutoff();
    return cutoff_;
  }

  void Unpacker::CalcCutoff() const{
    if(!cutoff_cached_){
      std::sort(data_.begin(), data_.end(), std::greater<dcfeb_data>());

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
    CalcCutoff();
    return data_.size()?data_.at(0).first:0;
  }

  float Unpacker::GetMuonAverage() const{
    CalcCutoff();
    if(cutoff_>0){
      std::vector<uint_least16_t> plain_data(data_.size());
      for(unsigned i(0); i<data_.size(); ++i){
        plain_data.at(i)=data_.at(i).first;
      }
      return static_cast<float>(std::accumulate(plain_data.begin(), plain_data.begin()+cutoff_, 0.0))/cutoff_;
    }else{
      return 0.0;
    }
  }

  float Unpacker::GetPedestalAverage() const{
    CalcCutoff();
    if(data_.size()-cutoff_>0){
      std::vector<uint_least16_t> plain_data(data_.size());
      for(unsigned i(0); i<data_.size(); ++i){
        plain_data.at(i)=data_.at(i).first;
      }
      return static_cast<float>(std::accumulate(plain_data.begin()+cutoff_, plain_data.end(), 0.0))/(data_.size()-cutoff_);
    }else{
      return 0.0;
    }
  }

  float Unpacker::GetMuonPedestalRatio() const{
    CalcCutoff();
    return GetMuonPeak()/GetPedestalAverage();
  }

  bool Unpacker::IsNeighbor(const dcfeb_data& a, const dcfeb_data& b) const{
    const unsigned short dt(std::abs(a.second.first-b.second.first));
    const unsigned short ds(std::abs(a.second.second.first-b.second.second.first));
    const unsigned short dl(std::abs(a.second.second.second-b.second.second.second));
    return dt<=1 && ds<=1 && dl<=1;
  }

  float Unpacker::GetAverageLayer() const{
    CalcCutoff();
    const float pedestal_avg(GetPedestalAverage());

    float layer_total(0.0), weight_total(0.0);
    for(unsigned i(0); i<cutoff_; ++i){
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
    CalcCutoff();
    const float pedestal_avg(GetPedestalAverage());
    
    float strip_total(0.0), weight_total(0.0);
    for(unsigned i(0); i<cutoff_; ++i){
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
    CalcCutoff();
    const float pedestal_avg(GetPedestalAverage());

    float time_total(0.0), weight_total(0.0);
    for(unsigned i(0); i<cutoff_; ++i){
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

TString Unpacker::RoundNumber(double num, int decimals, int length){
  double neg = 1; if(num<0) neg = -1;
  num /= neg; num += 0.5*pow(10.,-decimals);
  int num_int = static_cast<int>(num);
  int num_dec = static_cast<int>((1+num-num_int)*pow(10.,decimals));
  TString s_dec = ""; s_dec += num_dec; s_dec.Remove(0,1);
  TString result=""; 
  if(neg<0) result+="-";
  result+= num_int;
  if(decimals>0) {
    result+="."; result+=s_dec;
  }
  
  TString afterdot = result;
  afterdot.Remove(0,afterdot.First(".")+1);
  for(int i=0; i<decimals-afterdot.Length(); i++)
    result += "0";

  while(result.Sizeof() < length) result = " " + result;
  return result;
}

  void Unpacker::MakePlot(std::string file){
    gStyle->SetOptStat(0);              // No Stats box
    gStyle->SetPalette(1);              // Decent colors for 2D plots
    TCanvas canvas;
    canvas.Divide(4,2);

    TH1D *h1d[8][6];
    double mean[8][6], rms[8][6];
    int colors[] = {kBlue+3, kBlue+1, kBlue-7, kGreen-7, kGreen+1, kGreen+3};
    TString hname, htitle;
    for(int layer(0); layer<6; layer++){
      for(int time(0); time<8; time++){
	mean[time][layer] = 0; rms[time][layer] = 0;
	hname = "histo"; hname += time; hname += layer; 
	htitle = "Time sample "; htitle += time+1; htitle += ";Strip;ADC count";
	h1d[time][layer] = new TH1D(hname, htitle, 16, 0.5, 16.5);
	h1d[time][layer]->SetLineColor(colors[layer]);
	h1d[time][layer]->SetLineWidth(2);
      }
    }

    TH2D h1("h1","Time Sample 1;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h2("h2","Time Sample 2;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h3("h3","Time Sample 3;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h4("h4","Time Sample 4;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h5("h5","Time Sample 5;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h6("h6","Time Sample 6;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h7("h7","Time Sample 7;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    TH2D h8("h8","Time Sample 8;Strip;Layer", 16, 0.5, 16.5, 6, 0.5, 6.5);
    double min_val(static_cast<double>(0xFFFu));
    double max_val(0.0);
    for(unsigned i(0); i<data_.size(); ++i){
      const double value(data_.at(i).first);
      const int time(data_.at(i).second.first+1);
      const int strip(data_.at(i).second.second.first+1);
      const int layer(data_.at(i).second.second.second+1);
      
      h1d[time-1][layer-1]->Fill(strip, value);
      mean[time-1][layer-1] += value;
      rms[time-1][layer-1] += value*value;
      TH2D* h(NULL);
      switch(time){
      case 1: h=&h1; break;
      case 2: h=&h2; break;
      case 3: h=&h3; break;
      case 4: h=&h4; break;
      case 5: h=&h5; break;
      case 6: h=&h6; break;
      case 7: h=&h7; break;
      case 8: h=&h8; break;
      default: h=NULL; break;
      }
      h->SetBinContent(strip, layer, value);
      if(value<min_val) min_val=value;
      if(value>max_val) max_val=value;
    }
    
    for(unsigned time(1); time<=8; ++time){
      TH2D* h(NULL);
      switch(time){
      case 1: h=&h1; break;
      case 2: h=&h2; break;
      case 3: h=&h3; break;
      case 4: h=&h4; break;
      case 5: h=&h5; break;
      case 6: h=&h6; break;
      case 7: h=&h7; break;
      case 8: h=&h8; break;
      default: h=NULL; break;
      }
      canvas.cd(time);
      h->SetMinimum(min_val);
      h->SetMaximum(max_val);
      h->SetStats(0);
      h->Draw("COLZ");      
    }
    canvas.cd(0);
    canvas.Print(file.c_str());

    // Legend
    const double legX = 0.65, legY = 0.89;
    const double legW = 0.18, legH = 0.2;
    TLegend leg(legX, legY-legH, legX+legW, legY);
    leg.SetTextSize(0.056); leg.SetFillColor(0); leg.SetFillStyle(0); leg.SetBorderSize(0);
    leg.SetTextFont(132);
    for(int layer(0); layer<6; layer++){
      htitle = "Layer "; htitle += layer+1;
      leg.AddEntry(h1d[0][layer], htitle);
    }
    std::cout<<std::endl<<std::endl<<"Time sample ->  ";
    for(int time(0); time<8; time++){     
      std::cout<<" "<<time+1<<"\t";
      canvas.cd(time+1);
      for(int layer(0); layer<6; layer++){
	if(layer==0) {
	  h1d[time][0]->SetMinimum(min_val);
	  h1d[time][0]->SetMaximum(max_val*1.1);
	  h1d[time][layer]->Draw();
	  if(time==0) leg.Draw();
	} else h1d[time][layer]->Draw("same");
      }
    }
    file.replace(0,7,"onedim");
    canvas.cd(0);
    canvas.Print(file.c_str());

    std::cout<<std::endl<<"============================================================================="<<std::endl;
    for(int layer(0); layer<6; layer++){
      std::cout<<"Layer "<<layer+1<<":   \t";
      for(int time(0); time<8; time++){
	h1d[time][layer]->Delete();
	rms[time][layer] = sqrt(16*rms[time][layer]-mean[time][layer]*mean[time][layer])/15;
	std::cout << RoundNumber(rms[time][layer],0,4).Data() << ",\t";
      }
      std::cout<<std::endl;
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
      CalcCutoff();
      min_=data_.size()?data_.at(data_.size()-1).first:0;
      min_cached_=true;
    }
    return min_;
  }

  float Unpacker::GetMax() const{
    if(!max_cached_){
      CalcCutoff();
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
    CalcCutoff();
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
          throw std::logic_error("Value not bracketed properly.");
          return -1.0;
        }
      }
    }
  }
  
  float Unpacker::GetValueOf(const float quantile) const{
    CalcCutoff();
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

  float Unpacker::normalize(const uint_least16_t val) const{
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
    return GetCutoff()>4 && GetMuonPedestalRatio()>1.1 && GetValueOf(0.5)<0.4;
  }
}
