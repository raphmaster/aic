/*
 * Daisy Chain Network
 */

#ifndef dcn_hpp
#define dcn_hpp

namespace dcn
{
    extern unsigned char ethernet; //true if an ethernet shield is attached, otherwise false
    void init(unsigned int baud, const unsigned char * mac, const unsigned char * ip); //init dcn communication
    namespace
    {
	void _serialCheck(); //check serial incoming data task function
	void _ethernetCheck(); //check ethernet incoming data task function
    }
}

#endif
