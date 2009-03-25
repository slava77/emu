#include "emu/x2p/LOAD.h"

namespace emu {
  namespace x2p {

LOAD::LOAD()
{
  buffmem = NULL; 
  mem_size = 0;   
  cont_size = 0;  
  
  inited_ = false;
}

size_t LOAD::Callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  LOAD *This = (LOAD *)data;
  return This->getit(ptr, realsize);
}

size_t LOAD::getit(void *ptr, size_t realsize)
{
  if((cont_size + realsize + 1) > mem_size) 
  {   buffmem = (char *)realloc(buffmem, cont_size + realsize + 1);
      mem_size = cont_size + realsize + 1;
  }
  if (buffmem) {
    memcpy(&(buffmem[cont_size]), ptr, realsize);
    cont_size += realsize;
    buffmem[cont_size] = 0;
  }
  return realsize;
}

void LOAD::init(std::string url)
{
  buffmem = (char *)malloc(100000);   // initial buffer size 
  if(buffmem==NULL) return;
  mem_size = 100000;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
  URL_ = url;

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, Callback);

  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)this);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  inited_ = true;
}

int LOAD::reload()
{
  if(!inited_) return 0;
  /* discard the old content */
  cont_size=0;
  /* get it! */
  curl_easy_perform(curl_handle);

  return cont_size;
}

void LOAD::end()
{
  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  if(buffmem)  free(buffmem);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup();
}

  } // namespace emu::x2p
} // namespace emu

