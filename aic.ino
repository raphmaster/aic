#include <memorysaver.h>
#include <UTFT.h>
#include <URTouch.h>
#include <URTouchCD.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

//480x272 8bit display module
UTFT lcd(TFT01_43,38,39,40,41);

//Touch controller
URTouch touch(6,5,4,3,2);

void setup()
{
  //lcd
  lcd.InitLCD();
  lcd.setFont(smallFont);
  //touch
  touch.InitTouch();
  touch.setPrecision(PREC_HI);
}

void loop()
{
  
}
