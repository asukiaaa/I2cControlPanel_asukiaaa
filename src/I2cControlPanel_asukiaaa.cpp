#include "I2cControlPanel_asukiaaa.h"
#include <wire_asukiaaa.h>

#define VALUE_JOY_HIGH (0x00ff * 2 / 3)
#define VALUE_JOY_LOW (0xff / 3)

namespace I2cControlPanel_asukiaaa {

  Info::Info() {
    for (int i = 0; i < 16; ++i) {
      lcdChars[i] = ' ';
    }
    joyLeftHori = joyLeftVert = joyRightHori = joyRightVert = 0xff / 2;
    stateRead = -1;
  }

  void Info::putStringToLcdChars(String str, int from) {
    int strLen = str.length();
    for (int i = 0; i < strLen; ++i) {
      int charIndex = from + i;
      if (charIndex >= 16) break;
      lcdChars[charIndex] = str[i];
    }
  }

  bool Info::joyLeftOperatedDown() {
    return joyLeftVert < VALUE_JOY_LOW;
  }

  bool Info::joyLeftOperatedLeft() {
    return joyLeftHori < VALUE_JOY_LOW;
  }

  bool Info::joyLeftOperatedRight() {
    return joyLeftHori > VALUE_JOY_HIGH;
  }

  bool Info::joyLeftOperatedUp() {
    return joyLeftVert > VALUE_JOY_HIGH;
  }

  bool Info::joyRightOperatedDown() {
    return joyRightVert < VALUE_JOY_LOW;
  }

  bool Info::joyRightOperatedLeft() {
    return joyRightHori < VALUE_JOY_LOW;
  }

  bool Info::joyRightOperatedRight() {
    return joyRightHori > VALUE_JOY_HIGH;
  }

  bool Info::joyRightOperatedUp() {
    return joyRightVert > VALUE_JOY_HIGH;
  }

  Driver::Driver(uint8_t address): address(address) {
    wire = NULL;
  }

  void Driver::setWire(TwoWire *wire) {
    this->wire = wire;
  }

  void Driver::begin() {
    if (wire == NULL) {
      wire = &Wire;
      wire->begin();
    }
  }

  int Driver::read(Info* info, bool withWriteArea) {
    int result = wire_asukiaaa::readBytes(wire, address, 0, buffers, withWriteArea ? I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH : I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS);
    if (result != 0) return setStateRead(info, result);
    parseButtonsAndSwitches(info, buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES]);
    parseEncoders(info, &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS]);
    parseJoystickHoriAndVert(info, &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT]);
    if (withWriteArea) {
      parseLcdChars(info, &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS]);
      parseLeds(info, buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS]);
    }
    return setStateRead(info, result);
  }

  int Driver::readButtonsAndSwitches(Info* info) {
    uint8_t buff;
    int result = wire_asukiaaa::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES, &buff, 1);
    if (result != 0) return setStateRead(info, result);
    parseButtonsAndSwitches(info, buff);
    return setStateRead(info, result);
  }

  int Driver::readLeds(Info* info) {
    uint8_t buff;
    int result = wire_asukiaaa::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS, &buff, 1);
    if (result != 0) return setStateRead(info, result);
    parseLeds(info, buff);
    return setStateRead(info, result);
  }

  int Driver::readJoysticksHoriAndVert(Info* info) {
    int result = wire_asukiaaa::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT, (uint8_t *) buffers, 4);
    if (result != 0) return setStateRead(info, result);
    parseJoystickHoriAndVert(info, buffers);
    return setStateRead(info, result);
  }

  int Driver::readEncoders(Info* info) {
    int state = wire_asukiaaa::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS, info->encoders, 2);
    return setStateRead(info, state);
  }

  int Driver::readLcdChars(Info* info) {
    int state = wire_asukiaaa::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS, (uint8_t*) info->lcdChars, 16);
    return setStateRead(info, state);
  }

  int Driver::writeLeds(Info info) {
    uint8_t ledState = 0;
    if (info.leds[0]) ledState |= 0b0001;
    if (info.leds[1]) ledState |= 0b0010;
    if (info.leds[2]) ledState |= 0b0100;
    if (info.leds[3]) ledState |= 0b1000;

    wire->beginTransmission(address);
    wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS);
    wire->write(ledState);
    return wire->endTransmission();
  }

  int Driver::writeLcdChars(Info info) {
    wire->beginTransmission(address);
    wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS);
    wire->write((const uint8_t*) info.lcdChars, 16);
    return wire->endTransmission();
  }

  void Driver::parseButtonsAndSwitches(Info* info, uint8_t buff) {
    info->buttonsLeft[0] = (buff & 0b00000001) != 0;
    info->buttonsLeft[1] = (buff & 0b00000010) != 0;
    info->buttonsRight[0] = (buff & 0b00000100) != 0;
    info->buttonsRight[1] = (buff & 0b00001000) != 0;
    info->buttonJoyLeft = (buff & 0b00010000) != 0;
    info->buttonJoyRight = (buff & 0b00100000) != 0;
    info->slideSwitches[0] = (buff & 0b01000000) != 0;
    info->slideSwitches[1] = (buff & 0b10000000) != 0;
  }

  void Driver::parseEncoders(Info* info, uint8_t* buffs) {
    memcpy(info->encoders, buffs, 2);
  }

  void Driver::parseJoystickHoriAndVert(Info* info, uint8_t* buffs) {
    info->joyLeftHori = buffs[0];
    info->joyLeftVert = buffs[1];
    info->joyRightHori = buffs[2];
    info->joyRightVert = buffs[3];
  }

  void Driver::parseLcdChars(Info* info, uint8_t* buffs) {
    memcpy(info->lcdChars, buffs, 16);
  }

  void Driver::parseLeds(Info* info, uint8_t buff) {
    info->leds[0] = (buff & 0b0001) != 0;
    info->leds[1] = (buff & 0b0010) != 0;
    info->leds[2] = (buff & 0b0100) != 0;
    info->leds[3] = (buff & 0b1000) != 0;
  }

  int Driver::setStateRead(Info* info, int state) {
    return info->stateRead = state;
  }

}
