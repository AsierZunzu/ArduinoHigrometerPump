#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

//Constants
const int hygrometer = A0;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int pump = 12;  //Hygrometer sensor analog pin output at pin A0 of Arduino
const int hygrometerThreshold = 50; //System will start pumping water when humidity is under this threshold, representing percentages.
const int pumpAlertThreshold = 20; //The system will consider that if the time the pump has been active is higher thant this variable, a hardware error must have happened.
const int refreshingRate = 1000; //ms, looping time
const int sleepBlinkingRate = 1000; //ms
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
  if (!timeInRange()) {
    Serial.print(" --> SLEEPING");
    playSleepBlinkingSecence();
    return;
  }
  checkSystemErrors();
  hygrometerValue = getHygrometer();
  pumpIsOn = managePump(hygrometerValue);
  if (pumpIsOn) {
    Serial.print(" --> PUMP ON");
    trackSecondsPumping();
  } else {
    resetSecondsPumping();
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
    return true;
  }
}

/**
   Adds a second to the pumping seconds counter, used to detect excessive pumping
*/
void trackSecondsPumping()
{
  secondsPumping++;
}

/**
   Resets the pumping seconds counter
*/
void resetSecondsPumping()
{
  secondsPumping = 0;
}

/*
   Checks for escesive pumping. If detected, runs an alert, and an infinite blinking loop.
*/
void checkSystemErrors()
{
  if (secondsPumping > pumpAlertThreshold) {
    Serial.println(F("Excessive pumping detected, stopping all sistems now."));
    initilializeInfiniteBlinking();
  }
}

/**
   Runs an infinite blinking loop meaning the system found an error and it has stopped.
*/
void initilializeInfiniteBlinking()
{
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(alertBlinkingRate);
    digitalWrite(LED_BUILTIN, LOW);
    delay(alertBlinkingRate);
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

/*
   Runs 3 slow blinks, denoting that the system is sleeping
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

/*
   Gets the hidrometer's value converted to percentages
*/
int getHygrometer()
{
  hygrometerValueRaw = analogRead(hygrometer);     //Read analog value
  hygrometerValueConstrain = constrain(hygrometerValueRaw, 250, 1023); //Keep the ranges!
  hygrometerValue = map(hygrometerValueConstrain, 250, 1023, 100, 0); //Map value : 250 will be 100% and 1023 will be 0%

  Serial.print(" Moisture: " + String(hygrometerValue)/*+"("+hygrometerValueConstrain+")["+hygrometerValue+"]"*/ + "% ");
  return hygrometerValue;
}