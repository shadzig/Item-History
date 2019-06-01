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

// Current case State change
int state = IDLE_STATE;

// Time Figures
unsigned long Target_Time_Millis = 0;
boolean Timer_Running = false;


// Button pressed
const int BUTTON_A = 2;                                     // Pin number on arduino 
int pressed_A = LOW;
int transientState_A = LOW;


// Debounce button constant state switch
unsigned long debounceTime_A = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Pressure Sensor
int pressurePin = A0;
int force;
int LEDdata;
int LEDpin = 5;
int pressureDigitalConverter;

int get_event() { 

  if (Timer_Running && millis() > Target_Time_Millis){
    Timer_Running = false;
    return TIME_OUT;
    
  }
  int x = force;
  

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
      Serial.println("HIGH");
      return ADD; 
    } else if (x == LOW && pressed_A == HIGH) {
      pressed_A = LOW;
      Serial.println("LOW");
      return REMOVE;
    }
  }

  return IDLE_EVENT;    
}  

// ------------------------------------------------------------------------------------------------------------------------------



void setup() {
  pinMode(BUTTON_A, INPUT);
  Serial.begin(9600);
  strip.begin();
  strip.show();
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

void PRESSURE_SENSOR() {
    LEDdata = analogRead(pressurePin);
    Serial.println(LEDdata);
        if(LEDdata > 550)
        {
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


// Debouncing: reference: https://www.arduino.cc/en/Tutorial/Debounce
// ------------------------------------------------------------------------------------------------------------------------------



void loop() {

  int event = get_event();
//  if (event != 2)
//     Serial.print(event);

  unsigned long now= millis();                                        // Time value for NEO Pixel flash animation
  int t = now % 1000;

  PRESSURE_SENSOR();
  Serial.println(force);
  
switch (state) {                                                      // Call mode 0 on start up
    case IDLE_STATE:
      
      for(int i=0;i<NUM_STRIP_LEDS;i++){

        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(100,100,100)); // Moderately bright white color.

        strip.show(); // This sends the updated pixel color to the hardware.
      }
            
      if (event == ADD) {
        state = ACCEPT;
        Target_Time_Millis = millis() + 2000;                         // Setting timer for next state (Case)
        Timer_Running = true;
        
        Serial.println("ACCEPT");
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
          Serial.println("IDLE_STATE");
          // Timer_Running = false;                                   // timer stop safety measure (not needed for now)cancelled out by "Timer_Running = true;"

        } 

        if (event == TIME_OUT) {
         state = RECORDING_MODE;
         Serial.println("RECORDING_MODE");
         Target_Time_Millis = millis() + 30000;                        // Setting timer for next state (Case)
         Timer_Running = true;
        }
      break;
      
    case RECORDING_MODE:

        for(int i=0;i<NUM_STRIP_LEDS;i++){

            // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(100,0,0)); // Moderately bright RED color.

            strip.show(); // This sends the updated pixel color to the hardware.
      }
          
         if (event == REMOVE) {
         state = IDLE_STATE;
         Serial.println("IDLE_STATE");
      }

         if (event == TIME_OUT) {
         state = WAITING_PICKUP;
         Serial.println("WAITING_PICKUP");
         Target_Time_Millis = millis() + 30000;                        // Setting timer for next state (Case)
         Timer_Running = true;
        }
      break;

    case WAITING_PICKUP:
    
         for(int i=0;i<NUM_STRIP_LEDS;i++){

            // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(0,100,0)); // Moderately bright GREEN color.

            strip.show(); // This sends the updated pixel color to the hardware.
      }
         
         if (event == REMOVE) {
         state = PLAYBACK;
         Serial.println("PLAYBACK");
         Target_Time_Millis = millis() + 30000;                        // Setting timer for next state (Case)
         Timer_Running = true;
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
         Serial.println("PLAYBACK_PAUSE");
      }

         if (event == TIME_OUT) {
         state = IDLE_STATE;
         Serial.println("IDLE_STATE");
      }
      break;                                                          // Stop function while other mode is active

    case PLAYBACK_PAUSE:

         for(int i=0;i<NUM_STRIP_LEDS;i++){

            // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
            strip.setPixelColor(i, strip.Color(0,100,0)); // Moderately bright GREEN color.

            strip.show(); // This sends the updated pixel color to the hardware.
         }   
    
         if (event == REMOVE) {
         state = PLAYBACK;
         Serial.println("PLAYBACK");
         Target_Time_Millis = millis() + 30000;                        // Setting timer for next state (Case)
         Timer_Running = true;
      }

      break;      
  }
}
