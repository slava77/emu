#define DBG printf("OK")
#include "csc_display/utils.h"

/* getting form input value from HTTP request, return in string type
 * if no form with the same name, return 0
 */
std::string getFormValue(const std::string& elmt_name, xgi::Input* in) {
    
    const cgicc::Cgicc cgi(in);
    std::string form_value;
    cgicc::const_form_iterator iterator = cgi.getElement(elmt_name);
    
    // if it find the corresponding elmt_name, the iterator won't be the last elements
    if(iterator != cgi.getElements().end()) {
        form_value = cgi[elmt_name]->getValue();
    }
    else {
        printf("Cannot find form with name %s\n", elmt_name.c_str());
        form_value = "";
    }
    return form_value;
    
}

/* getting form input value from HTTP request, return in integer type
 * if no form with the same name, return 0
 */
int getFormIntValue(const std::string& elmt_name, xgi::Input* in) {
    
    const cgicc::Cgicc cgi(in);
    int form_value;
    cgicc::const_form_iterator iterator = cgi.getElement(elmt_name);
    
    // if it find the corresponding elmt_name, the iterator won't be the last elements
    if(iterator != cgi.getElements().end()) {
        form_value = cgi[elmt_name]->getIntegerValue();
    }
    else {
        printf("Cannot find form with name %s\n", elmt_name.c_str());
        form_value = 0;
    }
    return form_value;
    
}

/* Getting the content of a file inside a folder (html, js, css, or other) and put the content into the "content" variable
 * Input:
 * * fileType: specify the folder of the file (html, js, css, or other)
 * * filename: filename of the file inside a folder (including its extension). e.g. helloworld.html
 * * content: pointer to string (unallocated)
 * Output:
 * * allocated content, so remember to free the content outside this function!
 */ 
void getFileContent(const char* fileType, const char * filename, char ** content, int * fSize) {

    // get the application directory and its error handling
    char* appDir = getenv("WEBCSCDISP_DIR");
    if (appDir == NULL) {
        printf("Error: environment variable WEBCSCDISP_DIR is not specified, please specify it to /directory/to/web_csc_display/csc_display/\n");
        exit(101);
    }
    
    // getting the full address of the HTML file
    std::string filePath = std::string(appDir) + "webc/" + fileType + "/" + filename;
    
    // open the corresponding file
    FILE* fp = fopen(filePath.c_str(), "r");
    
    // get the file size
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    
    // allocate the content string and its error handling
    (*content) = (char*) malloc(sizeof(char) * (size+1));
    if ((*content) == NULL) {
        printf("Memory error\n");
        exit(102);
    }
    
    // copy the file content into the content string
    fread((*content), 1, size, fp);
    (*content)[size] = 0;
    
    // update the size parameter
    if (fSize != NULL) {
        *fSize = size;
    }
    
    // close the file
    fclose(fp);
}
