//
// HistoDisplayHelpText.cc
// by Tetsuo Arisawa 
// 1999 11 12

#include "HistoDisplayHelpText.hh"

#ifndef WIN32

const char gHelpHistoDisplay[] = "\
\n \t CDF Monitoring Display \n\n\
This GUI is under construction and updated daily.\n\
Also, this help is not complete at all.\n\
Simple explanation (of the previous version) was \n\
in a talk of T.Arisawa in CDF WEEK Meeting 1999 7/29.\n\n\
 1. Input Stream\n\
  Shared memory files and the other ROOT files,\n\
local and remote can be opened by selecting radio buttons\n\
and typing filenames in the box of input stream, such as: \n\n\
\t /dir1/dir2/dir3/filename.map (TMapFile)\n\
\t /dir1/dir2/dir3/filename.root (local file)\n\
\t  http://root.cern.ch/~rdm/hsimple.root\n\
\t  root://b0dau30.fnal.gov:5151/filename.root.\n\n\
  For the client/server socket connection, type\n\
\t (socket server name):(port number)\n\
in the box in stead of a file name.\n\
If you omit the port number, then a default port 9090 \n\
is set.\n\
Input \"localhost\" to use local host as a server\n\
with port of 9090.\n\
 (\"DisplayServer\" Class of F.Hartmann and H.Wenzel is being used \n\
for the socket communication.)\n\n\
 2. List Tree\n\
 After you open the input source, the contents (especially histograms now)\n\
are displayed as a list tree form in the bottom left box\n\
by pressing \"Start\" button.\n\
Horizontal or vertical scroll bar appears when the list tree\n\
exceeds the area.\n\
  You can specify a top folder in the list tree where items from \n\
the input sources are placed by putting a path string such as:\n\
\t folder1/folder2/folder3 \n\
in the \"Top Folder in List Tree\" area before clicking the start button.\n\
Items are put in the folder3 in the folder2 where the folder2 are \n\
in the folder1.\n\
 Protocol to make more detailed list tree structures are \n\
to be determined.  Currently YMON (by G.Veramendi)\n\
and WedgetTest (by H.Wenzel) output histograms are \n\
arranged as hierarchical list tree.\n\
To test, open a YMON root file on cdfsga.fnal.gov:\n\
 \t /data83/upgrade/consumer/greg/YMON_MC.root\n\
and type \"YMON\" or \"/YMON\" as the list tree top folder name,\n\
then nice tree appears. ( Type \"Wedge\" for the WedgeTest.)\n\n\
 3. Draw items\n\
 To handle items on the list tree, select/open/close,\n\
use left mouse button. To draw items such as histograms \n\
click right mouse button. If you click the open folder which contains \n\
several histograms with the right mouse button \n\
all of them are drawn on the same canvas. \n\n\
 4. Canvas handling\n\n\
 5. Automatical Update\n\n\
   Feedback is welcome.\n\n\
  \t Tetsuo Arisawa (Waseda Univ. Japan)\n\
  \t Email: arisawa@fhlab.phys.waseda.ac.jp\n\n\
";

#else
const char gHelpHistoDisplay[]         = "empty";
#endif
