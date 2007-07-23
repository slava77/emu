//-----------------------------------------------------------------------
// $Id: FEDCrateParser.h,v 3.2 2007/07/23 05:02:22 gilmore Exp $
// $Log: FEDCrateParser.h,v $
// Revision 3.2  2007/07/23 05:02:22  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.16  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
#ifndef FEDCrateParser_h
#define FEDCrateParser_h
/*
 *  class FEDCrateParser
 *  author Alex Tumanov 6/12/03
 *     
 */


#include "DDUParser.h"
#include "DCCParser.h"
#include "VMEParser.h"
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "Crate.h"

class FEDCrateParser {

public:
	FEDCrateParser() {}
	
	/** Parse the file
		@param name File Name
	*/
	void parseFile(const char* name);
	
	DDUParser dduParser() const {return dduParser_;}
	DCCParser dccParser() const {return dccParser_;}
	VMEParser vmeParser() const {return vmeParser_;}
	/* There has to be a way to get the crates in the XML
	back to the user, so that selectCrates will work. */
	std::vector<Crate*> crateVector() { return crateVector_; }

protected:
	DDUParser dduParser_;
	DCCParser dccParser_;
	VMEParser vmeParser_;
	int crateNumber;
	std::vector<Crate*> crateVector_;
};

#endif






