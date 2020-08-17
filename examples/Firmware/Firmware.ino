// This firmware depends on ST7032_asukiaaa
// Please install that before updating firmware

#include <Wire.h>
#include <Wire1.h>
#include <ST7032_asukiaaa.h>
#include <I2cControlPanel_asukiaaa.h>

#define LED_0 8
#define LED_1 9
#define LED_2 10
#define LED_3 3
#define BTN_SIG_0 0
#define BTN_SIG_1 1
#define BTN_SIG_2 2
#define BTN_0 4
#define BTN_1 5
#define BTN_2 6
#define BTN_3 7
#define BTN_JOY_L 11
#define BTN_JOY_R 12
#define ANALOG_JOY_L_HORI 0
#define ANALOG_JOY_L_VERT 1
#define ANALOG_JOY_R_HORI 2
#define ANALOG_JOY_R_VERT 3
#define ANALOG_ENCODE_L 6
#define ANALOG_ENCODE_R 7
#define TOGGLE_T 20
#define TOGGLE_B 21

int registerIndex = 0;
// 0 byte: button * 4, joy button * 2, toggle * 2]
// 1 byte: led * 3]
// 2-7 byte: analog * 6
// 8-23 byte: char * 16
const int registerLen = 24;
const int analogStartIndex = 2;
const int charStartIndex = 9;
uint8_t registers[registerLen];

ST7032_asukiaaa lcd;

void onReceive(int) {
#ifdef DEBUG
  Serial.print("onReceive:");
#endif
  uint8_t receivedLen = 0;
  while (0 < Wire.available()) {
    uint8_t v = Wire.read();
#ifdef DEBUG
    Serial.print(" ");
    Serial.print(v, HEX);
#endif
    if (receivedLen == 0) {
      registerIndex = v;
    } else {
      // write action here
      ++registerIndex;
    }
    ++receivedLen;
  }
#ifdef DEBUG
  Serial.println("");
#endif
}

void onRequest() {
  if (registerIndex < registerLen) {
    Wire.write(&registers[registerIndex], registerLen - registerIndex);
  } else {
    Wire.write(0);
  }
}

void setBtnSignal(int targetNum) {
  digitalWrite(BTN_SIG_0, targetNum != 0);
  digitalWrite(BTN_SIG_1, targetNum != 1);
  digitalWrite(BTN_SIG_2, targetNum != 2);
}

void readBtnState(bool* btn0, bool* btn1, bool* btn2, bool* btn3) {
  int onState = LOW;
  *btn0 = digitalRead(BTN_0) == onState;
  *btn1 = digitalRead(BTN_1) == onState;
  *btn2 = digitalRead(BTN_2) == onState;
  *btn3 = digitalRead(BTN_3) == onState;
}

uint8_t readAddress() {
  bool pads[8];
  setBtnSignal(2);
  delay(1);
  readBtnState(&pads[0], &pads[2], &pads[4], &pads[6]);
  setBtnSignal(1);
  delay(1);
  readBtnState(&pads[1], &pads[3], &pads[5], &pads[7]);
  setBtnSignal(-1);
  uint8_t address = 0;
  for (int i = 7; i >= 0; --i) {
    address <<= 1;
    if (pads[i]) address |= 0b1;
  }
  return address;
}

void updateLeds(bool l0, bool l1, bool l2, bool l3) {
  digitalWrite(LED_0, l0);
  digitalWrite(LED_1, l1);
  digitalWrite(LED_2, l2);
  digitalWrite(LED_3, l3);
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(BTN_SIG_0, OUTPUT);
  pinMode(BTN_SIG_1, OUTPUT);
  pinMode(BTN_SIG_2, OUTPUT);
  pinMode(BTN_0, INPUT_PULLUP);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_JOY_L, INPUT_PULLUP);
  pinMode(BTN_JOY_R, INPUT_PULLUP);
  pinMode(TOGGLE_T, INPUT_PULLUP);
  pinMode(TOGGLE_B, INPUT_PULLUP);

  pinMode(BTN_3, INPUT_PULLUP);

  lcd.begin(2, 8);
  lcd.setContrast(30);
  uint8_t address = readAddress();
  lcd.print("hi  0x");
  lcd.print(address, HEX);
  Wire1.begin(address);
  Wire1.onReceive(onReceive);
  Wire1.onRequest(onRequest);
}

void testButtons() {
  bool b0, b1, b2, b3;
  setBtnSignal(0);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.setCursor(4, 0);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
  setBtnSignal(1);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.setCursor(0, 1);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
  setBtnSignal(2);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
}

void testLeds() {
  updateLeds(true, false, false, false);
  delay(1000);
  updateLeds(false, true, false, false);
  delay(1000);
  updateLeds(false, false, true, false);
  delay(1000);
  updateLeds(false, false, false, true);
  delay(1000);
  updateLeds(false, false, false, false);
  delay(1000);
}

void printAnalogOnLcd(int pin) {
  lcd.print(analogRead(pin) / 4, HEX);
}

void testJoyAndVolumes() {
  lcd.setCursor(4, 0);
  printAnalogOnLcd(ANALOG_ENCODE_L);
  printAnalogOnLcd(ANALOG_ENCODE_R);
  lcd.setCursor(0, 1);
  printAnalogOnLcd(ANALOG_JOY_L_HORI);
  printAnalogOnLcd(ANALOG_JOY_L_VERT);
  printAnalogOnLcd(ANALOG_JOY_R_HORI);
  printAnalogOnLcd(ANALOG_JOY_R_VERT);
}

uint8_t readButtonsAndSwitches() {
  uint8_t buttons = 0;
  bool b0, b1, b2, b3;
  setBtnSignal(2);
  readBtnState(&b0, &b1, &b2, &b3);
  if (b0) buttons |= 0b0001;
  if (b1) buttons |= 0b0010;
  if (b2) buttons |= 0b0100;
  if (b3) buttons |= 0b1000;
  if (digitalRead(BTN_JOY_L) == LOW) buttons |= 0b00010000;
  if (digitalRead(BTN_JOY_R) == LOW) buttons |= 0b00100000;
  if (digitalRead(TOGGLE_T) == LOW) buttons |= 0b01000000;
  if (digitalRead(TOGGLE_B) == LOW) buttons |= 0b10000000;
  return buttons;
}

void readAll() {
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_BUTTONS_AND_SWITCHES] = readButtonsAndSwitches();
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT] = analogRead(ANALOG_JOY_L_HORI) / 4;
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_LEFT + 1] = analogRead(ANALOG_JOY_L_VERT) / 4;
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_RIGHT] = analogRead(ANALOG_JOY_R_HORI) / 4;
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_JOY_RIGHT + 1] = analogRead(ANALOG_JOY_R_VERT) / 4;
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS] = analogRead(ANALOG_ENCODE_L) / 4;
  registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_ENCODERS + 1] = analogRead(ANALOG_ENCODE_R) / 4;
}

void updateLedIfNeeded() {
  static const int charsLen = 16;
  static String line0 = "";
  static String line1 = "";
  if (line0.length() < charsLen) {
    while (line0.length() < charsLen) {
      line0 += " ";
      line1 += " ";
    }
  }
  bool neededToUpdateLine0 = false;
  bool neededToUpdateLine1 = false;
  for (int i = 0; i < charsLen; ++i) {
    char targetChar = registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LCD_CHARS + i];
    if (i < charsLen / 2) {
      if (line0[i] != targetChar) {
        line0[i] = targetChar;
        neededToUpdateLine0 = true;
      }
    } else {
      if (line1[i] != targetChar) {
        line1[i - charsLen / 2] = targetChar;
        neededToUpdateLine1 = true;
      }
    }
  }
  if (neededToUpdateLine0) {
    lcd.setCursor(0, 0);
    lcd.print(line0);
  }
  if (neededToUpdateLine1) {
    lcd.setCursor(0, 1);
    lcd.print(line1);
  }
}

void updateAll() {
  uint8_t leds = registers[I2C_CONTROL_PANEL_ASUKIAAA_REGISTER_LEDS];
  updateLeds((leds & 0b0001) != 0, (leds & 0b0010) != 0, (leds & 0b0100) != 0, (leds & 0b1000) != 0);
  updateLedIfNeeded();
}

void loop() {
  // testJoyAndVolumes();
  // delay(1000);
  readAll();
  updateAll();
  delay(1);
}
