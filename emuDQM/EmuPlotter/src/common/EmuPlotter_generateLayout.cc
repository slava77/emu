#include "EmuPlotter.h"

/*	
======	Generate Layouts file for Global DQM Web GUI from pre-booked Local DQM canvases 
*/	
void EmuPlotter::generateLayout(std::string filename, std::string rootfolder)
{
	std::ofstream out(filename.c_str()); 
	out << "def csclayout(i, p, *rows): i[\"Layouts/CSC Layouts/\" + p] = DQMItem(layout=rows)\n" << std::endl;

	char *stopstring;

	MECanvases_List_iterator itr;
	
	for (itr = commonCanvasesFactory.begin(); itr != commonCanvasesFactory.end(); ++itr) {
                EmuMonitoringCanvas * obj = itr->second;
		out << "csclayout(dqmitems,\"" << obj->getTitle() << "\"," << std::endl;
		int cols = strtol(obj->getParameter("NumPadsX").c_str(),&stopstring, 10);
		int rows = strtol(obj->getParameter("NumPadsY").c_str(),&stopstring, 10);
		for (int i=0; i<rows; i++) {
			out << "[";
			for (int j=1; j<=cols; j++) {
				std::ostringstream st;
				st.clear();
				st << "Pad" << i*cols+j;
				std::string name = obj->getParameter(st.str());
				ME_List_iterator me = commonMEfactory.find(name);
				if (me != commonMEfactory.end()) {
					out << "\"" << rootfolder << "/Common/" << me->second->getFullName() << "\"" 
					<< ((j==cols)?"]":",") << (((i<rows-1) && (j==cols))?",":"") << std::endl;
				}
			}
		}	
//		out << cols << "," << rows << ")\n" << std::endl;
		out << ")\n" << std::endl;
        }
	
	/* 
		Only top level Common canvases are defined
		Need support for templates to add actual DDU and CSC ID canvases
		Otherwise, all possible DDU and CSC ID should be defined
	*/
	return;

	for (itr = dduCanvasesFactory.begin(); itr != dduCanvasesFactory.end(); ++itr) {
                EmuMonitoringCanvas * obj = itr->second;
                out << "csclayout(dqmitems,\"DDU/" << obj->getTitle() << "\"," << std::endl;
		int cols = strtol(obj->getParameter("NumPadsX").c_str(),&stopstring, 10);
                int rows = strtol(obj->getParameter("NumPadsY").c_str(),&stopstring, 10);
                for (int i=0; i<rows; i++) {
                        out << "[";
                        for (int j=1; j<=cols; j++) {
                                std::ostringstream st;
                                st.clear();
                                st << "Pad" << i*cols+j;
				std::string name = obj->getParameter(st.str());
                                ME_List_iterator me = dduMEfactory.find(name);
                                if (me != dduMEfactory.end()) {
                                        out << "\"" << rootfolder << "/DDU_id/" << me->second->getFullName() << "\""
                                        << ((j==cols)?"]":",") << (((i<rows-1) && (j==cols))?",":"") << std::endl;
                                }


                        }
                }
//                out << cols << "," << rows << ")\n" << std::endl;
                out << ")\n" << std::endl;
        }

	for (itr = chamberCanvasesFactory.begin(); itr != chamberCanvasesFactory.end(); ++itr) {
                EmuMonitoringCanvas * obj = itr->second;
                out << "csclayout(dqmitems,\"CSC/" << obj->getTitle() << "\"," << std::endl;
                int cols = strtol(obj->getParameter("NumPadsX").c_str(),&stopstring, 10);
                int rows = strtol(obj->getParameter("NumPadsY").c_str(),&stopstring, 10);
                for (int i=0; i<rows; i++) {
                        out << "[";
                        for (int j=1; j<=cols; j++) {
                                std::ostringstream st;
                                st.clear();
                                st << "Pad" << i*cols+j;
				std::string name = obj->getParameter(st.str());
                                ME_List_iterator me = chamberMEfactory.find(name);
                                if (me != chamberMEfactory.end()) {
                                        out << "\"" << rootfolder << "/CSC_id_id/" << me->second->getFolder() << "/" << me->second->getFullName() << "\""
                                        << ((j==cols)?"]":",") << (((i<rows-1) && (j==cols))?",":"") << std::endl;
                                }
                        }
                }
//                out << cols << "," << rows << ")\n" << std::endl;
                out << ")\n" << std::endl;
        }

	
}
