/*
 * Daisy Chain Network Utility Functions
 */

#ifndef utils_hpp
#define utils_hpp

namespace dcn
{
    class cont
    {
	public:
	    cont(unsigned char* array, unsigned short length); //constructs an array container of length char
	    bool store(unsigned char data); //store data in array
	    unsigned short getLength() const;
	    void setLength(unsigned short length);
	private:
	    unsigned char* _array; //array pointer
	    unsigned short _length; //array length
	    unsigned short _it; //array iterator
    };
} //namespace dcn

#endif
