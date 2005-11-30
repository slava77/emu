
#ifdef D360

//----------------------------------------------------------------------
// $Id: VMEController.cc,v 2.4 2005/11/30 12:59:59 mey Exp $
// $Log: VMEController.cc,v $
// Revision 2.4  2005/11/30 12:59:59  mey
// DMB firmware loading
//
// Revision 2.3  2005/11/25 23:43:00  mey
// Update
//
// Revision 2.2  2005/11/21 15:48:24  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:24  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.25  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//----------------------------------------------------------------------
#include "VMEController.h"
#include "VMEModule.h"
#include "Crate.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl; 
#define PRINTSTRING(x) std::cout << #x << std::endl; 
#endif



VMEController::VMEController(int crate, std::string ipAddr, int port): 
 theSocket(0), ipAddress_(ipAddr), port_(port), theCurrentModule(0),
 indian(SWAP),  max_buff(0), tot_buff(0), Tbytes(0), crate_(crate)
{
  /*
  * Fill "serv_addr" with the address of the server we want to connect with
  */
  bzero( (char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
  serv_addr.sin_port        = htons(port);

  int socket = openSocket();
  //
  fpacket_delay = 0;
  packet_delay = 0;
  packet_delay_flg = 0;
  //
  DELAY2 = 0.016;
  DELAY3 = 16.384;
  //

}


VMEController::~VMEController(){
  std::cout << "VMEController: destructing  ... closing socket " << std::endl;
  closeSocket();
}


void VMEController::start(VMEModule * module) {
  if(theCurrentModule != module) {
    PRINTSTRING(OVAL: start method defined in VMEController.cc is starting )
    end();
    PRINTSTRING(OVAL: starting current module);
    module->start();
    PRINTSTRING(OVAL: current module was started);
    theCurrentModule = module;
  }
}



void VMEController::end() {
  if(theCurrentModule != 0) {
    theCurrentModule->end();
    theCurrentModule = 0;
  }
  assert(plev !=2);
  idevo = 0;
  feuseo = 0;
}


void VMEController::send_last() {
  char rcvx[2];
  char sndx[2];
  
  if(plev==2){
    scan(TERMINATE,sndx,0,rcvx,2);
  }
  plev=1;
}


int VMEController::openSocket() {
  if(theSocket == 0) {
    // open a TCP socket ( an internet stream socket )
    // cout<<"VMEController: open socket stream" << endl;

#ifndef DUMMY
    if( (theSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
      std::cerr<<"VMEController: error opening stream socket" << theSocket <<std::endl;

    // connect to the server
    std::cout <<"VMEController: crate " <<  crate_ << " connecting to server " 
	<<  ipAddress_.c_str() << ":" << port_ << std::endl;
    int stat = connect(theSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if( stat < 0) {
      std::cerr<<"VMEController: ERROR in connection - " << strerror(errno) << std::endl;
    }
#endif
  }

  return theSocket;
}


void VMEController::closeSocket() {
#ifndef DUMMY
  close(theSocket);
#endif
  theSocket = 0;
}

// changed return code to nrcv
int VMEController::readn(char *line)
{
  int nrcv = 0;
#ifndef DUMMY
  nrcv = read(theSocket, line, MAXLINE);
#endif
  // OVAL is a CMS validation tool.  We'd just grep for this line
  // we want to make sure every byte read and written from the
  // crate is identical to other versions of the code.
  #ifdef debugV 
    std::cout << "VMEController: readn sock " << theSocket;
    for(int i = 0; i < nrcv; ++i) {
      std::cout << " " << (int) line[i];
    }
    std::cout << std::endl;
  #endif

  /* Bytes swap is done here rather than at VME -Jan/20/2000 */
  /*  strcopy11(line,0,line2,n); */         /* swap */

  if(nrcv < 0) {
    std::cerr <<"VMEController: read ERROR - " << strerror(errno) << std::endl;
  }
  return nrcv;
}


int VMEController::writen(register const char *ptr, register int nbytes)
{
  #ifdef debugV
    std::cout << "VMEController: writen " << std::dec << theSocket << " " ;
    for(int i = 0; i < nbytes; ++i) {
      std::cout << (int) ptr[i] << " " ;
    }
    std::cout << std::endl;
  #endif
  int nleft = 0;
#ifndef DUMMY
  nleft  =  nbytes;
  while (nleft >0) {
    int nwritten = write(theSocket,ptr,nleft);

    if(nwritten <=0) {
      std::cerr << "VMEController: write ERROR -  " << strerror(errno) << std::endl;
      return nwritten;  /*error*/
    }

    nleft -= nwritten;
    ptr   += nwritten;
  }
#endif
  if(nleft > 0) {
     std::cout << "VMEController: write ERROR - nleft= " << nleft << std::endl;
  }
  return(nbytes - nleft);
}


int VMEController::writenn(const char *ptr,int nbytes)
{
  char  size[4];
  int nwritten = 0;
  int inttmp = nbytes;
  char * sizec = (char *) &inttmp;  // size of command

  /* endian format for VME computer, so byte swap is done here
     rather than in VME --- Jan-8-99 Chang*/
  if(indian==SWAP){
  size[0] = sizec[1];
  size[1] = sizec[0];
  size[2] = sizec[3];
  size[3] = sizec[2];
  }else{
  size[0] = sizec[0];
  size[1] = sizec[1];
  size[2] = sizec[2];
  size[3] = sizec[3];
  }
#ifndef DUMMY
  int nleft  =  nbytes;
  while (nleft >0) {
    // printf(" debug: send four bytes %d %02x %02x %02x %02x \n",nbytes,size[0]&0xff,size[1]&0xff,size[2]&0xff,size[3]&0xff);
    int ntmp = write(theSocket, size,4); // first 4 bytes for size of things  
#ifdef debugV
     printf("VMEController::writenn() - written for size %d\n", ntmp ); 
#endif
    if(nleft>20000) {
      nwritten = write(theSocket, ptr,20000);
      if(nwritten != 20000) {
        std::cerr<<"VMEController::writenn() - ERROR for write: nwritenn != 20000" << std::endl;
        return(nwritten);  /*error*/
      }
      nleft = nleft - 20000;
      ptr   = ptr + 20000;
    }

    nwritten = write(theSocket, ptr,nleft);

#ifdef debugV
    printf("writenn :  nbytes=%d string=%s \n", nbytes, ptr);
    std::cout << "writen ";
    for(int i = 0; i < nbytes; ++i) {
      std::cout << (int) ptr[i] << " " ;
    }
    std::cout << std::endl;

    printf("writenn : written for nbytes=%d nwritten=%d, left=%d\n",nbytes,nwritten,nleft);
#endif
    if(nwritten <=0)
      return(nwritten);  /*error*/

    nleft -= nwritten;
    ptr   += nwritten;
  }

  return(nbytes - nleft);
#endif
return nbytes;
}


VMEModule* VMEController::getTheCurrentModule(){
 return theCurrentModule;
}

#endif

#ifdef OSUcc
//----------------------------------------------------------------------
// $Id: VMEController.cc,v 2.4 2005/11/30 12:59:59 mey Exp $
// $Log: VMEController.cc,v $
// Revision 2.4  2005/11/30 12:59:59  mey
// DMB firmware loading
//
// Revision 2.3  2005/11/25 23:43:00  mey
// Update
//
// Revision 2.2  2005/11/21 15:48:24  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:24  mey
// Update for new controller
//
// Revision 1.25  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEController.h"
#include "VMEModule.h"
#include "Crate.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write
#include <fcntl.h>

#include <sys/socket.h>
#include <unistd.h>



#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEController::VMEController(int crate, string ipAddr, int port): 
 theSocket(0), ipAddress_(ipAddr), port_(port), theCurrentModule(0),
 indian(SWAP),  max_buff(0), tot_buff(0), crate_(crate),
 plev(1), idevo(0)
{
 
  int socket = openSocket();
  cout << "VMEController opened socket = " << socket << endl;
  cout << "VMEController opened port   = " << port << endl;
}


VMEController::~VMEController(){
  cout << "destructing VMEController .. closing socket " << endl;
  closeSocket();
}


void VMEController::start(VMEModule * module) {
  if(theCurrentModule != module) {
    PRINTSTRING(OVAL: start method defined in VMEController.cc is starting )
    end();
    PRINTSTRING(OVAL: starting current module);
    module->start();
    PRINTSTRING(OVAL: current module was started);
    theCurrentModule = module;
    board=module->boardType();
    vmeadd=(module->slot())<<19;
  }
}



void VMEController::end() {
  if(theCurrentModule != 0) {
    theCurrentModule->end();
    theCurrentModule = 0;
  }
//  assert(plev !=2);
//  idevo = 0;
//  feuseo = 0;
}


void VMEController::send_last() {
}


int VMEController::openSocket() {

  char schardev_name[12]="/dev/schar0";
  schardev_name[11]=0;
  if(port_ >0 || port_ <10)  schardev_name[10] += port_;
  theSocket = open(schardev_name, O_RDWR);
  if (theSocket == -1) {
    perror("open");
    return 1;
  }
  get_macaddr();
  // eth_enableblock();
  eth_reset();
  mrst_ff();
  set_VME_mode();   
  return theSocket;
}


void VMEController::closeSocket() {
#ifndef DUMMY
  close(theSocket);
#endif
  theSocket = 0;
}



void VMEController::goToScanLevel(){
}

void VMEController::release_plev(){
}

VMEModule* VMEController::getTheCurrentModule(){
 return theCurrentModule;
}


#endif
