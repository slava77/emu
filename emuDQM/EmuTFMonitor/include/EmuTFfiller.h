#ifndef EmuTFfiller_h
#define EmuTFfiller_h

#include "EmuTFbookkeeper.h"
#include "EmuTFxmlParsing.h"

class EmuTFfiller {
private:
	EmuTFbookkeeper tf;
	int nevents;
	int event_status;
	//
	int prev_l1a[12];

public:
	void fill(const unsigned short *buffer, unsigned int size, unsigned int errorFlag) throw();

	// For those, who are interested which histograms get filled
	enum {CLEAR, C_WORDS, DDU_CORRUPTION, SP_CORRUPTION};
	int last_event_status(void){ return event_status; }

	EmuTFbookkeeper& bookkeeper(void) throw() { return tf; }

	EmuTFfiller(std::map<std::string,EmuTFxmlParsing::HistAttributes> histList):tf(histList),nevents(0){
		for(int sp=0; sp<12; sp++) prev_l1a[sp] = -1;
	}
	~EmuTFfiller(void){}

};

#endif
