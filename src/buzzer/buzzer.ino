// Pin where the passive buzzer is connected
const int buzzerPin = 9;
// Pin where the button is connected
const int buttonPin = 7;

// Function to play a fail sound
void playFailSound() {
  // Frequencies for the fail sound
  int frequencies[] = {523, 494, 466, 440, 415, 392, 370, 349};
  int duration = 200; // Duration of each tone in milliseconds

  for (int i = 0; i < 8; i++) {
    tone(buzzerPin, frequencies[i], duration);
    delay(duration * 1.3); // Add a slight delay between tones
  }

  // Ensure the buzzer is off after the sequence
  noTone(buzzerPin);
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Initialize the button pin as input with internal pull-up resistor
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    // Play the fail sound if the button is pressed
    playFailSound();

    // Add a delay to avoid continuous playback while the button is held down
    delay(500); // 0.5 seconds delay before checking the button again
  }
}
