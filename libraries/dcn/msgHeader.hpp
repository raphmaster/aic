/*
 * Daisy Chain Network Message Header
 */

#ifndef msgHeader_hpp
#define msgHeader_hpp

namespace dcn
{
enum class msgType : unsigned char {inChain, outChain};
enum class msgId : unsigned char {undefined};

class msgHeader
{
    public:
	msgHeader(); //default constructor
	msgHeader(unsigned char sourceId, unsigned char destinationId, msgId id, unsigned short dataSize); //in chain message header constructor
	msgHeader(unsigned int sourceIp, unsigned char sourceId, unsigned int destinationIp, unsigned char destinationId, msgId id, unsigned short dataSize); //out chain message header constructor
	bool fromRawData(unsigned char data); //recover message header from raw data char by char
	unsigned short getDataSize() const;
    private:
	msgType _type; //message header type
	unsigned int _sourceIp; //source ip
	unsigned char _sourceId; //source id
	unsigned int _destinationIp; //destination ip
	unsigned char _destinationId; //destination id
	msgId _id; //message id
	unsigned short _dataSize; //message data size
	unsigned char _steps; //fromRawData control steps
}; //msgHeader
} //namespace dcn

#endif
