#include "EmuPlotter.h"


MECanvases_List EmuPlotter::bookCommonCanvases(int nodeNumber) 
{
	string prefix = Form("EMU_%d_", nodeNumber);;
	MECanvases_List commonCanvases;
	MECanvases_List_iterator itr;
	for (itr = commonCanvasesFactory.begin(); itr != commonCanvasesFactory.end(); ++itr) {
		EmuMonitoringCanvas * obj = new EmuMonitoringCanvas(*itr->second);
		obj->setPrefix(prefix);
		commonCanvases[obj->getName()] = obj;
		// commonCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
	}
	
	return commonCanvases;
}

MECanvases_List EmuPlotter::bookDDUCanvases(int dduNumber) 
{
	string prefix = Form("DDU_%02d_", dduNumber);
 	MECanvases_List dduCanvases;
        MECanvases_List_iterator itr;

	for (itr = dduCanvasesFactory.begin(); itr != dduCanvasesFactory.end(); ++itr) {
		EmuMonitoringCanvas* obj = new EmuMonitoringCanvas(*itr->second);
		obj->setPrefix(prefix);
		dduCanvases[obj->getName()] = obj;
		// dduCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
	}
	
	return dduCanvases;
}

MECanvases_List EmuPlotter::bookChamberCanvases(int chamberID) 
{
	int crate = (chamberID >> 4) & 0xFF;
	int slot = chamberID & 0xF;
	string prefix = Form("CSC_%03d_%02d_", crate, slot);
	MECanvases_List chamberCanvases;
        MECanvases_List_iterator itr;

	for (itr = chamberCanvasesFactory.begin(); itr != chamberCanvasesFactory.end(); ++itr) {
		EmuMonitoringCanvas* obj = new EmuMonitoringCanvas(*itr->second);
		obj->setPrefix(prefix);
		chamberCanvases[obj->getName()] = obj;
		// chamberCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
	}
	
	return chamberCanvases;
}
