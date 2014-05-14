#ifndef H_PARSEDDU
#define H_PARSEDDU

#include <fstream>
#include <stdint.h>
#include "data_packet.hpp"

void UpdateLastFewWords(const uint16_t, Packet::svu&);
bool FindStartOfPacket(std::ifstream&, Packet::svu&);
bool FindStartOfNextPacket(std::ifstream&, Packet::svu&);
void GetRestOfPacket(std::ifstream&, Packet::svu&);
void GetRestOfDDUPacket(std::ifstream&, Packet::svu&);
void GetRestOfPCPacket(std::ifstream&, Packet::svu&);

uint_fast64_t GetNumber(const std::string& input);

#endif
