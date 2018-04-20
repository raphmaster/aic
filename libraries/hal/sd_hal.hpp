/*
 * secure digital hardware abstraction layer
 */

#ifndef sd_hal_hpp
#define sd_hal_hpp

namespace hal
{
    namespace sd //one file open at a time
    {
	/*
	 * file open modes
	 * ro: read only, position at start of file
	 * rw: read and write, position at the end of file
	 */
	enum mode : unsigned char {ro, rw};
	bool init(); //initialize sd card
	bool exists(const char * file); //check if file exists
	bool mkdir(const char * dir); //create directory
	bool rmdir(const char * dir); //remove directory
	bool open(const char * file, unsigned char mode); //open file in specified mode
	bool close(); //close opened file ensuring all data saved physically to card
	bool rm(const char * file); //remove file
	unsigned int size(); //get opened file size in bytes
	unsigned int position(); //get current position within opened file
	bool seek(unsigned int pos); //go to pos in opened file
	bool read(char * dest, unsigned short size); //read size bytes from opened file and save them to dest
	bool write(const char * src, unsigned short size); //write size bytes from src to opened file
    }
}

#endif
