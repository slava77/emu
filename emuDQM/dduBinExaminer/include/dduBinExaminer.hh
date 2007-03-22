#ifndef OStream_hh
#define OStream_hh
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

class OStream : public std::ostream {
private:
	class buffer : public std::streambuf{};
	buffer     buff;
	std::streambuf *stream;
	std::streambuf *null;
	std::string     name;

public:
	void show(void){ rdbuf(stream); }
	void hide(void){ rdbuf(null);   }
	void sign(std::string nm)     { name=nm; }
	void sign(const char *nm){ name=nm; }

	void redirect(std::ostream &str){
		stream = str.rdbuf(); tie(&str);
		if( rdbuf() != null ) rdbuf(stream);
	}

	template<class T> std::ostream& operator<<(const T& val){
		return (*(std::ostream*)this)<<name<<val;
	}

	OStream(void):std::ostream(std::cout.rdbuf()),buff(),stream(std::cout.rdbuf()),null(&buff),name(""){}
	OStream(std::ostream &str):std::ostream(str.rdbuf()),buff(),stream(str.rdbuf()),null(&buff),name(""){}
};
#endif

#ifndef dduBinExaminer_hh
#define dduBinExaminer_hh
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>

class dduBinExaminer {
public:
	const uint16_t nERRORS, nWARNINGS;

private:
	std::vector<char*> sERROR,  sWARNING, sERROR_,  sWARNING_;
	int32_t               bERROR,  bWARNING;
	bool               fERROR  [25];//[nERRORS];
	bool               fWARNING[5]; //[nWARNINGS];

	std::set<int>      fCHAMB_ERR[25]; // Set of chambers which contain particular error
	std::set<int>      fCHAMB_WRN[5];  // Set of chambers which contain particular warning
	std::map<int,int32_t> bCHAMB_ERR;     // chamber <=> errors in bits
	std::map<int,int32_t> bCHAMB_WRN;     // chamber <=> errors in bits

	OStream cout, cerr;

	int currentChamber;       // ( (CrateNumber<<4) + DMBslot ) specifies chamber

	const uint16_t *buf_2, *buf_1, *buf0, *buf1, *buf2;
		  uint16_t tmpbuf[16];

	bool fDDU_Header;
	bool fDDU_Trailer;
	bool fDMB_Header;
	bool fDMB_Trailer;
	bool fALCT_Header;
	bool fTMB_Header;

	bool uniqueALCT, uniqueTMB; // Do not merge two DMBs if Trailer of the first and Header of the second are lost

	bool DAV_ALCT; // ...
	bool DAV_TMB;  // Check if DAV bits lie
	int  DAV_CFEB; // ...
	int  DAV_DMB;  // ...

public:
	int32_t cntDDU_Headers;
	int32_t cntDDU_Trailers;
	std::map<int,int32_t> cntCHAMB_Headers;
	std::map<int,int32_t> cntCHAMB_Trailers;

private:
	int32_t DDU_WordsSinceLastHeader;
	int32_t DDU_WordCount;
	int32_t DDU_WordMismatch_Occurrences;
	int32_t DDU_WordsSinceLastTrailer;

	int32_t ALCT_WordsSinceLastHeader;
	int32_t ALCT_WordCount;
	int32_t ALCT_WordsExpected;

	int32_t TMB_WordsSinceLastHeader;
	int32_t TMB_WordCount;
	int32_t TMB_WordsExpected;
	int32_t TMB_Tbins;
	int32_t TMB_WordsExpectedCorrection;

	int32_t CFEB_SampleWordCount;
	int32_t CFEB_SampleCount;
	int32_t CFEB_BSampleCount;

	bool checkCrcALCT;
	uint32_t ALCT_CRC;
	bool checkCrcTMB;
	uint32_t TMB_CRC;
	bool checkCrcCFEB;
	uint32_t CFEB_CRC;
	bool checkCrcDDU;
	uint32_t DDU_CRC;
	//int headerDAV_Active; // Obsolete since 16.09.05

public:
	OStream& output1(void){ return cout; }
	OStream& output2(void){ return cerr; }

	int32_t check(const unsigned short* &buffer, int32_t length);

	int32_t errors  (void) const { return bERROR;   }
	int32_t warnings(void) const { return bWARNING; }

	const char* errName(int num) const { if(num>=0&&num<nERRORS)   return sERROR[num];   else return ""; }
	const char* wrnName(int num) const { if(num>=0&&num<nWARNINGS) return sWARNING[num]; else return ""; }

	const char* errorName  (int num) const { if(num>=0&&num<nERRORS)   return sERROR_[num];   else return ""; }
	const char* warningName(int num) const { if(num>=0&&num<nWARNINGS) return sWARNING_[num]; else return ""; }

	bool error  (int num) const { if(num>=0&&num<nERRORS)   return fERROR  [num]; else return 0; }
	bool warning(int num) const { if(num>=0&&num<nWARNINGS) return fWARNING[num]; else return 0; }

	std::set<int> chambersWithError  (int num) const { if(num>=0&&num<nERRORS)   return fCHAMB_ERR[num]; else return std::set<int>(); }
	std::set<int> chambersWithWarning(int num) const { if(num>=0&&num<nWARNINGS) return fCHAMB_WRN[num]; else return std::set<int>(); }

	int32_t errorsForChamber(int chamber) const {
		std::map<int,int32_t>::const_iterator item = bCHAMB_ERR.find(chamber);
		if( item != bCHAMB_ERR.end() ) return item->second; else return 0;
	}

	int32_t warningsForChamber(int chamber) const {
		std::map<int,int32_t>::const_iterator item = bCHAMB_WRN.find(chamber);
		if( item != bCHAMB_WRN.end() ) return item->second; else return 0;
	}

	std::map<int,int32_t> errorsDetailed  (void) const { return bCHAMB_ERR; }
	std::map<int,int32_t> warningsDetailed(void) const { return bCHAMB_WRN; }

	void crcALCT(bool enable);
	void crcTMB (bool enable);
	void crcCFEB(bool enable);
	void crcDDU (bool enable);

	dduBinExaminer(void);
	~dduBinExaminer(void){}
};

#endif
