/*****************************************************************************\
* $Id: VMEModule.h,v 3.9 2008/08/31 21:18:27 paste Exp $
*
* $Log: VMEModule.h,v $
* Revision 3.9  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.8  2008/08/30 14:49:04  paste
* Attempts to make VME work under the new design model where VMEModules take over for the VMEController.
*
* Revision 3.7  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
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
		
		class VMEController;
		
		class VMEModule: public EmuFEDLoggable, public JTAGDevice
		{
		public:
			//enum {MAXLINE = 70000};
			
			VMEModule(int mySlot);
			virtual ~VMEModule() {};
			inline const int slot() {return slot_;}
			
// 			inline int getDevice() {return Device_;}
// 			inline int getLink() {return Link_;}
// 			inline int32_t getBHandle() { return BHandle_; }
			/**	I am doing something smart here.  Instead of making the BHandles extern,
			*	I am going to have the original constructor set its own BHanlde to a
			*	crazy value in case of an error.  Thus, if the device is already open, I
			*	will be able to check the BHandle to figure that out, then simply replace
			*	the BHandle with the correct value once I know it.
			**/
// 			inline void setBHandle(int32_t BHandle) { BHandle_ = BHandle; }
			
			// should automatically start().  Here's what you do if
			// you want to end() by hand
			void endDevice();
			
			enum BOARDTYPE { DDU_ENUM=0, DCC_ENUM };
			virtual unsigned int boardType() const = 0;
			
			void inline setBHandle(int32_t BHandle) { BHandle_ = BHandle; }
		
		protected:
		
			// JTAG stuff
			void CAEN_read(unsigned long Address, unsigned short int *data)
				throw (FEDException);

			/** Returns the value from a particular address on this module.
			*	Because single address reads are always, ALWAYS 16 bits for these
			*	particular boards, this function will always return a 16-bit
			*	value.  Other routines will perform cycles of reads to dig out
			*	values larger than 16 bits.
			*
			*	@note No pointers were harmed in the making of this method.
			*
			*	@param Address The address (technically on any module) from
			*	which to read the data.
			*
			*	@returns the 16-bit value at that address.
			**/
			unsigned long int CAEN_read(unsigned long Address)
				throw (FEDException);
			
			void CAEN_write(unsigned long Address, unsigned short int *data)
				throw (FEDException);

			/** Writes 16 bits of data to the specified address.
			*
			*	@note Again, pointers are not used.
			*
			*	@param Address The address (technically on any module) to
			*	where the data will be written
			*	@param data The data two be written.
			**/
			void CAEN_write(unsigned long Address, int16_t data)
				throw (FEDException);
			
			void vme_controller(int irdwr, unsigned short int *ptr, unsigned short int *data, char *rcv);
			
			void devdo(DEVTYPE dev, int ncmd, const char *cmd, int nbuf, const char *inbuf, char *outbuf, int irdsnd)
				throw (FEDException);
			
			void scan(DEVTYPE dev, int reg, const char *snd, int cnt, char *rcv, int ird)
				throw (FEDException);
			
			/* void handshake_vme(); */
			
			/* void flush_vme(); */
			
			void vmeser(const char *cmd, const char *snd, char *rcv);
			
			void vmepara(const char *cmd, const char *snd, char *rcv);
			
			void dcc(const char *cmd, char *rcv);
			
			void vme_adc(int ichp,int ichn,char *rcv);
			
			void RestoreIdle(enum DEVTYPE dev);

			void sleep_vme(const char *outbuf);   // in usecs (min 16 usec)
			
			
			
			int CAEN_reset(void);
			
			/* void initDevice(int a); */
			/// used for calls to do_vme
			//enum FCN { VME_READ=1, VME_WRITE=2 };
			//enum WRT { LATER, NOW };
		
			/// required for DDU/DCC communications
			//char sndbuf[4096];
			//char rcvbuf[4096];
			//char rcvbuf2[4096];
			//char cmd[4096];
		
		private:
			int slot_;
			unsigned long int vmeadd_;
			int idevo_;
			
			int32_t BHandle_;
			
			inline int pows_(int n, int m) { int ret = 1; for (int i=0; i<m; i++) ret *= n; return ret; }
			inline void udelay_(long int itim) { for (long int j=0; j<itim; j++) for (long int i=0; i<200; i++); }
		};

	}
}

#endif

