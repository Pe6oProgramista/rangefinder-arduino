#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <VL53L1X.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

VL53L1X sensor;

const int button = 2;
int activatePin = 3;

int dirPushCnt = 0;
int dirState = 0;
int lastDirState = 1;

String message = "Press button to measure the distance!";
int stringStart, stringStop = 0;
int scrollCursor = 16;

void setup() {
  Serial.begin(9600);

  pinMode(button, INPUT);
  pinMode(activatePin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(button), pin_ISR, CHANGE);

  // setup LCD
  lcd.init();
  lcd.backlight();

  // setup Sensor
  Wire.begin();
  Wire.setClock(400000);

  sensor.setTimeout(500);
  sensor.init();

  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  sensor.startContinuous(50);
}

void loop() {
  if (dirPushCnt % 2 == 1) {
    digitalWrite(activatePin, HIGH);
  } else {
    digitalWrite(activatePin, LOW);
  }
  distSensor();
}

void pin_ISR() {
  dirState = digitalRead(button);
  if (dirState != lastDirState) {
    if (dirState == HIGH) {
      dirPushCnt++;
    }
    delay(50);
  }
  lastDirState = dirState;
}

void distSensor() {
  if (dirPushCnt % 2 == 1) {
    float dist = sensor.read() / 10.;
    Serial.println(dist);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dist.: ");
    lcd.print(dist);
    lcd.print(" cm");
    delay(200);
    stringStart = stringStop = 0;
    scrollCursor = 16;
  } else {
    printScrollingText();
  }
}

void printScrollingText() {
  lcd.setCursor(scrollCursor, 0);
  lcd.print(message.substring(stringStart, stringStop));
  delay(170);
  lcd.clear();
  
  if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop){
    stringStart = stringStop = 0;
    scrollCursor = 16;
  } else if (stringStop == message.length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }
}

