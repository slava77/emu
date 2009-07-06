/*****************************************************************************\
* $Id: VMEModule.h,v 1.7 2009/07/06 16:05:40 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMEMODULE_H__
#define __EMU_FED_VMEMODULE_H__

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

#include "emu/fed/JTAGElement.h"

namespace emu {

	namespace fed {

		class VMEController;

		struct JTAGElement;
		
		class VMELock;

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
			VMEModule(const unsigned int &mySlot);

			virtual ~VMEModule() {};

			/** @returns the slot number. **/
			inline unsigned int slot() { return slot_; }
			inline unsigned int getSlot() { return slot_; }

			/** Sets the appropriate BHandle for proper CAEN communication.
			*
			*	@param myHandle is the new BHandle to use.
			**/
			inline void setBHandle(const int32_t &myHandle) { BHandle_ = myHandle; }

			/** @returns the current BHandle. **/
			inline int32_t getBHandle() { return BHandle_; }
			
			/** Sets the mutex to that given by the Crate.
			*
			*	@param myMutex is the new mutex to use.
			**/
			inline void setMutex(VMELock *myMutex) { mutex_ = myMutex; }

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
			int loadPROM(const enum DEVTYPE &dev, const char *fileName, const std::string &startString = "", const std::string &stopString = "", const bool &debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			int loadPROM(const enum DEVTYPE &dev, const std::string &fileName, const std::string &startString = "", const std::string &stopString = "", const bool &debug = false)
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
			*	@param data the data to write, with the first element of the vector being the LSB
			*	@param noRead if true, will read back the data shifted out of the JTAG device and return it
			**/
			std::vector<uint16_t> jtagWrite(const enum DEVTYPE &dev, const unsigned int &nBits, const std::vector<uint16_t> &data, const bool &noRead = false, const bool &debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads data from a particular JTAG device.
			*
			*	@param dev the JTAG device from which the data will be read
			*	@param nbits the number of bits to read
			**/
			std::vector<uint16_t> jtagRead(const enum DEVTYPE &dev, const unsigned int &nBits, const bool &debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

		protected:

			/** Sends a JTAG command cycle to a given JTAG device.
			*	@param dev the JTAG device to which the command will be sent
			*	@param myCommand the command code to send
			**/
			void commandCycle(const enum DEVTYPE &dev, const uint16_t &myCommand, const bool &debug = false)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads 16 bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			uint16_t readVME(const uint32_t &myAddress, const bool &debug = false)
			throw (emu::fed::exception::CAENException);

			/** Writes 16 bits to a given VME address.
			*
			*	@param myAddress the address to which to send the data.
			*	@param data the data to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeVME(const uint32_t &myAddress, const uint16_t &data, const bool &debug = false)
			throw (emu::fed::exception::CAENException);

			/** Reads any arbitrary number of bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*	@param nBits the number of bits to read.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			std::vector<uint16_t> readCycle(const uint32_t &myAddress, const unsigned int &nBits, const bool &debug = false)
			throw(emu::fed::exception::CAENException);

			/** Writes any arbitrary number of bits to a given VME address.
			*
			*	@param myAddress the address to which the data will be sent.
			*	@param nBits the number of bits to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeCycle(const uint32_t &myAddress, const unsigned int &nBits, const std::vector<uint16_t> &data, const bool &debug = false)
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
			VMELock *mutex_;

		};

	}
}

#endif

