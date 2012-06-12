#include <CLCD595.h>

//create a CLC595 object to control an lcd
CLCD595 lcd = CLCD595(12, 11, 10); //data, clk, latch

#define LINE_LENGTH 16
char strBuff[LINE_LENGTH+1];

// \r<line|strlen><data>
#define PACKET_START '\r'

void setup(void)
{
  lcd.init(); //initialize the LCD
  Serial.begin(19200);
  lcd.clear(); //clear the display
}

char inByte;
char lineNo = 0;
char strLen = 0;

void loop(void)
{
  if (Serial.available() < 2) {
    delay(10);
    return;
  }
  digitalWrite(13, HIGH); //turn on an LED for debugging
  inByte = Serial.read();
  if (inByte != PACKET_START) return;
  inByte = Serial.read();
  strLen = inByte & 0x7f;
  if (strLen > LINE_LENGTH) strLen = LINE_LENGTH;
  lineNo = (inByte * 0x80) >> 7;
  while (Serial.available() < strLen)
    delay(10);
  for (int i = 0; i < strLen; i++)
    strBuff[i] = (char)(Serial.read() & 0xff);
  for (int i = strLen; i < LINE_LENGTH; i++)
    strBuff[i] = ' ';
  strBuff[LINE_LENGTH] == 0x00;
  lcd.setCursor(lineNo, 0);
  lcd.putStr(strBuff);
  digitalWrite(13, LOW); //turn on an LED for debugging
} //repeat forever

