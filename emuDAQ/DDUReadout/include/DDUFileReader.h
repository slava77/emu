#ifndef DDUFileReader_h
#define DDUFileReader_h
#include <unistd.h>

class DDUFileReader {
private:
	unsigned short raw_event[200000];

	unsigned long long word_0, word_1, word_2; // To remember some history
	unsigned long long file_buffer[4000];      // Read data block for efficiency

	unsigned long long *end, *file_buffer_end; // where stoped last time and where is end

public:
	enum {Header=1,Trailer=2,DDUoversize=4,FFFF=8,Unknown=16,EndOfStream=32};
	enum {Type1=Header|Trailer, Type2=Header, Type3=Header|DDUoversize, Type4=Trailer, Type5=Unknown, Type6=Unknown|DDUoversize, Type7=FFFF}; // Andrey Korytov's notations
private:
	unsigned int eventStatus, selectCriteria, acceptCriteria, rejectCriteria;

	int fd;

public:
	int    open(const char *filename);
	size_t read(const unsigned short* &buf); // Just plain read function
	size_t next(const unsigned short* &buf); // Same as ``read'', but returns only events pass certain criteria
	void select(unsigned int criteria); // return events satisfying all criteria
	void accept(unsigned int criteria); // return all events satisfying any of criteria
	void reject(unsigned int criteria); // return events not satisfying any of criteria

	unsigned int status(void){ return eventStatus; }

	DDUFileReader(void);
	virtual ~DDUFileReader(void);
};

#endif
