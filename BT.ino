#include "BluetoothSerial.h"
#include <ESP32Servo.h>
#include <LiquidCrystal.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int servoPin = 13;
int RS = 15, E = 2, D4 = 4, D5 = 16, D6 = 17, D7 = 5;
int buttonPin = 18;

String incoming = "";
String prevIncoming = "";
String resetServo = "82101115101116321151011141181111310";
String engageServo = "6911010397103101321151011141181111310";
String resetAlarm = "821011151011163297108971141091310";
String changeAlarm = "67104971101031013297108971141091310";
String decVals[] = {"48", "49", "50", "51", "52", "53", "54", "55", "56", "57"};
unsigned long alarmSeconds = 0;
unsigned long secondsElapsed = 0;
String timeIncoming = "";
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
Servo ser;
void setup() {
  Serial.begin(115200);
  ser.attach(servoPin);
  lcd.begin(16, 2);

  pinMode(buttonPin, INPUT);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  lcd.clear();
  lcd.print("Ready to Connect");
  Serial.println("The device started, now you can pair it with bluetooth!");
  SerialBT.println("Enter time until alarm(HHMMSS):");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter time until");
  lcd.setCursor(0, 1);
  lcd.print("alarm(HHMMSS)");
  timeIncoming = "";
  while (!SerialBT.available()) {

  }
  while (SerialBT.available()) {
    timeIncoming += SerialBT.read();
  }
  alarmSeconds = incomingToSeconds(timeIncoming);
  secondsElapsed += (millis() / 1000);

}

void loop() {
  unsigned long currTime = (millis() / 1000) - secondsElapsed;
  int pos;
  incoming = "";
  while (SerialBT.available()) {
    //Serial.print(SerialBT.read());
    incoming += SerialBT.read();
  }
  Serial.println(incoming);

  if (digitalRead(buttonPin)) {
    ser.write(177);
    alarmSeconds = 0;
  }

  if (incoming.equals("")) {

  }
  else if (incoming.equals(resetServo)) {
    ser.write(177);
    SerialBT.println("The servo was reset.");
  }
  else if (incoming.equals(engageServo)) {
    ser.write(0);
    SerialBT.println("The servo was engaged." );
  }
  else if (incoming.equals(changeAlarm)) {
//    secondsElapsed += currTime;
    SerialBT.println("Enter time until alarm(HHMMSS):");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter time until");
    lcd.setCursor(0, 1);
    lcd.print("alarm(HHMMSS)");
    timeIncoming = "";
    while (!SerialBT.available()) {

    }
    while (SerialBT.available()) {
      timeIncoming += SerialBT.read();
    }
    alarmSeconds = incomingToSeconds(timeIncoming);
    secondsElapsed += currTime;// I think this needs to be changed. It should be += milis()/1000; I think this is what was creating the delay.
  }
  else if(incoming.equals(resetAlarm)){
    alarmSeconds = 0;
  }
  else{
    SerialBT.println("Invalid Command");
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Going off in:");
  lcd.setCursor(0, 1);
  if (alarmSeconds > currTime) {
    lcd.print(timeFormat(alarmSeconds - currTime));
    Serial.print(alarmSeconds);
    Serial.print("\t");
    Serial.print(currTime);
  }
  else if(alarmSeconds == currTime){    
       
    ser.write(0);
    alarmSeconds = 0; 
    lcd.print("Timer Elapsed");
  }
  else{
    alarmSeconds = 0;
    lcd.print("Timer Elapsed");
    
  }
  
  delay(100);
}
unsigned long incomingToSeconds(String s) {
  s.remove(s.length() - 4);

  String hour = s.substring(0, 4);
  String minute = s.substring(4, 8);
  String second = s.substring(8, 12);

  int h1 = 0, h2 = 0, m1 = 0, m2 = 0, s1 = 0, s2 = 0;
  for (int i = 0; i < 10; i++) {
    if (hour.substring(0, 2).equals(decVals[i])) {
      h1 = i;
    }
    if (hour.substring(2).equals(decVals[i])) {
      h2 = i;
    }
  }
  for (int i = 0; i < 10; i++) {
    if (minute.substring(0, 2).equals(decVals[i])) {
      m1 = i;
    }
    if (minute.substring(2).equals(decVals[i])) {
      m2 = i;
    }
  }
  for (int i = 0; i < 10; i++) {
    if (second.substring(0, 2).equals(decVals[i])) {
      s1 = i;
    }
    if (second.substring(2).equals(decVals[i])) {
      s2 = i;
    }
  }

  hour = String(h1) + String(h2);
  minute = String(m1) + String(m2);
  second = String(s1) + String(s2);

  unsigned long secs = (hour.toInt() * 3600) + (minute.toInt() * 60) + (second.toInt());
  return secs;
}

String timeFormat(unsigned long seconds) {
  int hour = seconds / 3600;
  String sH = String(hour);
  int minute = (seconds % 3600) / 60;
  String sM = String(minute);
  int second = seconds % 60;
  String sS = String(second);
  return sH + ":" + sM + ":" + sS;
}
