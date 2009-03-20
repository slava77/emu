#ifndef LOAD_h
#define LOAD_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <string>
#include <iostream>

namespace emu {
  namespace e2p {

class LOAD
{  
public:
  LOAD();
  ~LOAD();
  
  inline char *Content() { return buffmem; }
  inline int Content_Size() { return cont_size; }
  void init(std::string url);
  int reload();
  void end();

private:
  bool inited_;
  char *buffmem;
  size_t cont_size;
  size_t mem_size;
  std::string URL_;

  CURL *curl_handle;
  static size_t Callback(void *ptr, size_t size, size_t nmemb, void *data);
  size_t getit(void *ptr, size_t size);
};

  } // namespace emu::e2p
} // namespace emu

#endif
