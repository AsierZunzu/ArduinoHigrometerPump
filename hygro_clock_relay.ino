#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

//Constants
const int hygrometer = A0;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int pump = 12;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int hygrometerThreshold = 30; //System will start pumping water when humidity is under this threshold, representing percentages.
const int refreshingRate = 1000; //ms, looping time
const int sleepBlinkingRate = 1000; //ms
const int alertBlinkingRate = 100; //ms
const int hygrometerConstraintHigh = 980;
const int hygrometerConstraintLow = 300;

//Variables
int hygrometerValue , hygrometerValueRaw, hygrometerValueConstrain;
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
    Serial.println(F(" Couldn't find RTC"));
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  if (!timeInRange()) {
    Serial.print(" --> SLEEPING");
    playSleepBlinkingSecence();
    return;
  }
  hygrometerValue = getHygrometerValue();
  managePump(hygrometerValue);
  Serial.println();
  delay(refreshingRate);
}

void managePump(int hygrometerValue)
{
  if (hygrometerValue > hygrometerThreshold) {
    stopPump();
  } else {
    startPump();
  }
}

/**
   checks time is in range, meaning it's not on sleepo hours. If it's not in range
*/
bool timeInRange()
{
  DateTime now = rtc.now();
  sprintf(timeBuffer, "%02i-%02i-%04i %02i:%02i:%02i", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  Serial.print("[" + String(timeBuffer) + "]");
  if ( now.hour() >= 10 && now.hour() <= 22) {
    return true;
  }
  return false;
}

/**   
 * Runs 3 slow blinks, denoting that the system is sleeping
 */
void playSleepBlinkingSecence()
{
  for (int i = 0; i <= 3; ++i)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(sleepBlinkingRate);
    digitalWrite(LED_BUILTIN, LOW);
    delay(sleepBlinkingRate);
  }
}

/**
 * Gets the hidrometer's value converted to percentages. If hardware failure is found, it stops until it gets fixed.
 */
int getHygrometerValue()
{
  hygrometerValueRaw = analogRead(hygrometer);
  Serial.print(" hygrometerValueRaw: " + String(hygrometerValueRaw));
  while (hygrometerValueRaw >= hygrometerConstraintHigh || hygrometerValueRaw <= hygrometerConstraintLow) {
    hygrometerValueRaw = analogRead(hygrometer);
    Serial.println(F(" Hygrometer failure detected, pump stopped."));
    stopPump();
    errorBlink();
  }
  hygrometerValueConstrain = constrain(hygrometerValueRaw, hygrometerConstraintLow, hygrometerConstraintHigh); //Keep the ranges!
  
  //Map value : hygrometerConstraintLow will be 100% and hygrometerConstraintHigh will be 0%
  hygrometerValue = map(hygrometerValueConstrain, hygrometerConstraintLow, hygrometerConstraintHigh, 100, 0);
  
  Serial.print(" Moisture: " + String(hygrometerValue) + "% ");
  return hygrometerValue;
}

void stopPump()
{
  digitalWrite(pump, HIGH);
}

void startPump()
{
  digitalWrite(pump, LOW);
}

/**
 * Blinks once, meaning the system found an error.
 */
void errorBlink()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(alertBlinkingRate);
  digitalWrite(LED_BUILTIN, LOW);
  delay(alertBlinkingRate);
}
