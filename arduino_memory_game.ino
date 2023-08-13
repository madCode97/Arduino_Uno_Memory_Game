#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int score = 0;
int highscore = 0;

int ledPins[] = {2, 3, 4, 5}; // 4 LED's wired to digital pin 2-5
int buttonPins[] = {6, 7, 8, 9}; // 4 buttons wired to digital pin 6-9
int speakerPins[] = {10, 11, 12, 13}; // 4 speakers wired to digital pin 10-13

const int maxMemory = 100; // How many button presses the memory can hold for the sequence
int sequence[maxMemory];
int recordingArray[maxMemory];

int intRound = 0; // Keep track of how many buttons are stored in the array
bool sequencePlaying = true, recordingSequence = false;
bool newHighscore = false;
bool ready = false;

void setup() {

  Serial.begin(9600);

  // Make sure the screen works
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Screen not found!"));
    while(1);
  }
  display.display();
  display.setTextColor(SSD1306_WHITE);

  ledStartup(); // LED startup lightshow
  ready = true;
}

void loop() {
  if (!sequencePlaying) {
    recordSequence();
  } else {
    generateSequence(); // adds a random button to the memory
    screenUpdate(); // Displays the score and state to the OLED Screen
    playSequence(); // Plays the sequence stored in memory
  }
}

void playSequence() {
  sequencePlaying = true;
  for (int i = 0; i < intRound; i++) {
    int buttonIndex = sequence[i];
    digitalWrite(ledPins[buttonIndex], HIGH);
    digitalWrite(speakerPins[buttonIndex], HIGH);
    delay(1000);
    digitalWrite(ledPins[buttonIndex], LOW);
    digitalWrite(speakerPins[buttonIndex], LOW);
    delay(750);
  }
  sequencePlaying = false;
}

int prevState = -1; // -1 none, 0 button1, 1 button2, 2 button3, 3 button4
int recordedPresses = 0;
void recordSequence() {
  
  // Get all the button input states
  int buttonState[4] = {
    digitalRead(buttonPins[0]), digitalRead(buttonPins[1]), digitalRead(buttonPins[2]), digitalRead(buttonPins[3])
  };

  if (prevState == -1) {
    for (int i = 0; i < 4; i++) { // Do this 4 times
      if (buttonState[i] == HIGH) {
        prevState = i;
        digitalWrite(ledPins[i], HIGH);
        digitalWrite(speakerPins[i], HIGH);
        recordingArray[recordedPresses] = i;
        if (validatePress() == false) {
          gameLose();
          return;
        }
        recordedPresses++;
        // Verify
        return;
      }
    }
  } else {
    for (int j = 0; j < 4; j++) {
      if (buttonState[j] == HIGH) {
        return;
      }
    }
    digitalWrite(ledPins[prevState], LOW);
    digitalWrite(speakerPins[prevState], LOW);
    prevState = -1;
  }
}

bool validatePress() {
  if (recordingArray[recordedPresses] == sequence[recordedPresses]) {
    return false;
  } else {
    return true;
  }
}

void gameLose() {
  if (highscore < score) {
    highscore = score;
    newHighscore = true;
  }
  score = 0;
  memset(sequence, 0, sizeof(sequence)); // reset array
  memset(recordingArray, 0, sizeof(recordingArray)); // reset array
  intRound = 0; // Keep track of how many buttons are stored in the array
  sequencePlaying = true;
  recordingSequence = false;
  ledStartup();
  ready = true;
}

void generateSequence() {
  sequence[intRound] = random(1, 4);
  intRound++;
}

void screenUpdate() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Score: " + String(score));
  display.println("Highscore: " + String(highscore));
  display.setTextSize(2);
  if (sequencePlaying) {
    display.println(F("Please Wait..."));
  } else {
    display.println(F("Repeat the pattern!"));
  }
}

void ledStartup() {
  delay(100);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH);
      digitalWrite(speakerPins[j], HIGH);
      delay(100);
      digitalWrite(ledPins[j], LOW);
      digitalWrite(speakerPins[j], LOW);
    }
  }
}