#include <CLCD595.h>

//create a CLC595 object to control an lcd
CLCD595 lcd = CLCD595(12, 11, 10); //data, clk, latch

char string1[] = "CLCD with\nonly 3 wires!";

void setup(void)
{
  lcd.init(); //initialize the LCD
  digitalWrite(13, HIGH); //turn on an LED for debugging
}
void loop(void)
{
  lcd.clear(); //clear the display
  delay(1000); //delay 1000 ms to view change

  lcd.putStr(string1); //send the string to the LCD
  delay(1000); //delay 1000 ms to view change
} //repeat forever

