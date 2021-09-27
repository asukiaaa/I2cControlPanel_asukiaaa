#ifndef _I2C_CONTROL_PANEL_ASUKIAAA_TEMPLATE_HPP_
#define _I2C_CONTROL_PANEL_ASUKIAAA_TEMPLATE_HPP_

#include <Arduino.h>
#include <CRCx.h>

#include <wire_asukiaaa_template.hpp>

#define I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS 0x40
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES 0x00
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT 0x01
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_RIGHT 0x03
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS 0x05
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS 0x07
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS 0x08
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_PROTOCOL_VERION 24
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_CRC8 25
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH 26

#define STATE_READ_UNMATCH_CRC8 10

namespace I2cControlPanel_asukiaaa {

class Info {
 public:
  int stateRead = -1;
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
  uint8_t protocolVersion = 0;

  Info() {
    for (int i = 0; i < 16; ++i) {
      lcdChars[i] = ' ';
    }
    joyLeftHori = joyLeftVert = joyRightHori = joyRightVert = 0xff / 2;
  }

  void putStringToLcdChars(String str, int from = 0) {
    int strLen = str.length();
    for (int i = 0; i < strLen; ++i) {
      int charIndex = from + i;
      if (charIndex >= 16) break;
      lcdChars[charIndex] = str[i];
    }
  }
  bool joyLeftOperatedDown() const { return joyLeftVert < ValueJoyLow; }
  bool joyLeftOperatedLeft() const { return joyLeftHori < ValueJoyLow; }
  bool joyLeftOperatedRight() const { return joyLeftHori > ValueJoyHigh; }
  bool joyLeftOperatedUp() const { return joyLeftVert > ValueJoyHigh; }
  bool joyRightOperatedDown() const { return joyRightVert < ValueJoyLow; }
  bool joyRightOperatedLeft() const { return joyRightHori < ValueJoyLow; }
  bool joyRightOperatedRight() const { return joyRightHori > ValueJoyHigh; }
  bool joyRightOperatedUp() const { return joyRightVert > ValueJoyHigh; }

 private:
  enum JoyVariable {
    ValueJoyHigh = (0x00ff * 2 / 3),
    ValueJoyLow = (0xff / 3),
  };
};

template <class TemplateWire>
class DriverTemplate {
 public:
  DriverTemplate(uint8_t address = I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS)
      : address(address) {
    wire = NULL;
  }

  void setWire(TemplateWire* wire) { this->wire = wire; }
  void setUseCRC8(bool useCRC8) { this->useCRC8 = useCRC8; }

  void begin(){};

  int read(Info* info) {
    int result = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, 0, buffers, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH);
    if (result != 0) return setStateRead(info, result);
    // for (int i = 0; i < I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH; ++i) {
    //   Serial.print(buffers[i], HEX);
    //   Serial.print(" ");
    // }
    // Serial.println("");
    if (useCRC8 &&
        crcx::crc8(buffers, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_CRC8) !=
            buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_CRC8]) {
      return setStateRead(info, STATE_READ_UNMATCH_CRC8);
    }
    parseButtonsAndSwitches(
        info,
        buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES]);
    parseEncoders(info, &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS]);
    parseJoystickHoriAndVert(
        info, &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT]);
    parseLcdChars(info,
                  &buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS]);
    parseLeds(info, buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS]);
    info->protocolVersion =
        buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_PROTOCOL_VERION];
    return setStateRead(info, result);
  }

  int readButtonsAndSwitches(Info* info) {
    uint8_t buff;
    int result = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES,
        &buff, 1);
    if (result != 0) return setStateRead(info, result);
    parseButtonsAndSwitches(info, buff);
    return setStateRead(info, result);
  }

  int readEncoders(Info* info) {
    int state = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS,
        info->encoders, 2);
    return setStateRead(info, state);
  }

  int readJoysticksHoriAndVert(Info* info) {
    int result = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT,
        (uint8_t*)buffers, 4);
    if (result != 0) return setStateRead(info, result);
    parseJoystickHoriAndVert(info, buffers);
    return setStateRead(info, result);
  }

  int readLcdChars(Info* info) {
    int state = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS,
        (uint8_t*)info->lcdChars, 16);
    return setStateRead(info, state);
  }

  int readLeds(Info* info) {
    uint8_t buff;
    int result = wire_asukiaaa::readBytes<TemplateWire>(
        wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS, &buff, 1);
    if (result != 0) return setStateRead(info, result);
    parseLeds(info, buff);
    return setStateRead(info, result);
  }

  int write(const Info& info) {
    uint8_t ledState = createLedState(info);
    wire->beginTransmission(address);
    wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS);
    wire->write(ledState);
    wire->write((const uint8_t*)info.lcdChars, 16);
    return wire->endTransmission();
  }

  int writeLcdChars(const Info& info) {
    wire->beginTransmission(address);
    wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS);
    wire->write((const uint8_t*)info.lcdChars, 16);
    return wire->endTransmission();
  }

  int writeLeds(const Info& info) {
    uint8_t ledState = createLedState(info);
    wire->beginTransmission(address);
    wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS);
    wire->write(ledState);
    return wire->endTransmission();
  }

 private:
  uint8_t buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH];
  TemplateWire* wire;
  const uint8_t address;
  bool useCRC8 = false;

  static void parseButtonsAndSwitches(Info* info, uint8_t buff) {
    info->buttonsLeft[0] = (buff & 0b00000001) != 0;
    info->buttonsLeft[1] = (buff & 0b00000010) != 0;
    info->buttonsRight[0] = (buff & 0b00000100) != 0;
    info->buttonsRight[1] = (buff & 0b00001000) != 0;
    info->buttonJoyLeft = (buff & 0b00010000) != 0;
    info->buttonJoyRight = (buff & 0b00100000) != 0;
    info->slideSwitches[0] = (buff & 0b01000000) != 0;
    info->slideSwitches[1] = (buff & 0b10000000) != 0;
  }

  static void parseEncoders(Info* info, uint8_t* buffs) {
    memcpy(info->encoders, buffs, 2);
  }

  static void parseJoystickHoriAndVert(Info* info, uint8_t* buffs) {
    info->joyLeftHori = buffs[0];
    info->joyLeftVert = buffs[1];
    info->joyRightHori = buffs[2];
    info->joyRightVert = buffs[3];
  }

  static void parseLcdChars(Info* info, uint8_t* buffs) {
    memcpy(info->lcdChars, buffs, 16);
  }

  static void parseLeds(Info* info, uint8_t buff) {
    info->leds[0] = (buff & 0b0001) != 0;
    info->leds[1] = (buff & 0b0010) != 0;
    info->leds[2] = (buff & 0b0100) != 0;
    info->leds[3] = (buff & 0b1000) != 0;
  }

  static int setStateRead(Info* info, int state) {
    return info->stateRead = state;
  }

  static uint8_t createLedState(const Info& info) {
    uint8_t ledState = 0;
    if (info.leds[0]) ledState |= 0b0001;
    if (info.leds[1]) ledState |= 0b0010;
    if (info.leds[2]) ledState |= 0b0100;
    if (info.leds[3]) ledState |= 0b1000;
    return ledState;
  }
};

}  // namespace I2cControlPanel_asukiaaa

#endif
