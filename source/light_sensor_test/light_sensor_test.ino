// State machine architechture --- look into saving state on power off so that an experiment could be resumed afer power-on 

#include "ArduinoLowPower.h"

#define LIGHT A1  // setting light to analog input 1

float supply_voltage = 3.3;  // voltage supplied by adalogger
float light;  // declaring the light variable globally
unsigned long my_time;  // using this variable to keep track of how long the device has been running
int run_time = 30000;  // Experiment time, in milliseconds
const int WAKE = 10;  // pin for wake signal
const int TIME_RESET = 11;  // pin for time reset signal
int wake_flag = 0;  // used to kickoff experiment in loop()



void setup() {
  pinMode(LIGHT, INPUT);  // set light sensor pin to input
  pinMode(WAKE, INPUT_PULLUP);  // set wake pin to input pullup for interrupt, include ability to 'reset' state if needed
  pinMode(TIME_RESET, INPUT_PULLUP); 

  LowPower.attachInterruptWakeup(WAKE, wake, CHANGE);  // set WAKE to interrupt, run wake() when interrupted. just short the pin to ground to trigger
  attachInterrupt(digitalPinToInterrupt(TIME_RESET), time_reset, CHANGE);
  Serial.begin(9600); 
}



void loop() {
  if(wake_flag == 1){  // if the wake signal has been received
    my_time = millis();  // millis gives the time since wake-up
    if(my_time >= run_time){  // if the experiment has been running past the desired time
      Serial.println("Experiment Complete");
      wake_flag = 0;
      my_time = millis()- my_time;
      Serial.println(my_time);
      //LowPower.sleep();  // go back to sleep
    }
    else{  // if the experiment is not over,
      light = get_light();  // pull the value from the light sensor
      Serial.print(my_time); Serial.print(", "); Serial.println(light);
      delay(1000);
    }
  }
  else {
    LowPower.sleep();  // keep the device in low power mode if the wake flag has not gone up
  }
}


// Method for getting lux value from light sensor. Probably not valid, as this was written originally for an arduino uno
// Still works for this purpose
float get_light() {
  float light_voltage = analogRead(LIGHT) * supply_voltage / 1023.0;
  float amps = light_voltage / 10000.0; // output voltage across 10k resistor
  float u_amps = amps * 1000000; 
  float lux = u_amps * 2.0;
  return lux;
}


// Method for raising the wake flag after the wake signal is received
void wake() {
  Serial.println("Starting Experiment");
  wake_flag = 1;  
}



void time_reset(){
  Serial.println("Reset time");
  // currently marking where time was reset, but not setting time variable to 0. This may be good enough 
  // millis() is not resettable, so if we want to set the time to 'zero' we need to add math to offset the time variable
}
