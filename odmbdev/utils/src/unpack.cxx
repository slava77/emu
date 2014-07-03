/*
  DCFEB unpacking script
  Parses CSC raw files, extracts DCFEB data (ignoring CFEBs and any improperly parsed DCFEBs),
  and produces diagnostic plots and information.

  Command line options:
  -f: Sets input file.
  -s: Sets the first event to parse and process.
  -e: Sets the last event to parse and process.
  -p: Make plot to visualize muons

  Author: Adam Dishaw (ald77@physics.ucsb.edu)
  Last modified: 2014-05-29
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
#include "TFile.h"
#include "TTree.h"
#include "unpack.hpp"
#include "data_packet.hpp"
#include "unpacker.hpp"

using Packet::DataPacket;
using Packet::svu;
using Packet::InRange;

int main(int argc, char *argv[]){
  std::string filename("");
  unsigned start_entry(0), end_entry(0);
  bool plot_muons(false);

  std::stringstream ss("");

  if(argc==2 && argv[1][0]!='-'){
    filename=argv[1];
  }else{
    char opt(' ');
    while(( opt=getopt(argc, argv, "f:s:e:p") )!=-1){
      switch(opt){
      case 'f':
        filename=optarg;
        break;
      case 's':
        start_entry=GetNumber(optarg);
        break;
      case 'e':
        end_entry=GetNumber(optarg);
        break;
      case 'p':
        plot_muons=true;
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
    
    for(entry=1; entry<start_entry && FindStartOfNextPacket(ifs, packet); ++entry){
    }
    Packet::Unpacker unpacker;
    TFile file((filename+".root").c_str(),"recreate");
    file.cd();
    TTree tree("unpack","unpack");
    TTree tree2("data","data");
    uint16_t l1a(0), dcfeb(0), cutoff(0);
    float muon_peak(0.0), muon_average(0.0), pedestal_average(0.0), muon_pedestal_ratio(0.0), average_layer(0.0), average_strip(0.0), average_time_bin(0.0);
    float value000(0.0), value005(0.0), value010(0.0), value015(0.0), value020(0.0), value025(0.0), value030(0.0), value035(0.0), value040(0.0), value045(0.0), value050(0.0), value055(0.0), value060(0.0), value065(0.0), value070(0.0), value075(0.0), value080(0.0), value085(0.0), value090(0.0), value095(0.0), value100(0.0);
    float quantile000(0.0), quantile005(0.0), quantile010(0.0), quantile015(0.0), quantile020(0.0), quantile025(0.0), quantile030(0.0), quantile035(0.0), quantile040(0.0), quantile045(0.0), quantile050(0.0), quantile055(0.0), quantile060(0.0), quantile065(0.0), quantile070(0.0), quantile075(0.0), quantile080(0.0), quantile085(0.0), quantile090(0.0), quantile095(0.0), quantile100(0.0);
    float this_value(0.0), this_quantile(0.0);
    bool has_muon(false);

    tree.Branch("l1a", &l1a);
    tree.Branch("dcfeb", &dcfeb);
    tree.Branch("cutoff", &cutoff);
    tree.Branch("muon_peak", &muon_peak);
    tree.Branch("muon_average", &muon_average);
    tree.Branch("pedestal_average", &pedestal_average);
    tree.Branch("muon_pedestal_ratio", &muon_pedestal_ratio);
    tree.Branch("average_layer", &average_layer);
    tree.Branch("average_strip", &average_strip);
    tree.Branch("average_time_bin", &average_time_bin);
    tree.Branch("value000", &value000);
    tree.Branch("value005", &value005);
    tree.Branch("value010", &value010);
    tree.Branch("value015", &value015);
    tree.Branch("value020", &value020);
    tree.Branch("value025", &value025);
    tree.Branch("value030", &value030);
    tree.Branch("value035", &value035);
    tree.Branch("value040", &value040);
    tree.Branch("value045", &value045);
    tree.Branch("value050", &value050);
    tree.Branch("value055", &value055);
    tree.Branch("value060", &value060);
    tree.Branch("value065", &value065);
    tree.Branch("value070", &value070);
    tree.Branch("value075", &value075);
    tree.Branch("value080", &value080);
    tree.Branch("value085", &value085);
    tree.Branch("value090", &value090);
    tree.Branch("value095", &value095);
    tree.Branch("value100", &value100);
    tree.Branch("quantile000", &quantile000);
    tree.Branch("quantile005", &quantile005);
    tree.Branch("quantile010", &quantile010);
    tree.Branch("quantile015", &quantile015);
    tree.Branch("quantile020", &quantile020);
    tree.Branch("quantile025", &quantile025);
    tree.Branch("quantile030", &quantile030);
    tree.Branch("quantile035", &quantile035);
    tree.Branch("quantile040", &quantile040);
    tree.Branch("quantile045", &quantile045);
    tree.Branch("quantile050", &quantile050);
    tree.Branch("quantile055", &quantile055);
    tree.Branch("quantile060", &quantile060);
    tree.Branch("quantile065", &quantile065);
    tree.Branch("quantile070", &quantile070);
    tree.Branch("quantile075", &quantile075);
    tree.Branch("quantile080", &quantile080);
    tree.Branch("quantile085", &quantile085);
    tree.Branch("quantile090", &quantile090);
    tree.Branch("quantile095", &quantile095);
    tree.Branch("quantile100", &quantile100);
    tree.Branch("has_muon", &has_muon);
    tree2.Branch("l1a", &l1a);
    tree2.Branch("dcfeb", &dcfeb);
    tree2.Branch("value", &this_value);
    tree2.Branch("quantile", &this_quantile);

    for(; entry<=end_entry && FindStartOfNextPacket(ifs, packet); ++entry){
      GetRestOfPacket(ifs, packet);
      data_packet.SetData(packet);
      const std::vector<std::pair<unsigned, std::vector<Packet::dcfeb_data> > > raw_dcfeb_data(data_packet.GetValidDCFEBData());
      for(unsigned dcfeb_num(0); dcfeb_num<raw_dcfeb_data.size(); ++dcfeb_num){
        unpacker.SetData(raw_dcfeb_data.at(dcfeb_num).second, data_packet.GetL1A(), raw_dcfeb_data.at(dcfeb_num).first);
        l1a=unpacker.l1a();
        dcfeb=unpacker.dcfeb();
        cutoff=unpacker.GetCutoff();
        muon_peak=unpacker.GetMuonPeak();
        muon_average=unpacker.GetMuonAverage();
        pedestal_average=unpacker.GetPedestalAverage();
        muon_pedestal_ratio=unpacker.GetMuonPedestalRatio();
        average_layer=unpacker.GetAverageLayer();
        average_strip=unpacker.GetAverageStrip();
        average_time_bin=unpacker.GetAverageTimeBin();
	value000=unpacker.GetValueOf(0.00);
	value005=unpacker.GetValueOf(0.05);
	value010=unpacker.GetValueOf(0.10);
	value015=unpacker.GetValueOf(0.15);
	value020=unpacker.GetValueOf(0.20);
	value025=unpacker.GetValueOf(0.25);
	value030=unpacker.GetValueOf(0.30);
	value035=unpacker.GetValueOf(0.35);
	value040=unpacker.GetValueOf(0.40);
	value045=unpacker.GetValueOf(0.45);
	value050=unpacker.GetValueOf(0.50);
	value055=unpacker.GetValueOf(0.55);
	value060=unpacker.GetValueOf(0.60);
	value065=unpacker.GetValueOf(0.65);
	value070=unpacker.GetValueOf(0.70);
	value075=unpacker.GetValueOf(0.75);
	value080=unpacker.GetValueOf(0.80);
	value085=unpacker.GetValueOf(0.85);
	value090=unpacker.GetValueOf(0.90);
	value095=unpacker.GetValueOf(0.95);
	value100=unpacker.GetValueOf(1.00);
	quantile000=unpacker.GetQuantileOf(0.00);
	quantile005=unpacker.GetQuantileOf(0.05);
	quantile010=unpacker.GetQuantileOf(0.10);
	quantile015=unpacker.GetQuantileOf(0.15);
	quantile020=unpacker.GetQuantileOf(0.20);
	quantile025=unpacker.GetQuantileOf(0.25);
	quantile030=unpacker.GetQuantileOf(0.30);
	quantile035=unpacker.GetQuantileOf(0.35);
	quantile040=unpacker.GetQuantileOf(0.40);
	quantile045=unpacker.GetQuantileOf(0.45);
	quantile050=unpacker.GetQuantileOf(0.50);
	quantile055=unpacker.GetQuantileOf(0.55);
	quantile060=unpacker.GetQuantileOf(0.60);
	quantile065=unpacker.GetQuantileOf(0.65);
	quantile070=unpacker.GetQuantileOf(0.70);
	quantile075=unpacker.GetQuantileOf(0.75);
	quantile080=unpacker.GetQuantileOf(0.80);
	quantile085=unpacker.GetQuantileOf(0.85);
	quantile090=unpacker.GetQuantileOf(0.90);
	quantile095=unpacker.GetQuantileOf(0.95);
	quantile100=unpacker.GetQuantileOf(1.00);
	has_muon=unpacker.LooksLikeAMuon();
        tree.Fill();

	const std::vector<std::pair<float, float> > vqs(unpacker.GetValuesAndQuantiles());
	for(unsigned i(0); i<vqs.size(); ++i){
	  this_value=vqs.at(i).first;
	  this_quantile=vqs.at(i).second;
	  tree2.Fill();
	}

	/*for(double x(0.0); x<=1.0; x+=0.0001){
	  std::cout << ' ' << std::setw(16) << x
		    << ' ' << std::setw(16) << unpacker.GetValueOf(x)
		    << ' ' << std::setw(16) << unpacker.GetQuantileOf(x) << std::endl;
		    }*/

	//std::cout << unpacker.l1a() << ' ' << unpacker.LooksLikeAMuon() << std::endl;
        /*std::cout << ' ' << std::setw(8) << unpacker.l1a()
          << ' ' << std::setw(8) << unpacker.dcfeb()
          << ' ' << std::setw(8) << unpacker.GetCutoff()
          << ' ' << std::setw(8) << unpacker.GetMuonAverage()
          << ' ' << std::setw(8) << unpacker.GetPedestalAverage()
          << ' ' << std::setw(8) << unpacker.GetMuonPedestalRatio()
          << ' ' << std::setw(8) << unpacker.GetAverageLayer()
          << ' ' << std::setw(8) << unpacker.GetAverageStrip()
          << ' ' << std::setw(8) << unpacker.GetAverageTimeBin() << std::endl;*/
        std::ostringstream oss("");
        oss << "testing_" << unpacker.l1a() << '_' << unpacker.dcfeb() << ".pdf";
        //unpacker.PrintData();
        if(plot_muons) unpacker.MakePlot(oss.str());
      }
    } 
    file.cd();
    tree.Write();
    file.Close();
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
