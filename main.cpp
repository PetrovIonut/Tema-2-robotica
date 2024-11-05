#include <Arduino.h>

// RGB LED Pins
const int ledRedPin = 6;
const int ledGreenPin = 5;
const int ledBluePin = 4;
const int rgbLedPins[] = { ledRedPin, ledGreenPin, ledBluePin };

// Button Pins
const int startStopButtonPin = 3;
const int difficultyButtonPin = 2;
const int buttonPins[] = { startStopButtonPin, difficultyButtonPin };

// Debounce variables for buttons
volatile unsigned long lastDebounceTimeCycle = 0;
const unsigned long debounceDurationCycle = 50;

volatile unsigned long lastDebounceTimeStartStop = 0;
const unsigned long debounceDurationStartStop = 50;

// Game timing and configuration
const int gameDurationMs = 30000; // Round length in milliseconds
const int startAnimationDurationSecs = 3; // Duration of start animation countdown in seconds

const int numWords = 50;
const String wordList[numWords] = {
  "velvet", "sword", "ignite", "silver", "slam", "missile", "build", "stereotype", "remain", "reference",
  "trap", "bite", "genetic", "reduce", "bother", "confrontation", "faithful", "way", "stubborn", "breathe",
  "coup", "franchise", "abolish", "reader", "bolt", "hostile", "time", "shy", "quarter", "communication",
  "safety", "problem", "captain", "overcharge", "tread", "coin", "line", "association", "acquaintance",
  "station", "hobby", "prison", "trunk", "appoint", "disappoint", "collection", "treat", "charm", "light", "reliable"
};

enum GameDifficulty {
  UNKNOWN = -1,
  EASY = 0,
  MEDIUM = 1,
  HARD = 2
};

// State variables for the game
volatile bool triggerDifficultyChange = false;
volatile bool triggerStart = false;
volatile bool triggerStop = false;
volatile bool gameInProgress = false;
volatile bool triggerWordChange = false;

GameDifficulty difficulty = GameDifficulty::UNKNOWN;
int wordDuration = 0;

long roundStartMillis = 0;
int score = 0;
String displayedWord = "";
String userInput = "";

// Set the RGB LED to the specified color
void updateRgbLed(int red, int green, int blue) {
  digitalWrite(ledRedPin, red);
  digitalWrite(ledGreenPin, green);
  digitalWrite(ledBluePin, blue);
}

// Interrupt handler for difficulty change button
void handleDifficultyChange() {
  if (millis() - lastDebounceTimeCycle > debounceDurationCycle) {
    lastDebounceTimeCycle = millis();
    
    if (!gameInProgress) {
      triggerDifficultyChange = true;
    }
  }
}

// Cycle through difficulties when triggered
void changeDifficulty() {
  triggerDifficultyChange = false;
  
  difficulty = (GameDifficulty)(((int)difficulty + 1) % 3); // returns a value between 0-2

  switch (difficulty) {
    case GameDifficulty::EASY:
      Serial.println("Easy mode on!");
      wordDuration = 4000;
      break;
    case GameDifficulty::MEDIUM:
      Serial.println("Medium mode on!");
      wordDuration = 3000;
      break;
    case GameDifficulty::HARD:
      Serial.println("Hard mode on!");
      wordDuration = 2000;
      break;
    default:
      Serial.println("Unknown difficulty. Please select again!");
      difficulty = GameDifficulty::UNKNOWN;
      wordDuration = 0;
      break;
  }
}

// Interrupt handler for start/stop button
void handleStartStop() {
  if (millis() - lastDebounceTimeStartStop > debounceDurationStartStop) {
    lastDebounceTimeStartStop = millis();
    
    if (gameInProgress) {
      triggerStop = true;
    } else {
      triggerStart = true;
    }
  }
}

// Begin the game when triggered
void startGame() {
  triggerStart = false;

  if (difficulty == GameDifficulty::UNKNOWN) {
    Serial.println("Select a difficulty level first.");
    return;
  }

  score = 0;
  userInput = "";

  for (int i = startAnimationDurationSecs; i > 0; i--) {
    Serial.println(i);
    updateRgbLed(HIGH, HIGH, HIGH);
    delay(500);
    updateRgbLed(LOW, LOW, LOW);
    delay(500);
  }

  updateRgbLed(LOW, HIGH, LOW);
  
  noInterrupts();

  TCCR1A = 0;   //reset timer 1 reg A 
  TCCR1B = 0;   // reset timer 1 reg B
  TCNT1 = 0;     // reset timer 1 counter

  const unsigned long systemClock = 16000000;
  const int prescalerValue = 1024;
  const unsigned long compareMatch = (systemClock / prescalerValue) * wordDuration / 1000 - 1;

  OCR1A = compareMatch;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);

  interrupts();

  roundStartMillis = millis();
  gameInProgress = true;
  triggerWordChange = true;
}

// Timer interrupt for changing words
ISR(TIMER1_COMPA_vect) {
  triggerWordChange = true;
}

// Pick a new word when triggered
void selectNewWord() {
  triggerWordChange = false;
  TCNT1 = 0;

  int randomIndex = random(0, numWords);
  displayedWord = wordList[randomIndex];

  userInput = "";

  Serial.print("\n\n");
  Serial.println(displayedWord);
}

// Reset game state when stopped
void resetGameState() {
  updateRgbLed(HIGH, HIGH, HIGH);
  difficulty = GameDifficulty::UNKNOWN;
  wordDuration = 0;
  triggerDifficultyChange = false;
  triggerStart = false;
  triggerStop = false;
  gameInProgress = false;
  score = 0;
  triggerWordChange = false;
  displayedWord = "";
  userInput = "";

  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 0;
  TIMSK1 &= ~(1 << OCIE1A);

  interrupts();

  Serial.println("Select difficulty to begin.");
}

// Stop game and display score
void endGame() {
  Serial.print("\n\nFinal score: ");
  Serial.println(score);

  resetGameState();
}

String repeatChar(char c, int count) {
  String result = "";
  for (int i = 0; i < count; i++) {
    result += c;
  }
  return result;
}
// Setup function
void setup() {
  Serial.begin(28800);

  randomSeed(analogRead(0));

  for (int i = 0; i < 3; i++) {
    pinMode(rgbLedPins[i], OUTPUT);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  attachInterrupt(digitalPinToInterrupt(difficultyButtonPin), handleDifficultyChange, LOW);
   //attaches a handler to a digital pin, converts pin number in an interrupt number,
   // calls function when button state is LOW(button pressed)
  attachInterrupt(digitalPinToInterrupt(startStopButtonPin), handleStartStop, LOW);

  resetGameState();
}

// Main program loop
void loop() {
  if (triggerDifficultyChange) {
    changeDifficulty();
  }

  if (triggerStart) {
    startGame();
  }

  if (triggerStop) {
    endGame();
  }

  if (gameInProgress) {
    if (triggerWordChange) {
      selectNewWord();
    }

    if ((millis() - roundStartMillis >= gameDurationMs) && !triggerStop) {
      triggerStop = true;
    }

    if (Serial.available()) {
      char inputChar = Serial.read();

      if (inputChar == '\b') {
        if (userInput.length() > 0) {
          userInput.remove(userInput.length() - 1);
          
          Serial.print("\r" + repeatChar(' ', userInput.length()) + "\r" + userInput); // 
        }
      } else if (inputChar == '\n') {
        userInput = "";
      } else {
        userInput += inputChar;
      }

      if (displayedWord.startsWith(userInput)) {
        updateRgbLed(LOW, HIGH, LOW);
      } else {
        updateRgbLed(HIGH, LOW, LOW);
      }

      if (displayedWord.equals(userInput)) {
        score++;
        triggerWordChange = true;
      }
    }
  }
}
