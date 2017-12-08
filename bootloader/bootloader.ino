#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>

#define updateFile "update.bin"
#define firmwareMaxSize 485888
#define mac { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } // the media access control (ethernet hardware) address for the shield
#define ip { 192, 168, 0, 110 } //the IP address for the shield:

EthernetServer server(80); //Initialize ethernet server on port 80

void setup()
{
  pinMode(2, INPUT_PULLUP); //configure pin to update from serial or ethernet
  pinMode(13, OUTPUT); //config status pin
  digitalWrite(13, HIGH); //say that we are in bootloader mode
  Serial.begin(9600); //init serial
  bool sdInit = SD.begin(4); //init sd
  File log = SD.open("boot-log.txt", FILE_WRITE); //open or create boot log
  if (log && sdInit) log.println("sd init success"); //log sd init success
  
  //ethernet init
  byte _mac[] = mac;
  byte _ip[] = ip;
  Ethernet.begin(_mac, _ip); //gateway defaults to 192.168.0.1 and subnet to 255.255.255.0
  server.begin(); //start listening for clients
  if (log && sdInit)
  {
    log.print("ethernet server running at "); //log ethernet init
    log.println(Ethernet.localIP());
  }
  log.close();
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
   */

   //sd update
   if (SD.exists(updateFile))
   {
    File update = SD.open(updateFile); //open firmware read-only
    uint32_t size = update.size(); //get firmware size
    if (size <= firmwareMaxSize)
    {
      //Muste be set right before writing
      EFC0->EEFC_FMR = EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE); //set flash wait state to 6 when writing
      EFC1->EEFC_FMR = EEFC_FMR_FWS(CHIP_FLASH_WRITE_WAIT_STATE);
      uint8_t page[256]; //declare page buffer
      uint32_t (*iap)(uint32_t, uint32_t); //delcare iap function
      iap = (uint32_t (*)(uint32_t, uint32_t))0x800008; //retrieve function address from nmi vector
      update.read(page, 256); //read 256 bytes from firmware file and save it to buffer
    }
   }
}
