/*
 * universal asynchronous receiver transmitter hardware abstraction layer
 */

#ifndef uart_hal_hpp
#define uart_hal_hpp

namespace hal
{
    namespace uart //universal asynchronous receiver transmitter
    {
	char init(unsigned int baud); //initialize serial communication at baud rate
	unsigned short dataAvailable(); //return number of bytes available for reading
	char read(); //read one byte from available data
	unsigned short write(const char * buffer, unsigned short length); //send length byte from buffer
    }
}

#endif
