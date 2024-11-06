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
String input = "", genWord;
int score = 0;
unsigned long gameTime;
int gameDuration = 30000;
volatile int difficulty = 0; 
volatile int gameJustStarted = 0, gameOn = 0;
const int blinkDuration = 3000;
const int blinkInterval = 500;
unsigned long typeTime;
unsigned long gameStartTime;
String words[] = {
    "inquiry", "illustrate", "performer", "survey", "role", "admire", "hand", "accident",
    "situation", "factory", "ignore", "guerilla", "dragon", "face", "exit", "bounce",
    "mosaic", "unanimous", "gravel", "square", "house", "realism", "mountain", "trust",
    "denial", "wall", "action", "golf", "storage", "positive", "metal", "map", "arch",
    "crew", "improvement", "chicken", "banquet", "temperature", "composer", "confusion",
    "reproduction", "coverage", "exaggeration", "profession", "preference", "preparation",
    "practice", "equinox", "trench", "crutch", "robotics", "helicopter", "zeromunca", "glue",
    "admire", "door", "scandal", "combat", "ballet", "meaning", "coffee", "office",
    "morality", "gift", "congress", "imposter", "haircut"
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
String getRandomString(const String wordList[], int size);
void startGame();
void game();

void setup() {
noInterrupts();

  // pin mode setups
  for (int pin = RED_PIN; pin <= BLUE_PIN; pin++) pinMode(pin, OUTPUT);

  // serial monitor setup
  Serial.begin(9600);

  // initial state setup
  setRGB(4); // turn the LED white

  // interrupts setup
  attachInterrupt(digitalPinToInterrupt(BTN_START), startStopISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DIFF), diffSwitchISR, FALLING);

  Serial.println("Awaiting input...");
  randomSeed(analogRead(0)); // for random generation using noise on pin A0

  interrupts();
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
    if (gameOn || gameJustStarted) {
      gameOn = 0;  // Stop the game immediately
      gameJustStarted = 0;
      setRGB(4);   // Set LED to white
      Serial.println("\nGame Stopped!");
    } else {
      gameJustStarted = 1;  // Start the game if it is not running
    }
  }
  lastInterruptTime = interruptTime; 
}

void diffSwitchISR() { // difficulty switch interrupt routine
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > debounceDelay) { // debouncing
    if(!gameJustStarted && !gameOn) { // change difficulty only while game not active
      difficulty += 1 ;
      if(difficulty >= 4) difficulty = 1; // do not let difficulty fall out of range
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
          Serial.println("Error: difficulty variable out of range."); // just in case :)
      }
    }
  }
  lastInterruptTime = interruptTime;
}

String getRandomString(const String wordList[], int size) { // returns a random word from a string array
  int randomIndex = random(size);
  return wordList[randomIndex];
}

void startGame() { // blink loop at the beginning of the game
  static unsigned long startTime = 0;
  static unsigned long lastTime = 0;
  static int blinking = 0, state = 0;
  static int i = 3; // countdown variable
  gameStartTime = millis();  // Record the game start time

  if (!blinking) { // start blinking process
    blinking = 1;
    startTime = millis();
    lastTime = millis();
    i = 3;
    Serial.println("Game starting...\n");
  }

  if (millis() - startTime >= blinkDuration) { // runs when blinking has finished
    Serial.println("START!");
    blinking = 0;
    gameJustStarted = 0; // Resets the start flag
    gameOn = 1; // Allows the game loop to run
  }

  if (millis() - lastTime >= blinkInterval) {
    lastTime = millis();
    if (!state) {
      setRGB(4); // turns the LED white
      state = 1;
      Serial.print(i);
      Serial.print("...\n");
      i--;
    } else {
      setRGB(0); // turns the LED off
      state = 0;
    }
  }
}

void game() { // main function - loops until gameDuration is exceeded.
  
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

  // Checks if the game has exceeded its duration
  if (millis() - gameStartTime >= gameDuration) {
    gameOn = 0;  // Ends the game
    setRGB(4);   // Turns the LED white again
    Serial.println("\nGame Over!");
    Serial.print("Final Score: ");
    Serial.println(score);
    return;  // exits the game func
  }

  // Generate a word at the specified interval
  if (millis() - lastPrintTime >= typeTime) {
    int size = sizeof(words) / sizeof(words[0]);
    genWord = getRandomString(words, size); // Get random word
    input = "";
    Serial.println("");
    Serial.println(genWord); // Print the random genWord
    lastPrintTime = millis();
  }

  // Check if there is serial input and handle it immediately
  if (Serial.available() > 0) {
    char c = Serial.read(); // read a character
    if (c == 8 && input.length() > 0) { // handle backspace
      input.remove(input.length()-1);
      Serial.print("\b \b");
    } else if (c != '\n' && c != '\r') { // ignore newline characters
      input += c;
      Serial.print(c); // Print the character to Serial monitor
    }
    
    // Check if the input matches the generated word
    if (genWord == input) {
      // Serial.println("corret!");
      setRGB(2); // set RGB LED to green
      score++;
      input = ""; // reset input after correct word is typed
      Serial.println("");
      lastPrintTime = 0; // immediately print a new word
    } else if (input.length() > 0 && !genWord.startsWith(input)) { // detectes mismatches
      // Serial.println("Mismatch detected!");
      setRGB(1); // RGB red
    } else setRGB(2); // RGB green
  } 
}

