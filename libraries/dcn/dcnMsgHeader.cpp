#include  <dcnMsgHeader.hpp>

//default constructor
dcnMsgHeader::dcnMsgHeader() : _type(msgType::inChain), _sourceIp(0), _sourceId(0), _destinationIp(0), _destinationId(0), _id(msgId::undefined), _dataSize(0)
{
}

//in chain message header constructor
dcnMsgHeader::dcnMsgHeader(unsigned char sourceId, unsigned char destinationId, msgId id, unsigned short dataSize) : _type(msgType::inChain), _sourceIp(0), _sourceId(sourceId), _destinationIp(0), _destinationId(destinationId), _id(id), _dataSize(dataSize)
{
}

//out chain message header constructor
dcnMsgHeader::dcnMsgHeader(unsigned int sourceIp, unsigned char sourceId, unsigned int destinationIp, unsigned char destinationId, msgId id, unsigned short dataSize) : _type(msgType::outChain), _sourceIp(sourceIp), _sourceId(sourceId), _destinationIp(destinationIp), _destinationId(destinationId), _id(id), _dataSize(dataSize)
{
}

//recover message header from raw data
/*bool dcnMsgHeader::fromRawData(const unsigned char* data, msgType type)
{
}*/

/*
 * recover message header from raw data char by char
 * must be called several times to recover an entire header
 * data: new raw data to put in message header
 * return: true when header is completely recovered, otherwise false
 */
bool dcnMsgHeader::fromRawData(const char data)
{
    
}
