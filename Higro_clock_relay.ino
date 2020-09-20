#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

//Constants
const int hygrometer = A0;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int pump = 12;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int hygrometerThreshold = 50;
const int pumpAlertThreshold = 20; //s
const int refreshingRate = 1000; //ms
const int alertBlinkingRate = 100; //ms

//Variables
int hygrometerValue , hygrometerValueRaw, hygrometerValueConstrain;
bool pumpIsOn = true;
int secondsPumping = 0;
char timeBuffer[20] = "00-00-0000 00:00:00";

void setup() {
  Serial.begin(9600);

  //Initialize relay
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH);

  //Initialize build-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  //Initialize RTC
  delay(1000);
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  if (timeInRange()) {
    hygrometerValue = getHygrometer();
    pumpIsOn = managePump(hygrometerValue);
    trackSecondsPumping(pumpIsOn);
    checkSystemErrors();
  }

  Serial.println();
  delay(refreshingRate);
}

bool managePump(int hygrometerValue)
{
  if (hygrometerValue > hygrometerThreshold) {
    digitalWrite(pump, HIGH);
    return false;
  } else {
    digitalWrite(pump, LOW);
    Serial.print(" --> PUMP ON");
    return true;
  }
}

void trackSecondsPumping(bool pumpIsOn)
{
  if (pumpIsOn) {
    secondsPumping++;
  } else {
    secondsPumping = 0;
  }
}

void checkSystemErrors()
{
  if (secondsPumping > pumpAlertThreshold) {
    Serial.println(F("Excessive pumping detected, stopping all sistems now."));
    initilializeInfiniteBlinking();
  }
}

void initilializeInfiniteBlinking()
{
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(alertBlinkingRate);
    digitalWrite(LED_BUILTIN, LOW);
    delay(alertBlinkingRate);
  }
}

bool timeInRange()
{
  DateTime now = rtc.now();
  sprintf(timeBuffer, "%02i-%02i-%04i %02i:%02i:%02i", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  Serial.print("[" + String(timeBuffer) + "]");
  if ( now.hour() >= 10 && now.hour() <= 22) {
    return true;
  }
  Serial.print(" --> SLEEP");
  return false;
}

int getHygrometer()
{
  hygrometerValueRaw = analogRead(hygrometer);     //Read analog value
  hygrometerValueConstrain = constrain(hygrometerValueRaw, 250, 1023); //Keep the ranges!
  hygrometerValue = map(hygrometerValueConstrain, 250, 1023, 100, 0); //Map value : 250 will be 100% and 1023 will be 0%

  Serial.print(" Moisture: " + String(hygrometerValue)/*+"("+hygrometerValueConstrain+")["+hygrometerValue+"]"*/ + "% ");
  return hygrometerValue;
}
