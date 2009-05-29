/*****************************************************************************\
* $Id: DDU.cc,v 1.7 2009/05/29 11:23:18 paste Exp $
\*****************************************************************************/
#include "emu/fed/DDU.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "emu/fed/Fiber.h"
#include "emu/fed/JTAGElement.h"

emu::fed::DDU::DDU(int mySlot):
VMEModule(mySlot),
fiberVector_(15, new Fiber()),
gbe_prescale_(0),
killfiber_(0xf7fff),
rui_(0),
fmm_id_(0)
{

	// Build the JTAG chains

	// VMEPROM is one element
	JTAGChain chainVMEPROM;
	JTAGElement *elementVMEPROM = new JTAGElement("VMEPROM", VMEPROM, 2, PROM_BYPASS, 8, 0x00002000, false);
	chainVMEPROM.push_back(elementVMEPROM);
	JTAGMap[VMEPROM] = chainVMEPROM;

	// DDUPROM has two elements
	JTAGChain chainDDUPROM;
	JTAGElement *elementDDUPROM0 = new JTAGElement("DDUPROM0", DDUPROM0, 3, PROM_BYPASS, 8, 0x00003000, false);
	chainDDUPROM.push_back(elementDDUPROM0);

	JTAGElement *elementDDUPROM1 = new JTAGElement("DDUPROM1", DDUPROM1, 3, PROM_BYPASS, 8, 0x00003000, false);
	chainDDUPROM.push_back(elementDDUPROM1);
	JTAGMap[DDUPROM0] = chainDDUPROM;
	JTAGMap[DDUPROM1] = chainDDUPROM;

	// INPROM has two elements
	JTAGChain chainINPROM;
	JTAGElement *elementINPROM0 = new JTAGElement("INPROM0", INPROM0, 4, PROM_BYPASS, 8, 0x00004000, false);
	chainINPROM.push_back(elementINPROM0);

	JTAGElement *elementINPROM1 = new JTAGElement("INPROM1", INPROM1, 4, PROM_BYPASS, 8, 0x00004000, false);
	chainINPROM.push_back(elementINPROM1);
	JTAGMap[INPROM0] = chainINPROM;
	JTAGMap[INPROM1] = chainINPROM;

	// DDUFPGA is one element
	JTAGChain chainDDUFPGA;
	JTAGElement *elementDDUFPGA = new JTAGElement("DDUFPGA", DDUFPGA, 5, PROM_BYPASS, 10, 0x00005000, false);
	chainDDUFPGA.push_back(elementDDUFPGA);
	JTAGMap[DDUFPGA] = chainDDUFPGA;

	// INFPGAs look like two elements, but are actually individual
	JTAGChain chainINFPGA0;
	JTAGElement *elementINFPGA0 = new JTAGElement("INFPGA0", INFPGA0, 6, VTX2_BYPASS, 14, 0x00006000, false);
	chainINFPGA0.push_back(elementINFPGA0);
	JTAGMap[INFPGA0] = chainINFPGA0;

	// INFPGAs are one each
	JTAGChain chainINFPGA1;
	JTAGElement *elementINFPGA1 = new JTAGElement("INFPGA1", INFPGA1, 7, VTX2_BYPASS, 14, 0x00007000, false);
	chainINFPGA1.push_back(elementINFPGA1);
	JTAGMap[INFPGA1] = chainINFPGA1;

	// VME Parallel registers is one element
	JTAGChain chainVMEPARA;
	JTAGElement *elementVMEPARA = new JTAGElement("VMEPARA", VMEPARA, 9, PROM_BYPASS, 8, 0x00030000, true);
	chainVMEPARA.push_back(elementVMEPARA);
	JTAGMap[VMEPARA] = chainVMEPARA;

	// VME Serial registers is one element
	JTAGChain chainVMESERI;
	JTAGElement *elementVMESERI = new JTAGElement("VMESERI", VMESERI, 10, PROM_BYPASS, 8, 0x00040000, true);
	chainVMESERI.push_back(elementVMESERI);
	JTAGMap[VMESERI] = chainVMESERI;

	// The JTAG reset path is special, but looks like one element
	JTAGChain chainRESET;
	JTAGElement *elementRESET = new JTAGElement("RESET", RESET, 12, PROM_BYPASS, 8, 0x0000fffe, false);
	chainRESET.push_back(elementRESET);
	JTAGMap[RESET] = chainRESET;

	// The SADC is special, but looks like one element
	JTAGChain chainSADC;
	JTAGElement *elementSADC = new JTAGElement("SADC", SADC, 13, PROM_BYPASS, 8, 0x0000d000, false);
	chainSADC.push_back(elementSADC);
	JTAGMap[SADC] = chainSADC;

}


emu::fed::DDU::~DDU()
{
  //  std::cout << "DDU destruct" << std::endl;
}

void emu::fed::DDU::configure()
throw (emu::fed::exception::DDUException)
{
	try {
		writeGbEPrescale(gbe_prescale_);
		writeKillFiber(killfiber_);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



emu::fed::Fiber *emu::fed::DDU::getFiber(size_t fiberNumber)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fiberNumber >= fiberVector_.size()) {
		std::ostringstream error;
		error << "Fiber vector overflow, fiberNumber=" << fiberNumber;
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	return fiberVector_[fiberNumber];
}



void emu::fed::DDU::addFiber(emu::fed::Fiber *fiber, size_t fiberNumber)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fiberNumber > 14) {
		std::ostringstream error;
		error << "Fiber vector overflow, fiberNumber=" << fiberNumber;
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	fiberVector_[fiberNumber] = fiber;
	
	// Set the kill fiber appropriately
	if (fiber->isKilled()) killfiber_ &= ~(1 << fiberNumber);
	else killfiber_ |= (1 << fiberNumber);
}



void emu::fed::DDU::setFibers(std::vector<emu::fed::Fiber *> fiberVector)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fiberVector.size() > 15) {
		std::ostringstream error;
		error << "Fiber vector overflow, new fiberVector.size()=" << fiberVector.size();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	} else if (fiberVector.size() < 15) {
		// Resize the new vector just in case
		fiberVector.resize(15, new Fiber());
	}
	fiberVector_ = fiberVector;
	
	// Set the kill fiber appropriately
	killfiber_ &= ~(0x00007fff);
	for (size_t iFiber = 0; iFiber < fiberVector.size(); iFiber++) {
		if (!(fiberVector[iFiber]->isKilled())) killfiber_ |= (1 << iFiber);
	}
}



///////////////////////////////////////////////////////////////////////////////
// Read VME Parallel
///////////////////////////////////////////////////////////////////////////////

uint16_t emu::fed::DDU::readFMM()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0f09, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readCSCStatus()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x04, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFMMBusy()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x00, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFMMFullWarning()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x01, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFMMLostSync()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x02, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFMMError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x03, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readSwitches()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0e, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readParallelStatus()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0f, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readInputRegister(uint8_t iReg)
throw (emu::fed::exception::DDUException)
{
	if (iReg > 2) {
		std::ostringstream error;
		error << "InputRegister argument must be between 0 and 2 (inclusive)";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(VMEPARA, 0x0008 | (iReg << 8), 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFakeL1()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0509, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readGbEPrescale()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0009, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readTestRegister(uint8_t iReg)
throw (emu::fed::exception::DDUException)
{
	if (iReg > 4) {
		std::ostringstream error;
		error << "TestRegister argument must be between 0 and 4 (inclusive)";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(VMEPARA, 0x0008 | ((iReg + 3) << 8), 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readWarningHistory()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0005, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readBusyHistory()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMEPARA, 0x0006, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Write VME Parallel
///////////////////////////////////////////////////////////////////////////////

void emu::fed::DDU::writeFMM(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> bogoBits(1, value);
		writeRegister(VMEPARA, 0x8f09, 16, bogoBits);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeFakeL1(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> bogoBits(1, value);
		writeRegister(VMEPARA, 0x8509, 16, bogoBits);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeGbEPrescale(uint8_t value)
throw (emu::fed::exception::DDUException)
{
	value &= 0xf;
	uint8_t complement = 0xf - value;
	uint16_t loadMe = (complement << 12) | (value << 8) | (complement << 4) | value;
	try {
		std::vector<uint16_t> bogoBits(1, loadMe);
		writeRegister(VMEPARA, 0x8009, 16, bogoBits);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeInputRegister(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> bogoData(1,value);
		writeRegister(VMEPARA, 0x8008, 16, bogoData);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Read VME Serial/Flash
///////////////////////////////////////////////////////////////////////////////

uint8_t emu::fed::DDU::readSerialStatus()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMESERI, 0x04, 8)[0] & 0x00ff;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFlashKillFiber()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMESERI, 0x0104,16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFlashBoardID()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMESERI, 0x0304, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFlashRUI()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMESERI, 0x0704,16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



std::vector<uint16_t> emu::fed::DDU::readFlashGbEFIFOThresholds()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(VMESERI, 0x0504,34);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Write VME Serial/Flash
///////////////////////////////////////////////////////////////////////////////

void emu::fed::DDU::writeFlashKillFiber(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		// Input register needs to be written first before updating flash.
		writeInputRegister(value & 0x7fff);
		// Bogus data for sending to the VMESERI path.
		std::vector<uint16_t> bogoData(1,0);
		writeRegister(VMESERI, 0x0904, 16, bogoData);
		// Flash needs to sleep after writing
		usleep(100000);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeFlashBoardID(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		// Input register needs to be written first before updating flash.
		writeInputRegister(value);
		// Bogus data for sending to the VMESERI path.
		std::vector<uint16_t> bogoData(1,0);
		writeRegister(VMESERI, 0x0b04, 16, bogoData);
		// Flash needs to sleep after writing
		usleep(100000);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeFlashRUI(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		// Input register needs to be written first before updating flash.
		writeInputRegister(value);
		// Bogus data for sending to the VMESERI path.
		std::vector<uint16_t> bogoData(1,0);
		writeRegister(VMESERI, 0x0f04, 16, bogoData);
		// Flash needs to sleep after writing
		usleep(100000);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeFlashGbEFIFOThresholds(std::vector<uint16_t> values)
throw (emu::fed::exception::DDUException)
{
	if (values.size() != 3) {
		std::ostringstream error;
		error << "value to be written to GBEFIFOThresholds needs to be 34 bits (3 uint16_t values)";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		// Input register needs to be written first before updating flash.
		for (std::vector<uint16_t>::iterator iValue = values.begin(); iValue != values.end(); iValue++) {
			writeInputRegister((*iValue));
		}
		// Bogus data for sending to the VMESERI path.
		std::vector<uint16_t> bogoData(1,0);
		writeRegister(VMESERI, 0x0d04, 16, bogoData);
		// Flash needs to sleep after writing
		usleep(100000);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Read SADC
///////////////////////////////////////////////////////////////////////////////

float emu::fed::DDU::readTemperature(uint8_t sensor)
throw (emu::fed::exception::DDUException)
{
	if (sensor >= 4) {
		std::ostringstream error;
		error << "Temperature sensor argument must be 0-3 inclusive";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		float Vout= (float) readRegister(SADC, 0x0089 | (sensor << 4), 16)[0] / 1000.;
		float cval = 1/(0.1049406423E-2+0.2133635468E-3*log(65000.0/Vout-13000.0)+0.7522287E-7*pow(log(65000.0/Vout-13000.0),3.0))-0.27315E3; // Celcius
		float fval=9.0/5.0*cval+32.; // Ferinheit
		return fval;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



float emu::fed::DDU::readVoltage(uint8_t sensor)
throw (emu::fed::exception::DDUException)
{
	if (sensor >= 4) {
		std::ostringstream error;
		error << "Voltage sensor argument must be 0-3 inclusive";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return (float) readRegister(SADC, 0x0089 | ((sensor+4) << 4), 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Read DDUFPGA
///////////////////////////////////////////////////////////////////////////////

uint16_t emu::fed::DDU::readOutputStatus()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 6, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFIFOStatus(uint8_t fifo)
throw (emu::fed::exception::DDUException)
{
	if (fifo < 1 || fifo > 3) {
		std::ostringstream error;
		error << "FIFOStatus argument must be between 1 and 3 (inclusive)";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	uint16_t command = 7;
	if (fifo == 1)
		command = 7;
	else if (fifo == 2)
		command = 8;
	else if (fifo == 3)
		command = 11;

	try {
		return readRegister(DDUFPGA, command, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFFError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 9, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readCRCError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 10, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readXmitError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 12, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint32_t emu::fed::DDU::readKillFiber()
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> result = readRegister(DDUFPGA, 13, 20);
		return (result[0] | ((uint32_t) result[1] << 16)) & 0x000fffff;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readDMBError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 15, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readTMBError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 16, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}


uint16_t emu::fed::DDU::readALCTError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 17, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readLIEError()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 18, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readInRDStat()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 19, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readInCHistory()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 20, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readEBRegister(uint8_t reg = 1)
throw (emu::fed::exception::DDUException)
{
	if (reg < 1 || reg > 3) {
		std::ostringstream error;
		error << "EBRegister argument must be 1, 2, or 3";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(DDUFPGA, 21 + reg, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readDMBLive()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 25, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readDMBLiveAtFirstEvent()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 26, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readWarningMonitor()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 27, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readMaxTimeoutCount()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 28, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readBXOrbit()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 30, 12)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::toggleL1Calibration()
throw (emu::fed::exception::DDUException)
{
	try {
		// Magic
		readRegister(DDUFPGA, 31, 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readRUI()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 32, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::sendFakeL1A()
throw (emu::fed::exception::DDUException)
{
	try {
		// Magic
		readRegister(DDUFPGA, 33, 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



std::vector<uint32_t> emu::fed::DDU::readOccupancyMonitor()
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint32_t> result;
		for (unsigned int iTimes = 0; iTimes < 4; iTimes++) {
			std::vector<uint16_t> tempResult = readRegister(DDUFPGA, 34, 32);
			result.push_back((tempResult[1] << 16) | tempResult[0]);
		}
		return result;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readAdvancedFiberErrors()
throw (emu::fed::exception::DDUException)
{
	try {
		return readRegister(DDUFPGA, 35, 15)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Write DDUFPGA
///////////////////////////////////////////////////////////////////////////////

void emu::fed::DDU::writeKillFiber(uint32_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> bogoBits;
		bogoBits.push_back( (value & 0xffff) );
		bogoBits.push_back( ((value & 0xf0000) >> 16) );
		writeRegister(DDUFPGA, 14, 20, bogoBits);
		
		// Set the killed bit on the owned fibers for convenience
		reloadFiberKillBits(value & 0x7fff);

	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::writeBXOrbit(uint16_t value)
throw (emu::fed::exception::DDUException)
{
	try {
		std::vector<uint16_t> bogoBits(1, value & 0xfff);
		writeRegister(DDUFPGA, 29, 12, bogoBits);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Read INFPGA
///////////////////////////////////////////////////////////////////////////////

uint32_t emu::fed::DDU::readL1Scaler1(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{
	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		std::vector<uint16_t> result = readRegister(dev, 26, 24);
		return (result[1] << 16) | result[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFiberStatus(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 6, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readDMBSync(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 7, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFIFOStatus(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 8, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFIFOFull(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 9, 12)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readRxError(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 10, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readTimeout(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 11, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readTxError(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 12, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

}



uint16_t emu::fed::DDU::readActiveWriteMemory(enum DEVTYPE dev, uint8_t iFiber)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	if (iFiber > 3) {
		std::ostringstream error;
		error << "second argument must be between 0 and 3 (inclusive)";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		return readRegister(dev, 13 + iFiber, 10)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readAvailableMemory(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		return readRegister(dev, 17, 10)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readMinMemory(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 18, 10)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readLostError(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 19, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readCCodeStatus(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(dev, 20, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readDMBWarning(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		return readRegister(dev, 21, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint32_t emu::fed::DDU::readFiberDiagnostics(enum DEVTYPE dev, uint8_t iDiagnostic)
throw (emu::fed::exception::DDUException)
{

	if (dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "this method can only be called with a DEVTYPE INFPGA0 or INFPGA1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	if (iDiagnostic > 1) {
		std::ostringstream error;
		error << "second argument must be 0 or 1";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		std::vector<uint16_t> result = readRegister(dev, 30 + iDiagnostic, 32);
		return (result[1] << 16) | result[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Read User/ID codes
///////////////////////////////////////////////////////////////////////////////

uint32_t emu::fed::DDU::readUserCode(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	// The FPGAs are a beast.  We first need to send an instruction to open the
	// correct pathway.
	uint16_t ucCommand;
	if (dev == DDUFPGA) {
		ucCommand = VTX2P_USERCODE_L | (VTX2P_USERCODE_H << 8);
	} else if (dev == INFPGA0 || dev == INFPGA1) {
		ucCommand = VTX2P20_USERCODE_L | (VTX2P20_USERCODE_H << 8);
	} else {
		ucCommand = PROM_USERCODE;
	}

	try {
		commandCycle(dev, ucCommand);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	// Now this path is open.  We can read the usercode out.
	// Shove in (and read out)
	std::vector<uint16_t> result = jtagRead(dev, 32);

	// Now we have to send the "Bypass" command clean house.
	uint16_t bypassCommand;
	if (dev == DDUFPGA) {
		bypassCommand = VTX2P_BYPASS_L | (VTX2P_BYPASS_H << 8);
	} else if (dev == INFPGA0 || dev == INFPGA1) {
		bypassCommand = VTX2P20_BYPASS_L | (VTX2P20_BYPASS_H << 8);
	} else {
		bypassCommand = PROM_BYPASS;
	}

	try {
		commandCycle(dev, bypassCommand);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	return (result[0] & 0xffff) | (result[1] << 16);

}



uint32_t emu::fed::DDU::readIDCode(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{

	// The FPGAs are a beast.  We first need to send an instruction to open the
	// correct pathway.
	uint16_t ucCommand;
	if (dev == DDUFPGA) {
		ucCommand = VTX2P_IDCODE_L | (VTX2P_IDCODE_H << 8);
	} else if (dev == INFPGA0 || dev == INFPGA1) {
		ucCommand = VTX2P20_IDCODE_L | (VTX2P20_IDCODE_H << 8);
	} else {
		ucCommand = PROM_IDCODE;
	}

	try {
		commandCycle(dev, ucCommand);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	// Now this path is open.  We can read the usercode out.

	// Shove in (and read out)
	std::vector<uint16_t> result = jtagRead(dev, 32);

	// Now we have to send the "Bypass" command clean house.
	uint16_t bypassCommand;
	if (dev == DDUFPGA) {
		bypassCommand = VTX2P_BYPASS_L | (VTX2P_BYPASS_H << 8);
	} else if (dev == INFPGA0 || dev == INFPGA1) {
		bypassCommand = VTX2P20_BYPASS_L | (VTX2P20_BYPASS_H << 8);
	} else {
		bypassCommand = PROM_BYPASS;
	}

	try {
		commandCycle(dev, bypassCommand);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	return (result[0] & 0xffff) | (result[1] << 16);

}

///////////////////////////////////////////////////////////////////////////////
// Resets
///////////////////////////////////////////////////////////////////////////////

void emu::fed::DDU::resetFPGA(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{
	if (dev != DDUFPGA && dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "Need to specify a valid FPGA";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		commandCycle(dev, DDUFPGA_RST);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Universal reads
///////////////////////////////////////////////////////////////////////////////

uint32_t emu::fed::DDU::readFPGAStatus(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{
	if (dev != DDUFPGA && dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "Need to specify a valid FPGA";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		std::vector<uint16_t> result = readRegister(dev, 3, 32);
		return (result[0] & 0xffff) | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint32_t emu::fed::DDU::readL1Scaler(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{
	if (dev != DDUFPGA && dev != INFPGA0 && dev != INFPGA1) {
		std::ostringstream error;
		error << "Need to specify a valid FPGA";
		XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		std::vector<uint16_t> result = readRegister(dev, 2, 24);
		return (result[0] & 0xffff) | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



std::vector<uint16_t> emu::fed::DDU::readDebugTrap(enum DEVTYPE dev)
throw (emu::fed::exception::DDUException)
{
	try {
		if (dev == DDUFPGA)
			return readRegister(DDUFPGA, 21, 192);
		else if (dev == INFPGA0 || dev == INFPGA1)
			return readRegister(dev, 25, 192);
		else {
			std::ostringstream error;
			error << "Need to specify a valid FPGA";
			XCEPT_DECLARE(emu::fed::exception::DDUException, e2, error.str());
			std::ostringstream tag;
			tag << "slot:" << slot() << ",board:DDU";
			e2.setProperty("tag", tag.str());
			throw e2;
		}
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::disableFMM()
throw (emu::fed::exception::DDUException)
{
	try {
		return writeFMM(0xFED0);
	} catch (emu::fed::exception::DDUException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DDU::enableFMM()
throw (emu::fed::exception::DDUException)
{
	try {
		return writeFMM(0xFED8);
	} catch (emu::fed::exception::DDUException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readLiveFibers()
throw (emu::fed::exception::DDUException)
{
	try {
		return (readFiberStatus(INFPGA0)&0x000000ff) | ((readFiberStatus(INFPGA1)&0x000000ff)<<8);
	} catch (emu::fed::exception::DDUException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DDU::readFiberErrors()
throw (emu::fed::exception::DDUException)
{
	try {
		return (readCSCStatus() | readAdvancedFiberErrors());
	} catch (emu::fed::exception::DDUException &e) {
		std::ostringstream error;
		error << "Exception communicating with DDU";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DDUException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////////

void emu::fed::DDU::reloadFiberKillBits(uint16_t killfiber)
{
	for (size_t iFiber = 0; iFiber < fiberVector_.size(); iFiber++) {
		fiberVector_[iFiber]->killed_ = (killfiber & (1 << iFiber));
	}
}


std::vector<uint16_t> emu::fed::DDU::readRegister(enum DEVTYPE dev, uint16_t myRegister, unsigned int nBits)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{
	// The information about the element being written
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	JTAGChain chain = JTAGMap[dev];

	//std::clog << "Attempting to read from " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << std::endl;

	try {
		// Direct VME reads are different.  Doesn't matter what element in the chain it is.
		if (chain.front()->directVME) {

			// The address contains both the device to read and the register to read.
			uint8_t myDevice = myRegister & 0xff;
			uint8_t myChannel = ((myRegister & 0xff00) >> 8) & 0xff;
			uint32_t myAddress = (myDevice << 12) | (myChannel << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;

			if (dev == VMESERI) {
				// Serial reads are fun.  It gives you the LSB first, but the mask is applied
				// to the low n bits of the LSB instead of the high n bits of the MSB.

				// In order to accomplish this, we first need to force readCycle to not
				// mask things out.  Ask for full bytes only with a ceiling function
				unsigned int myBytes = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
				unsigned int remainderBits = nBits % 16;

				// Now do the read
				std::vector<uint16_t> tempResult = readCycle(myAddress, myBytes * 16);

				// Now shuffle the bits.
				if (remainderBits) {

					std::vector<uint16_t> result;

					for (unsigned int iValue = 0; iValue < tempResult.size(); iValue++) {

						// Shift and mask
						uint16_t newValue = (tempResult[iValue] >> (16 - remainderBits)) & ((1 << remainderBits) - 1);
						// Add on the bits from the next element if this is not the last element.
						if ((iValue + 1) != tempResult.size()) {
							newValue |= (tempResult[iValue + 1] << remainderBits);
						} else {
							// Mask the final value if this is the last
							newValue &= (1 << remainderBits) - 1;
						}

						// Push back the value.
						result.push_back(newValue);
					}

					return result;

				} else {

					return tempResult;

				}

			} else {
				// Parallel reads are standerd.
				return readCycle(myAddress,nBits);
			}

		} else if (dev == SADC) {

			// The register given is actually just a command to open a JTAG-like register:
			std::vector<uint16_t> bogoData(1,myRegister);
			writeCycle(chain.front()->bitCode, chain.front()->cmdBits, bogoData);

			// Now we read like a VME register
			return readCycle(chain.front()->bitCode | 0x4, nBits);

		// Everything else is a JTAG command?
		} else {

			// The FPGAs are a beast.  We first need to send an instruction to the USR1
			// pathway to load the register we want to read in the USR2 pathway.
			// The commands are different for the different FPGAs.
			uint16_t u1Command;
			if (dev == DDUFPGA) {
				u1Command = VTX2P_USR1_L | (VTX2P_USR1_H << 8);
			} else {
				u1Command = VTX2P20_USR1_L | (VTX2P20_USR1_H << 8);
			}

			commandCycle(dev, u1Command);

			// Now this path is open.  We can send the register that we want loaded.
			std::vector<uint16_t> bogoCommand(1,myRegister);
			jtagWrite(dev, 8, bogoCommand, true);

			// Now we have to send the "Bypass" command to actually load the register.
			uint16_t bypassCommand;
			if (dev == DDUFPGA) {
				bypassCommand = VTX2P_BYPASS_L | (VTX2P_BYPASS_H << 8);
			} else {
				bypassCommand = VTX2P20_BYPASS_L | (VTX2P20_BYPASS_H << 8);
			}

			commandCycle(dev, bypassCommand);

			// Finally, we have to open the USR2 pathway and do a standard JTAG read.
			uint16_t u2Command;
			if (dev == DDUFPGA) {
				u2Command = VTX2P_USR2_L | (VTX2P_USR2_H << 8);
			} else {
				u2Command = VTX2P20_USR2_L | (VTX2P20_USR2_H << 8);
			}

			commandCycle(dev, u2Command);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagRead(dev, nBits);

			// Before we leave, we need to reset the FPGA to the normal status.
			// Close the USR2 pathway with the bypass command.
			commandCycle(dev, bypassCommand);

			// Open the USR1 pathway and send a NORM_MODE command.
			commandCycle(dev, u1Command);

			bogoCommand.clear();
			bogoCommand.push_back(NORM_MODE);
			jtagWrite(dev, 8, bogoCommand, true);

			commandCycle(dev,bypassCommand);

			return result;

		}

	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in readRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ")";
		XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



std::vector<uint16_t> emu::fed::DDU::writeRegister(enum DEVTYPE dev, uint16_t myRegister, unsigned int nBits, std::vector<uint16_t> myData)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The information about the element being written
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	JTAGChain chain = JTAGMap[dev];

	//std::cout << "Attempting to write to " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << " values (low to high) ";
	//for (std::vector<uint16_t>::iterator iData = myData.begin(); iData != myData.end(); iData++) {
	//std::cout << std::showbase << std::hex << (*iData) << std::dec << " ";
	//}
	//std::cout << std::endl;

	// Direct VME writes are different.  Doesn't matter which element in the chain this is.
	try {

		if (chain.front()->directVME) {

			// The address contains both the device to read and the register to read.
			uint8_t myDevice = myRegister & 0xff;
			uint8_t myChannel = (myRegister & 0xff00) >> 8;
			uint32_t myAddress = (myDevice << 12) | (myChannel << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;

			writeCycle(myAddress, nBits, myData);

			// This sort of write does not read back, so return an empty vector.
			std::vector<uint16_t> bogoBits;
			return bogoBits;

		// Everything else is a JTAG command?
		} else {

			// The FPGAs are a beast.  We first need to send an instruction to the USR1
			// pathway to load the register we want to read in the USR2 pathway.
			// The commands are different for the different FPGAs.
			uint16_t u1Command;
			if (dev == DDUFPGA) {
				u1Command = VTX2P_USR1_L | (VTX2P_USR1_H << 8);
			} else {
				u1Command = VTX2P20_USR1_L | (VTX2P20_USR1_H << 8);
			}

			commandCycle(dev, u1Command);

			// Now this path is open.  We can send the register that we want loaded.
			std::vector<uint16_t> bogoCommand(1,myRegister);
			jtagWrite(dev, 8, bogoCommand, true);

			// Now we have to send the "Bypass" command to actually load the register.
			uint16_t bypassCommand;
			if (dev == DDUFPGA) {
				bypassCommand = VTX2P_BYPASS_L | (VTX2P_BYPASS_H << 8);
			} else {
				bypassCommand = VTX2P20_BYPASS_L | (VTX2P20_BYPASS_H << 8);
			}

			commandCycle(dev, bypassCommand);

			// Finally, we have to open the USR2 pathway and do a standard JTAG read.
			uint16_t u2Command;
			if (dev == DDUFPGA) {
				u2Command = VTX2P_USR2_L | (VTX2P_USR2_H << 8);
			} else {
				u2Command = VTX2P20_USR2_L | (VTX2P20_USR2_H << 8);
			}

			commandCycle(dev, u2Command);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagWrite(dev, nBits, myData);

			// Before we leave, we need to reset the FPGA to the normal status.
			// Close the USR2 pathway with the bypass command.
			commandCycle(dev, bypassCommand);

			// Open the USR1 pathway and send a NORM_MODE command.
			commandCycle(dev, u1Command);

			bogoCommand.clear();
			bogoCommand.push_back(NORM_MODE);
			jtagWrite(dev, 8, bogoCommand, true);

			commandCycle(dev,bypassCommand);

			return result;

		}

	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in writeRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ", myData=" << &myData << ")";
		XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DDU";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}
