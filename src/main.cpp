#include <Arduino.h>

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BTN_START 2
#define BTN_DIFF 3

// global vars
int score = 0;
unsigned long gameTime;
int gameDuration = 30000;
volatile int difficulty;
volatile int btnStartInterrupt = 0;

// debouncing vars
int btnState = 0, lasBtnState = 0;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 100;
volatile unsigned long interruptTime, lastInterruptTime = 0;

void setRGB(int x);
void startStopISR();

void setup() {
  // pin mode setups
  for(int pin=RED_PIN; pin<=BLUE_PIN; pin++) pinMode(pin, OUTPUT);
  pinMode(BTN_START, INPUT_PULLUP);  // Set to INPUT_PULLUP for stable interrupt

  // serial monitor setup
  Serial.begin(9600);

  // interrupts setup
  attachInterrupt(digitalPinToInterrupt(BTN_START), startStopISR, FALLING);

  setRGB(4);
}

void loop() {
  // nothing here for now
}

void setRGB(int x) {
  switch(x) {
    case 0: // turned off
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case 1: // red
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case 2: // green
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case 3:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    case 4: // white
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    default:
      Serial.println("RGB LED error. Check code.");
  }
}

void startStopISR() {
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) {
    // start-stop game code
  }
  lastInterruptTime = interruptTime;
}
