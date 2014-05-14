/*
  add_ddu_hdr: Adds DDU headers and trailers to raw file obtained with ODMB's spy PC channel.
  It takes the name of the input file as an argument. The output file can be given as an 
  argument, or the default is the same name with "ddu_" pre-pended.

  Authors: Adam Dishaw (ald77@physics.ucsb.edu), Manuel Franco Sevilla (manuelf@physics.ucsb.edu)
  Last modified: 2013-11-05
*/

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <unistd.h>
#include "parseddu.hpp"
#include "data_packet.hpp"

#define MaxWords 10000
using namespace std;

void UpdateLastFewWords(const uint16_t, Packet::svu&);
bool FindStartOfPacket(std::ifstream&, Packet::svu&);
void GetRestOfPacket(std::ifstream&, Packet::svu&);
void GetRestOfDDUPacket(std::ifstream&, Packet::svu&);
void GetRestOfPCPacket(std::ifstream&, Packet::svu&);
int calcDDUcrc(unsigned short uipacket[], unsigned int entries);
using Packet::DataPacket;
using Packet::svu;
using Packet::InRange;


int main(int argc, char *argv[]){
  std::string filename("");
  if(argc>=2) filename = argv[1];

  string outname("ddu_" + filename); 
  if(argc>=3)outname = argv[2];

  if(argc<2 || argc>3) {
    cout<<endl<<"USAGE:\n=======\n./add_ddu_hdr.exe InputFile [OutputFile]"<<endl<<endl;
    return 0;
  }

  unsigned short dduheader[] = {0xF860, 0x0002, 0x0000, 0x5000, 0x0000, 0x8000, 
                                0x0001, 0x8000, 0x2FC1, 0x0001, 0x3030, 0x0001};
  unsigned short ddutrailer[] = {0x8000, 0x8000, 0xFFFF, 0x8000, 0x0001, 0x0005, 
                                 0xC2DB, 0x8040, 0xC2C0, 0x4918, 0x000E, 0xA000};
  FILE *outfile;
  outfile = fopen(outname.c_str(),"wb");

  std::ifstream ifs(filename.c_str(),std::ifstream::in | std::ifstream::binary);
  if(ifs.is_open()){
    DataPacket data_packet;
    svu packet(0);
    unsigned short crcpacket[MaxWords];
    unsigned int entry=0;

    // Loop over all ODMB packets
    for(entry=0; FindStartOfPacket(ifs, packet); ++entry){
      GetRestOfPacket(ifs, packet);
      if(packet.size()>=MaxWords){cout<<"Event "<<entry<<" is too long"<<endl; continue;}

      // Parsing L1A/BX counters, and number of words in packet
      unsigned short l1a_lsb = 0x0FFF & packet[0];
      unsigned short l1a_msb = 0x0FFF & packet[1];
      unsigned short bxcnt = 0x0FFF & packet[3];
      dduheader[1] = (bxcnt<<4) | (dduheader[1] & 0x000F);
      dduheader[2] = (l1a_msb<<12) | l1a_lsb;
      dduheader[3] = (l1a_msb>>4) | (dduheader[3] & 0xFF00);
      int nWords = 6 + packet.size()/4;
      ddutrailer[9] = 0;
      ddutrailer[10] = 0xFFFF & nWords;
      ddutrailer[11] = (0x00FF & nWords>>16) | (ddutrailer[11] & 0xFF00);

      // Calculating CRC
      for(unsigned int ind=0; ind<12; ind++) crcpacket[ind] = dduheader[ind];
      for(unsigned int ind=0; ind<packet.size(); ind++){
        crcpacket[ind+12] = packet[ind];
      }
      for(unsigned int ind=0; ind<12; ind++) crcpacket[ind+12+packet.size()] = ddutrailer[ind];      
      int CRC = calcDDUcrc(crcpacket, packet.size()+24);
      crcpacket[packet.size()+21] = 0xFFFF & CRC;

      // Write packet to file
      fwrite(crcpacket, sizeof(crcpacket[0]), packet.size()+24, outfile);
    }
    ifs.close();
    if(outfile) fclose(outfile);
    cout<<outname<<" printed with "<<entry<<" events"<<endl;
  } else cout << filename << " does not exist"<<endl;

  return 1;
}

bool FindStartOfPacket(std::ifstream& ifs, svu& header){
  header.clear();
  header.resize(8,0);
  uint16_t word(0);
  while(ifs.read(reinterpret_cast<char*>(&word), sizeof(word))){
    UpdateLastFewWords(word, header);
    if(header.at(5)==0x8000 && header.at(6)==0x0001 && header.at(7)==0x8000){
      for(unsigned int word_count(8);
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
       && InRange(header.at(6), 0xA000, 0xAFFF)
       && InRange(header.at(7), 0xA000, 0xAFFF)) return true;
  }
  return false;
}

void UpdateLastFewWords(const uint16_t x, svu& buf){
  if(buf.size()>0){
    for(unsigned index(0); index+1<buf.size(); ++index){
      buf.at(index)=buf.at(index+1);
    }
    buf.at(buf.size()-1)=x;
  }
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
          && InRange(packet.at(packet.size()-4), 0xE000, 0xEFFF))
        //&& InRange(packet.at(packet.size()-3), 0xE000, 0xEFFF)
        //&& InRange(packet.at(packet.size()-2), 0xE000, 0xEFFF)
        //&& InRange(packet.at(packet.size()-1), 0xE000, 0xEFFF))
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

int calcDDUcrc(unsigned short vec[], unsigned int entries){
  int CRC=0;
  for (uint16_t j=0, w=0; j<entries; j++ ){
    w = vec[j] & 0xffff;
    for (uint32_t i=15, t=0, ncrc=0; i<16; i--){
      t = ((w >> i) & 1) ^ ((CRC >> 15) & 1);
      ncrc = (CRC << 1) & 0xfffc;
      ncrc |= (t ^ (CRC & 1)) << 1;
      ncrc |= t;
      CRC = ncrc;
    }
  }
  
  return CRC;
}
