/*
 * hardware abstraction layer
 */

#ifndef hal_hpp
#define hal_hpp

namespace hal
{
    //void digitalMode(unsigned short pin. unsigned char mode); //generic digital mode
    void digitalWrite(unsigned short pin, bool value); //generic digital write

    //generic general purpose backup register
    class gpbr
    {
	public:
	    gpbr(); //default constructor
	    virtual unsigned int get(char index); //get gpbr at index
	    virtual void set(char index, unsigned int value); //set gpbr at index with value
	    enum bit : char {update, ethernet}; //bits in gpbr
    };

    //generic uart
    class uart
    {
	public:
	    uart(); //default constructor
	    virtual void init(unsigned int baud); //initialize serial communication
	    virtual unsigned short dataAvailable(); //return number of bytes available for reading
	    virtual char read(); //read one byte from available data
	    virtual unsigned short write(char * buffer, unsigned short length); //send length byte in buffer
    };

    //generic ethernet communication class
    class ethernet
    {
	public:
	    ethernet(); //default constructor
	    virtual void init(unsigned char * mac, unsigned char * ip); //initialize ethernet communication
	    virtual unsigned short dataAvailable(); //return number of bytes available for reading
	    virtual char read(); //read one byte from available data
	    virtual unsigned short write(char * buffer, unsigned short length); //send length bytes in buffer
    };

    extern gpbr * backupReg; //generic general purpose backup register
    extern uart * serial; //generic serial communication object
    extern ethernet * eth; //generic ethernet communication
}
#endif
