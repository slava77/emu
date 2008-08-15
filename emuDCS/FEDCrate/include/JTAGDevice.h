/*****************************************************************************\
* $Id: JTAGDevice.h,v 3.2 2008/08/15 08:35:51 paste Exp $
*
* $Log: JTAGDevice.h,v $
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __JTAGDEVICE_H__
#define __JTAGDEVICE_H__

namespace emu {
	namespace fed {

		struct JTAGGeometry {
			char *nam;
			int jchan;
			char kbypass;
			int kbit;
			int kbybit;
			int nser;
			int seri[5];
			int sxtrbits;
			unsigned int femask;
		};
	
		class JTAGDevice
		{
		public:
			
			JTAGDevice();
			~JTAGDevice() {};
			
		protected:
			
			struct JTAGGeometry geo[25];
			
		};

	}
}

#endif
