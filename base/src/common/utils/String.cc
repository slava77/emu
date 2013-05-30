// $Id: String.cc,v 1.1 2012/04/11 21:34:47 khotilov Exp $

#include "emu/utils/String.h"

#include <iostream>


std::vector< std::string > emu::utils::splitString(const std::string& str, const std::string& delimiters)
{
  std::vector < std::string > pieces;
  size_t start = 0;
  size_t found = str.find_first_of(delimiters);
  while (found != std::string::npos)
  {
    pieces.push_back(str.substr(start, found - start));
    //cout << "split: " << pieces.back() << endl;
    start = found + 1;
    found = str.find_first_of(delimiters, start);
  }
  pieces.push_back(str.substr(start));
  return pieces;
}


std::string emu::utils::shaveOffBlanks(const std::string& str)
{
  const std::string blanks(" \t");
  size_t start = str.find_first_not_of(blanks);
  size_t stop = str.find_last_not_of(blanks);
  if (start == std::string::npos) start = 0;
  if (stop == std::string::npos) stop = str.size() - 1;
  return str.substr(start, stop + 1 - start);
}
