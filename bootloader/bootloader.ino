/*
 * pin usage:
 *  spi header pins: 74,75,76
 *  sd card slave: 4
 *  ethernet slave: 10
 */

/*
 * TODO:
 * wait for upload from ethernet or serial, update and bootjump
 * reset update bit at the end of firmware update
 */

#include <sam3x8e_hal.hpp> //sam3x8e hardware abstraction layer
#include <hal.hpp>
#include <msg.hpp>
#include <tasks.hpp>
#include <dcn.hpp>

//#define moduleID 0 //module id in daisy chain uart configuration (all module in bootloader mode will have same id, message will be read by the first one in chain)
//#define UPDATE_PIN 2 //pin to wait for an firmware upload from serial or ethernet (drive pin 2 low during reset to avoid short circuit)
//#define STATUS_PIN 13 //pin to tell status of the device
#define UPDATE_BIN "update.bin" //firmware filename
#define UPDATE_LOG "update.log" //update log
//#define BOOT_LOG "boot.log" //boot log
//#define FIRMWARE_START_PAGE 190 //start page of firmware in flash 0
#define BAUD 115200 //serial communication speed
#define MAC { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } // the media access control (ethernet hardware) address of a module in bootloader mode
#define IP { 192, 168, 0, 110 } //the IP address of a module with ethernet in bootloader mode
/*#define firmwareUploadCmd 'F'
#define pageUploadCmd 'P'
#define continueCmd 'C'
#define endCmd 'E'
#define verifyCmd 'V'
#define bufferSize 256*/
//#define UPDATE_BIT 0
//#define sdss 4
//#define ethss 10

//File gfile; //generic file object reused several times because only one file can be opened at a time

//uint32_t blinkTime = 0; //used to store millis for blinking
//bool blinkStatus = HIGH; //used to store blink status

//uint8_t steps = 0; //used to control code execution
/*uint16_t pageSize; //stores reveiving page size
uint8_t buf[bufferSize]; //multi-use buffer
uint16_t bufIterator = 0; //buffer iterator*/

//set flash wait state in flash mode register from ram function because register cannot be written during corresponding flash read
/*__attribute__ ((section(".ramfunc")))
void setFMR(Efc* efc, uint32_t value)
{
  efc->EEFC_FMR = (value & EEFC_FMR_FWS_Msk); //change fmr
}*/

//perform firmware programming from file on sd card
bool sdUpdate()
{
  if (hal::sd::exists(UPDATE_BIN))
  {
    hal::sd::open(UPDATE_LOG, hal::sd::rw); //log starting update
    hal::sd::write(UPDATE_BIN, 10);
    hal::sd::write(" found! starting update...\n", 27);
    hal::sd::close();
    hal::sd::open(UPDATE_BIN, hal::sd::ro); //open firmware read-only
    unsigned int size = hal::sd::size(); //get firmware size
    unsigned char catchError = 0; //used to catch errors (0 means ok, others error codes)
    //if (size <= ((IFLASH0_NB_OF_PAGES + IFLASH1_NB_OF_PAGES - FIRMWARE_START_PAGE) * IFLASH0_PAGE_SIZE)) //check if size is ok
    if (size <= ((hal::flash::maxPage - hal::flash::fsp) * hal::flash::pageSize)) //check if size is ok
    {
//      unsigned int efcStatus; //store efc status before testing if an error has occured
      char page[hal::flash::pageSize]; //declare page buffer
//      unsigned int * buf = (unsigned int *)IFLASH0_ADDR; //Retrieve page latch buffer start address
//      Efc* efcIndex; //placeholder to calculate on which efc to write
//      unsigned int (*iap)(unsigned int, unsigned int); //delcare iap function
//      iap = (unsigned int (*)(unsigned int, unsigned int))(*(unsigned int *)CHIP_FLASH_IAP_ADDRESS); //retrieve iap function address from nmi vector in rom because efc command on flash cant be executed from the same flash bank
      //for (unsigned short x = 0; x < ((size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0)); ++x) //for each page to write
      for (unsigned short x = 0; x < ((size / hal::flash::pageSize) + ((size % hal::flash::pageSize) ? 1 : 0)); ++x) //for each page to write
      {
        hal::sd::read(page, hal::flash::pageSize); //read one page from firmware file and save it to buffer
        //for (unsigned char y = 0; y < (IFLASH0_PAGE_SIZE / 4); ++y) buf[y] = *(unsigned int *)(&page[y * 4]); //only 32-bit width copy authorized to latch buffer
        //efcIndex = (x / (IFLASH0_NB_OF_PAGES - FIRMWARE_START_PAGE -1)) ? EFC1 : EFC0; //calculate with which efc we need to work
        //__disable_irq(); //ensure no flash read will be done during change to fmr or programming to the same flash bank
        //setFMR(efcIndex, EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE)); //set flash wait state to 6 when writing
        //efcStatus = iap((efcIndex == EFC0) ? 0 : 1, EEFC_FCR_FCMD(0x03) | EEFC_FCR_FARG((efcIndex == EFC0) ? FIRMWARE_START_PAGE + x : x - (IFLASH0_NB_OF_PAGES - FIRMWARE_START_PAGE)) | EEFC_FCR_FKEY(0x5A)); //call iap function from rom to send command to efc
        //setFMR(efcIndex, EEFC_FMR_FWS(4)); //set flash wait state to 6 when writing
        //__enable_irq(); //re-enable irqs
        //if ((efcStatus & (EEFC_FSR_FRDY | EEFC_FSR_FCMDE | EEFC_FSR_FLOCKE)) != EEFC_FSR_FRDY)
        if (!hal::flash::write((unsigned int *)page, hal::flash::fsp + x)) //if flash write is not success
        {
          catchError = 2; //throw programming error
          break; //exit for loop
        }
      }
    }
    else catchError = 1; //update firmware file is too big
    hal::sd::close(); //close the firmware file
    hal::sd::open(UPDATE_LOG, hal::sd::rw); //open log to save information
    if (catchError) //if an error has been catched
    {
      if (catchError == 1) hal::sd::write("update firmware file is too big!\n", 33);
      else if (catchError == 2) hal::sd::write("flash programming error!\n", 25);
    }
    else //verify flash
    {
      hal::sd::write("flash programming done. verifying...\n", 38);
      hal::sd::close();
      hal::sd::open(UPDATE_BIN, hal::sd::ro); //open firmware file read-only
      char page[IFLASH0_PAGE_SIZE]; //declare page buffer
      const unsigned int * rpp; //used to store read page pointer
      //for (unsigned short x = 0; x < (size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0); ++x) //for each page to verify
      for (unsigned short x = 0; x < ((size / hal::flash::pageSize) + ((size % hal::flash::pageSize) ? 1 : 0)); ++x)  //for each page to verify
      {
        hal::sd::read(page, hal::flash::pageSize); //read one page from firmware file and save it to buffer
        //pageStart = (unsigned int *)(IFLASH0_ADDR + (FIRMWARE_START_PAGE * IFLASH0_PAGE_SIZE) + (x * IFLASH0_PAGE_SIZE)); //Retrieve page start address
        rpp = hal::flash::read(hal::flash::fsp + x); //save pointer to read page
        for (unsigned short y = 0; y < (hal::flash::pageSize / 4); ++y) //for each 32-bit word in page
        {
          //if (*(unsigned int *)(&page[y * 4]) != pageStart[y]) //if file and programmed values are different
          if (rpp[y] != ((unsigned int *)page)[y]) //if programmed and file values are different
          {
            catchError = 3; //throw flash verification error
            break; //exit for loop
          }
        }
        if (catchError) break; //exit for loop if an error has been catched
      }
      hal::sd::close(); //close the firmware file
      hal::sd::open(UPDATE_LOG, hal::sd::rw); //open log to save information
    }
    if (catchError) //if an error has been catched
    {
      if (catchError == 3) hal::sd::write("flash verifying error!\n", 23);
    }
    else
    {
      hal::sd::write("verify successfull!\n", 20);
      hal::sd::rm(UPDATE_BIN); //remove update firmware file
      hal::sd::write(UPDATE_BIN, 10);
      hal::sd::write(" deleted\n", 9);
    }
    hal::sd::close();
    if (catchError) return false; //if we encountered errors, return false
    else return true;
  }
  else return false; //no update file, update failed
}

//jump to the firmware
/*void bootJump()
{
  __disable_irq(); //ensure no interrupts will be called until we are fully jumped to firmware
  uint32_t* vtor = (uint32_t*)(IFLASH0_ADDR + (FIRMWARE_START_PAGE * IFLASH0_PAGE_SIZE)); //calculate firmware vector table start address
  SCB->VTOR = (uint32_t)vtor; //relocate vector table to the one in the firmware
  __set_MSP(*vtor); //set main stack pointer to the one found in vtor of the firmware
  ((void(*)(void))vtor[1])(); //set the program counter to the reset handler in vtor via function call
  //firmware should re-enable irqs
}*/

void setup()
{
  unsigned char mac[] = MAC; //default bootloader mac
  unsigned char ip[] = IP; //default bootloader ip
  hal::boot::init(BAUD, mac, ip); //init bootloader sequence
}

/*
 * check incoming ethernet data periodically
 */
void echeck()
{
  //static unsigned char _steps = 0; //control ethernet incoming data reading
  //static EthernetClient _client; //ethernet client object (only one client can be connected at a time on one server)
 // static unsigned char _data; //received data
 // static dcn::msg _msg; //dcn message
  //if (_steps == 0 && steps == 1) _steps = 1; //if ethernet reading do nothing and we need to check for data, begin checking data
  
  //all of the above is for reference only
  /*
  client = server.available(); //Check if a client is connected and has data available for reading
  if (client) //if a client is connected and has data available
  {
    data = client.read(); //read one byte from received data
    if (steps == 1) //if we receive a command
    {
      if (data == firmwareUploadCmd)
      {
        if (SD.exists(UPDATE_BIN)) SD.remove(UPDATE_BIN); //ensure no update file is already present
        gfile = SD.open(UPDATE_BIN, FILE_WRITE); //open file for writing
        client.write(data); //reply that we are ready too
        steps++; //wait for a page upload
      }
      else if (data == verifyCmd)
      {
        client.write(data); //reply that we are ready too
        steps = 5; //wait for specification of file to verify
      }
    }
    else if (steps == 2) //if we are receiving upload commands
    {
      if (data == pageUploadCmd) //receive page upload command
      {
        steps++; //wait for page size
      }
      else if (data == endCmd) //receive firmware upload end command
      {
        gfile.close(); //close file and ensure all data is written to sd
        client.write(endCmd); //firmware upload ended successfully
        steps = 1; //wait for other command (verify or update)
      }
    }
    else if (steps == 3) //if we are receiving page size
    {
      if (receive(data, 2)) //if we received entire page size
      {
        pageSize = *(uint16_t *)buf; //store page size
        steps++; //wait for page data
      }
    }
    else if (steps == 4) // if we are receiving page data
    {
      if (receive(data, pageSize)) //if we received entire page data
      {
        gfile.write(buf, pageSize); //write data to file
        client.write(continueCmd); //send signal to continue
        steps = 2; //ready to receive next page
      }
    }
    else if (steps == 5) //wait for file type to verify
    {
      if (data == firmwareUploadCmd)
      {
        gfile = SD.open(UPDATE_BIN); //open firmware file read-only
        client.write(data); //reply that we are ready too
        steps++; //wait for page
      }
    }
    else if (steps == 6) //wait for a page upload command
    {
      if (data == pageUploadCmd)
      {
        steps++; //wait for page size
      }
      else if (data == endCmd)
      {
        gfile.close(); //close file and ensure all data is written to sd
        client.write(endCmd); //firmware verify ended successfully
        steps = 1; //wait for other command (update)
      }
    }
    else if (steps == 7) //wait for page size
    {
      if (receive(data, 2)) //if we received entire page size
      {
        pageSize = *(uint16_t *)buf; //store page size
        steps++; //wait for page data
      }
    }
    else if (steps == 8) //wait for page data
    {
      if (receive(data, pageSize)) //if we received entire page data
      {
        //check if pages are different
        uint8_t buf2[bufferSize]; //temp buffer to read page
        gfile.read(buf2, pageSize); //read page
        uint8_t diff = 0; //signal difference between pages
        for (uint16_t x = 0; x < pageSize; x++) //for each byte in page
        {
          if (buf[x] != buf2[x]) //if a byte is different
          {
            diff = 1; //page are different
            break; //exit for loop
          }
        }
        if (diff) //if pages are different
        {
          gfile.close(); //close file
          client.write(endCmd); //send end command
          steps = 1; //wait for other commands
        }
        else
        {
          client.write(continueCmd); //send signal to continue
          steps = 6; //ready to receive next page
        }
      }
    }
    
  }*/
}


void loop()
{
  tasks::doTasks(); //execute registered tasks
}
