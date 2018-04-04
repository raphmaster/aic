/*
 * General Utility Algorithms
 */

#ifndef algo_hpp
#define algo_hpp

namespace algo
{
    /*
     * find first occurence of value from <from> to <to> index in array
     * value: value to find in array
     * from: start search at from index
     * to: end search at to index
     * array: pointer to the start of the array
     * return: index of first matching element, otherwise to index
     */
    template <class arrayType>
	unsigned short find(const arrayType& value, unsigned short from, unsigned short to, const arrayType* array)
	{
	    for (;from <= to; from++) //for each element
	    {
		if (array[from] == value) return from; //if element value matches, return index
	    }
	    return from; //if no element found, return to + 1
	}
}

#endif
