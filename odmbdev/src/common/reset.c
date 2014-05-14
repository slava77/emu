#include <stdio.h>

extern int nwbuf;
extern char *wbuf;
extern char rbuf[];
extern int nrbuf;
extern int fd_schar;
extern char hw_source_addr[];
extern char hw_dest_addr[];

int main(int argc,char *argv[])
{
  int i;
  int n,l,lcnt;
  int hex;

  eth_open("schar3");

  eth_reset();
  
  eth_close();

  return 0;
}
