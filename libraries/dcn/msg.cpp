#include <msg.hpp>

namespace dcn
{
    //default constructor
    msg::msg() : _steps(0), _cont(_buffer, 0)
    {
    }

    /*
     * recover message from raw data char by char
     * must be called several times to recover an entire message
     * data: new raw data to put in message
     * return: true when message is recovered entirely, otherwise false
     */
    bool msg::fromRawData(unsigned char data)
    {
	if (_steps == 0) //receiving header
	{
	    if (_header.fromRawData(data)) //header received entirely
	    {
		_cont.setLength(_header.getDataSize()); //set buffer length to message data size
		_steps++;
	    }
	}
	else if (_steps == 1) //receiving message data
	{
	    if (_cont.store(data)) //all data received
	    {
		_steps = 0; //reset steps
		return true;
	    }
	}
	return false;
    }
} //namespace dcn
