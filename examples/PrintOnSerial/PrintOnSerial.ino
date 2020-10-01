#include <I2cControlPanel_asukiaaa.h>
#include <string_asukiaaa.h>

I2cControlPanel_asukiaaa::Driver controlPanel;

void setup() {
  Serial.begin(9600);
  controlPanel.begin();
}

void printInfo(I2cControlPanel_asukiaaa::Info info) {
  Serial.println("Buttons: " +
                 String(info.buttonsLeft[0]) + " " +
                 String(info.buttonsLeft[1]) + " " +
                 String(info.buttonsRight[0]) + " " +
                 String(info.buttonsRight[1]) + " " +
                 String(info.buttonJoyLeft) + " " +
                 String(info.buttonJoyRight) + " " +
                 String(info.slideSwitches[0]) + " " +
                 String(info.slideSwitches[1]));
  Serial.println("Encoders: " +
                 String(info.encoders[0]) + " " +
                 String(info.encoders[1]));
  Serial.println("Joy: hori, vert");
  Serial.println("L  : " +
                 string_asukiaaa::padNumStart(info.joyLeftHori, 4, ' ') +
                 "," +
                 string_asukiaaa::padNumStart(info.joyLeftVert, 4, ' '));
  Serial.println("R  : " +
                 string_asukiaaa::padNumStart(info.joyRightHori, 4, ' ') +
                 "," +
                 string_asukiaaa::padNumStart(info.joyRightVert, 4, ' '));
  Serial.print("LEDs:");
  for (int i = 0; i < 4; ++i) {
    Serial.print(" ");
    Serial.print(info.leds[i]);
  }
  Serial.println("");
  Serial.println("LCD chars");
  for (int i = 0; i < 16; ++i) {
    Serial.print(info.lcdChars[i]);
    if (i == 7) Serial.println("");
  }
  Serial.println("");
}

int count = 0;

void loop() {
  I2cControlPanel_asukiaaa::Info info;
  controlPanel.read(&info);
  if (info.stateRead == 0) {
    printInfo(info);
  } else {
    Serial.println("Cannot read info. Error: " + String(info.stateRead));
  }

  int i2cState;
  int ledToBright = count % 5;
  for (int i = 0; i < 4; ++i) {
    info.leds[i] = i == ledToBright;
  }
  i2cState = controlPanel.writeLeds(info);
  if (i2cState == 0) {
    Serial.println("Update leds");
  } else {
    Serial.println("Cannot update leds. Error: " + String(i2cState));
  }

  String strToShow = string_asukiaaa::padNumStart(count, 4, ' ');
  info.putStringToLcdChars(strToShow, 16 - strToShow.length());
  i2cState = controlPanel.writeLcdChars(info);
  if (i2cState == 0) {
    Serial.println("Update lcd chars");
  } else {
    Serial.println("Cannot update lcd. Error: " + String(i2cState));
  }

  ++count;
  delay(1000);
}
