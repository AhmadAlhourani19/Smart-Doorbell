#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>

#define RST_PIN         9
#define SS_PIN          10
#define SERVO_PIN       5
#define BUZZER_PIN      6
#define BUTTON_PIN      2

Servo doorServo;
SoftwareSerial mySerial(7, 8);
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0);

  display.begin(SSD1306_I2C_ADDRESS, 0, 2);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Someone is at the door!");

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      String cardID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardID.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      display.print("Card ID: ");
      display.println(cardID);
      delay(2000);

      if (grantAccess(cardID)) {
        openDoor();
        captureVisitorImage();
      } else {
        buzz();
        display.println("Access Denied!");
        delay(2000);
      }
      display.clearDisplay();
    }
  }
}

void openDoor() {
  doorServo.write(90); 
  delay(5000);
  doorServo.write(0); 
}

void buzz() {
  tone(BUZZER_PIN, 1000, 1000); 
}

void captureVisitorImage() {

mySerial.println("CAPTURE_IMAGE_COMMAND"); 
  delay(500); 
  

  while (mySerial.available()) {
    char imageData = mySerial.read();
  }
}

bool grantAccess(String cardID) {

  String authorizedCardIDs[] = {"00112233", "AABBCCDD", "11223344"}; 
  for (int i = 0; i < sizeof(authorizedCardIDs) / sizeof(authorizedCardIDs[0]); i++) {
    if (cardID.equals(authorizedCardIDs[i])) {
      return true;
    }
  }
  
  return false; 
