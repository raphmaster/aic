#include <uart_hal.hpp>
#include <Arduino.h>

#define PORT Serial

namespace hal
{
    namespace uart
    {
	/*
	* initialize serial communication at baud rate
	*/
	char init(unsigned int baud)
	{
	    PORT.begin(baud);
	    return 1;
	}

	/*
	* return number of bytes available for reading
	*/
	unsigned short dataAvailable()
	{
	    return PORT.available();
	}

	/*
	* read one byte from available data
	*/
	char read()
	{
	    return PORT.read();
	}

	/*
	* send length byte from buffer
	*/
	unsigned short write(const char * buffer, unsigned short length)
	{
	    return PORT.write(buffer, length);
	}
    }
}

#undef PORT
