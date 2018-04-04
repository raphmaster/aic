#include <dcn.hpp>
#include <tasks.hpp>
#include <Arduino.h>
#include <Ethernet.h>
#include <msg.hpp>

namespace dcn
{
    unsigned char ethernet = 0; //having ethernet
    namespace
    {
	EthernetServer _server(1025); //Initialize ethernet server on port 1025

	//check serial incoming data
	void _serialCheck()
	{
	    static unsigned char _data; //received data
	    static dcn::msg _msg; //dcn message
	    if (DCN_SERIAL_PORT.available()) //data available for reading
	    {
		_data = DCN_SERIAL_PORT.read(); //read one byte
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
    void init(unsigned char* mac, unsigned char* ip)
    {
	ethernet = (GPBR->SYS_GPBR[0] & (0x1 << ETHERNET_BIT)) >> ETHERNET_BIT; //recover ethernet configuration
	DCN_SERIAL_PORT.begin(DCN_SERIAL_BAUD); //init serial port
	tasks::registerTask(_serialCheck); //register serial check task
	if (ethernet)
	{
	    digitalWrite(DCN_SD_SS, HIGH);
	    digitalWrite(DCN_ETHERNET_SS, LOW); //we finished talking to sd card and need to talk to w5100
	    Ethernet.begin(mac, ip); //gateway and dns server defaults to ip argument with last byte replaced by 1 and subnet to 255.255.255.0
	    _server.begin(); //start listening for clients
	    tasks::registerTask(_ethernetCheck); //register ethernet check task
	}
    }
}
