#include <memorysaver.h>
#include <UTFT.h>
#include <URTouch.h>
#include <URTouchCD.h>
#include <SPI.h>
#include <Ethernet.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

//480x272 8bit display module
UTFT lcd(TFT01_43,38,39,40,41);

//Touch controller
URTouch touch(6,5,4,3,2);

// the media access control (ethernet hardware) address for the shield:
#define mac { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }
//the IP address for the shield:
#define ip { 192, 168, 0, 110 }

//Initialize ethernet server on port 80
EthernetServer server(80);

void setup()
{
  //lcd
  lcd.InitLCD();
  lcd.setFont(SmallFont);
  
  //touch
  touch.InitTouch();
  touch.setPrecision(PREC_HI);
  
  //ethernet
  byte _mac[] = mac;
  byte _ip[] = ip;
  //gateway defaults to 192.168.0.1 and subnet to 255.255.255.0
  Ethernet.begin(_mac, _ip);
  //start listening for clients
  server.begin();
}

void loop()
{
  
}
