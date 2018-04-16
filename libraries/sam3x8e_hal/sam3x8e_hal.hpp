/*
 * sam3x8e hardware abstraction layer
 */

#ifndef sam3x8e_hal_hpp
#define sam3x8e_hal_hpp

#include <hal.hpp>
#include <Arduino.h>
#include <Ethernet.h>

namespace hal
{
    //pin definitions
    enum pin : char {sdss=4, ethss=10};

    class samEthernet : public ethernet
    {
	public:
	    samEthernet(unsigned short port); //constructor
	    void init(unsigned char * mac, unsigned char * ip); //initialize ethernet communication
	    unsigned short dataAvailable(); //return number of bytes available for reading
	    char read(); //read one byte from available data
	    unsigned short write(char * buffer, unsigned short length); //send length bytes in buffer to the same client we just received data
	protected:
	    EthernetServer _server; //ethernet server
	    EthernetClient _client; //ethernet client object (only one client can be connected at a time on one server)
    };
}

#endif
