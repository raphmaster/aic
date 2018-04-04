/*
 * Daisy Chain Network
 */

#ifndef dcn_hpp
#define dcn_hpp

#ifndef DCN_SERIAL_BAUD
#define DCN_SERIAL_BAUD 115200
#endif

#ifndef DCN_SERIAL_PORT
#define DCN_SERIAL_PORT Serial
#endif

#ifndef DCN_SD_SS
#define DCN_SD_SS 4
#endif

#ifndef DCN_ETHERNET_SS
#define DCN_ETHERNET_SS 10
#endif

#define ETHERNET_BIT 1

namespace dcn
{
    extern unsigned char ethernet; //true if an ethernet shield is attached, otherwise false
    void init(unsigned char* mac, unsigned char* ip); //init dcn communication
    namespace
    {
	void _serialCheck(); //check serial incoming data task function
	void _ethernetCheck(); //check ethernet incoming data task function
    }
}

#endif
