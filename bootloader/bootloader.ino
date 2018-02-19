/*
 * pin usage:
 *  spi header pins: 74,75,76
 *  sd card slave: 10
 *  ethernet slave: 4
 */

/*
 * TODO:
 * wait for upload from ethernet or serial, update and bootjump
 */

#include <SPI.h> //ethernet and sd uses spi
#include <SD.h> //to use sd card
#include <Ethernet.h> //to use wiznet ethernet shield

#define updatePin 2 //pin to wait for an firmware upload from serial or ethernet (drive pin 2 low during reset to avoid short circuit)
#define statusPin 13 //pin to tell status of the device
#define sdPin 4 //slave select for sd card
#define updateBin "update.bin" //firmware filename
#define updateLog "update.log" //update log
#define firmwareStartPage 190 //start page of firmware in flash 0
#define mac { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } // the media access control (ethernet hardware) address for the shield
#define ip { 192, 168, 0, 110 } //the IP address for the shield:

EthernetServer server(80); //Initialize ethernet server on port 80
uint32_t blinkTime = 0; //used to store millis for blinking
bool blinkStatus = HIGH; //used to store blink status
uint8_t data; //received data
uint8_t steps = 0; //used to control code execution

//set flash wait state in flash mode register from ram function because register cannot be written during corresponding flash read
__attribute__ ((section(".ramfunc")))
void setFMR(Efc* efc, uint32_t value)
{
  efc->EEFC_FMR = (value & EEFC_FMR_FWS_Msk); //change fmr
}

//perform firmware programming from file on sd card
bool sdUpdate()
{
  if (SD.exists(updateBin))
  {
    File log = SD.open(updateLog, FILE_WRITE); //log starting update
    log.print(updateBin);
    log.println(" found! starting update...");
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
      iap = (uint32_t (*)(uint32_t, uint32_t))(*(uint32_t*)CHIP_FLASH_IAP_ADDRESS); //retrieve iap function address from nmi vector in rom because efc command on flash cant be executed from the same flash bank
      for (uint16_t x = 0; x < (size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0); ++x) //for each page to write
      {
        update.read(page, IFLASH0_PAGE_SIZE); //read one page from firmware file and save it to buffer
        for (uint8_t y = 0; y < (IFLASH0_PAGE_SIZE / 4); ++y) buf[y] = *(uint32_t*)(&page[y * 4]); //only 32-bit width copy authorized to latch buffer
        efcIndex = (x / (IFLASH0_NB_OF_PAGES - firmwareStartPage -1)) ? EFC1 : EFC0; //calculate with which efc we need to work
        __disable_irq(); //ensure no flash read will be done during change to fmr or programming to the same flash bank
        setFMR(efcIndex, EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE)); //set flash wait state to 6 when writing
        efcStatus = iap((efcIndex == EFC0) ? 0 : 1, EEFC_FCR_FCMD(0x03) | EEFC_FCR_FARG((efcIndex == EFC0) ? firmwareStartPage + x : x - (IFLASH0_NB_OF_PAGES - firmwareStartPage)) | EEFC_FCR_FKEY(0x5A)); //call iap function from rom to send command to efc
        setFMR(efcIndex, EEFC_FMR_FWS(4)); //set flash wait state to 6 when writing
        __enable_irq(); //re-enable irqs
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
      if (catchError == 1) log.println("update firmware file is too big!");
      else if (catchError == 2) log.println("flash programming error!");
    }
    else //verify flash
    {
      log.println("flash programming done. verifying...");
      log.close();
      update = SD.open(updateBin); //open firmware file read-only
      uint8_t page[IFLASH0_PAGE_SIZE]; //declare page buffer
      uint32_t* pageStart; //used to store page start address
      for (uint16_t x = 0; x < (size / IFLASH0_PAGE_SIZE) + ((size % IFLASH0_PAGE_SIZE) ? 1 : 0); ++x) //for each page to verify
      {
        update.read(page, IFLASH0_PAGE_SIZE); //read one page from firmware file and save it to buffer
        pageStart = (uint32_t*)(IFLASH0_ADDR + (firmwareStartPage * IFLASH0_PAGE_SIZE) + (x * IFLASH0_PAGE_SIZE)); //Retrieve page start address
        for (uint8_t y = 0; y < (IFLASH0_PAGE_SIZE / 4); ++y) //for each 32-bit word in page
        {
          if (*(uint32_t*)(&page[y * 4]) != pageStart[y]) //if file and programmed values are different
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
      if (catchError == 3) log.println("flash verifying error!");
    }
    else
    {
      log.println("verify successfull!");
      SD.remove(updateBin); //remove update firmware file
      log.print(updateBin);
      log.println(" deleted");
    }
    log.close();
    if (catchError) return false; //if we encountered errors, return false
    else return true;
  }
  else return false; //no update file, update failed
}

//jump to the firmware
void bootJump()
{
  __disable_irq(); //ensure no interrupts will be called until we are fully jumped to firmware
  uint32_t* vtor = (uint32_t*)(IFLASH0_ADDR + (firmwareStartPage * IFLASH0_PAGE_SIZE)); //calculate firmware vector table start address
  SCB->VTOR = (uint32_t)vtor; //relocate vector table to the one in the firmware
  __set_MSP(*vtor); //set main stack pointer to the one found in vtor of the firmware
  ((void(*)(void))vtor[1])(); //set the program counter to the reset handler in vtor via function call
  //firmware should re-enable irqs
}

void setup()
{
  pinMode(updatePin, INPUT_PULLUP); //configure pin to update from serial or ethernet
  pinMode(statusPin, OUTPUT); //config status pin
  digitalWrite(statusPin, HIGH); //say that we are in bootloader mode
  Serial.begin(9600); //init serial
  if (SD.begin(sdPin)) //if sd init successfull
  {
    File log = SD.open(updateLog, FILE_WRITE); //open or create boot log
    log.println("sd init success"); //log sd init success
    if (SD.exists(updateBin)) //if update firmware file exists
    {
      log.close(); //close log file because sdUpdate will reopen it
      if (sdUpdate()) bootJump(); //do the flash programming, if success jump to firmware
      steps = 1; //else wait for an upload
      log = SD.open(updateLog, FILE_WRITE); //re-open log file
    }
    else if (!digitalRead(updatePin)) //if no update file on the sd card and pin is driven low
    {
      log.print("pin "); //pin 2 driven low, waiting for an upload from serial or ethernet
      log.print(updatePin);
      log.print(" driven low. waiting for an upload from serial or ethernet...");
      steps = 1; //will need to wait for an upload from serial or ethernet before jumping to the firmware
    }
    else //if no update is needed
    {
      log.close();
      bootJump(); //jump to firmware
    }
    //ethernet init
    uint8_t _mac[] = mac;
    uint8_t _ip[] = ip;
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
  if (steps >= 1) //if we need to wait for an upload from ethernet or serial
  {
    EthernetClient client = server.available(); //Check if a client is connected and has data available for reading
    if (client) //if a client is connected and has data available
    {
      data = client.read(); //read one byte from received data
      if (data == 'F') //if someone is ready to upload firmware
      {
        client.write(data); //reply that we are ready too
        steps = 2;
      }
    }
    //check for upload from serial
    if ((millis() - blinkTime) >= 1000) //toggle led every 1000 ms to signal user we are waiting for upload
    {
      digitalWrite(statusPin, !blinkStatus); //toggle led status
      blinkStatus = !blinkStatus; //save new status
      blinkTime = millis(); //save new time
    }
  }
}
