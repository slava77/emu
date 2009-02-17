#ifndef _CSCReport_H
#define _CSCReport_H

#include <string>
#include <map>

typedef enum {
	NONE = 0,
	MINOR,
	TOLERABLE,
	SEVERE,
	CRITICAL,
} DQM_SEVERITY;

static const char* DQM_SEVERITY_STR[CRITICAL+1] = {
	"NONE",
	"MINOR",
	"TOLERABLE",
	"SEVERE",
	"CRITICAL"
};

typedef std::string ReportObjID;
typedef class ReportEntry{
public:
	ReportEntry() {clearEntry();}
	ReportEntry& fillEntry(std::string t="", DQM_SEVERITY sev=NONE, std::string r_histo = "", std::string r_test="") {
		text=t; severity=sev; rel_histo=r_histo; rel_test=r_test; return *this;
	}
	void clearEntry() {text=""; rel_histo="", rel_test="", severity=NONE;}
	std::string text;
	std::string rel_histo;
	std::string rel_test;
	DQM_SEVERITY severity;
} ReportEntry;


typedef std::map<ReportObjID, std::vector<ReportEntry> > T_DQMReport;

typedef class DQMReport {
public:
	DQMReport() {_report.clear();}
	void addEntry(ReportObjID id, ReportEntry entry) {_report[id].push_back(entry);}	
	std::vector <ReportEntry> getEntries(ReportObjID id) {return _report[id];}
	T_DQMReport& getReport() {return _report;}
//	std::vector<ReportObjID> getObjIDs() {};
	void clearReport() {_report.clear();}
private:	
	T_DQMReport _report;
} DQMReport;

#endif
