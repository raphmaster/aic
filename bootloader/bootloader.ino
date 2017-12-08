#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>

#define mac { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } // the media access control (ethernet hardware) address for the shield
#define ip { 192, 168, 0, 110 } //the IP address for the shield:

EthernetServer server(80); //Initialize ethernet server on port 80

void setup()
{
  pinMode(2, INPUT_PULLUP); //configure pin to update from serial or ethernet
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
   * 
   * After update, update.bin is renamed to current.bin
   * Drive pin 2 low during reset to avoid short circuit
   * 
   * 
   */

   //sd update
   if (SD.exists(update.bin))
   {
    
   }
}
