/*
 * CLCD595.cpp - Arduino library for CLCD with 74HC595 interface
 * 					- C++ adaptation
 * by Homin Lee (Suapapa) <http://www.suapapa.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation. You should have received
 * a copy of the license along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "CLCD595.h"

#define BITMASK_RS 0x10 //(0x01<<4)
#define BITMASK_EN 0x20 //(0x01<<5)

#define CMD_CLEAR 0x01
#define CMD_INDEX 0x80

#define fastWriteHigh(_pin_) ( _pin_ < 8 ?  PORTD |= 1 << (_pin_ & 0x07) : ( _pin_ < 14 ?  PORTB |= 1 << ((_pin_ -8) & 0x07) : PORTC |= 1 << ((_pin_ -14) & 0x07)  ) )
#define fastWriteLow(_pin_) ( _pin_ < 8 ?   PORTD &= ~(1 << (_pin_  & 0x07)) : ( _pin_ < 14 ?  PORTB &= ~(1 << ((_pin_ -8) & 0x07) )  :  PORTC &= ~(1 << ((_pin_ -14) & 0x07) )  ) )


#define bitWrite(pin, val) { \
  if ((val) == LOW) fastWriteLow(pin); \
  else		  fastWriteHigh(pin); \
}

#define shiftOutBit(dataPin, clockPin, val, bit) { \
  bitWrite(dataPin, ((val) & (1 << (bit))) ? HIGH:LOW); \
  bitWrite(clockPin, HIGH); \
  bitWrite(clockPin, LOW); \
}

#define shiftOutByte(dataPin, clockPin, val) { \
  shiftOutBit(dataPin, clockPin, val, 7); \
  shiftOutBit(dataPin, clockPin, val, 6); \
  shiftOutBit(dataPin, clockPin, val, 5); \
  shiftOutBit(dataPin, clockPin, val, 4); \
  shiftOutBit(dataPin, clockPin, val, 3); \
  shiftOutBit(dataPin, clockPin, val, 2); \
  shiftOutBit(dataPin, clockPin, val, 1); \
  shiftOutBit(dataPin, clockPin, val, 0); \
}

void CLCD595::_shiftOut(uint8_t myDataOut)
{
  fastWriteLow(_pinLatch);
  shiftOutByte(_pinData, _pinClock, myDataOut);
  fastWriteHigh(_pinLatch);
}

CLCD595::CLCD595(uint8_t pinData, uint8_t pinClock, uint8_t pinLatch) //,  uint8_t pinEn)
{
  _pinData = pinData;
  _pinLatch = pinLatch;

  _pinClock = pinClock;
}


void CLCD595::init(void)
{
  pinMode(_pinLatch, OUTPUT);
  pinMode(_pinData, OUTPUT);
  pinMode(_pinClock, OUTPUT);

  digitalWrite(_pinData, LOW);
  digitalWrite(_pinClock, LOW);
  digitalWrite(_pinLatch, LOW);

  _rsMode = RSMODE_CMD;
  _shiftOut(0x00);

  _putNibble(0x03);
  delay(5);
  _putNibble(0x03);
  delay(1);
  _putNibble(0x03);
  _putNibble(0x02);

  command(0x28); //function set: DL 8bit, 2-line, 5x8 dot

  //command(0x0E); //display control: turn display on, cursor on, no blinking
  command(0x04); // display control:
  command(0x01); //clear display
  command(0x06); //entry mode set: increment automatically, display shift, right shift
  command(0x0C); //display on
}


void CLCD595::clear(void)
{
  command(CMD_CLEAR);
}


void CLCD595::command(uint8_t cmd)
{
  _rsMode = RSMODE_CMD;
  _putByte(cmd);
  delay(2);
}


void CLCD595::putChar(char ch)
{
  _rsMode = RSMODE_CHR;
  _putByte(ch);
}


void CLCD595::putStr(char str[])
{
  uint8_t i = 0;
  uint8_t currChar = str[i];

  while (currChar) {
    if (currChar == '\n')
      setCursor(1, 0);
    else
      putChar(currChar);
    currChar = str[++i];
  }
}


/* itoa:  convert n to characters in s */
uint8_t CLCD595::_itoa(int n)
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    _itoaBuf[i++] = n % 10 + '0';
    if (i >= CLCD595_ITOA_BUF_LEN) {
      _itoaBuf[CLCD595_ITOA_BUF_LEN] = '\0';
      return CLCD595_ITOA_BUF_LEN;
    }
  } while ((n /= 10) > 0);
  if (sign < 0)
    _itoaBuf[i++] = '-';
  _itoaBuf[i] = '\0';
  return i;
}


void CLCD595::putDec(int num)
{
  uint8_t i = _itoa(num);

  while (i) {
    putChar(_itoaBuf[--i]);
  }
}


//0-79, index for one line display, 8 bit mode
//0-39 and 64-103 for lines one and two of two line display,
void CLCD595::setCursor(uint8_t line, uint8_t index)
{
  int cmd = CMD_INDEX + index;

  if (line == 1)
    cmd += 64;
  command(cmd);
}


void CLCD595::_putByte(uint8_t data)
{
  _putNibble(data >> 4);
  _putNibble(data);
}


void CLCD595::_putNibble(uint8_t dataIn)
{
  int data = dataIn & 0x0f;

  if (_rsMode == RSMODE_CHR)
    data |= BITMASK_RS;

  _shiftOut(data | BITMASK_EN);
  delayMicroseconds(50);
  _shiftOut(data);
  delay(2);
  _shiftOut(0x00);
}


