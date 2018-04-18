#include <dcn.hpp>
#include <tasks.hpp>
#include <msg.hpp>
#include <gpbr_hal.hpp>
#include <uart_hal.hpp>
#include <ethernet_hal.hpp>

namespace dcn
{
    unsigned char ethernet = 0; //having ethernet
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
    void init(unsigned int baud, const unsigned char * mac, const unsigned char * ip)
    {
	ethernet = (hal::gpbr::read(0) & (0x1 << hal::gpbr::bit::ethernet)) >> hal::gpbr::bit::ethernet; //recover ethernet configuration
	hal::uart::init(baud); //init serial port
	tasks::registerTask(_serialCheck); //register serial check task
	if (ethernet)
	{
	    hal::ethernet::init(mac, ip); //initialize ethernet communication
	    tasks::registerTask(_ethernetCheck); //register ethernet check task
	}
    }
}
