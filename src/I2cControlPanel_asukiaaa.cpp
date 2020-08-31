#include "I2cControlPanel_asukiaaa.h"
#include "utils_asukiaaa.h"
#include "utils_asukiaaa/wire.h"

I2cControlPanel_asukiaaa_info::I2cControlPanel_asukiaaa_info() {
  for (int i = 0; i < 16; ++i) {
    lcdChars[i] = ' ';
  }
}

void I2cControlPanel_asukiaaa_info::putStringToLcdChars(String str, int from) {
  int strLen = str.length();
  for (int i = 0; i < strLen; ++i) {
    int charIndex = from + i;
    if (charIndex >= 16) break;
    lcdChars[charIndex] = str[i];
  }
}

I2cControlPanel_asukiaaa::I2cControlPanel_asukiaaa(uint8_t address) {
  wire = NULL;
  this->address = address;
}

void I2cControlPanel_asukiaaa::setWire(TwoWire *wire) {
  this->wire = wire;
}

void I2cControlPanel_asukiaaa::begin() {
  if (wire == NULL) {
    wire = &Wire;
    wire->begin();
  }
}

int I2cControlPanel_asukiaaa::readButtonsAndSwitches(I2cControlPanel_asukiaaa_info* info) {
  uint8_t buff;
  int result = utils_asukiaaa::wire::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES, &buff, 1);
  if (result != 0) return result;
  info->buttonsLeft[0] = (buff & 0b00000001) != 0;
  info->buttonsLeft[1] = (buff & 0b00000010) != 0;
  info->buttonsRight[0] = (buff & 0b00000100) != 0;
  info->buttonsRight[1] = (buff & 0b00001000) != 0;
  info->buttonJoyLeft = (buff & 0b00010000) != 0;
  info->buttonJoyRight = (buff & 0b00100000) != 0;
  info->toggleSwitches[0] = (buff & 0b01000000) != 0;
  info->toggleSwitches[1] = (buff & 0b10000000) != 0;
  return 0;
}

int I2cControlPanel_asukiaaa::readLeds(I2cControlPanel_asukiaaa_info* info) {
  uint8_t buff;
  int result = utils_asukiaaa::wire::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS, &buff, 1);
  if (result != 0) return result;
  info->leds[0] = (buff & 0b0001) != 0;
  info->leds[1] = (buff & 0b0010) != 0;
  info->leds[2] = (buff & 0b0100) != 0;
  info->leds[3] = (buff & 0b1000) != 0;
  return 0;
}

int I2cControlPanel_asukiaaa::readJoysticksHoriAndVert(I2cControlPanel_asukiaaa_info* info) {
  uint8_t buff[4];
  int result = utils_asukiaaa::wire::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT, (uint8_t *) &buff, 4);
  if (result != 0) return result;
  info->joyLeftHori = buff[0];
  info->joyLeftVert = buff[1];
  info->joyRightHori = buff[2];
  info->joyRightVert = buff[3];
  return 0;
}

int I2cControlPanel_asukiaaa::readEncoders(I2cControlPanel_asukiaaa_info* info) {
  return utils_asukiaaa::wire::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS, info->encoders, 2);
}

int I2cControlPanel_asukiaaa::readLcdChars(I2cControlPanel_asukiaaa_info* info) {
  return utils_asukiaaa::wire::readBytes(wire, address, I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS, info->encoders, 16);
}

int I2cControlPanel_asukiaaa::writeLeds(I2cControlPanel_asukiaaa_info info) {
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

int I2cControlPanel_asukiaaa::writeLcdChars(I2cControlPanel_asukiaaa_info info) {
  wire->beginTransmission(address);
  wire->write(I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS);
  wire->write((const uint8_t*) info.lcdChars, 16);
  return wire->endTransmission();
}
