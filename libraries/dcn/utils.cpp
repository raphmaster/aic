#include <utils.hpp>

namespace dcn
{
    /*
     * constructs an array container of length char
     * array: pointer to the start of the array
     * length: maximum number of element in array
     */
    cont::cont(unsigned char* array, unsigned short length) : _array(array), _length(length), _it(0)
    {
    }
    
    /*
     * store data in array
     * return: true when end of array is reached, otherwise false
     */
    bool cont::store(unsigned char data)
    {
	_array[_it] = data; //save data
	_it++; //move to next element
	if (_it >= _length) //if reached end of array
	{
	    _it = 0; //reset iterator
	    return true;
	}
	return false;
    }

    unsigned short cont::getLength() const
    {
	return _length;
    }

    void cont::setLength(unsigned short length)
    {
	_length = length;
    }
} //namespace dcn
