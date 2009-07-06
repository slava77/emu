/*****************************************************************************\
* $Id: VMELock.cc,v 1.1 2009/07/06 16:05:40 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMELock.h"
#include <sstream>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>



emu::fed::VMELock::VMELock(const std::string &fileName)
throw (emu::fed::exception::SoftwareException):
lockfile_(fileName),
fd_(-1)
{
	// Try to open the file
	if ((fd_ = open(lockfile_.c_str(), O_CREAT)) < 0) {
		std::ostringstream error;
		error << "Error opening lock file " << lockfile_ << ":  permissions problem?";
		XCEPT_RAISE(emu::fed::exception::SoftwareException, error.str());
	}
	
	// Make sure other people can use this file
	if (fchmod(fd_, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) {
		std::ostringstream error;
		error << "Error changing permissions of lock file " << lockfile_;
		XCEPT_RAISE(emu::fed::exception::SoftwareException, error.str());
	}
}



emu::fed::VMELock::~VMELock()
{
	// Make sure everything is unlocked
	flock(fd_, LOCK_UN);
	close(fd_);
	delete lock_;
}



void emu::fed::VMELock::lock()
throw (emu::fed::exception::SoftwareException)
{

	// First do the non-expensive locking of the process
	
	lock_ = new boost::recursive_mutex::scoped_lock(mutex_);
	
	// Now try to lock the file
	if (flock(fd_, LOCK_EX)) {
		std::ostringstream error;
		error << "Error locking file " << lockfile_;
		XCEPT_RAISE(emu::fed::exception::SoftwareException, error.str());
	}
}



void emu::fed::VMELock::unlock()
throw (emu::fed::exception::SoftwareException)
{
	// First, try to unlock the file
	if (flock(fd_, LOCK_UN)) {
		std::ostringstream error;
		error << "Error unlocking file " << lockfile_;
		XCEPT_RAISE(emu::fed::exception::SoftwareException, error.str());
	}

	// Last, do the non-expensive unlocking of the process
	delete lock_;
}
