// $Id: EmuCompareMcsFiles.cc,v 1.2 2011/06/30 22:00:09 liu Exp $

#include "emu/pc/EmuCompareMcsFiles.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

namespace emu {
  namespace pc {

EmuCompareMcsFiles::EmuCompareMcsFiles(xdaq::ApplicationStub * s): xdaq::WebApplication(s)
{	
  //
  xgi::bind(this,&EmuCompareMcsFiles::Default, "By_Jinghua_Liu//");
  xgi::bind(this,&EmuCompareMcsFiles::Default, "Default");
  xgi::bind(this,&EmuCompareMcsFiles::MainPage, "MainPage");
  xgi::bind(this,&EmuCompareMcsFiles::SwitchBoard, "SwitchBoard");
  xgi::bind(this,&EmuCompareMcsFiles::SelectFile, "SelectFile");
  xgi::bind(this,&EmuCompareMcsFiles::Compare, "Compare");

  //
  this->getApplicationInfoSpace()->fireItemAvailable("FastLoop", &fastloop);
  this->getApplicationInfoSpace()->fireItemAvailable("SlowLoop", &slowloop);
  this->getApplicationInfoSpace()->fireItemAvailable("ExtraLoop", &extraloop);

  bbuf1=(char *)malloc(64*1024*1024);
  bbuf2=(char *)malloc(64*1024*1024);
  if(bbuf1==NULL || bbuf2==NULL) exit(-1);
  temp_mcsname[0]="Temp_Mcs_File1.mcs";
  temp_mcsname[1]="Temp_Mcs_File2.mcs";
  newfile=false;
  compared=false;
  identical=false;
}  

void EmuCompareMcsFiles::SwitchBoard(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  Page=cgiEnvi.getQueryString();
  std::string command_name=Page.substr(0,Page.find("=", 0) );
  std::string command_argu=Page.substr(Page.find("=", 0)+1);

  if (command_name=="FAST")
  {
     int interval = atoi(command_argu.c_str());
     std::cout << "Received " << interval << std::endl;
  }
}

void EmuCompareMcsFiles::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<head> <meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\"> </head>" <<std::endl;
}


void EmuCompareMcsFiles::MainPage(xgi::Input * in, xgi::Output * out ) 
     throw (xgi::exception::Exception) 
{
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  std::string myUrn = "By_Jinghua_Liu";
  xgi::Utils::getPageHeader(out,"Compare MCS (PROM Image) Files",myUrn,"", "");

  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Select MCS file #1").set("style","color:blue") << std::endl ;
  //
  std::string methodUpload = toolbox::toString("/%s/SelectFile",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
  *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","80") << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Select")  << std::endl ;
  *out << cgicc::input().set("type","hidden").set("value","1").set("name","MCS");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Select MCS file #2").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
  *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","80") << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Select")  << std::endl ;
  *out << cgicc::input().set("type","hidden").set("value","2").set("name","MCS");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Compare 2 MCS files").set("style","color:blue") << std::endl ;
  //
  for(int i=0; i<2; i++)
  {
    if(file_loaded[i])
    {
       *out << "File #" << i+1 << ": " << original_file[i] << "<br>" << std::endl;
       *out << "---->> File Size=" << fsize[i] << "<br>" << std::endl;
       if(file_checked[i])
       { 
          if(end_add[i]>0 && end_add[i]>start_add[i])
            *out << "------>>>> Start Address=0x" << std::hex << start_add[i] << "; End Address=0x" << end_add[i] << std::dec << "<br>" << std::endl;
          else
            *out << "------>>>> Not a valid MCS file! <br>" << std::endl;
       }
       *out<<cgicc::hr();
    }
  }
  std::string compare = toolbox::toString("/%s/Compare",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","POST").set("action",compare) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Compare")  << std::endl ;
  *out << cgicc::form() << std::endl ;

  if(compared && byte_error==0)
  {
     *out << "<br> <br> Contents are identical! <br>" << std::endl;
  }
  else if(compared && byte_error>0)
  {
     *out << "<br> <br> Total " << bit_error << " bit errors in " << byte_error << " bytes." << cgicc::br() << cgicc::hr() << std::endl;

     if(byte_error>200) 
         *out << cgicc::b("Different bytes (first 200)") << cgicc::br() << std::endl;
     else
         *out << cgicc::b("Different bytes") << cgicc::br() << std::endl;

     *out << cgicc::textarea().set("name","commands").set("WRAP","OFF").set("rows","20").set("cols","60");
        char bitprint[100];
        char *bittitle=" Address         File #1         File #2       \n";
        memcpy(bitprint, bittitle, strlen(bittitle));
        *out << bittitle << std::endl;
        // display difference
        int max_display = (byte_error < 200) ? byte_error : 200;
        for(int i=0; i< max_display; i++)
        {
             sprintf(bitprint, "%08X", error_idx[i]);
             bitprint[8]=' ';
             int bindata=bbuf1[error_idx[i]];
             int mask=128;
             for(int j=0;j<8;j++)
             {
                bitprint[16+j] = (mask & bindata)?'1':'0';
                mask >>= 1;
             }
             bindata=bbuf2[error_idx[i]];
             mask=128;
             for(int j=0;j<8;j++)
             {
                bitprint[32+j] = (mask & bindata)?'1':'0';
                mask >>= 1;
             }
           for(int i=0; i<42; i++) *out << bitprint[i];
           *out << std::endl;
        }
     *out << std::dec;
     *out << cgicc::textarea() << cgicc::br() << std::endl; 

  }
  *out << cgicc::fieldset() << std::endl;
  return;
  //
}

  void EmuCompareMcsFiles::SelectFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {

   try {
	std::cout << "MCS File Upload" << std::endl ;
	
	cgicc::Cgicc cgi(in);
	
	cgicc::const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");

        int mcs=0;
        cgicc::form_iterator name = cgi.getElement("MCS");
        if(name != cgi.getElements().end()) 
        {
            mcs = cgi["MCS"]->getIntegerValue();
        }
        if(mcs==1 || mcs==2)
        {
 	    if(file != cgi.getFiles().end())
            {
	       std::cout << "MCS file #" << mcs << std::endl;           
               original_file[mcs-1] = file->getFilename();
               std::cout << "File name is: " << original_file[mcs-1]  << std::endl ;
               fsize[mcs-1]=file->getDataLength();
               if(fsize[mcs-1]>0)
               {
	          std::ofstream TextFile;
	          TextFile.open(temp_mcsname[mcs-1]);
	          (*file).writeToStream(TextFile);
	          TextFile.close();

                  // loading a new file resets these flags 
                  newfile=true;
                  file_loaded[mcs-1]=true;
                  file_checked[mcs-1]=false;
                  compared=false;
               }
	    }
        }

	std::cout << "MCS File Upload done" << std::endl ;
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
	this->Default(in,out);
	//
  }


void EmuCompareMcsFiles::Compare(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{
   FILE  *inf[2]={NULL, NULL};
   int j, rdback[2]={0,0};

   if(!newfile) goto imm_return;
   byte_error=0;
   bit_error=0;
   for(int i=0; i<2; i++)
   {
      if(file_loaded[i])
      {
         inf[i]=fopen(temp_mcsname[i], "r");
         if(inf[i]==NULL)
            std::cout << "Can't open file " <<  temp_mcsname[i] << std::endl; 
         else
            rdback[i]=binsize(i+1, inf[i]);
      }
   }
   if(rdback[0]<=0 || rdback[1]<=0)  goto return_here;

   common_start = (start_add[0]>=start_add[1])?start_add[0]:start_add[1];
   common_end = (end_add[0]<=end_add[1])?end_add[0]:end_add[1];
   comp_size=common_end-common_start;
   if(comp_size<=0) goto return_here;

/* read file #1 */
   rdback[0]=readmcs(bbuf1, common_start, common_end, inf[0]);
   std::cout << "Read File #1 from " << std::hex << common_start << " to " << common_end << std::endl;

/* read file #2 */
   rdback[1]=readmcs(bbuf2, common_start, common_end, inf[1]);
   std::cout << "Read File #2 from " << common_start << " to " << common_end << std::dec << std::endl;

/* compare two buffers */
   for(unsigned i=0; i<comp_size; i++)
   {
      j=bitcmp(bbuf1[i], bbuf2[i]);
      if(j) 
      {  
          if(byte_error<200) error_idx[byte_error]=i;
          byte_error++;
          bit_error += j;
      }
   }
   compared=true;
   if(bit_error)
      std::cout << "Total compared " << comp_size << " bytes: " << bit_error << " bit error(s) in " << byte_error << " byte(s) found." << std::endl;
   else
      std::cout << "Total compared " << comp_size << " bytes: no error found." << std::endl;

return_here:
   if(inf[0]!=NULL) fclose(inf[0]);
   if(inf[1]!=NULL) fclose(inf[1]);

imm_return:
   this->Default(in,out);
}

int EmuCompareMcsFiles::bitcmp(char s, char c)
{
   int i;
   if (s==c) return 0;
   int diffs=0;
   for(i=0; i<8; i++)
   {
      if ((s&1)!=(c&1)) diffs++;
      s >>= 1;
      c >>= 1;
   }
   return diffs;
}

int EmuCompareMcsFiles::binsize(int mcs, FILE *finp)
{
   unsigned global_low=0xFFFFFFFF, global_hi=0;
   unsigned ext_add, loc_add, dsize, current_ext=0, current_add;
   char buf[1024], addbuf[5]={0,0,0,0,0}, lenbuf[3]={0,0,0};
   int finish=0, segmented=0, lines=0, badlines=0;
   char *tag1="995566AA", *tag2="5599AA66";
   int found1=0, found2=0, byte_swap=0;

   if (mcs<1 || mcs>2) return 0;
   rewind(finp);
   fgets(buf, 1020, finp);
   while(finish==0 && !feof(finp))
   {
       lines++;
       if(buf[0]!=':' || buf[7]!='0')
       { 
          fgets(buf, 1020, finp);
          badlines++;
          continue;
       }
       switch (buf[8])
       {
           case '0':
               strncpy(lenbuf,buf+1,2);
               sscanf(lenbuf, "%x", &dsize);
               strncpy(addbuf,buf+3,4);
               sscanf(addbuf, "%x", &loc_add);
               current_add=loc_add + (current_ext<<(segmented?4:16));
               if(current_add < global_low) global_low= current_add;
               current_add += dsize;
               if(current_add > global_hi) global_hi=current_add;
               if(lines<20)
               {
                  if(strcasestr(buf+9, tag1)) found1=1;
                  if(strcasestr(buf+9, tag2)) found2=1;
               }
               break;
           case '1':
               finish=1;
               break;
           case '2':
           case '4':
               if(buf[1]!='0' || buf[2]!='2')
               {
                  printf("ERROR: invalid type extended address record at line: %d!\n", lines);
                  break;
               }
               strncpy(addbuf,buf+9,4);
               sscanf(addbuf, "%x", &ext_add);
               current_ext=ext_add;
               segmented = (buf[8]=='2'?1:0);
               break;
       }
       fgets(buf, 1020, finp);
   }
   start_add[mcs-1]=global_low;
   end_add[mcs-1]=global_hi;
   file_checked[mcs-1]=true;
   if(found1==1 && found2==0) byte_swap=1;
   if(global_hi>global_low) return 1+byte_swap;
   else return 0;
}

int EmuCompareMcsFiles::readmcs(char *binbuf, unsigned start_address, unsigned end_address, FILE *finp)
{
   unsigned ext_add, loc_add, dsize, current_ext=0, current_add, index, i, j;
   char buf[1024], addbuf[5]={0,0,0,0,0}, lenbuf[3]={0,0,0};
   int finish=0, segmented=0, lines=0, c, n;
   int total_read=0;

   rewind(finp);
   fgets(buf, 1020, finp);
   while(!finish && !feof(finp))
   {
       lines++;
       if(buf[0]!=':' || buf[7]!='0') continue;
       switch (buf[8])
       {
           case '0':
               strncpy(lenbuf,buf+1,2);
               sscanf(lenbuf, "%x", &dsize);
               strncpy(addbuf,buf+3,4);
               sscanf(addbuf, "%x", &loc_add);
               current_add=loc_add + (current_ext<<(segmented?4:16));
               if(current_add >= start_address && current_add < end_address)
               {
                   index=current_add-start_address;
                   for(i=0; i<dsize; i++)
                   {
                       if(current_add+i<end_address)
                       {
                          strncpy(lenbuf, buf+9+i*2, 2);
                          sscanf(lenbuf, "%x", &c);
                          n=0;
                          for(j=0;j<8;j++)
                          {
                             n <<= 1;
                             n |= (c & 1);
                             c >>= 1;
                          }
                          binbuf[index+i] = n&0xFF;
                          total_read++;
                       }
                   }
               }
               break;
           case '1':
               finish=1;
               break;
           case '2':
           case '4':
               if(buf[1]!='0' || buf[2]!='2')
               {
                  /* printf("ERROR: invalid type extended address record at line: %d!\n", lines); */
                  break;
               }
               strncpy(addbuf,buf+9,4);
               sscanf(addbuf, "%x", &ext_add);
               current_ext=ext_add;
               segmented = (buf[8]=='2'?1:0);
               break;
       }
       fgets(buf, 1020, finp);
   }
   return total_read;
}

 }  // namespace emu::pc
}  // namespace emu

//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuCompareMcsFiles)
//
