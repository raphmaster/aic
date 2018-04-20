#include <boot_hal.hpp>
#include <gpio_hal.hpp>
#include <gpbr_hal.hpp>
#include <sam3x8e_hal.hpp>
#include <sd_hal.hpp>
#include <flash_hal.hpp>
#include <dcn.hpp>
#include <tasks.hpp>
#include <Arduino.h> //to be deleted when time hal implemented

#define UPDATE_PIN 2 //pin to wait for an firmware upload from serial or ethernet (drive pin 2 low during reset to avoid short circuit)
#define STATUS_PIN 13 //pin to tell status of the device
#define SDSS 4 //sd card spi slave select
#define ETHSS 10 //ethernet w5100 spi slave select

namespace hal
{
    namespace boot
    {
	namespace
	{
	    unsigned int blinkTime = 0; //used to store millis for blinking
	    bool blinkStatus = true; //used to store blink status

	    /*
	     * bootloader loop
	     */
	    void _loop()
	    {
		if ((millis() - blinkTime) >= 1000) //toggle led every 1000 ms to signal user we are waiting for upload
		{
		    gpio::write(STATUS_PIN, !blinkStatus); //toggle led status
		    blinkStatus = !blinkStatus; //save new status
		    blinkTime = millis(); //save new time
		}
	    }
	}

	/*
	 * initialize bootloader
	 */
	bool init(unsigned int baud, const unsigned char * mac, const unsigned char * ip)
	{
	    gpio::config(UPDATE_PIN, gpio::pinConfig{gpio::digital, gpio::input_pullup}); //configure pin to update from serial or ethernet
	    gpio::config(STATUS_PIN, gpio::pinConfig{gpio::digital, gpio::output}); //config status pin
	    gpio::write(STATUS_PIN, true); //say that we are in bootloader mode
	    gpio::config(ETHSS, gpio::pinConfig{gpio::digital, gpio::output}); //config ethernet pin
	    gpio::write(ETHSS, true); //we dont wanna talk to w5100 first 
	    bool ethEnabled = gpbr::read(0) & gpbr::ethernet; //check if an ethernet shield is present
	    bool sdEnabled = false;
	    if (ethEnabled) //if we have an ethernet shield
	    {
		sdEnabled = sd::init(); //save sd initialization result
		if (sdEnabled) //if sd init successfull
		{
		    sd::open(bootlog, sd::rw); //open or create boot log
		    sd::write("sd init success\n", 16); //log sd init success
		    sd::close(); //close log file because sdUpdate will open update log
		    if (sdUpdate()) flash::bootjump(); //do the flash programming, if success jump to firmware
		    sd::open(updatelog, sd::rw); //re-open log file
		}
		//else sd init not success, cant continue, find a way to tell user (ex: send message periodically in loop)
	    }
	    if (!gpio::read(UPDATE_PIN)) //if update pin is driven low
	    {
		if (sdEnabled)
		{
		    sd::write("update pin driven low. waiting for an upload from serial or ethernet...\n", 72);
		    sd::close();
		}
	    }
	    else if (gpbr::read(0) & gpbr::update) //if firmware want to be updated
	    {
		if (sdEnabled)
		{
		    sd::write("gpbr bit set, waiting for an upload from serial or ethernet...\n", 63);
		    sd::close();
		}
	    }
	    else //if no update is needed
	    {
		if (sdEnabled) sd::close();
		flash::bootjump(); //jump to firmware
	    }
	    dcn::init(baud, mac, ip); //init dcn communication
	    tasks::registerTask(_loop); //register bootloader task loop
	    return true;
	}
    }
}
