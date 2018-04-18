#include <sd_hal.hpp>
#include <SD.h>

namespace hal
{
    namespace sd
    {
	namespace
	{
	    File _file; //currently opened file, only one can be open at a time
	}

	/*
	 * initialize sd card
	 */
	bool init()
	{
	    return SD.begin(4);
	}

	/*
	 * check if file exists
	 */
	bool exists(const char * file)
	{
	    return SD.exists(file);
	}

	/*
	 * create directory
	 */
	bool mkdir(const char * dir)
	{
	    return SD.mkdir(dir);
	}

	/*
	 * remove directory
	 */
	bool rmdir(const char * dir)
	{
	    return SD.rmdir(dir);
	}

	/*
	 * open file in specified mode
	 */
	bool open(const char * file, unsigned char mode)
	{
	    switch (mode)
	    {
		case ro:
		    mode = FILE_READ;
		    break;
		case rw:
		    mode = FILE_WRITE;
		    break;
		default:
		    mode = FILE_READ;
		    break;
	    }
	    _file = SD.open(file, mode); //open file and save object
	    return _file; //evaluates to true if file has been successfully opened
	}

	/*
	 * close opened file
	 */
	bool close()
	{
	    _file.close();
	    return true;
	}

	/*
	 * remove file
	 */
	bool rm(const char * file)
	{
	    return SD.remove(file);
	}

	/*
	 * get opened file size in bytes
	 */
	unsigned int size()
	{
	    return _file.size();
	}

	/*
	 * get current position within opened file
	 */
	unsigned int position()
	{
	    return _file.position();
	}

	/*
	 * go to pos in opened file
	 */
	bool seek(unsigned int pos)
	{
	    return _file.seek(pos);
	}

	/*
	 * read size bytes from opened file and save them to dest
	 */
	bool read(unsigned char * dest, unsigned short size)
	{
	    if (_file.read(dest, size) <= 0) return false;
	    return true;
	}

	/*
	 * write size bytes from src to opened file
	 */
	bool write(const unsigned char * src, unsigned short size)
	{
	    if (_file.write(src, size) == size) return true;
	    return false;
	}
    }
}
