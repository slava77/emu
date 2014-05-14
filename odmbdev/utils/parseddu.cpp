/*
  DDU parsing script
  Parses CSC raw files for either single event or whole file diagnostic report. The diagnostic
  report produces a 34 bit error-code for each event containing, from MSB to LSB, an L1A
  mismatch error bit, an uncategorized word error bit, and the 32 bit DDU status

  Command line options:
  -f: Sets input file.
  -s: Sets the first event to parse and process.
  -e: Sets the last event to parse and process.
  -t: Text-only mode. Turns off colorization of parsed key words in output.
  -m: Set error bitmask.
  -k: Set 5 bit kill bitmask indicating which components to print (DDU, ODMB, ALCT+OTMB, DCFEBs, uncategorized)
  -w: Sets the number of words to print per line. Default is 20.
  -c: Counting mode. Counts number of packets without further processing
  -a: Analysis mode. Analyzes events without printing

  If only one command line option is given (without a "-"), it is used as a file name and a diagnostic report is produced.

  Author: Adam Dishaw (ald77@physics.ucsb.edu)
  Last modified: 2014-05-11
*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <stdint.h>
#include <unistd.h>
#include "parseddu.hpp"
#include "data_packet.hpp"

using Packet::DataPacket;
using Packet::svu;
using Packet::InRange;

int main(int argc, char *argv[]){
  unsigned words_per_line(40);
  std::string filename("");
  unsigned start_entry(0), end_entry(0);
  bool count_mode(false);
  bool text_mode(false);
  bool analysis_mode(false);
  uint_fast64_t temp_mask(std::numeric_limits<uint_fast64_t>::max());
  uint_fast8_t kill_mask(0xFF);
  DataPacket::ErrorType mask(static_cast<DataPacket::ErrorType>(temp_mask));

  std::stringstream ss("");

  if(argc==2 && argv[1][0]!='-'){
    filename=argv[1];
  }else{
    char opt(' ');
    while(( opt=getopt(argc, argv, "w:f:s:e:m:k:cta") )!=-1){
      switch(opt){
      case 'w':
        words_per_line=atoi(optarg);
        break;
      case 'f':
        filename=optarg;
        break;
      case 's':
        start_entry=atoi(optarg);
        break;
      case 'e':
        end_entry=atoi(optarg);
        break;
      case 'm':
	mask=static_cast<DataPacket::ErrorType>(GetNumber(optarg));
        break;
      case 'k':
	kill_mask=GetNumber(optarg);
	break;
      case 'c':
        count_mode=true;
        analysis_mode=false;
        break;
      case 'a':
        analysis_mode=true;
        count_mode=false;
      case 't':
        text_mode=true;
        break;
      default:
        std::cerr << "Error: Invalid option flag '" << opt << "'." << std::endl;
      }
    }
  }

  if(start_entry!=0 || end_entry!=0){
    if(end_entry==0) end_entry=start_entry;
    if(start_entry==0) start_entry=end_entry;
    if(start_entry>end_entry){
      const unsigned temp(start_entry);
      start_entry=end_entry;
      end_entry=temp;
    }
  }else{
    start_entry=1;
    end_entry=std::numeric_limits<unsigned>::max();
  }

  std::ifstream ifs(filename.c_str(),std::ifstream::in | std::ifstream::binary);
  if(ifs.is_open()){
    DataPacket data_packet;
    svu packet(0);
    unsigned entry(1);
    if(analysis_mode){
      std::map<DataPacket::ErrorType, unsigned> type_counter;
      std::vector<std::pair<std::pair<DataPacket::ErrorType, std::vector<bool> >, unsigned> > type_record(0);
      for(entry=1; entry<start_entry && FindStartOfNextPacket(ifs, packet); ++entry){
      }
      for(; entry<=end_entry && FindStartOfNextPacket(ifs, packet); ++entry){
        GetRestOfPacket(ifs, packet);
        data_packet.SetData(packet);
	
	const std::vector<bool> is_odmb(data_packet.GetDMBType());
        const DataPacket::ErrorType this_type(static_cast<DataPacket::ErrorType>(mask & data_packet.GetPacketType()));
	type_record.push_back(std::make_pair(std::make_pair(this_type, is_odmb), entry));
	
        if(type_counter.find(this_type)==type_counter.end()){
          type_counter[this_type]=1;
        }else{
          ++type_counter[this_type];
        }
      }
      for(unsigned i(0); i<type_record.size(); ++i){
	const DataPacket::ErrorType last((i==0)?(DataPacket::kGood):(type_record.at(i-1).first.first));
	const DataPacket::ErrorType ups=static_cast<DataPacket::ErrorType>(type_record.at(i).first.first & (~last));
	const DataPacket::ErrorType downs=static_cast<DataPacket::ErrorType>((~type_record.at(i).first.first) & last);

	bool have_dmb(false), have_odmb(false);
	for(unsigned dmb(0); dmb<type_record.at(i).first.second.size(); ++dmb){
	  if(type_record.at(i).first.second.at(dmb)){
	    have_odmb=true;
	  }else{
	    have_dmb=true;
	  }
	}
	std::string packet_type("ODMB");
	if(have_odmb){
	  if(have_dmb){
	    packet_type="ODMB+DMB";
	  }else{
	    packet_type="    ODMB";
	  }
	}else{
	  if(have_dmb){
	    packet_type="     DMB";
	  }else{
	    packet_type="   Empty";
	  }
	}

	if(ups | downs){
	  std::cout << packet_type << " packet " << std::dec << std::setw(8) << std::setfill(' ') << type_record.at(i).second
		    << " turns on " << std::hex << std::setw(9) << std::setfill('0')
		    << ups << " and turns off " << std::hex << std::setw(9) << std::setfill('0')
		    << downs << " (now at " << std::hex << std::setw(9) << std::setfill('0')
		    << type_record.at(i).first.first << ")." << std::endl;
	}
      }
      
      std::cout << std::dec << std::setw(8) << std::setfill(' ') << entry-start_entry
		<< " total packets:" <<std::endl;
      for(std::map<DataPacket::ErrorType, unsigned>::iterator it(type_counter.begin());
	  it!=type_counter.end(); ++it){
	std::cout << std::setw(8) << std::dec << std::setfill(' ') << it->second
		  << " packets of type " << std::setw(9) << std::setfill('0') << std::hex
		  << it->first << std::endl;
      }
    }else if(count_mode){
      unsigned event_count(0);
      for(entry=0; FindStartOfNextPacket(ifs, packet); ++entry){
	GetRestOfPacket(ifs, packet);
        ++event_count;
      }
      std::cout << std::dec << event_count << " total events." << std::endl;
    }else if(start_entry!=0 || end_entry!=0){
      for(entry=1; entry<start_entry && FindStartOfNextPacket(ifs, packet); ++entry){
      }
      for(; entry<=end_entry && FindStartOfNextPacket(ifs, packet); ++entry){
        GetRestOfPacket(ifs, packet);
        data_packet.SetData(packet);
        data_packet.Print(words_per_line, entry, kill_mask, text_mode);
      }
    }
    ifs.close();
  }else{
    std::cerr << "Error: could not open " << filename << std::endl;
  }
}

void UpdateLastFewWords(const uint16_t x, svu& buf){
  if(buf.size()>0){
    for(unsigned index(0); index+1<buf.size(); ++index){
      buf.at(index)=buf.at(index+1);
    }
    buf.at(buf.size()-1)=x;
  }
}

bool FindStartOfPacket(std::ifstream& ifs, svu& header){
  header.clear();
  header.resize(8,0);
  uint16_t word(0);
  while(ifs.read(reinterpret_cast<char*>(&word), sizeof(word))){
    UpdateLastFewWords(word, header);
    if(header.at(5)==0x8000 && header.at(6)==0x0001 && header.at(7)==0x8000){
      for(unsigned word_count(8);
          word_count<20 && ifs.read(reinterpret_cast<char*>(&word), sizeof(word));
          ++word_count){
        header.push_back(word);
      }
      return true;
    }
    if(InRange(header.at(0), 0x9000, 0x9FFF)
       && InRange(header.at(1), 0x9000, 0x9FFF)
       && InRange(header.at(2), 0x9000, 0x9FFF)
       && InRange(header.at(3), 0x9000, 0x9FFF)
       && InRange(header.at(4), 0xA000, 0xAFFF)
       && InRange(header.at(5), 0xA000, 0xAFFF)
       && InRange(header.at(6), 0xA000, 0xAFFF)) return true;
  }
  return false;
}

bool FindStartOfNextPacket(std::ifstream&ifs, svu& header){
  GetRestOfPacket(ifs, header);
  header.clear();
  return FindStartOfPacket(ifs, header);
}

void GetRestOfDDUPacket(std::ifstream& ifs, svu& packet){
  uint16_t word(0);
  while(packet.size()<12 && ifs.read(reinterpret_cast<char*>(&word), sizeof(word))){
    packet.push_back(word);
  }
  while(!(packet.at(packet.size()-12)==0x8000
          && packet.at(packet.size()-11)==0x8000
          && packet.at(packet.size()-10)==0xFFFF
          && packet.at(packet.size()-9)==0x8000)
        && ifs.read(reinterpret_cast<char*>(&word), sizeof(word))){
    packet.push_back(word);
  }
}

void GetRestOfPCPacket(std::ifstream& ifs, svu& packet){
  uint16_t word(0);
  while(!(InRange(packet.at(packet.size()-8), 0xF000, 0xFFFF)
          && InRange(packet.at(packet.size()-7), 0xF000, 0xFFFF)
          && InRange(packet.at(packet.size()-6), 0xF000, 0xFFFF)
          && InRange(packet.at(packet.size()-5), 0xF000, 0xFFFF)
          && InRange(packet.at(packet.size()-4), 0xE000, 0xEFFF)
          && InRange(packet.at(packet.size()-3), 0xE000, 0xEFFF)
          && InRange(packet.at(packet.size()-2), 0xE000, 0xEFFF))
        && ifs.read(reinterpret_cast<char*>(&word), sizeof(word))){
    packet.push_back(word);
  }
}

void GetRestOfPacket(std::ifstream& ifs, svu& packet){
  if(packet.size()<8) return;
  if(packet.at(5)==0x8000 && packet.at(6)==0x0001 && packet.at(7)==0x8000){
    GetRestOfDDUPacket(ifs, packet);
  }else{
    GetRestOfPCPacket(ifs, packet);
  }
}

uint_fast64_t GetNumber(const std::string& input){
  return strtol(input.c_str(), NULL, 0);
}
