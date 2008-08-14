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
