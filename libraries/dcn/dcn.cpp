#include <dcn.hpp>
#include <tasks.hpp>
//#include <Arduino.h>
//#include <Ethernet.h>
#include <hal.hpp>
#include <msg.hpp>

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
	    if (hal::serial->dataAvailable()) //data available for reading
	    {
		_data = hal::serial->read(); //read one byte
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
     * mac: mac address pointer
     * ip: ip address pointer
     */
    void init(unsigned int baud, unsigned char* mac, unsigned char* ip)
    {
	ethernet = (hal::backupReg->get(0) & (0x1 << hal::gpbr::bit::ethernet)) >> hal::gpbr::bit::ethernet; //recover ethernet configuration
	hal::serial->init(baud); //init serial port
	tasks::registerTask(_serialCheck); //register serial check task
	if (ethernet)
	{
	    hal::eth->init(mac, ip); //initialize ethernet communication
	    tasks::registerTask(_ethernetCheck); //register ethernet check task
	}
    }
}
