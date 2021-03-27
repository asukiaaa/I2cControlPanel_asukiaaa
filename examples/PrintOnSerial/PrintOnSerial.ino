#include <I2cControlPanel_asukiaaa.h>

I2cControlPanel_asukiaaa::Driver controlPanel;
// I2cControlPanel_asukiaaa::Driver controlPanel(0x41); // custom address

void setup() {
  Serial.begin(9600);

  // Wire1.begin();
  // controlPanel.setWire(&Wire1); // controller can use other wire port

  controlPanel.begin();
}

void printInfo(const I2cControlPanel_asukiaaa::Info& info) {
  Serial.println("buttonsLeft: " +
                 String(info.buttonsLeft[0]) + " " +
                 String(info.buttonsLeft[1]));
  Serial.println("buttonsRight: " +
                 String(info.buttonsRight[0]) + " " +
                 String(info.buttonsRight[1]));
  Serial.println("buttonsJoyLeft: " +
                 String(info.buttonJoyLeft));
  Serial.println("buttonsJoyRight: " +
                 String(info.buttonJoyRight));
  Serial.println("slideSwitches: " +
                 String(info.slideSwitches[0]) + " " +
                 String(info.slideSwitches[1]));
  Serial.println("Encoders: " +
                 String(info.encoders[0]) + " " +
                 String(info.encoders[1]));
  Serial.println("joyLeftHori: " + String(info.joyLeftHori));
  Serial.println("joyLeftVert: " + String(info.joyLeftVert));
  Serial.println("joyRightHori: " + String(info.joyRightHori));
  Serial.println("joyRightVert: " + String(info.joyRightVert));
  Serial.print("leds:");
  for (int i = 0; i < 4; ++i) {
    Serial.print(" ");
    Serial.print(info.leds[i]);
  }
  Serial.println("");
  Serial.print("lcdChars[0to7]  : ");
  for (int i = 0; i < 8; ++i) {
    Serial.print(info.lcdChars[i]);
  }
  Serial.println("");
  Serial.print("lcdChars[8to15] : ");
  for (int i = 8; i < 16; ++i) {
    Serial.print(info.lcdChars[i]);
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

  int ledToBright = count % 5;
  for (int i = 0; i < 4; ++i) {
    info.leds[i] = i == ledToBright;
  }
  String strToShow = String(count);
  info.putStringToLcdChars(strToShow, 16 - strToShow.length());
  int writeState = controlPanel.write(info);
  if (writeState == 0) {
    Serial.println("Write info about leds and lcdChars");
  } else {
    Serial.println("Cannot write. Error: " + String(writeState));
  }
  Serial.println("");

  ++count;
  delay(1000);
}
