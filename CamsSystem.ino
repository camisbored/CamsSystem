#include <LiquidCrystal.h>
#include <EEPROM.h>

//declare pins for joystick
const int X_pin = 0;
const int Y_pin = 1;
const int SW_pin = 0;
//input pin for buzzer
const int buzzer = 9;
//define pins for LCD (2-7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//data for programs to use
unsigned long int counter = 0;
static String totalMsg = "Luna is fish    George is cat   ";
static unsigned int currentIndex = 0;
static unsigned int menuIndex = 0;
bool changeDetected = false;

//define custom characters
byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte Star[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte Person[] = {
  B00000,
  B01110,
  B10001,
  B01110,
  B00100,
  B11111,
  B00100,
  B01010
};

byte PersonUp[] = {
  B01110,
  B10001,
  B01110,
  B00100,
  B11111,
  B01010,
  B10001,
  B00000
};

byte PersonDown[] = {
  B00000,
  B00000,
  B01110,
  B10001,
  B01110,
  B11111,
  B00100,
  B100001
};

byte TopArrow[] = {
  B01000,
  B11111,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte BottomArrow[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01000,
  B11111,
  B01000
};

void writeMessage() {
  String line1 = totalMsg.substring(0, 16);
  String line2 = totalMsg.substring(16, 32);
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void clearTop() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
}

void clearBottom() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void clearScreen() {
  clearTop();
  clearBottom();
}

void quickBuzz() {
  tone(buzzer, 261);
  delay(50);
  noTone(buzzer);
}

void longBuzz() {
  tone(buzzer, 600);
  delay(300);
  noTone(buzzer);
}

void note(int pitch, int noteLength, int postDelay) {
  tone(buzzer, pitch);
  delay(noteLength);
  noTone(buzzer);
  delay(postDelay);
}

//app 1: count forever- will count forever while open
void countForever() {
  lcd.setCursor(0, 0);
  lcd.print("Click to return");
  //check for pin press, if not, return
  while (1) {
    if (!digitalRead(SW_pin)) {
      longBuzz();
      clearScreen();
      return;
    }
    lcd.setCursor(0, 1);
    lcd.print(counter++);
  }
}

//app 2: use buzzer to play music
void song() {
  note(300, 100, 75);
  note(600, 75, 25);
  note(500, 75, 25);
  note(350, 250, 50);
  note(250, 250, 800);
  note(300, 100, 75);
  return;
}

//app 3: play game
void game() {
  boolean isEnemyStateSet = false;
  int playerState = 0;
  int delayTime=300;
  int enemyPos = 15;
  int enemyState = 0;
  int score = 0;
  while (1) {
    lcd.setCursor(0, 0);
    lcd.print("Score-");
    lcd.setCursor(8, 0);
    lcd.print(score);
    lcd.setCursor(0, 1);
    if (analogRead(X_pin) < 400)
      playerState = 5;
    else if (analogRead(X_pin) > 600)
      playerState = 6;
    else
      playerState = 2;
    lcd.write((byte)playerState);
    lcd.setCursor(enemyPos--, 1);
    if (!isEnemyStateSet) {
      enemyState = random(2);
      isEnemyStateSet = true;
    }
    if (enemyState == 0 && enemyPos > 0)
      lcd.write((byte)3);
    else if (enemyState == 1 && enemyPos > 0)
      lcd.write((byte)4);
    else {
      quickBuzz();
      if (playerState==2 || (playerState == 5 && enemyState == 0) || (playerState == 6 && enemyState == 1))
        break;
      score++;
      enemyPos = 15;
      isEnemyStateSet = false;
      delayTime-=10;
    }
    delay(delayTime);
    clearScreen();
  }
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.setCursor(7, 1);
  lcd.print(score);
  lcd.setCursor(9, 1);
  lcd.print(" HS: ");
  lcd.setCursor(14, 1);
  int highScore = EEPROM.read(0);
  if (score>highScore){
    EEPROM.write(0,score);
    lcd.print(score);
  }
  else
    lcd.print(highScore);
  while (1) {
    if (!digitalRead(SW_pin)) {
      longBuzz();
      clearScreen();
      return;
    }
  }
}

//app 4- changable message board
void messageBoard() {
  writeMessage();
  lcd.setCursor(14, 0);
  lcd.write((byte)0);
  lcd.setCursor(14, 1);
  lcd.write((byte)1);
  while (1) {
    if (!digitalRead(SW_pin)) {
      changeDetected = true;
      currentIndex = 0;
      longBuzz();
      clearScreen();
      return;
    }
    else if (analogRead(X_pin) < 400) {
      if ((int) totalMsg[currentIndex] < 126) {
        changeDetected = true;
        totalMsg[currentIndex]++;
      }
    }
    else if (analogRead(X_pin) > 600) {
      if ((int) totalMsg[currentIndex] > 32) {
        changeDetected = true;
        totalMsg[currentIndex]--;
      }
    }
    else if (analogRead(Y_pin) > 600) {
      if (currentIndex > 0) {
        changeDetected = true;
        currentIndex--;
      }
    }
    else if (analogRead(Y_pin) < 400) {
      if (currentIndex < 31) {
        changeDetected = true;
        currentIndex++;
      }
    }
    if (changeDetected) {
      changeDetected = false;
      writeMessage();
    }
    delay(150);
  }
}


void setup() {
  pinMode(SW_pin, INPUT);
  pinMode(X_pin, INPUT);
  pinMode(Y_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, Heart);
  lcd.createChar(1, Star);
  lcd.createChar(2, Person);
  lcd.createChar(3, TopArrow);
  lcd.createChar(4, BottomArrow);
  lcd.createChar(5, PersonUp);
  lcd.createChar(6, PersonDown);

  lcd.setCursor(0, 0);
  lcd.print(" **Welcome to**");
  lcd.setCursor(0, 1);
  lcd.print("**Cam's System**");
  delay(2000);
  longBuzz();
}

void loop() {
  clearScreen();
  lcd.setCursor(0, 0);
  lcd.print("Select App!");
  lcd.setCursor(0, 1);
  if (menuIndex == 0)
    lcd.print("Message Board");
  if (menuIndex == 1)
    lcd.print("Count Forever");
  if (menuIndex == 2)
    lcd.print("Song");
  if (menuIndex == 3)
    lcd.print("Game");
  while (1) {
    if (!digitalRead(SW_pin)) {
      longBuzz();
      clearScreen();
      if (menuIndex == 0)
        messageBoard();
      if (menuIndex == 1)
        countForever();
      if (menuIndex == 2)
        song();
      if (menuIndex == 3)
        game();
      return;
    }
    else if (analogRead(X_pin) < 400) {
      if (menuIndex > 0) {
        quickBuzz();
        menuIndex--;
        break;
      }
    }
    else if (analogRead(X_pin) > 600) {
      if (menuIndex < 3) {
        quickBuzz();
        menuIndex++;
        break;
      }
    }
    delay(150);
  }
}
