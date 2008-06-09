#include "VMECC.h"
#include "VMEController.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cmath>
#include <unistd.h>
#include <iomanip>

#include "EMU_CC_constants.h"

using namespace std;

VMECC::VMECC(Crate * theCrate,int slot)
:VMEModule(theCrate,slot)
{
  fplog=NULL;
  VCC_frmw_ver="4.31";
  std::cout << "Instantiate VMECC" << std::endl;
}

VMECC::~VMECC(){
  std::cout << "Killing VMECC" << std::endl;
}

bool VMECC::SelfTest(){return true;};
void VMECC::init(){};
void VMECC::configure(){};


void VMECC::jtag_init()
{
  int i,n;
  int rtn;
  JINSTR_t *jrtn;
  //  for(rtn=JC_Rd_DevID; rtn<=JC_Chk_Conn; rtn++){
  for(rtn=JC_Rd_DevID; rtn<=JC_User; rtn++){
    ld_rtn_base_addr(jtr[rtn].base);
    wbuf[0]=0x00;
    wbuf[1]=Write_Prg_Space;
    nwbuf=2;
    for(i=0;i<jtr[rtn].n;i++){
      jrtn = jtr[rtn].rtn+i;
      wbuf[nwbuf]=((jrtn->instr)>>8)&0xFF;
      wbuf[nwbuf+1]=(jrtn->instr)&0xFF;
      wbuf[nwbuf+2]=((jrtn->bc)>>8)&0xFF;
      wbuf[nwbuf+3]=(jrtn->bc)&0xFF;
      wbuf[nwbuf+4]=((jrtn->data)>>8)&0xFF;
      wbuf[nwbuf+5]=(jrtn->data)&0xFF;
      nwbuf+=6;
    }
    n=eth_write();
  }
}

int VMECC::chk_jtag_conn()
{
  int n,rslt;
  int ptyp;
  //Return value:
  //  1: Successful connection made 
  //  0: No response after sending check connection command 
  // -1: Problem with JTAG connection
  eth_read_timeout(RD_TMO_long);
  wbuf[0]=0x20;
  wbuf[1]=Chk_JTAG_Conn;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      rslt = 0;
    }
    if(AK_STATUS(rbuf) == CC_S){
      rslt = 1;
    }else{
      eth_read_timeout(RD_TMO_short);
      n=eth_read();
      if(n>6){
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
        if(ptyp>=INFO_PKT){
          printf("%s",dcode_msg_pkt(rbuf));
        }
      }
      rslt = -1;
    }
  }else{
    printf("No Response after Chk_JTAG_Conn command\n");
    rslt = 0;
  }
  eth_read_timeout(RD_TMO_short);
  return rslt;
}

unsigned int VMECC::read_dev_id()
{
  int n;
  unsigned int dev_id;
  int ptyp;
  std::cout << "entered read_dev_id"<<std::endl;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Dev_ID;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return 0;
    }
    dev_id = ((rbuf[DATA_OFF+2]&0xff)<<24)|((rbuf[DATA_OFF+3]&0xff)<<16)|((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
    return dev_id;
  } else {
    printf("no response\n");
    return 0;
  }
}

unsigned int VMECC::read_user_code()
{
  int n;
  unsigned int user_code;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Rd_User_Code;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return 0;
    }
    user_code = ((rbuf[DATA_OFF+2]&0xff)<<24)|((rbuf[DATA_OFF+3]&0xff)<<16)|((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
    return user_code;
  } else {
    printf("no response\n");
    return 0;
  }
}

char * VMECC::read_customer_code()
{
  int n;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Cust_Code;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return 0;
    }
/*      for(i=DATA_OFF;i<n;i++){ */
/*        printf("%02X",rbuf[i]&0xFF); */
/*      } */
/*      printf("\n"); */
    return &rbuf[DATA_OFF];
  } else {
    printf("no response\n");
    return 0;
  }
}

char * VMECC::dcode_msg_pkt(char *buf)
{
  //  static char *pkt_typ[] = {"INFO","WARN","ERROR","Not a message type"};
  static char *msg_src[] = {"Misc","VME Ctrl","VME Mstr","VME Rdbk",
			    "VME IH","VME Slave","VME Arb","Ext FIFO",
			    "Eth Rcvr","Eth Trns","JTAG mod","Flash mod",
			    "Config mod","CP mod","Rst Hndlr","StrtupShdwn"};
  static char *msg_typ[] = {"INF","WRN","ERR","NAL"};
  // static char *unknwn = "Unknown message";
  static char *trns_typ[] = {"SNGL", "BLOCK", "RMW", "UNALG"};
  static char *data_sz[]   = {"D08", "D16", "D32", "D64"};
  static char *addr_sz[]   = {"Axx","A16","A24","A32","A40","A64","Axx","Axx"};
  static char *rdwrt[]     = {"Read","Write"};
  static char *dly_type[]  = {"No_Dly","D4nsX16","D16nsX16","D16usX16",
			      "D4nsX32","D16nsX32","D16usX32","undefined"};
  static char *mstr_st[]   = {"M_idle","Capture1","Req_DTB","R1","No_Exe",
			      "Not_Used","Not_Used","Not_Used","Not_Used",
			      "Wait_4_DTB",
                              "Start","Add_Phase","Wait_4_DTACK_Rel","Sngl_Block",
                              "RMW_1","RMW_Read","RMW_2","RMW_Write","M_Read",
                              "M_Write","Cycle_Done","Next_Cycle","Xfer_Done",
                              "Load_Delay","Delay","Ld_Err_Typ","Bus_Error"};
  static char *ih_st[]     = {"IH_idle","IH_Cap_IRQ","IH_Req_DTB","IH_Wait4DTB",
                              "IH_Start","IACK_Cycle","Status_ID","IACK_Done",
                              "IH_Ld_Err","IH_Bus_Err","IH_Rel_BR","Mask_Intr",
                              "IH_Ld_Warn","IH_Warn"};

  static char *acc_typ_pg[] = {"DATA","PRGM"};
  static char *acc_typ_sp[] = {"NPRV","SUPV"};
  static char *acc_typ_lk[] = {"    ","LOCK"};
  static char *acc_typ_cr[] = {"    ","CCSR"};
  static char *acc_typ_ud[] = {"    ","UDAM"};


  static char msg[MAX_MSG_SIZE];
  static char tmp[MAX_MSG_SIZE];

  union hdr_stat  unh;
  union ctrl_stat unc;
  union mstr_stat unm;

  int i;
  int ptyp;
  int ack_stat;
  int pkt_flags;
  int srcid;
  int typid;

  int ih_state,ih_irq,ih_add,curmsk;


  unsigned short int proc_tag, cmnd_ack, seq_id, wrd_cnt;
  unsigned short int code;
  struct ucw *ucptr;

  msg[0]='\0';
  ptyp = buf[PKT_TYP_OFF]&0XFF;
  if(ptyp >= INFO_PKT){

    /* Header info */
    unh.full = buf[HDR_OFF];
    ack_stat = AK_STATUS(buf);
    pkt_flags = PKT_FLAGS(buf);
    proc_tag = PROC_TAG(buf);
    cmnd_ack = buf[CMND_ACK_OFF]&0xFF;
    seq_id   = SEQ_PKT_ID(buf);
    wrd_cnt  = DAT_WRD_CNT(buf);
    /* Message info */
    srcid = SOURCE_ID(buf);
    typid = MSG_TYP_ID(buf);
    code = UNV_CODE_WRD(buf);
    ucptr = (struct ucw *)ptr_bin_srch((int)code, ucwtab, nerrcodes);

    /* Start forming message */
    if(ucptr != NULL){
      sprintf(msg,"%3s: 0x%03X: %-15s: %s\n",ucptr->type,ucptr->code,ucptr->mnem,ucptr->msg);
    /* Check for additional information */
      switch(srcid){
      case VME_CTRL:
	switch(code){
	case VC_MTEr_Fifo:
	case VC_RdEr_Units:
	case VC_RdEr_CtrlWrd:
	  break;
	default:
          strcat(msg,"Additional information:\n");
	  unc.full=SECND_WRD(buf);
	  sprintf(tmp,"\tTransfer Type: %-3s:%-3s:%-5s:%-5s\n",addr_sz[unc.tg.asz],
		  data_sz[unc.tg.dsz],trns_typ[unc.tg.tt],rdwrt[unc.tg.wrt]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\t  Access Type: %4s+%4s+%4s+%4s+%4s\n",acc_typ_sp[unc.fl.sp],
		  acc_typ_pg[unc.fl.pg],acc_typ_lk[unc.fl.lk],
		  acc_typ_cr[unc.fl.cr],acc_typ_ud[unc.fl.ud]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\t   Delay Type: %s\n",dly_type[unc.tg.dt]);
	  strcat(msg,tmp);
	  break;
	}
	break;
      case VME_MSTR:
        strcat(msg,"Additional information:\n");
	unm.full=SECND_WRD(buf);
	sprintf(tmp,"\t    Master State: 0x%02X: %-16s\n",unm.tg.state,mstr_st[unm.tg.state]);
	strcat(msg,tmp);
	sprintf(tmp,"\t   Transfer Type: %-3s:%-5s:%-5s\n",data_sz[unm.tg.dsz],
		trns_typ[unm.tg.tt],rdwrt[unm.tg.wrt]);
	strcat(msg,tmp);
	sprintf(tmp,"\tAddress Modifier: %02X\n",unm.tg.am&0xFF);
	strcat(msg,tmp);
	strcat(msg,"\t  64 bit Address: "); 
	for(i=0;i<8;i++)sprintf(&tmp[2*i],"%02X",buf[MSG_OFF+4+i]&0xFF);
	strcat(msg,tmp);
	strcat(msg,"\n");
	break;
      case VME_IH:
        strcat(msg,"Additional information:\n");
	switch(ptyp){
	case ERR_PKT:
	  ih_state=EXTRACT_IH_STATE(buf);
	  ih_irq=EXTRACT_IH_IRQ(buf);
	  ih_add=EXTRACT_IH_ADD(buf);
	  sprintf(tmp,"\t       IH State: 0x%01X: %-16s\n",ih_state,ih_st[ih_state]);
	  strcat(msg,tmp);
	  sprintf(tmp,"\tIH IRQs pending: ");
	  strcat(msg,tmp);
	  for(i=0;i<7;i++){
	    sprintf(&tmp[i],"%01d",(ih_irq>>(6-i))&1);
	  }
	  strcat(msg,tmp);
	  strcat(msg,"\n");
	  sprintf(tmp,"\tIH IACK Address: %1d\n",ih_add);
	  strcat(msg,tmp);
	  break;
	default:
	  curmsk=EXTRACT_IRQ_MSK(buf);
	  strcat(msg,"\tIH Current IRQ Mask: ");
	  for(i=0;i<7;i++){
	    sprintf(&tmp[i],"%01d",(curmsk>>(6-i))&1);
	  }
	  strcat(msg,tmp);
	  strcat(msg,"\n");
	  break;
	}
	break;
      case ETH_RCV:
        strcat(msg,"Additional information:\n");
	unm.full=SECND_WRD(buf);
	sprintf(tmp,"\t      Command : 0x%02X\n",cmnd_ack&0xFF);
	strcat(msg,tmp);
	sprintf(tmp,"\t  Process Tag : 0x%02X\n",proc_tag&0xFF);
	strcat(msg,tmp);
	sprintf(tmp,"\tSequential ID : 0x%04X\n",seq_id&0xFFFF);
	strcat(msg,tmp);
	sprintf(tmp,"\t   Word Count : 0x%04X (%d)\n",wrd_cnt&0xFFFF,wrd_cnt);
	strcat(msg,tmp);
	break;
      case JTAG_MOD:
        strcat(msg,"Additional information:\n");
	sprintf(tmp,"\t       Status : 0x%04hX\n",SECND_WRD(buf));
	strcat(msg,tmp);
	break;
      default:
/*          strcat(msg,"Additional information:\n"); */
/*  	unm.full=SECND_WRD(buf); */
/*  	sprintf(tmp,"\t      Command : 0x%02X\n",cmnd_ack&0xFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\t  Process Tag : 0x%02X\n",proc_tag&0xFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\tSequential ID : 0x%04X\n",seq_id&0xFFFF); */
/*  	strcat(msg,tmp); */
/*  	sprintf(tmp,"\t   Word Count : 0x%04X (%d)\n",wrd_cnt&0xFFFF,wrd_cnt); */
/*  	strcat(msg,tmp); */
/*          sprintf(tmp,"\t       Source : %01X\n",srcid&0xF); */
/*          strcat(msg,tmp); */
/*          sprintf(tmp,"\t    Msg. Type : %01X\n",typid&0xF); */
/*          strcat(msg,tmp); */
/*          sprintf(tmp,"\t         Code : %03X\n",code&0xFFF); */
/*          strcat(msg,tmp); */
/*          for(i=0;i<DATA_OFF+2*wrd_cnt;i++){ */
/*            printf("%02X",buf[i]&0xFF); */
/*          } */
/*          printf("\n"); */
	break;
      }
    }
    else {
      sprintf(msg,"%3s: 0x%03X: %-15s: Unmatched code 0x%03X is undefined.\n",msg_typ[typid],code,msg_src[srcid],code);
      strcat(msg,"Additional information:\n");
      if((pkt_flags & SPONT)!=0){
        strcat(msg,"\tThis is a spontaneous packet\n");
      }
      else {
        sprintf(tmp,"\t     Status: %-6s: %s\n",ak_status[ack_stat].mnem,ak_status[ack_stat].status);
        strcat(msg,tmp);
        sprintf(tmp,"\tProcess Tag: 0x%02X\n",proc_tag);
        strcat(msg,tmp);
        sprintf(tmp,"\tCommand Ack: 0x%02X\n",cmnd_ack);
        strcat(msg,tmp);
        sprintf(tmp,"\tSeq.Pkt.ID : %d\n",seq_id);
        strcat(msg,tmp);
      }
    }
  }
  else {
    strcat(msg,"Not a message packet type");
  }
  strcat(msg,"\n");
  return msg;
}

void * VMECC::ptr_bin_srch(int code, struct ucw *arr, int n)
{
  struct ucw *low  = &arr[0];
  struct ucw *high = &arr[n];
  struct ucw *mid  = high;

  while((low < high) && ((high-low)/2 != 0)){
    mid = low + (high-low)/2;
    if(code < mid->code){
      high = mid;
    }
    else if(code > mid->code){
      low = mid;
    }
    else {
      return mid;
    }
  }
  return NULL;
}

struct rspn_t VMECC::flush_pkts()
{
  int n;
  union hdr_stat hdr;
  int pkt_type;
  //int srcid;
  int ackn;

  int spont,frag,newp,prio,data;
  // int maxbytes,maxwrds,maxints,maxlongs;
  int numwrds;
  //unsigned char *cp;
  // unsigned short int *hp;
  unsigned short int code;
  // unsigned int *ip,*lph,*lpl;

  unsigned char opcode;
  struct ucw *fopptr;
  struct ucw *pktptr;
  struct rspn_t rsp;

  rsp.npkt=0;
  rsp.nspt=0;
  rsp.nerr=0;
  rsp.nberr=0;
  rsp.nbto=0;
  rsp.nwrn=0;
  rsp.ninf=0;
  rsp.nintr=0;
  rsp.ndat=0;
  rsp.nack_only=0;
  rsp.nbad=0;
  rsp.buf_cnt=0;
  rsp.buf_typ=0;
  rsp.totwrds=0;
  rsp.ackn=0;

  while((n=eth_read())>6){
    rsp.npkt++;
    hdr.full = rbuf[HDR_OFF];
    spont = hdr.tg.spnt;
    frag  = hdr.tg.frag;
    newp   = hdr.tg.newp;
    prio  = hdr.tg.prio;
    ackn  = AK_STATUS(rbuf);
    data  = WITH_DATA(rbuf);
    pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
    opcode = rbuf[CMND_ACK_OFF];
    fopptr = (struct ucw *)ptr_bin_srch((int)opcode,foptab,nopcodes);
    pktptr = (struct ucw *)ptr_bin_srch((int)pkt_type,pkttab,npktcodes);
    numwrds = DAT_WRD_CNT(rbuf);
    if(spont){
      rsp.nspt++;
      if(pkt_type>=INFO_PKT){
        printf("    Message from controller:\n");
        printf("    %s", dcode_msg_pkt(rbuf));
	// if(fplog != NULL){
          printf("    Message from controller:\n");
          printf("    %s", dcode_msg_pkt(rbuf));
	  // }
        code = UNV_CODE_WRD(rbuf);
        switch(pkt_type){
	case INFO_PKT:
	  rsp.ninf++;
	  break;
	case WARN_PKT:
	  rsp.nwrn++;
	  break;
	case ERR_PKT:
 	  rsp.nerr++;
	  if(code == VM_BERR_Slv || code == VI_BERR_Slv){
            rsp.nberr++;
          }
          if(code == VM_BTO || code == VI_BTO){
            rsp.nbto++;
	  }
	  break;
	default:
	  break;
	}
      }
      else if(pkt_type>=IHD08_STAT_ID_PKT && pkt_type<=IHD32_STAT_ID_PKT){
        rsp.nintr++;
        rsp.totwrds += numwrds;
      }
      else {
	printf("Unexpected Spontaneous packet type: %02X\n",pkt_type&0xFF);
      }
    }
    else if(ackn >0 && data ==0){
      rsp.nack_only++;
      rsp.ackn=ackn;
      printf("Acknowledge packet:\n  Status:%-6s:%20s %s\n",ak_status[ackn].mnem,fopptr->mnem,ak_status[ackn].status);
    }
    else if(numwrds > 0){
      rsp.ndat++;
      rsp.totwrds += numwrds;
      printf("Flushing %d of data from a %s packet\n",numwrds,pktptr->mnem);
    }
    else {
      rsp.nbad++;
      printf("Illegal Header encountered\n");
    }
  }
  return rsp;
}

int VMECC::erase_prom()
{
  int n,rslt;
  int ptyp;
  //Return value:
  //  1: Successful completion of erase 
  //  0: No response after sending erase command 
  // -1: Unsuccessful erasure 
  eth_read_timeout(RD_TMO_xlong);
  wbuf[0]=0x20;
  wbuf[1]=Erase_PROM;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      rslt = 0;
    }
    if(AK_STATUS(rbuf) == CC_S){
      rslt = 1;
    }else{
      eth_read_timeout(RD_TMO_short);
      n=eth_read();
      if(n>6){
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
          printf("%s",dcode_msg_pkt(rbuf));
	}
      }
      rslt = -1;
    }
  }else{
    printf("No Response after Erase_PROM command\n");
    rslt = 0;
  }
  eth_read_timeout(RD_TMO_short);
  return rslt;
}
int VMECC::erase_prom_bcast()
{
  int n;

  //Return value:
  //  1: Successful completion of erase 
  //  0: No response after sending erase command 
  // -1: Unsuccessful erasure 
  wbuf[0]=0x00;
  wbuf[1]=Erase_PROM;
  nwbuf=2;
  n=eth_write();
  sleep(15);
  return 1;
}

void VMECC::program_prom_cmd()
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Program_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMECC::program_prom_cmd_no_ack()
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Program_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMECC::reload_fpga()
{
  int n;
  int ptyp;
  wbuf[0]=0x00;
  wbuf[1]=Reload_FPGA;
  nwbuf=2;
  n=eth_write();
  while((n=eth_read())>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
    }
  }
}
void VMECC::verify_prom_cmd()
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Verify_PROM;
  nwbuf=2;
  n=eth_write();
}
void VMECC::ld_rtn_base_addr(unsigned short base)
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Ld_Rtn_Base_Addr;
  wbuf[2]=(base>>8)&0xFF;
  wbuf[3]=base &0xFF;
  nwbuf=4;
  n=eth_write();
}
void VMECC::exec_routine(int rtn)
{
  int n;
  wbuf[0]=0x20;
  wbuf[1]=Exec_Routine;
  wbuf[2]=0x00;
  wbuf[3]=(unsigned char) rtn;
  nwbuf=4;
  n=eth_write();
}
unsigned int VMECC::get_jtag_status()
{
  int n;
  int ptyp;
  unsigned int status;
  wbuf[0]=0x00;
  wbuf[1]=Module_Status;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n>6){
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return 0;
    }
    status = ((rbuf[DATA_OFF]&0xff)<<8)|(rbuf[DATA_OFF+1]&0xff);
    return status;
  } else {
    printf("no response\n");
    return 0;
  }
}
void VMECC::abort_jtag_cmnds()
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Abort_JTAG_Cmnds;
  nwbuf=2;
  n=eth_write();
}

void VMECC::write_jtag_fifo_words(unsigned short *buf, int nw)
{
  int i,n;
  wbuf[0]=0x00;
  wbuf[1]=Write_JTAG_FIFO;
  nwbuf=2;
  for(i=0; i<nw; i++){
    wbuf[nwbuf]=(buf[i]>>8)&0xFF;
    wbuf[nwbuf+1]=(buf[i])&0xFF;
    nwbuf+=2;
  }
  n=eth_write();
}
void VMECC::write_jtag_fifo_bytes(unsigned char *buf, int nb)
{
  int i,n;
  wbuf[0]=0x00;
  wbuf[1]=Write_JTAG_FIFO;
  nwbuf=2;
  for(i=0; i<nb; i++){
    wbuf[nwbuf]=(buf[i])&0xFF;
    nwbuf++;
  }
  n=eth_write();
}
int VMECC::read_prg_space(unsigned short base)
{  
  int i,n,nwrds;
  int offset;
  int ptyp;
  ld_rtn_base_addr(base);
  wbuf[0]=0x00;
  wbuf[1]=Read_Prg_Space;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    return 0;
  }
  nwrds = DAT_WRD_CNT(rbuf);
  printf("Nwords = %d\n",nwrds);
  for(i=0;i<nwrds/3;i++){
    offset = 6*i+DATA_OFF;
    rbk_prg[i].instr = ((rbuf[offset]&0xff)<<8)|(rbuf[offset+1]&0xff);
    rbk_prg[i].bc    = ((rbuf[offset+2]&0xff)<<8)|(rbuf[offset+3]&0xff);
    rbk_prg[i].data  = ((rbuf[offset+4]&0xff)<<8)|(rbuf[offset+5]&0xff);
  }
  return nwrds/3;
}

std::string VMECC::print_routines()
{
  int i,ninstr;
  int rtn;
  char *rtnstr[] = {"Read Device ID", "Read User Code", "Read Customer Code",
                    "Readback PROM", "Erase PROM", "Program PROM", 
                    "Reload FPGA", "Verify PROM", "Check JTAG Connection",
		    "JC_User"};
  char strbuf[256];
  std::string rtns ="";
  for(rtn=JC_Rd_DevID; rtn<=JC_User; rtn++){
    sprintf(strbuf,"\nRoutine: %s\n-----------------------------------------\n",rtnstr[rtn]);
    rtns += strbuf;
    ninstr = read_prg_space(jtr[rtn].base);
    sprintf(strbuf,"ninstr = %d\n",ninstr);
    rtns += strbuf;
    sprintf(strbuf,"Address  Instruction  Bit Count  Data\n");
    rtns += strbuf;
    sprintf(strbuf,"--------------------------------------\n");
    rtns += strbuf;
    for(i=0;i<ninstr;i++){
      sprintf(strbuf,"  %03X :      %02X        %04hX     %04hX\n",jtr[rtn].base+i,
             rbk_prg[i].instr&0x3F,rbk_prg[i].bc,rbk_prg[i].data);
      rtns += strbuf;
    }
  }
  return rtns;
}
void  VMECC::rd_back_prom(const char* fn)
{
  int i,n,wc,naddr,wcntr,prtd,npkt;
  unsigned int faddr,chksum;
  unsigned short addr[2048];
  int ptyp;
  union {
    unsigned char *cp;
    unsigned short int *shtp;
  } flex_buf;
  FILE *fp=fopen(fn,"w");
  naddr = 1024;
  faddr = 0;
  for(i=0;i<naddr;i++){
    addr[2*i] = (1024*i)&0xFFFF;
    addr[2*i+1] = ((1024*i)>>16)&0xFF;
  }
  wbuf[0]=0x00;
  wbuf[1]=Rd_Back_PROM;
  nwbuf=2;
  n=eth_write();
  write_jtag_fifo_words(addr,2*naddr);
  wcntr=0;
  prtd=0;
  npkt=0;
  while((n=eth_read())>6){
    npkt++;
    flex_buf.cp =(unsigned char *) &rbuf[DATA_OFF];
    wc = DAT_WRD_CNT(rbuf);
    //    printf("Packet no. %d; size %d; word count %d\n",npkt,n,wc);
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
    } else {
      for(i=0;i<wc;i++){
	if(faddr%0x10000==0 && !prtd){
	  chksum = ~((6+((faddr>>24)&0xFF)+((faddr>>16)&0xFF))%256)+1;
	  fprintf(fp,":02000004%04X%02X\r\n",(faddr>>16)&0xFFFF,chksum&0xFF);
          prtd=1;
	}
        if(wcntr%8==0){
          chksum = 16+((faddr>>8)&0xFF)+(faddr&0xFF);
          fprintf(fp,":10%04X00",faddr&0xFFFF);
	}
        chksum = chksum+(flex_buf.cp[2*i]&0xFF)+(flex_buf.cp[2*i+1]&0xFF);
        fprintf(fp,"%04hX",flex_buf.shtp[i]);
        wcntr++;
        if(wcntr!=0 && wcntr%8==0){
          fprintf(fp,"%02X\r\n",(~(chksum%256)+1)&0xFF);
          faddr+=16;
          prtd=0;
	}
      }
    }
  }
  fprintf(fp,":00000001FF\r\n");
  fclose(fp);
}

int VMECC::read_mcs(const char *fn)
{
  char line[1024];
  FILE *fp;
  int paddr,lpcnt,parms;
  unsigned int tt;
  unsigned int ddl,ddh;
  unsigned int ll,addr;

  fp = fopen(fn,"r");
  if(fp==NULL){
    printf("Failed to open file %s\n",fn);
    return -1;
  }
  paddr = 0;
  lpcnt = 0;
  tt=0;
  while(tt!=1){
    parms = fscanf(fp,":%2x%4x%2x",&ll,&addr,&tt);
    //    printf("%1x:%02X%04X%02X",parms,ll,addr,tt);
    if(tt==0){
      for(unsigned int ii=0;ii<ll/2;ii++){
        parms = fscanf(fp,"%2x%2x",&ddl,&ddh);
        prm_dat[paddr++] = ((ddh&0xFF)<<8) | (ddl&0xFF);
	//        printf("%5X: %04hX\n",paddr,prm_dat[paddr-1]);
      }
    }
    fgets(line,1024,fp);
    lpcnt++;
    //    printf("%s %d\n",line,lpcnt);
  }
  fclose(fp);
  return lpcnt;
}

void VMECC::send_ver_prom_data()
{
  int i,j,n,nw,pktnum,nsend,tot_bytes;
  int lp,cdwrd,send,pause,abort;
  int addr,pnum,bi;
  int ptyp;
  int ack;
  VERIFY_t ver_dat[147];
  nw = 512;
  send = 1;
  pause = 0;
  abort = 0;
  pktnum = 0;
  lp=0;
  addr = 0;
  nsend = 0;
  pnum =-1;
  for(i=0;i<1024;i++){
    if(i!=0 && i%7==0){
      ver_dat[pnum].nw = bi;
    }
    if(i%7==0){
      ver_dat[++pnum].buf = (unsigned short int *)malloc(2*7*(nw+2));
      bi = 0;
    }
    ver_dat[pnum].buf[bi++] = addr&0xFFFF;
    ver_dat[pnum].buf[bi++] = (addr>>16)&0xFF;
    for(j=0;j<nw;j++){
      ver_dat[pnum].buf[bi++] = prm_dat[nw*i+j];
    }
    addr+=1024;
  }
  ver_dat[pnum].nw = bi;
  tot_bytes = 0;
  while(lp<pnum+1 && !abort){
    if(send){
      nsend++;
      write_jtag_fifo_words(ver_dat[lp].buf, ver_dat[lp].nw);
      tot_bytes+=2*ver_dat[lp++].nw;
      std::cout << nsend << ": have sent " << tot_bytes << " bytes" << std::endl;
      if(nsend%4 == 0){
        pause = 1;
        send  = 0;
        abort = 0;
      }
    }
    if(pause){
      while((n=eth_read())>6){
	pktnum++;
	std::cout << endl;
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
	  std::cout << "pktnum A" << pktnum << ": " << dcode_msg_pkt(rbuf) << std::endl;
	  cdwrd = UNV_CODE_WRD(rbuf);
	  if(cdwrd==JT_Buf_AF){
	    pause = 1;
	    send = 0;
	    abort = 0;
	  } else if(cdwrd==JT_Buf_AMT){
	    pause = 0;
	    send = 1;
	    abort = 0;
	  } else if((cdwrd==JT_Buf_Ovfl) || (cdwrd==JT_Ver_Fail) || (cdwrd==JT_Buf_RdErr)){
	    pause = 0;
	    send = 0;
	    abort = 1;
	  }
	} else {
	  std::cout << "pktnum A" << pktnum << ": ";
	  ack = AK_STATUS(rbuf); 
	  switch(ack){
	  case NO_ACK:
	    std::cout << "Non message or no acknowledge packet received" << std::endl;
	    break;
	  case CC_S:
	    std::cout << "Verify completed successfully" << std::endl;
	    break;
	  case CC_W: case CC_E: case CE_I: case CIP_W: case CIP_E:
	    std::cout << "\aAn error or warning occured verify!" << std::endl;
	    break;
	  default:
	    std::cout << "Unknown packet returned" << std::endl;
	    break;
	  }
	}
      }
    }
  }
  if(abort){
    abort_jtag_cmnds();
  }
  while((n=eth_read())>6){
    pktnum++;
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      std::cout << "pktnum B" << pktnum << ": " << dcode_msg_pkt(rbuf) << std::endl;
    } else {
      std::cout << "pktnum B" << pktnum << ": ";

      ack = AK_STATUS(rbuf); 
      switch(ack){
      case NO_ACK:
	std::cout << "Non message or no acknowledge packet received" << std::endl;
	break;
      case CC_S:
	std::cout << "Verify completed successfully" << std::endl;
	break;
      case CC_W:
	std::cout << "Verify completed with a warning" << std::endl;
	break;
      case CC_E:
	std::cout << "Verify completed with an error" << std::endl;
	break;
      case CE_I:
	std::cout << "Verify execution finished incomplete" << std::endl;
	break;
      case CIP_W: case CIP_E:
	std::cout << "Command in progress with an error or warning" << std::endl;
	break;
      default:
	std::cout << "Unknown packet returned" << std::endl;
	break;
      }
    }
  }
  for(i=0;i<pnum+1;i++){
    free(ver_dat[i].buf);
  }
}

void VMECC::send_prg_prom_data()
{
  int n,nw,pktnum,nsend,tot_bytes;
  int cdwrd,send,pause,abort;
  int ptyp;
  int ack;
  int npus_max = 32768;
  int npus_per_pkt_max = 249;
  int npus,tot_npus;
  send = 1;
  pause = 0;
  abort = 0;
  pktnum = 0;
  nsend = 0;
  tot_npus = 0;
  tot_bytes = 0;
  while(tot_npus<npus_max && !abort){
    if(send){
      nsend++;
      if((npus_max-tot_npus)< npus_per_pkt_max){
        npus = npus_max - tot_npus;
      }else{
        npus = npus_per_pkt_max;
      }
      nw=16*npus;
      write_jtag_fifo_words(&prm_dat[16*tot_npus], nw);
      tot_npus+=npus;
      tot_bytes+=2*nw;
      std::cout << nsend << ": have sent " << tot_npus << " pu's and " << tot_bytes << " bytes" << std::endl;
      if(nsend%4 == 0){
        pause = 1;
        send  = 0;
        abort = 0;
      }
    }
    if(pause){
      while((n=eth_read())>6){
	pktnum++;
	std::cout << endl;
	ptyp = rbuf[PKT_TYP_OFF]&0xff;
	if(ptyp>=INFO_PKT){
	  std::cout << "pktnum A" << pktnum << ": " << dcode_msg_pkt(rbuf) << std::endl;
	  cdwrd = UNV_CODE_WRD(rbuf);
	  if(cdwrd==JT_Buf_AF){
	    pause = 1;
	    send = 0;
	    abort = 0;
	  } else if(cdwrd==JT_Buf_AMT){
	    pause = 0;
	    send = 1;
	    abort = 0;
	  } else if((cdwrd==JT_Buf_Ovfl) || (cdwrd==JT_Ver_Fail) || (cdwrd==JT_Buf_RdErr)){
	    pause = 0;
	    send = 0;
	    abort = 1;
	  }
	} else {
	  std::cout << "pktnum A" << pktnum << ": ";
	  ack = AK_STATUS(rbuf); 
	  switch(ack){
	  case NO_ACK:
	    std::cout << "Non message or no acknowledge packet received" << std::endl;
	    break;
	  case CC_S:
	    std::cout << "Programming completed successfully" << std::endl;
	    break;
	  case CC_W: case CC_E: case CE_I: case CIP_W: case CIP_E:
	    std::cout << "\aAn error or warning occured during programming" << std::endl;
	    break;
	  default:
	    std::cout << "Unknown packet returned" << std::endl;
	    break;
	  }
	}
      }
    }
  }
  if(abort){
    abort_jtag_cmnds();
  }
}
void VMECC::send_prg_prom_data_bcast()
{
  int nw,nsend,tot_bytes;
  int npus_max = 32768;
  int npus_per_pkt_max = 249;
  int npus,tot_npus;
  nsend = 0;
  tot_npus = 0;
  tot_bytes = 0;
  while(tot_npus<npus_max){
    nsend++;
    if((npus_max-tot_npus)< npus_per_pkt_max){
      npus = npus_max - tot_npus;
    }else{
      npus = npus_per_pkt_max;
    }
    nw=16*npus;
    write_jtag_fifo_words(&prm_dat[16*tot_npus], nw);
    tot_npus+=npus;
    tot_bytes+=2*nw;
    std::cout << nsend << ": have sent " << tot_npus << " pu's and " << tot_bytes << " bytes" << std::endl;
    if(nsend%4 == 0){
      usleep(500000);
    }
  }
}
void VMECC::send_uc_cc_data(const char *fn)
{
  int i;
  unsigned int uc,proj,maj_ver,min_ver;
  char cc[33];
  for(i=0;i<33;i++){
    cc[i] = 0;
  }
  strncpy(cc,fn,32);
  sscanf(fn,"%5X.V%1X.%2X.mcs",&proj,&maj_ver,&min_ver);
  printf("proj: %05X\n",proj);
  printf("majv: %01X\n",maj_ver);
  printf("minv: %02X\n",min_ver);
  uc = (proj<<12)|(maj_ver<<8)|(min_ver);
  printf("uc  : %08X\n",uc);
  printf("cc  : %s\n\n",cc);

  write_jtag_fifo_words((unsigned short *)&uc, 2);
/*    printf("user code packet:\n"); */
/*    for(i=0;i<6;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */

  write_jtag_fifo_words((unsigned short *)&uc, 2);
/*    printf("user code verify packet:\n"); */
/*    for(i=0;i<6;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */

  write_jtag_fifo_bytes((unsigned char *)cc, 32);
/*    printf("customer code packet:\n"); */
/*    for(i=0;i<34;i++)printf("%02X",wbuf[i]&0xFF); */
/*    printf("\n"); */
}
    
void VMECC::prg_vcc_prom_ver(const char *path,const char *ver)
{
  char fname[36];
  char fullname[256];
  int n,pktnum,rslt;
  unsigned int temp_uint;
  static unsigned int device_id = 0xF5057093;
  char *tmp_cp;
  int ptyp;
  int ack;
  char buf[100];
  strcpy(fname,"D783C.V");
  strcat(fname,ver);
  strcat(fname,".mcs");
  strcpy(fullname,path);
  strcat(fullname,"/");
  strcat(fullname,fname);
  sprintf(buf," %s \n",fullname);
  std::cout << "VMECC PromLoad Filename: "<< buf << "\n" <<std::endl;
  rslt = chk_jtag_conn();
  if(rslt == 1){
    temp_uint = read_dev_id();
    if(temp_uint == device_id){
      temp_uint = read_user_code();
      printf("User Code is %08X\n",temp_uint);
      tmp_cp = read_customer_code();
      if(tmp_cp!=0){
	std::cout << "PROM currently programmed with " << tmp_cp << std::endl;
      }
      else {
	std::cout << "Did not receive customer code. Proceeding anyway" << std::endl;
      }
      std::cout << "Erasing PROM..." << std::endl;
      rslt = erase_prom();
      rslt = 1;
      if(rslt == 1){
	std::cout << "PROM successfully erased." << std::endl;
	pktnum = 0;
	std::cout << "Reading PROM data file..." << std::endl;
	if(read_mcs(fullname)>0){
	  std::cout << "Programming..." << std::endl;
          CNFG_ptr cp=read_crs();
          print_crs(cp);
          if((cp->rst_misc & RST_CR_MSGLVL != RST_CR_MSGLVL) || (cp->ether & ETH_CR_SPONT != ETH_CR_SPONT)){
	    std::cout << "Messages were not enabled; Will enable them now" << std::endl;
            set_clr_bits(SET, RST_MISC, RST_CR_MSGLVL);
            set_clr_bits(SET, ETHER, ETH_CR_SPONT);
	  }
          free(cp);
          cp=read_crs();
          print_crs(cp);
          free(cp);
	  program_prom_cmd();
	  send_prg_prom_data();
	  send_uc_cc_data(fname);
	  while((n=eth_read())>6){
	    pktnum++;
	    ptyp = rbuf[PKT_TYP_OFF]&0xff;
	    if(ptyp>=INFO_PKT){
	      std::cout << "pktnum B" << pktnum << ": " << dcode_msg_pkt(rbuf) << std::endl;
	    } else {
	      std::cout << "pktnum B" << pktnum << ": ";
	      ack = AK_STATUS(rbuf); 
	      switch(ack){
	      case NO_ACK:
		std::cout << "Non message or no acknowledge packet received" << std::endl;
		break;
	      case CC_S:
		std::cout << "Programming completed successfully" << std::endl;
		break;
	      case CC_W:
		std::cout << "Programming completed with a warning" << std::endl;
		abort_jtag_cmnds();
		break;
	      case CC_E:
		std::cout << "Programming completed with an error" << std::endl;
		abort_jtag_cmnds();
		break;
	      case CE_I:
		std::cout << "Command execution finished incomplete" << std::endl;
		abort_jtag_cmnds();
		break;
	      case CIP_W: case CIP_E:
		std::cout << "Command in progress with an error or warning" << std::endl;
		break;
	      default:
		std::cout << "Unknown packet returned" << std::endl;
		break;
	      }
	    }
	  }
	  std::cout << "Verifying..." << std::endl;
	  verify_prom_cmd();
	  send_ver_prom_data();
	  std::cout << "Reprogramming FPGA..." << std::endl;
	  reload_fpga();
        }
      }
      else{
	std::cout << "\a\aPROM is not erased!" << std::endl;
      }
    }
    else {
      std::cout << "\a\aWrong device ID -- aborting" << std::endl;
    }
  }
  else{
    std::cout << "\a\aJTAG connection is bad! Can not proceed." << std::endl;
  }
}

void VMECC::prg_vcc_prom_bcast(const char *path, const char *ver)
{
  char fname[36];
  char fullname[256];
  int rslt;

  strcpy(fname,"D783C.V");
  strcat(fname,ver);
  strcat(fname,".mcs");
  strcpy(fullname,path);
  strcat(fullname,"/");
  strcat(fullname,fname);

  std::cout << "Disabling messages..." << std::endl;
  set_clr_bits(CLR,RST_MISC,~RST_CR_MSGLVL);

  std::cout << "Erasing PROM..." << std::endl;
  rslt = erase_prom_bcast();
  std::cout << "Reading PROM data file..." << std::endl;
  if(read_mcs(fullname)>0){
    std::cout << "Programming..." << std::endl;
    program_prom_cmd_no_ack();
    send_prg_prom_data_bcast();
    send_uc_cc_data(fname);
    std::cout << "Reprogramming FPGA..." << std::endl;
    reload_fpga();
    std::cout << "Done" << std::endl;
  }
  else {
    std::cout << "PROM is erased but programming failed!" << std::endl;
  }
}

int VMECC::read_dev_id_broadcast(char * crates_info)
{
  int n=0, cs=0;
  int ptyp;
  std::cout<<"Enter: read_dev_id_broadcast() \n"<<std::endl;
  wbuf[0]=0x00;
  wbuf[1]=Rd_Dev_ID;
  nwbuf=2;
  n=eth_write();
  eth_read_timeout(RD_TMO_short);
  while(n>8){
    n=eth_read();

    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return cs;
    }
    if(n>8){
      memcpy(crates_info+cs*10, rbuf+MAC_SRC_OFF, 6);
      crates_info[cs*10+6]=rbuf[DATA_OFF+1];
      crates_info[cs*10+7]=rbuf[DATA_OFF];
      crates_info[cs*10+8]=rbuf[DATA_OFF+3];
      crates_info[cs*10+9]=rbuf[DATA_OFF+2];
      cs++;
    }
  }
  eth_read_timeout(RD_TMO_short);
  return cs;
}

int VMECC::vme_read_broadcast(char *dmbs_info)
{  
  int n;
  int ndmbs=0;
  int ptyp;
  eth_read_previous();
  if(nrbuf>8){
      memcpy(dmbs_info+ndmbs*10, rbuf+MAC_SRC_OFF, 6);
      dmbs_info[ndmbs*10+6]=rbuf[DATA_OFF+1];
      dmbs_info[ndmbs*10+7]=rbuf[DATA_OFF];
      dmbs_info[ndmbs*10+8]=rbuf[DATA_OFF+3];
      dmbs_info[ndmbs*10+9]=rbuf[DATA_OFF+2];
      ndmbs++;
  }else{return 0;}
  eth_read_timeout(RD_TMO_short);
  usleep(1000);
  int nleft=LeftToRead();
  printf(" LeftToRead %d \n",nleft );
  for(int k=0;k<nleft;k++){
    n=eth_read();
    
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      printf("%s",dcode_msg_pkt(rbuf));
      return ndmbs;
    }
    printf(" n from vme_read_broadcast %d \n",n);
    if(n>8){
      memcpy(dmbs_info+ndmbs*10, rbuf+MAC_SRC_OFF, 6);
      dmbs_info[ndmbs*10+6]=rbuf[DATA_OFF+1];
      dmbs_info[ndmbs*10+7]=rbuf[DATA_OFF];
      dmbs_info[ndmbs*10+8]=rbuf[DATA_OFF+3];
      dmbs_info[ndmbs*10+9]=rbuf[DATA_OFF+2];
      ndmbs++;
    }
  }
  eth_read_timeout(RD_TMO_short);
  printf(" exit vme_broadcast_read %d \n",ndmbs);
  return ndmbs;
}


int VMECC::dcs_read_broadcast(char *dcs_info)
{ 
  int nt=eth_read_previous();
  printf(" dcs_read_broadcast: nt %d nrbuf %d \n",nt,nrbuf); 
  if(nrbuf>8){
      memcpy(dcs_info, rbuf+MAC_SRC_OFF, 6);
      for(int i=0;i<nrbuf-DATA_OFF;i++)dcs_info[i+6]=rbuf[DATA_OFF+i];
  }else{return 0;}
  return 80;
}

float VMECC::dcs_convert(int n,char *buf)
{
static int order[38][2]={{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}, 
  {2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7},
  {3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},
  {4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7},
  {5,0},{5,1},{5,2},{5,3},{5,4},{5,5}};
static int conv_lowv[5][8]={
  {2,2,1,1,2,2,1,2},
  {2,1,2,2,1,2,2,1},
  {2,2,1,2,2,2,2,2},
  {2,2,2,2,2,2,2,2},
  {2,2,2,2,2,2,2,2}
}; 
 int ichp=order[n][0];
 int ichn=order[n][1];
   unsigned int ival=((buf[6+2*n]<<8)&0xff00)|(buf[7+2*n]&0xff);
  ival=ival*conv_lowv[ichp-1][ichn];
  float cval=ival*5.0/4.096;
  return cval;
}

CNFG_ptr VMECC::load_cnfg(char *fn)
{
  int i;
  char line[CNFG_MAXLINE],key_wrd[24];
static  char macfmt[] = "%2hx-%2hx-%2hx-%2hx-%2hx-%2hx";
  unsigned short tmp[6];
  CNFG_ptr cp;
  FILE *fp;
  if((fp=fopen(fn,"r"))==0){
    return((CNFG_ptr)(-1));
  }
  cp=(CNFG_ptr)malloc(sizeof(CNFG_t));
  while(fscanf(fp,"%s",key_wrd)>0){
    if(key_wrd[0]!='#'&&(strncmp(key_wrd,"//",2)!=0)){
      if(strcmp(key_wrd,"Device:")==0){
        fscanf(fp,macfmt,tmp,tmp+1,tmp+2,tmp+3,tmp+4,tmp+5);
        for(i=0;i<6;i++) cp->mac.device[i] = (unsigned char)tmp[i];
      }
      else if(strcmp(key_wrd,"Mcast1:")==0){
        fscanf(fp,macfmt,tmp,tmp+1,tmp+2,tmp+3,tmp+4,tmp+5);
        for(i=0;i<6;i++) cp->mac.mcast1[i] = (unsigned char)tmp[i];
      }
      else if(strcmp(key_wrd,"Mcast2:")==0){
        fscanf(fp,macfmt,tmp,tmp+1,tmp+2,tmp+3,tmp+4,tmp+5);
        for(i=0;i<6;i++) cp->mac.mcast2[i] = (unsigned char)tmp[i];
      }
      else if(strcmp(key_wrd,"Mcast3:")==0){
        fscanf(fp,macfmt,tmp,tmp+1,tmp+2,tmp+3,tmp+4,tmp+5);
        for(i=0;i<6;i++) cp->mac.mcast3[i] = (unsigned char)tmp[i];
      }
      else if(strcmp(key_wrd,"Dflt_Srv:")==0){
        fscanf(fp,macfmt,tmp,tmp+1,tmp+2,tmp+3,tmp+4,tmp+5);
        for(i=0;i<6;i++) cp->mac.dflt_srv[i] = (unsigned char)tmp[i];
      }
      else if(strcmp(key_wrd,"Ether:")==0){
        fscanf(fp,"%hX",&cp->ether);
      }
      else if(strcmp(key_wrd,"Ext_FIFO:")==0){
        fscanf(fp,"%hX",&cp->ext_fifo);
      }
      else if(strcmp(key_wrd,"Rst_Misc:")==0){
        fscanf(fp,"%hX",&cp->rst_misc);
      }
      else if(strcmp(key_wrd,"VME:")==0){
        fscanf(fp,"%X",&cp->vme);
      }
      else if(strcmp(key_wrd,"VME_BTO:")==0){
        fscanf(fp,"%hX",&cp->vme_bto);
      }
      else if(strcmp(key_wrd,"VME_BGTO:")==0){
        fscanf(fp,"%hX",&cp->vme_bgto);
      }
      else {
        printf("\nUnrecognized Keyword: %s\n",key_wrd);
        fgets(line,CNFG_MAXLINE,fp);
      }
    }
    else {
      fgets(line,CNFG_MAXLINE,fp);
    }
  }
  fclose(fp);
  return(cp);
}

int VMECC::write_cnfg(CNFG_ptr cp, char *fn)
{
  int i;
  FILE *fp;
  if((fp=fopen(fn,"w"))==0){
    printf("Error could not open %s\n",fn);
    return(-1);
  }
  fprintf(fp,"Device:   ");
  for(i=0;i<5;i++){
    fprintf(fp,"%02X-",cp->mac.device[i]);
  }
  fprintf(fp,"%02X\n",cp->mac.device[5]);
  fprintf(fp,"Mcast1:   ");
  for(i=0;i<5;i++){
    fprintf(fp,"%02X-",cp->mac.mcast1[i]);
  }
  fprintf(fp,"%02X\n",cp->mac.mcast1[5]);
  fprintf(fp,"Mcast2:   ");
  for(i=0;i<5;i++){
    fprintf(fp,"%02X-",cp->mac.mcast2[i]);
  }
  fprintf(fp,"%02X\n",cp->mac.mcast2[5]);
  fprintf(fp,"Mcast3:   ");
  for(i=0;i<5;i++){
    fprintf(fp,"%02X-",cp->mac.mcast3[i]);
  }
  fprintf(fp,"%02X\n",cp->mac.mcast3[5]);
  fprintf(fp,"Dflt_srv: ");
  for(i=0;i<5;i++){
    fprintf(fp,"%02X-",cp->mac.dflt_srv[i]);
  }
  fprintf(fp,"%02X\n",cp->mac.dflt_srv[5]);

  fprintf(fp,"Ether:        %04hX\n",cp->ether);
  fprintf(fp,"Ext_fifo:     %04hX\n",cp->ext_fifo);
  fprintf(fp,"Rst_Misc:     %04hX\n",cp->rst_misc);
  fprintf(fp,"VME:      %08X\n",cp->vme);
  fprintf(fp,"VME_BTO:      %04hX\n",cp->vme_bto);
  fprintf(fp,"VME_BGTO:     %04hX\n",cp->vme_bgto);
  fclose(fp);
  return(0);
}

int VMECC::set_cnfg_dflt(int dflt)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_Cnfg_Dflt;
  wbuf[2]=0x00;
  wbuf[3]=(dflt&0x1f);
  nwbuf=4;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMECC::read_cnfg_dflt()
{
  int n;
  int ptyp;
  wbuf[0]=0x20;
  wbuf[1]=Read_Cnfg_Dflt;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF)));
  }
  return(rbuf[23]&0xFF);
}

int VMECC::save_cnfg_num(int cnum)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Save_Cnfg_Num;
  wbuf[2]=0x00;
  wbuf[3]=(cnum&0x1f);  // cnum must be <= 20
  nwbuf=4;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMECC::rstr_cnfg_num(int cnum)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Rstr_Cnfg_Num;
  wbuf[2]=0x00;
  wbuf[3]=(cnum&0x1f);  // cnum must be <= 20
  nwbuf=4;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMECC::read_cnfg_num_dir(int cnum, char **dpp)
{
  int n,numwrds;
  int ptyp;
  wbuf[0]=0x20;
  wbuf[1]=Read_Cnfg_Num_Dir;
  wbuf[2]=0x00;
  wbuf[3]=(cnum&0x1f);  // cnum must be <= 20
  nwbuf=4;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF)));
  }
  numwrds = ((rbuf[20]&0xff)<<8)|(rbuf[21]&0xff);
  *dpp = &rbuf[22];
  return(numwrds);
}

CNFG_ptr VMECC::read_cnfg_num_dcd(int cnum)
{
  int n;
  int ptyp;
  CNFG_ptr cp;
  wbuf[0]=0x00;
  wbuf[1]=Read_Cnfg_Num_Dcd;
  wbuf[2]=0x00;
  wbuf[3]=(cnum&0x1f);  // cnum must be <= 20
  nwbuf=4;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return((CNFG_ptr)(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF))));
  }
  cp=(CNFG_ptr)malloc(sizeof(CNFG_t));
  cp->ether = ((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF);
  cp->ext_fifo = ((rbuf[24]&0xFF)<<8)|(rbuf[25]&0xFF);
  cp->rst_misc = ((rbuf[26]&0xFF)<<8)|(rbuf[27]&0xFF);
  cp->vme = ((rbuf[28]&0xFF)<<24)|((rbuf[29]&0xFF)<<16)|((rbuf[30]&0xFF)<<8)|(rbuf[31]&0xFF);
  cp->vme_bto = ((rbuf[32]&0xFF)<<8)|(rbuf[33]&0xFF);
  cp->vme_bgto = ((rbuf[34]&0xFF)<<8)|(rbuf[35]&0xFF);
  return(cp);
}

CNFG_ptr VMECC::read_crs()
{
  int n;
  int ptyp;
  CNFG_ptr cp;
  wbuf[0]=0x20;
  wbuf[1]=Read_CRs;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return((CNFG_ptr)(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF))));
  }
  cp=(CNFG_ptr)malloc(sizeof(CNFG_t));
  cp->ether = ((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF);
  cp->ext_fifo = ((rbuf[24]&0xFF)<<8)|(rbuf[25]&0xFF);
  cp->rst_misc = ((rbuf[26]&0xFF)<<8)|(rbuf[27]&0xFF);
  cp->vme = ((rbuf[28]&0xFF)<<24)|((rbuf[29]&0xFF)<<16)|((rbuf[30]&0xFF)<<8)|(rbuf[31]&0xFF);
  cp->vme_bto = ((rbuf[32]&0xFF)<<8)|(rbuf[33]&0xFF);
  cp->vme_bgto = ((rbuf[34]&0xFF)<<8)|(rbuf[35]&0xFF);
  return(cp);
}

int VMECC::set_macs(int mcid, CNFG_ptr cp)
{
  int i,n,l,lcnt,err;
  err=0;
  wbuf[0]=0x00;
  wbuf[1]=Set_MACs;
  wbuf[2]=0x00;
  wbuf[3]=mcid;
  nwbuf=4;
  switch(mcid){
    case DEVICE:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.device[i];
      nwbuf+=6;
      break;
    case MCAST1:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.mcast1[i];
      nwbuf+=6;
      break;
    case MCAST2:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.mcast2[i];
      nwbuf+=6;
      break;
    case MCAST3:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.mcast3[i];
      nwbuf+=6;
      break;
    case DFLT_SRV:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.dflt_srv[i];
      nwbuf+=6;
      break;
    case ALL_MACS:
      for(i=0;i<6;i++)wbuf[4+i]=cp->mac.device[i];
      nwbuf+=6;
      for(i=0;i<6;i++)wbuf[10+i]=cp->mac.mcast1[i];
      nwbuf+=6;
      for(i=0;i<6;i++)wbuf[16+i]=cp->mac.mcast2[i];
      nwbuf+=6;
      for(i=0;i<6;i++)wbuf[22+i]=cp->mac.mcast3[i];
      nwbuf+=6;
      for(i=0;i<6;i++)wbuf[28+i]=cp->mac.dflt_srv[i];
      nwbuf+=6;
      break;
    default:
      printf("illeagal MAC ID\n");
      err=-1;
      break;
  }
  if(err!=0){
    return(err);
  }
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMECC::read_macs_dir(char **dpp)
{
  int n,numwrds;
  int ptyp;
  wbuf[0]=0x20;
  wbuf[1]=Read_MACs_Dir;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF)));
  }
  numwrds = ((rbuf[20]&0xff)<<8)|(rbuf[21]&0xff);
  *dpp = &rbuf[22];
  return(numwrds);
}

CNFG_ptr VMECC::read_macs_dcd()
{
  int i,n;
  int ptyp;
  CNFG_ptr cp;
  wbuf[0]=0x20;
  wbuf[1]=Read_MACs_Dcd;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  ptyp = rbuf[PKT_TYP_OFF]&0xff;
  if(ptyp>=INFO_PKT){
    dcode_msg_pkt(rbuf);
    return((CNFG_ptr)(-(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF))));
  }
  cp=(CNFG_ptr)malloc(sizeof(CNFG_t));
  for(i=0;i<6;i++)cp->mac.device[i] = rbuf[22+i];
  for(i=0;i<6;i++)cp->mac.mcast1[i] = rbuf[28+i];
  for(i=0;i<6;i++)cp->mac.mcast2[i] = rbuf[34+i];
  for(i=0;i<6;i++)cp->mac.mcast3[i] = rbuf[40+i];
  for(i=0;i<6;i++)cp->mac.dflt_srv[i] = rbuf[46+i];
  return(cp);
}

int VMECC::wrt_crs(int crid, CNFG_ptr cp)
{
  int n,l,lcnt,err;
  err=0;
  wbuf[0]=0x00;
  wbuf[1]=Wrt_CR_ID;
  wbuf[2]=0x00;
  wbuf[3]=crid;
  nwbuf=4;
  switch(crid){
    case ETHER:
      wbuf[4]=((cp->ether)>>8)&0xFF;
      wbuf[5]=(cp->ether)&0xFF;
      nwbuf+=2;
      break;
    case EXTFIFO:
      wbuf[4]=((cp->ext_fifo)>>8)&0xFF;
      wbuf[5]=(cp->ext_fifo)&0xFF;
      nwbuf+=2;
      break;
    case RST_MISC:
      wbuf[4]=((cp->rst_misc)>>8)&0xFF;
      wbuf[5]=(cp->rst_misc)&0xFF;
      nwbuf+=2;
      break;
    case VME:
      wbuf[4]=((cp->vme)>>24)&0xFF;
      wbuf[5]=((cp->vme)>>16)&0xFF;
      wbuf[6]=((cp->vme)>>8)&0xFF;
      wbuf[7]=(cp->vme)&0xFF;
      nwbuf+=4;
      break;
    case BTO:
      wbuf[4]=((cp->vme_bto)>>8)&0xFF;
      wbuf[5]=(cp->vme_bto)&0xFF;
      nwbuf+=2;
      break;
    case BGTO:
      wbuf[4]=((cp->vme_bgto)>>8)&0xFF;
      wbuf[5]=(cp->vme_bgto)&0xFF;
      nwbuf+=2;
      break;
    case ALL_CRS:
      wbuf[4]=((cp->ether)>>8)&0xFF;
      wbuf[5]=(cp->ether)&0xFF;
      wbuf[6]=((cp->ext_fifo)>>8)&0xFF;
      wbuf[7]=(cp->ext_fifo)&0xFF;
      wbuf[8]=((cp->rst_misc)>>8)&0xFF;
      wbuf[9]=(cp->rst_misc)&0xFF;
      wbuf[10]=((cp->vme)>>24)&0xFF;
      wbuf[11]=((cp->vme)>>16)&0xFF;
      wbuf[12]=((cp->vme)>>8)&0xFF;
      wbuf[13]=(cp->vme)&0xFF;
      wbuf[14]=((cp->vme_bto)>>8)&0xFF;
      wbuf[15]=(cp->vme_bto)&0xFF;
      wbuf[16]=((cp->vme_bgto)>>8)&0xFF;
      wbuf[17]=(cp->vme_bgto)&0xFF;
      nwbuf+=14;
      break;
    default:
      printf("illeagal CR ID\n");
      err=-1;
      break;
  }
  if(err!=0){
    return(err);
  }
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

int VMECC::set_clr_bits(int sc, int crid, unsigned int mask)
{
  int n,l,lcnt,err;
  err=0;
  wbuf[0]=0x00;
  wbuf[1]=Set_Clr_CRs;
  wbuf[2]=sc;
  wbuf[3]=crid;
  nwbuf=4;
  switch(crid){
    case ETHER: case EXTFIFO: case RST_MISC: case BTO: case BGTO:
      wbuf[4]=(mask>>8)&0xFF;
      wbuf[5]=mask&0xFF;
      nwbuf+=2;
      break;
    case VME:
      wbuf[4]=(mask>>24)&0xFF;
      wbuf[5]=(mask>>16)&0xFF;
      wbuf[6]=(mask>>8)&0xFF;
      wbuf[7]=(mask)&0xFF;
      nwbuf+=4;
      break;
    default:
      printf("illeagal CR ID\n");
      err=-1;
      break;
  }
  if(err!=0){
    return(err);
  }
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}


int VMECC::wrt_ser_num(int ser_num)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Wrt_Ser_Num;
  wbuf[2]=0x00;
  wbuf[3]=(ser_num>>16)&0xFF;
  wbuf[4]=(ser_num>>8)&0xFF;
  wbuf[5]=ser_num&0xFF;
  nwbuf=6;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return(n);
}

SN_t VMECC::rd_ser_num()
{
  int n;
  int ptyp;
  SN_t sn;
  sn.status=0;
  sn.err_typ=0;
  sn.sn=0;
  wbuf[0]=0x20;
  wbuf[1]=Rd_Ser_Num;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  if(n<=6){
    sn.status=-1;
  }
  else {
    ptyp = rbuf[PKT_TYP_OFF]&0xff;
    if(ptyp>=INFO_PKT){
      dcode_msg_pkt(rbuf);
      sn.status=-2;
      sn.err_typ=(((rbuf[22]&0xFF)<<8)|(rbuf[23]&0xFF));
    }
    else{
      sn.sn = (int)((signed char)rbuf[23]); // sign extension
      sn.sn = (sn.sn&0xFF000000) | ((rbuf[23]&0xFF)<<16)|((rbuf[24]&0xFF)<<8)|(rbuf[25]&0xFF);
    }
  }
  return sn;
}

void VMECC::cp_macs(CNFG_ptr to, CNFG_ptr from)
{
  int i;
  for(i=0;i<6;i++)to->mac.device[i]=from->mac.device[i];
  for(i=0;i<6;i++)to->mac.mcast1[i]=from->mac.mcast1[i];
  for(i=0;i<6;i++)to->mac.mcast2[i]=from->mac.mcast2[i];
  for(i=0;i<6;i++)to->mac.mcast3[i]=from->mac.mcast3[i];
  for(i=0;i<6;i++)to->mac.dflt_srv[i]=from->mac.dflt_srv[i];
  return;
}

void VMECC::cp_crs(CNFG_ptr to, CNFG_ptr from)
{
  to->ether    = from->ether;
  to->ext_fifo = from->ext_fifo;
  to->rst_misc = from->rst_misc;
  to->vme      = from->vme;
  to->vme_bto  = from->vme_bto;
  to->vme_bgto = from->vme_bgto;
  return;
}

void VMECC::cp_cnfg(CNFG_ptr to, CNFG_ptr from)
{
  cp_macs(to, from);
  cp_crs(to, from);
  return;
}

int VMECC::cmp_macs(CNFG_ptr exp, CNFG_ptr rbk)
{
  int i,err;
  err=0;
  for(i=0;i<6;i++)if(rbk->mac.device[i]!=exp->mac.device[i])err++;
  for(i=0;i<6;i++)if(rbk->mac.mcast1[i]!=exp->mac.mcast1[i])err++;
  for(i=0;i<6;i++)if(rbk->mac.mcast2[i]!=exp->mac.mcast2[i])err++;
  for(i=0;i<6;i++)if(rbk->mac.mcast3[i]!=exp->mac.mcast3[i])err++;
  for(i=0;i<6;i++)if(rbk->mac.dflt_srv[i]!=exp->mac.dflt_srv[i])err++;
  return err;
}

int VMECC::cmp_crs(CNFG_ptr exp, CNFG_ptr rbk)
{
  int err;
  err=0;
  if(rbk->ether    != exp->ether)err++;
  if(rbk->ext_fifo != exp->ext_fifo)err++;
  if(rbk->rst_misc != exp->rst_misc)err++;
  // Must mask off auto reset bits before comparison!
  if(rbk->vme != ((exp->vme)&~(VME_CR_UPD_IRQ | VME_CR_FRC_SRST)))err++;
  if(rbk->vme_bto  != exp->vme_bto)err++;
  if(rbk->vme_bgto != exp->vme_bgto)err++;
  return err;
}

int VMECC::cmp_cnfg(CNFG_ptr exp, CNFG_ptr rbk)
{
  int err;
  err = cmp_macs(exp, rbk);
  err += cmp_crs(exp, rbk);
  return err;
}

void VMECC::print_macs(CNFG_ptr cp)
{
  int i;
  printf("              MAC address\n---------------------------\n");
  printf("Device:   ");
  for(i=0;i<5;i++)printf("%02X-",cp->mac.device[i]);
  printf("%02X\n",cp->mac.device[5]);
  printf("Mcast1:   ");
  for(i=0;i<5;i++)printf("%02X-",cp->mac.mcast1[i]);
  printf("%02X\n",cp->mac.mcast1[5]);
  printf("Mcast2:   ");
  for(i=0;i<5;i++)printf("%02X-",cp->mac.mcast2[i]);
  printf("%02X\n",cp->mac.mcast2[5]);
  printf("Mcast3:   ");
  for(i=0;i<5;i++)printf("%02X-",cp->mac.mcast3[i]);
  printf("%02X\n",cp->mac.mcast3[5]);
  printf("Dflt_Srv: ");
  for(i=0;i<5;i++)printf("%02X-",cp->mac.dflt_srv[i]);
  printf("%02X\n\n",cp->mac.dflt_srv[5]);
  return;
}

void VMECC::print_crs(CNFG_ptr cp)
{
  printf("           Config Registers\n---------------------------\n");
  printf("Ether:       %04hX\n",cp->ether);
  printf("Ext_FIFO:    %04hX\n",cp->ext_fifo);
  printf("Rst_Misc:    %04hX\n",cp->rst_misc);
  printf("VME:     %08X\n",cp->vme);
  printf("VME_BTO:     %04hX\n",cp->vme_bto);
  printf("VME_BGTO:    %04hX\n\n",cp->vme_bgto);
  return;
}

void VMECC::print_cnfg(CNFG_ptr cp)
{
  print_macs(cp);
  print_crs(cp);
  return;
}

void VMECC::log_mac_diffs(CNFG_ptr cp1, CNFG_ptr cp2)
{
  int i;
  char *fmt;
  char dsh[] = "%02X-"; 
  char nodshnr[] = "%02X"; 
  char nodsh[] = "%02X\n"; 
  fprintf(fplog,"    Device:   ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodshnr;
    fprintf(fplog,fmt,cp1->mac.device[i]);
  }
  fprintf(fplog," | ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodsh;
    fprintf(fplog,fmt,cp2->mac.device[i]);
  }
  fprintf(fplog,"    Mcast1:   ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodshnr;
    fprintf(fplog,fmt,cp1->mac.mcast1[i]);
  }
  fprintf(fplog," | ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodsh;
    fprintf(fplog,fmt,cp2->mac.mcast1[i]);
  }
  fprintf(fplog,"    Mcast2:   ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodshnr;
    fprintf(fplog,fmt,cp1->mac.mcast2[i]);
  }
  fprintf(fplog," | ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodsh;
    fprintf(fplog,fmt,cp2->mac.mcast2[i]);
  }
  fprintf(fplog,"    Mcast3:   ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodshnr;
    fprintf(fplog,fmt,cp1->mac.mcast3[i]);
  }
  fprintf(fplog," | ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodsh;
    fprintf(fplog,fmt,cp2->mac.mcast3[i]);
  }
  fprintf(fplog,"    Dflt_Srv: ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodshnr;
    fprintf(fplog,fmt,cp1->mac.dflt_srv[i]);
  }
  fprintf(fplog," | ");
  for(i=0;i<6;i++){
    fmt = (i<5)?dsh:nodsh;
    fprintf(fplog,fmt,cp2->mac.dflt_srv[i]);
  }
  return;
}

void VMECC::log_cr_diffs(CNFG_ptr cp1, CNFG_ptr cp2)
{
  fprintf(fplog,"    Ether:                 %04hX",cp1->ether);
  fprintf(fplog," | ");
  fprintf(fplog,"    %04hX\n",cp2->ether);

  fprintf(fplog,"    Ext_FIFO:              %04hX",cp1->ext_fifo);
  fprintf(fplog," | ");
  fprintf(fplog,"    %04hX\n",cp2->ext_fifo);

  fprintf(fplog,"    Rst_Misc:              %04hX",cp1->rst_misc);
  fprintf(fplog," | ");
  fprintf(fplog,"    %04hX\n",cp2->rst_misc);

  fprintf(fplog,"    VME:               %08X",cp1->vme);
  fprintf(fplog," | ");
  fprintf(fplog,"%08X\n",cp2->vme);

  fprintf(fplog,"    VME_BTO:               %04hX",cp1->vme_bto);
  fprintf(fplog," | ");
  fprintf(fplog,"    %04hX\n",cp2->vme_bto);

  fprintf(fplog,"    VME_BGTO:              %04hX",cp1->vme_bgto);
  fprintf(fplog," | ");
  fprintf(fplog,"    %04hX\n\n",cp2->vme_bgto);
  return;
}

void VMECC::log_cnfg_diffs(CNFG_ptr cp1, CNFG_ptr cp2)
{
  log_mac_diffs(cp1,cp2);
  log_cr_diffs(cp1,cp2);
  return;
}

// old cntrl_subs.c

int VMECC::flash_cmds(int nwrds)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Flash_R_W;
  nwbuf=2*nwrds+2;
  //  for(l=0;l<nwbuf;l++){
  //    printf("%02X",wbuf[l]&0xFF);
  //  }
  //  printf("\n");
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return n;
}
int VMECC::vme_cmds(int nwrds)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=VME_Cmds;
  nwbuf=2*nwrds+2;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return n;
}

int VMECC::wrt_ff(int nwrds)
{
  int n,l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Wrt_Ext_FF;
  nwbuf=2*nwrds+2;
  n=eth_write();
  for(l=0;l<8000;l++)lcnt++;
  return n;
}


int VMECC::rd_ff(int nwrds, int retrans, unsigned short int *buf)
{
  int i,n,numwrds,totwrds;
  int pkt_type;

  wbuf[0]=0x00;
  wbuf[1]=(retrans==1)?RT_Ext_FF:Rd_Ext_FF;
  wbuf[2]=(nwrds>>24)&0XFF;
  wbuf[3]=(nwrds>>16)&0XFF;
  wbuf[4]=(nwrds>>8)&0XFF;
  wbuf[5]=nwrds&0XFF;
  nwbuf=6;
  n=eth_write();
  for(i=0;i<nwrds;i++) buf[i] = 0;
  totwrds = 0;
  while(totwrds<nwrds){
    n=eth_read();
    if(n>6){
      pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
      if(pkt_type==EXFIFO_PKT){
	numwrds = ((rbuf[WRD_CNT_OFF]&0xff)<<8)|(rbuf[WRD_CNT_OFF+1]&0xff);
	for(i=0;i<numwrds;i++){
	  buf[i] = ((rbuf[2*i+DATA_OFF]&0xFF)<<8)|(rbuf[2*i+DATA_OFF+1]&0xFF);
	}
	totwrds += numwrds;
      }
      else if(pkt_type>INFO_PKT){
	return -1;
      }
      else {
	printf("Received unexpected packet type\n");
	return -2;
      }
    }
    else {
      return 0;
    }
  }
  return totwrds;
}

int VMECC::rd_pkt()
{
  int n;
  int pkt_type;

  n=eth_read();
  if(n>6){
    pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
    return pkt_type;
  }
  else {
    return -1;
  }
}


void VMECC::prst_ff()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=PRst_Ext_FF;
  nwbuf=2;
  n=eth_write();
  printf("Partial reset of FIFO done.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::mrst_ff()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=MRst_Ext_FF;
  nwbuf=2;
  n=eth_write();
  printf("Full reset of FIFO done.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::set_mark()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=ST_MK_Ext_FF;
  nwbuf=2;
  n=eth_write();
  printf("Mark set at current read pointer.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::rst_mark()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=RST_MK_Ext_FF;
  nwbuf=2;
  n=eth_write();
  printf("Reset mark -- Back to normal mode.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::set_ff_test_mode()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_FF_Test;
  nwbuf=2;
  n=eth_write();
  printf("Controller is in FIFO test mode.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::set_VME_mode()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_FF_VME;
  nwbuf=2;
  n=eth_write();
  printf("Controller is in VME mode.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::enable_ECC()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=ECC_enable;
  nwbuf=2;
  n=eth_write();
  printf("Error Correcting Code is enabled.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::disable_ECC()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=ECC_disable;
  nwbuf=2;
  n=eth_write();
  printf("Error Correcting Code is disabled.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::rst_err_cnt()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Rst_Ext_Err_Cnt;
  nwbuf=2;
  n=eth_write();
  printf("Reset error counters.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}
void VMECC::enable_inj_errs()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Set_Inj_Err;
  nwbuf=2;
  n=eth_write();
  printf("Set inject error mode.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}
void VMECC::disable_inj_errs()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Rst_Inj_Err;
  nwbuf=2;
  n=eth_write();
  printf("Error injecting is disabled.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::enable_warn_on_shutdown()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Warn_On_Shdwn;
  nwbuf=2;
  n=eth_write();
  printf("Shutdown warnings will be transmitted.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}
void VMECC::disable_warn_on_shutdown()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=No_Warn_On_Shdwn;
  nwbuf=2;
  n=eth_write();
  printf("Warnings of pending shutdowns are being disabled!\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::snd_startup_pkt()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Snd_Startup_Pkt;
  nwbuf=2;
  n=eth_write();
  printf("An Info packet will be sent on start-up.\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::no_startup_pkt()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=No_Startup_Pkt;
  nwbuf=2;
  n=eth_write();
  printf("Disabling Info packet on start-up\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::rst_seq_id()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Rst_Seq_ID;
  nwbuf=2;
  n=eth_write();
  printf("Reset Sequential ID\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

void VMECC::force_reload()
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Force_Reload;
  nwbuf=2;
  n=eth_write();
  printf("Forcing Reload!\n");
  for(l=0;l<8000;l++)lcnt++;
  return;
}

int VMECC::rd_err_cnts(unsigned int *cor_p, unsigned int *uncor_p)
{
  int n;
  int i;
  int pkt_type;
  int status = 0;
  wbuf[0]=0x40;
  wbuf[1]=Rd_Ext_Err_Cnts;
  nwbuf=2;
  n=eth_write();
  n=eth_read();
  pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
  //  printf("pkt_type = %02x: ",pkt_type);
  //  for (i=0; i<n; i++)printf("%02X",rbuf[i]);
  if(pkt_type==EXFIFO_PKT){
    *uncor_p = 0;
    *cor_p   = 0;
    for(i=0;i<4;i++){
      *uncor_p |= ((rbuf[DATA_OFF+i]&0xff)<<(24-8*i));
      *cor_p   |= ((rbuf[DATA_OFF+4+i]&0xff)<<(24-8*i));
    }
  }
  else if(pkt_type>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    status = -2;
  }
  else {
    status=-1;
  }
  return status;
}

int VMECC::prg_off(unsigned int full_off, unsigned int mt_off)
{
  int n;
  int l,lcnt;
  wbuf[0]=0x00;
  wbuf[1]=Prg_Ext_Off;
  nwbuf=10;
  wbuf[2]=(full_off>>24)&0xff;
  wbuf[3]=(full_off>>16)&0xff;
  wbuf[4]=(full_off>>8)&0xff;
  wbuf[5]=(full_off)&0xff;
  wbuf[6]=(mt_off>>24)&0xff;
  wbuf[7]=(mt_off>>16)&0xff;
  wbuf[8]=(mt_off>>8)&0xff;
  wbuf[9]=(mt_off)&0xff;
  n=eth_write();
  //  printf("Offsets programmed\n");
  for(l=0;l<8000;l++)lcnt++;
  return n;
}

int VMECC::rdbk_off(unsigned int *full_p, unsigned int *mt_p)
{
  int i,n;
  int pkt_type;
  int status = 0;
  wbuf[0]=0x00;
  wbuf[1]=Rdbk_Ext_Off;
  nwbuf=2 ;
  n=eth_write();
  n=eth_read();
  pkt_type = rbuf[PKT_TYP_OFF] & 0xFF;
  //  printf("pkt_type = %02x: ",pkt_type);
  //  for (i=0; i<n; i++)printf("%02X",rbuf[i]);
  if(pkt_type==EXFIFO_PKT){
    *full_p = 0;
    *mt_p = 0;
    for(i=0;i<4;i++){
      *full_p |= ((rbuf[DATA_OFF+i]&0xff)<<(24-8*i));
      *mt_p   |= ((rbuf[DATA_OFF+4+i]&0xff)<<(24-8*i));
    }
  }
  else if(pkt_type>=INFO_PKT){
    printf("%s",dcode_msg_pkt(rbuf));
    status = -2;
  }
  else {
    status = -1;
  }
  return status;
}

void VMECC::ld_usr_reg(unsigned int user)
{
  int n;
  wbuf[0]=0x00;
  wbuf[1]=Load_User_Reg;
  wbuf[2]=(user>>24)&0xFF;
  wbuf[3]=(user>>16)&0xFF;
  wbuf[4]=(user>>8)&0xFF;
  wbuf[5]=user&0xFF;
  nwbuf=6;
  n=eth_write();
  return;
}
