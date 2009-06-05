#include "DBImporter.h"

DBImporter::DBImporter(const std::string& rootFile_, const unsigned int& runNumber_, const oracle::occi::Connection *conn_) 
  : worker(conn_) {

  rootFile = 0;
  gSystem->Load("libHistPainter");

  rootFileName = rootFile_;
  runNumber = runNumber_;

  rootFile = TFile::Open(rootFileName.c_str());
  
  if (!rootFile) {
    LOG_ERROR << "Unable to open " << rootFileName;
    throw cscdqm::Exception("Unable to open root file");
  }

  if (!rootFile->cd("DQMData")) {
    LOG_ERROR << "No histos folder in file";
    throw cscdqm::Exception("No histos folder in file");
  }

  loadCache();

}

DBImporter::~DBImporter() {
  if (rootFile) rootFile->Close();
}

void DBImporter::loadCache() {

  unsigned int n = 0;
  std::vector<std::pair<TDirectory*, std::string> > dirs;
  std::set<std::string> skipped;
  dirs.push_back(std::make_pair(rootFile, ""));
  TKey* key;

  LOG_INFO << "Reading MOs..";

  while (dirs.begin() != dirs.end()) {

    std::pair<TDirectory*, std::string> dp = *dirs.begin();
    dirs.erase(dirs.begin());

    TDirectory* d = dp.first;
    std::string path = dp.second + d->GetName();
    if (path != "") path = path.append("/");

    TIter next(d->GetListOfKeys());
    while (key = (TKey*) next()) {

      if (key->IsFolder()) {
        dirs.push_back(std::make_pair(dynamic_cast<TDirectory*>(key->ReadObj()), path));
      } else {

        TH1 *obj = const_cast<TH1*>(dynamic_cast<const TH1*>(key->ReadObj()));
        const std::string fullpath = path + obj->GetName();
        
        bool found = false;
        std::string name = fullpath; 
  
        if (DQMUTL::regexMatch(reEMU, fullpath)) {
          DQMUTL::regexReplace(reEMU, name, "$1");
          cscdqm::HistoId id;
          if (cscdqm::HistoDef::getHistoIdByName(name, id)) {
            found = true;
            cscdqm::EMUHistoDef def(id);
            cache.put(def, new CSCMonitorObject(new EmuMonitoringObject(obj)));
          }
        } else

        if (DQMUTL::regexMatch(reDDU, fullpath)) {
          DQMUTL::regexReplace(reDDU, name, "$3");
          unsigned int number = 0;
          cscdqm::stringToNumber(number, DQMUTL::regexReplaceStr(reDDU, fullpath, "$1"), std::dec);
          cscdqm::HistoId id;
          if (cscdqm::HistoDef::getHistoIdByName(name, id)) {
            found = true;
            cscdqm::DDUHistoDef def(id, number);
            cache.put(def, new CSCMonitorObject(new EmuMonitoringObject(obj)));
          }
        } else

        if (DQMUTL::regexMatch(reCSC, fullpath)) {
          DQMUTL::regexReplace(reCSC, name, "$5");

          unsigned int crate = 0;
          cscdqm::stringToNumber(crate, DQMUTL::regexReplaceStr(reCSC, fullpath, "$1"), std::dec);

          unsigned int dmb = 0;
          cscdqm::stringToNumber(dmb, DQMUTL::regexReplaceStr(reCSC, fullpath, "$2"), std::dec);

          unsigned int add = 0;
          cscdqm::HistoId id;

          if (!cscdqm::HistoDef::getHistoIdByName(name, id) && DQMUTL::regexMatch(reAdd, name)) {
            cscdqm::stringToNumber(add, DQMUTL::regexReplaceStr(reAddNumber, name, "$2"), std::dec);
            DQMUTL::regexReplace(reAdd, name, "$1%d");
          }

          if (cscdqm::HistoDef::getHistoIdByName(name, id)) {
            found = true;
            cscdqm::CSCHistoDef def(id, crate, dmb, add);
            cache.put(def, new CSCMonitorObject(new EmuMonitoringObject(obj)));
          } 

        }

        if (!found && skipped.find(name) == skipped.end()) {
          LOG_WARN << "Object " << name << " (" << fullpath << ") was not recognized or not found. Skipping...";
          skipped.insert(name);
        } else {
          n++;
        }

      }
    }
  }

  LOG_INFO << n << " MOs loaded to cache";

}

  void DBImporter::finalize() {
    worker.commit();
    worker.setSQL("begin dqm_update.run_refresh(:1); end;");
    worker.setInt(1, runNumber);
    worker.execSQL();
    worker.commit();
  }
