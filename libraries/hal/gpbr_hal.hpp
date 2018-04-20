/*
 * general purpose backup register hardware abstraction layer
 */

#ifndef gpbr_hal_hpp
#define gpbr_hal_hpp

namespace hal
{
    namespace gpbr //general purpose backup register
    {
	enum bit : unsigned int {update=1}; //bits in gpbr0
	unsigned int read(char index); //get gpbr at index
	void write(char index, unsigned int value); //set gpbr at index with value
    }
}

#endif
