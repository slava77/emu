#include "emu/dqm/tfanalyzer/EmuTFvalidator.h"
#include "TH1.h"
#include <stdlib.h>
#include <math.h>
#include <set>

using namespace std;

bool EmuTFvalidator::empty_hist(const TH1 *hist, std::vector<std::string> args)
{
  bool error = false;
  int  skip_xbin=-1, skip_ybin=-1;
  for (vector<string>::const_iterator arg=args.begin(); arg!=args.end(); arg++)
    {
      size_t pos=string::npos;
      if ( (pos=arg->find("skip_xbin")) != string::npos ) skip_xbin = atoi(arg->substr(pos+10).c_str());
      if ( (pos=arg->find("skip_ybin")) != string::npos ) skip_ybin = atoi(arg->substr(pos+10).c_str());
    }
  for (int x=0; x<hist->GetNbinsX(); x++)
    {
      if ( skip_xbin>=0 && x==skip_xbin ) continue;
      for (int y=0; y<hist->GetNbinsY(); y++)
        {
          if ( skip_ybin>=0 && y==skip_ybin ) continue;
          if ( hist->GetBinContent(x,y) ) error = true;
        }
    }
  return error;
}

bool EmuTFvalidator::valid_range(const TH1 *hist, std::vector<std::string> args)
{
  bool error = false;
  int _max=0, _min=0;
  for (vector<string>::const_iterator arg=args.begin(); arg!=args.end(); arg++)
    {
      size_t pos=string::npos;
      if ( (pos=arg->find("min")) != string::npos ) _min = atoi(arg->substr(pos+4).c_str());
      if ( (pos=arg->find("max")) != string::npos ) _max = atoi(arg->substr(pos+4).c_str());
    }
  for (int x=1; x<=hist->GetNbinsX(); x++)
    {
      if ( hist->GetBinLowEdge(x)<_min && hist->GetBinContent(x) ) error = true;
      if ( hist->GetBinLowEdge(x)>_max && hist->GetBinContent(x) ) error = true;
    }
  return error;
}

bool EmuTFvalidator::allowed_values(const TH1 *hist, std::vector<std::string> args)
{
  bool error = false;
  set<int> values;
  for (vector<string>::const_iterator arg=args.begin(); arg!=args.end(); arg++)
    values.insert(atoi(arg->c_str()));
  for (int x=1; x<=hist->GetNbinsX(); x++)
    if ( hist->GetBinContent(x) && values.find(int(hist->GetBinLowEdge(x)+hist->GetBinWidth(x)/2.))==values.end() ) error = true;
  return error;
}

bool EmuTFvalidator::max_deviation(const TH1 *hist, std::vector<std::string> args)
{
  bool error = false;
  int meanX=0, deltaX=0, rmsX=0;
  for (vector<string>::const_iterator arg=args.begin(); arg!=args.end(); arg++)
    {
      size_t pos=string::npos;
      if ( (pos=arg->find("meanX")) != string::npos ) meanX = atoi(arg->substr(pos+6).c_str());
      if ( (pos=arg->find("deltaX")) != string::npos ) deltaX = atoi(arg->substr(pos+7).c_str());
      if ( (pos=arg->find("rmsX")) != string::npos ) rmsX = atoi(arg->substr(pos+5).c_str());
    }

  // Make a projection on X
  TH1F xProjection("xProj","",hist->GetNbinsX(),hist->GetXaxis()->GetXmax(),hist->GetXaxis()->GetXmin());
  for (int xBin=1,total=0; xBin<hist->GetNbinsX(); xBin++,total=0)
    {
      for (int yBin=1; yBin<hist->GetNbinsY(); yBin++)
        total += int(hist->GetBinContent(xBin,yBin));
      xProjection.SetBinContent(xBin,total);
    }

  if ( fabs(xProjection.GetMean()-meanX)>deltaX || xProjection.GetRMS()>rmsX ) error=true;

  return error;
}

int EmuTFvalidator::check(map<string,EmuTFxmlParsing::CheckAttributes> checkList)
{
  int nFailed=0;
  for (map<string,EmuTFxmlParsing::CheckAttributes>::const_iterator check=checkList.begin(); check!=checkList.end(); check++)
    {
      map< string, vector<string> >::const_iterator entry = check->second.items.begin();
      while ( entry != check->second.items.end() )
        {
          list<const TH1*> hists = bookkeeper.get(entry->first);
          for (list<const TH1*>::const_iterator hist=hists.begin(); hist!=hists.end(); hist++)
            {
              bool error = false;
              if ( check->first == "empty_hist"  ) error = empty_hist (*hist,entry->second);
              if ( check->first == "valid_range" ) error = valid_range(*hist,entry->second);
              if ( check->first == "allowed_values" ) error = allowed_values(*hist,entry->second);
              if ( check->first == "max_deviation" ) error = max_deviation(*hist,entry->second);
              if ( error )
                {
                  falied_.push_back((*hist)->GetName());
                  nFailed++;
                }
              else
                passed_.push_back((*hist)->GetName());

            }
          entry++;
        }
    }
  return nFailed;
}
