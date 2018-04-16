/*
 * little and big endian operations
 */

#ifndef endian_hpp
#define endian_hpp

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#warning "LITTLE_ENDIAN or BIG_ENDIAN not defined. One of them must be defined to use htons, htoni, ntohs or ntohi."
#endif

namespace endian
{
    short rev16(short value); //reverse byte order of 16 bits value
    int rev32(int value); //reverse byte order of 32 bits value
#if defined(LITTLE_ENDIAN) || defined(BIG_ENDIAN)
#if defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
#error "Only one of LITTLE_ENDIAN or BIG_ENDIAN must be defined."
#endif
    short nrev16(short value); //convert endianess for 16 bits network value
    int nrev32(int value); //convert endianess for 32 bits network value
#endif
}

#endif
