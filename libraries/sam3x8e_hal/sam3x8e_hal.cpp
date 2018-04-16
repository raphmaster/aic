/*
 * sam3x8e hardware abstraction layer
 */

#include <sam3x8e_hal.hpp>

#define PORT Serial

namespace hal
{ 
    /*
     * digital write
     */
    void digitalWrite(unsigned short pin, bool value)
    {
	::digitalWrite(pin, value);
    }

    /*
     * default constructor
     */
    gpbr::gpbr()
    {
    }

    /*
     * get gpbr at index
     */
    unsigned int gpbr::get(char index)
    {
	return GPBR->SYS_GPBR[index];
    }

    /*
     * set gpbr at index with value
     */
    void gpbr::set(char index, unsigned int value)
    {
	GPBR->SYS_GPBR[index] = value;
    }
    
    /*
     * default constructor
     */
    uart::uart()
    {
    }

    /*
     * initialize serial communication
     */
    void uart::init(unsigned int baud)
    {
        PORT.begin(baud);
    }

    /*
     * return number of bytes available for reading
     */
    unsigned short uart::dataAvailable()
    {
	return PORT.available();
    }

    /*
     * read one byte from available data
     */
    char uart::read()
    {
	return PORT.read();
    }

    /*
     * send length byte in buffer
     */
    unsigned short uart::write(char * buffer, unsigned short length)
    {
	return PORT.write(buffer, length);
    }

    /*
     * default constructor
     */
    ethernet::ethernet()
    {
    }

    /*
     * initialize ethernet communication
     */
    void ethernet::init(unsigned char * mac, unsigned char * ip)
    {
    }

    /*
     * return number of bytes available for reading
     */
    unsigned short ethernet::dataAvailable()
    {
	return 0;
    }

    /*
     * read one byte from available data
     */
    char ethernet::read()
    {
	return 0;
    }

    /*
     * send length bytes in buffer
     */
    unsigned short ethernet::write(char * buffer, unsigned short length)
    {
	return 0;
    }

    /*
     * constructor
     * initialize server on port port
     */
    samEthernet::samEthernet(unsigned short port) : _server(port)
    {
    }

    /*
     * initialize ethernet communication
     */
    void samEthernet::init(unsigned char * mac, unsigned char * ip)
    {
	//::digitalWrite(pin::sdss, HIGH);
	//::digitalWrite(pin::ethss, LOW); //we finished talking to sd card and need to talk to w5100
	Ethernet.begin(mac, ip); //gateway and dns server defaults to ip argument with last byte replaced by 1 and subnet to 255.255.255.0
	_server.begin(); //start listening for clients
    }

    /*
     * return number of bytes available for reading
     */
    unsigned short samEthernet::dataAvailable()
    {
	_client = _server.available(); //check if a client is connected and has data available for reading
	if (_client) return _client.available(); //return number of bytes available for reading
	return 0;
    }

    /*
     * read one byte from available data
     */
    char samEthernet::read()
    {
	return _client.read();
    }

    /*
     * send length bytes in buffer to the same client we just received data
     */
    unsigned short samEthernet::write(char * buffer, unsigned short length)
    {
	return _client.write(buffer, length);
    }

    namespace
    {
	gpbr _backupReg; //create general purpose backup register
	uart _serial; //create serial communication object
	samEthernet _eth(1025); //create custom ethernet communication server on port 1025
    }
    
    gpbr * backupReg = &_backupReg; //give access to generic gpbr
    uart * serial = &_serial; //give access to generic serial communication
    ethernet * eth = &_eth; //give access to generic ethernet communication
}
#undef PORT
