#ifndef DcsDimService_h
#define DcsDimService_h

#include <dis.hxx>
#include <string>
#include <vector>


  class DcsDimService : public DimService{
  public:
    void *value;
    static bool CONFIRMATION;

    DcsDimService(char *name, char *format, void *value, int size);

    void DcsUpdateService(bool isConfirmationNeeded=true);

  };

#endif
