#define DEBUG printf("OK\n")
#include "csc_display/SimpleWeb.h"

XDAQ_INSTANTIATOR_IMPL(csc_display::SimpleWeb)

csc_display::SimpleWeb::SimpleWeb(xdaq::ApplicationStub * s)
  throw (xdaq::exception::Exception): xdaq::Application(s) {
    
    // bind the handler to some functions
    xgi::bind(this,&SimpleWeb::Default, "Default");
    xgi::bind(this,&SimpleWeb::SQLForm, "sqlform");
    xgi::bind(this,&SimpleWeb::LoadJSFile, "js");
    xgi::bind(this,&SimpleWeb::LoadCSSFile, "css");
    xgi::bind(this,&SimpleWeb::ShowImage, "img");
    
    // process handler
    // for testing purpose only ???
    xgi::bind(this,&SimpleWeb::GetInitParams, "init");
    xgi::bind(this,&SimpleWeb::ShowDummyGraphics, "dummygraph");
    xgi::bind(this,&SimpleWeb::ShowGraphics, "graph");
}


void csc_display::SimpleWeb::Default(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
    
    this->showStaticHTML("index.html", out);
    
}


/* This page is for showing sql command textarea form
 * It takes the template from "sqlform.html"
 */
void csc_display::SimpleWeb::SQLForm(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    this->showStaticHTML("sqlform.html", out);
    
}


/* Load the corresponding JS file specified with URL /js?f=filename.js
 */
void csc_display::SimpleWeb::LoadJSFile(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    // getting the file name
    std::string filename = getFormValue("f", in);
    
    // getting the file content 
    char * content;
    getFileContent("js", filename.c_str(), &content);
    
    // show the content as javascript
    out->getHTTPResponseHeader().addHeader("Content-type", "text/script");
    *out << content << std::endl;
    free(content);
    
}


/* Load the corresponding CSS file specified with URL /css?f=filename.css
 */
void csc_display::SimpleWeb::LoadCSSFile(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    // getting the file name
    std::string filename = getFormValue("f", in);
    
    // getting the file content 
    char * content;
    getFileContent("css", filename.c_str(), &content);
    
    // show the content as javascript
    out->getHTTPResponseHeader().addHeader("Content-type", "text/css");
    *out << content << std::endl;
    free(content);
    
}


/* Load the corresponding image file specified with URL /img?f=filename.png
 * Currently support PNG only!
 */
void csc_display::SimpleWeb::ShowImage(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    // getting the file name
    std::string filename = getFormValue("f", in);
    
    // getting the file content 
    char * content;
    int size;
    getFileContent("img", filename.c_str(), &content, &size);
    
    // show the content as javascript
    out->getHTTPResponseHeader().addHeader("Content-type", "image/png");
    
    for (int i = 0; i < size; i++) {
        *out << content[i];
    }
    free(content);
    
}


void csc_display::SimpleWeb::showStaticHTML(const char* filename, xgi::Output * out) {
    // getting the file content
    char * content;
    getFileContent("", filename, &content);
    
    // show the content
    out->getHTTPResponseHeader().addHeader("Content-type", "text/html");
    *out << content << std::endl;
    free(content);
}


void csc_display::SimpleWeb::GetInitParams(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    try {
        // switch between using dummy file or real file
        bool dummy = false;
        
        // get the real data
        if (!dummy) {
            // to be the output string
            JSON* json = new JSON();
            JSON* jsonData = new JSON();
            
            // get the filename from the HTTP argument
            std::string filename = getFormValue("f", in);
            
            // open the file using daq reader or NULL if there is no file found
            emu::daq::reader::RawDataFile* fd = NULL;
            try {
                fd = new emu::daq::reader::RawDataFile(filename.c_str(), emu::daq::reader::RawDataFile::DDU);
            } catch (const std::runtime_error &e) {
                fd = NULL;
            }
            
            int totalEvent;
            int* chambers = NULL;
            int chambersLen;
            
            // if the file is error, show the error JSON
            if (fd == NULL) {
                json->addInt("status", 0);
                json->addStr("msg", "File not found");
                json->addObj("data", jsonData);
            }
            else {
                // get the chambers array (also performing binary check)
                int rtval = getChambers(fd, filename.c_str(), &chambers, &chambersLen);
                
                // if binary check fails, then show the error message
                if (rtval > 0) {
                    json->addInt("status", 0);
                    json->addStr("msg", "Binary check error. Please check your file format");
                    json->addObj("data", jsonData);
                }
                else {
                    // get the total event number
                    totalEvent = getTotalEvent(fd, filename.c_str());
                    
                    // show the result JSON
                    jsonData->addArr("chambersID", chambers, chambersLen);
                    jsonData->addInt("totalEventNum", totalEvent);
                    json->addInt("status", 1);
                    json->addStr("msg", "Success");
                    json->addObj("data", jsonData);
                }
            }
            
            *out << json->str() << std::endl;
            
            if (chambers != NULL) free(chambers);
            delete fd;
            delete jsonData;
            delete json;
        }
        
        // dummy file is always a successful response with dummy data
        else {
            // getting the file content
            char * content;
            getFileContent("", "init.json", &content);
            
            // show the content
            out->getHTTPResponseHeader().addHeader("Content-type", "text/js");
            *out << content << std::endl;
            free(content);
        }
    }
    catch (...) {
        *out << "Unknown error occured" << std::endl;
    }
}

void csc_display::SimpleWeb::ShowDummyGraphics(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    // get name of the image
    std::string id = getFormValue("evt", in);
    std::string name = std::string("gaus-") + id;
    
    // draw gaussian graphic onto a canvas
    TCanvas* c = new TCanvas;
    TH1F* h = new TH1F(name.c_str(), name.c_str(), 100, -5, 5);
    h->FillRandom("gaus", 10000);
    h->Draw();
    
    // create image from the canvas
    gSystem->ProcessEvents();
    TImage* img = TImage::Create();
    img->FromPad(c);
    
    // get the image's buffer
    char * imgBuf;
    int size;
    img->GetImageBuffer(&imgBuf, &size, TImage::kPng);
    
    // write all the buffer into the http response
    out->getHTTPResponseHeader().addHeader("Content-type", "image/png");
    for (int i = 0; i < size; i++) {
        *out << imgBuf[i];
    }
    
    delete h;
    delete c;
    delete img;
    free(imgBuf);
}

void csc_display::SimpleWeb::ShowGraphics(xgi::Input * in, xgi::Output * out)
  throw(xgi::exception::Exception) {
    
    try {
        // getting the parameters from the HTTP request
        int event_num  = getFormIntValue("evt", in);
        int chamberID  = getFormIntValue("cid", in);
        int withHeader = getFormIntValue("hdr", in);
        int hack       = getFormIntValue("hack", in);
        int autoscale  = getFormIntValue("ascale", in);
        int config     = getFormIntValue("conf", in);
        
        std::string filename = getFormValue("f", in);
        
        // set the default values
        if (event_num == 0) event_num = 1;
        if (config == 0) config = 1;
        if (filename == "") filename = "/afs/cern.ch/user/j/jhaley/TriDAS/emu/fast_daq/cosmics_2DMBs.raw";
        
        // get the config string
        char configStr[30];
        switch(config) {
            case 1: strcpy(configStr, "Wires And Strips"); break;
            case 2: strcpy(configStr, "Wires And Strips (Peak)"); break;
            case 3: strcpy(configStr, "CLCT Time"); break;
            case 4: strcpy(configStr, "ALCT Time"); break;
            case 5: strcpy(configStr, "CFEB Time Scale"); break;
            case 6: strcpy(configStr, "Ctrig"); break;
            case 7: strcpy(configStr, "Atrig"); break;
            default:strcpy(configStr, "No Config");
        }
        
        // open the file using daq reader or NULL if there is no file found
        emu::daq::reader::RawDataFile* fd = NULL;
        try {
            fd = new emu::daq::reader::RawDataFile(filename.c_str(), emu::daq::reader::RawDataFile::DDU);
        } catch (const std::runtime_error &e) {
            fd = NULL;
        }
        
        char* imgBuf;
        int size;
        
        // get the data from file descriptor based on event number and chamber ID
        data_type data;
        j_common_type j_data;
        getDataFromFD(fd, filename.c_str(), event_num, chamberID, &data, (hack == 1));
        unpack_data_cc(data, &j_data);
        
        
        // set the canvas
        TCanvas* c = new TCanvas("c", "c", 700, 700);
        float yBorder = 0.93;
        TPad* headPad = new TPad("head", "head", 0, yBorder, 1, 1);
        TPad* bodyPad = new TPad("body", "body", 0, 0, 1, yBorder);
        headPad->Draw();
        bodyPad->Draw();
        
        // add header
        addHeader(headPad, event_num, chamberID, configStr);
        
        // draw graphics based on its configuration
        switch(config) {
            case 1: // Wires And Strips
                getWiresAndStripsGraph(bodyPad, data, j_data, &imgBuf, &size); break;
            
            case 2: // Wires And Strips (Peak)
                getWiresAndStripsGraph(bodyPad, data, j_data, &imgBuf, &size, true); break;
            
            case 3: // CLCT Time
                getCLCTTimeGraph(bodyPad, data, j_data, &imgBuf, &size); break;
            
            case 4: // ALCT Time
                getALCTTimeGraph(bodyPad, data, j_data, &imgBuf, &size); break;
            
            case 5: // CFEB Time Scale
                getCFEBTimeScaleGraph(bodyPad, data, j_data, &imgBuf, &size); break;
                
            case 6: // Ctrig
                getCtrigGraph(bodyPad, data, j_data, &imgBuf, &size); break;
                
            case 7: // Atrig
                getAtrigGraph(bodyPad, data, j_data, &imgBuf, &size); break;
            
            default:
                break;
        }
        
        delete headPad;
        delete bodyPad;
        delete c;
        
        // write all the buffer into the http response
        out->getHTTPResponseHeader().addHeader("Content-type", "image/png");
        for (int i = 0; i < size; i++) {
            *out << imgBuf[i];
        }
        
        // delete fd;
        free(imgBuf);
    }
    catch (...) {
        *out << "Unknown error occured" << std::endl;
    }
}
