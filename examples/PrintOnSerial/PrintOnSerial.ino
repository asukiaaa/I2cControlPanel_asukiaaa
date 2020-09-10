#include <I2cControlPanel_asukiaaa.h>
#include <utils_asukiaaa.h>
#include <utils_asukiaaa/string.h>

I2cControlPanel_asukiaaa controlPanel;

void setup() {
  Serial.begin(9600);
  controlPanel.begin();
}

int count = 0;

void loop() {
  I2cControlPanel_asukiaaa_info info;
  int resultCode;
  resultCode = controlPanel.readButtonsAndSwitches(&info);
  if (resultCode == 0) {
    Serial.println("Buttons");
    Serial.println(String(info.buttonsLeft[0]) + " " +
                  String(info.buttonsLeft[1]) + " " +
                  String(info.buttonsRight[0]) + " " +
                  String(info.buttonsRight[1]) + " " +
                  String(info.buttonJoyLeft) + " " +
                  String(info.buttonJoyRight) + " " +
                  String(info.slideSwitches[0]) + " " +
                  String(info.slideSwitches[1]));
  } else {
    Serial.println("Cannot read buttons info");
  }

  resultCode = controlPanel.readEncoders(&info);
  if (resultCode == 0) {
    Serial.println("Encoders");
    Serial.println(String(info.encoders[0]) + " " +
                    String(info.encoders[1]));
  } else {
    Serial.println("Cannot read encoders info");
  }

  resultCode = controlPanel.readJoysticksHoriAndVert(&info);
  if (resultCode == 0) {
    Serial.println("Joy hori and vert");
    Serial.println(utils_asukiaaa::string::padNumStart(info.joyLeftHori, 4, ' ') +
                  utils_asukiaaa::string::padNumStart(info.joyLeftVert, 4, ' '));
    Serial.println(utils_asukiaaa::string::padNumStart(info.joyRightHori, 4, ' ') +
                  utils_asukiaaa::string::padNumStart(info.joyRightVert, 4, ' '));
  } else {
    Serial.println("Cannot read joy hori vert info");
  }

  int ledToBright = count % 5;
  for (int i = 0; i < 4; ++i) {
    info.leds[i] = i == ledToBright;
  }
  resultCode = controlPanel.writeLeds(info);
  if (resultCode == 0) {
    Serial.println("Update leds");
  } else {
    Serial.println("Cannot update leds");
  }

  String strToShow = String(count);
  info.putStringToLcdChars(strToShow, 8 - strToShow.length());
  resultCode = controlPanel.writeLcdChars(info);
  if (resultCode == 0) {
    Serial.println("Update lcd");
  } else {
    Serial.println("Cannot update lcd");
  }

  ++count;
  delay(1000);
}
