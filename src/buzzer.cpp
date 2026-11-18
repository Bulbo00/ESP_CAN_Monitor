#include "buzzer.h"

static int buzzerPin;

void buzzerInit(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

void VoltageLow_sound() {
  for(int i=1;i<5;i++){
    tone(buzzerPin, 1250, 200);
    tone(buzzerPin,  750, 200);
  }
  
}

void startup_sound(){
  tone(buzzerPin, 523, 200);
  delay(150);
  tone(buzzerPin, 659, 200);
  delay(150);
  tone(buzzerPin, 784, 200);
  delay(150);
  tone(buzzerPin, 1047, 200);
  delay(150);
  tone(buzzerPin, 1319, 200);
  delay(1000);
}