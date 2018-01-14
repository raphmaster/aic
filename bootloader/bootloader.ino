/*
 * pin usage:
 *  spi header pins: 74,75,76
 *  sd card slave: 10
 *  ethernet slave: 4
 */

#include <SPI.h> //ethernet and sd uses spi
#include <SD.h> //to use sd card
#include <Ethernet.h> /to use wiznet ethernet shield

#define updatePin 2 //pin to wait for an firmware upload from serial or ethernet (drive pin 2 low during reset to avoid short circuit)
#define statusPin 13 //pin to tell status of the device
#define updateBin "update.bin" //firmware filename
#define updateLog "update.log" //update log
#define firmwareStartPage 150 //start page of firmware in flash 0
#define mac { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } // the media access control (ethernet hardware) address for the shield
#define ip { 192, 168, 0, 110 } //the IP address for the shield:

EthernetServer server(80); //Initialize ethernet server on port 80
bool waitUpload = false; //tell to the main loop to listen for upload from serial or ethernet

//sd update --TODO: jump to firmware
//Possible bugs: reassigning new file object to previously closed one, iap function
void sdUpdate()
{
  if (SD.exists(updateBin))
  {
    File log = SD.open(updateLog, FILE_WRITE); //log starting update
    log.print(updateBin);
    log.println(" found! Starting update...");
    log.close();
    File update = SD.open(updateBin); //open firmware read-only
    uint32_t size = update.size(); //get firmware size
    uint8_t catchError = 0; //used to catch errors (0 means ok, others error codes)
    if (size <= ((IFLASH0_NB_OF_PAGES + IFLASH1_NB_OF_PAGES - firmwareStartPage) * IFLASH0_PAGE_SIZE)) //check if size is ok
    {
      uint32_t efcStatus; //store efc status before testing if an error has occured
      uint8_t page[IFLASH0_PAGE_SIZE]; //declare page buffer
      uint32_t* buf = (uint32_t*)IFLASH0_ADDR; //Retrieve page latch buffer start address
      Efc* efcIndex; //placeholder to calculate on which efc to write
      uint32_t (*iap)(uint32_t, uint32_t); //delcare iap function
      iap = (uint32_t (*)(uint32_t, uint32_t))(*(uint32_t*)(IROM_ADDR + 8)); //retrieve iap function address from nmi vector in rom because efc command on flash cant be executed from the same flash bank
      for (uint16_t x = 0; x < (size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0); ++x) //for each page to write
      {
        update.read(page, IFLASH0_PAGE_SIZE); //read one page from firmware file and save it to buffer
        for (uint8_t y = 0; y < (IFLASH0_PAGE_SIZE / 4); ++y) *(buf + (y * 4)) = *(uint32_t*)(page + (y * 4)); //only 32-bit width copy authorized to latch buffer
        efcIndex = (x / (IFLASH0_NB_OF_PAGES - firmwareStartPage)) ? EFC1 : EFC0; //calculate with which efc we need to work
        //__disable_irq();
        while ((efcIndex->EEFC_FSR & EEFC_FSR_FRDY) == 0); //wait for efc to be ready
        efcIndex->EEFC_FMR = EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE); //set flash wait state to 6 when writing
        efcStatus = iap((efcIndex == EFC0) ? 0 : 1, EEFC_FCR_FCMD(0x03) & EEFC_FCR_FARG((efcIndex == EFC0) ? firmwareStartPage + x : x - (IFLASH0_NB_OF_PAGES - firmwareStartPage)) & EEFC_FCR_FKEY(0x5A)); //call iap function from rom to send command to efc
        efcIndex->EEFC_FMR = EEFC_FMR_FWS(4); //set flash wait state to 4 when reading
        //__enable_irq();
        if ((efcStatus & (EEFC_FSR_FRDY | EEFC_FSR_FCMDE | EEFC_FSR_FLOCKE)) != EEFC_FSR_FRDY)
        {
          catchError = 2; //throw programming error
          break; //exit for loop
        }
      }
    }
    else catchError = 1; //update firmware file is too big
    update.close(); //close the firmware file
    log = SD.open(updateLog, FILE_WRITE); //open log to save information
    if (catchError) //if an error has been catched
    {
      if (catchError == 1) log.println("Update firmware file is too big!");
      else if (catchError == 2) log.println("Flash programming error!");
    }
    else //verify flash
    {
      log.println("Flash programming done. Verifying...");
      log.close();
      File update = SD.open(updateBin); //open firmware file read-only
      uint8_t page[IFLASH0_PAGE_SIZE]; //declare page buffer
      uint32_t* pageStart; //used to store page start address
      for (uint16_t x = 0; x < (size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0); ++x) //for each page to verify
      {
        update.read(page, IFLASH0_PAGE_SIZE); //read one page from firmware file and save it to buffer
        pageStart = (uint32_t*)(IFLASH0_ADDR + (firmwareStartPage * IFLASH0_PAGE_SIZE) + (x * IFLASH0_PAGE_SIZE)); //Retrieve page start address
        for (uint8_t y = 0; y < (IFLASH0_PAGE_SIZE / 4); ++y) //for each 32-bit word in page
        {
          if (*(uint32_t*)(page + (y * 4)) != *(pageStart + y)) //if file and programmed values are different
          {
            catchError = 3; //throw flash verification error
            break; //exit for loop
          }
        }
        if (catchError) break; //exit for loop if an error has been catched
      }
      update.close(); //close the firmware file
      log = SD.open(updateLog, FILE_WRITE); //open log to save information
    }
    if (catchError) //if an error has been catched
    {
      if (catchError == 3) log.println("Flash verifying error!");
    }
    else
    {
      log.println("Verify successfull!");
      SD.remove(updateBin); //remove update firmware file
      log.print("Deleted ");
      log.println(updateBin);
    }
    log.close();
  }
}

void bootJump()
{
  
}

void setup()
{
  pinMode(2, INPUT_PULLUP); //configure pin to update from serial or ethernet
  pinMode(13, OUTPUT); //config status pin
  digitalWrite(13, HIGH); //say that we are in bootloader mode
  /*Serial.begin(9600); //init serial
  bool sdInit = SD.begin(4); //init sd
  File log = SD.open(updateLog, FILE_WRITE); //open or create boot log
  if (log && sdInit) log.println("sd init success"); //log sd init success
  */
  Serial.begin(9600); //init serial
  if (SD.begin(4)) //if sd init successfull
  {
    File log = SD.open(updateLog, FILE_WRITE); //open or create boot log
    log.println("sd init success"); //log sd init success
    if (SD.exists(updateBin)) //if update firmware file exists
    {
      log.close(); //close log file because sdUpdate will reopen it
      sdUpdate(); //do the flash programming from it
      bootJump(); //jump to firmware
    }
    else if (!digitalRead(updatePin)) //if no update file on the sd card and pin is driven low
    {
      log.println(""); //pin 2 drien low, waiting for an upload from serial or ethernet
      waitUpload = true; //will need to wait for an upload from serial or ethernet before jumping to the firmware
    }
    else //if no update is needed
    {
      log.close();
      bootJump(); //jump to firmware
    }
    //ethernet init
    byte _mac[] = mac;
    byte _ip[] = ip;
    Ethernet.begin(_mac, _ip); //gateway defaults to 192.168.0.1 and subnet to 255.255.255.0
    server.begin(); //start listening for clients
    log.print("ethernet server running at "); //log ethernet init
    log.println(Ethernet.localIP());
    log.close();
  }
  else Serial.println("sd init error!");
}

void loop()
{
  /*
   * check if we need to replace current firmware
   * 1-update.bin on sd card
   * 2-if pin 2 is driven low, wait for an upload from serial or ethernet
   * 
   * replace firmware using iap or ram and flash peripheral
   * set flash wait states to 6 when programming accordinf to errata page 1452
   * firmware start at page 150 of flash 0 (0x89600)
   * max firmware size = flash end address - firmware start address + 1 -> 0xFFFFF - 0x89600 + 1 = 0x76A00 (485888 bytes)
   * 
   * After update, update.bin is renamed to current.bin
   * Drive pin 2 low during reset to avoid short circuit
   * 
   * IAP code example
(unsigned int) (*IAP_Function)(unsigned long);
void main (void){
unsigned long FlashSectorNum = 200; //
unsigned long flash_cmd = 0;
unsigned long flash_status = 0;
unsigned long EFCIndex = 0; // 0:EEFC0, 1: EEFC1
IAP_Function = ((unsigned long) (*)(unsigned long)) 0x00800008; //Initialize the function pointer (retrieve function address from NMI vector in rom)
//Send your data to the sector here
//build the command to send to EEFC
flash_cmd = (0x5A << 24) | (FlashSectorNum << 8) | AT91C_MC_FCMD_EWP;
//Call the IAP function with appropriate command
flash_status = IAP_Function (EFCIndex, flash_cmd);

// -- Disable interrupts
// Disable IRQ
__disable_irq();
// Disable IRQs
for (i = 0; i < 8; i ++) NVIC->ICER[i] = 0xFFFFFFFF;
// Clear pending IRQs
for (i = 0; i < 8; i ++) NVIC->ICPR[i] = 0xFFFFFFFF;
// -- Modify vector table location
// Barriars
__DSB();
__ISB();
// Change the vector table
SCB->VTOR = ((uint32_t)vStart & SCB_VTOR_TBLOFF_Msk);
// Barriars
__DSB();
__ISB();
// -- Enable interrupts
__enable_irq();
// -- Execute application
__asm ("mov r1, r0 \n"
"ldr r0, [r1, #4] \n"
"ldr sp, [r1] \n"
"blx r0"
   */


}
