/*****************************************************************************\
* $Id: Crate.h,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: Crate.h,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 1.8  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.7  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 1.6  2008/09/22 14:31:53  paste
* /tmp/cvsY7EjxV
*
* Revision 1.5  2008/08/19 14:51:01  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 1.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_CRATE_H__
#define __EMU_FED_CRATE_H__

#include <vector>

#include "Exception.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"

namespace emu {
	namespace fed {

		class VMEController;

		/** @class Crate An object that contains VMEModules and a VMEController.  **/
		class Crate
		{
		public:

			/** Default constructor.
			*
			*	@param myNumber the identification number of the crate.
			**/
			Crate(unsigned int myNumber);

			/** @returns the identification number of the crate. **/
			unsigned int number() {return number_;}

			/** Adds a VMEModule to the crate. **/
			void addBoard(VMEModule* myBoard);

			/** Sets the VMEController in the crate. **/
			void setController(VMEController* controller);

			/** @returns a pointer to the crate's VMEController. **/
			VMEController *getController() { return vmeController_; }

			/** @returns all the boards of type T that are in the crate. **/
			template<typename T>
			std::vector<T *> getBoards() 
			{

				// Check to see if we have any boards to return.
				std::vector<T *> returnVector;
				for (std::vector<VMEModule *>::iterator iBoard = boardVector_.begin(); iBoard != boardVector_.end();  iBoard++) {
					T *board = dynamic_cast<T *>((*iBoard));
					if (board != 0) returnVector.push_back(board);
				}
				
				return returnVector;
			}

			/** @returns a vector of all the DDUs in the crate. **/
			inline std::vector<DDU *> getDDUs() { return getBoards<DDU>(); }

			/** @returns a vector of all the DCCs in the crate. **/
			inline std::vector<DCC *> getDCCs() { return getBoards<DCC>(); }

			/** @returns a pointer to the special broadcast DDU for the crate. **/
			inline DDU *getBroadcastDDU() { return broadcastDDU_; }
			
			//inline DCC *getBroadcastDCC() { return broadcastDCC_; }
			
			/** @returns the calculated RUI for the given DDU slot number in this crate. **/
			int getRUI(const int slot);
			int getRUI(DDU *const myDDU) {
				return getRUI(myDDU->slot());
			}

			/** Relay the configure command to all the boards in the crate. **/
			void configure()
			throw (emu::fed::exception::ConfigurationException);
		
		private:
			
			/// The unique identification number for the crate.
			unsigned int number_;
			
			/// The boards in the crate.
			std::vector<VMEModule *> boardVector_;

			/// A pointer to VMEController for the crate.
			VMEController *vmeController_;

			/// A pointer to the special broadcast DDU for the crate.
			DDU *broadcastDDU_;
			
			//DCC *broadcastDCC_;
			
		};

	}
}

#endif

