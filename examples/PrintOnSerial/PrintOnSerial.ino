#include <I2cControlPanel_asukiaaa.h>

I2cControlPanel_asukiaaa controlPanel;

void setup() {
  Serial.begin(9600);
  controlPanel.begin();
}

void loop() {
  I2cControlPanel_asukiaaa_info info;
  controlPanel.readButtonsAndSwitches(&info);

  Serial.println("Buttons");
  Serial.println(String(info.buttonsLeft[0]) + " " +
                 String(info.buttonsLeft[1]) + " " +
                 String(info.buttonsRight[0]) + " " +
                 String(info.buttonsRight[1]) + " " +
                 String(info.buttonJoyLeft) + " " +
                 String(info.buttonJoyLeft));

  controlPanel.readEncoders(&info);
  Serial.println("Encoders");
  Serial.println(String(info.encoders[0]) + " " +
                 String(info.encoders[1]));

  controlPanel.readJoysticksHoriAndVert(&info);
  Serial.println("Joy hori and vert");
  Serial.println(info.joyLeftHori);
  Serial.println(info.joyLeftVert);
  Serial.println(info.joyRightHori);
  Serial.println(info.joyRightVert);

  delay(200);
}
