#include "data_packet.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdint.h>

namespace Packet{
  bool AllInRange(const svu& vec, const unsigned start, const unsigned end,
                  const uint16_t low, const uint16_t high){
    bool all_in_range(true);
    for(unsigned index(start); index<vec.size() && index<end && all_in_range; ++index){
      if(!InRange(vec.at(index), low, high)) all_in_range=false;
    }
    return all_in_range;
  }

  void DataPacket::PrintBuffer(const svu& buffer, const unsigned words_per_line,
                               const unsigned start, const bool text_mode) const{
    std::cout << std::hex << std::setfill('0');
    for(unsigned index(0); index<buffer.size(); ++index){
      if(index && !(index%words_per_line)) std::cout << std::endl;
      if(colorize_.at(index+start)){
        if(text_mode){
          std::cout << std::setw(4) << buffer.at(index) << " ";
        }else{
          std::cout << io::bold << io::bg_blue << io::fg_yellow
                    << std::setw(4) << buffer.at(index) << io::normal << " ";
        }
      }else{
        if(text_mode){
          std::cout << std::setw(4) << buffer.at(index) << " ";
        }else{
          std::cout << io::normal << std::setw(4) << buffer.at(index) << " ";
        }
      }
    }
    if(text_mode){
      std::cout << std::endl;
    }else{
      std::cout << io::normal << std::endl;
    }
  }

  void PutInRange(unsigned& a, unsigned& b, const unsigned min, const unsigned max){
    if(max<=min){
      a=max;
      b=max;
    }else{
      if(a<min || a>max) a=min;
      if(b<min || b>max) b=min;
      if(b<a) a=b;
    }
  }

  bool GetBit(const unsigned x, const unsigned bit){
    return (x>>bit) & 1u;
  }

  void PrintWithStars(const std::string& text, const unsigned full_width){
    const unsigned spares(full_width-text.length());
    if(spares>=2){
      for(unsigned i(0); i<3 && i<spares-1; ++i) std::cout << '*';
      std::cout << ' ' << text;
      if(spares>=6){
        std::cout << ' ';
        for(unsigned i(0); i<spares-5; ++i) std::cout << '*';
      }
      std::cout << std::endl;
    }else{
      std::cout << text << std::endl;
    }
  }

  DataPacket::DataPacket():
    full_packet_(0),
    colorize_(),
    ddu_header_start_(-1), ddu_header_end_(-1),
    odmb_header_start_(0), odmb_header_end_(0),
    alct_start_(0), alct_end_(0),
    otmb_start_(0), otmb_end_(0),
    dcfeb_start_(0), dcfeb_end_(0),
    odmb_trailer_start_(0), odmb_trailer_end_(0),
    ddu_trailer_start_(-1), ddu_trailer_end_(-1),
    parsed_(false){
  }

  DataPacket::DataPacket(const svu& packet_in):
    full_packet_(packet_in),
    colorize_(0),
    ddu_header_start_(-1), ddu_header_end_(-1),
    odmb_header_start_(0), odmb_header_end_(0),
    alct_start_(0), alct_end_(0),
    otmb_start_(0), otmb_end_(0),
    dcfeb_start_(0), dcfeb_end_(0),
    odmb_trailer_start_(0), odmb_trailer_end_(0),
    ddu_trailer_start_(-1), ddu_trailer_end_(-1),
    parsed_(false){
  }

  void DataPacket::SetData(const svu& packet_in){
    parsed_=false;
    colorize_=std::vector<bool>(packet_in.size(), false);
    full_packet_=packet_in;
  }

  svu DataPacket::GetData() const{
    return full_packet_;
  }

  svu DataPacket::GetDDUHeader() const{
    Parse();
    return GetComponent(ddu_header_start_, ddu_header_end_);
  }

  svu DataPacket::GetODMBHeader(const unsigned i) const{
    Parse();
    return GetComponent(odmb_header_start_.at(i), odmb_header_end_.at(i));
  }

  svu DataPacket::GetALCTData(const unsigned i) const{
    Parse();
    return GetComponent(alct_start_.at(i), alct_end_.at(i));
  }

  svu DataPacket::GetOTMBData(const unsigned i) const{
    Parse();
    return GetComponent(otmb_start_.at(i), otmb_end_.at(i));
  }

  std::vector<svu> DataPacket::GetDCFEBData(const unsigned i) const{
    Parse();
    std::vector<svu> data(0);
    for(unsigned dcfeb(0); dcfeb<dcfeb_start_.at(i).size(); ++dcfeb){
      data.push_back(GetComponent(dcfeb_start_.at(i).at(dcfeb), dcfeb_end_.at(i).at(dcfeb)));
    }
    return data;
  }

  svu DataPacket::GetODMBTrailer(const unsigned i) const{
    Parse();
    return GetComponent(odmb_trailer_start_.at(i), odmb_trailer_end_.at(i));
  }

  svu DataPacket::GetDDUTrailer() const{
    Parse();
    return GetComponent(ddu_trailer_start_, ddu_trailer_end_);
  }

  svu DataPacket::GetComponent(const unsigned start, const unsigned end) const{
    if(start<=end && end<=full_packet_.size()){
      return svu(full_packet_.begin()+start, full_packet_.begin()+end);
    }else{
      return svu(0);
    }
  }

  DataPacket::ErrorType DataPacket::GetPacketType() const{
    Parse();
    return static_cast<ErrorType>((HasL1AMismatch()?kL1AMismatch:kGood)
                                  | (HasUncategorizedWords()?kUncategorizedWords:kGood)
                                  | GetDDUStatus());
  }
  
  void DataPacket::Parse() const{
    if(!parsed_){
      FindDDUHeader();
      FindDDUTrailer();
      FindAllODMBHeadersAndTrailers();
      FixNumberOfODMBPackets();
      for(unsigned packet(0); packet<odmb_header_start_.size(); ++packet){
        FindALCTandOTMBData(packet);
        FindDCFEBData(packet);
      }
      parsed_=true;
    }
  }

  void DataPacket::FixNumberOfODMBPackets() const{
    if(odmb_trailer_start_.size()<odmb_header_start_.size()){
      odmb_trailer_start_.push_back(ddu_trailer_start_);
      odmb_trailer_end_.push_back(ddu_trailer_start_);
    }
    alct_start_.resize(odmb_header_start_.size());
    alct_end_.resize(odmb_header_start_.size());
    otmb_start_.resize(odmb_header_start_.size());
    otmb_end_.resize(odmb_header_start_.size());
    dcfeb_start_.resize(odmb_header_start_.size());
    dcfeb_end_.resize(odmb_header_start_.size());
  }

  void DataPacket::FindDDUHeader() const{
    ddu_header_start_=0;
    ddu_header_end_=0;
    for(unsigned index(0); index+11<full_packet_.size(); ++index){
      if(full_packet_.at(index+5)==0x8000
         && full_packet_.at(index+6)==0x0001
         && full_packet_.at(index+7)==0x8000){
        ddu_header_start_=index;
        ddu_header_end_=index+12;
        colorize_.at(index+5)=true;
        colorize_.at(index+6)=true;
        colorize_.at(index+7)=true;
        return;
      }
    }
  }

  bool DataPacket::FindODMBHeader(const unsigned low, const unsigned high) const{
    const unsigned upper(full_packet_.size()<high?full_packet_.size():high);
    for(unsigned index(low); index+7<upper; ++index){
      if(InRange(full_packet_.at(index), 0x9000, 0x9FFF)
         && InRange(full_packet_.at(index+1), 0x9000, 0x9FFF)
         && InRange(full_packet_.at(index+2), 0x9000, 0x9FFF)
         && InRange(full_packet_.at(index+3), 0x9000, 0x9FFF)
         && InRange(full_packet_.at(index+4), 0xA000, 0xAFFF)
         && InRange(full_packet_.at(index+5), 0xA000, 0xAFFF)
         && InRange(full_packet_.at(index+6), 0xA000, 0xAFFF)){
        odmb_header_start_.push_back(index);
        odmb_header_end_.push_back(index+8);
        for(unsigned disp(0); disp<7; ++disp){
          colorize_.at(index+disp)=true;
        }
        if(InRange(full_packet_.at(index+7), 0xA000, 0xAFFF)){
          colorize_.at(index+7)=true;
        }
        return true;
      }
    }
    return false;
  }
  
  void DataPacket::FindAllODMBHeadersAndTrailers() const{
    odmb_header_start_.clear();
    odmb_header_end_.clear();
    odmb_trailer_start_.clear();
    odmb_trailer_end_.clear();
    bool at_end(!FindODMBHeader(ddu_header_end_, ddu_trailer_start_));
    if(!at_end){
      at_end=!FindODMBTrailer(odmb_header_end_.at(0), ddu_trailer_start_);
      bool header(true);
      bool printed(false);
      while(!at_end){
        const unsigned low(odmb_header_end_.at(odmb_header_end_.size()-1));
        const unsigned high(ddu_trailer_start_);
        if(!printed){
          printed=true;
        }
        at_end|=(header?(!FindODMBHeader(low, high)):(!FindODMBTrailer(low, high)));
        header=!header;
      }
    }
  }
  
  void DataPacket::FindALCTandOTMBData(const unsigned packet) const{
    const unsigned low(odmb_header_end_.at(packet));
    const unsigned high(odmb_trailer_start_.at(packet));
    const unsigned d_run_threshhold(3);
    const unsigned bad_index(-1);
    alct_start_.at(packet)=bad_index;
    alct_end_.at(packet)=bad_index;
    otmb_start_.at(packet)=bad_index;
    otmb_end_.at(packet)=bad_index;
    unsigned d_run_start_1(bad_index), d_run_end_1(bad_index);
    unsigned d_run_start_2(bad_index), d_run_end_2(bad_index);
    unsigned d_run_start_3(bad_index), d_run_end_3(bad_index);
    const bool found_one(FindRunInRange(d_run_start_1, d_run_end_1, low, high, d_run_threshhold, 0xD000, 0xDFFF));
    const bool found_two(FindRunInRange(d_run_start_2, d_run_end_2, d_run_end_1, high, d_run_threshhold,
                                        0xD000, 0xDFFF));
    const bool found_three(FindRunInRange(d_run_start_3, d_run_end_3, d_run_end_2, high, d_run_threshhold,
                                          0xD000, 0xDFFF));
    if(!found_three){
      if(!found_two){
        if(found_one){
          alct_start_.at(packet)=d_run_start_1;
          alct_end_.at(packet)=d_run_end_1;
          otmb_start_.at(packet)=alct_end_.at(packet);
          otmb_end_.at(packet)=alct_end_.at(packet);
        }else{
          alct_start_.at(packet)=low;
          alct_end_.at(packet)=low;
          otmb_start_.at(packet)=low;
          otmb_end_.at(packet)=low;
        }
        if(FindRunInRange(d_run_start_2, d_run_end_2, d_run_end_1, high, d_run_threshhold, 0xB000, 0xBFFF)){
          //Dummy data (ALCT is n D-words straight, OTMB n B-words straight)
          otmb_start_.at(packet)=d_run_start_2;
          otmb_end_.at(packet)=d_run_end_2;
        }
      }else{
        alct_start_.at(packet)=d_run_start_1;
        alct_end_.at(packet)=d_run_end_2;
        otmb_start_.at(packet)=alct_end_.at(packet);
        otmb_end_.at(packet)=alct_end_.at(packet);
      }
    }else{
      alct_start_.at(packet)=d_run_start_1;
      otmb_start_.at(packet)=SplitALCTandOTMB(d_run_start_2, d_run_end_2);
      alct_end_.at(packet)=otmb_start_.at(packet);
      otmb_end_.at(packet)=d_run_end_3;
    }
    
    for(unsigned index(d_run_start_1);
        index<full_packet_.size() && index<d_run_end_1;
        ++index){
      colorize_.at(index)=true;
    }
    for(unsigned index(d_run_start_2);
        index<full_packet_.size() && index<d_run_end_2;
        ++index){
      colorize_.at(index)=true;
    }
    for(unsigned index(d_run_start_3);
        index<full_packet_.size() && index<d_run_end_3;
        ++index){
      colorize_.at(index)=true;
    }
  }

  void DataPacket::FindDCFEBData(const unsigned packet) const{
    const unsigned low(otmb_end_.at(packet));
    const unsigned high(odmb_trailer_start_.at(packet));
    const unsigned upper_bound(high<full_packet_.size()?high:full_packet_.size());
    dcfeb_start_.at(packet).clear();
    dcfeb_end_.at(packet).clear();
    std::vector<unsigned> temp_dcfeb_end(0);
    for(unsigned index(low+99); index<upper_bound; ++index){
      const bool good_here((InRange(full_packet_.at(index-2), 0x7000u, 0x700Fu)
                            || InRange(full_packet_.at(index-2), 0x7000u, 0x7FFFu))
                           && InRange(full_packet_.at(index-1), 0x7000u, 0x7FFFu));
      const bool good_earlier(index<low+102
                              || ((InRange(full_packet_.at(index-102), 0x7000u, 0x700Fu)
                                   || InRange(full_packet_.at(index-102), 0x7000u, 0x7FFFu))
                                  && InRange(full_packet_.at(index-101), 0x7000u, 0x7FFFu))
                              || (InRange(full_packet_.at(index-102), 0xD000u, 0xDFFFu)
                                  && InRange(full_packet_.at(index-101), 0xD000u, 0xDFFFu)));
      const bool found_7fff(full_packet_.at(index)==0x7FFFu);

      if(good_here && (good_earlier || found_7fff)){
        temp_dcfeb_end.push_back(index+1);
        colorize_.at(index-2)=true;
        colorize_.at(index-1)=true;
        if(found_7fff) colorize_.at(index)=true;
        index+=99;
      }
    }
    for(unsigned dcfeb(7); dcfeb<temp_dcfeb_end.size(); dcfeb+=8){
      dcfeb_start_.at(packet).push_back(temp_dcfeb_end.at(dcfeb-7)-100);
      dcfeb_end_.at(packet).push_back(temp_dcfeb_end.at(dcfeb));
    }
  }

  bool DataPacket::FindODMBTrailer(const unsigned low, const unsigned high) const{
    const unsigned upper(high<full_packet_.size()?high:full_packet_.size());
    for(unsigned index(low); index+7<upper; ++index){
      if(InRange(full_packet_.at(index), 0xF000, 0xFFFF)
         && InRange(full_packet_.at(index+1), 0xF000, 0xFFFF)
         && InRange(full_packet_.at(index+2), 0xF000, 0xFFFF)
         && InRange(full_packet_.at(index+3), 0xF000, 0xFFFF)
         && InRange(full_packet_.at(index+4), 0xE000, 0xEFFF)
         && InRange(full_packet_.at(index+5), 0xE000, 0xEFFF)
         && InRange(full_packet_.at(index+6), 0xE000, 0xEFFF)){
        odmb_trailer_start_.push_back(index);
        odmb_trailer_end_.push_back(index+8);
        for(unsigned disp(0); disp<7; ++disp){
          colorize_.at(index+disp)=true;
        }
        if(InRange(full_packet_.at(index+7), 0xE000, 0xEFFF)){
          colorize_.at(index+7)=true;
        }
        return true;
      }
    }
    return false;
  }

  void DataPacket::FindDDUTrailer() const{
    ddu_trailer_start_=full_packet_.size();
    ddu_trailer_end_=full_packet_.size();
    for(unsigned index(0); index+11<full_packet_.size(); ++index){
      if(full_packet_.at(index)==0x8000
         && full_packet_.at(index+1)==0x8000
         && full_packet_.at(index+2)==0xFFFF
         && full_packet_.at(index+3)==0x8000){
        ddu_trailer_start_=index;
        ddu_trailer_end_=index+12;
        for(unsigned disp(0); disp<4; ++disp){
          colorize_.at(index+disp)=true;
        }
      }
    }
  }

  bool DataPacket::FindRunInRange(unsigned& start, unsigned& end,
                                  const unsigned left, const unsigned right,
                                  const unsigned min_length, const uint16_t low,
                                  const uint16_t high) const{
    const unsigned upper_bound(right<full_packet_.size()?right:full_packet_.size());
    bool found(false);
    start=left;
    end=left;
    unsigned index(left);
    for(; index+min_length<=upper_bound; ++index){
      if(AllInRange(full_packet_, index, index+min_length, low, high)){
        start=index;
        end=upper_bound;
        found=true;
        break;
      }
    }
    for(index=index+min_length; index<upper_bound; ++index){
      if(!InRange(full_packet_.at(index), low, high)){
        end=index;
        break;
      }
    }
    return found;
  }

  unsigned DataPacket::SplitALCTandOTMB(const unsigned start,
                                        const unsigned end) const{
    if(start<end){
      for(unsigned index(start); index<end && index<full_packet_.size(); ++index){
        if(full_packet_.at(index)==0xDB0C) return index;
      }
      for(unsigned index(start); index<end && index<full_packet_.size(); ++index){
        if(full_packet_.at(index)==0xDB0A) return index;
      }
      return static_cast<unsigned>(ceil(start+0.5*(end-start)));
    }else{
      return end;
    }
  }

  void DataPacket::PrintODMB(const std::string& uncat,
                             const unsigned odmb,
                             const unsigned words_per_line,
                             const uint_fast64_t kill_mask,
                             const unsigned text_mode) const{
    const std::string dcfeb_text(GetDCFEBText(odmb));
    std::ostringstream oss("");
    const bool is_odmb(GetDMBType().at(odmb));
    if(is_odmb){
      oss << "ODMB Header ";
    }else{
      oss << "DMB Header ";
    }
    oss << odmb+1 << "; " << dcfeb_text;
    if(GetBit(kill_mask, 3)){
      PrintComponent(oss.str(), odmb_header_start_.at(odmb),
                     odmb_header_end_.at(odmb), words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 0)){
      PrintComponent(uncat, odmb_header_end_.at(odmb), alct_start_.at(odmb),
                     words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 2)){
      PrintComponent("ALCT", alct_start_.at(odmb), alct_end_.at(odmb), words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 0)){
      PrintComponent(uncat, alct_end_.at(odmb), otmb_start_.at(odmb), words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 2)){
      PrintComponent("OTMB", otmb_start_.at(odmb), otmb_end_.at(odmb), words_per_line, text_mode);
    }
    const unsigned num_dcfebs(dcfeb_start_.at(odmb).size());
    if(num_dcfebs>0){
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, otmb_end_.at(odmb), dcfeb_start_.at(odmb).at(0),
                       words_per_line, text_mode);
      }
      for(unsigned dcfeb(0); dcfeb+1<num_dcfebs; ++dcfeb){
        std::ostringstream oss2("");
        oss2 << "DCFEB " << dcfeb+1;
        if(GetBit(kill_mask, 1)){
          PrintComponent(oss2.str(), dcfeb_start_.at(odmb).at(dcfeb),
                         dcfeb_end_.at(odmb).at(dcfeb), words_per_line, text_mode);
        }
        if(GetBit(kill_mask, 0)){
          PrintComponent(uncat, dcfeb_end_.at(odmb).at(dcfeb),
                         dcfeb_start_.at(odmb).at(dcfeb+1), words_per_line, text_mode);
        }
      }
      std::ostringstream oss2("");
      oss2 << "DCFEB " << num_dcfebs;
      if(GetBit(kill_mask, 1)){
        PrintComponent(oss2.str(), dcfeb_start_.at(odmb).at(num_dcfebs-1),
                       dcfeb_end_.at(odmb).at(num_dcfebs-1), words_per_line, text_mode);
      }
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, dcfeb_end_.at(odmb).at(num_dcfebs-1),
                       odmb_trailer_start_.at(odmb), words_per_line, text_mode);
      }
    }else{
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, otmb_end_.at(odmb), odmb_trailer_start_.at(odmb),
                       words_per_line, text_mode);
      }
    }
    if(is_odmb){
      if(GetBit(kill_mask, 3)){
        PrintComponent("ODMB Trailer", odmb_trailer_start_.at(odmb),
                       odmb_trailer_end_.at(odmb), words_per_line, text_mode);
      }
    }else{
      if(GetBit(kill_mask, 3)){
        PrintComponent("DMB Trailer", odmb_trailer_start_.at(odmb),
                       odmb_trailer_end_.at(odmb), words_per_line, text_mode);
      }
    }
  }
  
  void DataPacket::Print(const unsigned words_per_line,
                         const unsigned entry,
                         const uint_fast64_t kill_mask,
                         const bool text_mode) const{
    Parse();

    std::ostringstream oss_uncat("");
    if(text_mode){
      oss_uncat << "Uncategorized";
    }else{
      oss_uncat << io::bold << io::bg_red << io::fg_white << "Uncategorized" << io::normal;
    }
    const std::string uncat(oss_uncat.str());
    std::ostringstream event_text("");
    event_text << "Event " << std::dec << entry << " (0x" << std::hex << entry << std::dec << ')';
    const std::string l1a_text(GetL1AText(text_mode));
    const std::string dmb_text(GetODMBText());

    std::vector<std::string> header_parts(0);
    header_parts.push_back(event_text.str());
    header_parts.push_back(l1a_text);
    header_parts.push_back(dmb_text);
    PrintHeader(header_parts, words_per_line);

    if(GetBit(kill_mask, 0)){
      PrintComponent(uncat, 0, ddu_header_start_, words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 4)){
      PrintComponent("DDU Header", ddu_header_start_, ddu_header_end_,
                     words_per_line, text_mode);
    }
    const unsigned num_odmbs(odmb_header_start_.size());

    if(num_odmbs>0){
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, ddu_header_end_, odmb_header_start_.at(0),
                       words_per_line, text_mode);
      }
      for(unsigned odmb(0); odmb+1<num_odmbs; ++odmb){
        PrintODMB(uncat, odmb, words_per_line, kill_mask, text_mode);
        if(GetBit(kill_mask, 0)){
          PrintComponent(uncat, odmb_trailer_end_.at(odmb),
                         odmb_header_start_.at(odmb+1), words_per_line, text_mode);
        }
      }
      PrintODMB(uncat, num_odmbs-1, words_per_line, kill_mask, text_mode);
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, odmb_trailer_end_.at(odmb_trailer_end_.size()-1), ddu_trailer_start_,
                       words_per_line, text_mode);
      }
    }else{
      if(GetBit(kill_mask, 0)){
        PrintComponent(uncat, ddu_header_end_,
                       ddu_trailer_start_, words_per_line, text_mode);
      }
    }
    if(GetBit(kill_mask, 4)){
      PrintComponent("DDU Trailer", ddu_trailer_start_,
                     ddu_trailer_end_, words_per_line, text_mode);
    }
    if(GetBit(kill_mask, 0)){
      PrintComponent(uncat, ddu_trailer_end_, full_packet_.size(),
                     words_per_line, text_mode);
    }
  }

  void DataPacket::PrintHeader(const std::vector<std::string>& parts, const unsigned words_per_line) const{
    if(parts.size()){
      std::ostringstream oss("");
      oss << parts.at(0);
      for(unsigned part(1); part<parts.size(); ++part){
        oss << "; " << parts.at(part);
      }
      PrintWithStars(oss.str(), 5*words_per_line);
    }
  }

  void DataPacket::PrintComponent(const std::string& str, const unsigned start,
                                  const unsigned end,
                                  const unsigned words_per_line,
                                  const bool text_mode) const{
    if(start<end){
      std::cout << str << std::endl;
      PrintBuffer(GetComponent(start, end), words_per_line, start, text_mode);
      std::cout << std::endl;
    }
  }

  unsigned short DataPacket::GetContainingRanges(const unsigned word) const{
    unsigned short num_ranges(0);
    if(InRange(word, ddu_header_start_, ddu_header_end_-1)) ++num_ranges;
    for(unsigned odmb(0); odmb<odmb_header_start_.size(); ++odmb){
      if(InRange(word, odmb_header_start_.at(odmb), odmb_header_end_.at(odmb)-1)) ++num_ranges;
      if(InRange(word, alct_start_.at(odmb), alct_end_.at(odmb)-1)) ++num_ranges;
      if(InRange(word, otmb_start_.at(odmb), otmb_end_.at(odmb)-1)) ++num_ranges;
      if(InRange(word, odmb_trailer_start_.at(odmb), odmb_trailer_end_.at(odmb)-1)) ++num_ranges;
      for(unsigned dcfeb(0); dcfeb<dcfeb_start_.size(); ++dcfeb){
        if(InRange(word, dcfeb_start_.at(odmb).at(dcfeb), dcfeb_end_.at(odmb).at(dcfeb)-1)) ++num_ranges;
      }
    }
    if(InRange(word, ddu_trailer_start_, ddu_trailer_end_-1)) ++num_ranges;
    return num_ranges;
  }

  std::vector<unsigned> DataPacket::GetValidDCFEBs(const unsigned odmb) const{
    Parse();
    if(odmb_header_end_.at(odmb)-odmb_header_start_.at(odmb)>=3){
      std::vector<unsigned> dcfebs(0);
      const unsigned check_word(full_packet_.at(odmb_header_start_.at(odmb)+2) & 0x7F);
      for(unsigned dcfeb(0); dcfeb<7; ++dcfeb){
        if(GetBit(check_word, dcfeb)) dcfebs.push_back(dcfeb+1);
      }
      return dcfebs;
    }else{
      return std::vector<unsigned>(0);
    }
  }

  std::string DataPacket::GetDCFEBText(const unsigned odmb) const{
    Parse();
    if(odmb_header_end_.at(odmb)==odmb_header_start_.at(odmb)){
      return "No ODMB data";
    }else{
      const std::vector<unsigned> dcfebs(GetValidDCFEBs(odmb));
      if(dcfebs.size()){
        std::ostringstream oss("");
        oss << "Expecting DCFEB(s): " << dcfebs.at(0);
        for(unsigned dcfeb(1); dcfeb<dcfebs.size(); ++dcfeb){
          oss << ", " << dcfebs.at(dcfeb);
        }
        return oss.str();
      }else{
        return "No DCFEBS expected";
      }
    }
  }

  std::vector<std::pair<uint_fast32_t, bool> > DataPacket::GetL1As() const{
    Parse();
    std::vector<std::pair<uint_fast32_t, bool> > l1as(0);
    if(ddu_header_end_-ddu_header_start_==3){
      l1as.push_back(std::make_pair(full_packet_.at(ddu_header_start_+2) & 0xFFFFu, true));
    }else if(ddu_header_end_-ddu_header_start_>=4){
      const uint_fast8_t first_8_bits(full_packet_.at(ddu_header_start_+3) & 0xFFu);
      const uint_fast16_t last_16_bits(full_packet_.at(ddu_header_start_+2) & 0xFFFFu);
      l1as.push_back(std::make_pair((first_8_bits << 16) | last_16_bits, true));
    }
    for(unsigned packet(0); packet<odmb_header_start_.size(); ++packet){
      if(odmb_header_end_.at(packet)-odmb_header_start_.at(packet)==1){
        l1as.push_back(std::make_pair(full_packet_.at(odmb_header_start_.at(packet)) & 0xFFFu, true));
      }else if(odmb_header_end_.at(packet)-odmb_header_start_.at(packet)>=2){
        const uint_fast16_t first_12_bits(full_packet_.at(odmb_header_start_.at(packet)+1) & 0xFFFu);
        const uint_fast16_t last_12_bits(full_packet_.at(odmb_header_start_.at(packet)) & 0xFFFu);
        l1as.push_back(std::make_pair((first_12_bits << 12) | last_12_bits, true));
      }
      if(alct_end_.at(packet)-alct_start_.at(packet)>=3){
        l1as.push_back(std::make_pair(full_packet_.at(alct_start_.at(packet)+2) & 0xFFFu, false));
      }
      if(otmb_end_.at(packet)-otmb_start_.at(packet)>=3){
        l1as.push_back(std::make_pair(full_packet_.at(otmb_start_.at(packet)+2) & 0xFFFu, false));
      }
    }
    return l1as;
  }

  bool DataPacket::HasL1AMismatch() const{
    const std::vector<std::pair<uint_fast32_t, bool> > l1as(GetL1As());
    if(l1as.size()){
      uint_fast32_t to_match(l1as.at(0).first);
      for(unsigned l1a(0); l1a<l1as.size(); ++l1a){
        if(l1as.at(l1a).second){
          to_match=l1as.at(0).first;
          break;
        }
      }
      const uint_fast16_t to_match_12_bits(to_match & 0xFFFu);
      for(unsigned l1a(0); l1a<l1as.size(); ++l1a){
        if((l1as.at(l1a).second && l1as.at(l1a).first!=to_match)
           || (!l1as.at(l1a).second && l1as.at(l1a).first!=to_match_12_bits)){
          return true;
        }
      }
      return false;
    }else{
      return true;
    }
  }

  std::string DataPacket::GetL1AText(const bool text_mode) const{
    const std::vector<std::pair<uint_fast32_t, bool> > l1as(GetL1As());
    if(l1as.size()){
      uint_fast32_t to_match(l1as.at(0).first);
      for(unsigned l1a(0); l1a<l1as.size(); ++l1a){
        if(l1as.at(l1a).second){
          to_match=l1as.at(0).first;
          break;
        }
      }
      const uint_fast16_t to_match_12_bits(l1as.at(0).first & 0xFFFu);
      std::ostringstream oss("");
      if(HasL1AMismatch()){
        oss << "L1As=(";
        for(unsigned l1a(0); l1a<l1as.size(); ++l1a){
          const bool is_match(l1as.at(l1a).second?(l1as.at(l1a).first==to_match):(l1as.at(l1a).first==to_match_12_bits));
          if(!is_match && !text_mode){
            if(l1a!=0){
              oss << ", " << io::bold << io::bg_red << io::fg_white
                  << l1as.at(l1a).first << io::normal;
            }else{
              oss << io::bold << io::bg_red << io::fg_white
                  << l1as.at(l1a).first << io::normal;
            }
          }else{
            if(l1a!=0){
              oss << ", " << l1as.at(l1a).first;
            }else{
              oss << l1as.at(l1a).first;
            }
          }
        }
        oss << ")";
      }else{
        oss << "L1A=" << to_match << " (0x" << std::hex << to_match << ")";
      }
      return oss.str();
    }else{
      return "No L1As";
    }
  }

  bool DataPacket::HasUncategorizedWords() const{
    Parse();

    if(ddu_header_start_>0) return true;
    const unsigned num_odmbs(odmb_header_start_.size());
    if(num_odmbs>0){
      if(ddu_header_end_!=odmb_header_start_.at(0)) return true;
      for(unsigned odmb(0); odmb+1<num_odmbs; ++odmb){
        if(odmb_header_end_.at(odmb)!=alct_start_.at(odmb)) return true;
        if(alct_end_.at(odmb)!=otmb_start_.at(odmb)) return true;
        const unsigned num_dcfebs(dcfeb_start_.at(odmb).size());
        if(num_dcfebs>0){
          if(otmb_end_.at(odmb)!=dcfeb_start_.at(odmb).at(0)) return true;
          for(unsigned dcfeb(0); dcfeb+1<num_dcfebs; ++dcfeb){
            if(dcfeb_end_.at(odmb).at(dcfeb)!=dcfeb_start_.at(odmb).at(dcfeb+1)) return true;
          }
          if(dcfeb_end_.at(odmb).at(num_dcfebs-1)!=odmb_trailer_start_.at(odmb)) return true;
        }else{
          if(otmb_end_.at(odmb)!=odmb_trailer_start_.at(odmb)) return true;
        }
        if(odmb_trailer_end_.at(odmb)!=odmb_header_start_.at(odmb+1)) return true;
      }
   
      if(odmb_header_end_.at(num_odmbs-1)!=alct_start_.at(num_odmbs-1)) return true;
      if(alct_end_.at(num_odmbs-1)!=otmb_start_.at(num_odmbs-1)) return true;
      const unsigned num_dcfebs(dcfeb_start_.at(num_odmbs-1).size());
      if(num_dcfebs>0){
        if(otmb_end_.at(num_odmbs-1)!=dcfeb_start_.at(num_odmbs-1).at(0)) return true;
        for(unsigned dcfeb(0); dcfeb+1<num_dcfebs; ++dcfeb){
          if(dcfeb_end_.at(num_odmbs-1).at(dcfeb)!=dcfeb_start_.at(num_odmbs-1).at(dcfeb+1)) return true;
        }
        if(dcfeb_end_.at(num_odmbs-1).at(num_dcfebs-1)!=odmb_trailer_start_.at(num_odmbs-1)) return true;
      }else{
        if(otmb_end_.at(num_odmbs-1)!=odmb_trailer_start_.at(num_odmbs-1)) return true;
      }
      if(odmb_trailer_end_.at(odmb_trailer_end_.size()-1)!=ddu_trailer_start_) return true;
    }else{
      if(ddu_header_end_!=ddu_trailer_start_) return true;
    }
    if(ddu_trailer_end_!=full_packet_.size()) return true;
    return false;
  }

  uint_fast32_t DataPacket::GetDDUStatus() const{
    Parse();
    if(ddu_trailer_end_-ddu_trailer_start_>=8){
      const uint_fast16_t low_16_bits(full_packet_.at(ddu_trailer_start_+6));
      const uint_fast16_t high_16_bits(full_packet_.at(ddu_trailer_start_+7));
      return (high_16_bits << 16) | low_16_bits;
    }else if(ddu_trailer_end_-ddu_header_start_==7){
      return full_packet_.at(ddu_trailer_start_+6);
    }else{
      return 0x0u;
    }
  }

  std::vector<bool> DataPacket::GetDMBType() const{
    Parse();
    std::vector<bool> is_odmb(0);
    for(unsigned dmb(0); dmb<odmb_header_start_.size(); ++dmb){
      if(odmb_header_end_.at(dmb)-odmb_header_start_.at(dmb)>=3){
        is_odmb.push_back(GetBit(full_packet_.at(odmb_header_start_.at(dmb)+2), 9));
      }
    }
    return is_odmb;
  }

  std::string DataPacket::GetODMBText() const{
    Parse();
    std::ostringstream oss("");
    const std::vector<bool> is_odmb(GetDMBType());
    if(is_odmb.size()){
      oss << "(" << (is_odmb.at(0)?"ODMB":"DMB");
      for(unsigned dmb(1); dmb<is_odmb.size(); ++dmb){
        oss << ", " << (is_odmb.at(dmb)?"ODMB":"DMB");
      }
      oss << ")";
    }else{
      oss << "No (O)DMBs";
    }
    return oss.str();
  }
}
