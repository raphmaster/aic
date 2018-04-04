/*
 * Daisy Chain Network Message
 */

#ifndef msg_hpp
#define msg_hpp

#include <msgHeader.hpp>
#include <cont.hpp>

namespace dcn
{
    class msg
    {
	public:
	    msg(); //default constructor
	    bool fromRawData(unsigned char data); //recover message from raw data char by char
	    static const unsigned short maxDataSize = 256; //maximum number of bytes for message data
	private:
	    msgHeader _header; //message header
	    unsigned char _buffer[maxDataSize]; //internal data buffer
	    unsigned char _steps; //fromRawData control steps
	    utils::cont _cont; //data buffer container
    };
}

#endif
