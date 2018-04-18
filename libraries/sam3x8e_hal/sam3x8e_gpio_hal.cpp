#include <gpio_hal.hpp>
#include <Arduino.h>

namespace hal
{
    namespace gpio
    {
	namespace
	{
	    pin _pins[PINS_COUNT]; //pins desciption array
	}

	/*
	 * initialize inputs/outputs
	 */
	bool init()
	{
	    analogReadResolution(12);
	    analogWriteResolution(12);
	    return true;
	}

	/*
	 * set pin configuration
	 */
	void config(unsigned short pin, pinConfig value)
	{
	    _pins[pin].config = value; //save configuration
	    switch (value.mode)
	    {
		case input:
		    value.mode = INPUT;
		    break;
		case input_pullup:
		    value.mode = INPUT_PULLUP;
		    break;
		case output:
		    value.mode = OUTPUT;
		    break;
		default:
		    value.mode = 0xFF; //invalid mode
		    break;
	    }
	    pinMode(pin, value.mode); //non valid values will do nothing
	}
	
	/*
	 * get pin value
	 */
	unsigned int read(unsigned short pin)
	{
	    switch (_pins[pin].config.type)
	    {
		case digital:
		    return digitalRead(pin);
		    break;
		case analog:
		    return analogRead(pin);
		    break;
	    } 
	    return 0; //pin is not digital or analog, cant do anything
	}

	/*
	 * set pin value
	 */
	void write(unsigned short pin, unsigned int value)
	{
	    switch (_pins[pin].config.type)
	    {
		case digital:
		    digitalWrite(pin, value);
		    break;
		case analog:
		    analogWrite(pin, value);
		    break;
	    }
	}
    }
}
