/*****************************************************************************\
* $Id: FEDCrate.h,v 1.6 2008/09/22 14:31:53 paste Exp $
*
* $Log: FEDCrate.h,v $
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
/*************************************************************************

The philosophy behind the objects FEDCrate, VMEModule, VMEController, DDU,
DCC, and IRQThread are as follows:

The FEDCrate contains DDU (pointer)s, DCC (pointer)s, and a VMEController
(pointer).  Because it contains everything, the FEDCrate is the highest
level object in the scheme.  When you pass a properly formatted XML file
to the FEDCrateParser, it will hand you back a std::vector of properly
initialized and filled FEDCrates.  Note that the FEDCrateParser is actually
a brain-dead object with only minimal error checking--the FEDCrate, DDU, DCC,
and VMEController objects it makes do all the magic of proper
initialization in their constructors.

Once you have a std::vector of FEDCrates from the FEDCrateParser, you can pick out
the various objects from the FEDCrate using the FEDCrate::ddus(), dccs(), and
vmeController() methods.  The VMEController object is the only thing that
is even remotely aware of which FEDCrate in which it resides (given by the
_private_ VMEController::crateNumber member).  Because of this, it is the
user's responsiblility to keep track of which FEDCrate he is addressing.

The VMEModule class is a dummy class with members that both DDU and DCC
objects inherit.

The VMEController object owns an instance of the IRQThread class.  All of
the interrupt handling is performed through this object.  You start and
stop the threads by addressing the VMEController with the methods
VMEController::start_thread(runnumber), stop_thread(), and kill_thread().
Once the thread has been started, you may access the thread directly by
picking it out of the VMEController with VMEController::thread().

Everything mentioned here has an example in the code.

 *************************************************************************/


#ifndef __FEDCRATE_H__
#define __FEDCRATE_H__

#include <vector>

#include "EmuFEDLoggable.h"

namespace emu {
	namespace fed {

		class VMEModule;
		class VMEController;
		class DDU;
		class DCC;

		class FEDCrate: public EmuFEDLoggable {
		public:
			FEDCrate(int myNumber, VMEController* myController = 0);
			~FEDCrate();
		
			int number() const {return number_;}
		
			void addDDU(DDU* myDDU);
			void addDCC(DCC* myDCC);
			void setController(VMEController* controller);
			void setBHandle(int32_t myBHandle);
		
			VMEController *getVMEController() const { return vmeController_; }

			inline std::vector<DDU *> getDDUs() const { return dduVector_; }
			inline std::vector<DCC *> getDCCs() const { return dccVector_; }
			template<class T>
				std::vector<T *> getBoards() const
			{

				// Check to see if we have any DDUs to return.
				if (dduVector_.size() > 0) {
					// Check to see if we want DDUs back anyway.
					T *board = dynamic_cast<T *>(dduVector_[0]);
					if (board != NULL) {
						
						std::vector<VMEModule *> vectorCache;
						vectorCache.reserve(dduVector_.size());
						// Convert one way...
						for (unsigned int iDDU = 0; iDDU < dduVector_.size(); iDDU++) {
							vectorCache.push_back((VMEModule *) dduVector_[iDDU]);
						}

						std::vector<T *> result;
						result.reserve(vectorCache.size());
						// Convert the other way...
						for (unsigned int iDDU = 0; iDDU < vectorCache.size(); iDDU++) {
							result.push_back(dynamic_cast<T *>(vectorCache[iDDU]));
						}
						return result;
					}
				}

				// Check to see if we have any DCCs to return.
				if (dccVector_.size() > 0) {
					// Check to see if we want DCCs back anyway.
					T *board = dynamic_cast<T *>(dccVector_[0]);
					if (board != NULL) {
						
						std::vector<VMEModule *> vectorCache;
						vectorCache.reserve(dccVector_.size());
						// Convert one way...
						for (unsigned int iDCC = 0; iDCC < dccVector_.size(); iDCC++) {
							vectorCache.push_back((VMEModule *) dccVector_[iDCC]);
						}
						
						std::vector<T *> result;
						result.reserve(vectorCache.size());
						// Convert the other way...
						for (unsigned int iDCC = 0; iDCC < vectorCache.size(); iDCC++) {
							result.push_back(dynamic_cast<T *>(vectorCache[iDCC]));
						}
						return result;
					}
				}

				// Else return an empty vector.
				std::vector<T *> result;
				return result;
			}

			inline DDU *getBroadcastDDU() { return broadcastDDU_; }
			//inline DCC *getBroadcastDCC() { return broadcastDCC_; }
			
			// Return the rui of the emu::fed::DDU in the given slot.  The crate number is
			//  needed to figure this out.
			int getRUI(int slot);
		
			void configure();
		// PGK, silly to have this belong to the crate and not the thread
		//	int irqtest(int crate,int ival);
		
		private:
			/*
			template<class T> T * findBoard() const
			{
				for(unsigned i = 0; i < moduleVector_.size(); ++i) {
					T * result = dynamic_cast<T *>(moduleVector_[i]);
					if (result != 0) return result;
				}
				return 0;
			}
			*/
			int number_;
			// No longer indexed by slot!
			std::vector<DDU *> dduVector_;
			std::vector<DCC *> dccVector_;
			VMEController * vmeController_;

			DDU *broadcastDDU_;
			//DCC *broadcastDCC_;
		};

	}
}

#endif

