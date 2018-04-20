/*
 * flash hardware abstraction layer
 */

#ifndef flash_hal_hpp
#define flash_hal_hpp

namespace hal
{
    namespace flash
    {
	extern const unsigned short pageSize; //memory page size
	extern const unsigned short maxPage; //maximum number of pages
	extern const unsigned short fsp; //firmware start page
	bool init(); //initialize memory
	void bootjump(); //jump to firmware code
	const unsigned int * read(unsigned short page); //read one page of memory and return pointer to it
	bool write(const unsigned int * data, unsigned short page); //write one page of data at specified page
    }
}

#endif
