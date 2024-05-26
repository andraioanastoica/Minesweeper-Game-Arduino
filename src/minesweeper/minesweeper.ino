#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Constants
#define DIMENSION_ROWS 4
#define DIMENSION_COLS 7
#define MINES 4

// LCD and board setup
LiquidCrystal_I2C lcd(0x3F, 16, 2);
char board_mines[DIMENSION_ROWS][DIMENSION_COLS];
char board[DIMENSION_ROWS][DIMENSION_COLS];

// Joystick pins
int X_pin = 0; // Analog pin for X axis
int Y_pin = 1; // Analog pin for Y axis

// Button pins
int select_button_pin = 2;
int reset_button_pin = 3;

// LED pins
int green_led_pin = 4; // Green LED
int red_led_pin = 5;   // Red LED

// Buzzer pin
const int buzzerPin = 9;

// Cursor position
int cursor_row = 0;
int cursor_col = DIMENSION_COLS; // Start cursor at the end of the first line
int start_row = 0;

// Timer
unsigned long startTime;
bool game_over = false;
bool win = false;

// Function to generate the board
void generate_board(char b[DIMENSION_ROWS][DIMENSION_COLS]) {
  for (int x = 0; x < DIMENSION_ROWS; x++) {
    for (int y = 0; y < DIMENSION_COLS; y++) {
      b[x][y] = '#';
    }
  }
}

// Function to initialize the game with predefined mines
void initialize_game() {
  char temp[DIMENSION_ROWS][DIMENSION_COLS] = {
    {'0', '2', '*', '2', '0', '0', '0'},
    {'0', '2', '*', '2', '1', '0', '0'},
    {'1', '2', '1', '2', '*', '1', '0'},
    {'*', '1', '0', '1', '1', '1', '0'}
  };
  for (int i = 0; i < DIMENSION_ROWS; i++) {
    for (int j = 0; j < DIMENSION_COLS; j++) {
      board_mines[i][j] = temp[i][j];
    }
  }
}

// Function to reveal connected zeros
void reveal_connected_zeros(int row, int col) {
  int queue[DIMENSION_ROWS * DIMENSION_COLS][2];
  int front = 0;
  int rear = 0;

  queue[rear][0] = row;
  queue[rear][1] = col;
  rear++;
  board[row][col] = board_mines[row][col];

  while (front < rear) {
    int cur_row = queue[front][0];
    int cur_col = queue[front][1];
    front++;

    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        int new_row = cur_row + i;
        int new_col = cur_col + j;
        if (new_row >= 0 && new_row < DIMENSION_ROWS && new_col >= 0 && new_col < DIMENSION_COLS) {
          if (board[new_row][new_col] == '#' && board_mines[new_row][new_col] != '*') {
            board[new_row][new_col] = board_mines[new_row][new_col];
            if (board_mines[new_row][new_col] == '0') {
              queue[rear][0] = new_row;
              queue[rear][1] = new_col;
              rear++;
            }
          }
        }
      }
    }
  }
}

// Function to select a cell and reveal its content
void select_cell() {
  int row = cursor_row;
  int col = cursor_col - 1;  // Adjust cursor column

  if (board[row][col] == '#') {
    if (board_mines[row][col] == '*') {
      game_over = true;
      win = false;
      digitalWrite(red_led_pin, LOW);   // Turn on red LED
      digitalWrite(green_led_pin, HIGH); // Ensure green LED is off
      reveal_final_state(true);  // Display the final state with bombs flickering
      playLosingSound(); // Play losing sound
      display_game_over_message();
      return;
    }
    if (board_mines[row][col] == '0') {
      reveal_connected_zeros(row, col);
    }
    board[row][col] = board_mines[row][col];

    if (check_win()) {
      game_over = true;
      win = true;
      digitalWrite(green_led_pin, LOW); // Turn on green LED
      digitalWrite(red_led_pin, HIGH);  // Ensure red LED is off
      reveal_final_state(true);  // Display the final state for win with flickering
      playWinningSound(); // Play winning sound
      display_win_message();
      scroll_message("Hold the Button to Play Again");
    }
  }
}

// Function to check if the player has won
bool check_win() {
  for (int x = 0; x < DIMENSION_ROWS; x++) {
    for (int y = 0; y < DIMENSION_COLS; y++) {
      if (board_mines[x][y] != '*' && board[x][y] == '#') {
        return false;
      }
    }
  }
  return true;
}

// Function to display the board
void display_board() {
  for (int x = start_row; x < start_row + 2 && x < DIMENSION_ROWS; x++) {
    lcd.setCursor(0, x - start_row);  // Set cursor to start of each row
    lcd.print(x + 1);  // Display row number
    lcd.print(':');
    for (int y = 0; y <= DIMENSION_COLS; y++) {
      if (x == cursor_row && y == cursor_col) {
        lcd.print('<');  // Display cursor
      } else if (y < DIMENSION_COLS) {
        lcd.print(board[x][y]);  // Display board content
      } else {
        lcd.print(' ');  // Ensure there is a space for the cursor after the last element
      }
    }
  }

  // Display the timer on the right side
  unsigned long elapsedTime = (millis() - startTime) / 1000;  // Calculate elapsed time in seconds
  int minutes = elapsedTime / 60;
  int seconds = elapsedTime % 60;

  lcd.setCursor(11, 0);
  lcd.print("Time:");
  lcd.setCursor(11, 1);
  if (minutes < 10) lcd.print('0');  // Add leading zero for single digit minutes
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) lcd.print('0');  // Add leading zero for single digit seconds
  lcd.print(seconds);
}

// Function to position the cursor based on joystick input
void position_cursor() {
  int joy_x = analogRead(X_pin);
  int joy_y = analogRead(Y_pin);

  if (joy_y > 700 && cursor_row < DIMENSION_ROWS - 1) {
    cursor_row++;
    if (cursor_row >= start_row + 2) {
      start_row++;
    }
    delay(200);
  } else if (joy_y < 300 && cursor_row > 0) {
    cursor_row--;
    if (cursor_row < start_row) {
      start_row--;
    }
    delay(200);
  }

  if (joy_x > 700 && cursor_col < DIMENSION_COLS) {
    cursor_col++;
    delay(200);
  } else if (joy_x < 300 && cursor_col > 1) {
    cursor_col--;
    delay(200);
  }
}

// Function to display the win message
void display_win_message() {
  lcd.clear();
  lcd.setCursor((16 - 7) / 2, 0); // Center "You Win!" on the screen
  lcd.print("You Win!");
  delay(2000);  // Display "You Win!" for 2 seconds
}

// Function to display the game over message
void display_game_over_message() {
  lcd.clear();
  lcd.setCursor((16 - 9) / 2, 0); // Center "Game Over!" on the screen
  lcd.print("Game Over!");
  delay(2000);  // Display "Game Over!" for 2 seconds
}

// Function to scroll a message on the LCD
void scroll_message(const char* message) {
  int len = strlen(message);
  for (int pos = 0; pos < len - 15; pos++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Play Again?");
    lcd.setCursor(0, 1);
    lcd.print(message + pos);
    delay(700);
  }
}

// Function to reveal the final state with flickering bombs
void reveal_final_state(bool flicker) {
  unsigned long start = millis();
  while (millis() - start < 3000) { // Flicker for 3 seconds
    for (int x = 0; x < DIMENSION_ROWS; x++) {
      for (int y = 0; y < DIMENSION_COLS; y++) {
        if (board_mines[x][y] == '*') {
          board[x][y] = flicker && ((millis() / 500) % 2) ? ' ' : '*';  // Flicker bombs
        } else {
          board[x][y] = board_mines[x][y];
        }
      }
    }
    display_board();
    delay(100);
  }
}

// Function to play a start sound
void playStartSound() {
  int melody[] = {660, 660, 0, 660, 0, 510, 660, 0, 770};
  int noteDurations[] = {100, 100, 100, 100, 100, 100, 100, 100, 300}; // Duration of each note in milliseconds

  for (int i = 0; i < 9; i++) {
    if (melody[i] == 0) {
      noTone(buzzerPin);
    } else {
      tone(buzzerPin, melody[i], noteDurations[i]);
    }
    delay(noteDurations[i] * 1.3); // Add a slight delay between tones
  }
  noTone(buzzerPin);
}

// Function to play a winning sound
void playWinningSound() {
  int melody[] = {880, 784, 880, 784, 880, 659, 880, 659};
  int noteDurations[] = {100, 100, 100, 100, 100, 100, 100, 300}; // Duration of each note in milliseconds

  for (int i = 0; i < 8; i++) {
    if (melody[i] == 0) {
      noTone(buzzerPin);
    } else {
      tone(buzzerPin, melody[i], noteDurations[i]);
    }
    delay(noteDurations[i] * 1.3); // Add a slight delay between tones
  }
  noTone(buzzerPin);
}

// Function to play a losing sound
void playLosingSound() {
  int melody[] = {880, 784, 698, 622, 587}; // Descending pitches for "tu tu tu tuuuu"
  int noteDurations[] = {200, 200, 200, 200, 400}; // Duration of each note in milliseconds

  for (int i = 0; i < 5; i++) {
    if (melody[i] == 0) {
      noTone(buzzerPin);
    } else {
      tone(buzzerPin, melody[i], noteDurations[i]);
    }
    delay(noteDurations[i] * 1.3); // Add a slight delay between tones
  }
  noTone(buzzerPin);
}

// Function to reset the game
void reset_game() {
  lcd.clear();
  lcd.setCursor((16 - 11) / 2, 0); // Center "Initializing" on the screen
  lcd.print("Initializing");
  delay(2000); // Display "Initializing" for 2 seconds

  playStartSound();  // Play sound during reset

  game_over = false;
  win = false;
  initialize_game();
  generate_board(board);
  cursor_row = 0;
  cursor_col = DIMENSION_COLS; // Reset cursor to the end of the first row
  start_row = 0;
  startTime = millis();  // Reset the timer
  lcd.clear();

  digitalWrite(green_led_pin, HIGH); // Turn off green LED
  digitalWrite(red_led_pin, HIGH);   // Turn off red LED
}

// Setup function
void setup() {
  pinMode(X_pin, INPUT);
  pinMode(Y_pin, INPUT);
  pinMode(select_button_pin, INPUT_PULLUP);  // Use internal pull-up resistor for the select button
  pinMode(reset_button_pin, INPUT_PULLUP);   // Use internal pull-up resistor for the reset button
  pinMode(green_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(green_led_pin, HIGH); // Turn off green LED
  digitalWrite(red_led_pin, HIGH);   // Turn off red LED
  
  Serial.begin(9600);

  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Ensure backlight is on
  
  lcd.setCursor((16 - 17) / 2, 0);   // Center "Minesweeper Game" (17 characters)
  lcd.print("Minesweeper Game");
  
  lcd.setCursor((16 - 12) / 2, 1);   // Center "Andra Stoica" (12 characters)
  lcd.print("Andra Stoica");
  delay(3000);
  lcd.clear();

  playStartSound();  // Play sound when the game starts initially

  initialize_game();
  generate_board(board);

  startTime = millis();
}

// Main loop function
void loop() {
  if (!game_over) {
    display_board();
    position_cursor();

    if (digitalRead(select_button_pin) == LOW) {
      select_cell();
      delay(200);  // Debounce delay
    }

    if (digitalRead(reset_button_pin) == LOW) {
      reset_game();
      delay(200);  // Debounce delay
    }
  } else {
    scroll_message("Hold the Button to Play Again");
    if (digitalRead(reset_button_pin) == LOW) {
      reset_game();
      delay(200);  // Debounce delay
    }
  }

  delay(100);
}
