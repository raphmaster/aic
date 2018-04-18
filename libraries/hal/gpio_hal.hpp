/*
 * general purpose inputs/outputs hardware abstraction layer
 */

#ifndef gpio_hal_hpp
#define gpio_hal_hpp

namespace hal
{
    namespace gpio //general purpose inputs/outputs
    {
	enum types : unsigned char {undefined, digital, analog}; //pin types
	enum modes : unsigned char {input=1, input_pullup, output}; //pin modes
	//bool read(unsigned short pin); //get pin state
        //void write(unsigned short pin, bool value); //set pin state
	class pinConfig //pin configuration
	{
	    public:
		unsigned char type; //pin type
		unsigned char mode; //pin mode
	};
	class pin //pin description
	{
	    public:
		pinConfig config; //pin configuration
	};
	bool init(); //initialize inputs/outputs
	void config(unsigned short pin, pinConfig value); //set pin configuration
	unsigned int read(unsigned short pin); //get pin value
	void write(unsigned short pin, unsigned int value); //set pin value
    }
}

#endif
