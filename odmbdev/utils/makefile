CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -Wshadow -O2
LD = g++
LDFLAGS =
LDLIBS = 

default: parseddu.exe add_ddu_hdr.exe

parseddu.exe: parseddu.o data_packet.o
	$(LD) $(LDFLAGS) -o parseddu.exe parseddu.o data_packet.o $(LDLIBS)

parseddu.o: parseddu.cpp data_packet.hpp
	$(CXX) $(CXXFLAGS) -c parseddu.cpp

add_ddu_hdr.exe: add_ddu_hdr.o data_packet.o
	$(LD) $(LDFLAGS) -o add_ddu_hdr.exe add_ddu_hdr.o data_packet.o $(LDLIBS)

add_ddu_hdr.o: add_ddu_hdr.cpp data_packet.hpp
	$(CXX) $(CXXFLAGS) -c add_ddu_hdr.cpp

data_packet.o: data_packet.hpp data_packet.cpp
	$(CXX) $(CXXFLAGS) -c data_packet.cpp

.PHONY: clean

clean:
	-rm -rf *.exe *.o *~ *#