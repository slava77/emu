#include "CrateSetup.h"
#include "Crate.h"
#include <iostream>

CrateSetup::CrateSetup() {
}
 

CrateSetup::~CrateSetup() {
	for(unsigned int i = 0; i < theCrates.size(); ++i) {
		delete theCrates[i];
	}
}


Crate * CrateSetup::crate(int crate_number) {
	Crate *result = NULL;
	
	for (unsigned int i = 0; i < theCrates.size(); ++i) {
		if (theCrates[i]->number() == crate_number) {
			result = theCrates[i];
			break;
		}
	}
	
	return result;
}


void CrateSetup::addCrate(int crate_number, Crate * crate) {
	if (this->crate(crate_number) == NULL) {
		// add the crate object only when the crate doesn't exist.
		theCrates.push_back(crate);
	}
}

