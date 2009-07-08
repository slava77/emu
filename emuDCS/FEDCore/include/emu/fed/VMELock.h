/*****************************************************************************\
* $Id: VMELock.h,v 1.2 2009/07/08 12:07:49 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMELOCK_H__
#define __EMU_FED_VMELOCK_H__

#include <pthread.h>
#include "emu/fed/Exception.h"
#include <string>

namespace emu {
	
	namespace fed {
		
		/** @class VMELock is a class for mutexing out VME reading and writing not only for this process (using standard POSIX mutexes), but for all processes on this machine (using a queued lockfile mutex).  The CAEN VME controller will autimatically serialize any reads and writes, but it does not prevent separate processes from interrupting a chain of commands, like those needed for JTAG reading/writing. **/
		class VMELock {
			
		public:
			
			/** Standard constructor. **/
			VMELock(const std::string &fileName)
			throw (emu::fed::exception::SoftwareException);
			
			/** Standard destructor. **/
			~VMELock();
			
			/** Attempts to issue a lock and blocks until the lock is established. **/
			void lock()
			throw (emu::fed::exception::SoftwareException);
			
			/** Releases the current lock if this process is indeed locking. **/
			void unlock()
			throw (emu::fed::exception::SoftwareException);
			
		private:
			
			/// A mutex for intra-process locking
			pthread_mutex_t mutex_;
			
			/// The name of the lock file
			std::string lockfile_;
			
			/// A file descriptor to use while the file is locked
			int fd_;
			
			/// Used to make the file lock recursive
			unsigned int nFileLock_;
		};

	}
}

#endif
