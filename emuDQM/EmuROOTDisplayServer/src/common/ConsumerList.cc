//*************************************************************************
//             ConsumerList for the Consumer Project
//                   03/31/00 Harmut Stadie
//
//*************************************************************************
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/ConsumerList.cc,v $
// $Revision: 1.1 $
// $Date: 2006/02/16 00:05:14 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************
#include "ConsumerList.hh"

#include "time.h"

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "TSocket.h"
#include "TServerSocket.h"
#include "TMonitor.h"

using std::string;

ConsumerList::ConsumerList(TString sitename) : _websitename(sitename)
{
  _statusnames[Unknown] = "Unknown";
  _statusnames[Dead]    = "Dead";
  _statusnames[Running] = "Running";
  _statusnames[NotUpdated] = "Running";
  _statusnames[Stopped] = "Stopped";
  _statusnames[Crashed] = "Crashed";
  _statusnames[Finished] = "Finished";

}

ConsumerList::~ConsumerList()
{
}

ConsumerList::ConsumerList(const ConsumerList& sm)
{
}
 
int ConsumerList::nEntries() const
{
  return _consumerlist.size();
}

const TString& ConsumerList::consumername(int i) const
{
  return _consumerlist[i].name;
}

const TString& ConsumerList::hostname(int i) const
{
  return _consumerlist[i].hostname;
}

int ConsumerList::port(int i) const 
{
  return  _consumerlist[i].port;
}

int ConsumerList::nevents(int i) const 
{
  return  _consumerlist[i].nevents;
}

int ConsumerList::runnumber(int i) const
{
  return  _consumerlist[i].runnumber;
}

int ConsumerList::status(int i) const
{
  return  _consumerlist[i].status;
}
 
const char *ConsumerList::statusString(int i) const
{
   return  _statusnames[_consumerlist[i].status];
}

const TString& ConsumerList::updateTime(int i) const
{
  return _consumerlist[i].time;
}

void ConsumerList::setPort(int i, int port)
{
  _consumerlist[i].port = port;
}

void ConsumerList::setNevents(int i, int nev)
{
  _consumerlist[i].nevents = nev;
}

void ConsumerList::setStatus(int i , int status)
{
  _consumerlist[i].status = status;
}


void ConsumerList::setRunnumber(int i, int runn)
{
  _consumerlist[i].runnumber = runn;
}

void ConsumerList::setUpdateTime(int i, char *tm)
{
  _consumerlist[i].time = tm;
}


void ConsumerList::removeEntry(const char *hostname,int port)
{
  std::vector<ConsumerEntry>::iterator pos = 
    _consumerlist.end();
  for(std::vector<ConsumerEntry>::iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)
    if((iter->hostname == hostname)&&(iter->port == port)) pos = iter;
  if(pos!=_consumerlist.end()) _consumerlist.erase(pos);
}

int ConsumerList::getOldNEvents(const char *hostname,int port)
{
  int nevents;
  std::vector<ConsumerEntry>::iterator pos = _consumerlist.end();

  for(std::vector<ConsumerEntry>::iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)
    if((iter->hostname == hostname)&&(iter->port == port)) pos = iter;

  if(pos!=_consumerlist.end()) 
    nevents = pos->nevents;

  return nevents;  
  
}



int ConsumerList::watchServers(const int port,const char*filename )
{
  // TMonitor *monitor = new TMonitor;
  // open sockets
  TServerSocket ss(port,kTRUE);
  if(!(ss.IsValid())) 
    {
      std::cout << "ConsumerList::watchServers: serversocket not valid" << std::endl;
      return 0;
    }
  writeWebPage(filename);
  while(1)
    {
      //std::cout << "waiting for first connection" << std::endl;
      TSocket *sock = ss.Accept();
      //std::cout << "connected from " << sock->GetInetAddress().GetHostName() << std::endl;
      char buffer[1000];
      std::string word;
      sock->RecvRaw(buffer,1000);
      std::istringstream message(buffer);
      //std::cout << buffer << std::endl;
      message >> word;
      if(word=="ConsumerList")
	{
	  message >> word;
	  while((word != "END") && message)
	    {
	      // search for name and remove old entry
	      // add new entry
	      ConsumerEntry entry;
	      entry.name = word.c_str();
	      message >> entry.hostname;
	      message >> entry.port;
	      message >> entry.runnumber; 
	      message >> entry.nevents;
	      message >> entry.status;
	      message >> word;
	      // look if processed event numbers have changed
	      // std::cout<<"/"<<getOldNEvents(entry.hostname,entry.port)<<"->"<< entry.nevents <<std::endl;;
	      if (entry.nevents == getOldNEvents(entry.hostname,entry.port))
		  entry.status = NotUpdated;
	      // search for hostname and port and remove old entry
	      removeEntry(entry.hostname,entry.port);
	      const time_t zeit = time(0);
	      entry.time = ctime(&zeit);
	      if((entry.status==Running)||(entry.status==Crashed)||(entry.status==NotUpdated)||
		 (entry.status==Unknown) && message) _consumerlist.push_back(entry);
	      //std::cout << word << std::endl;
	    }
	  //print();
	  writeWebPage(filename);
	}
      // remove connection
      sock->Close();
      delete sock;
    }
}


void ConsumerList::test()
{
  const time_t zeit = time(0);
  _consumerlist.push_back(ConsumerEntry("YMon","b0dau31.fnal.gov"
					,9090,10,10001,1,ctime(&zeit)));
  _consumerlist.push_back(ConsumerEntry("WedgeMon","b0dap30.fnal.gov",9091,110,10001,Crashed,ctime(&zeit)));
  _consumerlist.push_back(ConsumerEntry("XMon","b0dap30.fnal.gov",9092,200,10001,Running,ctime(&zeit)));
 _consumerlist.push_back(ConsumerEntry("ObjectMon","b0dap30.fnal.gov",9092,1200,9999,Running,ctime(&zeit)));
}

  
int ConsumerList::readWebPage()
{
  std::ostringstream message;
  std::string page,temp;
  int pos,last,pos2=0;
  char buffer[5000];
  TUrl website(_websitename);
  //open Socket connection
  TSocket sock(website.GetHost(),website.GetPort());
  if(!(sock.IsValid())) return 0;
  // build request
  message << "GET " << website.GetFile() << " HTTP/1.0" << std::endl 
	  << "Host:" << website.GetHost() << ":" << website.GetPort() 
	  << std::endl
	  << "Accept: */*" << std::endl << std::endl << std::ends;
  // get page
  temp = message.str();
  if (sock.SendRaw(temp.c_str(),temp.length()) != temp.length())
    return 0;
  sock.RecvRaw(buffer,5000);
  sock.Close();
  // check title
  page = buffer;
  pos  = page.find("<title>")+7;
  temp = page.substr(pos,page.find("</title>")-pos);
  if (temp != "Consumer Status Page") return 0;
  //fill the consumerlist
  _consumerlist.erase(_consumerlist.begin(),_consumerlist.end());
  pos = page.find("<tr>");
  last = page.rfind("</tr>");
  if (pos==page.rfind("<tr>")) return 0; //no entries
  pos = page.find("<tr>",pos+3);
  while(pos<last)
    {
      //read line
      ConsumerEntry ent;
      pos = page.find("<td>",pos) + 4;
      temp = page.substr(pos,page.find("</td>",pos)-pos).c_str();
      pos2 = temp.find(">");
      //std::cout << temp << "  " << pos2 << std::endl;
      if(pos2 > 0)
	  ent.name = temp.substr(pos2 + 1,temp.rfind("</font>")-pos2-1).c_str();
      else ent.name = temp.c_str();
      pos = page.find("<td>",pos) + 4; 
      temp = page.substr(pos,page.find("</td>",pos)-pos).c_str();
      pos2 = temp.find(">");
      //std::cout << temp << "  " << pos2 << std::endl;
      if(pos2 > 0)
	  ent.hostname = temp.substr(pos2 + 1,temp.rfind("</font>")-pos2-1).c_str();
      else ent.hostname = temp.c_str();
      pos = page.find("<td>",pos) + 4;
      ent.port = atoi(page.substr(pos,page.find("</td>",pos)-pos).c_str());
      pos = page.find("<td>",pos) + 4;
      ent.runnumber = atoi(page.substr(pos,page.find("</td>",pos)-pos).c_str());
      pos = page.find("<td>",pos) + 4;
      ent.nevents = atoi(page.substr(pos,page.find("</td>",pos)-pos).c_str());
      pos = page.find("<td>",pos) + 4;
      temp = page.substr(pos,page.find("</td>",pos)-pos);
      pos2 = temp.find(">") ;
      //std::cout << temp << "  " << pos2 << std::endl;
      if(pos2 > 0)
	temp = temp.substr(pos2 + 1,temp.rfind("</font>") - pos2 - 1).c_str();
      for(int i = 0; i < 6; i++)
	if (strcmp(temp.c_str(),_statusnames[i])==0) 
	  {
	    ent.status = i;
	    break;
	  }
      pos =  page.find("</tr>",pos);
      _consumerlist.push_back(ent);
    }
  return 1;
}

int ConsumerList::writeWebPage(const char * filename)
{

  int max_runnumber = 0;
  for(std::vector<ConsumerEntry>::const_iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)
    {
      if (iter->runnumber > max_runnumber)
	max_runnumber = iter->runnumber;
    } 
  
  // sort consumerlist
  std::sort(_consumerlist.begin(),_consumerlist.end(),sortbyName);
  
  const time_t zeit = time(0);
  //open file
  
  std::ofstream page(filename);
  if (!page) return 0;
  //write head of page 
  page << "<html> \n <head> \n <title>Consumer Status Page</title>" << std::endl;;
  page << "<meta http-equiv=\"cache-control\" content=\"no-cache\">" << std::endl;
  page << "<meta http-equiv=\"pragma\" content=\"no-cache\">" << std::endl;
  page << "<meta http-equiv=\"Refresh\" content=\"30;url=" << _websitename 
       << "\" >" << std::endl;                            
  page << "\n </head> \n <body background=\"home/cdfii_ybgr.jpg\"> \n ";
  page << "\n <table width=100% border=0> \n <tr> \n";
  page << "\n <td width=30%>&nbsp;</td> \n";
  page << "\n <td align=center><u><font color=\"#FF0000\" size=+2><b> List of Servers</b> </font></u></td> \n";
  page << "\n <td width=30% align=right><img src=\"home/cdfii_logo.gif\" align=right alt=\"CDF II\" height=100 width=100> </td>\n";
  page << "\n </tr> \n </table> \n";
  page << "<br> \n Status of Consumerserver Processes: \n";
  page << "<br> \n <center> <table BORDER COLS=7 WIDTH=\"100%\" NOSAVE> \n";
  page << "<tr> \n <td> Consumer </td> \n \n";
  page << "<td>Hostname</td> \n \n <td>Port</td> \n \n";
  page << "<td>Run number</td> \n \n" ;
  page << "<td># of Events processed</td> \n \n <td>Server Status</td>\n<td>Updated</td>";
  page << "\n \n <td>Time</td> \n </tr> \n";
  
  // fill table
  for(std::vector<ConsumerEntry>::const_iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)
    {
      char* update;
      char* color;
      if (iter->status == Running)
	{
	  update = "Y";
	  color  = "Green";
	}
      else
	{
	  update = "N";
	  color  = "Red";
	}

      if(iter->status == Crashed)
	page << "<tr> \n <td>" << "<font color=\"#FF0000\">"
	     << iter->name << "</font></td><td>" << "<font color=\"#FF0000\">"
	     << iter->hostname << "</font>"
	     << "</td><td>" << iter->port <<  "</td><td>"
	     << iter->runnumber << "</td><td>"
	     << iter->nevents << "</td><td>" << "<font color=\"#FF0000\">"
	     <<_statusnames[iter->status] << "</font>"  
	     << "</td>"<<"<td><font color=\"Red\">N</font></td>"<<"<td>" << iter->time << "</td></tr>\n";
      else if(iter->runnumber < max_runnumber)
	page << "<tr> \n <td>" << "<font color=\"#888888\">"
	     << iter->name << "</font></td><td>" << "<font color=\"#888888\">"
	     << iter->hostname << "</font>"
	     << "</td><td>" << iter->port <<  "</td><td>"
	     << iter->runnumber << "</td><td>"
	     << iter->nevents << "</td><td>" << "<font color=\"#888888\">"
	     <<"Old" << "</font>"  
	     << "</td>"<<"<td><font color=\""<<color<<"\">"<<update<<"</font></td>"<<"<td>" << iter->time << "</td></tr>\n";
      else
	page << "<tr> \n <td>" << iter->name << "</td><td>" << iter->hostname
	     << "</td><td>" << iter->port <<  "</td><td>"
	     << iter->runnumber << "</td><td>"
	     << iter->nevents << "</td><td>" << _statusnames[iter->status] 
	     << "</td>"<<"<td><font color=\""<<color<<"\">"<<update<<"</font></td>"<<"<td>" << iter->time << "</td></tr>\n";
    }
  // write end of page , time stamp 
  page << "</center> </table>\n";
  page << "<br> <br> <br> \n Last Update:" << ctime(&zeit) << "<br> \n";
  page << "</body>\n </html> \n";
  page.close();
  return 1;
}

void ConsumerList::print(std::ostream& output) const
{
  output << "ConsumerList:" << std::endl;
  output.width(15);
  output.setf(std::ios::left,std::ios::adjustfield);
  output << "name"; 
  output.width(30);
  output.setf(std::ios::left,std::ios::adjustfield);
  output << "hostname";
  output.width(6);
  output.setf(std::ios::right,std::ios::adjustfield);
  output << "port";
  output.width(7);
  output.setf(std::ios::right,std::ios::adjustfield);
  output << " runn";
  output.width(8);
  output.setf(std::ios::right,std::ios::adjustfield);
  output << "events";
  output.width(10);
  output.setf(std::ios::right,std::ios::adjustfield);
  output <<  " status" << std::endl; 
  for(std::vector<ConsumerEntry>::const_iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)    
    {
      output.width(15);
      output.setf(std::ios::left,std::ios::adjustfield);
      output << iter->name; 
      output.width(30);
      output.setf(std::ios::left,std::ios::adjustfield);
      output << iter->hostname;
      output.width(6);
      output.setf(std::ios::right,std::ios::adjustfield);
      output << iter->port;
      output.width(7);
      output.setf(std::ios::right,std::ios::adjustfield);
      output << iter->runnumber;
      output.width(8);
      output.setf(std::ios::right,std::ios::adjustfield);
      output << iter->nevents;
      output.width(10);
      output.setf(std::ios::right,std::ios::adjustfield);
      output << _statusnames[iter->status] << std::endl;
    }
}

int ConsumerList::sendList(const char *host,const int port)
{
  std::ostringstream message;
  std::string temp;

  TSocket sock(host,port);
  if (!(sock.IsValid())) 
    {
      std::cout << "ConsumerList::sendList: socket not valid" << std::endl;
      return 0;
    }
  message << "ConsumerList" << std::endl;
  for(std::vector<ConsumerEntry>::const_iterator iter = _consumerlist.begin();
      iter != _consumerlist.end(); ++iter)
    message << iter->name << std::endl << iter->hostname << std::endl 
	    << iter->port << std::endl << iter->runnumber << std::endl 
	    << iter->nevents << std::endl << iter->status << std::endl;
  message << "END" << std::endl;
  temp = message.str();
  if (sock.SendRaw(temp.c_str(),temp.length()) != temp.length())
    {
      sock.Close();
      return 0;
    }
  sock.Close();
  return 1;
}


void ConsumerList::addEntry(TString nam, TString hostnam,
			    int port,int nev,int runn,int stat,TString tm)
{
  //try to remove old entry first
  removeEntry(hostnam,port);
  _consumerlist.push_back(ConsumerEntry(nam,hostnam,port,nev,runn,stat,tm));
}

ConsumerList::ConsumerEntry::ConsumerEntry(TString nam,TString hostnam,
					   int p,int nev,int runn, int stat,
					   TString tm)
  : name(nam), hostname(hostnam), nevents(nev), runnumber(runn) , 
  status(stat) , port(p) , time(tm)
{
}

ConsumerList::ConsumerEntry::ConsumerEntry() 
  : name(""), hostname(""), nevents(0), runnumber(0) 
  , status(0), port(0) ,time("")
{
}

bool ConsumerList::sortbyName(const ConsumerList::ConsumerEntry a, 
			      const ConsumerList::ConsumerEntry b)
{ 
  return a.name < b.name;
}

