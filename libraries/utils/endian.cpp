/*
 * little or big endian operations
 */

#include <endian.hpp>

namespace endian
{
    //reverse byte order of 16 bits value
    short rev16(short value)
    {
	return ((value >> 8) & 0xFF) | ((value << 8) & 0xFF00);
    }

    //reverse byte order of 32 bits value
    int rev32(int value)
    {
	return ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
    }

#if defined(LITTLE_ENDIAN) || defined(BIG_ENDIAN)
    //convert endianess for 16 bits network value
    short nrev16(short value)
    {
#ifdef LITTLE_ENDIAN
	return rev16(value);
#else
	return value;
#endif
    }

    //convert endianess for 32 bits network value
    int nrev32(int value)
    {
#ifdef LITTLE_ENDIAN
	return rev32(value);
#else
	return value;
#endif
    }
#endif
}
