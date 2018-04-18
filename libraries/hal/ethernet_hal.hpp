/*
 * ethernet hardware abstraction layer
 */

#ifndef ethernet_hal_hpp
#define ethernet_hal_hpp

namespace hal
{
    namespace ethernet
    {
	char init(const unsigned char * mac, const unsigned char * ip); //initialize ethernet communication
	unsigned short dataAvailable(); //return number of bytes available for reading
	char read(); //read one byte from available data
	unsigned short write(const char * buffer, unsigned short length); //send length bytes from buffer
    }
}
#endif
