#include <gpbr_hal.hpp>
#include <Arduino.h>

namespace hal
{
    namespace gpbr
    {
	/*
         * get gpbr at index
	 */
        unsigned int read(char index)
	{
	    return GPBR->SYS_GPBR[index];
	}

	/*
	* set gpbr at index with value
	*/
	void write(char index, unsigned int value)
	{
	    GPBR->SYS_GPBR[index] = value;
	}
    }
}
