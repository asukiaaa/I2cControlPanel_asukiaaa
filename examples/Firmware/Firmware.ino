// This firmware depends on ST7032_asukiaaa
// Please install that before updating firmware

#include <Wire.h>
#include <Wire1.h>
#include <ST7032_asukiaaa.h>

#define LED_R 8
#define LED_Y 9
#define LED_G 10
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
#define LOGGLE_B 21

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

void updateLed(bool red, bool yellow, bool green) {
  digitalWrite(LED_R, red);
  digitalWrite(LED_Y, yellow);
  digitalWrite(LED_G, green);
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
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
  pinMode(LOGGLE_B, INPUT_PULLUP);

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
  setBtnSignal(-1);
  setBtnSignal(0);
  delay(5);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.setCursor(4, 0);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
  setBtnSignal(-1);
  setBtnSignal(1);
  delay(5);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.setCursor(0, 1);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
  setBtnSignal(-1);
  setBtnSignal(2);
  delay(5);
  readBtnState(&b0, &b1, &b2, &b3);
  lcd.print(b0);
  lcd.print(b1);
  lcd.print(b2);
  lcd.print(b3);
}

void testLeds() {
  updateLed(true, false, false);
  delay(1000);
  updateLed(false, true, false);
  delay(1000);
  updateLed(false, false, true);
  delay(1000);
  updateLed(false, false, false);
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

void loop() {
  testJoyAndVolumes();
  delay(1000);
}
