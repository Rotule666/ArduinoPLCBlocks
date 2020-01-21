#include <Arduino.h>

#include "ArduinoPLCBlocks.h"

#define LED_PIN 4

FLASHER mytimer(1000,500);


void setup() {

  pinMode(LED_PIN,OUTPUT);


}

void loop() {

  mytimer.IN(true);

  mytimer.exec();

  if (mytimer.Q()){
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }

}