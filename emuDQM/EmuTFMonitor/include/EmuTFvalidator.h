#ifndef EmuTFvalidator_h
#define EmuTFvalidator_h

#include <map>
#include <list>
#include <string>

#include "EmuTFxmlParsing.h"
#include "EmuTFbookkeeper.h"

class TH1;

class EmuTFvalidator {
private:
	const EmuTFbookkeeper &bookkeeper;

	static bool empty_hist    (const TH1 *hist, std::vector<std::string> args);
	static bool valid_range   (const TH1 *hist, std::vector<std::string> args);
	static bool allowed_values(const TH1 *hist, std::vector<std::string> args);
	static bool max_deviation (const TH1 *hist, std::vector<std::string> args);

	std::list<std::string> falied_, passed_;

public:
	std::list<std::string> falied(void){ return falied_; }
	std::list<std::string> passed(void){ return passed_; }

	int check(std::map<std::string,EmuTFxmlParsing::CheckAttributes> checkList);

	EmuTFvalidator(const EmuTFbookkeeper &bk):bookkeeper(bk){}
	~EmuTFvalidator(void){}

};

#endif
