/*
 * Daisy Chain Network Message Header
 */

#ifndef dcnMsgHeader_hpp
#define dcnMsgHeader_hpp

enum class msgType : unsigned char {inChain, outChain};
enum class msgId : unsigned char {undefined};

class dcnMsgHeader
{
    public:
	dcnMsgHeader(); //default constructor
	dcnMsgHeader(unsigned char sourceId, unsigned char destinationId, msgId id, unsigned short dataSize); //in chain message header constructor
	dcnMsgHeader(unsigned int sourceIp, unsigned char sourceId, unsigned int destinationIp, unsigned char destinationId, msgId id, unsigned short dataSize); //out chain message header constructor
	//bool fromRawData(const unsigned char* data, msgType type); //recover message header from raw data
	bool fromRawData(const char data); //recover message header from raw data char by char
    private:
	msgType _type; //message header type
	unsigned int _sourceIp; //source ip
	unsigned char _sourceId; //source id
	unsigned int _destinationIp; //destination ip
	unsigned char _destinationId; //destination id
	msgId _id; //message id
	unsigned short _dataSize; //message data size
};

#endif
