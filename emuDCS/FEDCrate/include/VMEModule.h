/*****************************************************************************\
* $Id: VMEModule.h,v 3.6 2008/08/19 14:51:02 paste Exp $
*
* $Log: VMEModule.h,v $
* Revision 3.6  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMEMODULE_H__
#define __VMEMODULE_H__

#include "JTAG_constants.h"
#include "FEDException.h"

#include "EmuFEDLoggable.h"
#include "JTAGDevice.h"

/* VMEModule is a virtual class for DCC and DDU classes.
 * A VMEModule should be apathetic to the controller and crate,
 * and although this breaks down at the program execution level (where
 * any given read/write from/to the DCC and DDU requires access of the
 * controller), it shouldn't break down at the object level.
 * Therefore, I removed or modified all public methods that referenced
 * the crate or controller.  Use the FEDCrate and VMEController objects
 * for those methods, not the DDU and DCC objects.
 */

namespace emu {

	namespace fed {
		
		class VMEModule: public EmuFEDLoggable, public JTAGDevice
		{
		public:
			//enum {MAXLINE = 70000};
			
			VMEModule(int mySlot);
			virtual ~VMEModule() {};
			inline const int slot() {return slot_;}
			
			/// Removing seperate VMEController class (not needed)
			inline int getDevice() {return Device_;}
			inline int getLink() {return Link_;}
			inline int32_t getBHandle() { return BHandle_; }
			/**	I am doing something smart here.  Instead of making the BHandles extern,
			*	I am going to have the original constructor set its own BHanlde to a
			*	crazy value in case of an error.  Thus, if the device is already open, I
			*	will be able to check the BHandle to figure that out, then simply replace
			*	the BHandle with the correct value once I know it.
			**/
			inline void setBHandle(int32_t BHandle) { BHandle_ = BHandle; }
			
			// should automatically start().  Here's what you do if
			// you want to end() by hand
			void endDevice();
			
			enum BOARDTYPE { DDU_ENUM=0, DCC_ENUM };
			virtual unsigned int boardType() const = 0;
		
		protected:
		
			// JTAG stuff
			void CAEN_read(unsigned long Address, unsigned short int *data)
				throw (FEDException);
			
			void CAEN_write(unsigned long Address, unsigned short int *data)
				throw (FEDException);
			
			void vme_controller(int irdwr, unsigned short int *ptr, unsigned short int *data, char *rcv);
			
			void devdo(DEVTYPE dev, int ncmd, const char *cmd, int nbuf, const char *inbuf, char *outbuf, int irdsnd)
				throw (FEDException);
			
			void scan(DEVTYPE dev, int reg, const char *snd, int cnt2, char *rcv, int ird)
				throw (FEDException);
			
			void handshake_vme();
			
			void flush_vme();
			
			void vmeser(const char *cmd, const char *snd, char *rcv);
			
			void vmepara(const char *cmd, const char *snd, char *rcv);
			
			void dcc(const char *cmd, char *rcv);
			
			void vme_adc(int ichp,int ichn,char *rcv);
			
			void RestoreIdle(enum DEVTYPE dv);

			void sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
			
			
			
			int CAEN_reset(void);
			
			/* void initDevice(int a); */
			/// used for calls to do_vme
			// FIXME!
			enum FCN { VME_READ=1, VME_WRITE=2 };
			enum WRT { LATER, NOW };
		
			/// required for DDU/DCC communications
			// FIXME!
			char sndbuf[4096];
			char rcvbuf[4096];
			char rcvbuf2[4096];
			char cmd[4096];
		
		private:
			int slot_;
			int Device_;
			int Link_;
			int32_t BHandle_;
			
			unsigned long int vmeadd_;
			//int idevo_;
			
			inline int pows_(int n, int m) { int ret = 1; for (int i=0; i<m; i++) ret *= n; return ret; }
			inline void udelay(long int itim) { for (long int j=0; j<itim; j++) for (long int i=0; i<200; i++); }
		};

	}
}

#endif

