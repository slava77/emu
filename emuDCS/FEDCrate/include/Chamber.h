/* 
	Phillip Killewald
	1/31/08
 */
#ifndef Chamber_h
#define Chamber_h

//using namespace std;
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

class Chamber
{
public:
	friend class ChamberParser; // Friends can access my private members.

	Chamber();
	~Chamber();

	std::string endcap;
	int station;
	int type;
	int number;

	std::string name();
	std::string peripheralCrate();
	std::string fiberCassette();

private:
	/* PGK We might need to make these visible to the outside world at some point.
	For now, though I don't see why they can't be private. */
	int fiberCassetteCrate_;
	int fiberCassettePos_;
	std::string fiberCassetteSocket_;

	int peripheralCrateId_;
	int peripheralCrateVMECrate_;
	int peripheralCrateVMESlot_;

};

#endif
