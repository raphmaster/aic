/*
 * bootloader hardware abstraction layer
 */

#ifndef boot_hal_hpp
#define boot_hal_hpp

namespace hal
{
    namespace boot
    {
	const char bootlog[] = "boot.log";
	const char updatelog[] = "update.log";
	const char updatebin[] = "update.bin";
	bool init(unsigned int baud, const unsigned char * mac, const unsigned char * ip); //initialize bootloader
	bool sdUpdate(); //sd card firmware update
    }
}

#endif
