/*
 * CLCD595.h - Arduino library for CLCD with 74HC595 interface
 * 					- header for C++ adaptation
 * by Homin Lee (Suapapa) <http://www.suapapa.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation. You should have received
 * a copy of the license along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLCD595_H
#define CLCD595_H

//#include "WConstants.h"
#include <WProgram.h>

#define CLCD595_ITOA_BUF_LEN 12

#define RSMODE_CMD 0
#define RSMODE_CHR 1

class CLCD595 {
public:
  CLCD595(uint8_t pinData, uint8_t pinClock, uint8_t pinLatch);
  void init(void);
  void clear(void);

  void command(uint8_t cmd);
  void putChar(char ch);
  void putStr(char str[]);
  void putDec(int num);

  void setCursor(uint8_t line, uint8_t index);

private:
  uint8_t _pinLatch;
  uint8_t _pinData;
  uint8_t _pinClock;

  uint8_t _rsMode;

  char _itoaBuf[CLCD595_ITOA_BUF_LEN+1];
  uint8_t _itoa(int n);

  void _putNibble(uint8_t data);
  void _putByte(uint8_t data);

  void _shiftOut(uint8_t myDataOut);
};
#endif
