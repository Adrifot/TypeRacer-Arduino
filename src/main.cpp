#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

// define pins
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BTN_START 2
#define BTN_DIFF 3

// define other stuff
#define MAX_STRING_LENGTH 25

// global vars
int score = 0;
unsigned long gameTime;
int gameDuration = 30000;
volatile int difficulty;
volatile int gameStarted = 0;
const int blinkDuration = 3000;
const int blinkInterval = 500;


// debouncing vars
int btnState = 0, lasBtnState = 0;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 100;
volatile unsigned long interruptTime, lastInterruptTime = 0;

// function headers
void setRGB(int x);
void startStopISR();
int loadStrings(const char* filename, char strings[][MAX_STRING_LENGTH], int maxStrings);
void getRandomString(char strings[][MAX_STRING_LENGTH], int count);
void startGame();

void setup() {
  // pin mode setups
  for(int pin=RED_PIN; pin<=BLUE_PIN; pin++) pinMode(pin, OUTPUT);

  // serial monitor setup
  Serial.begin(9600);

  // interrupts setup
  attachInterrupt(digitalPinToInterrupt(BTN_START), startStopISR, FALLING);

  // initial state setup
  setRGB(4); // turn the LED white
}

void loop() {
  if(gameStarted) startGame();
}

void setRGB(int x) { // function to set RGB LED to a color
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
    case 3: // blue
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
      Serial.println("Error: RGB LED error.");
  }
}

void startStopISR() { // interrupt routine for the Start/Stop button
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) { // debouncing
    gameStarted = 1;
  }
  lastInterruptTime = interruptTime;
}

/*  function: loadStrings()
    - loads strings from a file into memory 
    - returns: INT count (quantity of read strings) or -1 (on error)
*/
int loadStrings(const char* filename, char strings[][MAX_STRING_LENGTH], int maxStrings) {
  FILE *file = fopen(filename, "r");
  if(file == NULL) {
    Serial.println("Error: cannot open file.");
    return -1;
  }

  int count = 0;
  while(count < maxStrings && fscanf(file, "%49s", strings[count])) count++;
  fclose(file);
  return count;
}

void getRandomString(char* stringVar,char strings[][MAX_STRING_LENGTH], int count) {
  if(count <= 0) {
    Serial.println("Error: the provided file is empty.");
    return;
  }
  int randomIndex =  random(count);
  strcpy(stringVar, strings[randomIndex]);
}

void startGame() {
  static unsigned long startTime = 0;
  static unsigned long lastTime = 0;
  static int blinking = 0, state = 0;
  static int i = 3;

  if(!blinking) {
    blinking = 1;
    startTime = millis();
    lastTime = millis();
    i = 3;
    Serial.println("Game starting...\n");
  }

  if(millis() - startTime >= blinkDuration) {
    setRGB(2);
    blinking = 0;
    gameStarted = 0;
    Serial.println("START!");
    return;
  }

  if(millis() - lastTime >= blinkInterval) {
    lastTime = millis();
    if(!state) {
      setRGB(4);
      state = 1;
      Serial.print(i);
      Serial.print("...\n");
      i--;
    } else {
      setRGB(0);
      state = 0;
    }
  }
}