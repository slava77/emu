#ifndef DcsDimService_h
#define DcsDimService_h

#include "dim/dis.hxx"
#include <string>
#include <vector>


  class DcsDimService : public DimService{
  public:
    void *value;
    char service_name[100];
    static bool CONFIRMATION;

    DcsDimService(char *name, char *format, void *value, int size);

    void DcsUpdateService(bool isConfirmationNeeded=true);

  };

#endif
