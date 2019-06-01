#include <Adafruit_NeoPixel.h>

#define STRIP_PIN 6
#define NUM_STRIP_LEDS 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_STRIP_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800); // STRIP SETTINGS & VALUES

const int IDLE_STATE = 0;                                  // State machine values
const int ACCEPT = 1;
const int RECORDING_MODE = 2;
const int WAITING_PICKUP = 3;
const int PLAYBACK = 4;
const int PLAYBACK_PAUSE = 5;

const int REMOVE = 0;                                      // Transition events
const int ADD = 1;
const int IDLE_EVENT = 2;                                  // No state change detected (removed from serial.print)
const int TIME_OUT = 3;
int state = IDLE_STATE;                                    // Current case State change


unsigned long Target_Time_Millis = 0;                      // Time Figures
boolean Timer_Running = false;

// Timer between states
const int short_timer = 2000;
const int long_timer = 5000;

// Pressure state
const int BUTTON_A = 7;
int pressed_A = LOW;
int transientState_A = LOW;


// Debounce button constant state switch
unsigned long debounceTime_A = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Pressure Sensor
const int LEDpin = 5;
const int pressure = 80;
int force;
//int LEDdata;

const int fsrPin = 2;     // the FSR and cap are connected to pin2
int fsrReading;     // the digital reading


char mystr1[2] = "R\n"; //String data
char mystr2[2] = "E\n"; //String data
char mystr3[2] = "P\n"; //String data
char mystr4[2] = "S\n"; //String data

int get_event() {

  if (Timer_Running && millis() > Target_Time_Millis) {
    Timer_Running = false;
    return TIME_OUT;

  }
  int x = digitalRead(BUTTON_A) or force;


  // See if the button is still changing state; if so, nudge the record of the last time it changed:

  if (x != transientState_A) {
    debounceTime_A = millis();
  }

  transientState_A = x;

  /* Has the button been stable for a few milliseconds; if so, do the "pressed" vs current comparison
     to see whether it's an actual button change, and output serial data accordingly.
  */
  if (millis() - debounceTime_A > DEBOUNCE_DELAY) {
    if (x == HIGH && pressed_A == LOW) {
      pressed_A = HIGH;
      //Serial.println("HIGH");
      return ADD;
    } else if (x == LOW && pressed_A == HIGH) {
      pressed_A = LOW;
      //Serial.println("LOW");
      return REMOVE;
    }
  }

  return IDLE_EVENT;
}

// ------------------------------------------------------------------------------------------------------------------------------



void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();

  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  //pinMode(2, INPUT_PULLUP);
}

#define COLOUR1_R 100                       // Neo Pixel STRIP mapping colour values
#define COLOUR1_G 100                       // WHITE RGB
#define COLOUR1_B 100

#define COLOUR2_R 0
#define COLOUR2_G 0                         // BLUE RGB
#define COLOUR2_B 100

#define COLOUR3_R 0
#define COLOUR3_G 100                       // GREEN RGB
#define COLOUR3_B 0


//----------------------------------------- functions -------------------------------------------------------


//void PRESSURE_SENSOR() {
//  LEDdata = analogRead(pressurePin);
//  //Serial.println(LEDdata);
//  if (LEDdata > pressure)
//  {
//    digitalWrite(LEDpin, HIGH);
//    force = HIGH;
//  }
//  else
//  {
//    digitalWrite(LEDpin, LOW);
//    force = LOW;
//  }
//  //delay(100);
//}


void PRESSURE_SENSOR_DIGITAL() {
  // read the resistor using the RCtime technique
  fsrReading = RCtime(fsrPin);

  if (fsrReading == 30000) {
    // if we got 30000 that means we 'timed out'
    Serial.println("Nothing connected to sensor!");
  }

  if  (fsrReading < pressure) {
    digitalWrite(LEDpin, HIGH);
    force = HIGH;
  }
  else
  {
    digitalWrite(LEDpin, LOW);
    force = LOW;
  }
  //delay(100);
}
int RCtime(int RCpin) {
  int reading = 0;  // start with 0

  // set the pin to an output and pull to LOW (ground)
  pinMode(RCpin, OUTPUT);
  digitalWrite(RCpin, LOW);

  // Now set the pin to an input and...
  pinMode(RCpin, INPUT);
  while (digitalRead(RCpin) == LOW) { // count how long it takes to rise up to HIGH
    reading++;      // increment to keep track of time

    if (reading == 30000) {
      // if we got this far, the resistance is so high
      // its likely that nothing is connected!
      break;           // leave the loop
    }
  }
  // OK either we maxed out at 30000 or hopefully got a reading, return the count

  return reading;
}


void maintainDisplay(int event)
{
  static const unsigned long REFRESH_INTERVAL = 1000; // ms
  static unsigned long lastRefreshTime = 0;

  if (millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
    refreshDisplay(event);
  }
}

void refreshDisplay(int event)
{
//  Serial.println("----1 Second----");
//  //  Serial.print("Pressure value: ");
//  //  Serial.println(LEDdata);
//  Serial.print("RCtime reading = ");
//  Serial.println(fsrReading);     // the raw analog reading
//  Serial.print("state number: ");
//  Serial.println(state);
//  //Serial.print("Event: ");
//  //Serial.println(event);
//  Serial.print("force on/off: ");
//  Serial.println(force);
//  Serial.println("                ");
}

// Debouncing: reference: https://www.arduino.cc/en/Tutorial/Debounce
// ------------------------------------------------------------------------------------------------------------------------------



void loop() {

  //PRESSURE_SENSOR();
  PRESSURE_SENSOR_DIGITAL();
  int event = get_event();
  maintainDisplay(event);

  //  if (event != 2)
  //     Serial.print(event);

  unsigned long now = millis();                                       // Time value for NEO Pixel flash animation
  int t = now % 1000;

  switch (state) {                                                      // Call mode 0 on start up
    case IDLE_STATE:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {

        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(100, 100, 100)); // Moderately bright white color.

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == ADD) {
        state = ACCEPT;
        Target_Time_Millis = millis() + short_timer;                         // Setting timer for next state (Case)
        Timer_Running = true;
        //Serial.println("ACCEPT");
      }

      break;                                                          // Stop function while other mode is active

    case ACCEPT:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {
        strip.setPixelColor(i, map(t, 0, 999, COLOUR1_R, COLOUR2_R),            // Flashing Neo Pixel colour WHITE to BLUE
                            map(t, 0, 999, COLOUR1_G, COLOUR2_G),
                            map(t, 0, 999, COLOUR1_B, COLOUR2_B));

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == REMOVE) {                                        // REMOVE
        state = IDLE_STATE;
        //Serial.println("IDLE_STATE");
        // Timer_Running = false;                                   // timer stop safety measure (not needed for now)cancelled out by "Timer_Running = true;"

      }

      if (event == TIME_OUT) {
        state = RECORDING_MODE;
        //Serial.println("RECORDING_MODE");
        Target_Time_Millis = millis() + long_timer;                        // Setting timer for next state (Case)
        Timer_Running = true;
        Serial.write(mystr1, 2); //Write the serial data: "Start recording"
      }
      break;

    case RECORDING_MODE:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {

        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(100, 0, 0)); // Moderately bright RED color.

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == REMOVE) {
        state = IDLE_STATE;
        //Serial.println("IDLE_STATE");
        Serial.write(mystr2, 2); //Write the serial data: "Stop recording"
      }

      if (event == TIME_OUT) {
        state = WAITING_PICKUP;
        //Serial.println("WAITING_PICKUP");
        Target_Time_Millis = millis() + long_timer;                        // Setting timer for next state (Case)
        Timer_Running = true;
        Serial.write(mystr2, 2); //Write the serial data: "Stop recording"
      }
      break;

    case WAITING_PICKUP:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {

        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 100, 0)); // Moderately bright GREEN color.

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == REMOVE) {
        state = PLAYBACK;
        //Serial.println("PLAYBACK");
        Target_Time_Millis = millis() + long_timer;                        // Setting timer for next state (Case)
        Timer_Running = true;
        Serial.write(mystr3, 2); //Write the serial data: "Start playback"
      }
      break;

    case PLAYBACK:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {
        strip.setPixelColor(i, map(t, 0, 999, COLOUR1_R, COLOUR3_R),          // Flashing Neo Pixel colour WHITE to GREEN
                            map(t, 0, 999, COLOUR1_G, COLOUR3_G),
                            map(t, 0, 999, COLOUR1_B, COLOUR3_B));

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == ADD) {
        state = PLAYBACK_PAUSE;
        //Serial.println("PLAYBACK_PAUSE");
        Serial.write(mystr4, 2); //Write the serial data: "Stop playback"
      }

      if (event == TIME_OUT) {
        state = IDLE_STATE;
        //Serial.println("IDLE_STATE");
        Serial.write(mystr4, 2); //Write the serial data: "Stop playback"
      }
      break;                                                          // Stop function while other mode is active

    case PLAYBACK_PAUSE:

      for (int i = 0; i < NUM_STRIP_LEDS; i++) {

        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 100, 0)); // Moderately bright GREEN color.

        strip.show(); // This sends the updated pixel color to the hardware.
      }

      if (event == REMOVE) {
        state = PLAYBACK;
        //Serial.println("PLAYBACK");
        Target_Time_Millis = millis() + long_timer;                        // Setting timer for next state (Case)
        Timer_Running = true;
        Serial.write(mystr3, 2); //Write the serial data: "Start playback"
      }

      break;
  }
}
