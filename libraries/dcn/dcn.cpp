#include <dcn.hpp>
#include <tasks.hpp>
#include <msg.hpp>
#include <uart_hal.hpp>
#include <ethernet_hal.hpp>

namespace dcn
{
    namespace
    {
	//check serial incoming data
	void _serialCheck()
	{
	    static unsigned char _data; //received data
	    static dcn::msg _msg; //dcn message
	    if (hal::uart::dataAvailable()) //data available for reading
	    {
		_data = hal::uart::read(); //read one byte
		if (_msg.fromRawData(_data)) //message received entirely
		{

		}
	    }
	}

	//check ethernet incoming data
	void _ethernetCheck()
	{

	}
    }

    /*
     * init dcn communication
     * Note: must be called before checking value of ethernet
     * baud: serial baud rate
     * mac: mac address pointer
     * ip: ip address pointer
     */
    bool init(unsigned int baud, const unsigned char * mac, const unsigned char * ip)
    {
	hal::uart::init(baud); //init serial port
	tasks::registerTask(_serialCheck); //register serial check task
	if (hal::ethernet::init(mac, ip)) //if ethernet init success
	{
	    tasks::registerTask(_ethernetCheck); //register ethernet check task
	}
	return true; //always success
    }
}
