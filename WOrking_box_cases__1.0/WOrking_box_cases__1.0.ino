const int IDLE_STATE = 0;             // STate machine values
const int ACCEPT = 1;
const int RECORDING_MODE = 2;
const int WAITING_PICKUP = 3;
const int PLAYBACK = 4;
const int PLAYBACK_PAUSE = 5;

const int REMOVE = 0;                                      // Transition events 
const int ADD = 1;
const int IDLE_EVENT = 2; 
const int TIME_OUT = 3;

int state = IDLE_STATE;
unsigned long Target_Time_Millis = 0;
boolean Timer_Running = false;

const int BUTTON_A = 2;               // Pin mode number on arduino 


int pressed_A = LOW;
int transientState_A = LOW;


unsigned long debounceTime_A = 0;

const unsigned long DEBOUNCE_DELAY = 50;


int get_event() { 

  if (Timer_Running && millis() > Target_Time_Millis){
    Timer_Running = false;
    return TIME_OUT;
    
  }
  int x = digitalRead(BUTTON_A);
  

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
      return ADD; 
    } else if (x == LOW && pressed_A == HIGH) {
      pressed_A = LOW;
      return REMOVE;
    }
  }

  return IDLE_EVENT;    
}  


void setup() {
  pinMode(BUTTON_A, INPUT);
  Serial.begin(9600);
}

// Debouncing: reference: https://www.arduino.cc/en/Tutorial/Debounce

void loop() {
 int event = get_event();
//  if (event != 2)
//     Serial.print(event);



switch (state) {                                                    // Call mode 0 on start up
    case IDLE_STATE:

      if (event == ADD) {
        state = ACCEPT;
        Target_Time_Millis = millis() + 2000;                      // Setting timer for next state (Case)
        Timer_Running = true;
        
        Serial.println("ACCEPT");
      }
    
      break;                                                         // Stop function while other mode is active

    case ACCEPT:

        if (event == ADD) {                                          // REMOVE
          state = IDLE_STATE;
          Serial.println("IDLE_STATE");
          // Timer_Running = false;                                  // timer stop safety measure (not needed for now)cancelled out by "Timer_Running = true;"

        } 

        if (event == TIME_OUT) {
         state = IDLE_STATE;
         Serial.println("IDLE_STATE");
         

        }
      break;
      
    case RECORDING_MODE:

      break;

    case WAITING_PICKUP:
    
      break;

    case PLAYBACK:

      break;                                                         // Stop function while other mode is active

    case PLAYBACK_PAUSE:

      break;
      
      
  }

}
