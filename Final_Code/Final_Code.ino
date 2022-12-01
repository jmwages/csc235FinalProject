// include libraries
#include <TM1637.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// create variables for bumeprs
int bumpTrigPin = 2;
boolean buttonState = false;
boolean buttonStateOld = false;

// create variable for reset button
int resetPin = 10;

// create variable for buzzer
int buzzerPin = 13;

// create neopixel variables
int npNum = 10;
int npPin = 4;
Adafruit_NeoPixel strip(npNum, npPin);

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t purple = strip.Color(255, 0, 255);
uint32_t yellow = strip.Color(255, 255, 0);

// create variables for points
unsigned long points = 0;
int bonusMultiplier = 1;

// create variables for scoreboard
int CLK = 6;
int DIO = 7;
int DIO2 = 8;
TM1637 scoreboard(CLK, DIO);
TM1637 scoreboard2(CLK, DIO2);

void setup() {
  // initialize inputs
  pinMode(bumpTrigPin, INPUT);
  pinMode(resetPin, INPUT);

// start serial monitor
  Serial.begin(9600);

// start neopixels
  strip.begin();
  strip.setBrightness(50);
  strip.clear();

// start scoreboards
  scoreboard.init();
  scoreboard.set(5);

  scoreboard2.init();
  scoreboard2.set(5);
}

void loop() {
  lights();
  bumpers();

  showScore();
  resetScore();
}

// function to control lights
void lights() {
  switch (bonusMultiplier) {
    case 1 :
      lightPattern(500, green, blue);
      break;
    case 2:
      lightPattern(300, blue, purple);
      break;
    case 3:
      lightPattern(200, red, blue);
      break;
    case 4:
      lightPattern(100, yellow, blue);
  }
}

// function for light patterns
void lightPattern(int interval, uint32_t color1, uint32_t color2) {
  static uint32_t c1 = color1;
  static uint32_t c2 = color2;

  static unsigned long startTime = millis();
  static boolean stateOne = true;

  for (int i = 0; i < strip.numPixels(); i++) {
    if (i % 2 == 0) {
      strip.setPixelColor(i, c1);
    } else if (i % 2 == 1) {
      strip.setPixelColor(i, c2);
    }
  }

  if (millis() - startTime >= interval) {
    if (stateOne) {
      c1 = color1;
      c2 = color2;
      stateOne = false;
    } else {
      c1 = color2;
      c2 = color1;
      stateOne = true;
    }

    startTime = millis();
  }

  strip.show();
}

// function for sensing buttons and adding score
void bumpers() {
  
  buttonStateOld = buttonState;
  buttonState = digitalRead(bumpTrigPin);

  static unsigned long startTime = millis();
  int interval = 800;

  if (buttonStateOld == 0 && buttonState == 1 /*buttonState == HIGH8*/) {
    points += (10 * bonusMultiplier);

    if (bonusMultiplier < 4) {
      bonusMultiplier++;
    }
    playTone();
    startTime = millis();
  }

  if (millis() - startTime >= interval) {
    bonusMultiplier = 1;
  }
}

// function to play tone
void playTone() {
  tone(buzzerPin, 660 + 220 * (bonusMultiplier - 1), 100);
}

// debounce function ((didn't use))
boolean debounce(int aButtonPin, boolean aPrevState) {
  // gets current bouncy state
  boolean currentState = digitalRead(aButtonPin);

  // if button is messed up delay program for 10 ms
  if (currentState != aPrevState) {
    delay(10);
  }

  return (currentState);
}

// function to reset score when button is pressed
void resetScore() {
  boolean resetState = digitalRead(resetPin);
  if (resetState == HIGH) {
    points = 0;
  }
}

// function to show score on scoreboard
void showScore() {
  int digit3 = (points / 1U) % 10;
  int digit2 = (points / 10U) % 10;
  int digit1 = (points / 100U) % 10;
  int digit0 = (points / 1000U) % 10;

  scoreboard.display(3, digit3);
  scoreboard.display(2, digit2);
  scoreboard.display(1, digit1);
  scoreboard.display(0, digit0);

  int digit3II = (points / 10000U) % 10;
  int digit2II = (points / 100000U) % 10;
  int digit1II = (points / 1000000U) % 10;
  int digit0II = (points / 10000000U) % 10;

  scoreboard2.display(3, digit3II);
  scoreboard2.display(2, digit2II);
  scoreboard2.display(1, digit1II);
  scoreboard2.display(0, digit0II);
}
