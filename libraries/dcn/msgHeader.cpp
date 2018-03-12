#include  <msgHeader.hpp>

namespace dcn
{
//default constructor
msgHeader::msgHeader() : _type(msgType::inChain), _sourceIp(0), _sourceId(0), _destinationIp(0), _destinationId(0), _id(msgId::undefined), _dataSize(0), _steps(0)
{
}

//in chain message header constructor
msgHeader::msgHeader(unsigned char sourceId, unsigned char destinationId, msgId id, unsigned short dataSize) : _type(msgType::inChain), _sourceIp(0), _sourceId(sourceId), _destinationIp(0), _destinationId(destinationId), _id(id), _dataSize(dataSize), _steps(0)
{
}

//out chain message header constructor
msgHeader::msgHeader(unsigned int sourceIp, unsigned char sourceId, unsigned int destinationIp, unsigned char destinationId, msgId id, unsigned short dataSize) : _type(msgType::outChain), _sourceIp(sourceIp), _sourceId(sourceId), _destinationIp(destinationIp), _destinationId(destinationId), _id(id), _dataSize(dataSize), _steps(0)
{
}

/*
 * recover message header from raw data char by char
 * must be called several times to recover an entire header
 * data: new raw data to put in message header
 * return: true when header is completely recovered, otherwise false
 */
bool msgHeader::fromRawData(unsigned char data)
{
    if (_steps == 0) //if we received message header type
    {
	_type = msgType(data); //save type
	_steps++;
    }
    else if (_steps == 1)
    {
	if (_type == msgType::inChain) //received sourceid
	{
	    _sourceId = data; //save sourceid
	    _steps++;
	}
	else if (_type == msgType::outChain) //received byte of source ip
	{
	    ((unsigned char*)_sourceIp)[0] = data; //save byte 0 of ip
	    _steps++;
	}
    }
    else if (_steps == 2)
    {
	if (_type == msgType::inChain)
	{
	    _destinationId = data; //save destination id
	    _steps++;
	}
	else if (_type == msgType::outChain)
	{
	    ((unsigned char*)_sourceIp)[1] = data; //save byte 1 of ip
	    _steps++;
	}
    }
    else if (_steps == 3)
    {
	if (_type == msgType::inChain)
	{
	    _id = msgId(data); //save messsage id
	    _steps++;
	}
	else if (_type == msgType::outChain)
	{
	    ((unsigned char*)_sourceIp)[2] = data; //save byte 2 of ip
	    _steps++;
	}
    }
    else if (_steps == 4)
    {
	if (_type == msgType::inChain)
	{
	    ((unsigned char*)_dataSize)[0] = data; //save byte 0 of data size
	    _steps++;
	}
	else if (_type == msgType::outChain)
	{
	    ((unsigned char*)_sourceIp)[3] = data; //save byte 3 of ip
	    _steps++;
	}
    }
    else if (_steps == 5)
    {
	if (_type == msgType::inChain)
	{
	   ((unsigned char*)_dataSize)[1] = data; //save byte 1 of data size
	   _steps = 0; //reset steps
	   return true; //message header recovered
	}
	else if (_type == msgType::outChain)
	{
	    _sourceId = data; //save source id
	    _steps++;
	}
    }
    else if (_steps == 6)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_destinationIp)[0] = data; //save byte 0 of ip
	    _steps++;
	}
    }
    else if (_steps == 7)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_destinationIp)[1] = data; //save byte 1 of ip
	    _steps++;
	}
    }
    else if (_steps == 8)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_destinationIp)[2] = data; //save byte 2 of ip
	    _steps++;
	}
    }
    else if (_steps == 9)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_destinationIp)[3] = data; //save byte 3 of ip
	    _steps++;
	}
    }
    else if (_steps == 10)
    {
	if (_type == msgType::outChain)
	{
	    _destinationId = data; //save destination id
	    _steps++;
	}
    }
    else if (_steps == 11)
    {
	if (_type == msgType::outChain)
	{
	    _id = msgId(data); //save message id
	    _steps++;
	}
    }
    else if (_steps == 12)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_dataSize)[0] = data; //save byte 0 of data size
	    _steps++;
	}
    }
    else if (_steps == 13)
    {
	if (_type == msgType::outChain)
	{
	    ((unsigned char*)_dataSize)[1] = data; //save byte 1 of data size
	    _steps = 0; //reset steps
	    return true; //recovered message header
	}
    }
    return false; //if not returned earlier, header is not entirely recovered
}

unsigned short msgHeader::getDataSize() const
{
    return _dataSize;
}
} //namespace dcn
