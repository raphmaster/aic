#include <ethernet_hal.hpp>
#include <gpbr_hal.hpp>
#include <sam3x8e_hal.hpp>
#include <Ethernet.h>

namespace hal
{
    namespace ethernet
    {
	namespace
	{
	    EthernetServer _server(1025); //ethernet server on port 1025
	    EthernetClient _client; //ethernet client object (only one client can be connected at a time on one server)
	}

	/*
	* initialize ethernet communication
	* mac: pointer to 6 chars array
	* ip: pointer to 4 chars array
	*/
	bool init(const unsigned char * mac, const unsigned char * ip)
	{
	    if (gpbr::read(0) & gpbr::ethernet) //if a ethernet shield is present
	    {
		unsigned char _mac[6];
		memcpy(_mac, mac, 6);
		unsigned char _ip[4];
		memcpy(_ip, ip, 4);
		Ethernet.begin(_mac, _ip); //gateway and dns server defaults to ip argument with last byte replaced by 1 and subnet to 255.255.255.0
		_server.begin(); //start listening for clients
		return true; //init success
	    }
	    return false; //init failure, no ethernet shield
	}

	/*
	* return number of bytes available for reading
	*/
	unsigned short dataAvailable()
	{
	    _client = _server.available(); //check if a client is connected and has data available for reading
	    if (_client) return _client.available(); //return number of bytes available for reading
	    return 0;
	}

	/*
	* read one byte from available data
	*/
	char read()
	{
	    return _client.read();
	}

	/*
	* send length bytes from buffer to the same client we just received data
	*/
	unsigned short write(const char * buffer, unsigned short length)
	{
	    return _client.write(buffer, length);
	}
    }
}
