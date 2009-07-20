#ifndef EmuTFxmlParsing_h
#define EmuTFxmlParsing_h

#include <string>
#include <vector>
#include <map>

namespace EmuTFxmlParsing {

class HistAttributes {
public:
	std::string type;
	std::string prefix;
	std::string name;
	std::string title;
	int xMax, xMin, xNbins;
	int yMax, yMin, yNbins;
	std::string xLabel;
	std::string yLabel;
	std::vector<std::string> xBinLabels;
	std::vector<std::string> yBinLabels;
	std::vector<std::string> options;
	HistAttributes(void):
		type("Unknown"),prefix("Unknown"),
		name("Unknown"),title("Untitled"),
		xMax(0),xMin(0),xNbins(0),
		yMax(0),yMin(0),yNbins(0),
		xLabel(""),yLabel(""){}
};

std::map<std::string,HistAttributes> parseHistXML(const char *filename) throw (std::exception);

class CanvasAttributes {
public:
	std::string type;
	std::string prefix;
	std::string name;
	std::string title;
	std::map< std::string, std::vector<std::string> > items;
	std::vector<std::string> options;
	CanvasAttributes(void):
		type("Unknown"),prefix("Unknown"),
		name("Unknown"),title("Untitled"){}
};


std::map<std::string,CanvasAttributes> parseCanvasXML(const char *filename) throw (std::exception);

class CheckAttributes {
public:
	std::string name;
	std::string title;
	std::map< std::string, std::vector<std::string> > items;
	std::vector<std::string> options;
	CheckAttributes(void):name("Unknown"),title("Untitled"){}
};

std::map<std::string,CheckAttributes> parseCheckXML(const char *filename) throw (std::exception);

};//end of namespace

#endif
