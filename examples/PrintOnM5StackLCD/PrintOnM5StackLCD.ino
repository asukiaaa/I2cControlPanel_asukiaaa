#include <I2cControlPanel_asukiaaa.h>
#include <M5Stack.h>
#include <Wire.h>

I2cControlPanel_asukiaaa::Driver controlPanel;
// I2cControlPanel_asukiaaa::Driver controlPanel(0x41); // custom address

void setup() {
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.setTextSize(2);
  controlPanel.begin();
  controlPanel.setUseCRC8(true);  // crc8 is avairable for
  // protocol version 1 or more
}

void drawJoyRect(int x, int y, int wh, uint8_t joyHori, uint8_t joyVert,
                 bool pressed) {
  M5.Lcd.fillRect(x, y, wh, wh, WHITE);
  int circleSize = 2;
  int movableLen = wh - circleSize * 2;
  M5.Lcd.fillCircle(x + circleSize + movableLen * (float)(joyHori) / 0xff,
                    y + circleSize + movableLen * (1 - (float)joyVert / 0xff),
                    circleSize, pressed ? RED : BLUE);
}

void printInfo(const I2cControlPanel_asukiaaa::Info& info) {
  drawJoyRect(50, 30, 50, info.joyLeftHori, info.joyLeftVert,
              info.buttonJoyLeft);
  drawJoyRect(220, 30, 50, info.joyRightHori, info.joyRightVert,
              info.buttonJoyRight);
  int textBoxX = 108;
  int textBoxY = 32;
  int textBoxW = 105;
  int textBoxH = 44;
  int textBoxPadding = 5;
  int lineHeight = 18;
  M5.Lcd.drawRect(textBoxX, textBoxY, textBoxW, textBoxH, WHITE);
  M5.Lcd.setCursor(textBoxX + textBoxPadding, textBoxY + textBoxPadding);
  for (int i = 0; i < 8; ++i) {
    M5.Lcd.print(info.lcdChars[i]);
  }
  M5.Lcd.setCursor(textBoxX + textBoxPadding,
                   textBoxY + textBoxPadding + lineHeight);
  for (int i = 8; i < 16; ++i) {
    M5.Lcd.print(info.lcdChars[i]);
  }

  int ledStartX = 108;
  int ledY = 80;
  int ledW = 15;
  int ledH = 8;
  int ledIntervalX = 30;
  for (int i = 0; i < 4; ++i) {
    int isOn = info.leds[i];
    M5.Lcd.fillRect(ledStartX + ledIntervalX * i, ledY, ledW, ledH,
                    isOn ? GREEN : DARKGREEN);
  }

  int btnLStartX = 80, btnRStartX = 240;
  int btnR = 10;
  int btnStartY = 100;
  int btnIntervalY = 30;
  uint32_t colorPressed = WHITE;
  uint32_t colorReleased = DARKGREY;

  for (int i = 0; i < 2; ++i) {
    bool pressedL = info.buttonsLeft[i];
    bool pressedR = info.buttonsRight[i];
    M5.Lcd.fillCircle(btnLStartX, btnStartY + btnIntervalY * i, btnR,
                      pressedL ? colorPressed : colorReleased);
    M5.Lcd.fillCircle(btnRStartX, btnStartY + btnIntervalY * i, btnR,
                      pressedR ? colorPressed : colorReleased);
  }

  M5.Lcd.setCursor(0, 150);
  M5.Lcd.println("slideSwitches: " + String(info.slideSwitches[0]) + " " +
                 String(info.slideSwitches[1]));
  // M5.Lcd.fillCircleHelper(100, 100, 50, 10, 20, WHITE);
  M5.Lcd.println("Encoders: " + String(info.encoders[0]) + " " +
                 String(info.encoders[1]) + "       ");
}

int count = 0;

void loop() {
  M5.Lcd.setCursor(0, 0);
  I2cControlPanel_asukiaaa::Info info;
  controlPanel.read(&info);
  if (info.stateRead != 0) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println("Cannot read. Error: " + String(info.stateRead));
    delay(1000);
    return;
  } else {
    M5.Lcd.println("protocolVersion: " + String(info.protocolVersion) +
                   "      ");
  }

  int ledToBright = count % 5;
  for (int i = 0; i < 4; ++i) {
    info.leds[i] = i == ledToBright;
  }
  String strToShow = String(count);
  info.putStringToLcdChars(strToShow, 16 - strToShow.length());
  int writeState = controlPanel.write(info);
  if (writeState != 0) {
    M5.Lcd.println("Cannot write. Error: " + String(writeState));
  }

  printInfo(info);

  ++count;
  delay(500);
}
