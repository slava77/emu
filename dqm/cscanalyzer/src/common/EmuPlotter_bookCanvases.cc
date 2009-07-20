#include "emu/dqm/cscanalyzer/EmuPlotter.h"


MECanvases_List EmuPlotter::bookCommonCanvases(int nodeNumber)
{
//	string prefix = Form("EMU_%d", nodeNumber);;
  std::string prefix = "EMU";
  MECanvases_List commonCanvases;
  MECanvases_List_iterator itr;

  MECanvases_List& factory = MECanvasFactories["EMU"];
  for (itr = factory.begin(); itr != factory.end(); ++itr)
    {
// 	for (itr = commonCanvasesFactory.begin(); itr != commonCanvasesFactory.end(); ++itr) {
      EmuMonitoringCanvas * obj = new EmuMonitoringCanvas(*itr->second);
      obj->setPrefix(prefix);
      commonCanvases[obj->getName()] = obj;
      // commonCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
    }

  return commonCanvases;
}

MECanvases_List EmuPlotter::bookDDUCanvases(int dduNumber)
{
  std::string prefix = Form("DDU_%02d", dduNumber);
  MECanvases_List dduCanvases;
  MECanvases_List_iterator itr;
  MECanvases_List& factory = MECanvasFactories["DDU"];
  for (itr = factory.begin(); itr != factory.end(); ++itr)
    {
//	for (itr = dduCanvasesFactory.begin(); itr != dduCanvasesFactory.end(); ++itr) {
      EmuMonitoringCanvas* obj = new EmuMonitoringCanvas(*itr->second);
      obj->setPrefix(prefix);
      std::string title = obj->getTitle() + Form(" DDU = %02d", dduNumber);
      obj->setTitle(title);
      dduCanvases[obj->getName()] = obj;
      // dduCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
    }

  return dduCanvases;
}

MECanvases_List EmuPlotter::bookChamberCanvases(int chamberID)
{
  int crate = (chamberID >> 4) & 0xFF;
  int slot = chamberID & 0xF;
  std::string prefix = Form("CSC_%03d_%02d", crate, slot);
  MECanvases_List chamberCanvases;
  MECanvases_List_iterator itr;
  MECanvases_List& factory = MECanvasFactories["CSC"];
  for (itr = factory.begin(); itr != factory.end(); ++itr)
    {
//	for (itr = chamberCanvasesFactory.begin(); itr != chamberCanvasesFactory.end(); ++itr) {
      EmuMonitoringCanvas* obj = new EmuMonitoringCanvas(*itr->second);
      obj->setPrefix(prefix);
      std::string title = obj->getTitle() + Form(" Crate ID = %02d. DMB ID = %02d", crate, slot);
      obj->setTitle(title);
      chamberCanvases[obj->getName()] = obj;
      // chamberCanvases.insert(pair<string, EmuMonitoringCanvas>(obj.getName(),obj));
    }

  return chamberCanvases;
}
