// Define pin numbers for the bi-color LED
const int redPin = 9;
const int greenPin = 10;

void setup() {
  // Initialize the LED pins as outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  // Set the LED pins to HIGH to turn them off initially
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
}

void loop() {
  // Turn on the red LED (set LOW to light up)
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  delay(1000); // Wait for 1 second

  // Turn on the green LED (set LOW to light up)
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  delay(1000); // Wait for 1 second

  // Turn on both LEDs to create a yellow color (set LOW to light up)
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  delay(1000); // Wait for 1 second

  // Turn off both LEDs (set HIGH to turn off)
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  delay(1000); // Wait for 1 second
}
