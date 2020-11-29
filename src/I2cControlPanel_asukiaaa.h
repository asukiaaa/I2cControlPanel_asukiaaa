#ifndef _I2C_CONTROL_PANEL_ASUKIAAA_H_
#define _I2C_CONTROL_PANEL_ASUKIAAA_H_

#include <Arduino.h>
#include <Wire.h>

#define I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS 0x40
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES 0x00
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT 0x01
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_RIGHT 0x03
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS 0x05
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS 0x07
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS 0x08
#define I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH 24

namespace I2cControlPanel_asukiaaa {

  class Info {
  public:
    Info();
    int stateRead;
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
    bool joyLeftOperatedDown();
    bool joyLeftOperatedLeft();
    bool joyLeftOperatedRight();
    bool joyLeftOperatedUp();
    bool joyRightOperatedDown();
    bool joyRightOperatedLeft();
    bool joyRightOperatedRight();
    bool joyRightOperatedUp();
  };

  class Driver {
  public:
    Driver(uint8_t address = I2C_CONTROL_PANEL_ASUKIAAA_DEFAUT_ADDRESS);
    void setWire(TwoWire* wire);
    void begin();

    int read(Info* info, bool withWriteArea = true);
    int readButtonsAndSwitches(Info* info);
    int readEncoders(Info* info);
    int readJoysticksHoriAndVert(Info* info);
    int readLcdChars(Info* info);
    int readLeds(Info* info);
    int write(const Info& info);
    int writeLcdChars(const Info& info);
    int writeLeds(const Info& info);

  private:
    uint8_t buffers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LENGTH];
    TwoWire* wire;
    const uint8_t address;
    static void parseButtonsAndSwitches(Info* info, uint8_t buff);
    static void parseEncoders(Info* info, uint8_t* buffs);
    static void parseJoystickHoriAndVert(Info* info, uint8_t* buffs);
    static void parseLcdChars(Info* info, uint8_t* buffs);
    static void parseLeds(Info* info, uint8_t buff);
    static int setStateRead(Info* info, int result);
    static uint8_t createLedState(const Info& info);
  };

}

#endif
