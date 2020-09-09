#ifndef _I2C_CONTROL_PANEL_ASUKIAAA_H_
#define _I2C_CONTROL_PANEL_ASUKIAAA_H_

#include <Arduino.h>
#include <Wire.h>

#define I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS 0x40
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES 0x00
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS 0x01
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT 0x02
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_RIGHT 0x04
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS 0x06
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS 0x08

class I2cControlPanel_asukiaaa_info {
 public:
  I2cControlPanel_asukiaaa_info();
  bool buttonJoyLeft;
  bool buttonJoyRight;
  bool buttonsLeft[2];
  bool buttonsRight[2];
  uint8_t encoders[2];
  uint8_t joyLeftHori;
  uint8_t joyLeftVert;
  uint8_t joyRightHori;
  uint8_t joyRightVert;
  char lcdChars[16];
  bool leds[4];
  bool slideSwitches[2];
  void putStringToLcdChars(String str, int from);
};

class I2cControlPanel_asukiaaa {
 public:
  I2cControlPanel_asukiaaa(uint8_t address = I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS);
  void setWire(TwoWire* wire);
  void begin();

  int readButtonsAndSwitches(I2cControlPanel_asukiaaa_info* info);
  int readEncoders(I2cControlPanel_asukiaaa_info* info);
  int readLcdChars(I2cControlPanel_asukiaaa_info* info);
  int readLeds(I2cControlPanel_asukiaaa_info* info);
  int readJoysticksHoriAndVert(I2cControlPanel_asukiaaa_info* info);
  int writeLeds(I2cControlPanel_asukiaaa_info info);
  int writeLcdChars(I2cControlPanel_asukiaaa_info info);

 private:
  TwoWire* wire;
  uint8_t address;
};

#endif
