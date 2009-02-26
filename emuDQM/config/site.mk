#################################################################
#
# site.mk
#
#  Site definitions
#   Environment variables set in 'TrigDAQ.env'
#
#  Author: Holger Stoeck
#  Modifications:
#
#  $Id: site.mk,v 1.5 2009/02/26 14:15:34 barvic Exp $
#
#  Revision History
#  $Log: site.mk,v $
#  Revision 1.5  2009/02/26 14:15:34  barvic
#  Changes for makefile configurations
#
#  Revision 1.4  2006/10/19 20:12:24  barvic
#  changed default path to BOOST libs to /usr/include. boost and boost-devel RPMs should be installed
#
#  Revision 1.3  2006/10/18 17:09:53  pakhotin
#
#  Definitions of errors from DDU trailer were updated. Some histograms were updated.
#
#  Revision 1.2  2006/08/14 13:23:10  barvic
#  config changes for emuDQM
#
#  Revision 1.1  2006/03/15 13:30:32  barvic
#  build files changes
#
#  Revision 1.3  2005/02/08 15:53:18  tfcvs
#  HS - Commit changes before tagging
#
#  Revision 1.2  2004/11/16 10:54:48  tfcvs
#  changed default locations to TestBeam set-up
#
#  Revision 1.1  2004/10/01 11:42:25  tfcvs
#  *** empty log message ***
#
#  Revision 1.16  2004/04/15 19:48:20  tfcvs
#  HS - Added SBS version in TrigDAQ.env
#
#  Revision 1.15  2004/04/15 15:25:02  tfcvs
#  HS - Changes in scripts and makefiles
#
#  Revision 1.14  2003/11/21 01:21:20  tfcvs
#  HS - Added FED directory to config files
# 
#
#################################################################


CMSSW	 = $(HOME)/CMSSW/src
BOOST    = /usr/include
XDAQDIR    = $(XDAQ_DIR)
ROOTDIR    = $(ROOTSYS)
SQLITE	 = $(HOME)/sqlite


#################################################################
#
# End of site.mk
#
#################################################################
