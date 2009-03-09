/*****************************************************************************\
* $Id: VMEModule.h,v 1.2 2009/03/09 23:12:44 paste Exp $
*
* $Log: VMEModule.h,v $
* Revision 1.2  2009/03/09 23:12:44  paste
* * Fixed a minor bug in DCC MPROM ID/Usercode reading
* * Fixed a major bug in RESET path firmware loading
* * Added debug mode for CAEN reading/writing
*
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.21  2009/01/30 19:14:16  paste
* New emu::base namespace and emu::base::Supervised inheritance added.
*
* Revision 3.20  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.19  2008/10/22 20:23:57  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.18  2008/10/01 07:49:38  paste
* Removed busyloop waiting in favor of less accurate but more resource-friendly usleep.
*
* Revision 3.17  2008/09/29 08:36:26  paste
* Removed references to extinct JTAGDevice.h
*
* Revision 3.16  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.15  2008/09/22 14:31:53  paste
* /tmp/cvsY7EjxV
*
* Revision 3.14  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.13  2008/09/07 22:25:35  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.12  2008/09/03 17:52:58  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.11  2008/09/02 08:39:52  paste
* Better handling and display of new features in the DDU firmware.
*
* Revision 3.10  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
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
#ifndef __EMU_FED_VMEMODULE_H__
#define __EMU_FED_VMEMODULE_H__

#include <pthread.h> // For mutexes

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

#include "emu/fed/JTAGElement.h"

namespace emu {

	namespace fed {
		
		class VMEController;
	
		struct JTAGElement;

		/** @class VMEModule A inherited class for DCC and DDU classes.
		*	A VMEModule should be ignorant of the controller and crate, just as how a hardware board
		*	doesn't really care into which crate it is inserted.  In other words, a VMEModule does not
		*	own a VMEController or a VMECrate.
		*/
		class VMEModule
		{
		public:

			/** Default constructor.
			*
			*	@param mySlot is the board's slot number in the crate (needed for proper VME communication.)
			**/
			VMEModule(unsigned int mySlot);

			virtual ~VMEModule() {};

			/** @returns the slot number. **/
			inline unsigned int slot() {return slot_;}

			/** Sets the appropriate BHandle for proper CAEN communication.
			*
			*	@param myHandle is the new BHandle to use.
			**/
			inline void setBHandle(int16_t myHandle) { BHandle_ = myHandle; }

			/** @returns the current BHandle. **/
			inline int16_t getBHandle() { return BHandle_; }

			/** Call the appropriate methods to configure the board. **/
			virtual void configure() = 0;

			/** Parses and loads a given .svf file into a given PROM.
			 *
			 * @param dev the PROM to which to load the parsed .svf file.
			 * @param fileName the name on the local disk of the .svf file.
			 * @param startString if set will cause the loader to ignore all instructions until the line after the one matching it.
			 * @param stopString if set will cause the loader to stop immidately if it is found in the current line being read.  The line will not be loaded.
			 *
			 * @returns zero if no errors occurred, a positive int for warnings, a negative int for errors.
			 **/
			int loadPROM(enum DEVTYPE dev, char *fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			int loadPROM(enum DEVTYPE dev, const char *fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
			{
				try {
					return loadPROM(dev, (char *) fileName, startString, stopString, debug);
				} catch (...) {
					throw;
				}
			}

			int loadPROM(enum DEVTYPE dev, std::string fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
			{
				try {
					return loadPROM(dev, fileName.c_str(), startString, stopString, debug);
				} catch (...) {
					throw;
				}
			}

			/** Writes some data to a particular JTAG device.
			*
			*	@param dev the JTAG device to which the data will be sent
			*	@param nbits the number of bits to write
			*	@param myData the data to write, with the first element of the vector being the LSB
			*	@param noRead if true, will read back the data shifted out of the JTAG device and return it
			**/
			std::vector<uint16_t> jtagWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<uint16_t> myData, bool noRead = false, bool debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads data from a particular JTAG device.
			*
			*	@param dev the JTAG device from which the data will be read
			*	@param nbits the number of bits to read
			**/
			std::vector<uint16_t> jtagRead(enum DEVTYPE dev, unsigned int nBits, bool debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);
		
		protected:

			/** Sends a JTAG command cycle to a given JTAG device.
			*	@param dev the JTAG device to which the command will be sent
			*	@param myCommand the command code to send
			**/
			void commandCycle(enum DEVTYPE dev, uint16_t myCommand, bool debug = false)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads 16 bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			uint16_t readVME(uint32_t myAddress, bool debug = false)
			throw (emu::fed::exception::CAENException);

			/** Writes 16 bits to a given VME address.
			*
			*	@param myAddress the address to which to send the data.
			*	@param myData the data to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeVME(uint32_t myAddress, uint16_t myData, bool debug = false)
			throw (emu::fed::exception::CAENException);

			/** Reads any arbitrary number of bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*	@param nBits the number of bits to read.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			std::vector<uint16_t> readCycle(uint32_t myAddress, unsigned int nBits, bool debug = false)
			throw(emu::fed::exception::CAENException);

			/** Writes any arbitrary number of bits to a given VME address.
			*
			*	@param myAddress the address to which the data will be sent.
			*	@param nBits the number of bits to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeCycle(uint32_t myAddress, unsigned int nBits, std::vector<uint16_t> myData, bool debug = false)
			throw(emu::fed::exception::CAENException);

			/// A map of JTAG chains on this device.
			std::map<enum DEVTYPE, JTAGChain> JTAGMap;

		private:

			/// The slot number of the device (its location within it respective VME crate).
			unsigned int slot_;

			/// The CAEN BHandle used to communicate to the controller of the crate containing this device.
			int16_t BHandle_;

			/// The base address of the device as calculated from the slot number.
			uint32_t vmeAddress_;

			/// Mutex so that communication to and from the board is atomic.
			pthread_mutex_t mutex_;
			
		};

	}
}

#endif

