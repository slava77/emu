#include "EmuPlotter.h"


ME_List EmuPlotter::bookCommon(int nodeNumber) 
{
	string prefix = Form("EMU_%d_", nodeNumber);;
	ME_List commonMEs;
	ME_List_iterator itr;
	for (itr = commonMEfactory.begin(); itr != commonMEfactory.end(); ++itr) {
		EmuMonitoringObject * obj = new EmuMonitoringObject(*itr->second);
		obj->setPrefix(prefix);
		commonMEs[obj->getName()] = obj;
		// commonMEs.insert(pair<string, EmuMonitoringObject>(obj.getName(),obj));
	}
	
	return commonMEs;
}

ME_List EmuPlotter::bookDDU(int dduNumber) 
{
	string prefix = Form("DDU_%02d_", dduNumber);
 	ME_List dduMEs;
        ME_List_iterator itr;

	for (itr = dduMEfactory.begin(); itr != dduMEfactory.end(); ++itr) {
		EmuMonitoringObject* obj = new EmuMonitoringObject(*itr->second);
		obj->setPrefix(prefix);
		dduMEs[obj->getName()] = obj;
		// dduMEs.insert(pair<string, EmuMonitoringObject>(obj.getName(),obj));
	}
	
	return dduMEs;
}

ME_List EmuPlotter::bookChamber(int chamberID) 
{
	int crate = (chamberID >> 4) & 0xFF;
	int slot = chamberID & 0xF;
	string prefix = Form("CSC_%03d_%02d_", crate, slot);
	ME_List chamberMEs;
        ME_List_iterator itr;

	for (itr = chamberMEfactory.begin(); itr != chamberMEfactory.end(); ++itr) {
		EmuMonitoringObject* obj = new EmuMonitoringObject(*itr->second);
		obj->setPrefix(prefix);
		chamberMEs[obj->getName()] = obj;
		// chamberMEs.insert(pair<string, EmuMonitoringObject>(obj.getName(),obj));
	}
	
	return chamberMEs;
}
