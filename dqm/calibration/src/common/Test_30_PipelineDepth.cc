//***** Code to do analysis of pipeline depth scan
//***** Under development by J. Pilot, 1 Mar 2013
//***** Currently just copied another test code here, changed names

#include "emu/dqm/calibration/Test_30_PipelineDepth.h"
#include <boost/algorithm/string.hpp>

bool DEBUG = true;

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;

Test_30_PipelineDepth::Test_30_PipelineDepth(std::string dfile): Test_Generic(dfile)
{
	testID = "30_PipelineDepth";
	nExpectedEvents = 300;
	binCheckMask=0x18100001;
	logger = Logger::getInstance(testID);

	PD_min = 63;
	PD_max = 72;

	current_PD = PD_min;
	current_l1a = -1;
}

void Test_30_PipelineDepth::initCSC(std::string cscID)
{
	//= Init per CSC events counters
	nCSCEvents[cscID]=0;

	TestData cscdata;

	tdata[cscID] = cscdata;

	bookTestsForCSC(cscID);
}

void Test_30_PipelineDepth::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{
	nTotalEvents++;
	//if (nTotalEvents < 1) return;

	//= Examiner checks
	const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
	bin_checker.setMask( binCheckMask);
	if ( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 )
	{
		//   No ddu trailer found - force checker to summarize errors by adding artificial trailer
		const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
		tmp = dduTrailer;
		bin_checker.check(tmp,uint32_t(4));
	}

	if (bin_checker.errors() != 0)
	{
		//std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors()
	//		<< " mask:0x" << std::hex << binCheckMask << std::dec << std::endl;
		doBinCheck();
	//std::cout << "dbg: continue" << std::endl;    
	//	return;
	}

	CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

	std::vector<CSCEventData> chamberDatas;
	chamberDatas = dduData.cscData();
	if ((int)(dduData.header().lvl1num()) < current_l1a) current_PD++;
	current_l1a = (int)(dduData.header().lvl1num());

	int nCSCs = chamberDatas.size();

	//= Loop over found chambers and fill data structures
	for (int i=0; i < nCSCs; i++)
	{
		analyzeCSC(chamberDatas[i]);
	}
}

void Test_30_PipelineDepth::analyzeCSC(const CSCEventData& data)
{
	const CSCDMBHeader* dmbHeader = data.dmbHeader();
	const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
	if (!dmbHeader && !dmbTrailer)
	{
		return;
	}

	int csctype=0, cscposition=0;
	std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
	if (cscID == "") return;

	cscTestData::iterator td_itr = tdata.find(cscID);
	if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
	{
		LOG4CPLUS_INFO(logger, "Found " << cscID);
		initCSC(cscID);
		addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
	}
	nCSCEvents[cscID]++;

	if (DEBUG || nCSCEvents[cscID]%300==0) std::cout << nCSCEvents[cscID] << " " << cscID << " PD " << current_PD << " current_l1a " << current_l1a << std::endl;
	// == Define aliases to access chamber specific data
	MonHistos& cschistos = mhistos[cscID];

	TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
	TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
	TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
	TH2F* v04 = reinterpret_cast<TH2F*>(cschistos["V04"]);
	TH2F* v05 = reinterpret_cast<TH2F*>(cschistos["V05"]);

	v01->GetXaxis()->SetLimits(PD_min-0.5,PD_max+0.5);
	v02->GetXaxis()->SetLimits(PD_min-0.5,PD_max+0.5);
	v03->GetXaxis()->SetLimits(PD_min-0.5,PD_max+0.5);
	v04->GetXaxis()->SetLimits(PD_min-0.5,PD_max+0.5);
	v05->GetXaxis()->SetLimits(PD_min-0.5,PD_max+0.5);

	// == Check if CFEB Data Available
	if (dmbHeader->cfebAvailable())
	{
		for (int icfeb=0; icfeb<getNumStrips(cscID)/16; icfeb++) {  // loop over cfebs in a given chamber
			CSCCFEBData * cfebData =  data.cfebData(icfeb);
			if (!cfebData) continue;

			for (int layer = 1; layer <= NLAYERS; layer++) {  // loop over layers in a given chamber
				// first, we want to check to see which strip has the maximum charge on it
				int maxStrip = -1;
				double Qmax = -999;
				double Tmax = -1;
				double Qmin12 = -999;
				double Tmin1 = -1;
				double Tmin2 = -1;

				int nTimeSamples= cfebData->nTimeSamples();

				for (int strip = 1; strip <= 16; ++strip) {   // loop over cfeb strips
					int max_time = -1;
					double max_val = -999;

					int min_time_1 = -1;
					double min_val_1 = 9e99;
					int min_time_2 = -1;
					double min_val_2 = 9e99;

					for (int itime=0; itime<nTimeSamples; itime++) {   // loop over time samples (8 or 16)
						cout << (int)cfebData->adcCounts(layer,strip,itime) << " ";
						double Qi = cfebData->adcCounts(layer,strip,itime);
						if (Qi > max_val) {
							max_time = itime;
							max_val = Qi;
						}
						if (Qi <= min_val_1) {
							min_time_2 = min_time_1;
							min_val_2 = min_val_1;
							min_time_1 = itime;
							min_val_1 = cfebData->adcCounts(layer,strip,itime);
						} else if (cfebData->adcCounts(layer,strip,itime) < min_val_2) {
							min_time_2 = itime;
							min_val_2 = cfebData->adcCounts(layer,strip,itime);
						}
					} // by now you have found max and minimum values.

					double pedestal = (min_val_1 + min_val_2) / 2;
					double max_over_ped = max_val - pedestal;

					if (max_over_ped > Qmax) {
						maxStrip = strip;
						Qmax = max_over_ped;
						Tmax = max_time;
						Qmin12 = pedestal;
						Tmin1 = min_time_1;
						Tmin2 = min_time_2;
					}
				}

				v01->Fill(current_PD,Tmax);
				v02->Fill(current_PD,Tmin1);
				v02->Fill(current_PD,Tmin2);
				v03->Fill(current_PD,Qmax);
				v04->Fill(current_PD,Qmin12);
				v05->Fill(current_PD,maxStrip);
			}
		}
	} // CFEB data available
}


void Test_30_PipelineDepth::finishCSC(std::string cscID)
{

	MonHistos& cschistos = mhistos[cscID];

	TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
	TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
	TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
	TH2F* v04 = reinterpret_cast<TH2F*>(cschistos["V04"]);
	TH2F* v05 = reinterpret_cast<TH2F*>(cschistos["V05"]);

	colNormalize(v01);
	colNormalize(v02);
	colNormalize(v03);
	colNormalize(v04);
	colNormalize(v05);

	return;
}

void Test_30_PipelineDepth::colNormalize(TH2F* hist){

	int nbinsx = hist->GetNbinsX();
	int nbinsy = hist->GetNbinsY();

	for (int icol = 1; icol <= nbinsx; ++icol){

		double sum = 0.;
		for (int irow = 1; irow <= nbinsy; ++irow){
			sum += hist->GetBinContent(icol,irow);
		}

		for (int irow = 1; irow <= nbinsy; ++irow){
			double val = hist->GetBinContent(icol,irow);
			val /= sum;
			hist->SetBinContent(icol,irow,val);
		}

	}

	return;
}

void Test_30_PipelineDepth::setTestParams()
{

	LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
	std::map<std::string, std::string>::iterator itr;

	itr = test_params.find("PD_min");
	if (itr != test_params.end() )
	{
		PD_min = atoi((itr->second).c_str());
		LOG4CPLUS_INFO (logger, "parameter: PD_min: " << PD_min);
	}
	itr = test_params.find("PD_max");
	if (itr != test_params.end() )
	{
		PD_max = atoi((itr->second).c_str());
		LOG4CPLUS_INFO (logger, "parameter: PD_max: " << PD_max);
	}
	current_PD = PD_min;
}
