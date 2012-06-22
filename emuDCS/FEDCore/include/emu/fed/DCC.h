/*****************************************************************************\
* $Id: DCC.h,v 1.16 2012/06/22 15:22:44 cvuosalo Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCC_H__
#define __EMU_FED_DCC_H__

#include <string>

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

#include "emu/fed/VMEModule.h"

namespace emu {

	namespace fed {

		class FIFO;

		/** @class DCC A class for talking to DCC boards. **/
		class DCC: public VMEModule
		{
		public:
			//friend class DCCParser;
			//friend class AutoConfigurator;
			//friend class DCCDBAgent;

			enum DEVICE {
				TTS = 0,
				SLinkA,
				SLinkB,
				All
			};

			/** @param slot the slot of the board for VME addressing purposes. **/
			DCC(const unsigned int slot, const bool fake = false);

			/** Default destructor. **/
			virtual ~DCC();

			/** @return the FIFOInUse parameter from configuration **/
			const inline uint16_t getFIFOInUse() const { return fifoinuse_; }

			/** Sets the FIFOInUse parameter **/
			inline void setFIFOInUse(const uint16_t fifoinuse) { fifoinuse_ = fifoinuse; reloadFIFOUsedBits(fifoinuse_); }

			/** @return the Software switch parameter from configuration **/
			const inline uint16_t getSoftwareSwitch() const { return softsw_; }

			/** Sets the Software switch parameter **/
			inline void setSoftwareSwitch(const uint16_t softsw) { softsw_ = softsw; }

			/** @return the FMM ID parameter from configuration **/
			const inline uint16_t getFMMID() const { return fmm_id_; }

			/** Sets the FMMID parameter **/
			inline void setFMMID(const uint16_t fmm_id) { fmm_id_ = fmm_id; }

			/** @return the S-Link parameter from configuration **/
			const inline uint16_t getSLinkID(const unsigned short int iSlink) const
			throw (emu::fed::exception::OutOfBoundsException)
			{
				if (iSlink == 1) return slink1_id_;
				else if (iSlink == 2) return slink2_id_;
				else XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, "Parameter must be either 1 or 2");
			}

			/** Sets the S-Link ID parameter **/
			inline void setSLinkID(const unsigned short int iSlink, const uint16_t slink_id)
			throw (emu::fed::exception::OutOfBoundsException)
			{
				if (iSlink == 1) slink1_id_ = slink_id;
				else if (iSlink == 2) slink2_id_ = slink_id;
				else XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, "Parameter must be either 1 or 2");
			}

			/** Configure the DCC by loading the FIFOInUse and SoftwareSwitch variables to the PROMs. **/
			void configure()
			throw (emu::fed::exception::DCCException);

			/** Part of the suite of FIFO methods.
			*	@returns a vector of FIFOs in FIFO-order.
			**/
			const inline std::vector<FIFO *> getFIFOs() const { return fifoVector_; }

			/** Part of the suite of fiber methods.
			*	@param fifoNumber runs from 0-14.
			*	@returns the fifo at the given fifo input number.
			**/
			const FIFO *getFIFO(const unsigned int fifoNumber) const
			throw (emu::fed::exception::OutOfBoundsException);

			/** Adds a FIFO object to the DCC.
			*	@param fifo is the FIFO being added.
			*	@param fifoNumber is the FIFO number.
			**/
			void addFIFO(FIFO *fifo)
			throw (emu::fed::exception::OutOfBoundsException);

			/** Sets the vector of FIFO objects in the DCC to some vector.
			*	@param fifoVector is a vector of FIFOs to copy to the internal vector.
			**/
			void setFIFOs(const std::vector<FIFO *> &fifoVector)
			throw (emu::fed::exception::OutOfBoundsException);

			/** Deletes a given FIFO and invalidates the pointer **/
			inline void deleteFIFO(FIFO *fifo)
			{
				for (std::vector<FIFO *>::iterator iFIFO = fifoVector_.begin(); iFIFO != fifoVector_.end(); ++iFIFO) {
					if (fifo == (*iFIFO)) {
						fifoVector_.erase(iFIFO);
						break;
					}
				}
				reloadFIFOInUse();
			}

			/** Reloads the FIFO in use from the inserted FIFOs **/
			void reloadFIFOInUse();

			// PGK New interface

			/** @returns the high 16-bits of the DCC status register **/
			const uint16_t readStatusHigh()
			throw (emu::fed::exception::DCCException);

			/** @returns the low 16-bits of the DCC status register
			 *
			 * @note These 16 bits are actually independent of the value returned from
			 * readStatusHigh.  The name is purely historical.
			 **/
			const uint16_t readStatusLow()
			throw (emu::fed::exception::DCCException);

			/** @returns the FMM status of the DCC, as decoded from the StatusHigh register **/
			const inline uint8_t readFMMStatus()
			throw (emu::fed::exception::DCCException)
			{
				try {
					return ((readStatusHigh() >> 12) & 0xf);
				} catch (emu::fed::exception::DCCException &e) {
					throw e;
				}
			}

			/** @returns the SLink statuses, as decided from the StatusHigh register **/
			const inline uint8_t readSLinkStatus()
			throw (emu::fed::exception::DCCException)
			{
				try {
					return (readStatusHigh() & 0xf);
				} catch (emu::fed::exception::DCCException &e) {
					throw e;
				}
			}

			/** @return the low 16 bits of the L1A counter **/
			const inline uint16_t readL1ALow()
			throw (emu::fed::exception::DCCException)
			{
				try {
					return readStatusLow();
				} catch (emu::fed::exception::DCCException &e) {
					throw e;
				}
			}

			/** @return the high 8 bits of the L1A counter **/
			const uint8_t readL1AHigh()
			throw (emu::fed::exception::DCCException);

			/** @return the combined 24 bits of the L1A counter **/
			const inline uint32_t readL1A()
			throw (emu::fed::exception::DCCException)
			{
				try {
					return readL1ALow() && (((uint32_t) readL1AHigh()) << 16);
				} catch (emu::fed::exception::DCCException &e) {
					throw e;
				}
			}

			/** @return the complete input FIFO status.
			 *
			 * @note low 5 bits half full status for paired FIFOs 1-10,
			 * high 5 bits are almost full status for paired FIFOs 1-10,
			 * bits low are logical true.
			 * **/
			const uint16_t readFIFOStatus()
			throw (emu::fed::exception::DCCException);

			/** @returns the input FIFOs (DDUs) that are being read in
			 * to the DCC as a binary number where a 1 in the nth bit means that
			 * FIFO n is being read in.
			 *
			 * @note There are only 10 fifos, but the first 11 bits are not ignored.
			 *
			 * @sa getDDUSlotFromFIFO
			 **/
			const uint16_t readFIFOInUse()
			throw (emu::fed::exception::DCCException);

			/** Write a new value to the FIFOInUse register.
			 *
			 * @param value the new FIFOInUse register value to write.
			 *
			 * @note There are only 10 FIFOs, but only the high 5 bits of value are ignored.
			 *
			 * @sa getDDUSlotFromFIFO
			 **/
			void writeFIFOInUse(const uint16_t value)
			throw (emu::fed::exception::DCCException);

			/** @returns the current read-in or read-out data rate (in bytes/sec) of
			 * the given FIFO (DDU or S-Link).
			 *
			 * @param fifo is the FIFO from which to read the data rate.  fifo==0
			 * corresponds to S-Link 0, fifo==[1,5] correspond to the DDU input FIFOs
			 * 1-5, fifo==6 corresponds to S-Link 1, and fifo==[7,11] correspond to
			 * the DDU input FIFOs 6-10.
			 **/
			const uint16_t readRate(const unsigned int fifo)
			throw (emu::fed::exception::DCCException);

			/** @returns the current setting of the omni-purpose software switch.
			 *
			 * @note Some of the software switch values are documented in
			 * EmuFCrateHyperDAQ.cc.  The first 8 bits are decoded from the DCC
			 * manual as follows:
			 * bits 0-2:
			 * 111 == DCM on control FPGA will be reset
			 * ??1 == Immediately upon switching bit 0 from low to high, one L1A is sent
			 * 11? == L1As will be sent continuously with ~1.6 ms intervals between each
			 * bits 3-4:
			 * 00 == The INFPGA will generate (fake?) data on every L1A
			 * 01 == Normal operation
			 * 10 == Disable propagation of S-Link backpressure to DDUs
			 * 11 == Serial loopback on the custom backplane RocketIO
			 * bits 5-7: select the xilinx platform revision, which determines the
			 * S-Link (FMM) IDs.
			 **/
			const uint16_t readSoftwareSwitch()
			throw (emu::fed::exception::DCCException);

			/** Set the omni-purpose software switch.
			 *
			 * @param value the value to which to set the software switch.
			 *
			 * @sa readSoftwareSwitch()
			 **/
			void writeSoftwareSwitch(const uint16_t value)
			throw (emu::fed::exception::DCCException);

			/** @return the value of the FMM register. **/
			const uint16_t readFMM(const enum DEVICE dev = All)
			throw (emu::fed::exception::DCCException);

			/** Write a custom value to the FMM register.
			 *
			 * @param value the FMM value to set.
			 * Bits   0-3:  FMM status of DCC itself
			 * Bit      4:  Enable to force cusom FMM status sent to TTS
			 * Bits   5-8:  Slink 0 FMM status
			 * Bit      9:  Enable to force custom Slink 0 FMM status sent to TTS
			 * Bits 10-13:  Slink 1 FMM status
			 * Bit     14:  Enable to force custom Slink 1 FMM status sent to TTS
			 * Bit     15:  Unused
			 *
			 * @note Useful for TTS tests.  The FMM value sent down the TTS wire is a logical OR of
			 * either the actual hardware states of the DCC, SLink0, and SLink1, or the custom FMM value
			 * (if the force bit for that hardware element is high).
			 **/
			void inline writeFMM(const uint16_t value)
			throw (emu::fed::exception::DCCException)
			{
				try {
					writeFMM(All, value);
				} catch (...) {
					throw;
				}
			}

			void writeFMM(const enum DEVICE dev, const uint16_t value)
			throw (emu::fed::exception::DCCException);

			/** @return the last TTC command sent. **/
			const uint16_t readTTCCommand()
			throw (emu::fed::exception::DCCException);

			/** Immediately sends a TTC command to the FED crate.
			 *
			 * @param value the command to send.  Useful values are 0x34 (FED
			 * crate hard reset) and 0x03 (FED crate sync reset).  Note that bit
			 * 12_0 of the software switch must be set high for these commands to
			 * work properly.
			 *
			 * @note This routine takes care of the unused 2 bits at the beginning
			 * of the TTC command byte for the user.  These 2 bits can be sent using
			 * the commands resetBX() and resetEvents().
			 **/
			void writeTTCCommand(const uint8_t value)
			throw (emu::fed::exception::DCCException);

			/** Send the TTC command to reset BX values in the FED crate. **/
			void resetBX()
			throw (emu::fed::exception::DCCException);

			/** Send the TTC command to reset event counters in the FED crate. **/
			void resetEvents()
			throw (emu::fed::exception::DCCException);

			/** Set the fake L1A generation register.
			 *
			 * @param value the low 8 bits are used to set the number of L1As to generate,
			 * the high 8 bits are used to set the rate at which they are generated.
			 **/
			void writeFakeL1A(const uint16_t value)
			throw (emu::fed::exception::DCCException);
/* stan added routines Feb 9, 2012 */
                        /* */
                        void writeDisableOutOfSyncOnL1AMismatch(const uint16_t value)
                        throw (emu::fed::exception::DCCException);
                        /* */
                        void writeEnableOutOfSyncOnL1AMismatch(const uint16_t value)
                        throw (emu::fed::exception::DCCException);
                        /* */
                        const uint16_t readNumberOfL1AMismatches()
                        throw (emu::fed::exception::DCCException);
                        /* */
                        const uint16_t readOutofSyncEnableDisable()
                        throw (emu::fed::exception::DCCException);
/* end stan added routines Feb 9, 2012 */
/* start CRC error routines Feb 23, 2012 */
                        /* */
                        void writeDisableCRCErrOnL1AMismatch(const uint16_t value)
                        throw (emu::fed::exception::DCCException);
                        /* */
                        void writeEnableCRCErrOnL1AMismatch(const uint16_t value)
                        throw (emu::fed::exception::DCCException);
                        /* */
                        const uint16_t readCRCErrEnableDisable()
                        throw (emu::fed::exception::DCCException);
/* end CRC error routines Feb 23, 2012 */
			/** @return the ID code from the given PROM chip.
			 *
			 * @param dev the device from which to read the ID code.
			 **/
			const uint32_t readIDCode(const enum DEVTYPE dev)
			throw (emu::fed::exception::DCCException);

			/** @return the User code programmed into the given PROM chip.
			 *
			 * @param dev the device from which to read the User code.
			 **/
			const uint32_t readUserCode(const enum DEVTYPE dev)
			throw (emu::fed::exception::DCCException);

			/** Resets an FPGA through the given PROM.
			*
			*	@param dev the PROM through which the signal should be sent.
			*
			*	@note The RESET device will reset the MCTRL FPGA.
			**/
			void resetPROM(const enum DEVTYPE dev)
			throw (emu::fed::exception::DCCException);

			// Misc routines

			/** Hard reset the crate through a TTC-override command. **/
			void crateHardReset()
			throw (emu::fed::exception::DCCException);

			/** Sync reset the crate through a TTC-override command. **/
			void crateResync(bool ignoreBackPress = false)
			throw (emu::fed::exception::DCCException);

			/** @returns the slot number of the DDU corresponding to the given input FIFO
			*
			* @param fifo the input FIFO number from which to calculate the DDU slot
			**/
			const unsigned int getDDUSlotFromFIFO(const unsigned int fifo) const
			throw (emu::fed::exception::OutOfBoundsException);

			/** @returns the FIFO number corresponding to the given input DDU slot number
			*
			* @param mySlot the DDU slot number from which to calculate the FIFO number
			**/
			const unsigned int getFIFOFromDDUSlot(const unsigned int mySlot) const
			throw (emu::fed::exception::OutOfBoundsException);

			/** @returns the SLink number corresponding to the given output FIFO
			*
			* @param fifo the output FIFO number from which to calculate the SLink number
			**/
			const unsigned int getSLinkFromFIFO(const unsigned int fifo) const
			throw (emu::fed::exception::OutOfBoundsException);

			/** @returns the FIFO number corresponding to the given SLink number
			*
			* @param slink the SLink number from which to calculate the output FIFO number
			**/
			const unsigned int getFIFOFromSLink(const unsigned int slink) const
			throw (emu::fed::exception::OutOfBoundsException);

			/** @returns the current read-in data rate (in bytes/sec) of the given DDU.
			*
			* @param mySlot is the slot number from which to read the data rate.
			**/
			const uint16_t readDDURate(const unsigned int mySlot)
			throw (emu::fed::exception::DCCException);

			/** @returns the current read-out data rate (in bytes/sec) of the given SLink.
			*
			* @param slink is the SLink number from which to read the data rate.
			**/
			const uint16_t readSLinkRate(const unsigned int slink)
			throw (emu::fed::exception::DCCException);


		protected:

			/** Reloads the used members of the owned FIFOs based on the supplied FIFO-in-use setting
			*
			*	@param fifoInUse is the FIFO-in-use setting to parse into used bits for the FIFOs
			**/
			void reloadFIFOUsedBits(const uint16_t fifoInUse);

			// PGK New interface
			/** Reads an arbitrary number of bits from a given register on a given device.
			*
			*	@param dev is the device from which to read.
			*	@param myReg is the register on the device from which to read.
			*	@param @nBits is the number of bits to read.
			**/
			const std::vector<uint16_t> readRegister(const enum DEVTYPE dev, const uint16_t myReg, const unsigned int nBits, const bool debug = false)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Writes an arbitrary number of bits to a given register on a given device.
			*
			*	@param dev is the device to which to write.
			*	@param myReg is the register on the device to which to write.
			*	@param @nBits is the number of bits to write.
			*	@param @myData is the data to write.
			**/
			const std::vector<uint16_t> writeRegister(const enum DEVTYPE dev, const uint16_t myReg, const unsigned int nBits, const std::vector<uint16_t> &data, const bool debug = false)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/// The FIFOs that are associated with this DCC, in FIFO (NOT slot)-order.
			std::vector<FIFO *> fifoVector_;

			/// The FIFO-in-use parameter from the configuration.
			uint16_t fifoinuse_;

			/// The software switch setting from the configuration.
			uint16_t softsw_;

			/// The FMM ID from the configuration
			uint16_t fmm_id_;

			/// The first S-Link ID (should be the same as the FMM ID) from the configuration
			uint16_t slink1_id_;

			/// The second S-Link ID (should be the same as the FMM ID + 1) from the configuration
			uint16_t slink2_id_;

		};

	}
}

#endif

