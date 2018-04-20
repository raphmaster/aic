#include <flash_hal.hpp>
#include <Arduino.h>

namespace hal
{
    namespace flash
    {
	const unsigned short pageSize = IFLASH0_PAGE_SIZE; //256 bytes page size
	const unsigned short maxPage = IFLASH0_NB_OF_PAGES + IFLASH1_NB_OF_PAGES; //2048 total pages
	const unsigned short fsp = 190; //firmware start page

	namespace
	{
	    unsigned int _buffer[pageSize / 4]; //page buffer

	    /*
	     * set flash wait state in flash mode register from ram function because register cannot be written during corresponding flash read
	     */
	    __attribute__ ((section(".ramfunc")))
	    void _setFMR(Efc * efc, unsigned int value)
	    {
		efc->EEFC_FMR = (value & EEFC_FMR_FWS_Msk); //change fmr
	    }
	}
	
	/*
	 * initialize memory
	 */
	bool init()
	{
	    return true; //nothing to do
	}

	/*
	 * jump to firmware code
	 */
	void bootjump()
	{
	    __disable_irq(); //ensure no interrupts will be called until we are fully jumped to firmware
	    unsigned int * vtor = (unsigned int *)(IFLASH0_ADDR + (fsp * pageSize)); //calculate firmware vector table start address
	    SCB->VTOR = (unsigned int)vtor; //relocate vector table to the one in the firmware
	    __set_MSP(*vtor); //set main stack pointer to the one found in vtor of the firmware
	    ((void(*)(void))vtor[1])(); //set the program counter to the reset handler in vtor via function call
	    //firmware should re-enable irqs
	}

	/*
	 * read one page of memory and return a pointer to it
	 */
	const unsigned int * read(unsigned short page)
	{
	    unsigned int * start = (unsigned int *)(IFLASH0_ADDR + (page * pageSize)); //calculate page start address
	    for (unsigned short x = 0; x < (pageSize / 4); ++x) //for each 32 bit word in page
	    {
		_buffer[x] = start[x]; //save data to buffer
	    }
	    return _buffer; //return buffer address
	}

	/*
	 * write one page of data at specified page
	 * notes:
	 * uses IAP function to send flash command to EEFC
	 * commands on a flash bank cannot be executed from the same flash bank
	 */
	bool write(const unsigned int * data, unsigned short page)
	{
	    unsigned int * start = (unsigned int *)(IFLASH0_ADDR + (page * pageSize)); //calculate page latch buffer start address
	    for (unsigned short x = 0; x < (pageSize / 4); ++x) //for each 32 bit word in page
	    {
		start[x] = data[x]; //save data to page latch buffer
	    }
	    Efc * efc = (page / IFLASH0_NB_OF_PAGES) ? EFC1 : EFC0; //calculate with which efc we need to work
	    typedef unsigned int (*iap)(unsigned int, unsigned int); //create alias for iap function type
	    iap fcr = (iap)*(unsigned int *)CHIP_FLASH_IAP_ADDRESS; //retrieve iap function address from nmi vector
	    unsigned int efcStatus; //store efc status
	    __disable_irq(); //ensure no flash read will be done during change to fmr or programming to the same flash bank
	    _setFMR(efc, EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE)); //set flash wait state to 6 when writing
	    efcStatus = fcr((efc == EFC0) ? 0 : 1, EEFC_FCR_FCMD(0x03) | EEFC_FCR_FARG(page % IFLASH0_NB_OF_PAGES) | EEFC_FCR_FKEY(0x5a)); //call iap function from rom to send command to efc
	    _setFMR(efc, EEFC_FMR_FWS(4)); //set flash wait state to 4 when reading
	    __enable_irq(); //re-enable irqs
	    if ((efcStatus & (EEFC_FSR_FRDY | EEFC_FSR_FCMDE | EEFC_FSR_FLOCKE)) != EEFC_FSR_FRDY) return false; //flash write error
	    return true;
	}
    }
}
