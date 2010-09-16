#include "emu/pc/EmuPeripheralCrateConfig.h"
#include "emu/pc/EMU_CC_constants.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

namespace emu {
  namespace pc {

const std::string       VMECC_FIRMWARE_DIR = "vcc"; 

//
// BGB New Controller Utils start here 
//

void EmuPeripheralCrateConfig::VCCHeader(xgi::Input * in, xgi::Output * out, std::string title, std::string heading ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  *out << cgicc::title(title) << std::endl;
  //
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  //  xgi::Utils::getPageHeader(out,title,myUrn,"link","src here");
  //  xgi::Utils::getPageHeader(out,title,myUrn,"","");

  *out << "<table cellpadding=\"2\" cellspacing=\"2\" border=\"0\" style=\"width: 100%; font-family: arial;\">" << std::endl;
  *out << "<tbody>" << std::endl;
  *out << "<tr>" << std::endl;
  //  *out << "<td valign=middle style=\"height: 20px; width: 20px;\"><img border=\"0\" src=\"/usr/images/physics_web.png\" title=\"OSU_Phys\" alt=\"OSU_Phys\"></td><td valign=middle style=\"text-align: left; \">" << std::endl;
  *out << "<td valign=middle style=\"height: 20px; width: 20px;\"><img border=\"0\" src=\"/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"XDAQ_logo\"></td><td valign=middle style=\"text-align: left; \">" << std::endl;
  *out << "<p style=\"color:#003366; font-size=200%; font-style: italic; font-weight: bold;\">" << std::endl;
  *out << heading << std::endl;
  *out << "</td></tr></tbody></table>" << std::endl;
  *out << "<hr style=\"width: 100%; height: 1px;\"><br>" << std::endl;

}
void EmuPeripheralCrateConfig::ControllerUtils_Xfer(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
    VCC_UTIL_curr_crate = thisCrate;
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    for(int i=0; i<(int)crateVector.size(); i++){
      crateVector[i]->vmeController()->SetUseDCS(true);
      crateVector[i]->vmeController()->init();
    }
    this->ControllerUtils(in,out);
}

void EmuPeripheralCrateConfig::ControllerUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  Crate *lccc;
  static bool first = true;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    if ( lccc->vmeController()->GetDebug() == 0 ) {
      VCC_UTIL_cmn_tsk_dbg = "Disabled";
    } else {
      VCC_UTIL_cmn_tsk_dbg = "Enabled";
    }
    VCC_UTIL_cmn_tsk_sn="-";
    VCC_UTIL_cmn_tsk_cc="Not yet read back";
    VCC_UTIL_cmn_tsk_lpbk = "---";
    VCC_UTIL_cmn_tsk_lpbk_color = "#000000";
    VCC_UTIL_acc_cntrl = "disabled";
    VCC_UTIL_expert_pswd = "give me control now!";
  }

  char title[] = "VCC Utilities: Main Page";
  char pbuf[300];

  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));

  VCCHeader(in,out,title,pbuf);

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_CMNTSK_DO =
     toolbox::toString("/%s/VCC_CMNTSK_DO",getApplicationDescriptor()->getURN().c_str());


  *out << "<form action=\"" << VCC_CMNTSK_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Common Tasks</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"none\" style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Toggle Debug\" name=\"cmntsk_tog_dbg\"></td>" << std::endl;
  *out << "      <td align=\"left\">" << VCC_UTIL_cmn_tsk_dbg << "</td>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"VCC S/N\" name=\"cmntsk_vccsn\"></td>" << std::endl;
  *out << "      <td align=\"left\">" << VCC_UTIL_cmn_tsk_sn << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Firmware Version\" name=\"cmntsk_cc\"></td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"left\">" << VCC_UTIL_cmn_tsk_cc << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Load PROM\" name=\"cmntsk_ldfrmw\"></td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"left\"> With firmware version " << vmecc->VCC_frmw_ver << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Reload FPGA\" name=\"cmntsk_reload\"></td>" << std::endl;
  *out << "	 <td align=\"right\"><input type=\"submit\" value=\"Loopback Test\" name=\"cmntsk_lpbk\"></td>" << std::endl;
  *out << "      <td align=\"left\" style=\"color: " << VCC_UTIL_cmn_tsk_lpbk_color << "\">" << VCC_UTIL_cmn_tsk_lpbk << "</td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_CMNTSK;
}

void EmuPeripheralCrateConfig::VMECC_UTIL_Menu_Buttons(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::string VMECCGUI_GoTo_General =
    toolbox::toString("/%s/VMECCGUI_GoTo_General",getApplicationDescriptor()->getURN().c_str());
  std::string VMECCGUI_GoTo_Intermediate =
    toolbox::toString("/%s/VMECCGUI_GoTo_Intermediate",getApplicationDescriptor()->getURN().c_str());
   std::string VMECCGUI_GoTo_Expert =
     toolbox::toString("/%s/VMECCGUI_GoTo_Expert",getApplicationDescriptor()->getURN().c_str());
   std::string VCC_CRSEL_DO =
     toolbox::toString("/%s/VCC_CRSEL_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<div align=\"left\">" << std::endl;
  *out << "  <table width=\"100%\" cellpadding=\"4\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td align=\"left\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_General << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"left\">General Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Common Tasks\" name=\"gt_cut\" style=\"background-color: #88CCCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Yellow Page\" name=\"gt_ypg\" style=\"background-color: #FFFF00;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"CrateConfig\" name=\"gt_crc\" style=\"background-color: #00FF00;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"left\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_Intermediate << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"left\">Intermediate Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"left\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"VME Access\" name=\"gt_vmeacc\" style=\"background-color: #008800;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"center\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VMECCGUI_GoTo_Expert << "\" method=\"GET\">" << std::endl;
  *out << "          <fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Expert Users</legend>" << std::endl;
  *out << "            <table cellpadding=\"4\">" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Expert Mode\" name=\"gt_pswd\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "	             <input type=\"submit\" value=\"User Mode\" name=\"st_usrmd\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "          	   <input type=\"submit\" value=\"Misc. Commands.\" name=\"gt_misc\" style=\"background-color: #FFCC88;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"External FIFO\" name=\"gt_fifo\" style=\"background-color: #CCCCFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Config Regs\" name=\"gt_crs\" style=\"background-color: #FFCCFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"MAC Addr's\" name=\"gt_mac\" style=\"background-color: #FFFFCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "              <tr>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Send Packets\" name=\"gt_pktsnd\" style=\"background-color: #FFCCCC;\" " << VCC_UTIL_acc_cntrl << ">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Receive Packets\" name=\"gt_pktrcv\" style=\"background-color: #CCFFCC;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "                <td align=\"center\">" << std::endl;
  *out << "                  <input type=\"submit\" value=\"Firmware\" name=\"gt_frmw\" style=\"background-color: #CCFFFF;\">"  << std::endl;
  *out << "                </td>" << std::endl;
  *out << "              </tr>" << std::endl;
  *out << "            </table>" << std::endl;
  *out << "          </fieldset>" << std::endl;
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td align=\"right\" valign=\"top\">" << std::endl;
  *out << "        <form action=\"" << VCC_CRSEL_DO << "\" method=\"GET\">" << std::endl;
                     this->VMECC_UTIL_Crate_Selection(in,out);
  *out << "        </form>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;

}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_General(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_General"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_cut_name = cgi.getElement("gt_cut");
    cgicc::form_iterator gt_ypg_name = cgi.getElement("gt_ypg");
    cgicc::form_iterator gt_crc_name = cgi.getElement("gt_crc");

    if(gt_cut_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
    else if(gt_ypg_name != cgi.getElements().end()) {
      this->MainPage(in,out);
    }
    else if(gt_crc_name != cgi.getElements().end()) {
      this->CrateConfiguration(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_Intermediate(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_Intermediate"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_vmeacc_name = cgi.getElement("gt_vmeacc");

    if(gt_vmeacc_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#008800\"";
      this->VMECCGUI_VME_access(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECCGUI_GoTo_Expert(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    std::cout<<" entered VMECCGUI_GoTo_Expert"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator gt_pswd_name = cgi.getElement("gt_pswd");
    cgicc::form_iterator st_usrmd_name = cgi.getElement("st_usrmd");
    cgicc::form_iterator gt_frmw_name = cgi.getElement("gt_frmw");
    cgicc::form_iterator gt_crs_name = cgi.getElement("gt_crs");
    cgicc::form_iterator gt_mac_name = cgi.getElement("gt_mac");
    cgicc::form_iterator gt_fifo_name = cgi.getElement("gt_fifo");
    cgicc::form_iterator gt_pktsnd_name = cgi.getElement("gt_pktsnd");
    cgicc::form_iterator gt_pktrcv_name = cgi.getElement("gt_pktrcv");
    cgicc::form_iterator gt_misc_name = cgi.getElement("gt_misc");

    if(gt_pswd_name != cgi.getElements().end()) {
      std::string VCC_PSWD_DO =
        toolbox::toString("/%s/VCC_PSWD_DO",getApplicationDescriptor()->getURN().c_str());

      *out << "<form action=\"" << VCC_PSWD_DO << "\" method=\"GET\">" << std::endl;
      *out << "<div align=\"center\">" << std::endl;
      *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Access Control</legend>" << std::endl;
      *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" style=\"border-collapse: collapse\" rules=\"none\">" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td colspan=\"2\">" << std::endl;
      *out << "        <p>Expert Mode requires<br>that your name be logged.</p>" << std::endl;
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td align=\"right\">Name: " << std::endl;
      *out << "      </td>" << std::endl;
      *out << "      <td>" << std::endl;
      *out << "        <input type=\"text\" align=\"left\" maxwidth=\"90\" size=\"9\" value=\"\" name=\"expert_name\" >";
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td align=\"right\">Password: " << std::endl;
      *out << "      </td>" << std::endl;
      *out << "      <td>" << std::endl;
      *out << "        <input type=\"password\" align=\"left\" maxwidth=\"90\" size=\"9\" value=\"\" name=\"expert_pswd\">";
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "    <tr>" << std::endl;
      *out << "      <td colspan=\"2\" align=\"center\">" << std::endl;
      *out << "        <input type=\"submit\" value=\"Enter\" name=\"expert_mode\">" << std::endl;
      *out << "      </td>" << std::endl;
      *out << "    </tr>" << std::endl;
      *out << "  </table>" << std::endl;
      *out << "</fieldset>" << std::endl;
      *out << "</div>" << std::endl;
      *out << "</form>" << std::endl;
    }
    else if(st_usrmd_name != cgi.getElements().end()) {
      VCC_UTIL_acc_cntrl = "disabled";
      switch(VCC_UTIL_curr_page){
      case VCC_CMNTSK:
        this->ControllerUtils(in,out);
	break;
      case VCC_VME:
	this->VMECCGUI_VME_access(in,out);
	break;
      case VCC_FRMUTIL:
        this->VMECCGUI_firmware_utils(in,out);
	break;
      case VCC_CNFG:
	this->VMECCGUI_cnfg_utils(in,out);
	break;
      case VCC_MAC:
	this->VMECCGUI_MAC_utils(in,out);
	break;
      case VCC_FIFO:
	this->VMECCGUI_FIFO_utils(in,out);
	break;
      case VCC_PKTSND:
	this->VMECCGUI_pkt_send(in,out);
	break;
      case VCC_PKTRCV:
	this->VMECCGUI_pkt_rcv(in,out);
	break;
      case VCC_MISC:
	this->VMECCGUI_misc_utils(in,out);
        break;
      default:
        VCC_UTIL_curr_color = "\"#88CCCC\"";
        this->ControllerUtils(in,out);
	break;
      }
    }
    else if(gt_frmw_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCFFFF\"";
      this->VMECCGUI_firmware_utils(in,out);
    }
    else if(gt_crs_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCCFF\"";
      this->VMECCGUI_cnfg_utils(in,out);
    }
    else if(gt_mac_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFFFCC\"";
      this->VMECCGUI_MAC_utils(in,out);
    }
    else if(gt_fifo_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCCCFF\"";
      this->VMECCGUI_FIFO_utils(in,out);
    }
    else if(gt_pktsnd_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCCCC\"";
      this->VMECCGUI_pkt_send(in,out);
    }
    else if(gt_pktrcv_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#CCFFCC\"";
      this->VMECCGUI_pkt_rcv(in,out);
    }
    else if(gt_misc_name != cgi.getElements().end()) {
      VCC_UTIL_curr_color = "\"#FFCC88\"";
      this->VMECCGUI_misc_utils(in,out);
    }
    else {
      VCC_UTIL_curr_color = "\"#88CCCC\"";
      this->ControllerUtils(in,out);
    }
}

void EmuPeripheralCrateConfig::VMECC_UTIL_Crate_Selection(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  std::string optgroup = "INIT";
  std::string cr_label;
  bool first;
  //
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"right\">Crate Selection</legend>" << std::endl;
  *out << "<div align=\"right\">" << std::endl;
  *out << "<select name=\"crate_sel\">" << std::endl;
  first = true;
  for(int i=0; i< (int)crateVector.size(); i++){
    cr_label = crateVector[i]->GetLabel();
    if(cr_label.compare(0,5,optgroup)!= 0){
      if(!first){
        *out << "  </optgroup>" << std::endl;
      }
      optgroup = cr_label.substr(0,5);
      *out << "  <optgroup label=\"" << optgroup << "\">" << std::endl;
    }
    if(VCC_UTIL_curr_crate == crateVector[i]){
      *out << "    <option selected value=\"" << cr_label << "\">" << cr_label << "</option>" << std::endl;
    }
    else {
      *out << "    <option value=\"" << crateVector[i]->GetLabel() << "\">" << crateVector[i]->GetLabel() << "</option>" << std::endl;
    }
  }
  *out << "  </optgroup>" << std::endl;
  *out << "</select>" << std::endl;
  *out << "<input type=\"submit\" value=\"Set Crate\" name=\"set_crate\">" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;

}

void EmuPeripheralCrateConfig::VCC_CRSEL_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  int i;
  bool found;

  std::cout<<" entered VCC_CRSEL_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  std::cout << guiStr << std::endl ;
  cgicc::form_iterator crate_sel = cgi.getElement("crate_sel");

  if(crate_sel != cgi.getElements().end()) {
    VCC_UTIL_curr_crate_name = cgi["crate_sel"]->getValue();
    std::cout << "selected " << VCC_UTIL_curr_crate_name << std::endl;
    found = false;
    for(i=0; i<(int)crateVector.size() && !found; i++){
      if(VCC_UTIL_curr_crate_name==crateVector[i]->GetLabel()){
        VCC_UTIL_curr_crate = crateVector[i];
        found = true;
      }
    }
    if(!found){
      std::cout << "Crate " << VCC_UTIL_curr_crate_name << " was not found in crate vector" << std::endl;
      VCC_UTIL_curr_crate_name = VCC_UTIL_curr_crate->GetLabel();
    }
  }
  switch(VCC_UTIL_curr_page){
  case VCC_CMNTSK:
    this->ControllerUtils(in,out);
    break;
  case VCC_VME:
    this->VMECCGUI_VME_access(in,out);
    break;
  case VCC_FRMUTIL:
    this->VMECCGUI_firmware_utils(in,out);
    break;
  case VCC_CNFG:
    this->VMECCGUI_cnfg_utils(in,out);
    break;
  case VCC_MAC:
    this->VMECCGUI_MAC_utils(in,out);
    break;
  case VCC_FIFO:
    this->VMECCGUI_FIFO_utils(in,out);
    break;
  case VCC_PKTSND:
    this->VMECCGUI_pkt_send(in,out);
    break;
  case VCC_PKTRCV:
    this->VMECCGUI_pkt_rcv(in,out);
    break;
  case VCC_MISC:
    this->VMECCGUI_misc_utils(in,out);
    break;
  default:
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    this->ControllerUtils(in,out);
    break;
  }
}

void EmuPeripheralCrateConfig::VCC_PSWD_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  std::cout<<" entered VCC_PSWD_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  cgicc::form_iterator expert_mode = cgi.getElement("expert_mode");

  if(expert_mode != cgi.getElements().end()) {
    std::string pswd_entered = cgi["expert_pswd"]->getValue();
    std::string name_entered = cgi["expert_name"]->getValue();
    if(pswd_entered == VCC_UTIL_expert_pswd){
      std::cout << "VCC expert access granted to " << name_entered << std::endl;
      VCC_UTIL_acc_cntrl = "enabled";
    }
    else {
      std::cout << "VCC expert access denied to " << name_entered << std::endl;
      VCC_UTIL_acc_cntrl = "disabled";
    }
  }
  switch(VCC_UTIL_curr_page){
  case VCC_CMNTSK:
    this->ControllerUtils(in,out);
    break;
  case VCC_VME:
    this->VMECCGUI_VME_access(in,out);
    break;
  case VCC_FRMUTIL:
    this->VMECCGUI_firmware_utils(in,out);
    break;
  case VCC_CNFG:
    this->VMECCGUI_cnfg_utils(in,out);
    break;
  case VCC_MAC:
    this->VMECCGUI_MAC_utils(in,out);
    break;
  case VCC_FIFO:
    this->VMECCGUI_FIFO_utils(in,out);
    break;
  case VCC_PKTSND:
    this->VMECCGUI_pkt_send(in,out);
    break;
  case VCC_PKTRCV:
    this->VMECCGUI_pkt_rcv(in,out);
    break;
  case VCC_MISC:
    this->VMECCGUI_misc_utils(in,out);
    break;
  default:
    VCC_UTIL_curr_color = "\"#88CCCC\"";
    this->ControllerUtils(in,out);
    break;
  }
}


void EmuPeripheralCrateConfig::VCC_CMNTSK_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  unsigned char lb_tst[] = {0x00,Loopback,0xFA,0xCE,0xBA,0xC0};
  int n,i;
  char *cc;

  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  std::cout<<" entered VCC_CMNTSK_DO"<<std::endl;
  cgicc::Cgicc cgi(in);
  const cgicc::CgiEnvironment& env = cgi.getEnvironment();
  std::string guiStr = env.getQueryString() ;
  std::cout << guiStr << std::endl ;
  cgicc::form_iterator cmntsk_tog_dbg = cgi.getElement("cmntsk_tog_dbg");
  cgicc::form_iterator cmntsk_vccsn = cgi.getElement("cmntsk_vccsn");
  cgicc::form_iterator cmntsk_cc = cgi.getElement("cmntsk_cc");
  cgicc::form_iterator cmntsk_ldfrmw = cgi.getElement("cmntsk_ldfrmw");
  cgicc::form_iterator cmntsk_reload = cgi.getElement("cmntsk_reload");
  cgicc::form_iterator cmntsk_lpbk = cgi.getElement("cmntsk_lpbk");

  if(cmntsk_tog_dbg != cgi.getElements().end()) {
    if ( lccc->vmeController()->GetDebug() == 0 ) {
      std::cout << "debug 1 " << std::endl;
      lccc->vmeController()->Debug(1);
      VCC_UTIL_cmn_tsk_dbg = "Enabled";
    } else {
      std::cout << "debug 0 " << std::endl;
      lccc->vmeController()->Debug(0);
      VCC_UTIL_cmn_tsk_dbg = "Disabled";
    }
  }
  if(cmntsk_vccsn != cgi.getElements().end()) {
    SN_t sn = vmecc->rd_ser_num();
    if(sn.status == 0){
      sprintf(ctemp,"VCC%02d",sn.sn);
      VCC_UTIL_cmn_tsk_sn = ctemp;
    }
    else {
      std::cout << "Error while reading serial number. Status=" << sn.status << "  Error type = " << std::hex << sn.err_typ << std::endl;
      VCC_UTIL_cmn_tsk_sn = "-";
    }
  }
  if(cmntsk_cc != cgi.getElements().end()) {
    cc = vmecc->read_customer_code();
    if(cc != 0){
      std::cout << "Customer Code is " << cc << std::endl;
      VCC_UTIL_cmn_tsk_cc=cc;
    } else {
      std::cout << "Customer Code readback failed!" << std::endl;
      VCC_UTIL_cmn_tsk_cc="Readback Failed!";
    }
  }
  if(cmntsk_ldfrmw != cgi.getElements().end()) {
    vmecc->set_clr_bits(SET, ETHER, ETH_CR_SPONT);
    vmecc->set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL);
    std::string PROM_Path = FirmwareDir_+VMECC_FIRMWARE_DIR;
    std::cout << "Path = " << PROM_Path << "\nVer = " << vmecc->VCC_frmw_ver << std::endl;
    vmecc->prg_vcc_prom_ver(PROM_Path.c_str(),vmecc->VCC_frmw_ver.c_str());
  }
  if(cmntsk_reload != cgi.getElements().end()) {
    vmecc->force_reload();
  }
  if(cmntsk_lpbk != cgi.getElements().end()) {
    bool tst_err = false;
    vmecc->nwbuf=6;
    for(i=0;i<vmecc->nwbuf;i++){
      vmecc->wbuf[i]=lb_tst[i];
    }
    n = vmecc->eth_write();    
    n = vmecc->eth_read();
    if(n>6){
      for(i=0;i<vmecc->nwbuf;i++){
	if(vmecc->rbuf[DATA_OFF+i]&0xFF != lb_tst[i]&0xFF) tst_err = true;
      }
      if(tst_err){
        VCC_UTIL_cmn_tsk_lpbk = "failed";
        VCC_UTIL_cmn_tsk_lpbk_color = "#FF0000";
      }
      else {
        VCC_UTIL_cmn_tsk_lpbk = "passed";
        VCC_UTIL_cmn_tsk_lpbk_color = "#000000";
      }
    }
    else {
      VCC_UTIL_cmn_tsk_lpbk = "failed";
      VCC_UTIL_cmn_tsk_lpbk_color = "#FF0000";
    }
  }
  this->ControllerUtils(in,out);
}


void EmuPeripheralCrateConfig::VMECCGUI_VME_access(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  static std::vector<std::string> board_opt,rw_opt,rw_opt_lbl;
  unsigned int opt;

  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    VCC_UTIL_VME_board="";
    VCC_UTIL_VME_sub_addr="";
    VCC_UTIL_VME_rw="";
    VCC_UTIL_VME_data="";
    VCC_UTIL_VME_send_num="1";
    VCC_UTIL_VME_add_addr="";
    VCC_UTIL_VME_add_data=""; 
    VCC_UTIL_VME_brd_sel="TMB1";
    VCC_UTIL_VME_rw_sel="Read";
    VCC_UTIL_VME_msg_data = "";
    VCC_UTIL_VME_rbk_data = "";
    board_opt.push_back("DLY");
    board_opt.push_back("TMB1");
    board_opt.push_back("DMB1");
    board_opt.push_back("TMB2"); 
    board_opt.push_back("DMB2");
    board_opt.push_back("TMB3"); 
    board_opt.push_back("DMB3");
    board_opt.push_back("TMB4"); 
    board_opt.push_back("DMB4");
    board_opt.push_back("TMB5"); 
    board_opt.push_back("DMB5");
    board_opt.push_back("MPC"); 
    board_opt.push_back("CCB");
    board_opt.push_back("TMB6"); 
    board_opt.push_back("DMB6");
    board_opt.push_back("TMB7"); 
    board_opt.push_back("DMB7");
    board_opt.push_back("TMB8"); 
    board_opt.push_back("DMB8");
    board_opt.push_back("TMB9"); 
    board_opt.push_back("DMB9");
    rw_opt_lbl.push_back("Read"); 
    rw_opt_lbl.push_back("Write"); 
    rw_opt.push_back("R"); 
    rw_opt.push_back("W"); 
 }

  char title[] = "VCC Utilities: VME Access";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_VME_DO =
     toolbox::toString("/%s/VCC_VME_DO",getApplicationDescriptor()->getURN().c_str());
   std::string VCC_VME_FILL =
     toolbox::toString("/%s/VCC_VME_FILL",getApplicationDescriptor()->getURN().c_str());

  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">VME Access</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <form action=\"" << VCC_VME_FILL << "\" method=\"GET\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Input</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Board</td>" << std::endl;
  *out << "             <td align=\"center\">SubAddr</td>" << std::endl;
  *out << "             <td align=\"center\">R/W</td>" << std::endl;
  *out << "             <td align=\"center\">Data</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>" << std::endl;
  *out << "               <select name=\"Brd_Sel\">" << std::endl;
  for(opt=0;opt<board_opt.size();opt++){
    if(board_opt[opt] == VCC_UTIL_VME_brd_sel){
      *out << "                 <option selected value=\"" << board_opt[opt] << "\">" << board_opt[opt] << "</option>" << std::endl;
    }
    else {
      *out << "                 <option value=\"" << board_opt[opt] << "\">" << board_opt[opt] << "</option>" << std::endl;
    }
  }
  *out << "               </select>" << std::endl;
  *out << "             </td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_add_addr << "\" size=\"8\" maxwidth=\"80\" name=\"add_addr\" type=\"text\"></td>" << std::endl;
  *out << "             <td>" << std::endl;
  *out << "               <select name=\"RW_Sel\">" << std::endl;
  for(opt=0;opt<rw_opt.size();opt++){
    if(rw_opt[opt] == VCC_UTIL_VME_rw_sel){
      *out << "                 <option selected value=\"" << rw_opt[opt] << "\">" << rw_opt_lbl[opt] << "</option>" << std::endl;
    }
    else {
      *out << "                 <option value=\"" << rw_opt[opt] << "\">" << rw_opt_lbl[opt] << "</option>" << std::endl;
    }
  }
  *out << "               </select>" << std::endl;
  *out << "             </td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_add_data << "\" size=\"8\" maxwidth=\"80\" name=\"add_data\" type=\"text\"></td>" << std::endl;
  *out << "             <td><input type=\"submit\" value=\"Add to list\" name=\"add2pkt\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "      </form>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <form action=\"" << VCC_VME_DO << "\" method=\"GET\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Packet Contents</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Board</td>" << std::endl;
  *out << "             <td align=\"center\">SubAddr</td>" << std::endl;
  *out << "             <td align=\"center\">R/W</td>" << std::endl;
  *out << "             <td align=\"center\">Data</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><textarea name=\"vme_board\" rows=\"5\" cols=\"6\">" << VCC_UTIL_VME_board << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_sub_addr\" rows=\"5\" cols=\"8\">" << VCC_UTIL_VME_sub_addr << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_rw\" rows=\"5\" cols=\"3\">" << VCC_UTIL_VME_rw << "</textarea></td>" << std::endl;
  *out << "             <td><textarea name=\"vme_data\" rows=\"5\" cols=\"8\">" << VCC_UTIL_VME_data << "</textarea></td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"vme_send\" type=\"submit\" value=\"Send\"></td>" << std::endl;
  *out << "             <td><input align=\"right\" value=\"" << VCC_UTIL_VME_send_num << "\" size=\"3\" maxwidth=\"30\" name=\"send_num\" type=\"text\"> time(s)</td>" << std::endl;
  *out << "             <td align=\"center\" colspan=\"2\"></td>" << std::endl;
  *out << "             <td align=\"center\"><input name=\"vme_clear\" type=\"submit\" value=\"Clear list\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "      </form>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Output</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "                <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Readback Data</legend>" << std::endl;
  *out << "                  <pre><p>" << VCC_UTIL_VME_rbk_data << "</p></pre>" << std::endl;
  *out << "                </fieldset>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "              <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "                <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Messages</legend>" << std::endl;
  *out << "                  <pre><p>" << VCC_UTIL_VME_msg_data << "</p></pre></td>" << std::endl;
  *out << "                </fieldset>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;

  VCC_UTIL_curr_page = VCC_VME;
}

void EmuPeripheralCrateConfig::VCC_VME_FILL(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{

    std::cout<<" entered VCC_VME_FILL"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator add2pkt = cgi.getElement("add2pkt");

    if(add2pkt != cgi.getElements().end()) {
      VCC_UTIL_VME_add_addr = cgi["add_addr"]->getValue();
      VCC_UTIL_VME_add_data = cgi["add_data"]->getValue();
      VCC_UTIL_VME_brd_sel  = cgi["Brd_Sel"]->getValue();
      VCC_UTIL_VME_rw_sel   = cgi["RW_Sel"]->getValue();

      VCC_UTIL_VME_board += VCC_UTIL_VME_brd_sel;
      VCC_UTIL_VME_board += "\n";

      if(VCC_UTIL_VME_brd_sel.compare(0,3,"DLY") == 0){
        VCC_UTIL_VME_sub_addr += "n/a\n";
        VCC_UTIL_VME_rw += "n/a\n";
        VCC_UTIL_VME_data += VCC_UTIL_VME_add_data;
        VCC_UTIL_VME_data += "\n";
      }
      else {
        VCC_UTIL_VME_sub_addr += VCC_UTIL_VME_add_addr;
        VCC_UTIL_VME_sub_addr += "\n";

        VCC_UTIL_VME_rw += VCC_UTIL_VME_rw_sel;
        VCC_UTIL_VME_rw += "\n";

        if(VCC_UTIL_VME_rw_sel.compare(0,3,"W") == 0){
          VCC_UTIL_VME_data += VCC_UTIL_VME_add_data;
          VCC_UTIL_VME_data += "\n";
	}
        else {
          VCC_UTIL_VME_data += "n/a\n";
	}
      }
    }
    this->VMECCGUI_VME_access(in,out);
}

void EmuPeripheralCrateConfig::VCC_VME_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  //  unsigned short int *pbuf;
  int i,n,nbrds,nvme,offset,itemp,pkt_type;
  bool abrt;
  char ctemp[256];
  char *ptemp1,*ptemp2,*ptemp3;
  struct vcmd_t {
    char brd[10];
    unsigned long addr;
    bool wrt;
    unsigned long data;
    struct vcmd_t *nxt;
    struct vcmd_t *prv;
  } vcmd,*cur,*last;


  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_VME_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator vme_send = cgi.getElement("vme_send");
    cgicc::form_iterator vme_clear = cgi.getElement("vme_clear");

    vcmd.prv=NULL;
    nbrds=0;
    abrt=false;
    if(vme_clear != cgi.getElements().end()) {
      VCC_UTIL_VME_board = "";
      VCC_UTIL_VME_sub_addr = "";
      VCC_UTIL_VME_rw = "";
      VCC_UTIL_VME_data = "";
    }
    if(vme_send != cgi.getElements().end()) {
      int send_num = cgi["send_num"]->getIntegerValue();
      VCC_UTIL_VME_send_num = cgi["send_num"]->getValue();
      VCC_UTIL_VME_board = cgi["vme_board"]->getValue();
      VCC_UTIL_VME_sub_addr = cgi["vme_sub_addr"]->getValue();
      VCC_UTIL_VME_rw = cgi["vme_rw"]->getValue();
      VCC_UTIL_VME_data = cgi["vme_data"]->getValue();
      size_t slen = VCC_UTIL_VME_board.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_board.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3=NULL;
        strcpy(cur->brd,ptemp2);
        nbrds++;
	std::cout << "brd " << nbrds << " is " << cur->brd << std::endl;
	cur->nxt = (struct vcmd_t *) malloc(sizeof(struct vcmd_t));
        cur->nxt->prv=cur;
        cur=cur->nxt;
      }
      last=cur->prv;
      free(cur);
      last->nxt=NULL;
      nvme=nbrds;
      free(ptemp1);
      slen = VCC_UTIL_VME_sub_addr.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_sub_addr.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->addr = strtoul(ptemp2,NULL,16);
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for sub address" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      slen = VCC_UTIL_VME_rw.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_rw.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->wrt=false;
        if(ptemp2[0]=='W') cur->wrt=true;
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for R/W entry" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      slen = VCC_UTIL_VME_data.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_VME_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      n=0;
      cur=&vcmd;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL && n<nbrds){
        ptemp3=NULL;
        cur->data = strtoul(ptemp2,NULL,16);
        n++;
        cur=cur->nxt;
      }
      free(ptemp1);
      if(n<nbrds){
	std::cout << " Error: not enough data provided for data entry" << std::endl;
	std::cout << " Command not executed" << std::endl;
        abrt=true;
      }
      if(!abrt){
	offset=4;
        cur=&vcmd;
        n=0;
	for (cur=&vcmd;cur!=NULL && !abrt;cur=cur->nxt){
          n++;
	  vmecc->wbuf[offset+0]=0x00;
	  if(cur->wrt){
	    vmecc->wbuf[offset+1]=0x54;
	  }
	  else {
	    vmecc->wbuf[offset+1]=0x44;
	  }
	  if(strncmp(cur->brd,"TMB",3)==0){
	    sscanf(cur->brd,"TMB%d",&itemp);
	    if(itemp>5)itemp++;
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=((itemp<<4)&0xF0)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"DMB",3)==0){
	    sscanf(cur->brd,"DMB%d",&itemp);
	    if(itemp>5)itemp++;
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=((itemp<<4)&0xF0)|(0x08)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"MPC",3)==0){
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=(0x60)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"CCB",3)==0){
	    vmecc->wbuf[offset+2]=0x00;
	    vmecc->wbuf[offset+3]=(0x68)|((cur->addr>>16)&0x7);
	    vmecc->wbuf[offset+4]=(cur->addr>>8)&0xFF;
	    vmecc->wbuf[offset+5]=cur->addr&0xFF;
	    if(cur->wrt){
	      vmecc->wbuf[offset+6]=(cur->data>>8)&0xFF;
	      vmecc->wbuf[offset+7]=cur->data&0xFF;
	      offset+=8;
	    }
	    else {
	      offset+=6;
	    }
	  }
	  else if(strncmp(cur->brd,"DLY",3)==0){
	    vmecc->wbuf[offset+0]=0x02;
	    vmecc->wbuf[offset+1]=0x00;
	    vmecc->wbuf[offset+2]=(cur->data>>8)&0xFF;
	    vmecc->wbuf[offset+3]=cur->data&0xFF;
	    offset+=4;
	  }
	  else{
            n--;
	    std::cout<<"  Illegal board option: " << cur->brd << std::endl;
	    std::cout<<"  Only executing " << n << " VME commands" << std::endl;
	    abrt=true;
	  }
	}
        nvme=n;
        vmecc->wbuf[2]=(nvme>>8)&0xFF;
        vmecc->wbuf[3]=nvme&0xFF;
        for(n=0;n<send_num;n++){
          std::cout << "VME Data: " << std::endl;
          for(i=2;i<offset;i+=2){
            printf("%02X%02X\n",vmecc->wbuf[i]&0xFF,vmecc->wbuf[i+1]&0xFF);
          }
          vmecc->vme_cmds((offset-2)/2);
	}
      }
      VCC_UTIL_VME_msg_data = "";
      VCC_UTIL_VME_rbk_data = "";
      while((pkt_type=vmecc->rd_pkt())>=0){
        if(pkt_type>INFO_PKT){
          std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
          VCC_UTIL_VME_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
          VCC_UTIL_VME_msg_data += "\n";
	}
        else if (pkt_type==VMED16_PKT){
          int nw = ((vmecc->rbuf[WRD_CNT_OFF]&0xff)<<8)|(vmecc->rbuf[WRD_CNT_OFF+1]&0xff);
	  for(i=0;i<nw;i++){
	    sprintf(ctemp,"0x%02X%02X\n",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	    VCC_UTIL_VME_rbk_data += ctemp;
	  }
	}
        else {
          sprintf(ctemp,"Pkt Type: 0x%02X\n",pkt_type);
          std::cout << ctemp << std::endl;
          VCC_UTIL_VME_msg_data += ctemp;
	}
      }
    }
    this->VMECCGUI_VME_access(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_firmware_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    for(int i=0;i<7;i++){
        VCC_UTIL_Frmw_rbk_[i]="Not yet read back";
    }
    VCC_UTIL_base_addr_inp = "000";
    VCC_UTIL_Frmw_rbk_[6] = "???";
    VCC_UTIL_Prnt_Rtns = "";
    VCC_UTIL_prom_file_inp = "D783C.V4.29.mcs"; 
  }

  char title[] = "VCC Utilities: Firmware";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_FRMUTIL_DO =
     toolbox::toString("/%s/VCC_FRMUTIL_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_FRMUTIL_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Firmware Utilities</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\"  cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\" id=\"table9\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <fieldset ><legend style=\"font-size: 16pt;\" align=\"center\" >Read Backs</legend>" << std::endl;
  *out << "          <div><table cellpadding=\"2\">" << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Device ID\" name=\"dvid\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[0] << "</td></tr>" << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"User Code\" name=\"rd_uc\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[1] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Customer Code\" name=\"rd_cc\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[2] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Check Connection\" name=\"chk_con\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[3] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"JTAG Conrtol Status\" name=\"status\"></td><td align=\"left\">" << VCC_UTIL_Frmw_rbk_[4] << "</td></tr>"  << std::endl;
  *out << "	       <tr><td align=\"right\"><input type=\"submit\" value=\"Print Routines\" name=\"prt_rtn\"></td><td>" << VCC_UTIL_Frmw_rbk_[5] << "</td></tr>"  << std::endl;
  *out << "	     </table></div>" << std::endl;
  *out << "	   </fieldset>" << std::endl;
  *out << "	 </td>" << std::endl;
  *out << "    <td valign=\"top\">" << std::endl;
  *out << "      <fieldset align=top><legend style=\"font-size: 16pt;\" align=\"center\">Inputs</legend>" << std::endl;
  *out << "        <div><table cellpadding=\"2\">" << std::endl;
  *out << "            <tr><td align = \"right\">Base Address:</td>\n";
  *out << "                <td><input align=\"right\" maxwidth=\"30\" size=\"3\" value=\"" << VCC_UTIL_base_addr_inp << "\" name=\"base_addr\" type=\"text\"></td>\n";
  *out << "                <td><input type=\"submit\" value=\"Set\" name=\"set_base\" " << VCC_UTIL_acc_cntrl << "></td>\n";
  *out << "                <td></td></tr>" << std::endl;
  *out << "            <tr><td align = \"right\">Current Value:</td>\n";
  *out << "                <td>" << VCC_UTIL_Frmw_rbk_[6] << "</td>\n";
  *out << "                <td colspan = \"2\"></td></tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "          <hr>" << std::endl;
  *out << "          <table cellpadding=\"2\">" << std::endl;
  *out << "            <tr><td align = \"right\">PROM File:</td>\n";
  *out << "                <td><input align=\"left\" value=\"" << VCC_UTIL_prom_file_inp << "\" size=\"32\" maxwidth=\"256\" name=\"mcs_file\" type=\"text\">\n";
  *out << "                <td><input type=\"submit\" value=\"Read file\" name=\"rdmcs\"></td></tr>" << std::endl;
  *out << "            <tr><td align = \"right\">File in Mem:</td>\n";
  *out << "                <td>" << VCC_UTIL_Frmw_rbk_[7] << "</td>";
  *out << "                <td></td></tr>" << std::endl;
  *out << "          </table></div>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td colspan=\"2\" valign=\"top\">" << std::endl;
  *out << "        <fieldset align=top><legend style=\"font-size: 16pt;\" align=\"center\">Configure</legend>" << std::endl;
  *out << "          <div><table cellpadding=\"2\"><tr>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Load Routines\" name=\"ld_rtn\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Erase PROM\" name=\"erase\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program PROM\" name=\"program\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program with Verify\" name=\"prgver\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Execute Custom Routine\" name=\"custom\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Write Mem to file\" name=\"wrtdat\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Read Back PROM\" name=\"rdbk\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Verify Cmd\" name=\"verify\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Program FPGA\" name=\"jreload\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "            <td><input type=\"submit\" value=\"Abort JTAG Commands\" name=\"abort\"></td>" << std::endl;
  *out << "          </tr></table></div>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "    <tr><td colspan = \"2\" align = \"center\"><pre><p>" << VCC_UTIL_Prnt_Rtns << "</p></pre>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_FRMUTIL;
}

   // call back action from GUI

void EmuPeripheralCrateConfig::VCC_FRMUTIL_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  char *cptemp;
  int i,ptyp,ack;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  CNFG_ptr rbk_cp;
  vmecc=lccc->vmecc();
    std::cout<<" entered VCC_FRMUTIL_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;
    cgicc::form_iterator dvid_name = cgi.getElement("dvid");
    cgicc::form_iterator rd_uc_name = cgi.getElement("rd_uc");
    cgicc::form_iterator rd_cc_name = cgi.getElement("rd_cc");
    cgicc::form_iterator chk_con_name = cgi.getElement("chk_con");
    cgicc::form_iterator status_name = cgi.getElement("status");
    cgicc::form_iterator prt_rtn_name = cgi.getElement("prt_rtn");
    cgicc::form_iterator set_base_name = cgi.getElement("set_base");
    cgicc::form_iterator base_addr_name = cgi.getElement("base_addr");
    cgicc::form_iterator rdmcs_name = cgi.getElement("rdmcs");
    cgicc::form_iterator mcs_file_name = cgi.getElement("mcs_file");
    cgicc::form_iterator ld_rtn_name = cgi.getElement("ld_rtn");
    cgicc::form_iterator erase_name = cgi.getElement("erase");
    cgicc::form_iterator program_name = cgi.getElement("program");
    cgicc::form_iterator prgver_name = cgi.getElement("prgver");
    cgicc::form_iterator custom_name = cgi.getElement("custom");
    cgicc::form_iterator wrtdat_name = cgi.getElement("wrtdat");
    cgicc::form_iterator rdbk_name = cgi.getElement("rdbk");
    cgicc::form_iterator verify_name = cgi.getElement("verify");
    cgicc::form_iterator jreload_name = cgi.getElement("jreload");
    cgicc::form_iterator abort_name = cgi.getElement("abort");
    unsigned int dvid=0;
    unsigned int uc=0;
    char *cc;
    int con_stat=0;
    int nl=0,n;
    unsigned int status=0;
    unsigned short int base_addr=0;
    char *pch =(char *) &VCC_UTIL_base_addr_inp[0];
    std::cout.fill('0');
    std::cout.setf(std::ios::showbase);
    if(dvid_name != cgi.getElements().end()) {
      dvid = vmecc->read_dev_id();
      if(dvid>0){
        std::cout << "Device ID is " << std::setw(8) << std::hex << dvid << std::endl;
        sprintf(ctemp,"%08X",dvid);
        VCC_UTIL_Frmw_rbk_[0]=ctemp;
      } else {
        std::cout << "Device ID readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[0]="Readback Failed!";
      }
    }
    if(rd_uc_name != cgi.getElements().end()) {
      uc = vmecc->read_user_code();
      if(uc>0){
        std::cout << "User Code is " << std::setw(8) << std::hex  << uc << std::endl;
        sprintf(ctemp,"%08X",uc);
        VCC_UTIL_Frmw_rbk_[1]=ctemp;
      } else {
        std::cout << "User Code readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[1]="Readback Failed!";
      }
    }
    if(rd_cc_name != cgi.getElements().end()) {
      cc = vmecc->read_customer_code();
      if(cc != 0){
        std::cout << "Customer Code is " << cc << std::endl;
        VCC_UTIL_Frmw_rbk_[2]=cc;
      } else {
        std::cout << "Customer Code readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[2]="Readback Failed!";
      }
    }
    if(chk_con_name != cgi.getElements().end()) {
      con_stat = vmecc->chk_jtag_conn();
      if(con_stat == 1){
        std::cout << "JTAG connection is good." << std::endl;
        VCC_UTIL_Frmw_rbk_[3]="Good.";
      }
      else {
        std::cout << "JTAG connection is BAD!" << std::endl;
        VCC_UTIL_Frmw_rbk_[3]="BAD!";
      }
    }
    if(status_name != cgi.getElements().end()) {
      status = vmecc->get_jtag_status();
      if(status>0){
        std::cout << "JTAG Module Status is " << std::setw(4) << std::hex << status << std::endl;
        sprintf(ctemp,"0x%04X",status&0xFFFF);
        VCC_UTIL_Frmw_rbk_[4]=ctemp;
      } else {
        std::cout << "Status readback failed!" << std::endl;
        VCC_UTIL_Frmw_rbk_[4]="Readback Failed!";
      }
    }
    if(prt_rtn_name != cgi.getElements().end()) {
      VCC_UTIL_Prnt_Rtns = vmecc->print_routines();
      VCC_UTIL_Frmw_rbk_[5]="See below for output";
    }
    if(set_base_name != cgi.getElements().end()) {
      VCC_UTIL_base_addr_inp = cgi["base_addr"]->getValue();
      base_addr = (unsigned short) strtol(VCC_UTIL_base_addr_inp.c_str(),&pch,16);
      vmecc->ld_rtn_base_addr(base_addr);
      std::cout << "base address set " << std::setw(4) << std::hex  << base_addr << std::endl;
      VCC_UTIL_Frmw_rbk_[6]= VCC_UTIL_base_addr_inp;
    }
    if(rdmcs_name != cgi.getElements().end()) {
      VCC_UTIL_prom_file_inp = cgi["mcs_file"]->getValue();
      strcpy(ctemp,VCC_UTIL_prom_file_inp.c_str());
      cptemp=strtok(ctemp,".");
      if(cptemp!=NULL) VCC_UTIL_proj = cptemp;
      cptemp=strtok(0,".");
      if(cptemp!=NULL) VCC_UTIL_maj_ver = cptemp+1;
      cptemp=strtok(0,".");
      if(cptemp!=NULL) VCC_UTIL_min_ver = cptemp;
      std::cout << "Project: " << VCC_UTIL_proj << std::endl;
      std::cout << "Major Version: " << VCC_UTIL_maj_ver << std::endl;
      std::cout << "Minor Version: " << VCC_UTIL_min_ver << std::endl;
      VCC_UTIL_PROM_ver = VCC_UTIL_maj_ver + "." + VCC_UTIL_min_ver;
      std::cout << "PROM  Version: " << VCC_UTIL_PROM_ver << std::endl;
      std::string Full_PROM_Filename = FirmwareDir_+VMECC_FIRMWARE_DIR+"/"+VCC_UTIL_prom_file_inp;
      nl = vmecc->read_mcs(Full_PROM_Filename.c_str());
      if(nl>0){
        if(nl==65553){
          std::cout << "File " << Full_PROM_Filename << " has been readin." << std::endl;
          VCC_UTIL_Frmw_rbk_[7]=VCC_UTIL_prom_file_inp;
          VCC_UTIL_PROM_file_init = true;
	}
	else {
          VCC_UTIL_Frmw_rbk_[7]="Readin Failed!";
          std::cout << "Readin Failed!" << std::endl;
          std::cout << "Wrong number of lines readin! Lines read = " << nl << std::endl;
	  VCC_UTIL_PROM_file_init = false;
	}
      }
      else {
        VCC_UTIL_Frmw_rbk_[7]="Readin Failed!";
        std::cout << "Readin Failed!" << std::endl;
        VCC_UTIL_PROM_file_init = false;
      }
    }
    if(ld_rtn_name != cgi.getElements().end()) {
      vmecc->jtag_init();
    }
    if(erase_name != cgi.getElements().end()) {
      int rslt = vmecc->erase_prom();
      if(rslt == 1){
        printf("PROM successfully erased.\n");
      }
      else{
        printf("\a\aPROM is not erased!\n");
      }
    }
    if(program_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
	int pktnum = 0;
        rbk_cp = vmecc->read_crs();
        vmecc->set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL);
        vmecc->set_clr_bits(SET, ETHER, ETH_CR_SPONT);
	vmecc->program_prom_cmd();
	vmecc->send_prg_prom_data();
	vmecc->send_uc_cc_data(VCC_UTIL_Frmw_rbk_[7].c_str());
	while((n=vmecc->eth_read())>6){
	  pktnum++;
	  ptyp = vmecc->rbuf[PKT_TYP_OFF]&0xff;
	  if(ptyp>=INFO_PKT){
            std::cout << "pktnum B" << pktnum << ": " << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	  } else {
            std::cout << "pktnum B" << pktnum << ": ";
	    ack = AK_STATUS(vmecc->rbuf); 
	    switch(ack){
	    case NO_ACK:
	      std::cout << "Non message or no acknowledge packet received" << std::endl;
	      break;
	    case CC_S:
	      std::cout << "Command completed successfully" << std::endl;
	      break;
	    case CC_W:
	      std::cout << "Command completed with a warning" << std::endl;
	      break;
	    case CC_E:
	      std::cout << "Command  completed with an error" << std::endl;
	      break;
	    case CE_I:
	      std::cout << "Command execution finished incomplete" << std::endl;
	      break;
	    case CIP_W: case CIP_E:
	      std::cout << "Command in progress with an error or warning" << std::endl;
	      break;
	    default:
	      std::cout << "Unknow packet returned" << std::endl;
	      break;
	    }
	  }
	}
        vmecc->wrt_crs(RST_MISC, rbk_cp);
        free(rbk_cp);
      }
      else {
        std::cout << "You must read in the mcs file first\n" << std::endl;
      }
    }
    if(prgver_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
        vmecc->set_clr_bits(SET, ETHER, ETH_CR_SPONT);
        vmecc->set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL);
        CNFG_ptr cp=vmecc->read_crs();
        vmecc->print_crs(cp);
        free(cp);
        std::string PROM_Path = FirmwareDir_+VMECC_FIRMWARE_DIR;
        std::cout << "Path = " << PROM_Path << "\nVer = " << VCC_UTIL_PROM_ver << std::endl;
        vmecc->prg_vcc_prom_ver(PROM_Path.c_str(),VCC_UTIL_PROM_ver.c_str());
      }
      else {
        std::cout << "You must set the version number by reading in the mcs file first\n" << std::endl;
      }
    }
    if(custom_name != cgi.getElements().end()) {
      vmecc->ld_rtn_base_addr(0x180);
      vmecc->exec_routine(JC_Custom_Rtn);
      while((n=vmecc->eth_read())>6){
        ptyp = vmecc->rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
	  printf("%s",vmecc->dcode_msg_pkt(vmecc->rbuf));
	} else {
          printf("Rtn_Pkt: ");
          for(i=0;i<n;i++)printf("%02X",vmecc->rbuf[i]&0xFF);
          printf("\n");
	}
      }
    }
    if(wrtdat_name != cgi.getElements().end()) {
      std::string MCS_mem_file = FirmwareDir_+"/vcc/MCS_data_inmem.dat";
      FILE *fp=fopen(MCS_mem_file.c_str(),"w");
      for(i=0;i<0x80000;i++){
        if(i!=0 && i%8==0)fprintf(fp,"\n");
        fprintf(fp,"%04hX",prm_dat[i]);
      }
      fprintf(fp,"\n");
      fclose(fp);
    }
    if(rdbk_name != cgi.getElements().end()) {
      std::string MCS_rbk_file = FirmwareDir_+"/vcc/Prom_rbk.dat";
      vmecc->rd_back_prom(MCS_rbk_file.c_str());
    }
    if(verify_name != cgi.getElements().end()) {
      if(VCC_UTIL_PROM_file_init){
        rbk_cp = vmecc->read_crs();
        vmecc->set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL);
        vmecc->set_clr_bits(SET, ETHER, ETH_CR_SPONT);
        vmecc->verify_prom_cmd();
        vmecc->send_ver_prom_data();
        vmecc->wrt_crs(RST_MISC, rbk_cp);
        free(rbk_cp);
      }
      else {
        printf("You must read in the mcs file first\n");
      }
    }
    if(jreload_name != cgi.getElements().end()) {
      vmecc->reload_fpga();
    }
    if(abort_name != cgi.getElements().end()) {
      vmecc->abort_jtag_cmnds();
    }
    this->VMECCGUI_firmware_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_cnfg_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    for(int i=0;i<6;i++){
        VCC_UTIL_CR_cur_[i]="-";
        VCC_UTIL_CR_dflt_[i]="-";
        VCC_UTIL_CR_flashA_[i]="-";
        VCC_UTIL_CR_flashB_[i]="-";
    }
    VCC_UTIL_CR_wrt_[0]="0050";
    VCC_UTIL_CR_wrt_[1]="0002";
    VCC_UTIL_CR_wrt_[2]="031B";
    VCC_UTIL_CR_wrt_[3]="EDFF1D0F";
    VCC_UTIL_CR_wrt_[4]="30D4";
    VCC_UTIL_CR_wrt_[5]="03C5";
    VCC_UTIL_CR_wrt_[6]="0";
    VCC_UTIL_CR_cnumA="0";
    VCC_UTIL_CR_cnumB="1";
    VCC_UTIL_CR_sav_cnum="0";
    VCC_UTIL_CR_rstr_cnum="0";
    VCC_UTIL_CR_dflt_cnum="0";
    VCC_UTIL_CR_curr_dflt="-";
    VCC_UTIL_CR_ser_num="-";
  }

  char title[] = "VCC Utilities: Config Regs";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_CNFG_DO =
     toolbox::toString("/%s/VCC_CNFG_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_CNFG_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Configuration Registers</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"groups\">" << std::endl;
  *out << "    <colgroup span=\"3\"><colgroup span=\"4\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <th colspan=\"3\" style=\"font-size: 14pt; color: blue\">Inputs</th>" << std::endl;
  *out << "      <th colspan=\"4\" style=\"font-size: 14pt; color: blue\">Read Backs</th>" << std::endl;
  *out << "    </tr><tr>" << std::endl; 
  *out << "      <td></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td>In Mem</td>" << std::endl;
  *out << "      <td align=\"center\">In FPGA</td>" << std::endl;
  *out << "      <td colspan=\"3\" align=\"center\">In FLASH</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"center\">Register</td>" << std::endl;
  *out << "      <td>Sel</td>" << std::endl;
  *out << "      <td align=\"center\"><input name=\"wrt_sel_crs\" type=\"submit\" value=\"Write Sel.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cur_crs\" type=\"submit\" value=\"Current\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_dflt_cnfg\" type=\"submit\" value=\"Default\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cnumA\" type=\"submit\" value=\"Cnfg #\"><input maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_cnumA << "\" name=\"cnumA\" type=\"text\"></td>" << std::endl;
  *out << "      <td><input name=\"rbk_cnumB\" type=\"submit\" value=\"Cnfg #\"><input maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_cnumB << "\" name=\"cnumB\" type=\"text\"></td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Ethernet</td>" << std::endl;
  *out << "      <td><input name=\"eth_cr_chk\" type=\"checkbox\" value=\"eth_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[0] << "\" size=\"6\" maxwidth=\"60\" name=\"eth_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[0] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[0] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Ext. FIFO</td>" << std::endl;
  *out << "      <td><input name=\"exfifo_cr_chk\" type=\"checkbox\" value=\"exfifo_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[1] << "\" size=\"6\" maxwidth=\"60\" name=\"exfifo_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "       <td align=\"center\">" << VCC_UTIL_CR_cur_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[1] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[1] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">Rst / Misc.</td>" << std::endl;
  *out << "      <td><input name=\"rstmsc_cr_chk\" type=\"checkbox\" value=\"rstmsc_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[2] << "\" size=\"6\" maxwidth=\"60\" name=\"rstmsc_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[2] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[2] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">VME</td>" << std::endl;
  *out << "      <td><input name=\"vme_cr_chk\" type=\"checkbox\" value=\"vme_cr\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[3] << "\" size=\"10\" maxwidth=\"120\" name=\"vme_cr_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[3] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[3] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">BTO</td>" << std::endl;
  *out << "      <td><input name=\"bto_chk\" type=\"checkbox\" value=\"bto\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[4] << "\" size=\"6\" maxwidth=\"60\" name=\"bto_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[4] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[4] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\">BGTO</td>" << std::endl;
  *out << "      <td><input name=\"bgto_chk\" type=\"checkbox\" value=\"bgto\"></td>" << std::endl;
  *out << "      <td align=\"right\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[5] << "\" size=\"6\" maxwidth=\"60\" name=\"bgto_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_cur_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_dflt_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashA_[5] << "</td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_CR_flashB_[5] << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"rst_crs\" type=\"reset\" value=\"Reset\"></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td align=\"center\"><input name=\"wrt_all_crs\" type=\"submit\" value=\"Write All.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Save Curr as Cnfg#\" name=\"sav_crs\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "          <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_sav_cnum << "\" name=\"sav_cnum\" type=\"text\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Restore Cnfg#\" name=\"rstr_crs\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "          <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_rstr_cnum << "\" name=\"rstr_cnum\" type=\"text\"> &nbsp to Current </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td align=\"right\" colspan=\"2\"><input name=\"wrt_ser\" type=\"submit\" value=\"Write Serial Num.\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_CR_wrt_[6] << "\" size=\"6\" maxwidth=\"30\" name=\"wrt_ser_val\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Set Default Cnfg#\" name=\"set_dflt\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "        <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_CR_dflt_cnum << "\" name=\"dflt_cnum\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Read Curr Dflt\" name=\"rd_dflt\"> &nbsp : &nbsp " << VCC_UTIL_CR_curr_dflt << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td colspan=\"3\"></td>" << std::endl;
  *out << "      <td align=\"center\" colspan=\"4\"><input type=\"submit\" value=\"Read Serial Number\" name=\"rd_ser_num\"> &nbsp : &nbsp " << VCC_UTIL_CR_ser_num << "</td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_CNFG;
}

void EmuPeripheralCrateConfig::VCC_CNFG_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  CNFG_ptr rbk_cp;
  unsigned int temp;
  char *pch =(char *) &VCC_UTIL_CR_wrt_[0];
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
    std::cout<<" entered VCC_CNFG_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;
    cgicc::form_iterator eth_cr_chk_name = cgi.getElement("eth_cr_chk");
    cgicc::form_iterator exfifo_cr_chk_name = cgi.getElement("exfifo_cr_chk");
    cgicc::form_iterator rstmsc_cr_chk_name = cgi.getElement("rstmsc_cr_chk");
    cgicc::form_iterator vme_cr_chk_name = cgi.getElement("vme_cr_chk");
    cgicc::form_iterator bto_chk_name = cgi.getElement("bto_chk");
    cgicc::form_iterator bgto_chk_name = cgi.getElement("bgto_chk");
    cgicc::form_iterator wrt_sel_crs_name = cgi.getElement("wrt_sel_crs");
    cgicc::form_iterator wrt_all_crs_name = cgi.getElement("wrt_all_crs");
    cgicc::form_iterator wrt_ser_name = cgi.getElement("wrt_ser");
    cgicc::form_iterator rbk_cur_crs_name = cgi.getElement("rbk_cur_crs");
    cgicc::form_iterator rbk_dflt_cnfg_name = cgi.getElement("rbk_dflt_cnfg");
    cgicc::form_iterator rbk_cnumA_name = cgi.getElement("rbk_cnumA");
    cgicc::form_iterator rbk_cnumB_name = cgi.getElement("rbk_cnumB");
    cgicc::form_iterator sav_crs_name = cgi.getElement("sav_crs");
    cgicc::form_iterator rstr_crs_name = cgi.getElement("rstr_crs");
    cgicc::form_iterator set_dflt_name = cgi.getElement("set_dflt");
    cgicc::form_iterator rd_dflt_name = cgi.getElement("rd_dflt");
    cgicc::form_iterator rd_ser_num_name = cgi.getElement("rd_ser_num");
    VCC_UTIL_CR_wrt_[0] = cgi["eth_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[0].c_str(),&pch,16);
    VCC_UTIL_CR_to.ether = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[1] = cgi["exfifo_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[1].c_str(),&pch,16);
    VCC_UTIL_CR_to.ext_fifo = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[2] = cgi["rstmsc_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[2].c_str(),&pch,16);
    VCC_UTIL_CR_to.rst_misc = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[3] = cgi["vme_cr_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[3].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme = (unsigned int) temp;
    VCC_UTIL_CR_wrt_[4] = cgi["bto_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[4].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme_bto = (unsigned short int) temp;
    VCC_UTIL_CR_wrt_[5] = cgi["bgto_val"]->getValue();
    temp = strtoul(VCC_UTIL_CR_wrt_[5].c_str(),&pch,16);
    VCC_UTIL_CR_to.vme_bgto = (unsigned short int) temp;

    std::cout << "Values to send to controller are:" << std::endl;
    vmecc->print_crs(&VCC_UTIL_CR_to);
    if(wrt_sel_crs_name != cgi.getElements().end()) {
      if(eth_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(0,&VCC_UTIL_CR_to);
      }
      if(exfifo_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(1,&VCC_UTIL_CR_to);
      }
      if(rstmsc_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(2,&VCC_UTIL_CR_to);
      }
      if(vme_cr_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(3,&VCC_UTIL_CR_to);
      }
      if(bto_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(4,&VCC_UTIL_CR_to);
      }
      if(bgto_chk_name != cgi.getElements().end()) {
        vmecc->wrt_crs(5,&VCC_UTIL_CR_to);
      }
    }
    if(wrt_all_crs_name != cgi.getElements().end()) {
      vmecc->wrt_crs(8,&VCC_UTIL_CR_to);
    }
    if(wrt_ser_name != cgi.getElements().end()) {
      VCC_UTIL_CR_wrt_[6] = cgi["wrt_ser_val"]->getValue();
      int wrt_sn = cgi["wrt_ser_val"]->getIntegerValue();
      vmecc->wrt_ser_num(wrt_sn);
    }
    if(rbk_cur_crs_name != cgi.getElements().end()) {
      rbk_cp = vmecc->read_crs();
      vmecc->print_crs(rbk_cp);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_cur_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_cur_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_cur_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_cur_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_cur_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_cur_[5]=ctemp;
      free(rbk_cp);
    }
    if(rbk_dflt_cnfg_name != cgi.getElements().end()) {
      int cdflt = vmecc->read_cnfg_dflt();
      if(cdflt >= 0){
        sprintf(ctemp,"%d",cdflt);
        VCC_UTIL_CR_curr_dflt = ctemp;
        rbk_cp = vmecc->read_cnfg_num_dcd(cdflt);
        vmecc->print_crs(rbk_cp);
        sprintf(ctemp,"%04X",rbk_cp->ether);
        VCC_UTIL_CR_dflt_[0]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
        VCC_UTIL_CR_dflt_[1]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->rst_misc);
        VCC_UTIL_CR_dflt_[2]=ctemp;
        sprintf(ctemp,"%08X",rbk_cp->vme);
        VCC_UTIL_CR_dflt_[3]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->vme_bto);
        VCC_UTIL_CR_dflt_[4]=ctemp;
        sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
        VCC_UTIL_CR_dflt_[5]=ctemp;
        free(rbk_cp);
      }
      else {
	std::cout << "Error while reading default config. number" << std::endl;
        VCC_UTIL_CR_curr_dflt = "-";
      }
    }
    if(rbk_cnumA_name != cgi.getElements().end()) {
      VCC_UTIL_CR_cnumA = cgi["cnumA"]->getValue();
      int cnumA = cgi["cnumA"]->getIntegerValue();
      rbk_cp = vmecc->read_cnfg_num_dcd(cnumA);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_flashA_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_flashA_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_flashA_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_flashA_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_flashA_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_flashA_[5]=ctemp;
      vmecc->print_crs(rbk_cp);
      free(rbk_cp);
    }
    if(rbk_cnumB_name != cgi.getElements().end()) {
      VCC_UTIL_CR_cnumB = cgi["cnumB"]->getValue();
      int cnumB = cgi["cnumB"]->getIntegerValue();
      rbk_cp = vmecc->read_cnfg_num_dcd(cnumB);
      sprintf(ctemp,"%04X",rbk_cp->ether);
      VCC_UTIL_CR_flashB_[0]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->ext_fifo);
      VCC_UTIL_CR_flashB_[1]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->rst_misc);
      VCC_UTIL_CR_flashB_[2]=ctemp;
      sprintf(ctemp,"%08X",rbk_cp->vme);
      VCC_UTIL_CR_flashB_[3]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bto);
      VCC_UTIL_CR_flashB_[4]=ctemp;
      sprintf(ctemp,"%04X",rbk_cp->vme_bgto);
      VCC_UTIL_CR_flashB_[5]=ctemp;
      vmecc->print_crs(rbk_cp);
      free(rbk_cp);
    }
    if(sav_crs_name != cgi.getElements().end()) {
      VCC_UTIL_CR_sav_cnum = cgi["sav_cnum"]->getValue();
      int sav_cnum = cgi["sav_cnum"]->getIntegerValue();
      vmecc->save_cnfg_num(sav_cnum);
    }
    if(rstr_crs_name != cgi.getElements().end()) {
      VCC_UTIL_CR_rstr_cnum = cgi["rstr_cnum"]->getValue();
      int rstr_cnum = cgi["rstr_cnum"]->getIntegerValue();
      vmecc->rstr_cnfg_num(rstr_cnum);
    }
    if(set_dflt_name != cgi.getElements().end()) {
      VCC_UTIL_CR_dflt_cnum = cgi["dflt_cnum"]->getValue();
      int dflt_cnum = cgi["dflt_cnum"]->getIntegerValue();
      vmecc->set_cnfg_dflt(dflt_cnum);
    }
    if(rd_dflt_name != cgi.getElements().end()) {
      int cdflt = vmecc->read_cnfg_dflt();
      if(cdflt >= 0){
        sprintf(ctemp,"%d",cdflt);
        VCC_UTIL_CR_curr_dflt = ctemp;
      }
      else {
	std::cout << "Error while reading default config. number" << std::endl;
        VCC_UTIL_CR_curr_dflt = "-";
      }
    }
    if(rd_ser_num_name != cgi.getElements().end()) {
      SN_t sn = vmecc->rd_ser_num();
      if(sn.status == 0){
        sprintf(ctemp,"VCC%02d",sn.sn);
        VCC_UTIL_CR_ser_num = ctemp;
      }
      else {
	std::cout << "Error while reading serial number. Status=" << sn.status << "  Error type = " << std::hex << sn.err_typ << std::endl;
        VCC_UTIL_CR_ser_num = "-";
      }
    }
    this->VMECCGUI_cnfg_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_MAC_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  std::string ena_dis;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    VCC_UTIL_MAC_wrt_dev="02-00-00-00-00-3B";
    VCC_UTIL_MAC_wrt_mcast1="FF-FF-FF-FF-FF-FE";
    VCC_UTIL_MAC_wrt_mcast2="FF-FF-FF-FF-FF-FD";
    VCC_UTIL_MAC_wrt_mcast3="FF-FF-FF-FF-FF-FC";
    VCC_UTIL_MAC_wrt_dflt="00-0D-88-B2-65-21";
    VCC_UTIL_MAC_rbk_dev="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast1="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast2="Not yet read back";
    VCC_UTIL_MAC_rbk_mcast3="Not yet read back";
    VCC_UTIL_MAC_rbk_dflt="Not yet read back";
    VCC_UTIL_MAC_ena_dis="disabled";
  }

  char title[] = "VCC Utilities: MAC Addresses";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_MAC_DO =
     toolbox::toString("/%s/VCC_MAC_DO",getApplicationDescriptor()->getURN().c_str());
   if(VCC_UTIL_MAC_ena_dis=="disabled"){
     ena_dis = "Ena";
   }
   else {
     ena_dis = "Dis";
   }

  *out << "<form action=\"" << VCC_MAC_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">MAC Addresses</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << " rules=\"groups\">" << std::endl;
  *out << "    <colgroup span=\"3\"><colgroup span=\"1\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <th colspan=\"3\" style=\"font-size: 14pt; color: blue\">Inputs</th>" << std::endl;
  *out << "      <th style=\"font-size: 14pt; color: blue\">Read Backs</th>" << std::endl;
  *out << "    </tr><tr>" << std::endl; 
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"dev_mac\"" << VCC_UTIL_MAC_ena_dis << "></td>" << std::endl;
  *out << "      <td> Device MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_dev << "\" size=\"17\" maxwidth=\"170\" name=\"dev_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_dev << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt1_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 1 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast1 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt1_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast1 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt2_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 3 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast2 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt2_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast2 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"mlt3_mac\"></td>" << std::endl;
  *out << "      <td> MultiCast 3 MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_mcast3 << "\" size=\"17\" maxwidth=\"170\" name=\"mlt3_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_mcast3 << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"dflt_mac\"></td>" << std::endl;
  *out << "      <td> Default Server MAC </td>" << std::endl;
  *out << "      <td><input align=\"left\" value=\"" << VCC_UTIL_MAC_wrt_dflt << "\" size=\"17\" maxwidth=\"170\" name=\"dflt_mac_addr\" type=\"text\"></td>" << std::endl;
  *out << "      <td align=\"center\">" << VCC_UTIL_MAC_rbk_dflt << "</td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td><input name=\"mac_choice\" type=\"radio\" value=\"all_mac\"" << VCC_UTIL_MAC_ena_dis << "></td>" << std::endl;
  *out << "      <td> All MACs </td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td></td>" << std::endl;
  *out << "      <td><input name=\"rst_macs\" type=\"reset\" value=\"Reset\"> &nbsp &nbsp <input name=\"ena_dev_mac\" type=\"submit\" value=\"" << ena_dis << "\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td><input name=\"wrt_macs\" type=\"submit\" value=\"Write\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "      <td align=\"center\"><input type=\"submit\" value=\"Read\" name=\"rd_MAC_dcd\"></td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_MAC;
}

void EmuPeripheralCrateConfig::VCC_MAC_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  CNFG_ptr rbk_cp;
  CNFG_t wrt_mac;
  std::string stemp;
  unsigned int temp[6];

  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_MAC_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator ena_dev_mac = cgi.getElement("ena_dev_mac");
    cgicc::form_iterator wrt_macs = cgi.getElement("wrt_macs");
    cgicc::form_iterator which_mac = cgi.getElement("mac_choice");
    cgicc::form_iterator rd_macs = cgi.getElement("rd_MAC_dcd");
    if(ena_dev_mac != cgi.getElements().end()) {
      stemp = cgi["ena_dev_mac"]->getValue();
      if(stemp == "Ena"){
        VCC_UTIL_MAC_ena_dis = "";
      }
      else {
	VCC_UTIL_MAC_ena_dis = "disabled";
      }
    }
    if(wrt_macs != cgi.getElements().end()) {
      if(which_mac != cgi.getElements().end()) {
        stemp = cgi["mac_choice"]->getValue();
        std::cout << "Chosen MAC " << stemp.c_str() << std::endl;
        if(stemp == "dev_mac"){
          VCC_UTIL_MAC_wrt_dev = cgi["dev_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dev.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.device[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(0,&wrt_mac);
        }
        if(stemp == "mlt1_mac"){
          VCC_UTIL_MAC_wrt_mcast1 = cgi["mlt1_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast1.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast1[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(1,&wrt_mac);
        }
        if(stemp == "mlt2_mac"){
          VCC_UTIL_MAC_wrt_mcast2 = cgi["mlt2_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast2.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast2[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(2,&wrt_mac);
        }
        if(stemp == "mlt3_mac"){
          VCC_UTIL_MAC_wrt_mcast3 = cgi["mlt3_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast3.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast3[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(3,&wrt_mac);
        }
        if(stemp == "dflt_mac"){
          VCC_UTIL_MAC_wrt_dflt = cgi["dflt_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dflt.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.dflt_srv[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(4,&wrt_mac);
        }
        if(stemp == "all_mac"){
          VCC_UTIL_MAC_wrt_dev = cgi["dev_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dev.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.device[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast1 = cgi["mlt1_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast1.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast1[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast2 = cgi["mlt2_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast2.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast2[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_mcast3 = cgi["mlt3_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_mcast3.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.mcast3[i] = (unsigned char)(temp[i]&0xFF);
          VCC_UTIL_MAC_wrt_dflt = cgi["dflt_mac_addr"]->getValue();
          sscanf(VCC_UTIL_MAC_wrt_dflt.c_str(),"%2x-%2x-%2x-%2x-%2x-%2x",&temp[0],&temp[1],&temp[2],&temp[3],&temp[4],&temp[5]);
          for(int i=0;i<6;i++)wrt_mac.mac.dflt_srv[i] = (unsigned char)(temp[i]&0xFF);
          vmecc->set_macs(8,&wrt_mac);
        }
        VCC_UTIL_MAC_ena_dis = "disabled";
      }
    }
    if(rd_macs != cgi.getElements().end()) {
      rbk_cp = vmecc->read_macs_dcd();
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.device[0],rbk_cp->mac.device[1],rbk_cp->mac.device[2],rbk_cp->mac.device[3],rbk_cp->mac.device[4],rbk_cp->mac.device[5]);
      VCC_UTIL_MAC_rbk_dev = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast1[0],rbk_cp->mac.mcast1[1],rbk_cp->mac.mcast1[2],rbk_cp->mac.mcast1[3],rbk_cp->mac.mcast1[4],rbk_cp->mac.mcast1[5]);
      VCC_UTIL_MAC_rbk_mcast1 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast2[0],rbk_cp->mac.mcast2[1],rbk_cp->mac.mcast2[2],rbk_cp->mac.mcast2[3],rbk_cp->mac.mcast2[4],rbk_cp->mac.mcast2[5]);
      VCC_UTIL_MAC_rbk_mcast2 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.mcast3[0],rbk_cp->mac.mcast3[1],rbk_cp->mac.mcast3[2],rbk_cp->mac.mcast3[3],rbk_cp->mac.mcast3[4],rbk_cp->mac.mcast3[5]);
      VCC_UTIL_MAC_rbk_mcast3 = ctemp;
      sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",rbk_cp->mac.dflt_srv[0],rbk_cp->mac.dflt_srv[1],rbk_cp->mac.dflt_srv[2],rbk_cp->mac.dflt_srv[3],rbk_cp->mac.dflt_srv[4],rbk_cp->mac.dflt_srv[5]);
      VCC_UTIL_MAC_rbk_dflt = ctemp;
      vmecc->print_macs(rbk_cp);
      free(rbk_cp);
    }
    this->VMECCGUI_MAC_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_FIFO_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  CNFG_ptr rbk_cp;
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    rbk_cp = vmecc->read_crs();
    if((rbk_cp->ext_fifo & FIFO_CR_TST)== FIFO_CR_TST){
      VCC_UTIL_FIFO_mode="Test";
    }
    else {
      VCC_UTIL_FIFO_mode="VME";
    }
    if((rbk_cp->ext_fifo & FIFO_CR_ECC)== FIFO_CR_ECC){
      VCC_UTIL_FIFO_ecc="Ena";
    }
    else {
      VCC_UTIL_FIFO_ecc="Dis";
    }
    if((rbk_cp->ext_fifo & FIFO_CR_INJ)== FIFO_CR_INJ){
      VCC_UTIL_FIFO_inj="On";
    }
    else {
      VCC_UTIL_FIFO_inj="Off";
    }
    VCC_UTIL_FIFO_wrt_pae="127";
    VCC_UTIL_FIFO_wrt_paf="127";
    VCC_UTIL_FIFO_rbk_pae="- - -";
    VCC_UTIL_FIFO_rbk_paf="- - -";
    VCC_UTIL_FIFO_cor_errs="- - -";
    VCC_UTIL_FIFO_uncor_errs="- - -";
    VCC_UTIL_FIFO_wrt_data="Enter\n16 bit\nHex Data";
    VCC_UTIL_FIFO_rbk_data="";
    VCC_UTIL_FIFO_msg_data="";
    VCC_UTIL_FIFO_rd_num="1";
    free(rbk_cp);
  }

  char title[] = "VCC Utilities: External FIFO";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_FIFO_DO =
     toolbox::toString("/%s/VCC_FIFO_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_FIFO_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">External FIFO</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  //  *out << "    <tr><td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Modes and Resets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"fifo_mode\" type=\"submit\" value=\"FIFO Mode\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_mode << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"err_inj\" type=\"submit\" value=\"Error Inject\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_inj << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"ecc_state\" type=\"submit\" value=\"ECC\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_FIFO_ecc << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"prst\" type=\"submit\" value=\"Partial RST\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"mrst\" type=\"submit\" value=\"Master RST\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"set_mrk\" type=\"submit\" value=\"Set Mark\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"rst_mark\" type=\"submit\" value=\"Reset Mark\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Programmable Offsets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">PAE</td>" << std::endl;
  *out << "             <td align=\"center\">PAF</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_FIFO_wrt_pae << "\" size=\"7\" maxwidth=\"70\" name=\"pae_off_val\" type=\"text\"></td>" << std::endl;
  *out << "             <td align=\"center\"><input align=\"right\" value=\"" << VCC_UTIL_FIFO_wrt_paf << "\" size=\"7\" maxwidth=\"70\" name=\"paf_off_val\" type=\"text\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_rbk_pae << "</td>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_rbk_paf << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_offs\" type=\"submit\" value=\"Read Offsets\"></td>" << std::endl;
  *out << "             <td><input name=\"wrt_offs\" type=\"submit\" value=\"Write Offsets\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  //  *out << "    <tr><td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data to/from FIFO</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td rowspan=\"2\"><textarea name=\"fifo_data\" rows=\"3\" cols=\"12\">" << VCC_UTIL_FIFO_wrt_data << "</textarea></td>" << std::endl;
  *out << "             <td><input name=\"wrt_dat_fifo\" type=\"submit\" value=\"Write FIFO\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_dat_fifo\" type=\"submit\" value=\"Read\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "                 <input align=\"right\" value=\"" << VCC_UTIL_FIFO_rd_num << "\" size=\"3\" maxwidth=\"30\" name=\"rd_num\" type=\"text\"> Words</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Error Counts</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td align=\"center\">Corrected</td>" << std::endl;
  *out << "             <td align=\"center\">UnCorrected</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_cor_errs << "</td>" << std::endl;
  *out << "             <td align=\"center\">" << VCC_UTIL_FIFO_uncor_errs << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"rd_errs\" type=\"submit\" value=\"Read Counters\"></td>" << std::endl;
  *out << "             <td><input name=\"rst_errs\" type=\"submit\" value=\"Reset Counters\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Readback Data</legend>" << std::endl;
  *out << "          <pre><p>" << VCC_UTIL_FIFO_rbk_data << "</p></pre>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "      <td valign=\"top\" align = \"center\">" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Messages</legend>" << std::endl;
  *out << "          <pre><p>" << VCC_UTIL_FIFO_msg_data << "</p></pre></td>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_FIFO;
}

void EmuPeripheralCrateConfig::VCC_FIFO_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  unsigned int cor,uncor,full_off,mt_off;
  unsigned int itemp;
  unsigned short int *pbuf;
  int i,nwrds,status,pkt_type;
  char ctemp[256];
  char *ptemp1,*ptemp2,*ptemp3;
  char *pch = (char *) &VCC_UTIL_FIFO_wrt_data;
  std::string stemp;

  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_FIFO_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator fifo_mode = cgi.getElement("fifo_mode");
    cgicc::form_iterator err_inj = cgi.getElement("err_inj");
    cgicc::form_iterator ecc_state = cgi.getElement("ecc_state");
    cgicc::form_iterator prst = cgi.getElement("prst");
    cgicc::form_iterator mrst = cgi.getElement("mrst");
    cgicc::form_iterator set_mark = cgi.getElement("set_mark");
    cgicc::form_iterator rst_mark = cgi.getElement("rst_mark");
    cgicc::form_iterator rd_errs = cgi.getElement("rd_errs");
    cgicc::form_iterator rst_errs = cgi.getElement("rst_errs");
    cgicc::form_iterator rd_offs = cgi.getElement("rd_offs");
    cgicc::form_iterator wrt_offs = cgi.getElement("wrt_offs");
    cgicc::form_iterator wrt_dat_fifo = cgi.getElement("wrt_dat_fifo");
    cgicc::form_iterator rd_dat_fifo = cgi.getElement("rd_dat_fifo");

    if(fifo_mode != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_mode == "Test"){
        vmecc->set_clr_bits(CLR, EXTFIFO, ~FIFO_CR_TST);
        VCC_UTIL_FIFO_mode = "VME";
      }
      else {
        vmecc->set_clr_bits(SET, EXTFIFO, FIFO_CR_TST);
        VCC_UTIL_FIFO_mode = "Test";
      }
    }
    if(err_inj != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_inj == "On"){
        vmecc->set_clr_bits(CLR, EXTFIFO, ~FIFO_CR_INJ);
        VCC_UTIL_FIFO_inj = "Off";
      }
      else {
        vmecc->set_clr_bits(SET, EXTFIFO, FIFO_CR_INJ);
        VCC_UTIL_FIFO_inj = "On";
      }
    }
    if(ecc_state != cgi.getElements().end()) {
      if(VCC_UTIL_FIFO_ecc == "Ena"){
        vmecc->set_clr_bits(CLR, EXTFIFO, ~FIFO_CR_ECC);
        VCC_UTIL_FIFO_ecc = "Dis";
      }
      else {
        vmecc->set_clr_bits(SET, EXTFIFO, FIFO_CR_ECC);
        VCC_UTIL_FIFO_ecc = "Ena";
      }
    }
    if(prst != cgi.getElements().end()) {
      vmecc->prst_ff();
    }
    if(mrst != cgi.getElements().end()) {
      vmecc->mrst_ff();
    }
    if(set_mark != cgi.getElements().end()) {
      vmecc->set_mark();
    }
    if(rst_mark != cgi.getElements().end()) {
      vmecc->rst_mark();
    }
    if(rd_errs != cgi.getElements().end()) {
      status = vmecc->rd_err_cnts(&cor,&uncor);
      if(status == 0){
        std::cout << "uncorrected errors: " << uncor << std::endl;
        std::cout << "  corrected errors: " << cor << std::endl;
        sprintf(ctemp,"%d",cor);
        VCC_UTIL_FIFO_cor_errs=ctemp;
        sprintf(ctemp,"%d",uncor);
        VCC_UTIL_FIFO_uncor_errs=ctemp;
      }
      else {
        VCC_UTIL_FIFO_cor_errs="Bad status";
        VCC_UTIL_FIFO_uncor_errs="Bad status";
      }
    }
    if(rst_errs != cgi.getElements().end()) {
      vmecc->rst_err_cnt();
    }
    if(rd_offs != cgi.getElements().end()) {
      status = vmecc->rdbk_off(&full_off,&mt_off);
      if(status == 0){
        std::cout << "full offset: " << full_off << std::endl;
        std::cout << "  mt offset: " << mt_off << std::endl;
        sprintf(ctemp,"%d",full_off);
        VCC_UTIL_FIFO_rbk_paf=ctemp;
        sprintf(ctemp,"%d",mt_off);
        VCC_UTIL_FIFO_rbk_pae=ctemp;
      }
      else {
        VCC_UTIL_FIFO_rbk_paf="Bad Status";
        VCC_UTIL_FIFO_rbk_pae="Bad Status";
      }
    }
    if(wrt_offs != cgi.getElements().end()) {
      int paf_off = cgi["paf_off_val"]->getIntegerValue();
      int pae_off = cgi["pae_off_val"]->getIntegerValue();
      VCC_UTIL_FIFO_wrt_paf = cgi["paf_off_val"]->getValue();
      VCC_UTIL_FIFO_wrt_pae = cgi["pae_off_val"]->getValue();
      vmecc->prg_off((unsigned)paf_off,(unsigned)pae_off);
    }
    if(wrt_dat_fifo != cgi.getElements().end()) {
      VCC_UTIL_FIFO_wrt_data = cgi["fifo_data"]->getValue();
      size_t slen = VCC_UTIL_FIFO_wrt_data.length();
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_FIFO_wrt_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      i=0;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3 = 0;
        itemp = strtoul(ptemp2,&pch,16);
        vmecc->wbuf[2*(++i)]=(itemp>>8)&0xFF;
        vmecc->wbuf[2*i+1]=itemp&0xFF;
      }
      nwrds=i;
      free(ptemp1);
      std::cout << "FIFO Data: " << std::endl;
      for(i=1;i<=nwrds;i++){
        printf("%02X%02X\n",vmecc->wbuf[2*i]&0xFF,vmecc->wbuf[2*i+1]&0xFF);
      }
      vmecc->wrt_ff(nwrds);
    }
    if(rd_dat_fifo != cgi.getElements().end()) {
      VCC_UTIL_FIFO_msg_data = "";
      VCC_UTIL_FIFO_rbk_data = "";
      int nwrds = cgi["rd_num"]->getIntegerValue();
      VCC_UTIL_FIFO_rd_num = cgi["rd_num"]->getValue();
      pbuf = (unsigned short int *) malloc(2*nwrds);
      status = vmecc->rd_ff(nwrds,0,pbuf);
      if(status==0){
        VCC_UTIL_FIFO_msg_data = "Timed out trying to read";
      }
      else {
        if(status==-2){
          pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
	  sprintf(ctemp,"Unexpected Pkt Type: 0x%02X\n",pkt_type);
	  std::cout << ctemp << std::endl;
	  VCC_UTIL_FIFO_msg_data += ctemp;
	}
        else if (status==-1){
	  std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	  VCC_UTIL_FIFO_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
	  VCC_UTIL_FIFO_msg_data += "\n";
	}
        else {
          for(i=0;i<nwrds;i++){
	    sprintf(ctemp,"0x%04hX\n",pbuf[i]);
            VCC_UTIL_FIFO_rbk_data += ctemp;
	  }
	}
        while((pkt_type=vmecc->rd_pkt())>=0){
          if(pkt_type>INFO_PKT){
            std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
            VCC_UTIL_FIFO_msg_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
            VCC_UTIL_FIFO_msg_data += "\n";
	  }
          else if (pkt_type==EXFIFO_PKT){
            int nw = ((vmecc->rbuf[WRD_CNT_OFF]&0xff)<<8)|(vmecc->rbuf[WRD_CNT_OFF+1]&0xff);
	    for(i=0;i<nw;i++){
	      sprintf(ctemp,"0x%02X%02X\n",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	      VCC_UTIL_FIFO_rbk_data += ctemp;
	    }
	  }
          else {
            sprintf(ctemp,"Pkt Type: 0x%02X\n",pkt_type);
            std::cout << ctemp << std::endl;
            VCC_UTIL_FIFO_msg_data += ctemp;
	  }
	}
      }
      free(pbuf);
    }
    this->VMECCGUI_FIFO_utils(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_pkt_send(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  if(first){
    first = false;
    VCC_UTIL_PKTSND_prcs_tag = "0";
    VCC_UTIL_PKTSND_cmnd = "00";
    VCC_UTIL_PKTSND_data = "Enter\n16 bit\nHex Data\n";
  }

  char title[] = "VCC Utilities: Packet Send Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_PKTSND_DO =
     toolbox::toString("/%s/VCC_PKTSND_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_PKTSND_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend  style=\"font-size: 18pt;\"align=\"center\">Packet Send</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\" colspan=\"2\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Packet Header</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"prio_pkt_chk\" type=\"checkbox\" value=\"prio_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Priority Packet</td>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"ack_pkt_chk\" type=\"checkbox\" value=\"ack_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\" colspan=\"2\">Acknowledge Request</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td align=\"right\">Process Tag:</td>" << std::endl;
  *out << "             <td><input align=\"right\" maxwidth=\"40\" size=\"4\" value=\"" << VCC_UTIL_PKTSND_prcs_tag << "\" name=\"prcs_tag\" type=\"text\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td align=\"right\">Command:</td>" << std::endl;
  *out << "             <td><input align=\"right\" maxwidth=\"40\" size=\"4\" value=\"" << VCC_UTIL_PKTSND_cmnd << "\" name=\"pkt_cmnd\" type=\"text\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "    <td rowspan=\"2\" valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data</legend>" << std::endl;
  *out << "        <textarea name=\"pkt_data\" rows=\"5\" cols=\"12\">" << VCC_UTIL_PKTSND_data << "</textarea></td>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td>" << std::endl;
  *out << "    <td>" << std::endl;
  *out << "      <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "        <tr>" << std::endl;
  *out << "          <td><input name=\"rst_seq_id\" type=\"submit\" value=\"Reset Sequential ID\"></td>" << std::endl;
  *out << "        </tr><tr>" << std::endl;
  *out << "          <td><input name=\"snd_pkt_now\" type=\"submit\" value=\"Send Packet Now\"></td>" << std::endl;
  *out << "        </tr>" << std::endl;
  *out << "      </table>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_PKTSND;
}

void EmuPeripheralCrateConfig::VCC_PKTSND_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  unsigned int itemp;
  int i,nwrds;
  char *ptemp1,*ptemp2,*ptemp3;
  char *pch1 = (char *) &VCC_UTIL_PKTSND_cmnd;
  char *pch2 = (char *) &VCC_UTIL_PKTSND_data;

  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_PKTSND_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator rst_seq_id = cgi.getElement("rst_seq_id");
    cgicc::form_iterator snd_pkt_now = cgi.getElement("snd_pkt_now");
    cgicc::form_iterator prio_pkt_chk = cgi.getElement("prio_pkt_chk");
    cgicc::form_iterator ack_pkt_chk = cgi.getElement("ack_pkt_chk");

    if(rst_seq_id != cgi.getElements().end()) {
      vmecc->rst_seq_id();
    }
    if(snd_pkt_now != cgi.getElements().end()) {
      int prcs_tag = cgi["prcs_tag"]->getIntegerValue();
      VCC_UTIL_PKTSND_prcs_tag = cgi["prcs_tag"]->getValue();
      VCC_UTIL_PKTSND_cmnd = cgi["pkt_cmnd"]->getValue();
      VCC_UTIL_PKTSND_data = cgi["pkt_data"]->getValue();
      size_t slen = VCC_UTIL_PKTSND_data.length();
      vmecc->wbuf[0] = prcs_tag & 0x1F;
      vmecc->wbuf[1] = strtoul(VCC_UTIL_PKTSND_cmnd.c_str(),&pch1,16) & 0xFF;
      if(prio_pkt_chk != cgi.getElements().end()) {
        vmecc->wbuf[0]= vmecc->wbuf[0] | 0x40;
      }
      if(ack_pkt_chk != cgi.getElements().end()) {
        vmecc->wbuf[0]= vmecc->wbuf[0] | 0x20;
      }
      ptemp1 = (char *) malloc(slen+1);
      VCC_UTIL_PKTSND_data.copy(ptemp1,slen);
      ptemp1[slen]='\0';
      ptemp3=ptemp1;
      i=0;
      while((ptemp2 = strtok(ptemp3,"\n\r")) !=NULL){
        ptemp3 = 0;
        itemp = strtoul(ptemp2,&pch2,16);
        vmecc->wbuf[2*(++i)]=(itemp>>8)&0xFF;
        vmecc->wbuf[2*i+1]=itemp&0xFF;
      }
      nwrds=i;
      free(ptemp1);
//       std::cout << "Packet Data: " << std::endl;
//       for(i=0;i<=nwrds;i++){
//         printf("%02X%02X\n",vmecc->wbuf[2*i]&0xFF,vmecc->wbuf[2*i+1]&0xFF);
//       }
      vmecc->nwbuf = 2*nwrds + 2;
      //      std::cout <<"nwbuf = " << vmecc->nwbuf << std::endl;
      int n = vmecc->eth_write();
      std::cout << "Wrote " << n << " bytes to controller" << std::endl;
    }
    this->VMECCGUI_pkt_send(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_pkt_rcv(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  char ctemp[256];
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;

    VCC_UTIL_PKTRCV_num_pkt="1";
    VCC_UTIL_PKTRCV_raw_pkt="";
    VCC_UTIL_PKTRCV_pkt_len="0";
    VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
    VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
    VCC_UTIL_PKTRCV_pkt_flags="";
    VCC_UTIL_PKTRCV_frg_seq="Seq#";
    VCC_UTIL_PKTRCV_frg_seq_num="";
    VCC_UTIL_PKTRCV_ack_num="";
    VCC_UTIL_PKTRCV_ack_stat="";
    VCC_UTIL_PKTRCV_prc_tag="0";
    VCC_UTIL_PKTRCV_pkt_typ_num="00";
    VCC_UTIL_PKTRCV_pkt_type="";
    VCC_UTIL_PKTRCV_pkt_cmnd="";
    VCC_UTIL_PKTRCV_wrd_cnt="0";
    VCC_UTIL_PKTRCV_rbk_data="";

  }
  int pkts_left = vmecc->LeftToRead();
  sprintf(ctemp,"%d",pkts_left);
  VCC_UTIL_PKTRCV_pkts_inbuf = ctemp;

  char title[] = "VCC Utilities: Packet Receive Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_PKTRCV_DO =
     toolbox::toString("/%s/VCC_PKTRCV_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_PKTRCV_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 18pt;\" align=\"center\">Packet Receive</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr>" << std::endl;
  *out << "      <td valign=\"top\">" << std::endl;
  *out << "        <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "          <tr>" << std::endl;
  *out << "            <td align=\"right\">" << VCC_UTIL_PKTRCV_pkts_inbuf << "</td>" << std::endl;
  *out << "            <td colspan=\"2\" align=\"left\">Packets Remain in Buffer</td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td align=\"right\"><input name=\"rd_pkt\" type=\"submit\" value=\"Read\"></td>" << std::endl;
  *out << "            <td><input align=\"right\" maxwidth=\"20\" size=\"3\" value=\"" << VCC_UTIL_PKTRCV_num_pkt << "\" name=\"num_pkt\" type=\"text\"></td>" << std::endl;
  *out << "            <td align=\"left\">Packets from Driver</td>" << std::endl;
  *out << "          </tr><tr>" << std::endl;
  *out << "            <td align=\"right\"><input name=\"flsh_pkt\" type=\"submit\" value=\"Flush\"></td>" << std::endl;
  *out << "            <td colspan=\"2\" align=\"left\">All Packets from Driver</td>" << std::endl;
  *out << "          </tr>" << std::endl;
  *out << "        </table>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Raw Packet</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td>" << VCC_UTIL_PKTRCV_raw_pkt << "</td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Decoded Header</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td align=\"right\">Dest:</td>" << std::endl;
  *out << "              <td colspan=\"2\">" << VCC_UTIL_PKTRCV_dstn_addr << "</td>" << std::endl;
  *out << "              <td align=\"right\">Pkt Len:" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_len << " bytes</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Source:</td>" << std::endl;
  *out << "              <td colspan=\"2\">" << VCC_UTIL_PKTRCV_src_addr << "</td>" << std::endl;
  *out << "              <td align=\"right\">Data Wrd Cnt:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_wrd_cnt << " words</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Flags:</td>" << std::endl;
  *out << "              <td align=\"left\" colspan=\"2\">" << VCC_UTIL_PKTRCV_pkt_flags << "</td>" << std::endl;
  *out << "              <td align=\"right\">" << VCC_UTIL_PKTRCV_frg_seq << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_frg_seq_num << "</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Ack/Status:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_ack_num << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_ack_stat << "</td>" << std::endl;
  *out << "              <td align=\"right\">Process Tag:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_prc_tag << "</td>" << std::endl;
  *out << "            </tr><tr>" << std::endl;
  *out << "              <td align=\"right\">Pkt Type:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_typ_num << "</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_type << "</td>" << std::endl;
  *out << "              <td align=\"right\">Command:</td>" << std::endl;
  *out << "              <td align=\"left\">" << VCC_UTIL_PKTRCV_pkt_cmnd << "</td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr><tr>" << std::endl;
  *out << "      <td>" << std::endl;
  *out << "        <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Data</legend>" << std::endl;
  *out << "          <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "            <tr>" << std::endl;
  *out << "              <td align=\"left\">" << std::endl;
  *out << "                <pre><p>" << VCC_UTIL_PKTRCV_rbk_data << "</p></pre>" << std::endl;
  *out << "              </td>" << std::endl;
  *out << "            </tr>" << std::endl;
  *out << "          </table>" << std::endl;
  *out << "        </fieldset>" << std::endl;
  *out << "      </td>" << std::endl;
  *out << "    </tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_PKTRCV;
}

void EmuPeripheralCrateConfig::VCC_PKTRCV_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  char ctemp[256];
  char ctemp2[256];
  int i,n;
  int pkt,pkt_type,wc;
  std::string save1,save2;
  union hdr_stat hdr;
  bool new_pkt;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_PKTRCV_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator rd_pkt = cgi.getElement("rd_pkt");
    cgicc::form_iterator flsh_pkt = cgi.getElement("flsh_pkt");

    if(flsh_pkt != cgi.getElements().end()) {
      VCC_UTIL_PKTRCV_num_pkt = cgi["num_pkt"]->getValue();
      VCC_UTIL_PKTRCV_raw_pkt="";
      VCC_UTIL_PKTRCV_pkt_len="0";
      VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_pkt_flags="";
      VCC_UTIL_PKTRCV_frg_seq="Seq#";
      VCC_UTIL_PKTRCV_frg_seq_num="";
      VCC_UTIL_PKTRCV_ack_num="";
      VCC_UTIL_PKTRCV_ack_stat="";
      VCC_UTIL_PKTRCV_prc_tag="0";
      VCC_UTIL_PKTRCV_pkt_typ_num="00";
      VCC_UTIL_PKTRCV_pkt_type="";
      VCC_UTIL_PKTRCV_pkt_cmnd="";
      VCC_UTIL_PKTRCV_wrd_cnt="0";
      VCC_UTIL_PKTRCV_rbk_data="";
      while((n=vmecc->eth_read())>6){
        pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
        sprintf(ctemp,"Packet Type: %02x\n",pkt_type&0xFF);
        VCC_UTIL_PKTRCV_rbk_data += ctemp;
      }
    }
    if(rd_pkt != cgi.getElements().end()) {
      int npkts = cgi["num_pkt"]->getIntegerValue();
      VCC_UTIL_PKTRCV_num_pkt = cgi["num_pkt"]->getValue();
      VCC_UTIL_PKTRCV_raw_pkt="";
      VCC_UTIL_PKTRCV_pkt_len="0";
      VCC_UTIL_PKTRCV_dstn_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_src_addr="xx-xx-xx-xx-xx-xx";
      VCC_UTIL_PKTRCV_pkt_flags="";
      VCC_UTIL_PKTRCV_frg_seq="Seq#";
      VCC_UTIL_PKTRCV_frg_seq_num="";
      VCC_UTIL_PKTRCV_ack_num="";
      VCC_UTIL_PKTRCV_ack_stat="";
      save1 = VCC_UTIL_PKTRCV_prc_tag;
      VCC_UTIL_PKTRCV_prc_tag="0";
      VCC_UTIL_PKTRCV_pkt_typ_num="00";
      VCC_UTIL_PKTRCV_pkt_type="";
      save2 = VCC_UTIL_PKTRCV_pkt_cmnd;
      VCC_UTIL_PKTRCV_pkt_cmnd="";
      VCC_UTIL_PKTRCV_wrd_cnt="0";
      VCC_UTIL_PKTRCV_rbk_data="";
      n = 999;
      for(pkt=0;pkt<npkts && n>6;pkt++){
        n=vmecc->eth_read();
        if(n>6){
	  pkt_type = vmecc->rbuf[PKT_TYP_OFF]&0XFF;
	  wc = DAT_WRD_CNT(vmecc->rbuf);
          sprintf(ctemp,"%d",((vmecc->rbuf[PKT_LEN_OFF]&0xff)<<8)|(vmecc->rbuf[PKT_LEN_OFF+1]&0xff));
	  VCC_UTIL_PKTRCV_pkt_len = ctemp;
	  if(pkt_type>INFO_PKT){
	    std::cout << vmecc->dcode_msg_pkt(vmecc->rbuf) << std::endl;
	    VCC_UTIL_PKTRCV_rbk_data += vmecc->dcode_msg_pkt(vmecc->rbuf);
	    VCC_UTIL_PKTRCV_rbk_data += "\n";
	  }
	  else {
	    for(i=0;i<wc;i++){
	      if(i%8==0){
		sprintf(ctemp,"\nPkt%d Type: %02X:",pkt,pkt_type);
		VCC_UTIL_PKTRCV_rbk_data += ctemp;
	      }
	      sprintf(ctemp," %02X%02X",vmecc->rbuf[2*i+DATA_OFF]&0xFF,vmecc->rbuf[2*i+DATA_OFF+1]&0xFF);
	      VCC_UTIL_PKTRCV_rbk_data += ctemp;
	    }
	    strcpy(ctemp,"\n");
	    VCC_UTIL_PKTRCV_rbk_data += ctemp;
	  }
          VCC_UTIL_PKTRCV_raw_pkt = "";
          int neop =2*wc+22;
          int maxn = 36;
	  for(i=0;i<neop && i<maxn;i++){
            sprintf(ctemp,"%02x",vmecc->rbuf[i]&0xFF);
	    VCC_UTIL_PKTRCV_raw_pkt += ctemp;
	  }
          if(neop>i){
            strcpy(ctemp,"...");
	    VCC_UTIL_PKTRCV_raw_pkt += ctemp;
	  }
          int ofs = MAC_DEST_OFF;
          sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",vmecc->rbuf[ofs]&0xFF,vmecc->rbuf[ofs+1]&0xFF,vmecc->rbuf[ofs+2]&0xFF,vmecc->rbuf[ofs+3]&0xFF,vmecc->rbuf[ofs+4]&0xFF,vmecc->rbuf[ofs+5]&0xFF);
 	  VCC_UTIL_PKTRCV_dstn_addr = ctemp;
          ofs = MAC_SRC_OFF;
          sprintf(ctemp,"%02X-%02X-%02X-%02X-%02X-%02X",vmecc->rbuf[ofs]&0xFF,vmecc->rbuf[ofs+1]&0xFF,vmecc->rbuf[ofs+2]&0xFF,vmecc->rbuf[ofs+3]&0xFF,vmecc->rbuf[ofs+4]&0xFF,vmecc->rbuf[ofs+5]&0xFF);
 	  VCC_UTIL_PKTRCV_src_addr = ctemp;
          ctemp[0]='\0';
          hdr.full=vmecc->rbuf[HDR_OFF];
          new_pkt=false;
          if(hdr.tg.prio == 1){
            strcat(ctemp,"prio");
	  }
          if(hdr.tg.newp == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"new");
            new_pkt=true;
	  }
          if(hdr.tg.frag == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"frag");
	  }
          if(hdr.tg.spnt == 1){
            if(strlen(ctemp)>0)strcat(ctemp," : ");
            strcat(ctemp,"spnt");
	  }
 	  VCC_UTIL_PKTRCV_pkt_flags = ctemp;
          if(new_pkt){
            strcpy(ctemp,"Seq#");
            sprintf(ctemp2,"%d",SEQ_PKT_ID(vmecc->rbuf));
	  }
          else {
            strcpy(ctemp,"Frag#");
            sprintf(ctemp2,"%d",FRAG_NUM(vmecc->rbuf));
	  }
 	  VCC_UTIL_PKTRCV_frg_seq = ctemp;
          VCC_UTIL_PKTRCV_frg_seq_num = ctemp2;

          int ack_stat = AK_STATUS(vmecc->rbuf);
          sprintf(ctemp,"%d",ack_stat);
 	  VCC_UTIL_PKTRCV_ack_num = ctemp;
 	  VCC_UTIL_PKTRCV_ack_stat = ak_status[ack_stat].mnem;
          if(WITH_DATA(vmecc->rbuf)==1){
            strcpy(ctemp," with data");
	  }
	  else {
            strcpy(ctemp," no data");
	  }
 	  VCC_UTIL_PKTRCV_ack_stat += ctemp;

          sprintf(ctemp,"%d",pkt_type);
 	  VCC_UTIL_PKTRCV_pkt_typ_num = ctemp;
          switch(pkt_type){
          case LPBCK_PKT:
            strcpy(ctemp,"LPBCK");
            break;
          case TXNWRDS_PKT:
            strcpy(ctemp,"TXNWRDS");
            break;
	  case EXFIFO_PKT:
            strcpy(ctemp,"EXFIFO");
            break;
	  case VMED08_PKT:
            strcpy(ctemp,"VME_D08");
            break;
	  case VMED16_PKT:
            strcpy(ctemp,"VME_D16");
            break;
	  case VMED32_PKT:
            strcpy(ctemp,"VME_D32");
            break;
	  case VMED64_PKT:
            strcpy(ctemp,"VME_D64");
            break;
	  case JTAG_MOD_PKT:
            strcpy(ctemp,"JTAG_MOD");
            break;
	  case CNFG_MOD_PKT:
            strcpy(ctemp,"CNFG_MOD");
            break;
	  case FLSH_RBK_PKT:
            strcpy(ctemp,"FLSH_RBK");
            break;
	  case ETH_NET_PKT:
            strcpy(ctemp,"ETH_NET");
            break;
	  case IHD08_STAT_ID_PKT:
            strcpy(ctemp,"IH_D08");
            break;
	  case IHD16_STAT_ID_PKT:
            strcpy(ctemp,"IH_D16");
            break;
	  case IHD32_STAT_ID_PKT:
            strcpy(ctemp,"IH_D32");
            break;
	  case INFO_PKT:
            strcpy(ctemp,"INFO");
            break;
	  case WARN_PKT:
            strcpy(ctemp,"WARN");
            break;
	  case ERR_PKT:
            strcpy(ctemp,"ERROR");
            break;
	  default:
            strcpy(ctemp,"Unk. Pkt. Typ");
            break;
	  }
 	  VCC_UTIL_PKTRCV_pkt_type = ctemp;

          if(new_pkt){
	    sprintf(ctemp,"%d",PROC_TAG(vmecc->rbuf));
	    VCC_UTIL_PKTRCV_prc_tag = ctemp;
	    save1 = VCC_UTIL_PKTRCV_prc_tag;
	    sprintf(ctemp,"%02X",vmecc->rbuf[CMND_ACK_OFF]&0XFF);
	    VCC_UTIL_PKTRCV_pkt_cmnd = ctemp;
	    save2 = VCC_UTIL_PKTRCV_pkt_cmnd;
	  }
          else{
	    VCC_UTIL_PKTRCV_prc_tag = save1;
	    VCC_UTIL_PKTRCV_pkt_cmnd = save2;
	  }
          sprintf(ctemp,"%d",wc);
          VCC_UTIL_PKTRCV_wrd_cnt = ctemp;
	}
        else {
	  std::cout << npkts << " packets requested " << pkt << " packets read" << std::endl;
          strcpy(ctemp,"No more packets in driver\n");
          VCC_UTIL_PKTRCV_rbk_data += ctemp;
	}
      }
    }
    this->VMECCGUI_pkt_rcv(in,out);
}

void EmuPeripheralCrateConfig::VMECCGUI_misc_utils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  CNFG_ptr rbk_cp;
  int msglvl;
  char ctemp[256];
  static bool first = true;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();
  if(first){
    first = false;
    rbk_cp = vmecc->read_crs();
    if((rbk_cp->rst_misc & RST_CR_WARN)== RST_CR_WARN){
      VCC_UTIL_misc_warn="Enabled";
    }
    else {
      VCC_UTIL_misc_warn="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_STRTUP)== RST_CR_STRTUP){
      VCC_UTIL_misc_strtup="Enabled";
    }
    else {
      VCC_UTIL_misc_strtup="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_INT)== RST_CR_INT){
      VCC_UTIL_misc_int="Enabled";
    }
    else {
      VCC_UTIL_misc_int="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_FP)== RST_CR_FP){
      VCC_UTIL_misc_fp="Enabled";
    }
    else {
      VCC_UTIL_misc_fp="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_SRST)== RST_CR_SRST){
      VCC_UTIL_misc_srst="Enabled";
    }
    else {
      VCC_UTIL_misc_srst="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_HR)== RST_CR_HR){
      VCC_UTIL_misc_hr="Enabled";
    }
    else {
      VCC_UTIL_misc_hr="Disabled";
    }
    if((rbk_cp->rst_misc & RST_CR_JTAG)== RST_CR_JTAG){
      VCC_UTIL_misc_jtag="Enabled";
    }
    else {
      VCC_UTIL_misc_jtag="Disabled";
    }
    if((rbk_cp->ether & ETH_CR_SPONT)== ETH_CR_SPONT){
      VCC_UTIL_misc_spont="Enabled";
    }
    else {
      VCC_UTIL_misc_spont="Disabled";
    }
    msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
    sprintf(ctemp,"%d",msglvl);
    VCC_UTIL_misc_rd_msglvl = ctemp;
    VCC_UTIL_misc_wrt_msglvl = ctemp;
    free(rbk_cp);
  }

  char title[] = "VCC Utilities: Misc. Commands";
  char pbuf[300];
  sprintf(pbuf,"%s<br>Current Crate is %s<br>MAC Addr: %02x-%02x-%02x-%02x-%02x-%02x",title,(lccc->GetLabel()).c_str(),lccc->vmeController()->GetDestMAC(0),lccc->vmeController()->GetDestMAC(1),lccc->vmeController()->GetDestMAC(2),lccc->vmeController()->GetDestMAC(3),lccc->vmeController()->GetDestMAC(4),lccc->vmeController()->GetDestMAC(5));
  //
  VCCHeader(in,out,title,pbuf);
  //

  this->VMECC_UTIL_Menu_Buttons(in,out);

   std::string VCC_MISC_DO =
     toolbox::toString("/%s/VCC_MISC_DO",getApplicationDescriptor()->getURN().c_str());

  *out << "<form action=\"" << VCC_MISC_DO << "\" method=\"GET\">" << std::endl;
  *out << "<fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Misc. Commands</legend>" << std::endl;
  *out << "<div align=\"center\">" << std::endl;
  *out << "  <table border=\"3\" cellspacing=\"2\" cellpadding=\"0\" bgcolor=" << VCC_UTIL_curr_color << " style=\"border-collapse: collapse\">" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Startup/Shutdown Packets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td>Warn on shutdown:</td>" << std::endl;
  *out << "             <td><input name=\"misc_warn_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_warn_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_warn << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>Send pkt. on startup:</td>" << std::endl;
  *out << "             <td><input name=\"misc_strt_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_strt_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_strtup << "</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td>" << std::endl;
  *out << "    <td rowspan=\"3\" valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Reload/Reset Options</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"2\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td>Source</td>" << std::endl;
  *out << "             <td>Current</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_int_chk\" type=\"checkbox\" value=\"int_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Internal</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_int << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_fp_chk\" type=\"checkbox\" value=\"fp_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Front Panel</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_fp << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_srst_chk\" type=\"checkbox\" value=\"srst_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">SYSRESET</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_srst << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_hr_chk\" type=\"checkbox\" value=\"hr_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">Hard Reset</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_hr << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_jtag_chk\" type=\"checkbox\" value=\"jtag_ck\"></td>" << std::endl;
  *out << "             <td align=\"left\">JTAG (PROM)</td>" << std::endl;
  *out << "             <td align=\"right\">" << VCC_UTIL_misc_jtag << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td></td>" << std::endl;
  *out << "             <td><input name=\"misc_rst_src_wrt\" type=\"submit\" value=\"Write\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td><input name=\"misc_rst_src_rd\" type=\"submit\" value=\"Read\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td><input name=\"misc_int_rst\" type=\"submit\" value=\"Int. Reload Cmnd.\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_jtag_rst\" type=\"submit\" value=\"JTAG Reload Cmnd.\"></td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td><input name=\"misc_sysrst\" type=\"submit\" value=\"VME SYSRESET\"></td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <fieldset><legend style=\"font-size: 16pt;\" align=\"center\">Errors, Warnings and Info Packets</legend>" << std::endl;
  *out << "         <table cellspacing=\"2\" cellpadding=\"4\" bgcolor=" << VCC_UTIL_curr_color << ">" << std::endl;
  *out << "           <tr>" << std::endl;
  *out << "             <td>Spontaneous Packets:</td>" << std::endl;
  *out << "             <td><input name=\"misc_spont_ena\" type=\"submit\" value=\"Ena\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>/</td>" << std::endl;
  *out << "             <td><input name=\"misc_spont_dis\" type=\"submit\" value=\"Dis\" " << VCC_UTIL_acc_cntrl << "></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_spont << "</td>" << std::endl;
  *out << "           </tr><tr>" << std::endl;
  *out << "             <td>Message Level:</td>" << std::endl;
  *out << "             <td colspan=\"3\"><input name=\"misc_msglvl\" type=\"submit\" value=\"Set\" " << VCC_UTIL_acc_cntrl << ">" << std::endl;
  *out << "                 <input align=\"right\" maxwidth=\"20\" size=\"2\" value=\"" << VCC_UTIL_misc_wrt_msglvl << "\" name=\"rd_msglvl\" type=\"text\"></td>" << std::endl;
  *out << "             <td>" << VCC_UTIL_misc_rd_msglvl << "</td>" << std::endl;
  *out << "           </tr>" << std::endl;
  *out << "         </table>" << std::endl;
  *out << "      </fieldset>" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "    <tr><td valign=\"top\">" << std::endl;
  *out << "      <input name=\"misc_refresh\" type=\"submit\" value=\"Refresh\">" << std::endl;
  *out << "    </td></tr>" << std::endl;
  *out << "  </table>" << std::endl;
  *out << "</div>" << std::endl;
  *out << "</fieldset>" << std::endl;
  *out << "</form>" << std::endl;

  VCC_UTIL_curr_page = VCC_MISC;
}

void EmuPeripheralCrateConfig::VCC_MISC_DO(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
  CNFG_ptr rbk_cp;
  char ctemp[256];
  int msglvl;
  Crate *lccc;
  lccc = VCC_UTIL_curr_crate;
  vmecc=lccc->vmecc();

    std::cout<<" entered VCC_MISC_DO"<<std::endl;
    cgicc::Cgicc cgi(in);
    const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    std::string guiStr = env.getQueryString() ;
    std::cout << guiStr << std::endl ;

    cgicc::form_iterator misc_warn_ena = cgi.getElement("misc_warn_ena");
    cgicc::form_iterator misc_warn_dis = cgi.getElement("misc_warn_dis");
    cgicc::form_iterator misc_strt_ena = cgi.getElement("misc_strt_ena");
    cgicc::form_iterator misc_strt_dis = cgi.getElement("misc_strt_dis");
    cgicc::form_iterator misc_spont_ena = cgi.getElement("misc_spont_ena");
    cgicc::form_iterator misc_spont_dis = cgi.getElement("misc_spont_dis");
    cgicc::form_iterator misc_msglvl = cgi.getElement("misc_msglvl");
    cgicc::form_iterator misc_rst_src_wrt = cgi.getElement("misc_rst_src_wrt");
    cgicc::form_iterator misc_rst_src_rd = cgi.getElement("misc_rst_src_rd");
    cgicc::form_iterator misc_int_chk = cgi.getElement("misc_int_chk");
    cgicc::form_iterator misc_fp_chk = cgi.getElement("misc_fp_chk");
    cgicc::form_iterator misc_srst_chk = cgi.getElement("misc_srst_chk");
    cgicc::form_iterator misc_hr_chk = cgi.getElement("misc_hr_chk");
    cgicc::form_iterator misc_jtag_chk = cgi.getElement("misc_jtag_chk");
    cgicc::form_iterator misc_int_rst = cgi.getElement("misc_int_rst");
    cgicc::form_iterator misc_jtag_rst = cgi.getElement("misc_jtag_rst");
    cgicc::form_iterator misc_sysrst = cgi.getElement("misc_sysrst");
    cgicc::form_iterator misc_refresh = cgi.getElement("misc_refresh");

    if(misc_warn_ena != cgi.getElements().end()) {
      vmecc->enable_warn_on_shutdown();
      VCC_UTIL_misc_warn = "Enabled";
    }
    if(misc_warn_dis != cgi.getElements().end()) {
      vmecc->disable_warn_on_shutdown();
      VCC_UTIL_misc_warn = "Disabled";
    }
    if(misc_strt_ena != cgi.getElements().end()) {
      vmecc->snd_startup_pkt();
      VCC_UTIL_misc_strtup = "Enabled";
    }
    if(misc_strt_dis != cgi.getElements().end()) {
      vmecc->no_startup_pkt();
      VCC_UTIL_misc_strtup = "Disabled";
    }
    if(misc_spont_ena != cgi.getElements().end()) {
      vmecc->set_clr_bits(SET, ETHER, ETH_CR_SPONT);
      VCC_UTIL_misc_spont = "Enabled";
    }
    if(misc_spont_dis != cgi.getElements().end()) {
      vmecc->set_clr_bits(CLR, ETHER, ~ETH_CR_SPONT);
      VCC_UTIL_misc_spont = "Disabled";
    }
    if(misc_msglvl != cgi.getElements().end()) {
      msglvl = cgi["rd_msglvl"]->getIntegerValue();
      vmecc->set_clr_bits(CLR, RST_MISC, ~RST_CR_MSGLVL);
      vmecc->set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL & (msglvl<<8));
      sprintf(ctemp,"%d",msglvl);
      VCC_UTIL_misc_wrt_msglvl = ctemp;
      rbk_cp = vmecc->read_crs();
      msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
      sprintf(ctemp,"%d",msglvl);
      VCC_UTIL_misc_rd_msglvl = ctemp;
      free(rbk_cp);
    }
    if(misc_rst_src_wrt != cgi.getElements().end()) {
      unsigned int val = 0;
      if(misc_int_chk != cgi.getElements().end()) {
        val |= RST_CR_INT;
      }
      if(misc_fp_chk != cgi.getElements().end()) {
        val |= RST_CR_FP;
      }
      if(misc_srst_chk != cgi.getElements().end()) {
        val |= RST_CR_SRST;
      }
      if(misc_hr_chk != cgi.getElements().end()) {
        val |= RST_CR_HR;
      }
      if(misc_jtag_chk != cgi.getElements().end()) {
        val |= RST_CR_JTAG;
      }
      vmecc->set_clr_bits(CLR, RST_MISC, ~(0x1F));
      vmecc->set_clr_bits(SET, RST_MISC, val);
    }
    if(misc_rst_src_rd != cgi.getElements().end() || misc_refresh != cgi.getElements().end()) {
      rbk_cp = vmecc->read_crs();
      if((rbk_cp->rst_misc & RST_CR_INT)!=0){
        VCC_UTIL_misc_int = "Enabled";
      }else{
        VCC_UTIL_misc_int = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_FP)!=0){
        VCC_UTIL_misc_fp = "Enabled";
      }else{
        VCC_UTIL_misc_fp = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_SRST)!=0){
        VCC_UTIL_misc_srst = "Enabled";
      }else{
        VCC_UTIL_misc_srst = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_HR)!=0){
        VCC_UTIL_misc_hr = "Enabled";
      }else{
        VCC_UTIL_misc_hr = "Disabled";
      }
      if((rbk_cp->rst_misc & RST_CR_JTAG)!=0){
        VCC_UTIL_misc_jtag = "Enabled";
      }else{
        VCC_UTIL_misc_jtag = "Disabled";
      }
      if(misc_refresh != cgi.getElements().end()){
	if((rbk_cp->rst_misc & RST_CR_WARN)== RST_CR_WARN){
	  VCC_UTIL_misc_warn="Enabled";
	}
	else {
	  VCC_UTIL_misc_warn="Disabled";
	}
	if((rbk_cp->rst_misc & RST_CR_STRTUP)== RST_CR_STRTUP){
	  VCC_UTIL_misc_strtup="Enabled";
	}
	else {
	  VCC_UTIL_misc_strtup="Disabled";
	}
	if((rbk_cp->ether & ETH_CR_SPONT)== ETH_CR_SPONT){
	  VCC_UTIL_misc_spont="Enabled";
	}
	else {
	  VCC_UTIL_misc_spont="Disabled";
	}
	msglvl = (rbk_cp->rst_misc & RST_CR_MSGLVL)>>8;
	sprintf(ctemp,"%d",msglvl);
	VCC_UTIL_misc_rd_msglvl = ctemp;
      }
      free(rbk_cp);
    }
    if(misc_int_rst != cgi.getElements().end()) {
      vmecc->force_reload();
    }
    if(misc_jtag_rst != cgi.getElements().end()) {
      vmecc->reload_fpga();
    }
    if(misc_sysrst != cgi.getElements().end()) {
      vmecc->set_clr_bits(SET, VME, VME_CR_FRC_SRST);
    }
    this->VMECCGUI_misc_utils(in,out);
}
//
// BGB End of New Controller Utils 
//

 }  // namespace emu::pc
}  // namespace emu
