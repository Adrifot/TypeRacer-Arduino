#include <Arduino.h>
#include <string.h>

// define pins
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BTN_START 2
#define BTN_DIFF 3

// define other stuff
#define MAX_STRING_LENGTH 25
#define MAX_STRINGS 100

// global vars
int score = 0;
unsigned long gameTime;
int gameDuration = 30000;
volatile int difficulty = 0; // intervals: easy - 5s, medium - 3s, hard - 1.5s
volatile int gameJustStarted = 0, gameOn = 0;
const int blinkDuration = 3000;
const int blinkInterval = 500;
const char* words[] = {
    "inquiry", "illustrate", "performer", "survey", "role", "admire", "hand", "accident",
    "situation", "factory", "ignore", "guerilla", "dragon", "face", "exit", "bounce",
    "mosaic", "unanimous", "gravel", "square", "house", "realism", "mountain", "trust",
    "denial", "wall", "action", "golf", "storage", "positive", "metal", "map", "arch",
    "crew", "improvement", "chicken", "banquet", "temperature", "composer", "confusion",
    "reproduction", "coverage", "exaggeration", "profession", "preference", "preparation",
    "practice", "equinox", "trench", "crutch"
};

// debouncing vars
int btnState = 0, lasBtnState = 0;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 100;
volatile unsigned long interruptTime, lastInterruptTime = 0;

// function headers
void setRGB(int x);
void startStopISR();
void diffSwitchISR();
const char* getRandomString(const char* wordList[], int size);
void startGame();
void game();

void setup() {
  // pin mode setups
  for (int pin = RED_PIN; pin <= BLUE_PIN; pin++) pinMode(pin, OUTPUT);

  // serial monitor setup
  Serial.begin(9600);

  // initial state setup
  setRGB(4); // turn the LED white

  // interrupts setup
  attachInterrupt(digitalPinToInterrupt(BTN_START), startStopISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIFF), diffSwitchISR, FALLING);
}

void loop() {
  if (gameJustStarted) {
    startGame(); // Start game logic
  }
  
  if (gameOn) {
    game(); // Continue game logic
  }
}

void setRGB(int x) { // function to set RGB LED to a color
  switch (x) {
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
    gameJustStarted = 1;
  }
  lastInterruptTime = interruptTime;
}

void diffSwitchISR() { // difficulty switch interrupt routine
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) { // debouncing
    difficulty += 1 ;
    if(difficulty >= 4) difficulty = 1;
    switch(difficulty) {
      case 1:
        Serial.println("Easy mode ON. Have fun.");
        break;
      case 2:
        Serial.println("Medium mode ON. Ready for a challenge?");
        break;
      case 3:
        Serial.println("Hard mode ON. Prepare to lose.");
        break;
      default:
        Serial.println("Error: difficulty variable out of range.");
    }
  }
  lastInterruptTime = interruptTime;
}

const char* getRandomString(const char* wordList[], int size) { // returns a random word from a string array
  int randomIndex = random(size);
  return wordList[randomIndex];
}

void startGame() { // blink loop at the beginning of the game
  static unsigned long startTime = 0;
  static unsigned long lastTime = 0;
  static int blinking = 0, state = 0;
  static int i = 3;

  if (!blinking) {
    blinking = 1;
    startTime = millis();
    lastTime = millis();
    i = 3;
    Serial.println("Game starting...\n");
  }

  if (millis() - startTime >= blinkDuration) {
    Serial.println("START!");
    blinking = 0;
    gameJustStarted = 0; // Reset the start flag
    gameOn = 1; // Allow the game loop to run
  }

  if (millis() - lastTime >= blinkInterval) {
    lastTime = millis();
    if (!state) {
      setRGB(4); // turn the LED white
      state = 1;
      Serial.print(i);
      Serial.print("...\n");
      i--;
    } else {
      setRGB(0); // turn the LED off
      state = 0;
    }
  }
}

void game() { // main game loop
  unsigned long typeTime;
  switch (difficulty) {
    case 1:
      typeTime = 5000; // easy - 5 seconds
      break;
    case 2:
      typeTime = 3000; // medium - 3 seconds
      break;
    case 3:
      typeTime = 1500; // hard - 1.5 seconds
      break;
    default:
      typeTime = 2000; // for testing purposes
  }

  static unsigned long lastPrintTime = 0;
  static unsigned long gameStartTime = millis();  // Record the game start time

  // Set LED to green while the game is on and generating words
  setRGB(2);  // Set LED to green

  // Check if the game has exceeded its duration
  if (millis() - gameStartTime >= gameDuration) {
    gameOn = 0;  // End the game
    setRGB(4);   // Turn the LED white again
    Serial.println("Game Over!");
    return;  // Exit the game function, stop word generation
  }

  // Generate a word at the specified interval
  if (millis() - lastPrintTime >= typeTime) {
    int size = sizeof(words) / sizeof(words[0]);
    const char* word = getRandomString(words, size); // Get random word
    Serial.println(word); // Print the random word
    lastPrintTime = millis();
  }
}
